#include <stdlib.h>

#include "../log.h"
#include "../vsurface.h"
#include "mc6847.h"


#define	P60W		256			// 水平有効表示期間(N60)
#define	P60H		192			// 垂直有効表示ライン(N60)
#define	P62W		320			// 水平有効表示期間(N60m)
#define	P62H		200			// 垂直有効表示ライン(N60m)
#define	P64W		320			// 水平有効表示期間(SR)
#define	P64H		((!CharMode && SRLine204) ? 204 : 200)	// 垂直有効表示ライン(SR)

// バックバッファサイズ
#define	WBBUF		(376-16)	// 必ず4の倍数
#define	HBBUF		(242-12)

// ボーダーサイズ
#define	LB60		((int)((WBBUF-P60W)/2))
#define	RB60		(WBBUF-P60W-LB60)
#define	TB60		((int)((HBBUF-P60H)/2))
#define	BB60		(HBBUF-P60H-TB60)
#define	LB62		((int)((WBBUF-P62W)/2))
#define	RB62		(WBBUF-P62W-LB62)
#define	TB62		((int)((HBBUF-P62H)/2))
#define	BB62		(HBBUF-P62H-TB62)
#define	LB64		((int)((WBBUF-P64W)/2))
#define	RB64		(WBBUF-P64W-LB64)
#define	TB64		((int)((HBBUF-P64H)/2))
#define	BB64		(HBBUF-P64H-TB64)

// アトリビュート
#define ATTR_AG		0x80
#define ATTR_AS		0x40
#define ATTR_INTEXT	0x20
#define ATTR_GM0	0x10
#define ATTR_GM1	0x08
#define ATTR_GM2	0x04
#define ATTR_CSS	0x02
#define ATTR_INV	0x01

#define ANMODE		( ( AT_AS << 1 ) | AT_IE )
#define AM_AN0		0			// アルファニューメリック(内部フォント)
#define AM_AN1		1			// アルファニューメリック(外部フォント)
#define AM_SG4		2			// セミグラフィック4
#define AM_SG6		3			// セミグラフィック6

#define GM_CG1		0			//  64x 64 カラー
#define GM_CG2		1			// 128x 64 カラー
#define GM_CG3		2			// 128x 96 カラー
#define GM_CG6		3			// 128x192 カラー
#define GM_RG1		4			// 128x 64 モノクロ
#define GM_RG2		5			// 128x 96 モノクロ
#define GM_RG3		6			// 128x192 モノクロ
#define GM_RG6		7			// 256x192 モノクロ


// VDG Font data
const BYTE MC6847::VDGfont[] = {
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x02, 0x1a, 0x2a, 0x2a, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// @
	0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// A
	0x00, 0x00, 0x00, 0x3c, 0x12, 0x12, 0x1c, 0x12, 0x12, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// B
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x20, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// C
	0x00, 0x00, 0x00, 0x3c, 0x12, 0x12, 0x12, 0x12, 0x12, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// D
	0x00, 0x00, 0x00, 0x3e, 0x20, 0x20, 0x3c, 0x20, 0x20, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// E
	0x00, 0x00, 0x00, 0x3e, 0x20, 0x20, 0x3c, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// F
	0x00, 0x00, 0x00, 0x1e, 0x20, 0x20, 0x26, 0x22, 0x22, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// G
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// H
	0x00, 0x00, 0x00, 0x1c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// I
	0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// J
	0x00, 0x00, 0x00, 0x22, 0x24, 0x28, 0x30, 0x28, 0x24, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// K
	0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// L
	0x00, 0x00, 0x00, 0x22, 0x36, 0x2a, 0x2a, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// M
	0x00, 0x00, 0x00, 0x22, 0x32, 0x2a, 0x26, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// N
	0x00, 0x00, 0x00, 0x3e, 0x22, 0x22, 0x22, 0x22, 0x22, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// O

	0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// P
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x2a, 0x24, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Q
	0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x3c, 0x28, 0x24, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// R
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x10, 0x08, 0x04, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// S
	0x00, 0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// T
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// U
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// V
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x2a, 0x2a, 0x36, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// W
	0x00, 0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x14, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// X
	0x00, 0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Y
	0x00, 0x00, 0x00, 0x3e, 0x02, 0x04, 0x08, 0x10, 0x20, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Z
	0x00, 0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// [
	0x00, 0x00, 0x00, 0x20, 0x20, 0x10, 0x08, 0x04, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// \(BackSlash)
	0x00, 0x00, 0x00, 0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ]
	0x00, 0x00, 0x00, 0x08, 0x1c, 0x2a, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ↑
	0x00, 0x00, 0x00, 0x00, 0x08, 0x10, 0x3e, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ←

	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ' '
	0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// !
	0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// "
	0x00, 0x00, 0x00, 0x14, 0x14, 0x36, 0x00, 0x36, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// #
	0x00, 0x00, 0x00, 0x08, 0x1e, 0x20, 0x1c, 0x02, 0x3c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// $
	0x00, 0x00, 0x00, 0x32, 0x32, 0x04, 0x08, 0x10, 0x26, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// %
	0x00, 0x00, 0x00, 0x10, 0x28, 0x28, 0x10, 0x2a, 0x24, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// &
	0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// '
	0x00, 0x00, 0x00, 0x08, 0x10, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// (
	0x00, 0x00, 0x00, 0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// )
	0x00, 0x00, 0x00, 0x00, 0x08, 0x2a, 0x1c, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// *
	0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// +
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// -
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// .
	0x00, 0x00, 0x00, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// /

	0x00, 0x00, 0x00, 0x18, 0x24, 0x24, 0x24, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 0
	0x00, 0x00, 0x00, 0x08, 0x18, 0x08, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 1
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x02, 0x1c, 0x20, 0x20, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 2
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x02, 0x0c, 0x02, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 3
	0x00, 0x00, 0x00, 0x04, 0x0c, 0x14, 0x3e, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 4
	0x00, 0x00, 0x00, 0x3e, 0x20, 0x3c, 0x02, 0x02, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 5
	0x00, 0x00, 0x00, 0x1c, 0x20, 0x20, 0x3c, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 6
	0x00, 0x00, 0x00, 0x3e, 0x02, 0x04, 0x08, 0x10, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 7
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x1c, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 8
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x1e, 0x02, 0x02, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 9
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// :
	0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ;
	0x00, 0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// <
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// =
	0x00, 0x00, 0x00, 0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// >
	0x00, 0x00, 0x00, 0x18, 0x24, 0x04, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// ?
};

