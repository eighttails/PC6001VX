#include <stdlib.h>

#include "../log.h"
#include "mc6847.h"


#define	P6WIDTH		(256)	/* 水平有効表示期間(N60)     */
#define	P6HEIGHT	(192)	/* 垂直有効表示ライン(N60)   */
#define	P2WIDTH		(320)	/* 水平有効表示期間(N60m)    */
#define	P2HEIGHT	(200)	/* 垂直有効表示ライン(N60m)  */

// バックバッファサイズ
#define	WBBUF		(376-16)	/* 必ず4の倍数 */
#define	HBBUF		(242-12)

// ボーダーサイズ
#define	LB60		((int)((WBBUF-P6WIDTH)/2))
#define	RB60		(WBBUF-P6WIDTH-LB60)
#define	TB60		((int)((HBBUF-P6HEIGHT)/2))
#define	BB60		(HBBUF-P6HEIGHT-TB60)
#define	LB62		((int)((WBBUF-P2WIDTH)/2))
#define	RB62		(WBBUF-P2WIDTH-LB62)
#define	TB62		((int)((HBBUF-P2HEIGHT)/2))
#define	BB62		(HBBUF-P2HEIGHT-TB62)

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
#define AM_AN0		0		/* アルファニューメリック(内部フォント) */
#define AM_AN1		1		/* アルファニューメリック(外部フォント) */
#define AM_SG4		2		/* セミグラフィック4 */
#define AM_SG6		3		/* セミグラフィック6 */

#define GM_CG1		0		/*  64x 64 カラー	*/
#define GM_CG2		1		/* 128x 64 カラー	*/
#define GM_CG3		2		/* 128x 96 カラー	*/
#define GM_CG6		3		/* 128x192 カラー	*/
#define GM_RG1		4		/* 128x 64 モノクロ	*/
#define GM_RG2		5		/* 128x 96 モノクロ	*/
#define GM_RG3		6		/* 128x192 モノクロ	*/
#define GM_RG6		7		/* 256x192 モノクロ	*/

//色にじみカラーコードテーブル
//数値はCOL_CG上のインデックス
static const BYTE NIJIMI_TBL[][2] = {
    {0,0}, {0,0}, {0,7}, {0,7},
    {5,4}, {5,4}, {5,4}, {5,4},
    {6,7}, {6,7}, {6,2}, {6,7},
    {6,4}, {6,4}, {6,3}, {6,3},
    {5,0}, {5,0}, {5,7}, {5,7},
    {1,4}, {1,1}, {5,4}, {5,4},
    {6,7}, {6,7}, {6,7}, {6,7},
    {3,4}, {3,4}, {3,3}, {3,3},
    {0,0}, {0,0}, {0,7}, {0,7},
    {5,4}, {5,4}, {5,4}, {5,4},
    {6,7}, {6,7}, {2,2}, {2,7},
    {6,4}, {6,4}, {6,3}, {6,3},
    {5,0}, {5,0}, {5,7}, {5,7},
    {5,4}, {5,1}, {5,4}, {5,4},
    {6,7}, {6,7}, {6,7}, {6,7},
    {3,4}, {3,4}, {3,3}, {3,3}
};
//黒　0
//白　3
//赤/桃　明←6 2 7→暗
//青/緑　明←4 1 5→暗

////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cMC6847core::cMC6847core( void ) :
	CrtDisp(true), N60Win(true),
	Mode4Col(0), VAddr(0), HAddr(0), RowCntA(0), RowCntG(0),
	AT_AG(0), AT_AS(0), AT_IE(0), AT_GM(0), AT_CSS(0), AT_INV(0) {}

cMC6847_1::cMC6847_1( void ){}

cMC6847_2::cMC6847_2( void ) : 
	Mk2CharMode(true), Mk2GraphMode(false), Css1(1), Css2(1), Css3(1) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cMC6847core::~cMC6847core( void ){}

cMC6847_1::~cMC6847_1( void ){}

cMC6847_2::~cMC6847_2( void ){}


////////////////////////////////////////////////////////////////
// バックバッファ幅取得(規定値)
////////////////////////////////////////////////////////////////
int cMC6847core::GetW( void )
{
	return WBBUF;
}


