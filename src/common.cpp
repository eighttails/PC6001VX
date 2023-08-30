/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <cstring>
#include <algorithm>
#include <new>
#include <map>
#include <vector>

#include "pc6001v.h"

#include "common.h"
#include "log.h"
#include "osd.h"
//#include "png.h"


#include <locale.h>

#ifndef NOLIBINTL
#include <libintl.h>
#define	_(str)	gettext(str)
#define N_(str)	gettext_noop(str)
#define gettext_noop(str)	str
#else
#define	_(str)	str
#define N_(str)	str
#define gettext_noop(str)	str
#endif


/////////////////////////////////////////////////////////////////////////////
// 文字コード操作関数
/////////////////////////////////////////////////////////////////////////////

// isioさんの txt2bas をベースに大幅改変
// http://retropc.net/isio/mysoft/#txt2bas
/****************************************************/
/* txt2bas                                          */
/* sjis.h                                           */
/* 2000.03.26. by ishioka                           */
/* 2000.05.25., 2001.01.06.                         */
/****************************************************/

// SJIS -> P6 character set table
static const std::map<WORD, std::vector<BYTE>> sjisp6tbl =
{
	{ 0x8140, { ' ' } },		//
	{ 0x8149, { '!' } },		// ！
	{ 0x8168, { '"' } },		// ”
	{ 0x8194, { '#' } },		// ＃
	{ 0x8190, { '$' } },		// ＄
	{ 0x8193, { '%' } },		// ％
	{ 0x8195, { '&' } },		// ＆
	{ 0x8166, { '\'' } },		// ’
	{ 0x8169, { '(' } },		// （
	{ 0x816a, { ')' } },		// ）
	{ 0x8196, { '*' } },		// ＊
	{ 0x817b, { '+' } },		// ＋
	{ 0x8143, { ',' } },		// ，
	{ 0x817c, { '-' } },		// －
	{ 0x815b, { '-' } },		// ー
	{ 0x8144, { '.' } },		// ．
	{ 0x815e, { '/' } },		// ／
	{ 0x824f, { '0' } },		// ０
	{ 0x8250, { '1' } },		// １
	{ 0x8251, { '2' } },		// ２
	{ 0x8252, { '3' } },		// ３
	{ 0x8253, { '4' } },		// ４
	{ 0x8254, { '5' } },		// ５
	{ 0x8255, { '6' } },		// ６
	{ 0x8256, { '7' } },		// ７
	{ 0x8257, { '8' } },		// ８
	{ 0x8258, { '9' } },		// ９
	{ 0x8146, { ':' } },		// ：
	{ 0x8147, { ';' } },		// ；
	{ 0x8183, { '<' } },		// ＜
	{ 0x8181, { '=' } },		// ＝
	{ 0x8184, { '>' } },		// ＞
	{ 0x8148, { '?' } },		// ？
	{ 0x8197, { '@' } },		// ＠
	{ 0x8260, { 'A' } },		// Ａ
	{ 0x8261, { 'B' } },		// Ｂ
	{ 0x8262, { 'C' } },		// Ｃ
	{ 0x8263, { 'D' } },		// Ｄ
	{ 0x8264, { 'E' } },		// Ｅ
	{ 0x8265, { 'F' } },		// Ｆ
	{ 0x8266, { 'G' } },		// Ｇ
	{ 0x8267, { 'H' } },		// Ｈ
	{ 0x8268, { 'I' } },		// Ｉ
	{ 0x8269, { 'J' } },		// Ｊ
	{ 0x826a, { 'K' } },		// Ｋ
	{ 0x826b, { 'L' } },		// Ｌ
	{ 0x826c, { 'M' } },		// Ｍ
	{ 0x826d, { 'N' } },		// Ｎ
	{ 0x826e, { 'O' } },		// Ｏ
	{ 0x826f, { 'P' } },		// Ｐ
	{ 0x8270, { 'Q' } },		// Ｑ
	{ 0x8271, { 'R' } },		// Ｒ
	{ 0x8272, { 'S' } },		// Ｓ
	{ 0x8273, { 'T' } },		// Ｔ
	{ 0x8274, { 'U' } },		// Ｕ
	{ 0x8275, { 'V' } },		// Ｖ
	{ 0x8276, { 'W' } },		// Ｗ
	{ 0x8277, { 'X' } },		// Ｘ
	{ 0x8278, { 'Y' } },		// Ｙ
	{ 0x8279, { 'Z' } },		// Ｚ
	{ 0x816d, { '[' } },		// ［
	{ 0x818f, { '\\' } },		// ￥
	{ 0x816e, { ']' } },		// ］
	{ 0x814f, { '^' } },		// ＾
	{ 0x8151, { '_' } },		// ＿
	{ 0x8281, { 'a' } },		// ａ
	{ 0x8282, { 'b' } },		// ｂ
	{ 0x8283, { 'c' } },		// ｃ
	{ 0x8284, { 'd' } },		// ｄ
	{ 0x8285, { 'e' } },		// ｅ
	{ 0x8286, { 'f' } },		// ｆ
	{ 0x8287, { 'g' } },		// ｇ
	{ 0x8288, { 'h' } },		// ｈ
	{ 0x8289, { 'i' } },		// ｉ
	{ 0x828a, { 'j' } },		// ｊ
	{ 0x828b, { 'k' } },		// ｋ
	{ 0x828c, { 'l' } },		// ｌ
	{ 0x828d, { 'm' } },		// ｍ
	{ 0x828e, { 'n' } },		// ｎ
	{ 0x828f, { 'o' } },		// ｏ
	{ 0x8290, { 'p' } },		// ｐ
	{ 0x8291, { 'q' } },		// ｑ
	{ 0x8292, { 'r' } },		// ｒ
	{ 0x8293, { 's' } },		// ｓ
	{ 0x8294, { 't' } },		// ｔ
	{ 0x8295, { 'u' } },		// ｕ
	{ 0x8296, { 'v' } },		// ｖ
	{ 0x8297, { 'w' } },		// ｗ
	{ 0x8298, { 'x' } },		// ｘ
	{ 0x8299, { 'y' } },		// ｙ
	{ 0x829a, { 'z' } },		// ｚ
	{ 0x8c8e, { 0x14, 0x01 } },	// 月 +0x30
	{ 0x89ce, { 0x14, 0x02 } },	// 火 +0x30
	{ 0x9085, { 0x14, 0x03 } },	// 水 +0x30
	{ 0x96d8, { 0x14, 0x04 } },	// 木 +0x30
	{ 0x8be0, { 0x14, 0x05 } },	// 金 +0x30
	{ 0x9379, { 0x14, 0x06 } },	// 土 +0x30
	{ 0x93fa, { 0x14, 0x07 } },	// 日 +0x30
	{ 0x944e, { 0x14, 0x08 } },	// 年 +0x30
	{ 0x897e, { 0x14, 0x09 } },	// 円 +0x30
	{ 0x8e9e, { 0x14, 0x0a } },	// 時 +0x30
	{ 0x95aa, { 0x14, 0x0b } },	// 分 +0x30
	{ 0x9562, { 0x14, 0x0c } },	// 秒 +0x30
	{ 0x9553, { 0x14, 0x0d } },	// 百 +0x30
	{ 0x90e7, { 0x14, 0x0e } },	// 千 +0x30
	{ 0x969c, { 0x14, 0x0f } },	// 万 +0x30
	{ 0x83ce, { 0x14, 0x10 } },	// π +0x30
	{ 0x84a8, { 0x14, 0x11 } },	// ┴ +0x30
	{ 0x84a6, { 0x14, 0x12 } },	// ┬ +0x30
	{ 0x84a7, { 0x14, 0x13 } },	// ┤ +0x30
	{ 0x84a5, { 0x14, 0x14 } },	// ├ +0x30
	{ 0x84a9, { 0x14, 0x15 } },	// ┼ +0x30
	{ 0x84a0, { 0x14, 0x16 } },	// │ +0x30
	{ 0x849f, { 0x14, 0x17 } },	// ─ +0x30
	{ 0x84a1, { 0x14, 0x18 } },	// ┌ +0x30
	{ 0x84a2, { 0x14, 0x19 } },	// ┐ +0x30
	{ 0x84a4, { 0x14, 0x1a } },	// └ +0x30
	{ 0x84a3, { 0x14, 0x1b } },	// ┘ +0x30
	{ 0x84b3, { 0x14, 0x11 } },	// ┻ +0x30
	{ 0x84b1, { 0x14, 0x12 } },	// ┳ +0x30
	{ 0x84b2, { 0x14, 0x13 } },	// ┫ +0x30
	{ 0x84b0, { 0x14, 0x14 } },	// ┣ +0x30
	{ 0x84b4, { 0x14, 0x15 } },	// ╋ +0x30
	{ 0x84ab, { 0x14, 0x16 } },	// ┃ +0x30
	{ 0x84aa, { 0x14, 0x17 } },	// ━ +0x30
	{ 0x84ac, { 0x14, 0x18 } },	// ┏ +0x30
	{ 0x84ad, { 0x14, 0x19 } },	// ┓ +0x30
	{ 0x84af, { 0x14, 0x1a } },	// ┗ +0x30
	{ 0x84ae, { 0x14, 0x1b } },	// ┛ +0x30
	{ 0x817e, { 0x14, 0x1c } },	// × +0x30
	{ 0x91e5, { 0x14, 0x1d } },	// 大 +0x30
	{ 0x9286, { 0x14, 0x1e } },	// 中 +0x30
	{ 0x8fac, { 0x14, 0x1f } },	// 小 +0x30
	{ 0x81a3, { 0x80 } },		// ▲
	{ 0x81a5, { 0x81 } },		// ▼
	{ 0x819a, { 0x82 } },		// ★
	{ 0x819f, { 0x83 } },		// ◆
	{ 0x819b, { 0x84 } },		// ○
	{ 0x819c, { 0x85 } },		// ●
	{ 0x82f0, { 0x86 } },		// を
	{ 0x829f, { 0x87 } },		// ぁ
	{ 0x82a1, { 0x88 } },		// ぃ
	{ 0x82a3, { 0x89 } },		// ぅ
	{ 0x82a5, { 0x8a } },		// ぇ
	{ 0x82a7, { 0x8b } },		// ぉ
	{ 0x82e1, { 0x8c } },		// ゃ
	{ 0x82e3, { 0x8d } },		// ゅ
	{ 0x82e5, { 0x8e } },		// ょ
	{ 0x82c1, { 0x8f } },		// っ
	{ 0x82a0, { 0x91 } },		// あ
	{ 0x82a2, { 0x92 } },		// い
	{ 0x82a4, { 0x93 } },		// う
	{ 0x82a6, { 0x94 } },		// え
	{ 0x82a8, { 0x95 } },		// お
	{ 0x82a9, { 0x96 } },		// か
	{ 0x82ab, { 0x97 } },		// き
	{ 0x82ad, { 0x98 } },		// く
	{ 0x82af, { 0x99 } },		// け
	{ 0x82b1, { 0x9a } },		// こ
	{ 0x82b3, { 0x9b } },		// さ
	{ 0x82b5, { 0x9c } },		// し
	{ 0x82b7, { 0x9d } },		// す
	{ 0x82b9, { 0x9e } },		// せ
	{ 0x82bb, { 0x9f } },		// そ
	{ 0x8142, { 0xa1 } },		// 。
	{ 0x8175, { 0xa2 } },		// 「
	{ 0x8176, { 0xa3 } },		// 」
	{ 0x8141, { 0xa4 } },		// 、
	{ 0x8145, { 0xa5 } },		// ・
	{ 0x8392, { 0xa6 } },		// ヲ
	{ 0x8340, { 0xa7 } },		// ァ
	{ 0x8342, { 0xa8 } },		// ィ
	{ 0x8344, { 0xa9 } },		// ゥ
	{ 0x8346, { 0xaa } },		// ェ
	{ 0x8348, { 0xab } },		// ォ
	{ 0x8383, { 0xac } },		// ャ
	{ 0x8385, { 0xad } },		// ュ
	{ 0x8387, { 0xae } },		// ョ
	{ 0x8362, { 0xaf } },		// ッ
	{ 0x8341, { 0xb1 } },		// ア
	{ 0x8343, { 0xb2 } },		// イ
	{ 0x8345, { 0xb3 } },		// ウ
	{ 0x8347, { 0xb4 } },		// エ
	{ 0x8349, { 0xb5 } },		// オ
	{ 0x834a, { 0xb6 } },		// カ
	{ 0x834c, { 0xb7 } },		// キ
	{ 0x834e, { 0xb8 } },		// ク
	{ 0x8350, { 0xb9 } },		// ケ
	{ 0x8352, { 0xba } },		// コ
	{ 0x8354, { 0xbb } },		// サ
	{ 0x8356, { 0xbc } },		// シ
	{ 0x8358, { 0xbd } },		// ス
	{ 0x835a, { 0xbe } },		// セ
	{ 0x835c, { 0xbf } },		// ソ
	{ 0x835e, { 0xc0 } },		// タ
	{ 0x8360, { 0xc1 } },		// チ
	{ 0x8363, { 0xc2 } },		// ツ
	{ 0x8365, { 0xc3 } },		// テ
	{ 0x8367, { 0xc4 } },		// ト
	{ 0x8369, { 0xc5 } },		// ナ
	{ 0x836a, { 0xc6 } },		// ニ
	{ 0x836b, { 0xc7 } },		// ヌ
	{ 0x836c, { 0xc8 } },		// ネ
	{ 0x836d, { 0xc9 } },		// ノ
	{ 0x836e, { 0xca } },		// ハ
	{ 0x8371, { 0xcb } },		// ヒ
	{ 0x8374, { 0xcc } },		// フ
	{ 0x8377, { 0xcd } },		// ヘ
	{ 0x837a, { 0xce } },		// ホ
	{ 0x837d, { 0xcf } },		// マ
	{ 0x837e, { 0xd0 } },		// ミ
	{ 0x8380, { 0xd1 } },		// ム
	{ 0x8381, { 0xd2 } },		// メ
	{ 0x8382, { 0xd3 } },		// モ
	{ 0x8384, { 0xd4 } },		// ヤ
	{ 0x8386, { 0xd5 } },		// ユ
	{ 0x8388, { 0xd6 } },		// ヨ
	{ 0x8389, { 0xd7 } },		// ラ
	{ 0x838a, { 0xd8 } },		// リ
	{ 0x838b, { 0xd9 } },		// ル
	{ 0x838c, { 0xda } },		// レ
	{ 0x838d, { 0xdb } },		// ロ
	{ 0x838f, { 0xdc } },		// ワ
	{ 0x8393, { 0xdd } },		// ン
	{ 0x814a, { 0xde } },		// ゛
	{ 0x814b, { 0xdf } },		// ゜
	{ 0x82bd, { 0xe0 } },		// た
	{ 0x82bf, { 0xe1 } },		// ち
	{ 0x82c2, { 0xe2 } },		// つ
	{ 0x82c4, { 0xe3 } },		// て
	{ 0x82c6, { 0xe4 } },		// と
	{ 0x82c8, { 0xe5 } },		// な
	{ 0x82c9, { 0xe6 } },		// に
	{ 0x82ca, { 0xe7 } },		// ぬ
	{ 0x82cb, { 0xe8 } },		// ね
	{ 0x82cc, { 0xe9 } },		// の
	{ 0x82cd, { 0xea } },		// は
	{ 0x82d0, { 0xeb } },		// ひ
	{ 0x82d3, { 0xec } },		// ふ
	{ 0x82d6, { 0xed } },		// へ
	{ 0x82d9, { 0xee } },		// ほ
	{ 0x82dc, { 0xef } },		// ま
	{ 0x82dd, { 0xf0 } },		// み
	{ 0x82de, { 0xf1 } },		// む
	{ 0x82df, { 0xf2 } },		// め
	{ 0x82e0, { 0xf3 } },		// も
	{ 0x82e2, { 0xf4 } },		// や
	{ 0x82e4, { 0xf5 } },		// ゆ
	{ 0x82e6, { 0xf6 } },		// よ
	{ 0x82e7, { 0xf7 } },		// ら
	{ 0x82e8, { 0xf8 } },		// り
	{ 0x82e9, { 0xf9 } },		// る
	{ 0x82ea, { 0xfa } },		// れ
	{ 0x82eb, { 0xfb } },		// ろ
	{ 0x82ed, { 0xfc } },		// わ
	{ 0x82f1, { 0xfd } },		// ん
	{ 0x82aa, { 0x96, 0xde } },	// が
	{ 0x82ac, { 0x97, 0xde } },	// ぎ
	{ 0x82ae, { 0x98, 0xde } },	// ぐ
	{ 0x82b0, { 0x99, 0xde } },	// げ
	{ 0x82b2, { 0x9a, 0xde } },	// ご
	{ 0x82b4, { 0x9b, 0xde } },	// ざ
	{ 0x82b6, { 0x9c, 0xde } },	// じ
	{ 0x82b8, { 0x9d, 0xde } },	// ず
	{ 0x82ba, { 0x9e, 0xde } },	// ぜ
	{ 0x82bc, { 0x9f, 0xde } },	// ぞ
	{ 0x82be, { 0xe0, 0xde } },	// だ
	{ 0x82c0, { 0xe1, 0xde } },	// ぢ
	{ 0x82c3, { 0xe2, 0xde } },	// づ
	{ 0x82c5, { 0xe3, 0xde } },	// で
	{ 0x82c7, { 0xe4, 0xde } },	// ど
	{ 0x82ce, { 0xea, 0xde } },	// ば
	{ 0x82d1, { 0xeb, 0xde } },	// び
	{ 0x82d4, { 0xec, 0xde } },	// ぶ
	{ 0x82d7, { 0xed, 0xde } },	// べ
	{ 0x82da, { 0xee, 0xde } },	// ぼ
	{ 0x834b, { 0xb6, 0xde } },	// ガ
	{ 0x834d, { 0xb7, 0xde } },	// ギ
	{ 0x834f, { 0xb8, 0xde } },	// グ
	{ 0x8351, { 0xb9, 0xde } },	// ゲ
	{ 0x8353, { 0xba, 0xde } },	// ゴ
	{ 0x8355, { 0xbb, 0xde } },	// ザ
	{ 0x8357, { 0xbc, 0xde } },	// ジ
	{ 0x8359, { 0xbd, 0xde } },	// ズ
	{ 0x835b, { 0xbe, 0xde } },	// ゼ
	{ 0x835d, { 0xbf, 0xde } },	// ゾ
	{ 0x835f, { 0xc0, 0xde } },	// ダ
	{ 0x8361, { 0xc1, 0xde } },	// ヂ
	{ 0x8364, { 0xc2, 0xde } },	// ヅ
	{ 0x8366, { 0xc3, 0xde } },	// デ
	{ 0x8368, { 0xc4, 0xde } },	// ド
	{ 0x836f, { 0xca, 0xde } },	// バ
	{ 0x8372, { 0xcb, 0xde } },	// ビ
	{ 0x8375, { 0xcc, 0xde } },	// ブ
	{ 0x8378, { 0xcd, 0xde } },	// ベ
	{ 0x837b, { 0xce, 0xde } },	// ボ
	{ 0x8394, { 0xb3, 0xde } },	// ヴ
	{ 0x82cf, { 0xea, 0xdf } },	// ぱ
	{ 0x82d2, { 0xeb, 0xdf } },	// ぴ
	{ 0x82d5, { 0xec, 0xdf } },	// ぷ
	{ 0x82d8, { 0xed, 0xdf } },	// ぺ
	{ 0x82db, { 0xee, 0xdf } },	// ぽ
	{ 0x8370, { 0xca, 0xdf } },	// パ
	{ 0x8373, { 0xcb, 0xdf } },	// ピ
	{ 0x8376, { 0xcc, 0xdf } },	// プ
	{ 0x8379, { 0xcd, 0xdf } },	// ペ
	{ 0x837c, { 0xce, 0xdf } }	// ポ
};



