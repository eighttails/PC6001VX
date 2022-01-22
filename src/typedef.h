/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#ifndef TYPEDEF_H_INCLUDED
#define TYPEDEF_H_INCLUDED

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <climits>


/////////////////////////////////////////////////////////////////////////////
// 型,定数定義
/////////////////////////////////////////////////////////////////////////////
#if defined(_WIN32) || defined(_WIN64)
#define	NOMINMAX	1				// max,min無効化
#include <windows.h>
#else
using BYTE  = uint8_t;
using WORD  = uint16_t;
using DWORD = uint32_t;
#endif


// OSD関連オブジェクトハンドル
// 面倒なのでとりあえず何でもvoid *
using HTHREAD    = void*;	// スレッド
using HCRSECT    = void*;	// クリティカルセクション
using HSEMAPHORE = void*;	// セマフォ
using HWINDOW    = void*;	// ウィンドウハンドル的な
using HSURFACE   = void*;	// サーフェス的な
using HJOYINFO   = void*;	// ジョイスティック

// OSD関連変数型
using TIMERID = DWORD;		// タイマID

// OSD関連コールバック関数へのポインタ
using CBF_SND = void  (*)( void*, BYTE*, int );	// サウンドストリーム
using CBF_TMR = DWORD (*)( DWORD, void* );		// タイマ

// filesystem
#ifdef	USEFILESYSTEM

#include <filesystem>
using P6VPATH = std::filesystem::path;
#define STR2P6VPATH(st)			std::filesystem::u8path(st)
#define P6VPATH2STR(st)			(st).u8string()

#else

using P6VPATH = std::string;
#define STR2P6VPATH(st)			(st)
#define P6VPATH2STR(st)			(st)

#endif




/////////////////////////////////////////////////////////////////////////////
// 定数など
/////////////////////////////////////////////////////////////////////////////
#define INBPP			8	// 内部色深度

#ifndef M_PI
#define M_PI			3.14159265358979323846
#endif

#ifndef PATH_MAX
#define	PATH_MAX		260
#endif

#define P6V_LIL_ENDIAN	1234
#define P6V_BIG_ENDIAN	4321

#ifndef BYTEORDER
#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define BYTEORDER		P6V_BIG_ENDIAN
#else
#define BYTEORDER		P6V_LIL_ENDIAN
#endif
#endif


// ステータスバーインジケータID(DWORD)
#define	ST_IDLE			0x00000000
#define	ST_REPLAYREC	0x00000001
#define	ST_REPLAYPLAY	0x00000002
#define	ST_CAPTUREREC	0x00000004




/////////////////////////////////////////////////////////////////////////////
// 汎用マクロ
/////////////////////////////////////////////////////////////////////////////
#define STATIC_CAST(t, o)	static_cast<t> (o)
#define CONST_CAST(t, o)	const_cast<t> (o)

#define FSGETBYTE(fs)		((BYTE)fs.get())
#define FSGETWORD(fs)		((WORD)(FSGETBYTE(fs)|(FSGETBYTE(fs)<<8)))
#define FSGETDWORD(fs)		((DWORD)((FSGETBYTE(fs))|(FSGETBYTE(fs)<<8)|(FSGETBYTE(fs)<<16)|(FSGETBYTE(fs)<<24)))
#define FSPUTBYTE(data,fs)	fs.put((data)&0xff)
#define FSPUTWORD(data,fs)	{ FSPUTBYTE(data,fs); FSPUTBYTE(data>>8,fs); }
#define FSPUTDWORD(data,fs)	{ FSPUTBYTE(data,fs); FSPUTBYTE(data>>8,fs); FSPUTBYTE(data>>16,fs); FSPUTBYTE(data>>24,fs); }
#define FSPUTSTR(str,fs)	fs.write(str,std::strlen(str))

#define CTODW(a,b,c,d)		((DWORD)(((BYTE)(a))|(((DWORD)((BYTE)(b)))<<8)|(((DWORD)((BYTE)(c)))<<16)|(((DWORD)((BYTE)(d)))<<24)))

#if BYTEORDER == P6V_LIL_ENDIAN
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
#define ZeroMemory(d,l)		std::memset((d), 0, (l))
#endif


// 32bit環境だとNOMINMAXが効かないみたい?
#undef max
#undef min

template <class T, class S> T max( T v1, S v2 ){ T v2_(v2); return v1 > v2_ ? v1 : v2_; }
template <class T, class S> T min( T v1, S v2 ){ T v2_(v2); return v1 < v2_ ? v1 : v2_; }

#endif	// TYPEDEF_H_INCLUDED