// 色にじみカラーコードテーブル
// 数値はCOL_CG上のインデックス
/*
// ベタ塗り優先
const BYTE MC6847core::NJM_TBL[][2] = {
				{ 0,0 }, { 0,0 }, { 0,7 }, { 0,7 },
				{ 5,1 }, { 5,1 }, { 5,4 }, { 5,4 },
				{ 2,7 }, { 2,7 }, { 2,2 }, { 2,2 },
				{ 6,4 }, { 6,4 }, { 6,3 }, { 6,3 },
				{ 5,0 }, { 5,0 }, { 5,7 }, { 5,7 },
				{ 1,1 }, { 1,1 }, { 1,4 }, { 1,4 },
				{ 6,7 }, { 6,7 }, { 6,2 }, { 6,2 },
				{ 3,4 }, { 3,4 }, { 3,3 }, { 3,3 },
				{ 0,0 }, { 0,0 }, { 0,7 }, { 0,7 },
				{ 5,1 }, { 5,1 }, { 5,4 }, { 5,4 },
				{ 2,7 }, { 2,7 }, { 2,2 }, { 2,2 },
				{ 6,4 }, { 6,4 }, { 6,3 }, { 6,3 },
				{ 5,0 }, { 5,0 }, { 5,7 }, { 5,7 },
				{ 1,1 }, { 1,1 }, { 1,4 }, { 1,4 },
				{ 6,7 }, { 6,7 }, { 6,2 }, { 6,2 },
				{ 3,4 }, { 3,4 }, { 3,3 }, { 3,3 }
			};
*/
// 文字の視認性優先
const BYTE MC6847core::NJM_TBL[][2] = {
				{ 0,0 }, { 0,0 }, { 0,7 }, { 0,7 },
				{ 5,4 }, { 5,4 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 6,2 }, { 6,7 },
				{ 6,4 }, { 6,4 }, { 6,3 }, { 6,3 },
				{ 5,0 }, { 5,0 }, { 5,7 }, { 5,7 },
				{ 1,4 }, { 1,1 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 6,7 }, { 6,7 },
				{ 3,4 }, { 3,4 }, { 3,3 }, { 3,3 },
				{ 0,0 }, { 0,0 }, { 0,7 }, { 0,7 },
				{ 5,4 }, { 5,4 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 2,2 }, { 2,7 },
				{ 6,4 }, { 6,4 }, { 6,3 }, { 6,3 },
				{ 5,0 }, { 5,0 }, { 5,7 }, { 5,7 },
				{ 5,4 }, { 5,1 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 6,7 }, { 6,7 },
				{ 3,4 }, { 3,4 }, { 3,3 }, { 3,3 }
			};
//色にじみは2ドット単位で描画する。
//描画する対象の2ドットとその前後の各2ドット、計6ビットをVRAMから取得し
//その値をインデックスとして、2ドット分の色情報を取得する。
//テーブルに格納された値はさらにCOL_CG上のインデックスとなる。
//黒　0
//白　3
//赤/桃　明←6 2 7→暗
//青/緑　明←4 1 5→暗


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
MC6847core::MC6847core( void ) :
	CrtDisp(true), BusReq(false), N60Win(true),
	Mode4Col(0), VAddr(0), HAddr(0), RowCntA(0), RowCntG(0),
	CharMode(true), GraphMode(false), Css1(1), Css2(1), Css3(1),
	SRmode(false), SRBusReq(true), SRBitmap(false), SRBMPage(false), SRLine204(false),
	SRCharLine(true), SRCharWidth(true),
	SRTextAddr(0), SRRollX(0), SRRollY(0), SRVramAddrY(0),
	AT_AG(0), AT_AS(0), AT_IE(0), AT_GM(0), AT_CSS(0), AT_INV(0) {}

MC6847::MC6847( void ){}

PCZ80_07::PCZ80_07( void ){}