////////////////////////////////////////////////////////////////
// バックバッファ高さ取得(規定値)
////////////////////////////////////////////////////////////////
int cMC6847core::GetH( void )
{
	return HBBUF;
}


////////////////////////////////////////////////////////////////
// CRT表示状態設定
////////////////////////////////////////////////////////////////
void cMC6847core::SetCrtDisp( bool st )
{
	CrtDisp = st;
}


////////////////////////////////////////////////////////////////
// ウィンドウサイズ取得
////////////////////////////////////////////////////////////////
bool cMC6847core::GetWinSize( void )
{
	return N60Win;
}


////////////////////////////////////////////////////////////////
// モード4カラーモード取得
////////////////////////////////////////////////////////////////
int cMC6847core::GetMode4Color( void )
{
	return Mode4Col;
}


////////////////////////////////////////////////////////////////
// モード4カラーモード設定
////////////////////////////////////////////////////////////////
void cMC6847core::SetMode4Color( int col )
{
	Mode4Col = col;
}


////////////////////////////////////////////////////////////////
// バックバッファ更新
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void cMC6847_1::UpdateBackBuf( void )
{
	PRINTD( GRP_LOG, "[VDG][60][UpdateBackBuf]\n" );
	
	BYTE *doff = GetBufAddr();
	
	VAddr = HAddr = RowCntA = RowCntG = 0;
	
	// 上側ボーダ描画
	for( int i=0; i<TB60*GetBufPitch(); i++ ) *(doff++) = GetBcol();
	
	// 表示エリア描画
	for( int i=0; i<P6HEIGHT; i++ ) Draw1line1( i );
	
	// 下側ボーダ描画
	doff = GetBufAddr() + ( TB60 + P6HEIGHT ) * GetBufPitch();
	for( int i=0; i<BB60*GetBufPitch(); i++ ) *(doff++) = GetBcol();
}

void cMC6847_2::UpdateBackBuf( void )
{
	PRINTD( GRP_LOG, "[VDG][62][UpdateBackBuf]\n" );
	
	BYTE *doff = GetBufAddr();
	
	VAddr = HAddr = RowCntA = 0;
	
	if( N60Win ){	// N60
		// 上側ボーダ描画
		for( int i=0; i<TB60*GetBufPitch(); i++ ) *(doff++) = GetBcol();
		
		// 表示エリア描画
		for( int i=0; i<P6HEIGHT; i++ ) Draw1line1( i );
		
		// 下側ボーダ描画
		doff = GetBufAddr() + ( TB60 + P6HEIGHT ) * GetBufPitch();
		for( int i=0; i<BB60*GetBufPitch(); i++ ) *(doff++) = GetBcol();
	}else{			// N60m
		// 上側ボーダ描画
		for( int i=0; i<TB62*GetBufPitch(); i++ ) *(doff++) = GetBcol();
		
		// 表示エリア描画
		for( int i=0; i<P2HEIGHT; i++ ) Draw1line2( i );
		
		// 下側ボーダ描画
		doff = GetBufAddr() + ( TB62 + P2HEIGHT ) * GetBufPitch();
		for( int i=0; i<BB62*GetBufPitch(); i++ ) *(doff++) = GetBcol();
	}
}


