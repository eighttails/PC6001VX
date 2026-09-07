////////////////////////////////////////////////////////////////
// shareutils.h
//  Android/iOSで生成したファイルをインテント(Intent)/
//  アクティビティビュー(UIActivityViewController)経由で
//  他のアプリと共有するためのユーティリティ
////////////////////////////////////////////////////////////////
#ifndef SHAREUTILS_H
#define SHAREUTILS_H

#include <QString>

// ファイルを他のアプリへ共有する(Android/iOSのみ有効)
// filePath		: 共有するファイルの絶対パス
// title		: 共有ダイアログ(チューザー)に表示するタイトル
// mimeType		: 共有するファイルのMIMEタイプ
// 返値: true:共有処理を開始できた false:失敗(Android/iOS以外では常にfalse)
bool ShareFile( const QString& filePath, const QString& title, const QString& mimeType );

// 共有シート(チューザー)を表示中かどうかを返す
// 共有シートが手前に来ると自アプリが非アクティブとなりApplicationStateChangeが
// 発生するが、共有はアプリの中断ではないため、これを見て自動サスペンド等を抑止する
bool IsSharing();

// 共有シートの表示状態を設定する
void SetSharing( bool sharing );

#endif // SHAREUTILS_H