/////////////////////////////////////////////////////////////////////////////
// SJIS -> P6
//
// 引数:	dstr		P6文字列格納バッファへの参照(末尾に追記)
//			sstr		SJIS文字列への参照
// 返値:	bool		true:正常終了 false:変換できない文字があった
/////////////////////////////////////////////////////////////////////////////
bool Sjis2P6( std::string& dstr, const std::string& sstr )
{
	WORD w = 0;

	for( auto &c : sstr ){
		if( w ){
			w += (BYTE)c;
			auto c2 = sjisp6tbl.find( w );
			if( c2 == sjisp6tbl.end() ){
				return false;
			}
			dstr.insert( dstr.end(), c2->second.begin(), c2->second.end());
			w = 0;
		}else if( (((BYTE)c >= 0x80) && ((BYTE)c <= 0x9f)) || (((BYTE)c >= 0xe0) && ((BYTE)c <= 0xef)) ){
			w = (BYTE)c << 8;
		}else if( c <= 0x1f ){
			switch( c ){
			case 0x09:
			case 0x0d:
				dstr += c;
			}
		}else{
			dstr += c;
		}
	}
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// 文字列操作関数
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// 小文字による文字列比較
//
// 引数:	s1			比較する文字列へのポインタ1
//			s2			比較する文字列へのポインタ2
// 返値:	int			0:等しい 正数:s1>s2 負数:s1<s2
/////////////////////////////////////////////////////////////////////////////
int StriCmp( const std::string& s1, const std::string& s2 )
{
	std::string str1 = s1;
	std::string str2 = s2;

	// 小文字化
	std::transform( str1.begin(), str1.end(), str1.begin(), ::tolower );
	std::transform( str2.begin(), str2.end(), str2.begin(), ::tolower );

	return str1.compare( str2 );
}


/////////////////////////////////////////////////////////////////////////////
// 画像ファイル操作関数
/////////////////////////////////////////////////////////////////////////////
#if 0 // P6VXでは独自実装
/////////////////////////////////////////////////////////////////////////////
// Img SAVE from Data
//   情報参照「PNG利用術」 http://gmoon.jp/png/
//			 「プログラミング・ライブラリ」 http://dencha.ojaru.jp/
//
// 引数:	filepath		出力ファイルパス
//			pixels			保存するデータ領域へのポインタ
//			bpp				色深度
//			ww				幅
//			hh				高さ
//			pos				保存する領域情報へのポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool SaveImgData( const P6VPATH& filepath, BYTE* pixels, const int bpp, const int ww, const int hh, VRect* pos )
{
	PRINTD( GRP_LOG, "[COMMON][SaveImgData] -> %s\n", P6VPATH2STR( filepath ).c_str() );

	FILE* fp           = nullptr;
	png_structp PngPtr = nullptr;
	png_infop InfoPtr  = nullptr;
	png_bytepp image   = nullptr;	// image[HEIGHT][WIDTH]の形式
	VRect rec;


	if( !(bpp == 32 || bpp == 24 || bpp == 8 || bpp == 1) ){
		return false;
	}

	// 領域設定
	if( pos ){
		rec.x = pos->x;	rec.y = pos->y;
		rec.w = pos->w;	rec.h = pos->h;
	}else{
		rec.x =     rec.y = 0;
		rec.w = ww; rec.h = hh;
	}

	int spit = ( (ww    * bpp + 31 ) / 32 ) * sizeof(DWORD);
	int dpit = ( (rec.w * bpp + 31 ) / 32 ) * sizeof(DWORD);


	// 各構造体を確保・初期化する
	if( !(PngPtr = png_create_write_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)) ){
		return false;
	}

	if( !(InfoPtr = png_create_info_struct( PngPtr )) ){
		png_destroy_write_struct( &PngPtr, nullptr );
		return false;
	}

	// エラー処理
	if( setjmp( png_jmpbuf( PngPtr ) ) ){
		png_destroy_write_struct( &PngPtr, &InfoPtr );
		if( fp ){ fclose( fp ); }
		if( image ){
			for( int i = 0; i < rec.h; i++ ){
				if( image[i] ){ delete [] image[i]; }
			}
			delete [] image;
		}
		return false;
	}

	// 圧縮率の設定
	png_set_compression_level( PngPtr, 9 );

	// IHDRチャンク情報を設定する
	png_set_IHDR( PngPtr, InfoPtr, rec.w, rec.h, bpp == 1 ? 1 : 8,
					bpp == 1  ? PNG_COLOR_TYPE_GRAY :
					bpp == 8  ? PNG_COLOR_TYPE_PALETTE :
					bpp == 24 ? PNG_COLOR_TYPE_RGB :
							    PNG_COLOR_TYPE_RGB_ALPHA,
					PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT,
					PNG_FILTER_TYPE_DEFAULT );

	if( bpp == 8 ){	// 8bitの場合
		// パレットを作る
		png_colorp Palette = (png_colorp)png_malloc( PngPtr, 256 * sizeof(png_color) );
		// サーフェスからパレットを取得
		for( int i = 0; i < 256; i++ ){
			Palette[i].red   = (VSurface::GetColor( i ) >> RSHIFT32) & 0xff;
			Palette[i].green = (VSurface::GetColor( i ) >> GSHIFT32) & 0xff;
			Palette[i].blue  = (VSurface::GetColor( i ) >> BSHIFT32) & 0xff;
		}
		png_set_PLTE( PngPtr, InfoPtr, Palette, 256 );
	}

	// イメージデータを2次元配列に配置する
	image = (png_bytepp)png_malloc( PngPtr, sizeof(png_bytep) * rec.h );
	ZeroMemory( image, sizeof(png_bytep) * rec.h );
	BYTE* doff = pixels + rec.x + rec.y * spit;
	for( int i = 0; i < rec.h; i++ ){
		image[i] = (png_byte*)png_malloc( PngPtr, dpit );
		std::memcpy( image[i], doff, dpit );
		doff += spit;
	}

	// イメージデータをlibpngに知らせる
	png_set_rows( PngPtr, InfoPtr, image );

	// 画像ファイルを開く
	if( !(fp = OSD_Fopen( filepath, "wb" )) ){
		for( int i = 0; i < rec.h; i++ ){
			if( image[i] ){ delete [] image[i]; }
		}
		delete [] image;
		png_destroy_write_struct( &PngPtr, &InfoPtr );
		return false;
	}

	// libpngにfpを知らせる
	png_init_io( PngPtr, fp );

	// 画像データを書込む
	png_write_png( PngPtr, InfoPtr, PNG_TRANSFORM_IDENTITY, nullptr );

	// 画像ファイルを閉じる
	fclose( fp );

	// イメージデータを開放する
	for( int i = 0; i < rec.h; i++ ) delete [] image[i];
	delete [] image;

	// 2つの構造体のメモリを解放する
	png_destroy_write_struct( &PngPtr, &InfoPtr );

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Img SAVE
//   情報参照「PNG利用術」 http://gmoon.jp/png/
//			 「プログラミング・ライブラリ」 http://dencha.ojaru.jp/
//
// 引数:	filepath		出力ファイルパス
//			sur				保存するサーフェスへのポインタ
//			pos				保存する領域情報へのポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool SaveImg( const P6VPATH& filepath, VSurface* sur, VRect* pos )
{
	return SaveImgData( filepath, (BYTE*)sur->GetPixels().data(), 8, sur->Width(), sur->Height(), pos );
}


/////////////////////////////////////////////////////////////////////////////
// Img LOAD
//   情報参照「PNG利用術」 http://gmoon.jp/png/
//			 「プログラミング・ライブラリ」 http://dencha.ojaru.jp/
//
// 引数:	filepath		入力ファイルパス
// 返値:	VSurface*		読込まれたサーフェスへのポインタ
//							(問答無用で32bit化)
/////////////////////////////////////////////////////////////////////////////
VSurface* LoadImg( const P6VPATH& filepath )
{
	PRINTD( GRP_LOG, "[COMMON][LoadImg] <- %s\n", P6VPATH2STR( filepath ).c_str() );

	FILE* fp           = nullptr;
	png_structp PngPtr = nullptr;
	png_infop InfoPtr  = nullptr;
	png_infop EndInfo  = nullptr;
	png_uint_32 width, height;
	png_byte BitDepth, ColorType, Channels;
	png_bytep* RowPtrs = nullptr;
	VSurface* sur      = nullptr;


	// 各構造体を確保・初期化する
	if( !(PngPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)) ){
		return nullptr;
	}

	if( !(InfoPtr = png_create_info_struct( PngPtr )) ){
		png_destroy_read_struct( &PngPtr, nullptr, nullptr );
		return nullptr;
	}

	if( !(EndInfo = png_create_info_struct( PngPtr )) ){
		png_destroy_read_struct( &PngPtr, &InfoPtr, nullptr );
		return nullptr;
	}

	// エラー処理
	if( setjmp( png_jmpbuf( PngPtr ) ) ){
		if( fp ){ fclose( fp ); }
		png_destroy_read_struct( &PngPtr, &InfoPtr, &EndInfo );
		if( sur ){ delete sur; }
		return nullptr;
	}

	// 画像ファイルを開く
	if( !(fp = OSD_Fopen( filepath, "rb" )) ){
		png_destroy_read_struct( &PngPtr, &InfoPtr, &EndInfo );
		return nullptr;
	}

	// libpngにfpを知らせる
	png_init_io( PngPtr, fp );

	// 画像データを読込む
	png_read_png( PngPtr, InfoPtr, PNG_TRANSFORM_IDENTITY, nullptr );

	// 各種情報抽出
	RowPtrs   = png_get_rows( PngPtr, InfoPtr );
	width     = png_get_image_width( PngPtr, InfoPtr );
	height    = png_get_image_height( PngPtr, InfoPtr );
	BitDepth  = png_get_bit_depth( PngPtr, InfoPtr );
	ColorType = png_get_color_type( PngPtr, InfoPtr );
	Channels  = png_get_channels( PngPtr, InfoPtr );


	// サーフェスを作成
	sur = new VSurface;
	sur->InitSurface( width, height );

	// 画像データを取得
	BYTE* doff  = (BYTE*)sur->GetPixels().data();

	if( Channels == 1 && ( ColorType == PNG_COLOR_TYPE_GRAY || ColorType == PNG_COLOR_TYPE_PALETTE ) &&
		( BitDepth == 1 || BitDepth == 8 ) ){
		// GRAY, PALETTE
		int num_palette;
		png_colorp palette;

		// パレットがあれば読込む
		if( ColorType == PNG_COLOR_TYPE_PALETTE )
			png_get_PLTE( PngPtr, InfoPtr, &palette, &num_palette );

		for( int i = 0; i < (int)height; i++){
			png_bytep rpt = RowPtrs[i];
			for( int j = 0; j < (int)width; j++ ){
				doff[j] = BitDepth == 1 ? (rpt[j / 8] & (0x80 >> (j & 7))) ? 15 : 0
										: rpt[j];
			}
			doff += sur->Pitch();
		}
	}else if( Channels != 1 && ( ColorType == PNG_COLOR_TYPE_RGB_ALPHA || ColorType == PNG_COLOR_TYPE_RGB ) &&
				BitDepth == 8 ){
		// GRAY_ALPHA, RGB, RGB_ALPHA, RGB+x
		for( int i = 0; i < (int)height; i++ ){
//			png_bytep rpt = RowPtrs[i];
			doff += sur->Pitch();	// このへん適当。あとで考える。
		}
	}else{
		delete sur;
		sur = nullptr;
	}

	// 画像ファイルを閉じる
	fclose( fp );

	// 各構造体を開放する
	png_destroy_read_struct( &PngPtr, &InfoPtr, &EndInfo );

	return sur;
}
#endif //#if 0

