#include "log.h"
#include "common.h"
#include "png.h"

#ifdef __APPLE__
#include <Carbon/Carbon.h>  // add Windy for UTF8 変換
#endif


////////////////////////////////////////////////////////////////
// CRC32計算
//
// 引数:	buf				データバッファへのポインタ
//			num				データ数(バイト)
// 返値:	DWORD			CRC値
////////////////////////////////////////////////////////////////
DWORD CalcCrc32( BYTE *buf, int num )
{
	const DWORD Crc32Table[] ={
		0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
		0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
		0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
		0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
		0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
		0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
		0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
		0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
		0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
		0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
		0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
		0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
		0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
		0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
		0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
		0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
		
		0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
		0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
		0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
		0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
		0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
		0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
		0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
		0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
		0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
		0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
		0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
		0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
		0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
		0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
		0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
		0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
		
		0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
		0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
		0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
		0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
		0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
		0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
		0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
		0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
		0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
		0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
		0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
		0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
		0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
		0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
		0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
		0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
		
		0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
		0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
		0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
		0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
		0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
		0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
		0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
		0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
		0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
		0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
		0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
		0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
		0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
		0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
		0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
		0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
	};
	
	DWORD crc = ~0;
	for( int i=0; i<num; i++ )
		crc = ( crc >> 8 ) ^ Crc32Table[ ( buf[i] ^ ( crc & 0x000000FF ) ) &0x00ff ];
	
	return ~crc;
}


////////////////////////////////////////////////////////////////
// EUC -> JIS
//
// 引数:	c1				第1バイトへのポインタ
//			c2				第2バイトへのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void Euc2Jis( BYTE *c1, BYTE *c2 )
{
	*c1 &= 0x7f;
	*c2 &= 0x7f;
}


////////////////////////////////////////////////////////////////
// JIS -> EUC
//
// 引数:	c1				第1バイトへのポインタ
//			c2				第2バイトへのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
/*
void Jis2Euc( BYTE *c1, BYTE *c2 )
{
	*c1 |= 0x80;
	*c2 |= 0x80;
}
*/


////////////////////////////////////////////////////////////////
// EUC -> SJIS
//
// 引数:	c1				第1バイトへのポインタ
//			c2				第2バイトへのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
/*
void Euc2Sjis( BYTE *c1, BYTE *c2 )
{
	// 第2バイトの変換
	if( ( *c1 % 2 ) == 0 )
		*c2 -= 0x02;
	else{
		*c2 -= 0x61;
		if( *c2 > 0x7e ) ++ *c2;
	}
	
	// 第1バイトの変換
	if( *c1 < 0xdf ){
		++ *c1;
		*c1 /= 2;
		*c1 += 0x30;
	}else{
		++ *c1;
		*c1 /= 2;
		*c1 += 0x70;
	}
}
*/


////////////////////////////////////////////////////////////////
// SJIS -> EUC
//
// 引数:	c1				第1バイトへのポインタ
//			c2				第2バイトへのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
/*
void Sjis2Euc( BYTE *c1, BYTE *c2 )
{
	if( *c2 < 0x9f ){
		if( *c1 < 0xa0 ){
			*c1 -= 0x81;
			*c1 *= 2;
			*c1 += 0xa1;
		}else{
			*c1 -= 0xe0;
			*c1 *= 2;
			*c1 += 0xdf;
		}
		if( *c2 > 0x7f ) -- *c2;
		*c2 += 0x61;
	}else{
		if( *c1 < 0xa0 ){
			*c1 -= 0x81;
			*c1 *= 2;
			*c1 += 0xa2;
		}else{
			*c1 -= 0xe0;
			*c1 *= 2;
			*c1 += 0xe0;
		}
		*c2 += 2;
	}
}
*/


