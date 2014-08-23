#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED


#include "typedef.h"
#include "vsurface.h"


////////////////////////////////////////////////////////////////
// 汎用計算関数
////////////////////////////////////////////////////////////////
DWORD CalcCrc32( BYTE *, int );						// CRC32計算
WORD CalcCrc16( BYTE *, int );						// CRC16計算


////////////////////////////////////////////////////////////////
// 文字コード操作関数
////////////////////////////////////////////////////////////////
void Euc2Jis( BYTE *, BYTE * );						// EUC  -> JIS
//void Jis2Euc( BYTE *, BYTE * );						// JIS  -> EUC
//void Euc2Sjis( BYTE *, BYTE * );					// EUC  -> SJIS
//void Sjis2Euc( BYTE *, BYTE * );					// SJIS -> EUC
void Sjis2Jis( BYTE *, BYTE * );					// SJIS -> JIS
void Jis2Sjis( BYTE *, BYTE * );					// JIS  -> SJIS
void Convert2Jis( BYTE *, BYTE * );					// 文字コード形式変換(SJIS,EUC -> JIS)

/* not use by Windy
#ifdef __APPLE__
const char *Sjis2UTF8( const char * );				// SJIS -> UTF8
#endif
*/
char *UTF8toLocal( const char * );                  // Utf8 -> システム文字コード


int Sjis2P6( char *, const char * );				// SJIS -> P6

// Mac用文字コード変換 ShiftJIS -> UTF-8
#ifdef __APPLE__
#define FOPENEN(a,b)	fopen(a,b)
#else
#define FOPENEN(a,b)	fopen(UTF8toLocal(a),b)
#endif

////////////////////////////////////////////////////////////////
// 画像ファイル操作関数
////////////////////////////////////////////////////////////////
bool SaveImgData( const char *, BYTE *, int, int, int, VRect * );	// Img SAVE from Data
bool SaveImg( const char *, VSurface *, VRect * );	// Img SAVE
VSurface *LoadImg( const char * );					// Img LOAD


////////////////////////////////////////////////////////////////
// 描画関係
////////////////////////////////////////////////////////////////
void RectAdd( VRect *, VRect *, VRect * );			// 矩形領域合成

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

#endif	// COMMON_H_INCLUDED