PCZ80_12::PCZ80_12( void )
{
	SRmode    = true;
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
MC6847core::~MC6847core( void ){}

MC6847::~MC6847( void ){}

PCZ80_07::~PCZ80_07( void ){}

PCZ80_12::~PCZ80_12( void ){}


////////////////////////////////////////////////////////////////
// バックバッファ幅取得(規定値)
////////////////////////////////////////////////////////////////
int MC6847core::GetW( void ) const
{
	return WBBUF;
}


////////////////////////////////////////////////////////////////
// バックバッファ高さ取得(規定値)
////////////////////////////////////////////////////////////////
int MC6847core::GetH( void ) const
{
	return HBBUF;
}


////////////////////////////////////////////////////////////////
// セミグラ4データ取得
////////////////////////////////////////////////////////////////
BYTE MC6847core::GetSemi4( BYTE data ) const
{
	volatile BYTE bdat = (data<<(4+(RowCntA/6)*2)&0x80) | (data<<(1+(RowCntA/6)*2)&0x08);
	bdat |= bdat>>1;
	bdat |= bdat>>2;
	return bdat;
}


////////////////////////////////////////////////////////////////
// セミグラ6データ取得
////////////////////////////////////////////////////////////////
BYTE MC6847core::GetSemi6( BYTE data ) const
{
	volatile BYTE bdat = (data<<(2+(RowCntA/4)*2)&0x80) | (((data<<(3+(RowCntA/4)*2))>>4)&0x08);
	bdat |= bdat>>1;
	bdat |= bdat>>2;
	return bdat;
}

BYTE PCZ80_07::GetSemi6( BYTE data ) const
{
	return GetFont1( 0x1000 + (data&0x3f)*16 + RowCntA );	// セミグラフォントは 1000H-13FFH
}

BYTE PCZ80_12::GetSemi6( BYTE data ) const
{
	return MC6847core::GetSemi6( data );	// SRは初代と同様に生成
}


////////////////////////////////////////////////////////////////
// セミグラ8データ取得
////////////////////////////////////////////////////////////////
BYTE PCZ80_12::GetSemi8( BYTE data ) const
{
	volatile BYTE bdat = ((data<<(RowCntA&0xe))&0x80) | (((data<<(RowCntA&0xe))>>3)&0x08);
	bdat |= bdat>>1;
	bdat |= bdat>>2;
	return bdat;
}


////////////////////////////////////////////////////////////////
// CRT表示状態取得
////////////////////////////////////////////////////////////////
bool MC6847core::GetCrtDisp( void ) const
{
	return CrtDisp;
}


////////////////////////////////////////////////////////////////
// CRT表示状態設定
////////////////////////////////////////////////////////////////
void MC6847core::SetCrtDisp( bool st )
{
	CrtDisp = st;
}


////////////////////////////////////////////////////////////////
// ウィンドウサイズ取得
////////////////////////////////////////////////////////////////
bool MC6847core::GetWinSize( void ) const
{
	return N60Win;
}


////////////////////////////////////////////////////////////////
// モード4カラーモード取得
////////////////////////////////////////////////////////////////
int MC6847core::GetMode4Color( void ) const
{
	return Mode4Col;
}


////////////////////////////////////////////////////////////////
// モード4カラーモード設定
////////////////////////////////////////////////////////////////
void MC6847core::SetMode4Color( int col )
{
	Mode4Col = col;
}


////////////////////////////////////////////////////////////////
// 色の組合せ指定
////////////////////////////////////////////////////////////////
void MC6847core::SetCss( BYTE data )
{
	PRINTD( VDG_LOG, "[VDG][SetCss] -> %02X : ", data );

	Css1 =  data    &1;
	Css2 = (data>>1)&1;
	Css3 = (data>>2)&1;

	PRINTD( VDG_LOG, "1:%d 2:%d 3:%d\n", Css1, Css2, Css3 );
}


////////////////////////////////////////////////////////////////
// CRTコントローラモード設定
////////////////////////////////////////////////////////////////
void MC6847core::SetCrtControler( BYTE data )
{
	PRINTD( VDG_LOG, "[VDG][62][SetCrtControler] -> %02X\n", data );

	N60Win    = data&2 ? true : false;	// ウィンドウサイズ		true:N60		false:N60m
	CharMode  = data&4 ? true : false;	// 表示モード			true:キャラクタ	false:グラフィック
	GraphMode = data&8 ? true : false;	// グラフィック解像度	true:160*200	false:320*200

	PRINTD( VDG_LOG, " DATA      :%02X\n", data );
	PRINTD( VDG_LOG, " N60Win    :%s\n", N60Win    ? "N60"  : "N60m" );
	PRINTD( VDG_LOG, " CharMode  :%s\n", CharMode  ? "Char" : "Graph" );
	PRINTD( VDG_LOG, " GraphMode :%d\n", GraphMode ? 160    : 320 );
}

void PCZ80_12::SetCrtControler( BYTE data )
{
	PRINTD( VDG_LOG, "[VDG][64][SetCrtControler]" );

	if( SRmode ){	// SRモード
		PRINTD( VDG_LOG, "[SRmode] -> %02X\n", data );

		SRLine204   = data&1 ? false : true;	// グラフィックライン数	true:204ライン	false:200ライン
		SRCharWidth = data&2 ? true : false;	// テキスト文字数		true:40文字		false:80文字
		CharMode    = data&4 ? true : false;	// テキスト表示モード	true:キャラクタ	false:グラフィック
		GraphMode   = data&8 ? true : false;	// グラフィック解像度	true:320*200	false:640*200

		PRINTD( VDG_LOG, " Lines       :%d\n", SRLine204   ? 204    : 200 );
		PRINTD( VDG_LOG, " SRTextWidth :%d\n", SRCharWidth ? 40     : 80 );
		PRINTD( VDG_LOG, " CharMode    :%s\n", CharMode    ? "Char" : "Graph" );
		PRINTD( VDG_LOG, " GraphMode   :%d\n", GraphMode   ? 640    : 320 );
	}else{			// 旧モード
		PRINTD( VDG_LOG, "[66mode] -> %02X\n", data );

		N60Win    = data&2 ? true : false;	// ウィンドウサイズ		true:N60		false:N60m
		CharMode  = data&4 ? true : false;	// テキスト表示モード	true:キャラクタ	false:グラフィック
		GraphMode = data&8 ? true : false;	// グラフィック解像度	true:160*200	false:320*200

		PRINTD( VDG_LOG, " N60Win    :%s\n", N60Win    ? "N60"  : "N60m" );
		PRINTD( VDG_LOG, " CharMode  :%s\n", CharMode  ? "Char" : "Graph" );
		PRINTD( VDG_LOG, " GraphMode :%d\n", GraphMode ? 160    : 320 );
	}
}


////////////////////////////////////////////////////////////////
// CRTコントローラタイプ設定
////////////////////////////////////////////////////////////////
void PCZ80_12::SetCrtCtrlType( BYTE data )
{
	PRINTD( VDG_LOG, "[VDG][64][SetCrtCtrlType] -> %02X\n", data );

	SRmode     = data&0x01 ? false : true;	// SRモードフラグ 		true:SR-BASIC	false:旧BASIC
	SRBusReq   = data&0x02 ? false : true;	// バスリクエスト		true:ON			false:OFF
	SRCharLine = data&0x04 ? true  : false;	// SRテキスト行数		true:20行		false:25行
	SRBitmap   = data&0x08 ? false : true;	// SRビットマップフラグ	true:有効		false:無効
	SRBMPage   = data&0x10 ? true  : false;	// SRビットマップページ	true:上位32KB	false:下位32KB

	PRINTD( VDG_LOG, " BasicMode    :%s\n",   SRmode     ? "SR"     : "66" );
	PRINTD( VDG_LOG, " SRBusReqest  :%s\n",   SRBusReq   ? "ON"     : "OFF" );
	PRINTD( VDG_LOG, " SRTextLines  :%d\n",   SRCharLine ? 20       : 25 );
	PRINTD( VDG_LOG, " SRBitmapMode :%s\n",   SRBitmap   ? "Bitmap" : "Text" );
	PRINTD( VDG_LOG, " SRBitmapPage :%04X\n", SRBMPage   ? 0x8000   : 0x0000 );
}


////////////////////////////////////////////////////////////////
// バックバッファ更新
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void MC6847::UpdateBackBuf( void )
{
	PRINTD( VDG_LOG, "[VDG][60][UpdateBackBuf]\n" );

	#if INBPP == 8	// 8bit
	BYTE bcol;
	BYTE *doff   = (BYTE *)GetBufAddr();
	const int pt = GetBufPitch();
	#else			// 32bit
	DWORD bcol;
	DWORD *doff  = (DWORD *)GetBufAddr();
	const int pt = GetBufPitch() / sizeof(DWORD);
	#endif

	VAddr = HAddr = RowCntA = RowCntG = 0;

	const int tbline = TB60 * pt;
	const int bbline = BB60 * pt;

	// 上側ボーダ描画
	bcol = GetBcol();
	for( int i=0; i<tbline; i++ ) *doff++ = bcol;

	// 表示エリア描画
	for( int i=0; i<P60H; i++ ) Draw1line1( i );

	// 下側ボーダ描画
	doff += P60H * pt;
	bcol = GetBcol();
	for( int i=0; i<bbline; i++ ) *doff++ = bcol;
}

void PCZ80_07::UpdateBackBuf( void )
{
	PRINTD( VDG_LOG, "[VDG][62][UpdateBackBuf]\n" );

	#if INBPP == 8	// 8bit
	BYTE bcol;
	BYTE *doff   = (BYTE *)GetBufAddr();
	const int pt = GetBufPitch();
	#else			// 32bit
	DWORD bcol;
	DWORD *doff  = (DWORD *)GetBufAddr();
	const int pt = GetBufPitch() / sizeof(DWORD);
	#endif

	VAddr = HAddr = RowCntA = 0;

	bcol = GetBcol();
	if( N60Win ){	// N60
		const int tbline = TB60 * pt;
		const int bbline = BB60 * pt;

		// 上側ボーダ描画
		for( int i=0; i<tbline; i++ ) *doff++ = bcol;

		// 表示エリア描画
		for( int i=0; i<P60H; i++ ) Draw1line1( i );

		// 下側ボーダ描画
		doff += P60H * pt;
		bcol = GetBcol();
		for( int i=0; i<bbline; i++ ) *doff++ = bcol;
	}else{			// N60m
		const int tbline = TB62 * pt;
		const int bbline = BB62 * pt;

		// 上側ボーダ描画
		for( int i=0; i<tbline; i++ ) *doff++ = bcol;

		// 表示エリア描画
		for( int i=0; i<P62H; i++ ) Draw1line2( i );

		// 下側ボーダ描画
		doff += P62H * pt;
		bcol = GetBcol();
		for( int i=0; i<bbline; i++ ) *doff++ = bcol;
	}
}

void PCZ80_12::UpdateBackBuf( void )
{
	PRINTD( VDG_LOG, "[VDG][64][UpdateBackBuf]\n" );

	#if INBPP == 8	// 8bit
	BYTE bcol;
	BYTE *doff   = (BYTE *)GetBufAddr();
	const int pt = GetBufPitch();
	#else			// 32bit
	DWORD bcol;
	DWORD *doff  = (DWORD *)GetBufAddr();
	const int pt = GetBufPitch() / sizeof(DWORD);
	#endif

	VAddr = HAddr = RowCntA = 0;

	bcol = GetBcol();
	if( SRmode ){	// SRモード
		const int tbline = TB64 * pt;
		const int bbline = BB64 * pt;

		// 上側ボーダ描画
		for( int i=0; i<tbline; i++ ) *doff++ = bcol;

		// 表示エリア描画
		for( int i=0; i<P64H; i++ ) Draw1line3( i );

		// 下側ボーダ描画
		doff += P64H * pt;
		bcol = GetBcol();
		for( int i=0; i<bbline; i++ ) *doff++ = bcol;
	}else{			// 旧モード
		if( N60Win ){	// N60
			const int tbline  = TB60 * pt;
			const int bbline  = BB60 * pt;

			// 上側ボーダ描画
			for( int i=0; i<tbline; i++ ) *doff++ = bcol;

			// 表示エリア描画
			for( int i=0; i<P60H; i++ ) Draw1line1( i );

			// 下側ボーダ描画
			doff += P60H * pt;
			bcol = GetBcol();
			for( int i=0; i<bbline; i++ ) *doff++ = bcol;
		}else{			// N60m
			const int tbline  = TB62 * pt;
			const int bbline  = BB62 * pt;

			// 上側ボーダ描画
			for( int i=0; i<tbline; i++ ) *doff++ = bcol;

			// 表示エリア描画
			for( int i=0; i<P62H; i++ ) Draw1line2( i );

			// 下側ボーダ描画
			doff += P62H * pt;
			bcol = GetBcol();
			for( int i=0; i<bbline; i++ ) *doff++ = bcol;
		}
	}
}


////////////////////////////////////////////////////////////////
// 1ライン描画(N60)
//
// 引数:	line	描画するライン番号(0-191)
// 返値:	なし
////////////////////////////////////////////////////////////////
void MC6847::Draw1line1( int line )
{
	if( !line ){ PRINTD( VDG_LOG, "[VDG][Draw1line1] %d\n", line ); }

	BYTE data=0xff, prevdata=0, nextdata=0, bdat=0;
	BYTE LAT_AG=0, LAT_GM=0;

	#if INBPP == 8	// 8bit
	BYTE wdat, fg=0, bg=0;
	BYTE *doff  = (BYTE *)GetBufAddr()  + ( TB60 + line ) * GetBufPitch();
	#else			// 32bit
	DWORD wdat, fg=0, bg=0;
	DWORD *doff = (DWORD *)GetBufAddr() + ( TB60 + line ) * GetBufPitch() / sizeof(DWORD);
	#endif

	// 左側ボーダー描画
	wdat = GetBcol();
	for( int i=0; i<LB60; i++ ) *doff++ = wdat;

	// 表示エリア描画
	for( int x=0; x<( P60W / 8 ); x++ ){
		if( !CrtDisp ){
			data = AT_AG ? rand() : 0;	// ホント？ 後でどうにかする
		}else{
			LatchAttr();
			// 1バイト前のデータ(色にじみ算出に使用)
			prevdata = data;
			data = GetVram();
			HAddr++;
			// 1バイト後のデータ(色にじみ算出に使用)
			nextdata = ( x < (P60W/8-1) ) ? GetVram() : 0xff;
			LAT_AG |= AT_AG;				// とりあえず
			if( x == 2 ) LAT_GM = AT_GM;	// とりあえず

			// 1byte先のグラフィックモード取得
			if( LAT_AG ) LatchGMODE();
				// 初代機のVDG(周辺回路?)にはバグがあるらしい
				// アトリビュートアドレスに対しカラーセットは正しく読めるが
				// モード判定の時には1byte先のアドレスから読んでしまうようだ
				// 右端の場合はそのラインの先頭(左端)のデータを読むらしい
				// mk2,66ではどちらも正しく読める
		}

		if( AT_AG ){	// グラフィック
			switch( AT_GM ){
			case GM_CG1:	//  64x 64 カラー   (CG1)
				for( int i=3; i>=0; i-- ){
					#if INBPP == 8	// 8bit
					wdat = COL_CG[AT_CSS][(data>>(i*2))&3];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_CG[AT_CSS][(data>>(i*2))&3] );
					#endif
					for( int j=0; j<4; j++ ) *doff++ = wdat;
				}
				x++;
				break;

			case GM_CG2:	// 128x 64 カラー   (CG2)
			case GM_CG3:	// 128x 96 カラー   (CG3)
			case GM_CG6:	// 128x192 カラー   (CG6)
				for( int i=6; i>=0; i-=2 ){
					#if INBPP == 8	// 8bit
					wdat = COL_CG[AT_CSS][(data>>i)&3];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_CG[AT_CSS][(data>>i)&3] );
					#endif
					*doff++ = wdat;
					*doff++ = wdat;
				}
				break;

			case GM_RG1:	// 128x 64 モノクロ (RG1)
			case GM_RG2:	// 128x 96 モノクロ (RG2)
			case GM_RG3:	// 128x192 モノクロ (RG3)
				for( int i=7; i>=0; i-- ){
					#if INBPP == 8	// 8bit
					wdat = COL_RG[AT_CSS][(data>>i)&1];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_RG[AT_CSS][(data>>i)&1] );
					#endif
					*doff++ = wdat;
					*doff++ = wdat;
				}
				x++;
				break;

			case GM_RG6:	// 256x192 モノクロ (RG6)
				if( Mode4Col ){	// 128x192 カラー(にじみ)
					// VRAM上の連続した3バイト
					DWORD data3 = prevdata << 16 | data << 8 | nextdata;
					int CsC = AT_CSS + Mode4Col*2;
					for( int i=6; i>=0; i-=2 ){
						// にじみテーブル上のインデックス
						DWORD nijimiIdx = (data3 & (0x0000003f << (i + 6))) >> (i + 6);
						#if INBPP == 8	// 8bit
						*doff++ = COL_CG[CsC][NJM_TBL[nijimiIdx][0]];
						*doff++ = COL_CG[CsC][NJM_TBL[nijimiIdx][1]];
						#else			// 32bit
						*doff++ = VSurface::GetColor( COL_CG[CsC][NJM_TBL[nijimiIdx][0]] );
						*doff++ = VSurface::GetColor( COL_CG[CsC][NJM_TBL[nijimiIdx][1]] );
						#endif
					}
				}else{			// 256x192 モノクロ
					for( int i=7; i>=0; i-- )
						#if INBPP == 8	// 8bit
						*doff++ = COL_RG[AT_CSS][(data>>i)&1];
						#else			// 32bit
						*doff++ = VSurface::GetColor( COL_RG[AT_CSS][(data>>i)&1] );
						#endif
				}
			}

		}else{			// アルファニューメリック
			switch( ANMODE ){
			case AM_AN0:		// アルファニューメリック(内部フォント)
				#if INBPP == 8	// 8bit
				fg   = COL_AN[(AT_CSS<<1) |   AT_INV];
				bg   = COL_AN[(AT_CSS<<1) | (~AT_INV&1)];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) |   AT_INV] );
				bg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) | (~AT_INV&1)] );
				#endif
				bdat = VDGfont[ (data&0x3f)*16 + RowCntA ];
				break;

			case AM_AN1:		// アルファニューメリック(外部フォント)
				#if INBPP == 8	// 8bit
				fg   = COL_AN[(AT_CSS<<1) |   AT_INV];
				bg   = COL_AN[(AT_CSS<<1) | (~AT_INV&1)];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) |   AT_INV] );
				bg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) | (~AT_INV&1)] );
				#endif
				bdat = GetFont1( data*16 + RowCntA );
				break;

			case AM_SG4:		// セミグラフィック4
				#if INBPP == 8	// 8bit
				fg   = COL_SG[(data>>4)&7];
				bg   = COL_SG[8];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_SG[(data>>4)&7] );
				bg   = VSurface::GetColor( COL_SG[8] );
				#endif
				bdat = GetSemi4( data );
				break;

			case AM_SG6:		// セミグラフィック6
				#if INBPP == 8	// 8bit
				fg   = COL_SG[(AT_CSS<<2) | ((data>>6)&3)];
				bg   = COL_SG[8];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_SG[(AT_CSS<<2) | ((data>>6)&3)] );
				bg   = VSurface::GetColor( COL_SG[8] );
				#endif
				bdat = GetSemi6( data );
			}
			for( int i=7; i>=0; i-- )
				*doff++ = (bdat>>i)&1 ? fg : bg;
		};
	}


	// メモリアドレスオフセット求める
	//   どうやら各ライン3byte目の表示モードで決まってくるらしい
	//   ただし同一ラインに異なるモードが混在するときは挙動が変わる
	if( LAT_AG ){	// グラフィック
		switch( LAT_GM ){
		case GM_CG1:	//  64x 64 カラー   (CG1)
		case GM_CG2:	// 128x 64 カラー   (CG2)
		case GM_RG1:	// 128x 64 モノクロ (RG1)
			if( RowCntG++ == 2 ){
				VAddr++;
				RowCntG = 0;
			}
			break;
		case GM_CG3:	// 128x 96 カラー   (CG3)
		case GM_RG2:	// 128x 96 モノクロ (RG2)
			if( RowCntG++ == 1 ){
				VAddr++;
				RowCntG = 0;
			}
			break;
		case GM_CG6:	// 128x192 カラー   (CG6)
		case GM_RG3:	// 128x192 モノクロ (RG3)
		case GM_RG6:	// 256x192 モノクロ (RG6)
			VAddr++;
		}
		RowCntA = 0;
	}else{			// アルファニューメリック
		if( ++RowCntA == 12 ){
			VAddr++;
			RowCntA = 0;
		}
		RowCntG = 0;
	}
	HAddr = 0;

	// 右側ボーダー描画
	wdat = GetBcol();
	for( int i=0; i<RB60; i++ ) *doff++ = wdat;
}