////////////////////////////////////////////////////////////////
// SJIS -> JIS
//
// 引数:	c1				第1バイトへのポインタ
//			c2				第2バイトへのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void Sjis2Jis( BYTE *c1, BYTE *c2 )
{
	if( *c2 < 0x9f ){
		if( *c1 < 0xa0 ){
			*c1 -= 0x81;
			*c1 *= 2;
			*c1 += 0x21;
		}else{
			*c1 -= 0xe0;
			*c1 *= 2;
			*c1 += 0x5f;
		}
		if( *c2 > 0x7f ) --*c2;
		*c2 -= 0x1f;
	}else{
		if( *c1 < 0xa0 ){
			*c1 -= 0x81;
			*c1 *= 2;
			*c1 += 0x22;
		}else{
			*c1 -= 0xe0;
			*c1 *= 2;
			*c1 += 0x60;
		}
		*c2 -= 0x7e;
	}
}


////////////////////////////////////////////////////////////////
// JIS -> SJIS
//
// 引数:	c1				第1バイトへのポインタ
//			c2				第2バイトへのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void Jis2Sjis( BYTE *c1, BYTE *c2 )
{
	// 第2バイトの変換
	if( ( *c1 & 1 ) == 0 ) *c2 += 0x7d;
	else                   *c2 += 0x1f;
	if( *c2 > 0x7e ) ++ *c2;
	
	// 第1バイトの変換
	if( *c1 < 0x5f ){
		++ *c1;
		*c1 /= 2;
		*c1 += 0x70;
	}else{
		++ *c1;
		*c1 /= 2;
		*c1 += 0xb0;
	}
}


////////////////////////////////////////////////////////////////
// 文字コード形式変換(SJIS,EUC -> JIS)
//
// 引数:	c1				第1バイトへのポインタ
//			c2				第2バイトへのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void Convert2Jis( BYTE *c1, BYTE *c2 )
{
	// ShiftJIS?
	if( (((*c1 >= 0x81) && (*c1 <= 0x9f)) || ((*c1 >= 0xe0) && (*c1 <= 0xfc))) && ((*c2 >= 0x40) && (*c2 <= 0xfc) && (*c2 != 0x7f)) )
		Sjis2Jis( c1, c2 );
	// EUC?
	else if( ((*c1 >= 0xa1) && (*c1 <= 0xfe)) && ((*c2 >= 0xa1) && (*c1 <= 0xfe)) )
		Euc2Jis( c1, c2 );
}


////////////////////////////////////////////////////////////////
// SJIS -> UTF8
//
// 引数:	str				SJIS文字列へのポインタ
// 返値:	char *			UTF8文字列へのポインタ
////////////////////////////////////////////////////////////////
#ifdef __APPLE__
char *Sjis2UTF8( const char *str )
{
	static char dst[PATH_MAX];
	CFStringRef cfstr;
	
	cfstr = CFStringCreateWithCString( NULL, str, kCFStringEncodingShiftJIS_X0213_00 );
	CFStringGetCString( cfstr, dst, PATH_MAX, kCFStringEncodingUTF8 );
	CFRelease( cfstr );
	return dst;
}
#endif


// isioさんの txt2bas から流用
/****************************************************/
/* txt2bas                                          */
/* sjis.h                                           */
/* 2000.03.26. by ishioka                           */
/* 2000.05.25., 2001.01.06.                         */
/****************************************************/

