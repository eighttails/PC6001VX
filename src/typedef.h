#ifndef TYPEDEF_H_INCLUDED
#define TYPEDEF_H_INCLUDED

#define __STDC_LIMIT_MACROS

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

/////////////////////////////////////////////////////////////////////////////
// 型,定数定義
/////////////////////////////////////////////////////////////////////////////
#if defined(WIN32) || defined(WIN64)
#include <windows.h>
#else
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef uint32_t DWORD;
#endif

// OSD関連オブジェクトへのポインタ
// 面倒なのでとりあえず何でもvoid *
typedef void *HTHREAD;				// スレッド
typedef void *HCRSECT;				// クリティカルセクション
typedef void *HSEMAPHORE;			// セマフォ
typedef void *HTIMERID;				// タイマID
typedef void *HWINDOW;				// ウィンドウハンドル的な
typedef void *HSURFACE;				// サーフェス的な
typedef void *HJOYINFO;				// ジョイスティック

// OSD関連コールバック関数へのポインタ
typedef void (*CBF_SND)( void *, BYTE *, int );	// サウンドストリーム
typedef DWORD (*CBF_TMR)( DWORD, void * );		// タイマ

/////////////////////////////////////////////////////////////////////////////
// 定数など
/////////////////////////////////////////////////////////////////////////////
#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif


#ifndef PATH_MAX
#define	PATH_MAX	260
#endif

#ifndef MAX_PATH
#define MAX_PATH PATH_MAX
#endif

#ifndef LIL_ENDIAN
#define LIL_ENDIAN	1234
#endif
#ifndef BIG_ENDIAN
#define BIG_ENDIAN	4321
#endif

#ifndef BYTEORDER
#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define BYTEORDER	BIG_ENDIAN
#else
#define BYTEORDER	LIL_ENDIAN
#endif
#endif







/////////////////////////////////////////////////////////////////////////////
// 汎用マクロ
/////////////////////////////////////////////////////////////////////////////
#define STATIC_CAST(t, o)	static_cast<t> (o)
#define CONST_CAST(t, o)	const_cast<t> (o)

#define FGETBYTE(fp)		((BYTE)fgetc(fp))
#define FGETWORD(fp)		((WORD)(((BYTE)fgetc(fp))|((BYTE)fgetc(fp)<<8)))
#define FGETDWORD(fp)		((DWORD)(((BYTE)fgetc(fp))|((BYTE)fgetc(fp)<<8)|((BYTE)fgetc(fp)<<16)|((BYTE)fgetc(fp)<<24)))
#define FPUTBYTE(data,fp)	fputc((data)&0xff,fp)
#define FPUTWORD(data,fp)	{ fputc((data)&0xff,fp); fputc(((data)>>8)&0xff,fp); }
#define FPUTDWORD(data,fp)	{ fputc((data)&0xff,fp); fputc(((data)>>8)&0xff,fp); fputc(((data)>>16)&0xff,fp); fputc(((data)>>24)&0xff,fp); }

#define CTODW(a,b,c,d)		((DWORD)(((BYTE)(a))|(((DWORD)((BYTE)(b)))<<8)|(((DWORD)((BYTE)(c)))<<16)|(((DWORD)((BYTE)(d)))<<24)))

#if BYTEORDER == LIL_ENDIAN

#define GET3BYTE(a,b,c,p)	{ a = *((BYTE *)p++); b = *((BYTE *)p++); c = *((BYTE *)p++); }
#define PUT3BYTE(a,b,c,p)	{ *((BYTE *)p++) = a; *((BYTE *)p++) = b; *((BYTE *)p++) = c; }
#define BTODW(a,b,c,d)		((DWORD)(((BYTE)(a))|(((DWORD)((BYTE)(b)))<<8)|(((DWORD)((BYTE)(c)))<<16)|(((DWORD)((BYTE)(d)))<<24)))
#define DWTOB(s,a,b,c,d)	{ a = (BYTE)((s>>24)&0x000000ff); b = (BYTE)((s>>16)&0x000000ff); c = (BYTE)((s>>8)&0x000000ff); d = (BYTE)(s&0x000000ff); }

#else

#define GET3BYTE(a,b,c,p)	{ c = *((BYTE *)p++); b = *((BYTE *)p++); a = *((BYTE *)p++); }
#define PUT3BYTE(a,b,c,p)	{ *((BYTE *)p++) = c; *((BYTE *)p++) = b; *((BYTE *)p++) = a; }
#define BTODW(a,b,c,d)		((DWORD)(((BYTE)(d))|(((DWORD)((BYTE)(c)))<<8)|(((DWORD)((BYTE)(b)))<<16)|(((DWORD)((BYTE)(a)))<<24)))
#define DWTOB(s,a,b,c,d)	{ d = (BYTE)((s>>24)&0x000000ff); c = (BYTE)((s>>16)&0x000000ff); b = (BYTE)((s>>8)&0x000000ff); a = (BYTE)(s&0x000000ff); }

#endif

#define	COUNTOF(arr)		(int)(sizeof(arr)/sizeof((arr)[0]))

#define INITARRAY(arr,val)	{for(int i=0; i<COUNTOF(arr); i++) arr[i] = val;}
#ifndef ZeroMemory
#define ZeroMemory(d,l)	memset((d), 0, (l))
#endif

#ifndef max
#define max(a,b) ((a)>(b)?(a):(b))
#endif
#ifndef min
#define min(a,b) ((a)<(b)?(a):(b))
#endif



#endif	// TYPEDEF_H_INCLUDED