void PCZ80_07::Draw1line1( int line )
{
	if( !line ){ PRINTD( VDG_LOG, "[VDG][Draw1line1] %d\n", line ) };

	BYTE data=0, prevdata=0, nextdata=0, bdat=0;
	BYTE LAT_AG=0;

	#if INBPP == 8	// 8bit
	BYTE wdat, fg=0, bg=0;
	BYTE *doff  = (BYTE *)GetBufAddr()  + ( TB60 + line ) * GetBufPitch();
	#else			// 32bit
	DWORD wdat, fg=0, bg=0;
	DWORD *doff = (DWORD *)GetBufAddr() + ( TB60 + line ) * GetBufPitch() / sizeof(DWORD);
	#endif

	// 左側ボーダー描画
	wdat = GetBcol();
	for( int i=0; i<LB60; i++ ) *doff++ = wdat;

	// 表示エリア描画
	for( int x=0; x<( P60W / 8 ); x++ ){
		if( !CrtDisp ){
			data = 0;	// ホント？RGB出力とビデオ出力で異なるはず
		}else{
			LatchAttr();
			// 1バイト前のデータ(色にじみ算出に使用)
			prevdata = data;
			data = GetVram();
			HAddr++;
			// 1バイト後のデータ(色にじみ算出に使用)
			nextdata = ( x < (P60W/8-1) ) ? GetVram() : 0;
			LAT_AG |= AT_AG;				// とりあえず
		}

		if( AT_AG ){	// グラフィック
			switch( AT_GM ){
			case GM_CG1:	//  64x 64 カラー   (CG1)(無効)
			case GM_CG2:	// 128x 64 カラー   (CG2)(無効)
			case GM_CG3:	// 128x 96 カラー   (CG3)(無効)
			case GM_CG6:	// 128x192 カラー   (CG6)
				for( int i=6; i>=0; i-=2 ){
					#if INBPP == 8	// 8bit
					wdat = COL_CG[AT_CSS][(data>>i)&3];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_CG[AT_CSS][(data>>i)&3] );
					#endif
					*doff++ = wdat;
					*doff++ = wdat;
				}
				break;

			case GM_RG1:	// 128x 64 モノクロ (RG1)(無効)
			case GM_RG2:	// 128x 96 モノクロ (RG2)(無効)
			case GM_RG3:	// 128x192 モノクロ (RG3)(無効)
			case GM_RG6:	// 256x192 モノクロ (RG6)
				if( Mode4Col ){	// 128x192 カラー(にじみ)
					// VRAM上の連続した3バイト
					DWORD data3 = prevdata << 16 | data << 8 | nextdata;
					int CsC = AT_CSS + Mode4Col*2;
					for( int i=6; i>=0; i-=2 ){
						// にじみテーブル上のインデックス
						DWORD nijimiIdx = (data3 & (0x0000003f << (i + 6))) >> (i + 6);
						#if INBPP == 8	// 8bit
						*doff++ = COL_CG[CsC][NJM_TBL[nijimiIdx][0]];
						*doff++ = COL_CG[CsC][NJM_TBL[nijimiIdx][1]];
						#else			// 32bit
						*doff++ = VSurface::GetColor( COL_CG[CsC][NJM_TBL[nijimiIdx][0]] );
						*doff++ = VSurface::GetColor( COL_CG[CsC][NJM_TBL[nijimiIdx][1]] );
						#endif
					}
				}else{			// 256x192 モノクロ
					for( int i=7; i>=0; i-- )
						#if INBPP == 8	// 8bit
						*doff++ = COL_RG[AT_CSS][(data>>i)&1];
						#else			// 32bit
						*doff++ = VSurface::GetColor( COL_RG[AT_CSS][(data>>i)&1] );
						#endif
				}
			}
		}else{			// アルファニューメリック
			switch( ANMODE ){
			case AM_AN0:		// アルファニューメリック(内部フォント)(無効)
			case AM_AN1:		// アルファニューメリック(外部フォント)
				#if INBPP == 8	// 8bit
				fg   = COL_AN[(AT_CSS<<1) |   AT_INV];
				bg   = COL_AN[(AT_CSS<<1) | (~AT_INV&1)];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) |   AT_INV] );
				bg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) | (~AT_INV&1)] );
				#endif
				bdat = GetFont1( data*16 + RowCntA );
				break;

			case AM_SG4:		// セミグラフィック4(無効)
			case AM_SG6:		// セミグラフィック6
				#if INBPP == 8	// 8bit
				fg   = COL_SG[(AT_CSS<<2) | ((data>>6)&3)];
				bg   = COL_SG[8];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_SG[(AT_CSS<<2) | ((data>>6)&3)] );
				bg   = VSurface::GetColor( COL_SG[8] );
				#endif
				bdat = GetSemi6( data );
			}
			for( int i=7; i>=0; i-- )
				*doff++ = (bdat>>i)&1 ? fg : bg;
		};
	}


	// メモリアドレスオフセット求める
	//   どうやら各ライン3byte目の表示モードで決まってくるらしい
	//   ただし同一ラインに異なるモードが混在するときは挙動が変わる
	//   mk,66も同じかどうかは不明だがもちょっとシンプルだと思われる
	if( LAT_AG ){	// グラフィック
		VAddr++;
		RowCntA = 0;
	}else{			// アルファニューメリック
		if( ++RowCntA == 12 ){
			VAddr++;
			RowCntA = 0;
		}
	}
	HAddr = 0;

	// 右側ボーダー描画
	wdat = GetBcol();
	for( int i=0; i<RB60; i++ ) *doff++ = wdat;
}


