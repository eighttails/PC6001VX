/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include "common.h"
#include "config.h"
#include "log.h"
#include "keyboard.h"
#include "osd.h"
#include "p6vm.h"


#define	NOM	( (int)P6Matrix.size() / 2 )


// P6キーコード -> P6キーマトリクス定義
static const std::vector<P6KeyMatrix> P6KeyMx = {
	
	{ KP6_UNKNOWN,		0x00 },	// 不明キー
	
	// Y0
	{ KP6_CTRL,			0x01 },	// CTRL
	{ KP6_SHIFT,		0x02 },	// SHIFT
	{ KP6_GRAPH,		0x03 },	// GRAPH
	
	// Y1
	{ KP6_1,			0x10 },	// 1	!
	{ KP6_Q,			0x11 },	// q	Q
	{ KP6_A,			0x12 },	// a	A
	{ KP6_Z,			0x13 },	// z	Z
	{ KP6_K,			0x14 },	// k	K
	{ KP6_I,			0x15 },	// i	I
	{ KP6_8,			0x16 },	// 8	(
	{ KP6_COMMA,		0x17 },	// ,	<
	
	// Y2
	{ KP6_2,			0x20 },	// 2	"
	{ KP6_W,			0x21 },	// w	W
	{ KP6_S,			0x22 },	// s	S
	{ KP6_X,			0x23 },	// x	X
	{ KP6_L,			0x24 },	// l	L
	{ KP6_O,			0x25 },	// o	O
	{ KP6_9,			0x26 },	// 9	)
	{ KP6_PERIOD,		0x27 },	// .	>
	
	// Y3
	{ KP6_3,			0x30 },	// 3	#
	{ KP6_E,			0x31 },	// e	E
	{ KP6_D,			0x32 },	// d	D
	{ KP6_C,			0x33 },	// c	C
	{ KP6_SEMICOLON,	0x34 },	// ;	+
	{ KP6_P,			0x35 },	// p	P
	{ KP6_F1,			0x36 },	// F1	F6
	{ KP6_SLASH,		0x37 },	// /	?
	
	// Y4
	{ KP6_4,			0x40 },	// 4	$
	{ KP6_R,			0x41 },	// r	R
	{ KP6_F,			0x42 },	// f	F
	{ KP6_V,			0x43 },	// v	V
	{ KP6_COLON,		0x44 },	// :	*
	{ KP6_AT,			0x45 },	// @
	{ KP6_F2,			0x46 },	// F2	F7
	{ KP6_UNDERSCORE,	0x47 },	// 		_
	
	// Y5
	{ KP6_5,			0x50 },	// 5	%
	{ KP6_T,			0x51 },	// t	T
	{ KP6_G,			0x52 },	// g	G
	{ KP6_B,			0x53 },	// b	B
	{ KP6_RBRACKET,		0x54 },	// ]
	{ KP6_LBRACKET,		0x55 },	// [
	{ KP6_F3,			0x56 },	// F3	F8
	{ KP6_SPACE,		0x57 },	// SPC
	
	// Y6
	{ KP6_6,			0x60 },	// 6	&
	{ KP6_Y,			0x61 },	// y	Y
	{ KP6_H,			0x62 },	// h	H
	{ KP6_N,			0x63 },	// n	N
	{ KP6_MINUS,		0x64 },	// -	=
	{ KP6_CARET,		0x65 },	// ^
	{ KP6_F4,			0x66 },	// F4	F9
	{ KP6_0,			0x67 },	// 0
	
	// Y7
	{ KP6_7,			0x70 },	// 7	'
	{ KP6_U,			0x71 },	// u	U
	{ KP6_J,			0x72 },	// j	J
	{ KP6_M,			0x73 },	// m	M
	{ KP6_YEN,			0x75 },	// ￥
	{ KP6_F5,			0x76 },	// F5	F10
	
	// Y8
	{ KP6_RETURN,		0x80 },	// RET
	{ KP6_STOP,			0x81 },	// STOP
	{ KP6_UP,			0x82 },	// ↑
	{ KP6_DOWN,			0x83 },	// ↓
	{ KP6_RIGHT,		0x84 },	// →
	{ KP6_LEFT,			0x85 },	// ←
	{ KP6_TAB,			0x86 },	// TAB
	{ KP6_ESC,			0x87 },	// ESC
	
	// Y9
	{ KP6_KANA,			0x90 },	// かな
	{ KP6_INS,			0x91 },	// INS
	{ KP6_DEL,			0x92 },	// DEL
	{ KP6_PAGE,			0x93 },	// PAGE
	{ KP6_HOME,			0x94 },	// HOME
	{ KP6_MODE,			0x95 },	// MODE
	{ KP6_CAPS,			0x96 },	// CAPS
	
	// これ以降は独自拡張
	// YA
	{ KP6_P0,			0xa0 },	// [0]
	{ KP6_P1,			0xa1 },	// [1]
	{ KP6_P2,			0xa2 },	// [2]
	{ KP6_P3,			0xa3 },	// [3]
	{ KP6_P4,			0xa4 },	// [4]
	{ KP6_P5,			0xa5 },	// [5]
	{ KP6_P6,			0xa6 },	// [6]
	{ KP6_P7,			0xa7 },	// [7]
	
	// YB
	{ KP6_P8,			0xb0 },	// [8]
	{ KP6_P9,			0xb1 },	// [9]
	{ KP6_PPERIOD,		0xb2 },	// [.]
	{ KP6_PPLUS,		0xb3 },	// [+]
	{ KP6_PMINUS,		0xb4 },	// [-]
	{ KP6_PMULTIPLY,	0xb5 },	// [*]
	{ KP6_PDIVIDE,		0xb6 },	// [/]
	{ KP6_PRETURN,		0xb7 },	// [RETURN]
};