// sjis <-> p6 character set table
int sjistbl[] = {
	0x0000, 0x8c8e, 0x89ce, 0x9085, 0x96d8, 0x8be0, 0x9379, 0x93fa, 
	0x944e, 0x897e, 0x8e9e, 0x95aa, 0x9562, 0x9553, 0x90e7, 0x969c, 
	0x83ce, 0x84b3, 0x84b1, 0x84b2, 0x84b0, 0x84b4, 0x84ab, 0x84aa, 
	0x84ac, 0x84ad, 0x84af, 0x84ae, 0x817e, 0x91e5, 0x9286, 0x8fac, 
	0x8140, 0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166, 
	0x8169, 0x816a, 0x8196, 0x817b, 0x8143, 0x817c, 0x8144, 0x815e, 
	0x824f, 0x8250, 0x8251, 0x8252, 0x8253, 0x8254, 0x8255, 0x8256, 
	0x8257, 0x8258, 0x8146, 0x8147, 0x8183, 0x8181, 0x8184, 0x8148, 
	0x8197, 0x8260, 0x8261, 0x8262, 0x8263, 0x8264, 0x8265, 0x8266, 
	0x8267, 0x8268, 0x8269, 0x826a, 0x826b, 0x826c, 0x826d, 0x826e, 
	0x826f, 0x8270, 0x8271, 0x8272, 0x8273, 0x8274, 0x8275, 0x8276, 
	0x8277, 0x8278, 0x8279, 0x816b, 0x818f, 0x816c, 0x814f, 0x8151, 
	0x0000, 0x8281, 0x8282, 0x8283, 0x8284, 0x8285, 0x8286, 0x8287, 
	0x8288, 0x8289, 0x828a, 0x828b, 0x828c, 0x828d, 0x828e, 0x828f, 
	0x8290, 0x8291, 0x8292, 0x8293, 0x8294, 0x8295, 0x8296, 0x8297, 
	0x8298, 0x8299, 0x829a, 0x816f, 0x8162, 0x8170, 0x8160, 0x8140, 
	0x81a3, 0x81a5, 0x819a, 0x819f, 0x819b, 0x819c, 0x82f0, 0x829f, 
	0x82a1, 0x82a3, 0x82a5, 0x82a7, 0x82e1, 0x82e3, 0x82e5, 0x82c1, 
	0x0000, 0x82a0, 0x82a2, 0x82a4, 0x82a6, 0x82a8, 0x82a9, 0x82ab, 
	0x82ad, 0x82af, 0x82b1, 0x82b3, 0x82b5, 0x82b7, 0x82b9, 0x82bb, 
	0x0000, 0x8142, 0x8175, 0x8176, 0x8141, 0x8145, 0x8392, 0x8340, 
	0x8342, 0x8344, 0x8346, 0x8348, 0x8383, 0x8385, 0x8387, 0x8362, 
	0x815b, 0x8341, 0x8343, 0x8345, 0x8347, 0x8349, 0x834a, 0x834c, 
	0x834e, 0x8350, 0x8352, 0x8354, 0x8356, 0x8358, 0x835a, 0x835c, 
	0x835e, 0x8360, 0x8363, 0x8365, 0x8367, 0x8369, 0x836a, 0x836b, 
	0x836c, 0x836d, 0x836e, 0x8371, 0x8374, 0x8377, 0x837a, 0x837d, 
	0x837e, 0x8380, 0x8381, 0x8382, 0x8384, 0x8386, 0x8388, 0x8389, 
	0x838a, 0x838b, 0x838c, 0x838d, 0x838f, 0x8393, 0x814a, 0x814b, 
	0x82bd, 0x82bf, 0x82c2, 0x82c4, 0x82c6, 0x82c8, 0x82c9, 0x82ca, 
	0x82cb, 0x82cc, 0x82cd, 0x82d0, 0x82d3, 0x82d6, 0x82d9, 0x82dc, 
	0x82dd, 0x82de, 0x82df, 0x82e0, 0x82e2, 0x82e4, 0x82e6, 0x82e7, 
	0x82e8, 0x82e9, 0x82ea, 0x82eb, 0x82ed, 0x82f1, 0x0000, 0x0000, 
};

// add 2000.05.25.
int sjistbl2[] = {
	// sjis dakuon list
	0x82aa, 0x82ac, 0x82ae, 0x82b0, 0x82b2, 
	0x82b4, 0x82b6, 0x82b8, 0x82ba, 0x82bc, 
	0x82be, 0x82c0, 0x82c3, 0x82c5, 0x82c7, 
	0x82ce, 0x82d1, 0x82d4, 0x82d7, 0x82da, 
	0x834b, 0x834d, 0x834f, 0x8351, 0x8353, 
	0x8355, 0x8357, 0x8359, 0x835b, 0x835d, 
	0x835f, 0x8361, 0x8364, 0x8366, 0x8368, 
	0x836f, 0x8372, 0x8375, 0x8378, 0x837b, 
	// sjis han-dakuon list
	0x82cf, 0x82d2, 0x82d5, 0x82d8, 0x82db, 
	0x8370, 0x8373, 0x8376, 0x8379, 0x837c, 
	// for "vu" (2001.01.06)
	0x8394,
};