////////////////////////////////////////////////////////////////
// 1ライン描画(N60m)
//
// 引数:	line	描画するライン番号(0-199)
// 返値:	なし
////////////////////////////////////////////////////////////////
void PCZ80_07::Draw1line2( int line )
{
	if( !line ){ PRINTD( VDG_LOG, "[VDG][Draw1line2] %d %s\n", line, CharMode ? "Char" : GraphMode ? "Graph3" : "Graph4" ); }

	BYTE attr, data, bdat;

	#if INBPP == 8	// 8bit
	BYTE wdat, fg, bg;
	BYTE *doff  = (BYTE *)GetBufAddr()  + ( TB62 + line ) * GetBufPitch();
	#else			// 32bit
	DWORD wdat, fg, bg;
	DWORD *doff = (DWORD *)GetBufAddr() + ( TB62 + line ) * GetBufPitch() / sizeof(DWORD);
	#endif

	// 左側ボーダー描画
	wdat = GetBcol();
	for( int i=0; i<LB62; i++ ) *doff++ = wdat;

	// 表示エリア描画
	for( int x=0; x<( P62W / 8 ); x++){
		if( !CrtDisp ){
			attr = 0;	// ホント？
			data = 0;	// ホント？
		}else{
			attr = GetAttr();
			data = GetVram();
			HAddr++;
		}

		if( CharMode ){	// キャラクタモード
			#if INBPP == 8	// 8bit
			fg   = COL_AN2[attr&0x0f];
			bg   = COL_AN2[((attr>>4)&0x07)|((Css2&1)<<3)];
			#else			// 32bit
			fg   = VSurface::GetColor( COL_AN2[attr&0x0f] );
			bg   = VSurface::GetColor( COL_AN2[((attr>>4)&0x07)|((Css2&1)<<3)] );
			#endif
			bdat = GetFont2( (data+((attr&0x80)?256:0))*16 + RowCntA );
			for( int i=7; i>=0; i-- )
				*doff++ = (bdat>>i)&1 ? fg : bg;
		}else{				// グラフィックモード
			if( GraphMode ){	// グラフィック モード3
				for( int i=6; i>=0; i-=2 ){
					#if INBPP == 8	// 8bit
					wdat = COL_CG2[Css3][(((data<<2)>>i)&0x0c)|((attr>>i)&3)];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_CG2[Css3][(((data<<2)>>i)&0x0c)|((attr>>i)&3)] );
					#endif
					*doff++ = wdat;
					*doff++ = wdat;
				}
			}else{				// グラフィック モード4
				BYTE c = (Css2<<3)|(Css1<<2);
				for( int i=7; i>=0; i-- )
					#if INBPP == 8	// 8bit
					*doff++ = COL_CG2[Css3][c|(((data<<1)>>i)&2)|((attr>>i)&1)];
					#else			// 32bit
					*doff++ = VSurface::GetColor( COL_CG2[Css3][c|(((data<<1)>>i)&2)|((attr>>i)&1)] );
					#endif
			}
		}
	}


	// メモリアドレスオフセット求める
	if( CharMode ){	// アルファニューメリック
		if( ++RowCntA == 10 ){
			VAddr++;
			RowCntA = 0;
		}
	}else{				// グラフィック
		VAddr++;
		RowCntA = 0;
	}
	HAddr = 0;

	// 右側ボーダー描画
	wdat = GetBcol();
	for( int i=0; i<RB62; i++ ) *doff++ = wdat;
}