/////////////////////////////////////////////////////////////////////////////
// 文字列取得関数等
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// メッセージテーブル
/////////////////////////////////////////////////////////////////////////////
static const std::map<TextID, const std::string> MsgString =
{
	{ T_EMPTY,					"???" },

	// 一般メッセージ
	{ T_QUIT,					N_( "終了してよろしいですか?" ) },
	{ T_QUITC,					N_( "終了確認" ) },
	{ T_RESTART0,				N_( "再起動してよろしいですか?" ) },
	{ T_RESTART,				N_( "変更は再起動後に有効となります。\n今すぐ再起動しますか?" ) },
	{ T_RESTARTC,				N_( "再起動確認" ) },
	{ T_RESTARTI,				N_( "拡張カートリッジを挿入して再起動します。" ) },
	{ T_RESTARTE,				N_( "拡張カートリッジを排出して再起動します。" ) },
	{ T_RESETC,					N_( "リセット確認" ) },
	{ T_DOKOC,					N_( "どこでもLOAD確認" ) },
	{ T_DOKOSLOT,				N_( "どこでもLOADを実行してよろしいですか？" ) },
	{ T_REPLAYRES,				N_( "リプレイを途中保存地点まで巻き戻します\nよろしいですか？" ) },

	// INIファイル用メッセージ ------
	{ TINI_TITLE,				"; === PC6001V " N_( "初期設定ファイル" ) " ===\n\n" },
	// [CONFIG]
	{ TINI_Model,				N_( "機種" ) " 60:PC-6001 61:PC-6001A 62:PC-6001mkⅡ 66:PC-6601 64:PC-6001mkⅡSR 68:PC-6601SR" },
	{ TINI_OverClock,			N_( "オーバークロック率" ) " (1-1000)%" },
	{ TINI_CheckCRC,			N_( "CRCチェック" ) " "		N_( "Yes:有効 No:無効" ) },
	{ TINI_Romaji,				N_( "ローマ字入力" ) " "	N_( "Yes:有効 No:無効") },
	{ TINI_RomajiWait,			N_( "ローマ字入力ウェイト" ) " (0-60)" },

	// [CMT]
	{ TINI_TurboTAPE,			N_( "Turbo TAPE" ) " "		N_( "Yes:有効 No:無効" ) },
	{ TINI_BoostUp,				N_( "Boost Up" ) " "		N_( "Yes:有効 No:無効" ) },
	{ TINI_MaxBoost60,			N_( "Boost Up" ) " "		N_( "最大倍率(N60モード)" ) },
	{ TINI_MaxBoost62,			N_( "Boost Up" ) " "		N_( "最大倍率(N60m/N66モード)" ) },
	{ TINI_StopBit,				N_( "ストップビット数" ) " (2-10)bit" },
	// [FDD]
	{ TINI_FDDrive,				N_( "ドライブ数" ) " (0-2)" },
	{ TINI_FDDWait,				N_( "アクセスウェイト" ) " "	N_( "Yes:有効 No:無効" ) },
	// [DISPLAY]
	{ TINI_Mode4Color,			N_( "MODE4カラーモード 0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク" ) },
	{ TINI_WindowZoom,			N_( "ウィンドウ表示倍率" ) "(%)" },
	{ TINI_FrameSkip,			N_( "フレームスキップ" ) },
	{ TINI_ScanLine,			N_( "スキャンライン" ) " "	N_( "Yes:あり No:なし" ) },
	{ TINI_ScanLineBr,			N_( "スキャンライン輝度" ) " (0-100)%" },
	{ TINI_DispNTSC,			N_( "4:3表示" ) " "					N_( "Yes:有効 No:無効" ) },
	{ TINI_Filtering,			N_( "フィルタリング" ) " "			N_( "Yes:アンチエイリアシング No:ニアレストネイバー" ) },
	{ TINI_FullScreen,			N_( "フルスクリーンモード" ) " "	N_( "Yes:有効 No:無効" ) },
	{ TINI_DispStatus,			N_( "ステータスバー" ) " "			N_( "Yes:表示 No:非表示" ) },
	// [MOVIE]
	{ TINI_AviBpp,				N_( "ビデオキャプチャ時" ) N_( "色深度" ) " (16,24,32)bit" },
	{ TINI_AviZoom,				N_( "ビデオキャプチャ時" ) N_( "ウィンドウ表示倍率" ) "(%)" },
	{ TINI_AviFrameSkip,		N_( "ビデオキャプチャ時" ) N_( "フレームスキップ" ) },
	{ TINI_AviScanLine,			N_( "ビデオキャプチャ時" ) N_( "スキャンライン" ) " "	N_( "Yes:あり No:なし" ) },
	{ TINI_AviScanLineBr,		N_( "ビデオキャプチャ時" ) N_( "スキャンライン輝度" ) " (0-100)%" },
	{ TINI_AviDispNTSC,			N_( "ビデオキャプチャ時" ) N_( "4:3表示" ) " "			N_( "Yes:有効 No:無効" ) },
	{ TINI_AviFiltering,		N_( "ビデオキャプチャ時" ) N_( "フィルタリング" ) " "	N_( "Yes:アンチエイリアシング No:ニアレストネイバー" ) },
	// [SOUND]
	{ TINI_SampleRate,			N_( "サンプリングレート" ) " (44100/22050/11025)Hz" },
	{ TINI_SoundBuffer,			N_( "サウンドバッファサイズ" ) },
	{ TINI_MasterVolume,		N_( "マスター音量" ) " (0-100)" },
	{ TINI_PsgVolume,			N_( "PSG音量" ) " (0-100)" },
	{ TINI_PsgLPF,				N_( "PSG LPFカットオフ周波数(0で無効)" ) },
	{ TINI_VoiceVolume,			N_( "音声合成音量" ) " (0-100)" },
	{ TINI_TapeVolume,			N_( "TAPEモニタ音量" ) " (0-100)" },
	{ TINI_TapeLPF,				N_( "TAPE LPFカットオフ周波数(0で無効)" ) },
	// [FILES]
	{ TINI_ExtRom,				N_( "拡張ROMファイル名" ) },
	{ TINI_tape,				N_( "TAPE(LODE)ファイル名(起動時に自動マウント)" ) },
	{ TINI_save,				N_( "TAPE(SAVE)ファイル名(SAVE時に自動マウント)" ) },
	{ TINI_disk1,				N_( "DISK1ファイル名(起動時に自動マウント)" ) },
	{ TINI_disk2,				N_( "DISK2ファイル名(起動時に自動マウント)" ) },
	{ TINI_printer,				N_( "プリンタ出力ファイル名" ) },
	// [PATH]
	{ TINI_RomPath,				N_( "ROMイメージ" )			N_( "ファイル格納パス" ) },
	{ TINI_TapePath,			N_( "TAPEイメージ" )		N_( "ファイル格納パス" ) },
	{ TINI_DiskPath,			N_( "DISKイメージ" )		N_( "ファイル格納パス" ) },
	{ TINI_ExtRomPath,			N_( "拡張ROMイメージ" )		N_( "ファイル格納パス" ) },
	{ TINI_ImgPath,				N_( "スナップショット" )	N_( "ファイル格納パス" ) },
	{ TINI_WavePath,			N_( "WAVE" )				N_( "ファイル格納パス" ) },
	{ TINI_FontPath,			N_( "FONT" )				N_( "ファイル格納パス" ) },
	{ TINI_DokoPath,			N_( "どこでもSAVE" )		N_( "ファイル格納パス" ) },
	// [CHECK]
	{ TINI_CkDokoLoad,			N_( "どこでもLOAD(SLOT)実行時確認" ) " "	N_( "Yes:する No:しない" ) },
	{ TINI_CkQuit,				N_( "終了時確認" ) " "						N_( "Yes:する No:しない" ) },
	{ TINI_SaveQuit,			N_( "終了時INIファイルを保存" ) " "			N_( "Yes:する No:しない" ) },
	// [OPTION]
	{ TINI_ExCartridge,			N_( "拡張カートリッジ 0:なし" ) },

	// どこでもSAVE用メッセージ ------
	{ TDOK_TITLE,				"; === PC6001V" N_( "どこでもSAVEファイル" ) " ===\n\n" },

	// Error用メッセージ ------
	{ TERR_ERROR,				N_( "Error" ) },
	{ TERR_WARNING,				N_( "Warning" ) },
	{ TERR_NoError,				N_( "エラーはありません" ) },
	{ TERR_Unknown,				N_( "原因不明のエラーが発生しました" ) },
	{ TERR_MemAllocFailed,		N_( "メモリの確保に失敗しました" ) },
	{ TERR_RomChange,			N_( "指定された機種のROMイメージが見つからないため機種を変更しました" ) },
	{ TERR_NoRom,				N_( "ROMイメージが見つかりません\n設定とファイル名を確認してください" ) },
	{ TERR_NoRomChange,			N_( "ROMイメージが見つかりません\n他の機種を検索しますか？" ) },
	{ TERR_RomSizeNG,			N_( "ROMイメージのサイズが不正です" ) },
	{ TERR_RomCrcNG,			N_( "ROMイメージのCRCが不正です\nCRCが一致しないROMを使用すると、予期せぬ不具合を引き起こす可能性があります。\nそれでも起動しますか？" ) },
	{ TERR_LibInitFailed,		N_( "ライブラリの初期化に失敗しました" ) },
	{ TERR_InitFailed,			N_( "初期化に失敗しました\n設定を確認してください" ) },
	{ TERR_FontLoadFailed,		N_( "フォントの読込みに失敗しました" ) },
	{ TERR_FontCreateFailed,	N_( "フォントファイルの作成に失敗しました" ) },
	{ TERR_IniDefault,			N_( "INIファイルの読込みに失敗しました\nデフォルト設定で起動します" ) },
	{ TERR_IniReadFailed,		N_( "INIファイルの読込みに失敗しました" ) },
	{ TERR_IniWriteFailed,		N_( "INIファイルの保存に失敗しました" ) },
	{ TERR_TapeMountFailed,		N_( "TAPEイメージのマウントに失敗しました" ) },
	{ TERR_DiskMountFailed,		N_( "DISKイメージのマウントに失敗しました" ) },
	{ TERR_ExtRomMountFailed,	N_( "拡張ROMイメージのマウントに失敗しました" ) },
	{ TERR_DokoReadFailed,		N_( "どこでもLOADに失敗しました" ) },
	{ TERR_DokoWriteFailed,		N_( "どこでもSAVEに失敗しました" ) },
	{ TERR_DokoDiffVersion,		N_( "どこでもSAVEファイルのバージョンが異なるため\n正しく動作しない可能性があります\n続けますか？" ) },
	{ TERR_ReplayPlayError,		N_( "リプレイ再生に失敗しました" ) },
	{ TERR_ReplayRecError,		N_( "リプレイ記録に失敗しました" ) },
	{ TERR_NoReplayData,		N_( "リプレイデータがありません" ) },
	{ TERR_CaptureFailed,		N_( "ビデオキャプチャに失敗しました" ) }
};