////////////////////////////////////////////////////////////////
// SJIS -> P6
//
// 引数:	dstr			P6文字列格納ポインタ
//			sstr			SJIS文字列へのポインタ
// 返値:	int				P6文字列の長さ
////////////////////////////////////////////////////////////////
int Sjis2P6( char *dstr, char *sstr )
{
	char *ss = sstr;
	char *ds = dstr;
	int c;
	
	while( (c = (BYTE)(*ss++)) != '\0' ){
		if( (c >= 0x80) && (c <= 0x9f) ){
			c = c * 256 + (BYTE)(*ss++);
			
			// for dakuon/han-dakuon 2000.05.25.
			int opt = 0;
			for( int j=0; j<COUNTOF(sjistbl2); j++ ){
				if( sjistbl2[j] == c ){
					if( j == 50 ){
						// for "vu" (2001.01.06)
						opt = 1;
						c = 0x8345;
					}else{
						opt = (j < 40) ? 1 : 2;
						c = c - opt;
						break;
					}
				}
			}
			
			for( int j=0; j<256; j++ ){
				if( sjistbl[j] == c ){
					if( j <= 0x1f ){
						// if graphic character
						*ds++ = (char)0x14;
						*ds++ = (char)(j + 0x30);
					}else{
						*ds++ = (char)j;
						if( opt != 0 )
							*ds++ = (char)(0xde + opt - 1);
					}
					break;
				}
			}
		}else if( c <= 0x1f ){
			switch( c ){
			case 0x09:
				*ds++ = 0x09;
				break;
			case 0x0a:
				*ds++ = 0x0d;
				break;
			case 0x0d:
				if( *ss == 0x0a ) ss++;
				*ds++ = 0x0d;
				break;
			}
		}else
			*ds++ = (char)c;
	}
	*ds++ = '\0';
	
	return ds-dstr;
}