////////////////////////////////////////////////////////////////
// 1ライン描画(SR)
//
// 引数:	line	描画するライン番号(0-204)
// 返値:	なし
////////////////////////////////////////////////////////////////
void PCZ80_12::Draw1line3( int line )
{
	if( !line ){ PRINTD( VDG_LOG, "[VDG][Draw1line3] %d %s\n", line, CharMode ? "Char" : GraphMode ? "Graph2" : "Graph3" ); }

	BYTE attr, data, bdat;
	BYTE attr1=0, data1=0, scrl1=0, scrl2=0;

	#if INBPP == 8	// 8bit
	BYTE wdat, fg, bg;
	BYTE *doff  = (BYTE *)GetBufAddr()  + ( TB64 + line ) * GetBufPitch();
	#else			// 32bit
	DWORD wdat, fg, bg;
	DWORD *doff = (DWORD *)GetBufAddr() + ( TB64 + line ) * GetBufPitch() / sizeof(DWORD);
	#endif

	const int xsc = ((CharMode && !SRCharWidth) || (!CharMode && !GraphMode)) ? 2 : 1;

	// 左側ボーダー描画
	wdat = GetBcol();
	for( int i=0; i<(LB64*xsc); i++ ) *doff++ = wdat;

	// 表示エリア描画
	for( int x=0; x<( P64W / 8 * xsc ); x++){
		if( CharMode ){	// テキストモード
			if( !CrtDisp ){
				attr = 0;	// ホント？
				data = 0;	// ホント？
			}else{
				data = GetVram();
				HAddr++;
				attr = GetVram();
				HAddr++;
			}

			#if INBPP == 8	// 8bit
			fg   = COL_AN2[attr&0x0f];
			bg   = COL_AN2[((attr>>4)&0x07)|((Css2&1)<<3)];
			#else			// 32bit
			fg   = VSurface::GetColor( COL_AN2[attr&0x0f] );
			bg   = VSurface::GetColor( COL_AN2[((attr>>4)&0x07)|((Css2&1)<<3)] );
			#endif
			bdat = SRCharLine  ? GetFont2( (data+((attr&0x80)?256:0))*16 + RowCntA ) :	// 20行
				   attr & 0x80 ? GetSemi8( data ) :										// 25行 セミグラモード
								 GetFont3( (data+((attr&0x80)?256:0))*16 + RowCntA );	// 25行 キャラクタモード

			for( int i=7; i>=0; i-- )
				*doff++ = (bdat>>i)&1 ? fg : bg;

		}else{			// ビットマップモード
			if( GraphMode ){	// グラフィック モード2
				BYTE d1;
				if( (SRRollX&1) && (x == 0) ){
					data1 = GetVram();
					HAddr += 2;

					scrl1 = data1<<4;
					data1 >>= 4;
				}

				for( int i=0; i<4; i++ ){
					if( !CrtDisp ){
						data = 0;	// ホント？
					}else if( SRRollX&1 ){
						if( HAddr < 320 ){
							d1 = GetVram();
							HAddr += 2;

							data  = data1 | (d1<<4);
							data1 = d1>>4;
						}else{
							data = data1 | scrl1;
						}
					}else{
						data = GetVram();
						HAddr += 2;
					}
					#if INBPP == 8	// 8bit
					*doff++ = COL_CG2[Css3][ data    &0x0f];
					*doff++ = COL_CG2[Css3][(data>>4)&0x0f];
					#else			// 32bit
					*doff++ = VSurface::GetColor( COL_CG2[Css3][ data    &0x0f] );
					*doff++ = VSurface::GetColor( COL_CG2[Css3][(data>>4)&0x0f] );
					#endif
				}
			}else{				// グラフィック モード3
				if( !CrtDisp ){
					attr = 0;	// ホント？
					data = 0;	// ホント？
				}else if( SRRollX&3 ){	// スクロールあり
					BYTE d1, d2;
					if( x == 0 ){
						data1 = GetVram(); HAddr += 2;
						attr1 = GetVram(); HAddr += 2;

						scrl1 = data1 >> (8-((SRRollX&3)<<1));
						scrl2 = attr1 >> (8-((SRRollX&3)<<1));

						data1 <<= ((SRRollX&3)<<1);
						attr1 <<= ((SRRollX&3)<<1);
					}
					if( HAddr < 320 ){
						d1 = GetVram(); HAddr += 2;
						d2 = GetVram(); HAddr += 2;

						data = data1 | (d1>>(8-((SRRollX&3)<<1)));
						attr = attr1 | (d2>>(8-((SRRollX&3)<<1)));

						data1 = d1 << ((SRRollX&3)<<1);
						attr1 = d2 << ((SRRollX&3)<<1);
					}else{
						data = data1 | scrl1;
						attr = attr1 | scrl2;
					}
				}else{				// スクロールなし
					data = GetVram(); HAddr += 2;
					attr = GetVram(); HAddr += 2;
				}

				for( int i=0; i<8; i++ )
					#if INBPP == 8	// 8bit
					*doff++ =                     COL_CG2[Css3][((Css2&1)<<3)|((Css1&1)<<2)|(((attr>>(7-i))<<1)&2)|((data>>(7-i))&1)];
					#else			// 32bit
					*doff++ = VSurface::GetColor( COL_CG2[Css3][((Css2&1)<<3)|((Css1&1)<<2)|(((attr>>(7-i))<<1)&2)|((data>>(7-i))&1)] );
					#endif
			}
		}
	}

	// メモリアドレスオフセット求める
	if( CharMode ){	// アルファニューメリック
		if( ++RowCntA == (SRCharLine ? 10 : 8) ){
			VAddr++;
			RowCntA = 0;
		}
	}else{				// グラフィック
		VAddr++;
		RowCntA = 0;
	}
	HAddr = 0;

	// 右側ボーダー描画
	wdat = GetBcol();
	for( int i=0; i<(RB64*xsc); i++ ) *doff++ = wdat;
}