/////////////////////////////////////////////////////////////////////////////
// 色の名前テーブル
/////////////////////////////////////////////////////////////////////////////
static const std::vector<std::string> JColorName =
{
	"MODE1,2 "		N_( "黒(ボーダー)" ),
	"MODE1 Set1 "	N_( "緑" ),
	"MODE1 Set1 "	N_( "深緑" ),
	"MODE1 Set2 "	N_( "橙" ),
	"MODE1 Set2 "	N_( "深橙" ),
	"MODE2/3 "		N_( "緑" ),
	"MODE2/3 "		N_( "黄" ),
	"MODE2/3 "		N_( "青" ),
	"MODE2/3 "		N_( "赤" ),
	"MODE2/3 "		N_( "白" ),
	"MODE2/3 "		N_( "シアン" ),
	"MODE2/3 "		N_( "マゼンタ" ),
	"MODE2/3 "		N_( "橙" ),
	"MODE4 Set1 "	N_( "深緑" ),
	"MODE4 Set1 "	N_( "緑" ),
	"MODE4 Set2 "	N_( "黒" ),
	"MODE4 Set2 "	N_( "白" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "赤" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "青" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "桃" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "緑" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "明赤" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "暗赤" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "明青" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "暗青" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "明桃" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "暗桃" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "明緑" ),
	"MODE4 Set1 "	N_( "にじみ" ) " " N_( "暗緑" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "赤" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "青" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "桃" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "緑" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "明赤" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "暗赤" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "明青" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "暗青" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "明桃" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "暗桃" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "明緑" ),
	"MODE4 Set2 "	N_( "にじみ" ) " " N_( "暗緑" ),
	"RGB "			N_( "透明(黒)" ),
	"RGB "			N_( "橙" ),
	"RGB "			N_( "青緑" ),
	"RGB "			N_( "黄緑" ),
	"RGB "			N_( "青紫" ),
	"RGB "			N_( "赤紫" ),
	"RGB "			N_( "空色" ),
	"RGB "			N_( "灰色" ),
	"RGB "			N_( "黒" ),
	"RGB "			N_( "赤" ),
	"RGB "			N_( "緑" ),
	"RGB "			N_( "黄" ),
	"RGB "			N_( "青" ),
	"RGB "			N_( "マゼンタ" ),
	"RGB "			N_( "シアン" ),
	"RGB "			N_( "白" )
};