////////////////////////////////////////////////////////////////
// Img SAVE
//   情報参照「PNG利用術」 http://gmoon.jp/png/
//
// 引数:	filename		保存ファイル名
//			sur				保存するサーフェスへのポインタ
//			pos				保存する領域情報へのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void SaveImg( char *filename, VSurface *sur, VRect *pos )
{
	PRINTD1( GRP_LOG, "[COMMON][SaveImg] -> %s\n", filename );
	
	FILE *fp;
	png_structp PngPtr;
	png_infop InfoPtr;
	png_bytepp image;	// image[HEIGHT][WIDTH]の形式
	png_colorp Palette = NULL;
	png_color_8 SigBit = { 8, 8, 8, 0 };
	VRect rec;
	int bpp;
	
	// 領域設定
	if( pos ){
		rec.x = pos->x;	rec.y = pos->y;
		rec.w = pos->w;	rec.h = pos->h;
	}else{
		rec.x =               rec.y = 0;
		rec.w = sur->Width(); rec.h = sur->Height();
	}
	
	// bit/pixel取得
	bpp = sur->Bpp();
	
	
	// PngPtr構造体を確保・初期化する
	PngPtr  = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL );
	
	// InfoPtr構造体を確保・初期化する
	InfoPtr = png_create_info_struct( PngPtr );
	
	// フィルタの設定
	png_set_filter( PngPtr, 0, PNG_ALL_FILTERS );
	
	// 圧縮率の設定
	png_set_compression_level( PngPtr, 9 );
	
	// IHDRチャンク情報を設定する
	png_set_IHDR( PngPtr, InfoPtr, rec.w, rec.h, 8,
					bpp == 8 ? PNG_COLOR_TYPE_PALETTE : PNG_COLOR_TYPE_RGB,
					PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT,
					PNG_FILTER_TYPE_DEFAULT );
	
	// イメージデータを2次元配列に配置する
	image = new png_bytep[rec.h];
	
	switch( bpp ){
	case 8:		// 8bitモード
		{
		BYTE *doff = (BYTE *)sur->GetPixels() + rec.x + rec.y * sur->Pitch();
		for( int i=0; i<rec.h; i++ ){
			image[i] = new png_byte[rec.w];
			memcpy( image[i], doff, rec.w );
			doff += sur->Pitch();
		}
		
		// パレットを作る
		Palette = ( png_colorp )png_malloc( PngPtr, sur->GetPalette()->ncols * sizeof (png_color) );
		// サーフェスからパレットを取得
		for( int i=0; i<sur->GetPalette()->ncols; i++ ){
			Palette[i].red   = sur->GetPalette()->colors[i].r;
			Palette[i].green = sur->GetPalette()->colors[i].g;
			Palette[i].blue  = sur->GetPalette()->colors[i].b;
		}
		png_set_PLTE( PngPtr, InfoPtr, Palette, sur->GetPalette()->ncols );
		}
		break;
		
	case 16:	// 16bitモード
		{		// PNGには16bitモードがないみたいなので24bitに変換(5:6:5決め打ちで)
		WORD *doff = (WORD *)sur->GetPixels() + ( rec.x + rec.y * sur->Pitch() ) / sizeof(WORD);
		for( int i=0; i<rec.h; i++ ){
			BYTE *ip = image[i] = new png_byte[rec.w * 3];
			for( int x=0; x<rec.w; x++ ){
				*(ip++) = BYTE((doff[x]<<3)&0x00f8);
				*(ip++) = BYTE((doff[x]>>3)&0x00fc);
				*(ip++) = BYTE((doff[x]>>8)&0x00f8);
			}
			doff += sur->Pitch() / sizeof(WORD);
		}
		
		png_set_sBIT( PngPtr, InfoPtr, &SigBit );
		png_set_bgr( PngPtr );
		}
		break;
		
	case 24:	// 24bitモード
		{
		BYTE *doff = (BYTE *)sur->GetPixels() + rec.x + rec.y * sur->Pitch();
		doff += rec.x * 3 + rec.y * sur->Pitch();
		for( int i=0; i<rec.h; i++ ){
			image[i] = new png_byte[rec.w * 3];
			memcpy( image[i], doff, rec.w * 3 );
			doff += sur->Pitch();
		}
		
		png_set_sBIT( PngPtr, InfoPtr, &SigBit );
		png_set_bgr( PngPtr );
		}
		break;
	}
	
	
	// ファイルを開く
	fp = FOPENEN( filename, "wb" );
	
	// libpngにfpを知らせる
	png_init_io( PngPtr, fp );
	
	// PNGファイルのヘッダを書き込む
	png_write_info( PngPtr, InfoPtr );
	// 画像データを書き込む
	png_write_image( PngPtr, image );
	// 残りの情報を書き込む
	png_write_end( PngPtr, InfoPtr );
	
	// ファイルを閉じる
	fclose( fp );
	
	// パレットを開放する
	if( Palette ) png_free( PngPtr, Palette );
	
	// イメージデータを開放する
	for( int i=0; i<rec.h; i++ ) delete [] image[i];
	delete [] image;
	
	// 2つの構造体のメモリを解放する
	png_destroy_write_struct( &PngPtr, &InfoPtr );
}



