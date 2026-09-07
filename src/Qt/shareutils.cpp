////////////////////////////////////////////////////////////////
// shareutils.cpp
//  ShareFile()の実装
//   ・Android	: android/res/xml/filepaths.xml で定義したFileProviderを
//				  経由してACTION_SENDインテントを発行する
//   ・iOS		: shareutils_ios.mm で実装(UIActivityViewController)
//   ・その他	: 何もしない(呼び出し元でファイルは既に指定フォルダへ
//				  保存済みのため、共有処理自体が不要)
////////////////////////////////////////////////////////////////
#include "./shareutils.h"

#include <QAtomicInt>

namespace {
// 共有シート表示中フラグ(GUIスレッドとエミュレーションスレッドの双方から参照される)
QAtomicInt SharingFlag(0);
}

bool IsSharing()
{
	return SharingFlag.loadAcquire() != 0;
}

void SetSharing( bool sharing )
{
	SharingFlag.storeRelease(sharing ? 1 : 0);
}

#ifdef Q_OS_ANDROID

#include <QCoreApplication>
#include <QFileInfo>
#include <QJniObject>
#include <QJniEnvironment>
#include <QCoreApplication>
#include <QThread>
#include <QtCore/qnativeinterface.h>
#include <QtCore/qcoreapplication_platform.h>

// startActivity()を含むJNI呼び出しはAndroidのメインスレッドから行う必要があるため、
// 実処理は本関数内に切り出し、呼び出し元でrunOnAndroidMainThread()経由で実行する
static bool ShareFileOnMainThread( const QString& filePath, const QString& title, const QString& mimeType )
{
	QJniObject context = QNativeInterface::QAndroidApplication::context();
	if( !context.isValid() ){
		return false;
	}

	// FileProviderのauthorityは applicationId + ".fileprovider"
	QJniObject packageName = context.callObjectMethod("getPackageName", "()Ljava/lang/String;");
	QString authority = packageName.toString() + QStringLiteral(".fileprovider");

	QJniObject jFile("java/io/File", "(Ljava/lang/String;)V", QJniObject::fromString(filePath).object<jstring>());
	if( !jFile.isValid() ){
		return false;
	}

	QJniObject jAuthority = QJniObject::fromString(authority);
	QJniObject jUri = QJniObject::callStaticObjectMethod(
				"androidx/core/content/FileProvider",
				"getUriForFile",
				"(Landroid/content/Context;Ljava/lang/String;Ljava/io/File;)Landroid/net/Uri;",
				context.object(), jAuthority.object<jstring>(), jFile.object());
	// 共有対象がFileProviderの公開範囲(res/xml/filepaths.xml)外の場合、
	// getUriForFile()はIllegalArgumentExceptionを投げる。
	// 保留中の例外を残したまま次のJNI呼び出しを行うとVMがabortするため、
	// ここで必ず例外を検査してクリアする。
	if( QJniEnvironment().checkAndClearExceptions() || !jUri.isValid() ){
		return false;
	}

	QJniObject jAction = QJniObject::getStaticObjectField("android/content/Intent", "ACTION_SEND", "Ljava/lang/String;");
	QJniObject jIntent("android/content/Intent", "(Ljava/lang/String;)V", jAction.object<jstring>());
	if( !jIntent.isValid() ){
		return false;
	}

	QJniObject jMimeType = QJniObject::fromString(mimeType);
	jIntent.callObjectMethod("setType", "(Ljava/lang/String;)Landroid/content/Intent;", jMimeType.object<jstring>());

	QJniObject jExtraStream = QJniObject::getStaticObjectField("android/content/Intent", "EXTRA_STREAM", "Ljava/lang/String;");
	jIntent.callObjectMethod("putExtra", "(Ljava/lang/String;Landroid/os/Parcelable;)Landroid/content/Intent;", jExtraStream.object<jstring>(), jUri.object());

	// URIへの読み取りを一時的に許可する
	const jint FLAG_GRANT_READ_URI_PERMISSION = 0x00000001;
	jIntent.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", FLAG_GRANT_READ_URI_PERMISSION);

	QJniObject jChooser = QJniObject::callStaticObjectMethod(
				"android/content/Intent",
				"createChooser",
				"(Landroid/content/Intent;Ljava/lang/CharSequence;)Landroid/content/Intent;",
				jIntent.object(), QJniObject::fromString(title).object<jstring>());
	if( !jChooser.isValid() ){
		return false;
	}

	// FileProviderの一時URI権限をchooser側の候補アプリにも付与するため、
	// Intent.createChooser()が生成したchooser自体にもフラグを追加しておく
	jChooser.callObjectMethod("addFlags", "(I)Landroid/content/Intent;", FLAG_GRANT_READ_URI_PERMISSION);

	context.callMethod<void>("startActivity", "(Landroid/content/Intent;)V", jChooser.object());
	// startActivity()も失敗しうるため、例外を残さないようにする
	if( QJniEnvironment().checkAndClearExceptions() ){
		return false;
	}

	return true;
}

bool ShareFile( const QString& filePath, const QString& title, const QString& mimeType )
{
	// 呼び出し元のスレッドによって処理が異なる。
	// ・ポップアップメニュー経由(ExecMenu)の場合、エミュレーションスレッドは
	//   既に停止しておりAndroidのUIスレッド(=Qtのメインスレッド)上で呼ばれる。
	// ・F12ショートカット等、EventLoop()経由の場合はエミュレーション用の
	//   バックグラウンドスレッド(cThread)上で呼ばれ、UIスレッドではない。
	// startActivity()等のJNI呼び出しはUIスレッドから行う必要がある。
	// UIスレッドから呼ばれた場合にrunOnAndroidMainThread()の完了を待つと、
	// Androidのメインスレッド(Looper)自身が自分自身に投稿した処理の完了を
	// 待つ形になりデッドロックするため、既にUIスレッドであれば直接実行する。
	// バックグラウンドスレッドから呼ばれた場合は、戻り値を待つ必要がないため
	// runOnAndroidMainThread()に処理を委譲するのみで完了を待たない。
	if (QThread::currentThread() == qApp->thread()){
		SetSharing(true);
		if( !ShareFileOnMainThread(filePath, title, mimeType) ){
			SetSharing(false);
			return false;
		}
		return true;
	}

	SetSharing(true);
	QNativeInterface::QAndroidApplication::runOnAndroidMainThread(
				[filePath, title, mimeType](){
		if( !ShareFileOnMainThread(filePath, title, mimeType) ){
			SetSharing(false);
		}
	});

	return true;
}

#elif defined(Q_OS_IOS)

// iOS向けの実装は shareutils_ios.mm を参照

#else

bool ShareFile( const QString& filePath, const QString& title, const QString& mimeType )
{
	// Android/iOS以外では共有インテントを使わず、
	// SaveImgData()等の呼び出し元で既に設定フォルダへ保存済みのため何もしない
	Q_UNUSED(filePath)
	Q_UNUSED(title)
	Q_UNUSED(mimeType)
	return false;
}

#endif
