/* $Id:	d7752.h,v 1.3 2004/02/25 12:25:58 cisc Exp $ */

/*
 * μPD7752	風味 音声合成エンジン
 * D7752 用サービスモジュール
 *
 * Copyright (c) 2004 cisc.
 * All rights reserved.
 *
 * This	software is	provided 'as-is', without any express or implied
 * warranty.  In no	event will the authors be held liable for any damages
 * arising from	the	use	of this	software.
 *
 * Permission is granted to	anyone to use this software	for	any	purpose,
 * including commercial	applications, and to alter it and redistribute it
 * freely, subject to the following	restrictions:
 *
 * 1. The origin of	this software must not be misrepresented; you must not
 *	  claim	that you wrote the original	software. If you use this software
 *	  in a product,	an acknowledgment in the product documentation would be
 *	  appreciated but is not required.
 * 2. Altered source versions must be plainly marked as	such, and must not be
 *	  misrepresented as	being the original software.
 * 3. This notice may not be removed or	altered	from any source	distribution.
 */

#ifndef	PD7752_H_INCLUDED
#define	PD7752_H_INCLUDED

#include "../typedef.h"


typedef	int	D7752_SAMPLE;
typedef	int	D7752_FIXED;

#define	D7752_ERR_SUCCESS		(0)
#define	D7752_ERR_PARAM			(-1)

#define	D7752_ERR_DEVICE_MODE	(-2)
#define	D7752_ERR_MEMORY		(-3)
#define	D7752_ERR_BUFFER_FULL	(-4)
#define	D7752_ERR_BUFFER_EMPTY	(-5)


#define	D7752E_BSY	(0x80)	/* b7 BSY -	音声合成中なら 1 */
#define	D7752E_REQ	(0x40)	/* b6 REQ -	音声パラメータバッファに余裕があれば 1 */
#define	D7752E_EXT	(0x20)	/* b5 INT/EXT -	1 */
#define	D7752E_ERR	(0x10)	/* b4 ERR -	転送エラーがあった場合 1 */
#define	D7752E_IDL	(0x00)	/* 待機中 */


////////////////////////////////////////////////////////////////
// クラス定義
////////////////////////////////////////////////////////////////
class cD7752 {
private:
	// フィルタ係数
	typedef	struct{
		D7752_FIXED	f[5];
		D7752_FIXED	b[5];
		D7752_FIXED	amp;
		D7752_FIXED	pitch;
	} D7752Coef;
	
	// ボイス
	D7752Coef Coef;
	int	Y[5][2];
	int	PitchCount;
	int	FrameSize;
	
	// 振幅展開テーブル
	const static int amp_table[16];
	
	// 謎のフィルタ係数	(uPD7752風味)
	const static int iir1[128];
	const static int iir2[64];
	
public:
	cD7752();								// コンストラクタ
	virtual	~cD7752();						// デストラクタ
	
	int	Start( int );						// 音声合成開始
	int	GetFrameSize();						// 1フレーム分のサンプル数取得
	int	Synth( BYTE *, D7752_SAMPLE * );	// 1フレーム分の音声を合成
};

#endif	// PD7752_H_INCLUDED