////////////////////////////////////////////////////////////////
// 1ライン描画(N60)
//
// 引数:	line	描画するライン番号(0-191)
// 返値:	なし
////////////////////////////////////////////////////////////////
void cMC6847_1::Draw1line1( int line )
{
    BYTE data, fdat=0, prevdata=0, nextdata=0, fg=0, bg=0;
	BYTE LAT_AG=0, LAT_GM=0;
	
	// バックバッファアドレス求める
	BYTE *doff = GetBufAddr() + ( TB60 + line ) * GetBufPitch();
	
	// 左側ボーダー描画
	for( int i=0; i<LB60; i++ ) *(doff++) = GetBcol();
	
	// 表示エリア描画
	for( int x=0; x<( P6WIDTH / 8 ); x++ ){
		if( CrtDisp ){
			LatchAttr();
            //1バイト前のデータ(色にじみ算出に使用)
            prevdata = data;
            data = GetVram();
            HAddr++;
            //1バイト後のデータ(色にじみ算出に使用)
            nextdata = GetVram();
			LAT_AG |= AT_AG;				// とりあえず
			if( x == 2 ) LAT_GM = AT_GM;	// とりあえず
			
			// 1byte先のグラフィックモード取得
			if( LAT_AG ) LatchGMODE();
				// 初代機のVDG(周辺回路?)にはバグがあるらしい
				// アトリビュートアドレスに対しカラーセットは正しく読めるが
				// モード判定の時には1byte先のアドレスから読んでしまうようだ
				// 右端の場合はそのラインの先頭(左端)のデータを読むらしい
				// mk2,66ではどちらも正しく読める
		}else{
			data = AT_AG ? rand() : 0;	// ホント？ 後でどうにかする
		}
		
		if( AT_AG ){	// グラフィック
			switch( AT_GM ){
			case GM_CG1:	//  64x 64 カラー   (CG1)
				for( int i=3; i>=0; i-- ){
					fdat = COL_CG[AT_CSS][(data>>(i*2))&3];
					for( int j=0; j<4; j++ ) *(doff++) = fdat;
				}
				x++;
				break;
				
			case GM_CG2:	// 128x 64 カラー   (CG2)
			case GM_CG3:	// 128x 96 カラー   (CG3)
			case GM_CG6:	// 128x192 カラー   (CG6)
				for( int i=6; i>=0; i-=2 ){
					fdat = COL_CG[AT_CSS][(data>>i)&3];
					*(doff++) = fdat;
					*(doff++) = fdat;
				}
				break;
				
			case GM_RG1:	// 128x 64 モノクロ (RG1)
			case GM_RG2:	// 128x 96 モノクロ (RG2)
			case GM_RG3:	// 128x192 モノクロ (RG3)
				for( int i=7; i>=0; i-- ){
					fdat = COL_RG[AT_CSS][(data>>i)&1];
					*(doff++) = fdat;
					*(doff++) = fdat;
				}
				x++;
				break;
				
			case GM_RG6:	// 256x192 モノクロ (RG6)
                if( Mode4Col ){	// 128x192 カラー(にじみ)
                    //VRAM上の連続した3バイト
                    uint32_t data3 = prevdata << 16 | data << 8 | nextdata;
                    int CsC = AT_CSS + Mode4Col*2;
                    for( int i=6; i>=0; i-=2 ){
                        //にじみテーブル上のインデックス
                        uint32_t nijimiIdx = (data3 & (0x0000003F << (i + 6))) >> (i + 6);
                        *(doff++) = COL_CG[CsC][NIJIMI_TBL[nijimiIdx][0]];
                        *(doff++) = COL_CG[CsC][NIJIMI_TBL[nijimiIdx][1]];
                    }
				}else{			// 256x192 モノクロ
					for( int i=7; i>=0; i-- )
						*(doff++) = COL_RG[AT_CSS][(data>>i)&1];
				}
			}
			
		}else{			// アルファニューメリック
			switch( ANMODE ){
			case AM_AN0:		// アルファニューメリック(内部フォント)
				fg   = COL_AN[(AT_CSS<<1) |   AT_INV];
				bg   = COL_AN[(AT_CSS<<1) | (~AT_INV&1)];
				fdat = GetFont0( (data&0x3f)*16 + RowCntA );
				break;
				
			case AM_AN1:		// アルファニューメリック(外部フォント)
				fg   = COL_AN[(AT_CSS<<1) |   AT_INV];
				bg   = COL_AN[(AT_CSS<<1) | (~AT_INV&1)];
				fdat = GetFont1( data*16 + RowCntA );
				break;
				
			case AM_SG4:		// セミグラフィック4
				fg   = COL_SG[(data>>4)&7];
				bg   = COL_SG[8];
				fdat = (data<<(4+(RowCntA/6)*2)&0x80) | (data<<(1+(RowCntA/6)*2)&0x08);
				fdat |= fdat>>1 | fdat>>2 | fdat>>3;
				break;
				
			case AM_SG6:		// セミグラフィック6
				fg   = COL_SG[(AT_CSS<<2) | ((data>>6)&3)];
				bg   = COL_SG[8];
				fdat = (data<<(2+(RowCntA/4)*2)&0x80) | (((data<<(3+(RowCntA/4)*2))>>4)&0x08);
				fdat |= fdat>>1 | fdat>>2 | fdat>>3;
			}
			for( int i=7; i>=0; i-- )
				*(doff++) = (fdat>>i)&1 ? fg : bg;
			
			
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
		if( RowCntA++ == 11 ){
			VAddr++;
			RowCntA = 0;
		}
		RowCntG = 0;
	}
	HAddr = 0;
	
	// 右側ボーダー描画
	doff = GetBufAddr() + ( TB60 + line ) * GetBufPitch() + LB60 + P6WIDTH;
	for( int i=0; i<RB60; i++ ) *(doff++) = GetBcol();
}

void cMC6847_2::Draw1line1( int line )
{
    BYTE data=0, prevdata=0, nextdata=0, fdat=0, fg=0, bg=0;
	BYTE LAT_AG=0;
	
	// バックバッファアドレス求める
	BYTE *doff = GetBufAddr() + ( TB60 + line ) * GetBufPitch();
	
	// 左側ボーダー描画
	for( int i=0; i<LB60; i++ ) *(doff++) = GetBcol();
	
	// 表示エリア描画
	for( int x=0; x<( P6WIDTH / 8 ); x++ ){
		if( CrtDisp ){
			LatchAttr();
            //1バイト前のデータ(色にじみ算出に使用)
            prevdata = data;
			data = GetVram();
			HAddr++;
            //1バイト後のデータ(色にじみ算出に使用)
            nextdata = GetVram();
            LAT_AG |= AT_AG;				// とりあえず
		}else{
			data = 0;	// ホント？RGB出力とビデオ出力で異なるはず
		}
		
		if( AT_AG ){	// グラフィック
			switch( AT_GM ){
			case GM_CG1:	//  64x 64 カラー   (CG1)(無効)
			case GM_CG2:	// 128x 64 カラー   (CG2)(無効)
			case GM_CG3:	// 128x 96 カラー   (CG3)(無効)
			case GM_CG6:	// 128x192 カラー   (CG6)
				for( int i=6; i>=0; i-=2 ){
					fdat = COL_CG[AT_CSS][(data>>i)&3];
					*(doff++) = fdat;
					*(doff++) = fdat;
				}
				break;
				
			case GM_RG1:	// 128x 64 モノクロ (RG1)(無効)
			case GM_RG2:	// 128x 96 モノクロ (RG2)(無効)
			case GM_RG3:	// 128x192 モノクロ (RG3)(無効)
			case GM_RG6:	// 256x192 モノクロ (RG6)
				if( Mode4Col ){	// 128x192 カラー(にじみ)
                    //VRAM上の連続した3バイト
                    uint32_t data3 = prevdata << 16 | data << 8 | nextdata;
                    int CsC = AT_CSS + Mode4Col*2;
                    for( int i=6; i>=0; i-=2 ){
                        //にじみテーブル上のインデックス
                        uint32_t nijimiIdx = (data3 & (0x0000003F << (i + 6))) >> (i + 6);
                        *(doff++) = COL_CG[CsC][NIJIMI_TBL[nijimiIdx][0]];
                        *(doff++) = COL_CG[CsC][NIJIMI_TBL[nijimiIdx][1]];
                    }
//					int CsC = AT_CSS + Mode4Col*2;
//					for( int i=6; i>=0; i-=2 ){
//						fdat = COL_CG[CsC][(data>>i)&3];
//						*(doff++) = fdat;
//						*(doff++) = fdat;
//					}
				}else{			// 256x192 モノクロ
					for( int i=7; i>=0; i-- )
						*(doff++) = COL_RG[AT_CSS][(data>>i)&1];
				}
			}
		}else{			// アルファニューメリック
			switch( ANMODE ){
			case AM_AN0:		// アルファニューメリック(内部フォント)(無効)
			case AM_AN1:		// アルファニューメリック(外部フォント)
				fg   = COL_AN[(AT_CSS<<1) |   AT_INV];
				bg   = COL_AN[(AT_CSS<<1) | (~AT_INV&1)];
				fdat = GetFont1( data*16 + RowCntA );
				break;
				
			case AM_SG4:		// セミグラフィック4(無効)
			case AM_SG6:		// セミグラフィック6
				fg   = COL_SG[(AT_CSS<<2) | ((data>>6)&3)];
				bg   = COL_SG[8];
				fdat = GetFont1( 0x1000 + (data&0x3f)*16 + RowCntA );	// セミグラフォントは 1000H-13FFH
			}
			for( int i=7; i>=0; i-- )
				*(doff++) = (fdat>>i)&1 ? fg : bg;
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
		if( RowCntA++ == 11 ){
			VAddr++;
			RowCntA = 0;
		}
	}
	HAddr = 0;
	
	// 右側ボーダー描画
	doff = GetBufAddr() + ( TB60 + line ) * GetBufPitch() + LB60 + P6WIDTH;
	for( int i=0; i<RB60; i++ ) *(doff++) = GetBcol();
}


////////////////////////////////////////////////////////////////
// 1ライン描画(N60m)
//
// 引数:	line	描画するライン番号(0-199)
// 返値:	なし
////////////////////////////////////////////////////////////////
void cMC6847_2::Draw1line2( int line )
{
	BYTE attr, data;
	
	// バックバッファアドレス求める
	BYTE *doff = GetBufAddr() + ( TB62 + line ) * GetBufPitch();
	
	// 左側ボーダー描画
	for( int i=0; i<LB62; i++ ) *(doff++) = GetBcol();
	
	// 表示エリア描画
	for( int x=0; x<( P2WIDTH / 8 ); x++){
		if( CrtDisp ){
			attr = GetAttr();
			data = GetVram();
			HAddr++;
		}else{
			attr = 0;	// ホント？
			data = 0;	// ホント？
		}
		
		if( Mk2CharMode ){	// キャラクタモード
			BYTE fg   = COL_AN2[attr&0x0f];
			BYTE bg   = COL_AN2[((attr>>4)&0x07)|((Css2&1)<<3)];
			BYTE fdat = GetFont2( (data+((attr&0x80)?256:0))*16 + RowCntA );
			for( int i=7; i>=0; i-- )
				*(doff++) = (fdat>>i)&1 ? fg : bg;
		}else{				// グラフィックモード
			if( Mk2GraphMode ){	// グラフィック モード3
				for( int i=6; i>=0; i-=2 ){
					BYTE fdat = COL_CG3[Css3][(((data<<2)>>i)&0x0c)|((attr>>i)&3)];
					*(doff++) = fdat;
					*(doff++) = fdat;
				}
			}else{				// グラフィック モード4
				BYTE c = (Css2<<3)|(Css1<<2);
				for( int i=7; i>=0; i-- )
					*(doff++) = COL_CG4[Css3][c|(((data<<1)>>i)&2)|((attr>>i)&1)];
			}
		}
	}
	
	
	// メモリアドレスオフセット求める
	if( Mk2CharMode ){	// アルファニューメリック
		if( RowCntA++ == 9 ){
			VAddr++;
			RowCntA = 0;
		}
	}else{				// グラフィック
		VAddr++;
		RowCntA = 0;
	}
	HAddr = 0;
	
	// 右側ボーダー描画
	doff = GetBufAddr() + ( TB62 + line ) * GetBufPitch() + LB62 + P2WIDTH;
	for( int i=0; i<RB62; i++ ) *(doff++) = GetBcol();
}


////////////////////////////////////////////////////////////////
// ボーダーカラー取得
//
// 引数:	なし
// 返値:	BYTE	ボーダーカラーコード
////////////////////////////////////////////////////////////////
BYTE cMC6847_1::GetBcol( void )
{
	BYTE bcol;
	
	if( AT_AG ){	// グラフィックモード
		if( AT_GM & 4 ) bcol = COL_RG[AT_CSS][1];	// モノクロ
		else			bcol = COL_CG[AT_CSS][0];	// カラー
	}else{			// アルファニューメリックモード
		if( AT_AS ) 	bcol = COL_SG[8];			// セミグラフィック
		else			bcol = COL_AN[4];			// アルファニューメリック
	}
	
	return bcol;
}

BYTE cMC6847_2::GetBcol( void )
{
	BYTE bcol;
	
	// 実際は全部同じかも
	if( N60Win )               bcol = COL_AN[4];		// N60
	else{												// N60m
		if( Mk2CharMode )      bcol = COL_AN2[0];		// キャラクタモード
		else{
			if( Mk2GraphMode ) bcol = COL_CG3[0][0];	// グラフィック モード3
			else			   bcol = COL_CG4[0][0];	// グラフィック モード4
		}
	}
	
	return bcol;
}