////////////////////////////////////////////////////////////////
// Img LOAD
//   情報参照「PNG利用術」 http://gmoon.jp/png/
//
// 引数:	filename		読込むファイル名
// 返値:	VSurface *		読込まれたサーフェスへのポインタ
////////////////////////////////////////////////////////////////
VSurface *LoadImg( char *filename )
{
	PRINTD1( GRP_LOG, "[COMMON][LoadImg] <- %s\n", filename );
	
	FILE *fp;
	png_structp PngPtr;
	png_infop InfoPtr;
	png_uint_32 width, height;
	int BitDepth, ColorType, InterlaceType;
	png_bytepp image;	// image[HEIGHT][WIDTH]の形式
	png_colorp Palette;
	int PaletteCount = 0;
	
	VSurface *sur = NULL;
	COLOR24 PaletteS[256];
	
	// ファイルを開く
	fp = FOPENEN( filename, "rb" );
	if( !fp ) return NULL;
	
	// PNGファイルかどうかを調べる
	png_byte sig[4];
	if( fread( sig, 1, 4, fp ) != 4 ){
		fclose( fp );
		return NULL;
	}
	if( png_sig_cmp( sig, 0, 4 ) ){
		fclose( fp );
		return NULL;
	}
	fseek( fp, 0, SEEK_SET );
	
	// PngPtr構造体を確保・初期化する
	PngPtr  = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	
	// InfoPtr構造体を確保・初期化する
	InfoPtr = png_create_info_struct( PngPtr );
	
	// libpngにfpを知らせる
	png_init_io( PngPtr, fp );
	
	// ヘッダを読込む
	png_read_info( PngPtr, InfoPtr );
	
	// IHDRチャンク情報を取得
	png_get_IHDR( PngPtr, InfoPtr, &width, &height, &BitDepth, &ColorType, &InterlaceType, NULL, NULL );
	
	// 2次元配列を確保する
	image = new png_bytep[height*sizeof( png_bytep )];
	for( int i=0; i<(int)height; i++ ) image[i] = new png_byte[png_get_rowbytes( PngPtr, InfoPtr )];
	
	// 画像データを読込む
	png_read_image( PngPtr, image );
	
	// ファイルを閉じる
	fclose( fp );
	
	
	// サーフェスを作成
	sur = new VSurface;
	sur->InitSurface( width, height, BitDepth );
	
	// 画像データを取得
	BYTE *doff = (BYTE *)sur->GetPixels();
	for( int i=0; i<(int)sur->Height(); i++ ){
		memcpy( doff, image[i], sur->Width() * BitDepth / 8 );
		doff += sur->Pitch();
	}
		
	if( BitDepth == 8 ){	// 8bitカラー限定
		// パレットを取得
		png_get_PLTE( PngPtr, InfoPtr, &Palette, &PaletteCount );
		for( int i=0; i<PaletteCount; i++ ){
			PaletteS[i].r = Palette[i].red;
			PaletteS[i].g = Palette[i].green;
			PaletteS[i].b = Palette[i].blue;
		}
		sur->SetPalette( PaletteS, PaletteCount );
	}
	
	
	// イメージデータを開放する
	for( int i=0; i<(int)height; i++ ) delete [] image[i];
	delete [] image;
	
	// 2つの構造体のメモリを解放する
	png_destroy_read_struct( &PngPtr, &InfoPtr, (png_infopp)NULL );
	
	return sur;
}


////////////////////////////////////////////////////////////////
// 矩形領域合成
//
// 引数:	rr		合成結果を出力する矩形領域へのポインタ
//			r1,r2	合成する矩形領域へのポインタ
// 返値:	なし
////////////////////////////////////////////////////////////////
void RectAdd( VRect *rr, VRect *r1, VRect *r2 )
{
	if( !rr || !r1 || !r2 ) return;
	
	int x1 = max( r1->x, r2->x );
	int y1 = max( r1->y, r2->y );
	int x2 = min( r1->x + r1->w - 1, r2->x + r2->w - 1);
	int y2 = min( r1->y + r1->h - 1, r2->y + r2->h - 1);
	
	rr->w = x1 > x2 ? 0 : x2 - x1 + 1;
	rr->h = y1 > y2 ? 0 : y2 - y1 + 1;
	
	rr->x = rr->w ? x1 : 0;
	rr->y = rr->h ? y1 : 0;
}