/////////////////////////////////////////////////////////////////////////////
// 拡張カートリッジ名テーブル
/////////////////////////////////////////////////////////////////////////////
static const std::map<WORD, const std::string> CartName =
{
	{ 0,			"" },
	{ EXC6001,		"PCS-6001R "	N_( "拡張BASIC" ) },
	{ EXC6005,		"PC-6005 "		N_( "ROMカートリッジ" ) },
	{ EXC6006,		"PC-6006 "		N_( "拡張ROM/RAMカートリッジ" ) },
	{ EXC660101,	"PC-6601-01 "	N_( "拡張漢字ROMカートリッジ" ) },
	{ EXC6006SR,	"PC-6006SR "	N_( "拡張64KRAMカートリッジ" ) },
	{ EXC6007SR,	"PC-6007SR "	N_( "拡張漢字ROM&RAMカートリッジ" ) },
	{ EXC6053,		"PC-6053 "		N_( "ボイスシンセサイザー" ) },
	{ EXC60M55,		"PC-60m55 "		N_( "FM音源カートリッジ" ) },
	{ EXCSOL1,		N_( "戦士のカートリッジ" ) },
	{ EXCSOL2,		N_( "戦士のカートリッジmkⅡ" ) },
	{ EXCSOL3,		N_( "戦士のカートリッジmkⅢ" ) }
};