////////////////////////////////////////////////////////////////
// ボーダーカラー取得
//
// 引数:	なし
// 返値:	BYTE	ボーダーカラーコード
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
BYTE MC6847::GetBcol( void ) const
{
	BYTE bcol;

	if( AT_AG ){	// グラフィックモード
		if( AT_GM & 4 )	bcol = COL_RG[AT_CSS][1];	// モノクロ
		else			bcol = COL_CG[AT_CSS][0];	// カラー
	}else{			// アルファニューメリックモード
		if( AT_AS )		bcol = COL_SG[8];			// セミグラフィック
		else			bcol = COL_AN[4];			// アルファニューメリック
	}

	return bcol;
}

BYTE PCZ80_07::GetBcol( void ) const
{
	BYTE bcol;

	// 実際は全部同じかも
	if( SRmode ){
		if( CharMode )		bcol = COL_AN2[0];		// キャラクタモード
		else				bcol = COL_CG2[0][0];	// グラフィック
	}else{
		if( N60Win )		bcol = COL_AN[4];		// N60
		else{										// N60m
			if( CharMode )	bcol = COL_AN2[0];		// キャラクタモード
			else			bcol = COL_CG2[0][0];	// グラフィック
		}
	}

	return bcol;
}
#else			// 32bit
DWORD MC6847::GetBcol( void ) const
{
	DWORD bcol;

	if( AT_AG ){	// グラフィックモード
		if( AT_GM & 4 )	bcol = VSurface::GetColor( COL_RG[AT_CSS][1] );	// モノクロ
		else			bcol = VSurface::GetColor( COL_CG[AT_CSS][0] );	// カラー
	}else{			// アルファニューメリックモード
		if( AT_AS )		bcol = VSurface::GetColor( COL_SG[8] );			// セミグラフィック
		else			bcol = VSurface::GetColor( COL_AN[4] );			// アルファニューメリック
	}

	return bcol;
}