// P6キーマトリクスマップ定義
// (日本語キーボードの場合)
static const struct {
	BYTE Keys[8][8];	// キーマトリクスマップ
} P6MxCode[] = {
	
	//	  STD   STD+S GRAPH かな かな+S カナ カナ+S CTRL
	//	--------------------------------------------------
	// Y0
	{ {	{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	// CTRL
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	// SHIFT
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	// GRAPH
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	} },
	
	// Y1
	//	  1     !     日    ぬ          ヌ
	{ {	{ 0x31, 0x21, 0x07, 0xe7, 0x00, 0xc7, 0x00, 0x00 },
	//	  q     Q           た          タ          Q
		{ 0x71, 0x51, 0x00, 0xe0, 0x00, 0xc0, 0x00, 0x11 },
	//	  a     A           ち          チ          A
		{ 0x61, 0x41, 0x00, 0xe1, 0x00, 0xc1, 0x00, 0x01 },
	//	  z     Z           つ    っ    ツ    ッ    Z
		{ 0x7a, 0x5a, 0x00, 0xe2, 0x8f, 0xc2, 0xaf, 0x1a },
	//	  k     K           の          ノ          K
		{ 0x6b, 0x4b, 0x00, 0xe9, 0x00, 0xc9, 0x00, 0x0b },
	//	  i     I     ┃    に          ニ          I
		{ 0x69, 0x49, 0x16, 0xe6, 0x00, 0xc6, 0x00, 0x09 },
	//	  8     (     百    ゆ    ゅ    ユ    ュ
		{ 0x38, 0x28, 0x0d, 0xf5, 0x8d, 0xd5, 0xad, 0x00 },
	//	  ，    <     小    ね    ，    ネ    ，
		{ 0x2c, 0x3c, 0x1f, 0xe8, 0xa4, 0xc8, 0xa4, 0x00 }
	} },
	
	// Y2
	//	  2     "     月    ふ          フ
	{ {	{ 0x32, 0x22, 0x01, 0xec, 0x00, 0xcc, 0x00, 0x00 },
	//	  w     W           て          テ          W
		{ 0x77, 0x57, 0x00, 0xe3, 0x00, 0xc3, 0x00, 0x17 },
	//	  s     S     秒    と          ト          S
		{ 0x73, 0x53, 0x0c, 0xe4, 0x00, 0xc4, 0x00, 0x13 },
	//	  x     X     ×    さ          サ          X
		{ 0x78, 0x58, 0x1c, 0x9b, 0x00, 0xbb, 0x00, 0x18 },
	//	  l     L     中    り          リ          L
		{ 0x6c, 0x4c, 0x1e, 0xf8, 0x00, 0xd8, 0x00, 0x0c },
	//	  o     O           ら          ラ          O
		{ 0x6f, 0x4f, 0x00, 0xf7, 0x00, 0xd7, 0x00, 0x0f },
	//	  9     )     千    よ    ょ    ヨ    ョ
		{ 0x39, 0x29, 0x0e, 0xf6, 0x8e, 0xd6, 0xae, 0x00 },
	//	  .     >     大    る    。    ル    。
		{ 0x2e, 0x3e, 0x1d, 0xf9, 0xa1, 0xd9, 0xa1, 0x00 }
	} },
	
	// Y3
	//	  3     #     火    あ    ぁ    ア    ァ
	{ {	{ 0x33, 0x23, 0x02, 0x91, 0x87, 0xb1, 0xa7, 0x00 },
	//	  e     E     ┏    い    ぃ    イ    ィ    E
		{ 0x65, 0x45, 0x18, 0x92, 0x88, 0xb2, 0xa8, 0x05 },
	//	  d     D     ┣    し          シ          D
		{ 0x64, 0x44, 0x14, 0x9c, 0x00, 0xbc, 0x00, 0x04 },
	//	  c     C     ┗    そ          ソ          C
		{ 0x63, 0x43, 0x1a, 0x9f, 0x00, 0xbf, 0x00, 0x03 },
	//	  ;     +    クラブ れ          レ
		{ 0x3b, 0x2b, 0x82, 0xfa, 0x00, 0xda, 0x00, 0x00 },
	//	  p     P     π    せ          セ          P
		{ 0x70, 0x50, 0x10, 0x9e, 0x00, 0xbe, 0x00, 0x10 },
	//	        F6    F1
		{ 0x00, 0xf5, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00 },
	//	  /     ?   スペード め   ・    メ    ・
		{ 0x2f, 0x3f, 0x80, 0xf2, 0xa5, 0xd2, 0xa5, 0x00 }
	} },
	
	// Y4
	//	  4     $     水    う    ぅ    ウ    ゥ
	{ {	{ 0x34, 0x24, 0x03, 0x93, 0x89, 0xb3, 0xa9, 0x00 },
	//	  r     R     ┳    す          ス          R
		{ 0x72, 0x52, 0x12, 0x9d, 0x00, 0xbd, 0x00, 0x12 },
	//	  f     F     ╋    は          ハ          F
		{ 0x66, 0x46, 0x15, 0xea, 0x00, 0xca, 0x00, 0x06 },
	//	  v     V     ┻    ひ          ヒ          V
		{ 0x76, 0x56, 0x11, 0xeb, 0x00, 0xcb, 0x00, 0x16 },
	//	  :     *    ハート け          ケ
		{ 0x3a, 0x2a, 0x81, 0x99, 0x00, 0xb9, 0x00, 0x00 },
	//	  @                 ゛          ゛
		{ 0x40, 0x00, 0x00, 0xde, 0x00, 0xde, 0x00, 0x00 },
	//	        F7    F2
		{ 0x00, 0xf6, 0xf1, 0x00, 0x00, 0x00, 0x00, 0x00 },
	//	  _    ダイヤ       ろ          ロ
		{ 0x00, 0x5f, 0x83, 0xfb, 0x00, 0xdb, 0x00, 0x00 }
	} },
	
	// Y5
	//	  5     %     木    え    ぇ    エ    ェ
	{ {	{ 0x35, 0x25, 0x04, 0x94, 0x8a, 0xb4, 0xaa, 0x00 },
	//	  t     T     ┓    か          カ          T
		{ 0x74, 0x54, 0x19, 0x96, 0x00, 0xb6, 0x00, 0x14 },
	//	  g     G     ┫    き          キ          G
		{ 0x67, 0x47, 0x13, 0x97, 0x00, 0xb7, 0x00, 0x07 },
	//	  b     B     ┛    こ          コ          B
		{ 0x62, 0x42, 0x1b, 0x9a, 0x00, 0xba, 0x00, 0x02 },
	//	  ]           ●    む    」    ム    」
		{ 0x5d, 0x00, 0x85, 0xf1, 0xa3, 0xd1, 0xa3, 0x00 },
	//	  [           ○    ゜    「     ゜    「
		{ 0x5b, 0x00, 0x84, 0xdf, 0xa2, 0xdf, 0xa2, 0x00 },
	//	        F8    F3
		{ 0x00, 0xf7, 0xf2, 0x00, 0x00, 0x00, 0x00, 0x00 },
	//	  Space Space       Space Space Space Space
		{ 0x20, 0x20, 0x00, 0x20, 0x20, 0x20, 0x20, 0x00 }
	} },
	
	// Y6
	//	  6     &     金    お    ぉ    オ    ォ
	{ {	{ 0x36, 0x26, 0x05, 0x95, 0x8b, 0xb5, 0xab, 0x00 },
	//	  y     Y     年    ん          ン          Y
		{ 0x79, 0x59, 0x08, 0xfd, 0x00, 0xdd, 0x00, 0x19 },
	//	  h     H     時    く          ク          H
		{ 0x68, 0x48, 0x0a, 0x98, 0x00, 0xb8, 0x00, 0x08 },
	//	  n     N     み    ミ                      N
		{ 0x6e, 0x4e, 0x00, 0xf0, 0x00, 0xd0, 0x00, 0x0e },
	//	  -     =     ━    ほ          ホ
		{ 0x2d, 0x3d, 0x17, 0xee, 0x00, 0xce, 0x00, 0x00 },
	//	  ^                 へ          ヘ
		{ 0x5e, 0x00, 0x00, 0xed, 0x00, 0xcd, 0x00, 0x00 },
	//	        F9    F4
		{ 0x00, 0xf8, 0xf3, 0x00, 0x00, 0x00, 0x00, 0x00 },
	//	  0           万    わ    を    ワ    ヲ
		{ 0x30, 0x00, 0x0f, 0xfc, 0x86, 0xdc, 0xa6, 0x00 }
	} },
	
	// Y7
	//	  7     '     土    や    ゃ    ヤ    ャ
	{ {	{ 0x37, 0x27, 0x06, 0xf4, 0x8c, 0xd4, 0xac, 0x00 },
	//	  u     U           な          ナ          U
		{ 0x75, 0x55, 0x00, 0xe5, 0x00, 0xc5, 0x00, 0x15 },
	//	  j     J           ま          マ          J
		{ 0x6a, 0x4a, 0x00, 0xef, 0x00, 0xcf, 0x00, 0x0a },
	//	  m     M     分    も          モ          M
		{ 0x6d, 0x4d, 0x0b, 0xf3, 0x00, 0xd3, 0x00, 0x0d },
	//
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
	//	  ￥          円    ー          ー
		{ 0x5c, 0x00, 0x09, 0xb0, 0x00, 0xb0, 0x00, 0x00 },
	//	        F10   F5
		{ 0x00, 0xf9, 0xf4, 0x00, 0x00, 0x00, 0x00, 0x00 },
	//
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	} },
	
	// Y8
	//	  RET   RET         RET   RET   RET   RET
	{ {	{ 0x0d, 0x0d, 0x00, 0x0d, 0x0d, 0x0d, 0x0d, 0x00 },
	//	  STOP  STOP  STOP  STOP  STOP  STOP  STOP
		{ 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0xfa, 0x00 },
	//	  ↑    ↑    ↑    ↑    ↑    ↑    ↑
		{ 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x1e, 0x00 },
	//	  ↓    ↓    ↓    ↓    ↓    ↓    ↓
		{ 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x1f, 0x00 },
	//	  →    →    →    →    →    →    →
		{ 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x1c, 0x00 },
	//	  ←    ←    ←    ←    ←    ←    ←
		{ 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x00 },
	//	  TAB   TAB         TAB   TAB   TAB   TAB
		{ 0x09, 0x09, 0x00, 0x09, 0x09, 0x09, 0x09, 0x00 },
	//	  ESC   ESC         ESC   ESC   ESC   ESC
		{ 0x1b, 0x1b, 0x00, 0x1b, 0x1b, 0x1b, 0x1b, 0x00 }
	} },
	
	// Y9
	//	              かな
	{ {	{ 0x00, 0x00, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00 },
	//	  INS   INS   ┳    INS   INS   INS   INS
		{ 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x12, 0x00 },
	//	  DEL   DEL   ????  DEL   DEL   DEL   DEL
		{ 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x00 },
	//	  PAGE  PAGE  PAGE  PAGE  PAGE  PAGE  PAGE
		{ 0xfe, 0xfe, 0xfc, 0xfe, 0xfe, 0xfe, 0xfe, 0x00 },
	//	  HOME  HOME  秒    HOME  HOME  HOME  HOME
		{ 0x0c, 0x0b, 0x0c, 0x0c, 0x0b, 0x0c, 0x0b, 0x00 },
	//	              MODE
		{ 0x00, 0x00, 0xfd, 0x00, 0x00, 0x00, 0x00, 0x00 },
	//	              CAPS
		{ 0x00, 0x00, 0xfb, 0x00, 0x00, 0x00, 0x00, 0x00 },
	//
		{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
	} },
	
	// これ以降は独自拡張
	// YA
	//	  [0]   [0]         [0]   [0]   [0]   [0]
	{ {	{ 0x30, 0x30, 0x00, 0x30, 0x30, 0x30, 0x30, 0x00 },
	//	  [1]   [1]         [1]   [1]   [1]   [1]
		{ 0x31, 0x31, 0x00, 0x31, 0x31, 0x31, 0x31, 0x00 },
	//	  [2]   [2]         [2]   [2]   [2]   [2]
		{ 0x32, 0x32, 0x00, 0x32, 0x32, 0x32, 0x32, 0x00 },
	//	  [3]   [3]         [3]   [3]   [3]   [3]
		{ 0x33, 0x33, 0x00, 0x33, 0x33, 0x33, 0x33, 0x00 },
	//	  [4]   [4]         [4]   [4]   [4]   [4]
		{ 0x34, 0x34, 0x00, 0x34, 0x34, 0x34, 0x34, 0x00 },
	//	  [5]   [5]         [5]   [5]   [5]   [5]
		{ 0x35, 0x35, 0x00, 0x35, 0x35, 0x35, 0x35, 0x00 },
	//	  [6]   [6]         [6]   [6]   [6]   [6]
		{ 0x36, 0x36, 0x00, 0x36, 0x36, 0x36, 0x36, 0x00 },
	//	  [7]   [7]         [7]   [7]   [7]   [7]
		{ 0x37, 0x37, 0x00, 0x37, 0x37, 0x37, 0x37, 0x00 }
	} },
	
	// YB
	//	  [8]   [8]         [8]   [8]   [8]   [8]
	{ {	{ 0x38, 0x38, 0x00, 0x38, 0x38, 0x38, 0x38, 0x00 },
	//	  [9]   [9]         [9]   [9]   [9]   [9]
		{ 0x39, 0x39, 0x00, 0x39, 0x39, 0x39, 0x39, 0x00 },
	//	  [.]   [.]         [.]   [.]   [.]   [.]
		{ 0x2e, 0x2e, 0x00, 0x2e, 0x2e, 0x2e, 0x2e, 0x00 },
	//	  [+]   [+]         [+]   [+]   [+]   [+]
		{ 0x2b, 0x2b, 0x00, 0x2b, 0x2b, 0x2b, 0x2b, 0x00 },
	//	  [-]   [-]         [-]   [-]   [-]   [-]
		{ 0x2d, 0x2d, 0x00, 0x2d, 0x2d, 0x2d, 0x2d, 0x00 },
	//	  [*]   [*]         [*]   [*]   [*]   [*]
		{ 0x2a, 0x2a, 0x00, 0x2a, 0x2a, 0x2a, 0x2a, 0x00 },
	//	  [/]   [/]         [/]   [/]   [/]   [/]
		{ 0x2f, 0x2f, 0x00, 0x2f, 0x2f, 0x2f, 0x2f, 0x00 },
	//	  [RET] [RET]       [RET] [RET] [RET] [RET]
		{ 0x0d, 0x0d, 0x00, 0x0d, 0x0d, 0x0d, 0x0d, 0x00 }
	} }
};



enum KeyGroup{
	KEYSTD = 0,
	KEYSHIFT,
	KEYGRAPH,
	KEYKANA,
	KEYKANASHIFT,
	KEYKKANA,
	KEYKKANASHIFT,
	KEYCTRL
};



/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
KEY6::KEY6( VM6* vm, const ID& id ) : Device( vm, id ),
	ON_SHIFT( false ), ON_GRAPH( false ), ON_KANA( false ), ON_KKANA( false ),
	ON_CTRL( false ), ON_STOP( false ), ON_CAPS( false )
{
	// P6キーコード -> マトリクス 変換テーブル初期化
	MatTable.clear();
	for( auto &i : P6KeyMx )
		MatTable.emplace( i.P6Key, i.Mat );
	
	// キーマトリクス初期化
	for( int i=0; i<16*2; i++ ){
		P6Matrix.emplace_back( 0xff );
		P6Mtrx.emplace_back( 0xff );
	}
}

KEY60::KEY60( VM6* vm, const ID& id ) : KEY6( vm, id )
{
	// MODE と CAPS 無効化
	try{
		MatTable.at( KP6_MODE ) = 0;
		MatTable.at( KP6_CAPS ) = 0;
	}
	catch( std::out_of_range& ){}
}

KEY62::KEY62( VM6* vm, const ID& id ) : KEY6( vm, id )
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
KEY6::~KEY6( void )
{
}

KEY60::~KEY60( void )
{
}

KEY62::~KEY62( void )
{
}


/////////////////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool KEY6::Init( void )
{
	PRINTD( KEY_LOG, "[KEY][Init]\n" );
	
	// 特殊キー フラグ初期化
	Reset();
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// リセット
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void KEY6::Reset( void )
{
	PRINTD( KEY_LOG, "[KEY][Reset]\n" );
	
	// 特殊キー フラグ初期化
	ON_SHIFT =
	ON_GRAPH =
	ON_KANA  =
	ON_KKANA =
	ON_CTRL  =
	ON_STOP  =
	ON_CAPS  = false;
}


/////////////////////////////////////////////////////////////////////////////
// キーマトリクス更新(キー)
//
// 引数:	code	仮想キーコード
//			pflag	押したフラグ true:押した false:離した
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void KEY6::UpdateMatrixKey( PCKEYsym code, bool pflag )
{
	PRINTD( KEY_LOG, "[KEY][UpdateMatrixKey] %02X %s\n", code, pflag ? "PUSH" : "RELEASE" );
	
	try{
		// P6キーコード->キーマトリクスを取得
		BYTE mtx = MatTable.at( K6Table.at( code ) );
		if( !mtx ) return;
		
		// マトリクス更新(押したキーに対応するbitが0になる)
		BYTE matX = 1<<(mtx&0x0f);
		BYTE matY = (mtx>>4)&0x0f;
		
		if( pflag ){
			P6Matrix.at( matY ) &= ~matX;
			
			// 【キーリピート対応の暫定処置】
			// リピート時はリリース情報が出力されないので
			// 前回のマトリクスをリリースに書き換える。
			// リピートをSDL任せにしている間の暫定処置で
			// サブCPU側で処理するようになったら不要。
			if( ~P6Matrix.at( matY+NOM ) & matX ) P6Matrix.at( matY+NOM ) |= matX;
		}else{
			P6Matrix.at( matY ) |=  matX;
			//【キーを離した時の取りこぼし防止】
			if( P6Matrix.at( matY+NOM ) & matX ) P6Matrix.at( matY+NOM ) &= ~matX;
		}
	}
	catch( std::out_of_range& ){}
}


/////////////////////////////////////////////////////////////////////////////
// キーマトリクス更新(ジョイスティック)
//
// 引数:	joy1	ジョイスティック1の状態
//			joy2	ジョイスティック2の状態
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void KEY6::UpdateMatrixJoy( const BYTE joy1, const BYTE joy2 )
{
	PRINTD( KEY_LOG, "[KEY][UpdateMatrixJoy] JOY1:%02X JOY2:%02X\n", joy1, joy2 );
	
	try{
		P6Matrix.at( NOM-2 ) = joy1;
		P6Matrix.at( NOM-1 ) = joy2;
	}
	catch( std::out_of_range& ){}
}


/////////////////////////////////////////////////////////////////////////////
// キーマトリクススキャン
//
// 引数:	なし
// 返値:	true:変化あり false:変化なし
/////////////////////////////////////////////////////////////////////////////
bool KEY6::ScanMatrix( void )
{
	PRINTD( KEY_LOG, "[KEY][ScanMatrix] " );
	
	bool MatChg  = false;	// 前回のマトリクスと変化したかフラグ
	bool KeyPUSH = false;	// キー押したフラグ
	bool ON_FUNC = false;	// ファンクションキーとかフラグ
	BYTE MatData = 0;		// P6のマトリクスコード(bit7-4:Y-1 bit3-0:X)
	BYTE KeyData = 0;		// P6のキーコード
	
	// キーマトリクスを保存
	P6Mtrx = P6Matrix;
	
	try{
		// 特殊キー判定(ON-OFF状態を判定) キーマトリクスY0
		ON_CTRL  = P6Mtrx.at( 0 ) & 0x02 ? false : true;
		ON_SHIFT = P6Mtrx.at( 0 ) & 0x04 ? false : true;
		ON_GRAPH = P6Mtrx.at( 0 ) & 0x08 ? false : true;
		// 前回のマトリクスと変化あり?
		if( P6Mtrx.at( 0 ) != P6Mtrx.at( 0+NOM ) ) MatChg = true;
	}
	catch( std::out_of_range& ){}
	
	// 一般キー判定 キーマトリクスY1～
	for( int y=1; (y<(NOM-2)) && !KeyPUSH; y++ ) try{
		// 前回のマトリクスと変化あり?
		if( P6Mtrx.at( y ) != P6Mtrx.at( y+NOM ) ){
			MatChg = true;
			// キー押した?
			if( (P6Mtrx.at( y ) ^ P6Mtrx.at( y+NOM )) & P6Mtrx.at( y+NOM ) ){
				KeyPUSH = true;
				for( int x=0; x<8; x++ ) try{
					// マトリクスコードセット bit7-4:Y-1 bit3-0:X
					// 1->0 になったビットを検出
					if( (~P6Mtrx.at( y )>>x)&1 && (P6Mtrx.at( y+NOM )>>x)&1 ){
						MatData = (y<<4) | (x&7);
						break;
					}
				}
				catch( std::out_of_range& ){}
			}
		}
	}
	catch( std::out_of_range& ){}
	
	if( KeyPUSH ){	// キー押した?
		switch( MatData ){	// マトリクスコード
		case 0x96:	// CAPS
			ON_CAPS = !ON_CAPS;
			ON_FUNC = true;
			break;
			
		case 0x95:	// MODE
			ON_FUNC = true;
			break;
			
		case 0x90:	// かな
			ON_KANA = !ON_KANA;
			ON_FUNC = true;
			break;
			
		case 0x81:	// STOP
			ON_STOP = true;
			break;
			
		case 0x93:	// PAGE or かな/カナ切り替え
			if( ON_SHIFT ){
				ON_KKANA = !ON_KKANA;
				ON_FUNC  = true;
			}
			break;
			
		case 0x36:			// F01～F05(ファンクションキー)
		case 0x46:
		case 0x56:
		case 0x66:
		case 0x76:
			ON_FUNC = true;
			break;
		}
		
		BYTE* KeyMap = (BYTE*)P6MxCode[MatData>>4].Keys[MatData&7];
		if( ON_FUNC ){
			switch( MatData ){	// マトリクスコード
			case 0x36:			// F01～F05(ファンクションキー)
			case 0x46:
			case 0x56:
			case 0x66:
			case 0x76:
				KeyData = KeyMap[ON_SHIFT ? KEYSHIFT : KEYGRAPH];
				break;
			default:
				KeyData = KeyMap[KEYGRAPH];
			}
		}else{
			if( ON_KANA ){
				if( ON_KKANA ) KeyData = KeyMap[ON_SHIFT ? KEYKKANASHIFT : KEYKKANA];
				else           KeyData = KeyMap[ON_SHIFT ? KEYKANASHIFT  : KEYKANA];
			}else{
				if( ON_GRAPH )
					KeyData = KeyMap[KEYGRAPH];
				else
					if( ON_CTRL ) KeyData = KeyMap[KEYCTRL];
					else          KeyData = KeyMap[ON_SHIFT ? KEYSHIFT : KEYSTD];
			}
		}
		
		// キー割込み処理
		if( KeyData ){	// 有効なキー?
			// flag
			// bit 0 : ON_STOP
			// bit 1 : ON_GRAPH
			// bit 2 : ON_FUNC
			
			PRINTD( KEY_LOG, "[Intr] %02X", KeyData );
			vm->CpusReqKeyIntr( ON_STOP ? 1 : 0 | ON_GRAPH ? 2 : 0 | ON_FUNC ? 4 : 0, KeyData );
			ON_STOP = false;
		}
	}
	
	// ジョイスティック判定
	for( int y=NOM-2; y<NOM; y++ ) try{
		// 前回のマトリクスと変化あり?
		if( P6Mtrx.at( y ) != P6Mtrx.at( y+NOM ) ) MatChg = true;
	}
	catch( std::out_of_range& ){}
	
	// 変化があればキーマトリクス保存
	if( MatChg )
		for( int i=0; i<NOM; i++ ) try{
			P6Matrix.at( i+NOM ) = P6Mtrx.at( i );
		}
		catch( std::out_of_range& ){}
	
	PRINTD( KEY_LOG, "\n" );
	
	return MatChg;
}


/////////////////////////////////////////////////////////////////////////////
// キーマトリクス取得
//
// 引数:	なし
// 返値:	vector<BYTE>&	マトリクスデータ
/////////////////////////////////////////////////////////////////////////////
std::vector<BYTE>& KEY6::GetMatrix( void )
{
	return P6Matrix;
}


/////////////////////////////////////////////////////////////////////////////
// キーマトリクス(保存用)取得
//
// 引数:	なし
// 返値:	vector<BYTE>&	マトリクスデータ(保存用)
/////////////////////////////////////////////////////////////////////////////
const std::vector<BYTE>& KEY6::GetMatrix2( void ) const
{
	return P6Mtrx;
}


/////////////////////////////////////////////////////////////////////////////
// カーソルキー状態取得
//
// 引数:	なし
// 返値:	BYTE	カーソルキー等の状態 1:ON 0:OFF
//				bit7: スペースキー
//				bit6: 常に0
//				bit5: 左
//				bit4: 右
//				bit3: 下
//				bit2: 上
//				bit1: STOP
//				bit0: SHIFT
/////////////////////////////////////////////////////////////////////////////
BYTE KEY6::GetKeyJoy( void ) const
{
	PRINTD( KEY_LOG, "[KEY][GetKeyJoy]\n" );
	
	try{
		return	(~P6Mtrx.at( 5 )&0x80) | (~P6Mtrx.at( 8 )&0x20) | (~P6Mtrx.at( 8 )&0x10) | (~P6Mtrx.at( 8 )&0x08) |
				(~P6Mtrx.at( 8 )&0x04) | (~P6Mtrx.at( 8 )&0x02) | (~P6Mtrx.at( 0 )&0x04)>>2;
	}
	catch( std::out_of_range& ){
		return 0;
	}
}


/////////////////////////////////////////////////////////////////////////////
// ジョイスティック状態取得
//
// 引数:	JoyNo	ジョイスティックの番号 ( 0 - 1 )
// 返値:	BYTE	ジョイスティックの状態 1:OFF 0:ON <-注意!
//				bit7: -
//				bit6: -
//				bit5: ボタン2
//				bit4: ボタン1
//				bit3: 右
//				bit2: 左
//				bit1: 上
//				bit0: 下
/////////////////////////////////////////////////////////////////////////////
BYTE KEY6::GetJoy( const int JoyNo ) const
{
	try{
		return P6Mtrx.at( NOM-2+(JoyNo&1) ) | 0xc0;
	}
	catch( std::out_of_range& ){
		return 0xff;
	}
}


/////////////////////////////////////////////////////////////////////////////
// キーボードインジケータ状態取得
//
// 引数:	なし
// 返値:	BYTE	インジケータ状態
//				KI_KANA		かなモードON
//				KI_KKANA	カタカナモードON
//				KI_CAPS		CAPS ON
//				KI_SHIFT	SHIFT ON
//				KI_GRAPH	GRAPH ON
//				KI_CTRL		CTRL ON
/////////////////////////////////////////////////////////////////////////////
BYTE KEY6::GetKeyIndicator( void ) const
{
	PRINTD( KEY_LOG, "[KEY][GetKeyIndicator] -> " );
	
	BYTE ret = (ON_KANA  ? KI_KANA  : 0)
			 | (ON_KKANA ? KI_KKANA : 0)
			 | (ON_CAPS  ? KI_CAPS  : 0)
			 | (ON_SHIFT ? KI_SHIFT : 0)
			 | (ON_GRAPH ? KI_GRAPH : 0)
			 | (ON_CTRL  ? KI_CTRL  : 0);
	
	PRINTD( KEY_LOG, "%d\n", ret );
	
	return ret;
}


/////////////////////////////////////////////////////////////////////////////
// 仮想キーコード -> P6キーコード 設定
//
// 引数:	vcnv	仮想キーコード -> P6キーコード定義構造体 先頭ポインタ
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void KEY6::SetVKeySymbols( std::vector<VKeyConv>& vcnv )
{
	K6Table.clear();
	for( auto &i : vcnv )
		K6Table.emplace( i.PCKey, i.P6Key );
}


/////////////////////////////////////////////////////////////////////////////
// 英字<->かな切換
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void KEY6::ChangeKana( void )
{
	ON_KANA = !ON_KANA;
}


/////////////////////////////////////////////////////////////////////////////
// かな<->カナ切換
//
// 引数:	なし
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void KEY6::ChangeKKana( void )
{
	ON_KKANA = !ON_KKANA;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもSAVE
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool KEY6::DokoSave( cIni* Ini )
{
	if( !Ini ) return false;
	
	Ini->SetVal( "KEY", "ON_KANA",	"", ON_KANA  );
	Ini->SetVal( "KEY", "ON_KKANA",	"", ON_KKANA );
	Ini->SetVal( "KEY", "ON_STOP",	"", ON_STOP  );
	Ini->SetVal( "KEY", "ON_CAPS",	"", ON_CAPS  );
	
	std::string strva;
	for( auto &i : P6Matrix )
		strva += Stringf( "%02X", i );
	Ini->SetEntry( "KEY", "P6Matrix",	"", strva.c_str() );
	
	strva.clear();
	for( auto &i : P6Mtrx )
		strva += Stringf( "%02X", i );
	Ini->SetEntry( "KEY", "P6Mtrx",		"", strva.c_str() );
	
	return true;
}


/////////////////////////////////////////////////////////////////////////////
// どこでもLOAD
//
// 引数:	Ini		INIオブジェクトポインタ
// 返値:	bool	true:成功 false:失敗
/////////////////////////////////////////////////////////////////////////////
bool KEY6::DokoLoad( cIni* Ini )
{
	std::string strva;
	
	if( !Ini ) return false;
	
	Ini->GetVal( "KEY", "ON_KANA",	ON_KANA  );
	Ini->GetVal( "KEY", "ON_KKANA",	ON_KKANA );
	Ini->GetVal( "KEY", "ON_STOP",	ON_STOP  );
	Ini->GetVal( "KEY", "ON_CAPS",	ON_CAPS  );
	
	if( Ini->GetEntry( "KEY", "P6Matrix", strva ) ){
		strva.resize( P6Matrix.size() * 2, 'F' );
		int i = 0;
		for( auto &m : P6Matrix )
			m = std::stoul( strva.substr( i++ * 2, 2 ), nullptr, 16 );
	}
	if( Ini->GetEntry( "KEY", "P6Mtrx", strva ) ){
		strva.resize( P6Mtrx.size() * 2, 'F' );
		int i = 0;
		for( auto &m : P6Mtrx )
			m = std::stoul( strva.substr( i++ * 2, 2 ), nullptr, 16 );
	}
	
	return true;
}