/////////////////////////////////////////////////////////////////////////////
// 仮想キーコード -> 名称 変換テーブル
/////////////////////////////////////////////////////////////////////////////
static const std::map<PCKEYsym, const std::string> VKname =
{
	{ KVC_UNKNOWN,		"Unknown" },

	{ KVC_1,			"1" },
	{ KVC_2,			"2" },
	{ KVC_3,			"3" },
	{ KVC_4,			"4" },
	{ KVC_5,			"5" },
	{ KVC_6,			"6" },
	{ KVC_7,			"7" },
	{ KVC_8,			"8" },
	{ KVC_9,			"9" },
	{ KVC_0,			"0" },

	{ KVC_A,			"A" },
	{ KVC_B,			"B" },
	{ KVC_C,			"C" },
	{ KVC_D,			"D" },
	{ KVC_E,			"E" },
	{ KVC_F,			"F" },
	{ KVC_G,			"G" },
	{ KVC_H,			"H" },
	{ KVC_I,			"I" },
	{ KVC_J,			"J" },
	{ KVC_K,			"K" },
	{ KVC_L,			"L" },
	{ KVC_M,			"M" },
	{ KVC_N,			"N" },
	{ KVC_O,			"O" },
	{ KVC_P,			"P" },
	{ KVC_Q,			"Q" },
	{ KVC_R,			"R" },
	{ KVC_S,			"S" },
	{ KVC_T,			"T" },
	{ KVC_U,			"U" },
	{ KVC_V,			"V" },
	{ KVC_W,			"W" },
	{ KVC_X,			"X" },
	{ KVC_Y,			"Y" },
	{ KVC_Z,			"Z" },

	{ KVC_F1,			"F1" },
	{ KVC_F2,			"F2" },
	{ KVC_F3,			"F3" },
	{ KVC_F4,			"F4" },
	{ KVC_F5,			"F5" },
	{ KVC_F6,			"F6" },
	{ KVC_F7,			"F7" },
	{ KVC_F8,			"F8" },
	{ KVC_F9,			"F9" },
	{ KVC_F10,			"F10" },
	{ KVC_F11,			"F11" },
	{ KVC_F12,			"F12" },

	{ KVC_MINUS,		"-" },
	{ KVC_CARET,		"^" },
	{ KVC_BACKSPACE,	"BackSpace" },
	{ KVC_AT,			"@" },
	{ KVC_LBRACKET,		"[" },
	{ KVC_SEMICOLON,	";" },
	{ KVC_COLON,		":" },
	{ KVC_COMMA,		"," },
	{ KVC_PERIOD,		"." },
	{ KVC_SLASH,		"/" },
	{ KVC_SPACE,		"Space" },

	{ KVC_ESC,			"ESC" },
	{ KVC_HANZEN,		N_( "半角/全角" ) },
	{ KVC_TAB,			"Tab" },
	{ KVC_CAPSLOCK,		"CapsLock" },
	{ KVC_ENTER,		"Enter" },
	{ KVC_LCTRL,		"L-Ctrl" },
	{ KVC_RCTRL,		"R-Ctrl" },
	{ KVC_LSHIFT,		"L-Shift" },
	{ KVC_RSHIFT,		"R-Shift" },
	{ KVC_LALT,			"L-Alt" },
	{ KVC_RALT,			"R-Alt" },
	{ KVC_PRINT,		"PrintScreen" },
	{ KVC_SCROLLLOCK,	"ScrollLock" },
	{ KVC_PAUSE,		"Pause" },
	{ KVC_INSERT,		"Insert" },
	{ KVC_DELETE,		"Delete" },
	{ KVC_END,			"End" },
	{ KVC_HOME,			"Home" },
	{ KVC_PAGEUP,		"PageUp" },
	{ KVC_PAGEDOWN,		"PageDown" },

	{ KVC_UP,			"↑" },
	{ KVC_DOWN,			"↓" },
	{ KVC_LEFT,			"←" },
	{ KVC_RIGHT,		"→" },

	{ KVC_P0,			"0(Numeric key)" },
	{ KVC_P1,			"1(Numeric key)" },
	{ KVC_P2,			"2(Numeric key)" },
	{ KVC_P3,			"3(Numeric key)" },
	{ KVC_P4,			"4(Numeric key)" },
	{ KVC_P5,			"5(Numeric key)" },
	{ KVC_P6,			"6(Numeric key)" },
	{ KVC_P7,			"7(Numeric key)" },
	{ KVC_P8,			"8(Numeric key)" },
	{ KVC_P9,			"9(Numeric key)" },
	{ KVC_NUMLOCK,		"NumLock" },
	{ KVC_P_PLUS,		"+(Numeric key)" },
	{ KVC_P_MINUS,		"-(Numeric key)" },
	{ KVC_P_MULTIPLY,	"*(Numeric key)" },
	{ KVC_P_DIVIDE,		"/(Numeric key)" },
	{ KVC_P_PERIOD,		".(Numeric key)" },
	{ KVC_P_ENTER,		"Enter(Numeric key)" },

	// 日本語キーボードのみ
	{ KVC_YEN,			"\\" },
	{ KVC_RBRACKET,		"]" },
	{ KVC_UNDERSCORE,	"_" },
	{ KVC_MUHENKAN,		N_( "無変換" ) },
	{ KVC_HENKAN,		N_( "変換" ) },
	{ KVC_HIRAGANA,		N_( "カタカナ/ひらがな" ) },

	// 英語キーボードのみ
	{ KVE_BACKSLASH,	"BackSlash" },

	// 追加キー
	{ KVX_RMETA,		"L-Windows" },
	{ KVX_LMETA,		"R-Windows" },
	{ KVX_MENU,			"Menu" }
};


