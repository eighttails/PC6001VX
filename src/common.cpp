/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999,2021 Yumitaro
/////////////////////////////////////////////////////////////////////////////
#include <cctype>
#include <cstring> 
#include <algorithm>
#include <new>
#include <unordered_map>
#include <vector>

#include "common.h"
#include "log.h"
#include "osd.h"
#include "png.h"


/////////////////////////////////////////////////////////////////////////////
// 文字コード操作関数
/////////////////////////////////////////////////////////////////////////////

// isioさんの txt2bas から流用
/****************************************************/
/* txt2bas                                          */
/* sjis.h                                           */
/* 2000.03.26. by ishioka                           */
/* 2000.05.25., 2001.01.06.                         */
/****************************************************/

// sjis <-> p6 character set table
const int sjistbl[] = {
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
const int sjistbl2[] = {
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


/////////////////////////////////////////////////////////////////////////////
// SJIS -> P6
//
// 引数:	dstr			P6文字列格納バッファへの参照(末尾に追記)
//			sstr			SJIS文字列への参照
// 返値:	なし
/////////////////////////////////////////////////////////////////////////////
void Sjis2P6( std::string& dstr, const std::string& sstr )
{
	auto ss = sstr.begin();
	
	while( ss != sstr.end() ){
		int c = *ss++;
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
						dstr += 0x14;
						dstr += (char)(j + 0x30);
					}else{
						dstr += (char)j;
						if( opt != 0 )
							dstr += (char)(0xde + opt - 1);
					}
					break;
				}
			}
		}else if( c <= 0x1f ){
			switch( c ){
			case 0x09:
				dstr += 0x09;
				break;
				
			case 0x0a:
				dstr += 0x0d;
				break;
				
			case 0x0d:
				if( *ss == 0x0a ) ss++;
				dstr += 0x0d;
			}
		}else
			dstr += (char)c;
	}
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
	
	
	if( !(bpp == 32 || bpp == 24 || bpp == 8 || bpp == 1) )
		return false;
	
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
	if( !(PngPtr = png_create_write_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)) )
		return false;
	
	if( !(InfoPtr = png_create_info_struct( PngPtr )) ){
		png_destroy_write_struct( &PngPtr, nullptr );
		return false;
	}
	
	// エラー処理
	if( setjmp( png_jmpbuf( PngPtr ) ) ){
		png_destroy_write_struct( &PngPtr, &InfoPtr );
		if( fp ) fclose( fp );
		if( image ){
			for( int i=0; i<rec.h; i++ )
				if( image[i] ) delete [] image[i];
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
		for( int i=0; i<256; i++ ){
			Palette[i].red   = (VSurface::GetColor( i )>>RSHIFT32)&0xff;
			Palette[i].green = (VSurface::GetColor( i )>>GSHIFT32)&0xff;
			Palette[i].blue  = (VSurface::GetColor( i )>>BSHIFT32)&0xff;
		}
		png_set_PLTE( PngPtr, InfoPtr, Palette, 256 );
	}
	
	// イメージデータを2次元配列に配置する
	image = (png_bytepp)png_malloc( PngPtr, sizeof(png_bytep) * rec.h );
	ZeroMemory( image, sizeof(png_bytep) * rec.h );
	BYTE* doff = pixels + rec.x + rec.y * spit;
	for( int i=0; i<rec.h; i++ ){
		image[i] = (png_byte*)png_malloc( PngPtr, dpit );
		std::memcpy( image[i], doff, dpit );
		doff += spit;
	}
	
	// イメージデータをlibpngに知らせる
	png_set_rows( PngPtr, InfoPtr, image );
	
	// 画像ファイルを開く
	if( !(fp = OSD_Fopen( filepath, "wb" )) ){
		for( int i=0; i<rec.h; i++ )
			if( image[i] ) delete [] image[i];
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
	for( int i=0; i<rec.h; i++ ) delete [] image[i];
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
	if( !(PngPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr)) )
		return nullptr;
	
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
		if( fp ) fclose( fp );
		png_destroy_read_struct( &PngPtr, &InfoPtr, &EndInfo );
		if( sur ) delete sur;
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
		
		for( int i=0; i<(int)height; i++){
			png_bytep rpt = RowPtrs[i];
			for( int j=0; j<(int)width; j++ ){
				doff[j] = BitDepth == 1 ? (rpt[j/8]&(0x80>>(j&7))) ? 15 : 0
										: rpt[j];
			}
			doff += sur->Pitch();
		}
	}else if( Channels != 1 && ( ColorType == PNG_COLOR_TYPE_RGB_ALPHA || ColorType == PNG_COLOR_TYPE_RGB ) &&
				BitDepth == 8 ){
		// GRAY_ALPHA, RGB, RGB_ALPHA, RGB+x
		for( int i=0; i<(int)height; i++ ){
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


/////////////////////////////////////////////////////////////////////////////
// 文字列取得関数等
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// メッセージテーブル
/////////////////////////////////////////////////////////////////////////////
static const std::unordered_map<TextID, const std::string> MsgString =
{
	{ T_EMPTY,					"???" },
	
	// 一般メッセージ
	{ T_QUIT,					"終了してよろしいですか?" },
	{ T_QUITC,					"終了確認" },
	{ T_RESTART0,				"再起動してよろしいですか?" },
	{ T_RESTART,				"変更は再起動後に有効となります。\n今すぐ再起動しますか?" },
	{ T_RESTARTC,				"再起動確認" },
	{ T_RESTARTI,				"拡張カートリッジを挿入して再起動します。" },
	{ T_RESTARTE,				"拡張カートリッジを排出して再起動します。" },
	{ T_RESETC,					"リセット確認" },
	{ T_DOKOC,					"どこでもLOAD確認" },
	{ T_DOKOSLOT,				"どこでもLOADを実行してよろしいですか？" },
	{ T_REPLAYRES,				"リプレイを途中保存地点まで巻き戻します\nよろしいですか？" },
	
	// INIファイル用メッセージ ------
	{ TINI_TITLE,				"; === PC6001V 初期設定ファイル ===\n\n" },
	// [CONFIG]
	{ TINI_Model,				"機種 60:PC-6001 61:PC-6001A 62:PC-6001mkⅡ 66:PC-6601 64:PC-6001mkⅡSR 68:PC-6601SR" },
	{ TINI_OverClock,			"オーバークロック率 (1-1000)%" },
	{ TINI_CheckCRC,			"CRCチェック Yes:有効 No:無効" },
	// [CMT]
	{ TINI_TurboTAPE,			"Turbo TAPE Yes:有効 No:無効" },
	{ TINI_BoostUp,				"BoostUp Yes:有効 No:無効" },
	{ TINI_MaxBoost60,			"BoostUp 最大倍率(N60モード)" },
	{ TINI_MaxBoost62,			"BoostUp 最大倍率(N60m/N66モード)" },
	{ TINI_StopBit,				"ストップビット数 (2-10)bit" },
	// [FDD]
	{ TINI_FDDrive,				"ドライブ数 (0-2)" },
	{ TINI_FDDWait,				"アクセスウェイト Yes:有効 No:無効" },
	// [DISPLAY]
	{ TINI_Mode4Color,			"MODE4カラーモード 0:モノクロ 1:赤/青 2:青/赤 3:ピンク/緑 4:緑/ピンク" },
	{ TINI_WindowZoom,			"ウィンドウ表示倍率(%)" },
	{ TINI_FrameSkip,			"フレームスキップ" },
	{ TINI_ScanLine,			"スキャンライン Yes:あり No:なし" },
	{ TINI_ScanLineBr,			"スキャンライン輝度 (0-100)%" },
	{ TINI_DispNTSC,			"4:3表示 Yes:有効 No:無効" },
	{ TINI_Filtering,			"フィルタリング Yes:アンチエイリアシング No:ニアレストネイバー" },
	{ TINI_FullScreen,			"フルスクリーンモード Yes:有効 No:無効" },
	{ TINI_DispStatus,			"ステータスバー Yes:表示 No:非表示" },
	// [MOVIE]
	{ TINI_AviBpp,				"ビデオキャプチャ色深度 (16,24,32)bit" },
	{ TINI_AviZoom,				"ビデオキャプチャ時ウィンドウ表示倍率(%)" },
	{ TINI_AviFrameSkip,		"ビデオキャプチャ時フレームスキップ" },
	{ TINI_AviScanLine,			"ビデオキャプチャ時スキャンライン Yes:あり No:なし" },
	{ TINI_AviScanLineBr,		"ビデオキャプチャ時スキャンライン輝度 (0-100)%" },
	{ TINI_AviDispNTSC,			"ビデオキャプチャ時4:3表示 Yes:有効 No:無効" },
	{ TINI_AviFiltering,		"ビデオキャプチャ時フィルタリング Yes:アンチエイリアシング No:ニアレストネイバー" },
	// [SOUND]
	{ TINI_SampleRate,			"サンプリングレート (44100/22050/11025)Hz" },
	{ TINI_SoundBuffer,			"サウンドバッファサイズ" },
	{ TINI_MasterVolume,		"マスター音量 (0-100)" },
	{ TINI_PsgVolume,			"PSG音量 (0-100)" },
	{ TINI_PsgLPF,				"PSG LPFカットオフ周波数(0で無効)" },
	{ TINI_VoiceVolume,			"音声合成音量 (0-100)" },
	{ TINI_TapeVolume,			"TAPEモニタ音量 (0-100)" },
	{ TINI_TapeLPF,				"TAPE LPFカットオフ周波数(0で無効)" },
	// [FILES]
	{ TINI_ExtRom,				"拡張ROMファイル名" },
	{ TINI_tape,				"TAPE(LODE)ファイル名(起動時に自動マウント)" },
	{ TINI_save,				"TAPE(SAVE)ファイル名(SAVE時に自動マウント)" },
	{ TINI_disk1,				"DISK1ファイル名(起動時に自動マウント)" },
	{ TINI_disk2,				"DISK2ファイル名(起動時に自動マウント)" },
	{ TINI_printer,				"プリンタ出力ファイル名" },
	// [PATH]
	{ TINI_RomPath,				"ROMイメージ格納パス" },
	{ TINI_TapePath,			"TAPEイメージ格納パス" },
	{ TINI_DiskPath,			"DISKイメージ格納パス" },
	{ TINI_ExtRomPath,			"拡張ROMイメージ格納パス" },
	{ TINI_ImgPath,				"スナップショット格納パス" },
	{ TINI_WavePath,			"WAVEファイル格納パス" },
	{ TINI_FontPath,			"FONTファイル格納パス" },
	{ TINI_DokoPath,			"どこでもSAVE格納パス" },
	// [CHECK]
	{ TINI_CkQuit,				"終了時確認 Yes:する No:しない" },
	{ TINI_SaveQuit,			"終了時INIファイルを保存 Yes:する No:しない" },
	// [OPTION]
	{ TINI_ExCartridge,			"拡張カートリッジ 0:なし" },
	
	// どこでもSAVE用メッセージ ------
	{ TDOK_TITLE,				"; === PC6001V どこでもSAVEファイル ===\n\n" },
	
	// Error用メッセージ ------
	{ TERR_ERROR,				"Error" },
	{ TERR_WARNING,				"Warning" },
	{ TERR_NoError,				"エラーはありません" },
	{ TERR_Unknown,				"原因不明のエラーが発生しました" },
	{ TERR_MemAllocFailed,		"メモリの確保に失敗しました" },
	{ TERR_RomChange,			"指定された機種のROMイメージが見つからないため機種を変更しました" },
	{ TERR_NoRom,				"ROMイメージが見つかりません\n設定とファイル名を確認してください" },
	{ TERR_NoRomChange,			"ROMイメージが見つかりません\n他の機種を検索しますか？" },
	{ TERR_RomSizeNG,			"ROMイメージのサイズが不正です" },
	{ TERR_RomCrcNG,			"ROMイメージのCRCが不正です\nCRCが一致しないROMを使用すると、予期せぬ不具合を引き起こす可能性があります。\nそれでも起動しますか？" },
	{ TERR_LibInitFailed,		"ライブラリの初期化に失敗しました" },
	{ TERR_InitFailed,			"初期化に失敗しました\n設定を確認してください" },
	{ TERR_FontLoadFailed,		"フォントの読込みに失敗しました" },
	{ TERR_FontCreateFailed,	"フォントファイルの作成に失敗しました" },
	{ TERR_IniDefault,			"INIファイルの読込みに失敗しました\nデフォルト設定で起動します" },
	{ TERR_IniReadFailed,		"INIファイルの読込みに失敗しました" },
	{ TERR_IniWriteFailed,		"INIファイルの保存に失敗しました" },
	{ TERR_TapeMountFailed,		"TAPEイメージのマウントに失敗しました" },
	{ TERR_DiskMountFailed,		"DISKイメージのマウントに失敗しました" },
	{ TERR_ExtRomMountFailed,	"拡張ROMイメージのマウントに失敗しました" },
	{ TERR_DokoReadFailed,		"どこでもLOADに失敗しました" },
	{ TERR_DokoWriteFailed,		"どこでもSAVEに失敗しました" },
	{ TERR_DokoDiffVersion,		"どこでもSAVEファイルのバージョンが異なるため\n正しく動作しない可能性があります\n続けますか？" },
	{ TERR_ReplayPlayError,		"リプレイ再生に失敗しました" },
	{ TERR_ReplayRecError,		"リプレイ記録に失敗しました" },
	{ TERR_NoReplayData,		"リプレイデータがありません" },
	{ TERR_CaptureFailed,		"ビデオキャプチャに失敗しました" }
};


/////////////////////////////////////////////////////////////////////////////
// 色の名前テーブル
/////////////////////////////////////////////////////////////////////////////
static const std::vector<std::string> JColorName = {
	"MODE1,2 黒(ボーダー)",
	"MODE1 Set1 緑",			"MODE1 Set1 深緑",			"MODE1 Set2 橙",			"MODE1 Set2 深橙",
	"MODE2/3 緑",				"MODE2/3 黄",				"MODE2/3 青",				"MODE2/3 赤",
	"MODE2/3 白",				"MODE2/3 シアン",			"MODE2/3 マゼンタ",			"MODE2/3 橙",
	"MODE4 Set1 深緑",			"MODE4 Set1 緑",			"MODE4 Set2 黒",			"MODE4 Set2 白",
	"MODE4 Set1 にじみ 赤",		"MODE4 Set1 にじみ 青",		"MODE4 Set1 にじみ 桃",		"MODE4 Set1 にじみ 緑",
	"MODE4 Set1 にじみ 明赤",	"MODE4 Set1 にじみ 暗赤",	"MODE4 Set1 にじみ 明青",	"MODE4 Set1 にじみ 暗青",
	"MODE4 Set1 にじみ 明桃",	"MODE4 Set1 にじみ 暗桃",	"MODE4 Set1 にじみ 明緑",	"MODE4 Set1 にじみ 暗緑",
	"MODE4 Set2 にじみ 赤",		"MODE4 Set2 にじみ 青",		"MODE4 Set2 にじみ 桃",		"MODE4 Set2 にじみ 緑",
	"MODE4 Set2 にじみ 明赤",	"MODE4 Set2 にじみ 暗赤",	"MODE4 Set2 にじみ 明青",	"MODE4 Set2 にじみ 暗青",
	"MODE4 Set2 にじみ 明桃",	"MODE4 Set2 にじみ 暗桃",	"MODE4 Set2 にじみ 明緑",	"MODE4 Set2 にじみ 暗緑",
	"RGB 透明(黒)",				"RGB 橙",					"RGB 青緑",					"RGB 黄緑",
	"RGB 青紫",					"RGB 赤紫",					"RGB 空色",					"RGB 灰色",
	"RGB 黒",					"RGB 赤",					"RGB 緑",					"RGB 黄",
	"RGB 青",					"RGB マゼンタ",				"RGB シアン",				"RGB 白"
};


/////////////////////////////////////////////////////////////////////////////
// 仮想キーコード -> 名称 変換テーブル
/////////////////////////////////////////////////////////////////////////////
static const std::unordered_map<PCKEYsym, const std::string> VKname =
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
	{ KVC_HANZEN,		"半角/全角" },
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
	
	{ KVC_P0,			"0(テンキー)" },
	{ KVC_P1,			"1(テンキー)" },
	{ KVC_P2,			"2(テンキー)" },
	{ KVC_P3,			"3(テンキー)" },
	{ KVC_P4,			"4(テンキー)" },
	{ KVC_P5,			"5(テンキー)" },
	{ KVC_P6,			"6(テンキー)" },
	{ KVC_P7,			"7(テンキー)" },
	{ KVC_P8,			"8(テンキー)" },
	{ KVC_P9,			"9(テンキー)" },
	{ KVC_NUMLOCK,		"NumLock" },
	{ KVC_P_PLUS,		"+(テンキー)" },
	{ KVC_P_MINUS,		"-(テンキー)" },
	{ KVC_P_MULTIPLY,	"*(テンキー)" },
	{ KVC_P_DIVIDE,		"/(テンキー)" },
	{ KVC_P_PERIOD,		".(テンキー)" },
	{ KVC_P_ENTER,		"Enter(テンキー)" },
	
	// 日本語キーボードのみ
	{ KVC_YEN,			"\\" },
	{ KVC_RBRACKET,		"]" },
	{ KVC_UNDERSCORE,	"_" },
	{ KVC_MUHENKAN,		"無変換" },
	{ KVC_HENKAN,		"変換" },
	{ KVC_HIRAGANA,		"カタカナ/ひらがな" },
	
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
static const std::unordered_map<PCKEYsym, const BYTE> VKChar0 =
{
	{ KVC_UNKNOWN,		0 },
	
	{ KVC_1,			'1' },
	{ KVC_2,			'2' },
	{ KVC_3,			'3' },
	{ KVC_4,			'4' },
	{ KVC_5,			'5' },
	{ KVC_6,			'6' },
	{ KVC_7,			'7' },
	{ KVC_8,			'8' },
	{ KVC_9,			'9' },
	{ KVC_0,			'0' },
	
	{ KVC_A,			'a' },
	{ KVC_B,			'b' },
	{ KVC_C,			'c' },
	{ KVC_D,			'd' },
	{ KVC_E,			'e' },
	{ KVC_F,			'f' },
	{ KVC_G,			'g' },
	{ KVC_H,			'h' },
	{ KVC_I,			'i' },
	{ KVC_J,			'j' },
	{ KVC_K,			'k' },
	{ KVC_L,			'l' },
	{ KVC_M,			'm' },
	{ KVC_N,			'n' },
	{ KVC_O,			'o' },
	{ KVC_P,			'p' },
	{ KVC_Q,			'q' },
	{ KVC_R,			'r' },
	{ KVC_S,			's' },
	{ KVC_T,			't' },
	{ KVC_U,			'u' },
	{ KVC_V,			'v' },
	{ KVC_W,			'w' },
	{ KVC_X,			'x' },
	{ KVC_Y,			'y' },
	{ KVC_Z,			'z' },
	
	{ KVC_F1,			0 },	// F1
	{ KVC_F2,			0 },	// F2
	{ KVC_F3,			0 },	// F3
	{ KVC_F4,			0 },	// F4
	{ KVC_F5,			0 },	// F5
	{ KVC_F6,			0 },	// F6
	{ KVC_F7,			0 },	// F7
	{ KVC_F8,			0 },	// F8
	{ KVC_F9,			0 },	// F9
	{ KVC_F10,			0 },	// F10
	{ KVC_F11,			0 },	// F11
	{ KVC_F12,			0 },	// F12
	
	{ KVC_MINUS,		'-' },
	{ KVC_CARET,		'^' },
	{ KVC_AT,			'@' },
	{ KVC_LBRACKET,		'[' },
	{ KVC_RBRACKET,		']' },
	{ KVC_SEMICOLON,	';' },
	{ KVC_COLON,		':' },
	{ KVC_COMMA,		',' },
	{ KVC_PERIOD,		'.' },
	{ KVC_SLASH,		'/' },
	{ KVC_SPACE,		' ' },
	
	{ KVC_BACKSPACE,	0 },	// BackSpace
	{ KVC_ESC,			0 },	// ESC
	{ KVC_TAB,			0 },	// Tab
	{ KVC_CAPSLOCK,		0 },	// CapsLock
	{ KVC_ENTER,		0 },	// Enter
	{ KVC_LCTRL,		0 },	// L-Ctrl
	{ KVC_RCTRL,		0 },	// R-Ctrl
	{ KVC_LSHIFT,		0 },	// L-Shift
	{ KVC_RSHIFT,		0 },	// R-Shift
	{ KVC_LALT,			0 },	// L-Alt
	{ KVC_RALT,			0 },	// R-Alt
	{ KVC_PRINT,		0 },	// PrintScreen
	{ KVC_SCROLLLOCK,	0 },	// ScrollLock
	{ KVC_PAUSE,		0 },	// Pause
	{ KVC_INSERT,		0 },	// Insert
	{ KVC_DELETE,		0 },	// Delete
	{ KVC_END,			0 },	// End
	{ KVC_HOME,			0 },	// Home
	{ KVC_PAGEUP,		0 },	// PageUp
	{ KVC_PAGEDOWN,		0 },	// PageDown
	
	{ KVC_UP,			0 },	// ↑
	{ KVC_DOWN,			0 },	// ↓
	{ KVC_LEFT,			0 },	// ←
	{ KVC_RIGHT,		0 },	// →
	
	{ KVC_P0,			'0' },	// [0]
	{ KVC_P1,			'1' },	// [1]
	{ KVC_P2,			'2' },	// [2]
	{ KVC_P3,			'3' },	// [3]
	{ KVC_P4,			'4' },	// [4]
	{ KVC_P5,			'5' },	// [5]
	{ KVC_P6,			'6' },	// [6]
	{ KVC_P7,			'7' },	// [7]
	{ KVC_P8,			'8' },	// [8]
	{ KVC_P9,			'9' },	// [9]
	{ KVC_P_PLUS,		'+' },	// [+]
	{ KVC_P_MINUS,		'-' },	// [-]
	{ KVC_P_MULTIPLY,	'*' },	// [*]
	{ KVC_P_DIVIDE,		'/' },	// [/]
	{ KVC_P_PERIOD,		'.' },	// [.]
	{ KVC_NUMLOCK,		0 },	// NumLock
	{ KVC_P_ENTER,		0 },	// [Enter]
	
	//
	{ KVC_UNDERSCORE,	'\\' },
	{ KVC_YEN,			'\\' },
	{ KVC_HIRAGANA,		0 },	// カタカナ/ひらがな
	{ KVC_HENKAN,		0 },	// 変換
	{ KVC_MUHENKAN,		0 },	// 無変換
	{ KVC_HANZEN,		0 },	// 半角/全角
	
	//
	{ KVX_LMETA,		0 },	// L-Meta
	{ KVX_RMETA,		0 },	// R-Meta
	{ KVX_MENU,			0 }		// Menu
};


/////////////////////////////////////////////////////////////////////////////
// 仮想キーコード  -> 文字コード 変換テーブル(SHIFT)
/////////////////////////////////////////////////////////////////////////////
static const std::unordered_map<PCKEYsym, const BYTE> VKChar1 =
{
	{ KVC_UNKNOWN,		0 },
	
	{ KVC_1,			'!' },
	{ KVC_2,			'\"' },
	{ KVC_3,			'#' },
	{ KVC_4,			'$' },
	{ KVC_5,			'%' },
	{ KVC_6,			'&' },
	{ KVC_7,			'\'' },
	{ KVC_8,			'(' },
	{ KVC_9,			')' },
	{ KVC_0,			0 },
	
	{ KVC_A,			'A' },
	{ KVC_B,			'B' },
	{ KVC_C,			'C' },
	{ KVC_D,			'D' },
	{ KVC_E,			'E' },
	{ KVC_F,			'F' },
	{ KVC_G,			'G' },
	{ KVC_H,			'H' },
	{ KVC_I,			'I' },
	{ KVC_J,			'J' },
	{ KVC_K,			'K' },
	{ KVC_L,			'L' },
	{ KVC_M,			'M' },
	{ KVC_N,			'N' },
	{ KVC_O,			'O' },
	{ KVC_P,			'P' },
	{ KVC_Q,			'Q' },
	{ KVC_R,			'R' },
	{ KVC_S,			'S' },
	{ KVC_T,			'T' },
	{ KVC_U,			'U' },
	{ KVC_V,			'V' },
	{ KVC_W,			'W' },
	{ KVC_X,			'X' },
	{ KVC_Y,			'Y' },
	{ KVC_Z,			'Z' },
	
	{ KVC_F1,			0 },	// F1
	{ KVC_F2,			0 },	// F2
	{ KVC_F3,			0 },	// F3
	{ KVC_F4,			0 },	// F4
	{ KVC_F5,			0 },	// F5
	{ KVC_F6,			0 },	// F6
	{ KVC_F7,			0 },	// F7
	{ KVC_F8,			0 },	// F8
	{ KVC_F9,			0 },	// F9
	{ KVC_F10,			0 },	// F10
	{ KVC_F11,			0 },	// F11
	{ KVC_F12,			0 },	// F12
	
	{ KVC_MINUS,		'=' },
	{ KVC_CARET,		'~' },
	{ KVC_AT,			'`' },
	{ KVC_LBRACKET,		'{' },
	{ KVC_RBRACKET,		'}' },
	{ KVC_SEMICOLON,	'+' },
	{ KVC_COLON,		'*' },
	{ KVC_COMMA,		'<' },
	{ KVC_PERIOD,		'>' },
	{ KVC_SLASH,		'?' },
	{ KVC_SPACE,		' ' },
	
	{ KVC_BACKSPACE,	0 },	// BackSpace
	{ KVC_ESC,			0 },	// ESC
	{ KVC_TAB,			0 },	// Tab
	{ KVC_CAPSLOCK,		0 },	// CapsLock
	{ KVC_ENTER,		0 },	// Enter
	{ KVC_LCTRL,		0 },	// L-Ctrl
	{ KVC_RCTRL,		0 },	// R-Ctrl
	{ KVC_LSHIFT,		0 },	// L-Shift
	{ KVC_RSHIFT,		0 },	// R-Shift
	{ KVC_LALT,			0 },	// L-Alt
	{ KVC_RALT,			0 },	// R-Alt
	{ KVC_PRINT,		0 },	// PrintScreen
	{ KVC_SCROLLLOCK,	0 },	// ScrollLock
	{ KVC_PAUSE,		0 },	// Pause
	{ KVC_INSERT,		0 },	// Insert
	{ KVC_DELETE,		0 },	// Delete
	{ KVC_END,			0 },	// End
	{ KVC_HOME,			0 },	// Home
	{ KVC_PAGEUP,		0 },	// PageUp
	{ KVC_PAGEDOWN,		0 },	// PageDown
	
	{ KVC_UP,			0 },	// ↑
	{ KVC_DOWN,			0 },	// ↓
	{ KVC_LEFT,			0 },	// ←
	{ KVC_RIGHT,		0 },	// →
	
	{ KVC_P0,			0 },	// [0]
	{ KVC_P1,			0 },	// [1]
	{ KVC_P2,			0 },	// [2]
	{ KVC_P3,			0 },	// [3]
	{ KVC_P4,			0 },	// [4]
	{ KVC_P5,			0 },	// [5]
	{ KVC_P6,			0 },	// [6]
	{ KVC_P7,			0 },	// [7]
	{ KVC_P8,			0 },	// [8]
	{ KVC_P9,			0 },	// [9]
	{ KVC_P_PLUS,		'+' },	// [+]
	{ KVC_P_MINUS,		'-' },	// [-]
	{ KVC_P_MULTIPLY,	'*' },	// [*]
	{ KVC_P_DIVIDE,		'/' },	// [/]
	{ KVC_P_PERIOD,		'.' },	// [.]
	{ KVC_NUMLOCK,		0 },	// NumLock
	{ KVC_P_ENTER,		0 },	// [Enter]
	
	//
	{ KVC_UNDERSCORE,	'_' },
	{ KVC_YEN,			'|' },
	{ KVC_HIRAGANA,		0 },	// カタカナ/ひらがな
	{ KVC_HENKAN,		0 },	// 変換
	{ KVC_MUHENKAN,		0 },	// 無変換
	{ KVC_HANZEN,		0 },	// 半角/全角
	
	//
	{ KVX_LMETA,		0 },	// L-Meta
	{ KVX_RMETA,		0 },	// R-Meta
	{ KVX_MENU,			0 }		// Menu
};


/////////////////////////////////////////////////////////////////////////////
// メッセージ文字列取得
//
// 引数:	id				メッセージID
// 返値:	std::string&	メッセージ文字列への参照
/////////////////////////////////////////////////////////////////////////////
const std::string& GetText( TextID id )
{
	try{
		return MsgString.at( id );
	}
	catch( std::out_of_range& ){
		return MsgString.at( T_EMPTY );	// "???"
	}
}


/////////////////////////////////////////////////////////////////////////////
// 色の名前取得
//
// 引数:	num				色コード
// 返値:	std::string&	色の名前文字列への参照
/////////////////////////////////////////////////////////////////////////////
const std::string& GetColorName( int num )
{
	try{
		return JColorName.at( num );
	}
	catch( std::out_of_range& ){
		return MsgString.at( T_EMPTY );	// "???"
	}
}


/////////////////////////////////////////////////////////////////////////////
// キーの名前取得
//
// 引数:	sym				仮想キーコード
// 返値:	std::string&	キー名前文字列への参照
/////////////////////////////////////////////////////////////////////////////
const std::string& GetKeyName( PCKEYsym sym )
{
	try{
		return VKname.at( sym );
	}
	catch( std::out_of_range& ){
		return VKname.at( KVC_UNKNOWN );
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
		return shift ? VKChar1.at( sym ) : VKChar0.at( sym );
	}
	catch( std::out_of_range& ){
		return VKChar0.at( KVC_UNKNOWN );
	}
}