DWORD PCZ80_07::GetBcol( void ) const
{
	DWORD bcol;

	// 実際は全部同じかも
	if( SRmode ){
		if( CharMode )		bcol = VSurface::GetColor( COL_AN2[0] );	// キャラクタモード
		else				bcol = VSurface::GetColor( COL_CG2[0][0] );	// グラフィック
	}else{
		if( N60Win )		bcol = VSurface::GetColor( COL_AN[4] );		// N60
		else{															// N60m
			if( CharMode )	bcol = VSurface::GetColor( COL_AN2[0] );	// キャラクタモード
			else			bcol = VSurface::GetColor( COL_CG2[0][0] );	// グラフィック
		}
	}

	return bcol;
}
#endif


////////////////////////////////////////////////////////////////
// パレット設定
////////////////////////////////////////////////////////////////
void PCZ80_12::SetPalette( int col, BYTE dat )
{
	PRINTD( VDG_LOG, "[VDG][SetPalette] %d ->%d\n", col, dat );

	int tpal  =   (col&8) | ((col&4)>>1) | ((col&2)>>1) | ((col&1)<<2);
	BYTE pdat = ( (dat&8) | ((dat&4)>>1) | ((dat&2)>>1) | ((dat&1)<<2) ) + 65;

	COL_AN2[tpal]   = pdat;
	COL_CG2[0][col] = pdat;
	COL_CG2[1][col] = pdat;
}