/////////////////////////////////////////////////////////////////////////////
// 仮想キーコード  -> 文字コード 変換テーブル
/////////////////////////////////////////////////////////////////////////////
static const std::map<PCKEYsym, const std::vector<BYTE>> VKChar =
{
	{ KVC_UNKNOWN,		{ 0,	0    } },

	{ KVC_1,			{ '1',	'!'  } },
	{ KVC_2,			{ '2',	'\"' } },
	{ KVC_3,			{ '3',	'#'  } },
	{ KVC_4,			{ '4',	'$'  } },
	{ KVC_5,			{ '5',	'%'  } },
	{ KVC_6,			{ '6',	'&'  } },
	{ KVC_7,			{ '7',	'\'' } },
	{ KVC_8,			{ '8',	'('  } },
	{ KVC_9,			{ '9',	')'  } },
	{ KVC_0,			{ '0',	0    } },

	{ KVC_A,			{ 'a',	'A'  } },
	{ KVC_B,			{ 'b',	'B'  } },
	{ KVC_C,			{ 'c',	'C'  } },
	{ KVC_D,			{ 'd',	'D'  } },
	{ KVC_E,			{ 'e',	'E'  } },
	{ KVC_F,			{ 'f',	'F'  } },
	{ KVC_G,			{ 'g',	'G'  } },
	{ KVC_H,			{ 'h',	'H'  } },
	{ KVC_I,			{ 'i',	'I'  } },
	{ KVC_J,			{ 'j',	'J'  } },
	{ KVC_K,			{ 'k',	'K'  } },
	{ KVC_L,			{ 'l',	'L'  } },
	{ KVC_M,			{ 'm',	'M'  } },
	{ KVC_N,			{ 'n',	'N'  } },
	{ KVC_O,			{ 'o',	'O'  } },
	{ KVC_P,			{ 'p',	'P'  } },
	{ KVC_Q,			{ 'q',	'Q'  } },
	{ KVC_R,			{ 'r',	'R'  } },
	{ KVC_S,			{ 's',	'S'  } },
	{ KVC_T,			{ 't',	'T'  } },
	{ KVC_U,			{ 'u',	'U'  } },
	{ KVC_V,			{ 'v',	'V'  } },
	{ KVC_W,			{ 'w',	'W'  } },
	{ KVC_X,			{ 'x',	'X'  } },
	{ KVC_Y,			{ 'y',	'Y'  } },
	{ KVC_Z,			{ 'z',	'Z'  } },

	{ KVC_F1,			{ 0,	0    } },	// F1
	{ KVC_F2,			{ 0,	0    } },	// F2
	{ KVC_F3,			{ 0,	0    } },	// F3
	{ KVC_F4,			{ 0,	0    } },	// F4
	{ KVC_F5,			{ 0,	0    } },	// F5
	{ KVC_F6,			{ 0,	0    } },	// F6
	{ KVC_F7,			{ 0,	0    } },	// F7
	{ KVC_F8,			{ 0,	0    } },	// F8
	{ KVC_F9,			{ 0,	0    } },	// F9
	{ KVC_F10,			{ 0,	0    } },	// F10
	{ KVC_F11,			{ 0,	0    } },	// F11
	{ KVC_F12,			{ 0,	0    } },	// F12

	{ KVC_MINUS,		{ '-',	'='  } },
	{ KVC_CARET,		{ '^',	'~'  } },
	{ KVC_AT,			{ '@',	'`'  } },
	{ KVC_LBRACKET,		{ '[',	'{'  } },
	{ KVC_RBRACKET,		{ ']',	'}'  } },
	{ KVC_SEMICOLON,	{ ';',	'+'  } },
	{ KVC_COLON,		{ ':',	'*'  } },
	{ KVC_COMMA,		{ ',',	'<'  } },
	{ KVC_PERIOD,		{ '.',	'>'  } },
	{ KVC_SLASH,		{ '/',	'?'  } },
	{ KVC_SPACE,		{ ' ',	' '  } },

	{ KVC_BACKSPACE,	{ 0,	0    } },	// BackSpace
	{ KVC_ESC,			{ 0,	0    } },	// ESC
	{ KVC_TAB,			{ 0,	0    } },	// Tab
	{ KVC_CAPSLOCK,		{ 0,	0    } },	// CapsLock
	{ KVC_ENTER,		{ 0,	0    } },	// Enter
	{ KVC_LCTRL,		{ 0,	0    } },	// L-Ctrl
	{ KVC_RCTRL,		{ 0,	0    } },	// R-Ctrl
	{ KVC_LSHIFT,		{ 0,	0    } },	// L-Shift
	{ KVC_RSHIFT,		{ 0,	0    } },	// R-Shift
	{ KVC_LALT,			{ 0,	0    } },	// L-Alt
	{ KVC_RALT,			{ 0,	0    } },	// R-Alt
	{ KVC_PRINT,		{ 0,	0    } },	// PrintScreen
	{ KVC_SCROLLLOCK,	{ 0,	0    } },	// ScrollLock
	{ KVC_PAUSE,		{ 0,	0    } },	// Pause
	{ KVC_INSERT,		{ 0,	0    } },	// Insert
	{ KVC_DELETE,		{ 0,	0    } },	// Delete
	{ KVC_END,			{ 0,	0    } },	// End
	{ KVC_HOME,			{ 0,	0    } },	// Home
	{ KVC_PAGEUP,		{ 0,	0    } },	// PageUp
	{ KVC_PAGEDOWN,		{ 0,	0    } },	// PageDown

	{ KVC_UP,			{ 0,	0    } },	// ↑
	{ KVC_DOWN,			{ 0,	0    } },	// ↓
	{ KVC_LEFT,			{ 0,	0    } },	// ←
	{ KVC_RIGHT,		{ 0,	0    } },	// →

	{ KVC_P0,			{ '0',	0    } },	// [0]
	{ KVC_P1,			{ '1',	0    } },	// [1]
	{ KVC_P2,			{ '2',	0    } },	// [2]
	{ KVC_P3,			{ '3',	0    } },	// [3]
	{ KVC_P4,			{ '4',	0    } },	// [4]
	{ KVC_P5,			{ '5',	0    } },	// [5]
	{ KVC_P6,			{ '6',	0    } },	// [6]
	{ KVC_P7,			{ '7',	0    } },	// [7]
	{ KVC_P8,			{ '8',	0    } },	// [8]
	{ KVC_P9,			{ '9',	0    } },	// [9]
	{ KVC_P_PLUS,		{ '+',	'+'  } },	// [+]
	{ KVC_P_MINUS,		{ '-',	'-'  } },	// [-]
	{ KVC_P_MULTIPLY,	{ '*',	'*'  } },	// [*]
	{ KVC_P_DIVIDE,		{ '/',	'/'  } },	// [/]
	{ KVC_P_PERIOD,		{ '.',	'.'  } },	// [.]
	{ KVC_NUMLOCK,		{ 0,	0    } },	// NumLock
	{ KVC_P_ENTER,		{ 0,	0    } },	// [Enter]

	//
	{ KVC_UNDERSCORE,	{ '\\',	'_'  } },
	{ KVC_YEN,			{ '\\',	'|'  } },
	{ KVC_HIRAGANA,		{ 0,	0    } },	// カタカナ/ひらがな
	{ KVC_HENKAN,		{ 0,	0    } },	// 変換
	{ KVC_MUHENKAN,		{ 0,	0    } },	// 無変換
	{ KVC_HANZEN,		{ 0,	0    } },	// 半角/全角

	//
	{ KVX_LMETA,		{ 0,	0    } },	// L-Meta
	{ KVX_RMETA,		{ 0,	0    } },	// R-Meta
	{ KVX_MENU,			{ 0,	0    } }	// Menu
};


