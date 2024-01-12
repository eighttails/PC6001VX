#ifndef P6VXCOMMON_H
#define P6VXCOMMON_H

#ifdef QTP6VX
#include <QCoreApplication>
#include <QString>
#include <QDebug>

#include "../typedef.h"

#define QSTR2P6VPATH(st)			STR2P6VPATH(st.toStdString())
#define P6VPATH2QSTR(st)			QString::fromStdString(P6VPATH2STR(st))

//翻訳された文字列を返す
#define TRANS(a) (QCoreApplication::translate("PC6001VX", a).toUtf8().data())
#else
#define TRANS(a) a
#define QT_TRANSLATE_NOOP(a,b) b
#endif

#if (defined Q_OS_ANDROID)
#include <string>
#include <sstream>
namespace std{
template <typename T>
std::string to_string(T value)
{
	std::ostringstream os ;
	os << value ;
	return os.str() ;
}
}
#endif

enum TiltDirection{
	NEWTRAL,
	LEFT,
	RIGHT,
};
#ifdef QTP6VX
Q_DECLARE_METATYPE(TiltDirection)
#endif

// TILTモード用に画面を傾ける
void TiltScreen(TiltDirection dir);
void UpdateTilt();

// Android用ファイル名エンコーディング
QString EncodeContentURI(const QString& uri);

// シグナルを受信したら終了するようにする
void HandleQuitSignal();

#endif // P6VXCOMMON_H
