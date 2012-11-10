/* $Id:	d7752.c,v 1.3 2004/02/25 12:25:58 cisc Exp $ */

/*
 * μPD7752	風味 音声合成エンジン
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

#include <stdlib.h>

#include "pd7752.h"

// internal	macros
#define	I2F(a) (((D7752_FIXED) a) << 16)
#define	F2I(a) ((int)((a) >> 16))


// 振幅展開テーブル
const int cD7752::amp_table[16] = { 0,	1, 1, 2, 3,	4, 5, 7, 9,	13,	17,	23,	31,	42,	56,	75 };

// 謎のフィルタ係数	(uPD7752風味)
const int cD7752::iir1[128] = {
	 11424,	11400, 11377, 11331, 11285,	11265, 11195, 11149,
	 11082,	11014, 10922, 10830, 10741,	10629, 10491, 10332,
	 10172,	 9992,	9788,  9560,  9311,	 9037,	8721,  8377,
	  8016,	 7631,	7199,  6720,  6245,	 5721,	5197,  4654,
	-11245,-11200,-11131,-11064,-10995,-10905,-10813,-10700,
	-10585,-10447,-10291,-10108, -9924,	-9722, -9470, -9223,
	 -8928,	-8609, -8247, -7881, -7472,	-7019, -6566, -6068,
	 -5545,	-4999, -4452, -3902, -3363,	-2844, -2316, -1864,
	 11585,	11561, 11561, 11561, 11561,	11540, 11540, 11540,
	 11515,	11515, 11494, 11470, 11470,	11448, 11424, 11400,
	 11377,	11356, 11307, 11285, 11241,	11195, 11149, 11082,
	 11014,	10943, 10874, 10784, 10695,	10583, 10468, 10332,
	 10193,	10013,	9833,  9628,  9399,	 9155,	8876,  8584,
	  8218,	 7857,	7445,  6970,  6472,	 5925,	5314,  4654,
	  3948,	 3178,	2312,  1429,   450,	 -543, -1614, -2729,
	 -3883,	-5066, -6250, -7404, -8500,	-9497,-10359,-11038
};

const int cD7752::iir2[64]	= {
	 8192, 8105, 7989, 7844, 7670, 7424, 7158, 6803,
	 6370, 5860, 5252, 4579, 3824, 3057, 2307, 1623,
	 8193, 8100, 7990, 7844, 7672, 7423, 7158, 6802,
	 6371, 5857, 5253, 4576, 3825, 3057, 2309, 1617,
	-6739,-6476,-6141,-5738,-5225,-4604,-3872,-2975,
	-1930, -706,  686, 2224, 3871, 5518, 6992, 8085,
	-6746,-6481,-6140,-5738,-5228,-4602,-3873,-2972,
	-1931, -705,  685, 2228, 3870, 5516, 6993, 8084
};


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cD7752::cD7752( void ) : PitchCount(0), FrameSize(0)
{
	for( int i=0; i < COUNTOF(Y); i++ ){
		INITARRAY( Y[i], 0 );
	}
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cD7752::~cD7752( void ){}


////////////////////////////////////////////////////////////////
// 音声合成を開始します。
// 引数:	mode	音声合成のモードを指定します
//					b2 F: 分析フレーム BIT
//						F=0	10ms/フレーム
//						F=1	20ms/フレーム
//					b1-0 S:	発声速度 BIT
//						S=00: NORMAL SPEED
//						S=01: SLOW SPEED
//						S=10: FAST SPEED
// 返値:	int		エラーコード
////////////////////////////////////////////////////////////////
int	cD7752::Start( int mode )
{
	const static int frame_size[8] = {
		100,		 //	10ms, NORMAL
		120,		 //	10ms, SLOW
		 80,		 //	10ms, FAST
		100,		 //	PROHIBITED
		200,		 //	20ms, NORMAL
		240,		 //	20ms, SLOW
		160,		 //	20ms, FAST
		200,		 //	PROHIBITED
	};
	
	// フィルタパラメータの初期値
	const static int f_default[5] =	{ 126, 64, 121,	111, 96	};
	const static int b_default[5] =	{	9,	4,	 9,	  9, 11	};
	
	// パラメータ変数の初期化
	FrameSize =	frame_size[mode	& 7];
	
	for( int i=0; i<5; i++ ){
		Y[i][0]	  =	0;
		Y[i][1]	  =	0;
		Coef.f[i] =	I2F( f_default[i] );
		Coef.b[i] =	I2F( b_default[i] );
	}
	PitchCount = 0;
	Coef.amp   = 0;
	Coef.pitch = I2F( 30 );
	
	return D7752_ERR_SUCCESS;
}


////////////////////////////////////////////////////////////////
//	音声合成の1フレーム分の長さを求めます。
//	フレーム長は、Start	を呼び出した時点で設定されます。
//	引数:	なし
//	返値:	int		フレームサイズ
////////////////////////////////////////////////////////////////
int	cD7752::GetFrameSize( void	)
{
	return FrameSize;
}


////////////////////////////////////////////////////////////////
// 1フレーム分の音声を合成します。
// 引数:	frame	合成した波形データの格納先。1フレーム分の個数
// 返値:	int		エラーコード
////////////////////////////////////////////////////////////////
int	cD7752::Synth(	BYTE *param, D7752_SAMPLE *frame )
{
	int	vu;
	int	qmag;
	D7752Coef *curr;
	D7752Coef incr,	next;
	
	if(	!param || !frame ) return D7752_ERR_PARAM;
	
	curr = &Coef;
	
	// まず、パラメータを係数に展開する
	qmag = (param[0] & 4) != 0 ? 1 : 0;
	
	for( int i=0; i<5; i++ ){
		int	f =	(param[i+1]	>> 3) &	31;
		if(	f &	16 ) f -= 32;
		next.f[i] =	curr->f[i] + I2F( f	<< qmag	);
		
		int	b =	param[i+1] & 7;
		if(	b &	4 )	b -= 8;
		next.b[i] =	curr->b[i] + I2F( b	<< qmag	);
	}
	
	next.amp = I2F(	( param[6] >> 4	) &	15 );
	
	int	p =	param[6] & 7;
	if(	p &	4 )	p -= 8;
	next.pitch = curr->pitch + I2F(p);
	
	// 線形補完用の増分値の計算
	incr.amp   = ( next.amp	  -	curr->amp )	  /	FrameSize;
	incr.pitch = ( next.pitch -	curr->pitch	) /	FrameSize;
	for( int i=0; i<5; i++ ){
		incr.b[i] =	( next.b[i]	- curr->b[i] ) / FrameSize;
		incr.f[i] =	( next.f[i]	- curr->f[i] ) / FrameSize;
	}
	
	// インパルス・ノイズの発生有無をチェック
	vu	= param[0] & 1 ? 1 : 2;
	vu |= param[6] & 4 ? 3 : 0;
	
	// 合成する
	for( int i=0; i<FrameSize; i++ ){
		int	y =	0;
		
		// インパルス発生
		int	c =	F2I( curr->pitch );
		if(	PitchCount > (c	> 0	? c	: 128) ){
			if(	vu & 1 ) y = amp_table[F2I(curr->amp)] * 16	- 1;
			PitchCount = 0;
		}
		PitchCount++;
		
		// ノイズ発生
		if(	vu & 2 )
			if(	rand() & 1 ) y += amp_table[F2I(curr->amp)]	* 4	- 1;	 //	XXX	ノイズ詳細不明
		
		// 謎のディジタルフィルタ
		for( int j=0; j<5; j++ ){
			int	t;
			t  = Y[j][0] * iir1[ F2I( curr->f[j] ) & 0x7f		  ]	/ 8192;
			y += t		 * iir1[(F2I( curr->b[j] ) * 2 + 1)	& 0x7f]	/ 8192;
			y -= Y[j][1] * iir2[ F2I( curr->b[j] ) & 0x3f		  ]	/ 8192;
			
			y =	y >	8191 ? 8191	: y	< -8192	? -8192	: y;
			
			Y[j][1]	= Y[j][0];
			Y[j][0]	= y;
		}
		
		// データを保存
		*frame++ = y;
		
		// パラメータを増分
		curr->amp	+= incr.amp;
		curr->pitch	+= incr.pitch;
		for( int j=0; j<5; j++ ){
			curr->b[j] += incr.b[j];
			curr->f[j] += incr.f[j];
		}
	}
	
	// パラメータをシフトする
	memcpy(	curr, &next, sizeof(D7752Coef) );
	
	return D7752_ERR_SUCCESS;
}