/////////////////////////////////////////////////////////////////////////////
// メッセージ文字列取得
//
// 引数:	id				メッセージID
// 返値:	std::string		メッセージ文字列
/////////////////////////////////////////////////////////////////////////////
const std::string GetText( TextID id )
{
	try{
		return _( MsgString.at( id ).c_str() );
	}
	catch( std::out_of_range& ){
		return _( MsgString.at( T_EMPTY ).c_str() );	// "???"
	}
}


/////////////////////////////////////////////////////////////////////////////
// 色の名前取得
//
// 引数:	num				色コード
// 返値:	std::string		色の名前文字列
/////////////////////////////////////////////////////////////////////////////
const std::string GetColorName( int num )
{
	try{
		return _( JColorName.at( num ).c_str() );
	}
	catch( std::out_of_range& ){
		return _( MsgString.at( T_EMPTY ).c_str() );	// "???"
	}
}


/////////////////////////////////////////////////////////////////////////////
// 拡張カートリッジの名前取得
//
// 引数:	num				カートリッジ番号
// 返値:	std::string&	カートリッジ名への参照
/////////////////////////////////////////////////////////////////////////////
const std::string GetExtCartName( int num )
{
	try{
		return _( CartName.at( num ).c_str() );
	}
	catch( std::out_of_range& ){
		return _( CartName.at( 0 ).c_str() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// キーの名前取得
//
// 引数:	sym				仮想キーコード
// 返値:	std::string		キー名前文字列
/////////////////////////////////////////////////////////////////////////////
const std::string GetKeyName( PCKEYsym sym )
{
	try{
		return _( VKname.at( sym ).c_str() );
	}
	catch( std::out_of_range& ){
		return _( VKname.at( KVC_UNKNOWN ).c_str() );
	}
}


/////////////////////////////////////////////////////////////////////////////
// キーの文字コード取得
//
// 引数:	sym				仮想キーコード
//			shift			シフトキーの状態 true:押されている false:押されていない
// 返値:	BYTE			文字コード
/////////////////////////////////////////////////////////////////////////////
BYTE GetKeyChar( PCKEYsym sym, bool shift )
{
	try{
		return VKChar.at( sym ).at( shift ? 1 : 0 );
	}
	catch( std::out_of_range& ){
		return VKChar.at( KVC_UNKNOWN )[0];
	}
}
