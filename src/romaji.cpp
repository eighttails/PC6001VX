/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
// Based on:
/////////////////////////////////////////////////////////////////////////////
/*
name is romaji.cpp

Copyright (c) 2020-2022 Windy
Version 2.0
This software is released under the MIT License.

https://github.com/windy6001/ROMAJI/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
/////////////////////////////////////////////////////////////////////////////


#include <algorithm>
#include <map>
#include <vector>

#include "keydef.h"
#include "romaji.h"
#include "log.h"



/////////////////////////////////////////////////////////////////////////////
// P6 文字コード
/////////////////////////////////////////////////////////////////////////////
enum { 
	P6_WO = 0x86,
	P6_LA,	P6_LI,	P6_LU,	P6_LE,	P6_LO,	P6_LYA,	P6_LYU,	P6_LYO,	P6_LTU,
	P6_A = 0x91,	P6_I,	P6_U,	P6_E,	P6_O,	P6_KA,	P6_KI,	P6_KU,	P6_KE,	P6_KO,
	P6_SA,	P6_SI,	P6_SU,	P6_SE,	P6_SO,
	P6_KUTEN = 0xA1, P6_LBRK,	P6_RBRK,	P6_TOUTEN,	P6_NAKATEN,	P6_CYOUON = 0xB0,
	P6_TA = 0xE0,	P6_TI,	P6_TU,	P6_TE,	P6_TO,
	P6_NA,	P6_NI,	P6_NU,	P6_NE,	P6_NO,
	P6_HA,	P6_HI,	P6_HU,	P6_HE,	P6_HO,
	P6_MA,	P6_MI,	P6_MU,	P6_ME,	P6_MO,
	P6_YA,	P6_YU,	P6_YO,
	P6_RA,	P6_RI,	P6_RU,	P6_RE,	P6_RO,	P6_WA,	P6_N,
	P6_DD = 0xDE,	P6_HD = 0xDF		// P6_DD=濁点、 P6_HD=半濁点
 };


/////////////////////////////////////////////////////////////////////////////
// P6キーコード->ローマ字変換テーブル
/////////////////////////////////////////////////////////////////////////////
static const std::map<std::vector<P6KEYsym>,std::vector<uint8_t>> romaji_tbl =
{
	//あいうえお
	{ { KP6_A },							{ P6_A } },
	{ { KP6_I },							{ P6_I } },
	{ { KP6_U },							{ P6_U } },
	{ { KP6_E },							{ P6_E } },
	{ { KP6_O },							{ P6_O } },
	
	//ぁぃぅぇぉ
	{ { KP6_L,	KP6_A },					{ P6_LA } },
	{ { KP6_X,	KP6_A },					{ P6_LA } },
	{ { KP6_L,	KP6_I },					{ P6_LI } },
	{ { KP6_X,	KP6_I },					{ P6_LI } },
	{ { KP6_L,	KP6_U },					{ P6_LU } },
	{ { KP6_X,	KP6_U },					{ P6_LU } },
	{ { KP6_L,	KP6_E },					{ P6_LE } },
	{ { KP6_X,	KP6_E },					{ P6_LE } },
	{ { KP6_L,	KP6_O },					{ P6_LO } },
	{ { KP6_X,	KP6_O },					{ P6_LO } },
	
	//うぁうぃう　うぇうぉ
	{ { KP6_W,	KP6_H,	KP6_A },			{ P6_U,	P6_LA } },
	{ { KP6_W,	KP6_H,	KP6_I },			{ P6_U,	P6_LI } },
	{ { KP6_W,	KP6_H,	KP6_U },			{ P6_U } },
	{ { KP6_W,	KP6_H,	KP6_E },			{ P6_U,	P6_LE } },
	{ { KP6_W,	KP6_H,	KP6_O },			{ P6_U,	P6_LO } },
	
	
	//かきくけこ
	{ { KP6_K,	KP6_A },					{ P6_KA } },
	{ { KP6_K,	KP6_I },					{ P6_KI } },
	{ { KP6_K,	KP6_U },					{ P6_KU } },
	{ { KP6_K,	KP6_E },					{ P6_KE } },
	{ { KP6_K,	KP6_O },					{ P6_KO } },
	
	//きゃきぃきゅきぇきょ
	{ { KP6_K,	KP6_Y,	KP6_A },			{ P6_KI,	P6_LYA } },
	{ { KP6_K,	KP6_Y,	KP6_I },			{ P6_KI,	P6_LI } },
	{ { KP6_K,	KP6_Y,	KP6_U },			{ P6_KI,	P6_LYU } },
	{ { KP6_K,	KP6_Y,	KP6_E },			{ P6_KI,	P6_LE } },
	{ { KP6_K,	KP6_Y,	KP6_O },			{ P6_KI,	P6_LYO } },
	
	//くぁくぃくぅくぇくぉ
	{ { KP6_K,	KP6_W,	KP6_A },			{ P6_KU,	P6_LA } },
	{ { KP6_K,	KP6_W,	KP6_I },			{ P6_KU,	P6_LI } },
	{ { KP6_K,	KP6_W,	KP6_U },			{ P6_KU,	P6_LU } },
	{ { KP6_K,	KP6_W,	KP6_E },			{ P6_KU,	P6_LE } },
	{ { KP6_K,	KP6_W,	KP6_O },			{ P6_KU,	P6_LO } },
	
	//くぁくぃくくぇくぉ
	{ { KP6_Q,	KP6_A },					{ P6_KU,	P6_LA } },
	{ { KP6_Q,	KP6_I },					{ P6_KU,	P6_LI } },
	{ { KP6_Q,	KP6_U },					{ P6_KU } },
	{ { KP6_Q,	KP6_E },					{ P6_KU,	P6_LE } },
	{ { KP6_Q,	KP6_O },					{ P6_KU,	P6_LO } },
	
	//がぎぐげご
	{ { KP6_G,	KP6_A },					{ P6_KA,	P6_DD } },
	{ { KP6_G,	KP6_I },					{ P6_KI,	P6_DD } },
	{ { KP6_G,	KP6_U },					{ P6_KU,	P6_DD } },
	{ { KP6_G,	KP6_E },					{ P6_KE,	P6_DD } },
	{ { KP6_G,	KP6_O },					{ P6_KO,	P6_DD } },
	
	//ぎゃぎぃぎゅぎぇぎょ
	{ { KP6_G,	KP6_Y,	KP6_A },			{ P6_KI,	P6_DD,	P6_LYA } },
	{ { KP6_G,	KP6_Y,	KP6_I },			{ P6_KI,	P6_DD,	P6_LI } },
	{ { KP6_G,	KP6_Y,	KP6_U },			{ P6_KI,	P6_DD,	P6_LYU } },
	{ { KP6_G,	KP6_Y,	KP6_E },			{ P6_KI,	P6_DD,	P6_LE } },
	{ { KP6_G,	KP6_Y,	KP6_O },			{ P6_KI,	P6_DD,	P6_LYO } },
	
	//ぐぁぐぃぐぅぐぇぐぉ
	{ { KP6_G,	KP6_W,	KP6_A },			{ P6_KU,	P6_DD,	P6_LA } },
	{ { KP6_G,	KP6_W,	KP6_I },			{ P6_KU,	P6_DD,	P6_LI } },
	{ { KP6_G,	KP6_W,	KP6_U },			{ P6_KU,	P6_DD,	P6_LU } },
	{ { KP6_G,	KP6_W,	KP6_E },			{ P6_KU,	P6_DD,	P6_LE } },
	{ { KP6_G,	KP6_W,	KP6_O },			{ P6_KU,	P6_DD,	P6_LO } },
	
	
	//さしすせそ
	{ { KP6_S,	KP6_A },					{ P6_SA } },
	{ { KP6_S,	KP6_I },					{ P6_SI } },
	{ { KP6_S,	KP6_H,	KP6_I },			{ P6_SI } },
	{ { KP6_S,	KP6_U },					{ P6_SU } },
	{ { KP6_S,	KP6_E },					{ P6_SE } },
	{ { KP6_S,	KP6_O },					{ P6_SO } },
	
	//しゃしぃしゅしぇしょ
	{ { KP6_S,	KP6_Y,	KP6_A },			{ P6_SI,	P6_LYA } },
	{ { KP6_S,	KP6_Y,	KP6_I },			{ P6_SI,	P6_LI } },
	{ { KP6_S,	KP6_Y,	KP6_U },			{ P6_SI,	P6_LYU } },
	{ { KP6_S,	KP6_Y,	KP6_E },			{ P6_SI,	P6_LE } },
	{ { KP6_S,	KP6_Y,	KP6_O },			{ P6_SI,	P6_LYO } },
	
	//しゃし　しゅしぇしょ
	{ { KP6_S,	KP6_H,	KP6_A },			{ P6_SI,	P6_LYA } },
	{ { KP6_S,	KP6_H,	KP6_I },			{ P6_SI } },
	{ { KP6_S,	KP6_H,	KP6_U },			{ P6_SI,	P6_LYU } },
	{ { KP6_S,	KP6_H,	KP6_E },			{ P6_SI,	P6_LE } },
	{ { KP6_S,	KP6_H,	KP6_O },			{ P6_SI,	P6_LYO } },
	
	//すぁすぃすぅすぇすぉ
	{ { KP6_S,	KP6_W,	KP6_A },			{ P6_SU,	P6_LA } },
	{ { KP6_S,	KP6_W,	KP6_I },			{ P6_SU,	P6_LI } },
	{ { KP6_S,	KP6_W,	KP6_U },			{ P6_SU,	P6_LU } },
	{ { KP6_S,	KP6_W,	KP6_E },			{ P6_SU,	P6_LE } },
	{ { KP6_S,	KP6_W,	KP6_O },			{ P6_SU,	P6_LO } },
	
	//ざじずぜぞ
	{ { KP6_Z,	KP6_A },					{ P6_SA,	P6_DD } },
	{ { KP6_Z,	KP6_I },					{ P6_SI,	P6_DD } },
	{ { KP6_J,	KP6_I },					{ P6_SI,	P6_DD } },
	{ { KP6_Z,	KP6_U },					{ P6_SU,	P6_DD } },
	{ { KP6_Z,	KP6_E },					{ P6_SE,	P6_DD } },
	{ { KP6_Z,	KP6_O },					{ P6_SO,	P6_DD } },
	
	//じゃじぃじゅじぇじょ
	{ { KP6_Z,	KP6_Y,	KP6_A },			{ P6_SI,	P6_DD,	P6_LYA } },
	{ { KP6_Z,	KP6_Y,	KP6_I },			{ P6_SI,	P6_DD,	P6_LI } },
	{ { KP6_Z,	KP6_Y,	KP6_U },			{ P6_SI,	P6_DD,	P6_LYU } },
	{ { KP6_Z,	KP6_Y,	KP6_E },			{ P6_SI,	P6_DD,	P6_LE } },
	{ { KP6_Z,	KP6_Y,	KP6_O },			{ P6_SI,	P6_DD,	P6_LYO } },
	
	//じゃじぃじゅじぇじょ
	{ { KP6_J,	KP6_Y,	KP6_A },			{ P6_SI,	P6_DD,	P6_LYA } },
	{ { KP6_J,	KP6_Y,	KP6_I },			{ P6_SI,	P6_DD,	P6_LI } },
	{ { KP6_J,	KP6_Y,	KP6_U },			{ P6_SI,	P6_DD,	P6_LYU } },
	{ { KP6_J,	KP6_Y,	KP6_E },			{ P6_SI,	P6_DD,	P6_LE } },
	{ { KP6_J,	KP6_Y,	KP6_O },			{ P6_SI,	P6_DD,	P6_LYO } },
	
	//じゃじ　じゅじぇじょ
	{ { KP6_J,	KP6_A },					{ P6_SI,	P6_DD,	P6_LYA } },
	{ { KP6_J,	KP6_I },					{ P6_SI,	P6_DD } },
	{ { KP6_J,	KP6_U },					{ P6_SI,	P6_DD,	P6_LYU } },
	{ { KP6_J,	KP6_E },					{ P6_SI,	P6_DD,	P6_LE } },
	{ { KP6_J,	KP6_O },					{ P6_SI,	P6_DD,	P6_LYO } },
	
	//ずぁずぃずぅずぇずぉ
	{ { KP6_Z,	KP6_W,	KP6_A },			{ P6_SU,	P6_DD,	P6_LA } },
	{ { KP6_Z,	KP6_W,	KP6_I },			{ P6_SU,	P6_DD,	P6_LI } },
	{ { KP6_Z,	KP6_W,	KP6_U },			{ P6_SU,	P6_DD,	P6_LU } },
	{ { KP6_Z,	KP6_W,	KP6_E },			{ P6_SU,	P6_DD,	P6_LE } },
	{ { KP6_Z,	KP6_W,	KP6_O },			{ P6_SU,	P6_DD,	P6_LO } },
	
	
	//たちつてと
	{ { KP6_T,	KP6_A },					{ P6_TA } },
	{ { KP6_T,	KP6_I },					{ P6_TI } },
	{ { KP6_T,	KP6_U },					{ P6_TU } },
	{ { KP6_T,	KP6_E },					{ P6_TE } },
	{ { KP6_T,	KP6_O },					{ P6_TO } },
	
	//っ
	{ { KP6_L,	KP6_T,	KP6_U },			{ P6_LTU } },
	{ { KP6_X,	KP6_T,	KP6_U },			{ P6_LTU } },
	{ { KP6_L,	KP6_T,	KP6_S,	KP6_U },	{ P6_LTU } },
	{ { KP6_X,	KP6_T,	KP6_S,	KP6_U },	{ P6_LTU } },
	
	//ちゃちぃちゅちぇちょ
	{ { KP6_T,	KP6_Y,	KP6_A },			{ P6_TI,	P6_LYA } },
	{ { KP6_T,	KP6_Y,	KP6_I },			{ P6_TI,	P6_LI } },
	{ { KP6_T,	KP6_Y,	KP6_U },			{ P6_TI,	P6_LYU } },
	{ { KP6_T,	KP6_Y,	KP6_E },			{ P6_TI,	P6_LE } },
	{ { KP6_T,	KP6_Y,	KP6_O },			{ P6_TI,	P6_LYO } },
	
	//ちゃちぃちゅちぇちょ
	{ { KP6_C,	KP6_Y,	KP6_A },			{ P6_TI,	P6_LYA } },
	{ { KP6_C,	KP6_Y,	KP6_I },			{ P6_TI,	P6_LI } },
	{ { KP6_C,	KP6_Y,	KP6_U },			{ P6_TI,	P6_LYU } },
	{ { KP6_C,	KP6_Y,	KP6_E },			{ P6_TI,	P6_LE } },
	{ { KP6_C,	KP6_Y,	KP6_O },			{ P6_TI,	P6_LYO } },
	
	//ちゃち　ちゅちぇちょ
	{ { KP6_C,	KP6_H,	KP6_A },			{ P6_TI,	P6_LYA } },
	{ { KP6_C,	KP6_H,	KP6_I },			{ P6_TI } },
	{ { KP6_C,	KP6_H,	KP6_U },			{ P6_TI,	P6_LYU } },
	{ { KP6_C,	KP6_H,	KP6_E },			{ P6_TI,	P6_LE } },
	{ { KP6_C,	KP6_H,	KP6_O },			{ P6_TI,	P6_LYO } },
	
	//つぁつぃつつぇつぉ
	{ { KP6_T,	KP6_S,	KP6_A },			{ P6_TU,	P6_LA } },
	{ { KP6_T,	KP6_S,	KP6_I },			{ P6_TU,	P6_LI } },
	{ { KP6_T,	KP6_S,	KP6_U },			{ P6_TU } },
	{ { KP6_T,	KP6_S,	KP6_E },			{ P6_TU,	P6_LE } },
	{ { KP6_T,	KP6_S,	KP6_O },			{ P6_TU,	P6_LO } },
	
	//てゃてぃてゅてぇてょ
	{ { KP6_T,	KP6_H,	KP6_A },			{ P6_TE,	P6_LYA } },
	{ { KP6_T,	KP6_H,	KP6_I },			{ P6_TE,	P6_LI } },
	{ { KP6_T,	KP6_H,	KP6_U },			{ P6_TE,	P6_LYU } },
	{ { KP6_T,	KP6_H,	KP6_E },			{ P6_TE,	P6_LE } },
	{ { KP6_T,	KP6_H,	KP6_O },			{ P6_TE,	P6_LYO } },
	
	//とぁとぃとぅとぇとぉ
	{ { KP6_T,	KP6_W,	KP6_A },			{ P6_TO,	P6_LA } },
	{ { KP6_T,	KP6_W,	KP6_I },			{ P6_TO,	P6_LI } },
	{ { KP6_T,	KP6_W,	KP6_U },			{ P6_TO,	P6_LU } },
	{ { KP6_T,	KP6_W,	KP6_E },			{ P6_TO,	P6_LE } },
	{ { KP6_T,	KP6_W,	KP6_O },			{ P6_TO,	P6_LO } },
	
	//だぢづでど
	{ { KP6_D,	KP6_A },					{ P6_TA,	P6_DD } },
	{ { KP6_D,	KP6_I },					{ P6_TI,	P6_DD } },
	{ { KP6_D,	KP6_U },					{ P6_TU,	P6_DD } },
	{ { KP6_D,	KP6_E },					{ P6_TE,	P6_DD } },
	{ { KP6_D,	KP6_O },					{ P6_TO,	P6_DD } },
	
	//ぢゃぢぃぢゅぢぇぢょ
	{ { KP6_D,	KP6_Y,	KP6_A },			{ P6_TI,	P6_DD,	P6_LYA } },
	{ { KP6_D,	KP6_Y,	KP6_I },			{ P6_TI,	P6_DD,	P6_LI } },
	{ { KP6_D,	KP6_Y,	KP6_U },			{ P6_TI,	P6_DD,	P6_LYU } },
	{ { KP6_D,	KP6_Y,	KP6_E },			{ P6_TI,	P6_DD,	P6_LE } },
	{ { KP6_D,	KP6_Y,	KP6_O },			{ P6_TI,	P6_DD,	P6_LYO } },
	
	//でゃでぃでゅでぇでょ
	{ { KP6_D,	KP6_H,	KP6_A },			{ P6_TE,	P6_DD,	P6_LYA } },
	{ { KP6_D,	KP6_H,	KP6_I },			{ P6_TE,	P6_DD,	P6_LI } },
	{ { KP6_D,	KP6_H,	KP6_U },			{ P6_TE,	P6_DD,	P6_LYU } },
	{ { KP6_D,	KP6_H,	KP6_E },			{ P6_TE,	P6_DD,	P6_LE } },
	{ { KP6_D,	KP6_H,	KP6_O },			{ P6_TE,	P6_DD,	P6_LYO } },
	
	//どぁどぃどぅどぇどぉ
	{ { KP6_D,	KP6_W,	KP6_A },			{ P6_TO,	P6_DD,	P6_LA } },
	{ { KP6_D,	KP6_W,	KP6_I },			{ P6_TO,	P6_DD,	P6_LI } },
	{ { KP6_D,	KP6_W,	KP6_U },			{ P6_TO,	P6_DD,	P6_LU } },
	{ { KP6_D,	KP6_W,	KP6_E },			{ P6_TO,	P6_DD,	P6_LE } },
	{ { KP6_D,	KP6_W,	KP6_O },			{ P6_TO,	P6_DD,	P6_LO } },
	
	
	//なにぬねの
	{ { KP6_N,	KP6_A },					{ P6_NA } },
	{ { KP6_N,	KP6_I },					{ P6_NI } },
	{ { KP6_N,	KP6_U },					{ P6_NU } },
	{ { KP6_N,	KP6_E },					{ P6_NE } },
	{ { KP6_N,	KP6_O },					{ P6_NO } },
	
	//にゃにぃにゅにぇにょ
	{ { KP6_N,	KP6_Y,	KP6_A },			{ P6_NI,	P6_LYA } },
	{ { KP6_N,	KP6_Y,	KP6_I },			{ P6_NI,	P6_LI } },
	{ { KP6_N,	KP6_Y,	KP6_U },			{ P6_NI,	P6_LYU } },
	{ { KP6_N,	KP6_Y,	KP6_E },			{ P6_NI,	P6_LE } },
	{ { KP6_N,	KP6_Y,	KP6_O },			{ P6_NI,	P6_LYO } },
	
	
	//はひふへほ
	{ { KP6_H,	KP6_A },					{ P6_HA } },
	{ { KP6_H,	KP6_I },					{ P6_HI } },
	{ { KP6_H,	KP6_U },					{ P6_HU } },
	{ { KP6_H,	KP6_E },					{ P6_HE } },
	{ { KP6_H,	KP6_O },					{ P6_HO } },
	
	//ひゃひぃひゅひぇひょ
	{ { KP6_H,	KP6_Y,	KP6_A },			{ P6_HI,	P6_LYA } },
	{ { KP6_H,	KP6_Y,	KP6_I },			{ P6_HI,	P6_LI } },
	{ { KP6_H,	KP6_Y,	KP6_U },			{ P6_HI,	P6_LYU } },
	{ { KP6_H,	KP6_Y,	KP6_E },			{ P6_HI,	P6_LE } },
	{ { KP6_H,	KP6_Y,	KP6_O },			{ P6_HI,	P6_LYO } },
	
	//びゃびぃびゅびぇびょ
	{ { KP6_B,	KP6_Y,	KP6_A },			{ P6_HI,	P6_DD,	P6_LYA } },
	{ { KP6_B,	KP6_Y,	KP6_I },			{ P6_HI,	P6_DD,	P6_LI } },
	{ { KP6_B,	KP6_Y,	KP6_U },			{ P6_HI,	P6_DD,	P6_LYU } },
	{ { KP6_B,	KP6_Y,	KP6_E },			{ P6_HI,	P6_DD,	P6_LE } },
	{ { KP6_B,	KP6_Y,	KP6_O },			{ P6_HI,	P6_DD,	P6_LYO } },
	
	//ぴゃぴぃぴゅぴぇぴょ
	{ { KP6_P,	KP6_Y,	KP6_A },			{ P6_HI,	P6_HD,	P6_LYA } },
	{ { KP6_P,	KP6_Y,	KP6_I },			{ P6_HI,	P6_HD,	P6_LI } },
	{ { KP6_P,	KP6_Y,	KP6_U },			{ P6_HI,	P6_HD,	P6_LYU } },
	{ { KP6_P,	KP6_Y,	KP6_E },			{ P6_HI,	P6_HD,	P6_LE } },
	{ { KP6_P,	KP6_Y,	KP6_O },			{ P6_HI,	P6_HD,	P6_LYO } },
	
	//ふぁふぃふゅふぇふぉ
	{ { KP6_H,	KP6_W,	KP6_A },			{ P6_HU,	P6_LA } },
	{ { KP6_H,	KP6_W,	KP6_I },			{ P6_HU,	P6_LI } },
	{ { KP6_H,	KP6_W,	KP6_Y,	KP6_U },	{ P6_HU,	P6_LU } },
	{ { KP6_H,	KP6_W,	KP6_E },			{ P6_HU,	P6_LE } },
	{ { KP6_H,	KP6_W,	KP6_O },			{ P6_HU,	P6_LO } },
	
	//ふぁふぃふ　ふぇふぉ
	{ { KP6_F,	KP6_A },					{ P6_HU,	P6_LA } },
	{ { KP6_F,	KP6_I },					{ P6_HU,	P6_LI } },
	{ { KP6_F,	KP6_U },					{ P6_HU } },
	{ { KP6_F,	KP6_E },					{ P6_HU,	P6_LE } },
	{ { KP6_F,	KP6_O },					{ P6_HU,	P6_LO } },
	
	//ふゃふゅふょ
	{ { KP6_F,	KP6_Y,	KP6_A },			{ P6_HU,	P6_LYA } },
	{ { KP6_F,	KP6_Y,	KP6_U },			{ P6_HU,	P6_LYU } },
	{ { KP6_F,	KP6_Y,	KP6_O },			{ P6_HU,	P6_LYO } },
	
	
	//ばびぶべぼ
	{ { KP6_B,	KP6_A },					{ P6_HA,	P6_DD } },
	{ { KP6_B,	KP6_I },					{ P6_HI,	P6_DD } },
	{ { KP6_B,	KP6_U },					{ P6_HU,	P6_DD } },
	{ { KP6_B,	KP6_E },					{ P6_HE,	P6_DD } },
	{ { KP6_B,	KP6_O },					{ P6_HO,	P6_DD } },
	
	//ぱぴぷぺぽ
	{ { KP6_P,	KP6_A },					{ P6_HA,	P6_HD } },
	{ { KP6_P,	KP6_I },					{ P6_HI,	P6_HD } },
	{ { KP6_P,	KP6_U },					{ P6_HU,	P6_HD } },
	{ { KP6_P,	KP6_E },					{ P6_HE,	P6_HD } },
	{ { KP6_P,	KP6_O },					{ P6_HO,	P6_HD } },
	
	
	//まみむめも
	{ { KP6_M,	KP6_A },					{ P6_MA } },
	{ { KP6_M,	KP6_I },					{ P6_MI } },
	{ { KP6_M,	KP6_U },					{ P6_MU } },
	{ { KP6_M,	KP6_E },					{ P6_ME } },
	{ { KP6_M,	KP6_O },					{ P6_MO } },
	
	//みゃみぃみゅみぇみょ
	{ { KP6_M,	KP6_Y,	KP6_A },			{ P6_MI,	P6_LYA } },
	{ { KP6_M,	KP6_Y,	KP6_I },			{ P6_MI,	P6_LI } },
	{ { KP6_M,	KP6_Y,	KP6_U },			{ P6_MI,	P6_LYU } },
	{ { KP6_M,	KP6_Y,	KP6_E },			{ P6_MI,	P6_LE } },
	{ { KP6_M,	KP6_Y,	KP6_O },			{ P6_MI,	P6_LYO } },
	
	
	//やゆよ
	{ { KP6_Y,	KP6_A },					{ P6_YA } },
	{ { KP6_Y,	KP6_U },					{ P6_YU } },
	{ { KP6_Y,	KP6_O },					{ P6_YO } },
	
	//ゃゅょ
	{ { KP6_L,	KP6_Y,	KP6_A },			{ P6_LYA } },
	{ { KP6_X,	KP6_Y,	KP6_A },			{ P6_LYA } },
	{ { KP6_L,	KP6_Y,	KP6_U },			{ P6_LYU } },
	{ { KP6_X,	KP6_Y,	KP6_U },			{ P6_LYU } },
	{ { KP6_L,	KP6_Y,	KP6_O },			{ P6_LYO } },
	{ { KP6_X,	KP6_Y,	KP6_O },			{ P6_LYO } },
	
	
	//らりるれろ
	{ { KP6_R,	KP6_A },					{ P6_RA } },
	{ { KP6_R,	KP6_I },					{ P6_RI } },
	{ { KP6_R,	KP6_U },					{ P6_RU } },
	{ { KP6_R,	KP6_E },					{ P6_RE } },
	{ { KP6_R,	KP6_O },					{ P6_RO } },
	
	//りゃりぃりゅりぇりょ
	{ { KP6_R,	KP6_Y,	KP6_A },			{ P6_RI,	P6_LYA } },
	{ { KP6_R,	KP6_Y,	KP6_I },			{ P6_RI,	P6_LI } },
	{ { KP6_R,	KP6_Y,	KP6_U },			{ P6_RI,	P6_LYU } },
	{ { KP6_R,	KP6_Y,	KP6_E },			{ P6_RI,	P6_LE } },
	{ { KP6_R,	KP6_Y,	KP6_O },			{ P6_RI,	P6_LYO } },
	
	
	//わ　うぃう　うぇを
	{ { KP6_W,	KP6_A },					{ P6_WA } },
	{ { KP6_W,	KP6_I },					{ P6_U,	P6_LI } },
	{ { KP6_W,	KP6_U },					{ P6_U } },
	{ { KP6_W,	KP6_E },					{ P6_U,	P6_LE } },
	{ { KP6_W,	KP6_O },					{ P6_WO } },
	
	
	//ん
	{ { KP6_N,	KP6_N },					{ P6_N } },
	{ { KP6_X,	KP6_N },					{ P6_N } },
	
	
	//かしくせこ
	{ { KP6_C,	KP6_A },					{ P6_KA } },
	{ { KP6_C,	KP6_I },					{ P6_SI } },
	{ { KP6_C,	KP6_U },					{ P6_KU } },
	{ { KP6_C,	KP6_E },					{ P6_SE } },
	{ { KP6_C,	KP6_O },					{ P6_KO } },
	
	//ぃ
	{ { KP6_L,	KP6_Y,	KP6_I },			{ P6_LI } },
	{ { KP6_X,	KP6_Y,	KP6_I },			{ P6_LI } },
	
	//ぇ
	{ { KP6_L,	KP6_Y,	KP6_E },			{ P6_LE } },
	{ { KP6_X,	KP6_Y,	KP6_E },			{ P6_LE } },
	
	//いぇ
	{ { KP6_Y,	KP6_E },					{ P6_I,	P6_LE } },
	
	//ゔぁゔぃゔゔぇゔぉ
	{ { KP6_V,	KP6_A },					{ P6_U,	P6_DD,	P6_LA } },
	{ { KP6_V,	KP6_I },					{ P6_U,	P6_DD,	P6_LI } },
	{ { KP6_V,	KP6_U },					{ P6_U,	P6_DD } },
	{ { KP6_V,	KP6_E },					{ P6_U,	P6_DD,	P6_LE } },
	{ { KP6_V,	KP6_O },					{ P6_U,	P6_DD,	P6_LO } },
	
	//ゔゃゔぃゔゅゔぇゔょ
	{ { KP6_V,	KP6_Y,	KP6_A },			{ P6_U,	P6_DD,	P6_LYA } },
	{ { KP6_V,	KP6_Y,	KP6_I },			{ P6_U,	P6_DD,	P6_LI } },
	{ { KP6_V,	KP6_Y,	KP6_U },			{ P6_U,	P6_DD,	P6_LYU } },
	{ { KP6_V,	KP6_Y,	KP6_E },			{ P6_U,	P6_DD,	P6_LE } },
	{ { KP6_V,	KP6_Y,	KP6_O },			{ P6_U,	P6_DD,	P6_LYO } },
	
	
	// 特殊設定キー
	{ { KP6_MINUS },						{ P6_CYOUON } },	//ー
	{ { KP6_LBRACKET },						{ P6_LBRK } },		// 「
	{ { KP6_RBRACKET },						{ P6_RBRK } },		// 」
	{ { KP6_COMMA },						{ P6_TOUTEN } },	//。
	{ { KP6_PERIOD },						{ P6_KUTEN } },		//、
	{ { KP6_SLASH },						{ P6_NAKATEN } },	//・
};


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
Romaji::Romaji( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
Romaji::~Romaji( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// ローマ字かな変換
//
// エミュレータレイヤのkeydownイベントで、かなモードかつ、ローマ字変換モードなら、これを呼ぶ。
// 処理：ローマ字変換できるかチェックして、変換に成功したら、HENKAN_SUCCESS を返す。
//       GetResult() で、変換結果を取得する。
//
// 引数:	keycode	P6キーコード
// 返値:	int		HENKAN_SUCCESS	ローマ字変換に成功した
//					HENKAN_FAILED	変換失敗
//					HENKAN_DOING	変換中
//					HENKAN_CANCEL	無変換
/////////////////////////////////////////////////////////////////////////////
int Romaji::ConvertRomaji2kana( const P6KEYsym keycode )
{
	// CTRL,SHIFT,ALT,CAPSはローマ字変換を無視する
	const std::vector<P6KEYsym> keys = { KP6_CTRL, KP6_SHIFT, KP6_GRAPH, KP6_CAPS };
	if( std::find( keys.begin(), keys.end(), keycode ) != keys.end() ){
		return HENKAN_CANCEL;
	}
	
	// ローマ字入力に関係ないキーは捨てる
	const std::vector<P6KEYsym> igkeys = { 
		KP6_1, KP6_2, KP6_3, KP6_4, KP6_5, KP6_6, KP6_7, KP6_8, KP6_9, KP6_0,
		KP6_CARET, KP6_YEN, KP6_AT, KP6_SEMICOLON, KP6_COLON, KP6_UNDERSCORE };
	if( std::find( igkeys.begin(), igkeys.end(), keycode ) != igkeys.end() ){
		return HENKAN_DOING;
	}
	
	buff.push_back( keycode );
	
	// 同じ子音が重なって入力されたら「っ」に変換
	if( buff.size() > 1 && isShiin( buff[0] ) && buff[0] == buff[1] ){
		SetOutStr( romaji_tbl.find( { KP6_L, KP6_T, KP6_U } )->second );
		buff.erase( buff.begin() );	// 1つ目の子音を削除
		return HENKAN_SUCCESS;
	}
	
	// Nの後にN,Y,母音以外が入力されたら「ん」に変換
	if( buff.size() > 1 && buff[0] == KP6_N && buff[1] != KP6_N && buff[1] != KP6_Y && !isBoin( buff[1] ) ){
		SetOutStr( romaji_tbl.find( { KP6_N, KP6_N } )->second );
		buff.erase( buff.begin() );	// Nを削除
		return HENKAN_SUCCESS;
	}
	
	// 完全一致パターンを探す
	auto pkey = romaji_tbl.find( buff );
	if( pkey != romaji_tbl.end() ){
		SetOutStr( pkey->second );
		buff.clear();
		return HENKAN_SUCCESS;		// complete match
	}else{
		// 部分一致パターンを探す
		pkey = romaji_tbl.upper_bound( buff );
		if( pkey != romaji_tbl.end() && buff.size() < pkey->first.size() ){
			return HENKAN_DOING;	// part match
		}else if( buff.size() > 1 ){
			// あり得ない組み合わせだったら無視する
			buff.clear();
			return HENKAN_DOING;
		}
	}
	
	buff.clear();
	return HENKAN_FAILED;
}


/////////////////////////////////////////////////////////////////////////////
// ローマ字かな変換結果取得
//
// 引数:	kkana			カタカナフラグ true:カタカナ false:ひらがな
// 返値:	std::string&	文字列への参照
/////////////////////////////////////////////////////////////////////////////
const std::string& Romaji::GetResult( bool kkana )
{
	if( kkana ){	// ひらがな→カタカナ変換
		for( char& c : outstr ){
			if( (0x86 <= (uint8_t)c && (uint8_t)c <= 0x8f) ||
			    (0x91 <= (uint8_t)c && (uint8_t)c <= 0x9f) ||
			    (0xe0 <= (uint8_t)c && (uint8_t)c <= 0xfd) ){
				c ^= (uint8_t)0x20;
			}
		}
	}
	return outstr;
}


/////////////////////////////////////////////////////////////////////////////
// 母音かどうか？
//
// 引数:	keycode	P6キーコード
// 返値:	bool	true:はい false:いいえ
/////////////////////////////////////////////////////////////////////////////
bool Romaji::isBoin( const P6KEYsym keycode )
{
	const std::vector<P6KEYsym> keys = { KP6_A, KP6_I, KP6_U, KP6_E, KP6_O };
	
	return std::find( keys.begin(), keys.end(), keycode ) != keys.end() ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// 子音かどうか？
//
// 引数:	keycode	P6キーコード
// 返値:	bool	true:はい false:いいえ
/////////////////////////////////////////////////////////////////////////////
bool Romaji::isShiin( const P6KEYsym keycode )
{
	const std::vector<P6KEYsym> keys = {
		KP6_K, KP6_S, KP6_T, KP6_H, KP6_M, KP6_Y, KP6_R, KP6_W, KP6_P, KP6_F,
		KP6_G, KP6_D, KP6_Z, KP6_B, KP6_V, KP6_Q, KP6_X, KP6_J, KP6_C, KP6_L	// Lも子音に混ぜる
	};
	
	return std::find( keys.begin(), keys.end(), keycode ) != keys.end() ? true : false;
}


/////////////////////////////////////////////////////////////////////////////
// 出力文字列設定(vector->string)
//
// 引数:	vec		文字列のvectorデータへの参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void Romaji::SetOutStr( const std::vector<uint8_t>& vec )
{
	outstr = std::string( vec.begin(), vec.end() );	// vector -> string
}
