#include "log.h"
#include "movie.h"
#include "common.h"


// [参考] MSDN
// AVI ファイル フォーマット
// http://msdn.microsoft.com/library/ja/default.asp?url=/library/ja/directx9_c/directx/htm/avifileformat.asp
// AVI ファイル フォーマットの DV データ(アーカイブコンテンツ)
// http://msdn.microsoft.com/library/ja/default.asp?url=/library/ja/jpdx8_c/ds/filtdev/DV_Data_AVI_File_Format.asp

// 16bitモードについてのメモ
// SDLは R:G:B=5:6:5 らしいのだがこのフォーマットはQuickTimeで上手く再生できないようなので
// R:G:B=5:5:5 としてみる。

#define	CIDVIDS		(CTODW( 'v', 'i', 'd', 's' ))
#define	CIDAUDS		(CTODW( 'a', 'u', 'd', 's' ))
#define	CIDRLE		(CTODW( 'R', 'L', 'E', ' ' ))
#define	CIDDIB		(CTODW( 'D', 'I', 'B', ' ' ))
#define	CID00DC		(CTODW( '0', '0', 'd', 'c' ))
#define	CID00DB		(CTODW( '0', '0', 'd', 'b' ))
#define	CID01WB		(CTODW( '0', '1', 'w', 'b' ))


////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
AVI6::AVI6( void ) : vfp(NULL), ABPP(8), AVIRLE(true), PosMOVI(0),
						RiffSize(0), MoviSize(0), anum(0) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
AVI6::~AVI6( void )
{
	if( IsAVI() ) StopAVI();
}


////////////////////////////////////////////////////////////////
// 各種ヘッダ出力
////////////////////////////////////////////////////////////////
void AVI6::putBMPINFOHEADER6( BMPINFOHEADER6 *p )
{
	FPUTDWORD( p->biSize,			vfp );
	FPUTDWORD( p->biWidth,			vfp );	// LONG
	FPUTDWORD( p->biHeight,			vfp );	// LONG
	FPUTWORD(  p->biPlanes,			vfp );
	FPUTWORD(  p->biBitCount,		vfp );
	FPUTDWORD( p->biCompression,	vfp );
	FPUTDWORD( p->biSizeImage,		vfp );
	FPUTDWORD( p->biXPelsPerMeter,	vfp );	// LONG
	FPUTDWORD( p->biYPelsPerMeter,	vfp );	// LONG
	FPUTDWORD( p->biClrUsed,		vfp );
	FPUTDWORD( p->biClrImportant,	vfp );
}

void AVI6::putMAINAVIHEADER6( MAINAVIHEADER6 *p )
{
	FPUTDWORD( p->dwMicroSecPerFrame,		vfp );
	FPUTDWORD( p->dwMaxBytesPerSec,			vfp );
	FPUTDWORD( p->dwReserved1,				vfp );
	FPUTDWORD( p->dwFlags,					vfp );
	FPUTDWORD( p->dwTotalFrames,			vfp );
	FPUTDWORD( p->dwInitialFrames,			vfp );
	FPUTDWORD( p->dwStreams,				vfp );
	FPUTDWORD( p->dwSuggestedBufferSize,	vfp );
	FPUTDWORD( p->dwWidth,					vfp );
	FPUTDWORD( p->dwHeight,					vfp );
	fwrite( (DWORD *)&p->dwReserved ,sizeof(DWORD), 4, vfp );
}

void AVI6::putAVISTRMHEADER6( AVISTRMHEADER6 *p )
{
	FPUTDWORD( p->fccType,					vfp );
	FPUTDWORD( p->fccHandler,				vfp );
	FPUTDWORD( p->dwFlags,					vfp );
	FPUTDWORD( p->dwPriority,				vfp );
	FPUTDWORD( p->dwInitialFrames,			vfp );
	FPUTDWORD( p->dwScale,					vfp );
	FPUTDWORD( p->dwRate,					vfp );
	FPUTDWORD( p->dwStart,					vfp );
	FPUTDWORD( p->dwLength,					vfp );
	FPUTDWORD( p->dwSuggestedBufferSize,	vfp );
	FPUTDWORD( p->dwQuality,				vfp );
	FPUTDWORD( p->dwSampleSize,				vfp );
	
	FPUTDWORD( p->rcFrame.left,		vfp );	// LONG
	FPUTDWORD( p->rcFrame.top,		vfp );	// LONG
	FPUTDWORD( p->rcFrame.right,	vfp );	// LONG
	FPUTDWORD( p->rcFrame.bottom,	vfp );	// LONG
}

// AVIINDEXENTRY相当
void AVI6::putAVIINDEXENTRY6( AVIINDEXENTRY6 *p )
{
	FPUTDWORD( p->ckid,				vfp );
	FPUTDWORD( p->dwFlags,			vfp );
	FPUTDWORD( p->dwChunkOffset,	vfp );
	FPUTDWORD( p->dwChunkLength,	vfp );
} 

// WAVEFORMATEX相当
void AVI6::putWAVEFORMATEX6( WAVEFORMATEX6 *p )
{
	FPUTWORD( p->wFormatTag,		vfp );
	FPUTWORD( p->nChannels,			vfp );
	FPUTDWORD(p->nSamplesPerSec,	vfp );
	FPUTDWORD(p->nAvgBytesPerSec,	vfp );
	FPUTWORD( p->nBlockAlign,		vfp );
	FPUTWORD( p->wBitsPerSample,	vfp );
	FPUTWORD( p->cbSize,			vfp );
	
	FPUTWORD( 0,            		vfp );
}




////////////////////////////////////////////////////////////////
// 初期化
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool AVI6::Init( void )
{
	PRINTD( GRP_LOG, "[MOVIE][Init]\n" );
	
	if( vfp ) fclose( vfp );
	vfp = NULL;
	
	ZeroMemory( &vmh, sizeof(MAINAVIHEADER6) );
	ZeroMemory( &vsh, sizeof(AVISTRMHEADER6) );
	ZeroMemory( &ash, sizeof(AVISTRMHEADER6) );
	ZeroMemory( &vbf, sizeof(BMPINFOHEADER6) );
	ZeroMemory( &pal, sizeof(RGBPAL6)*256 );
	ZeroMemory( &awf, sizeof(WAVEFORMATEX6) );
	
	ABPP     = 8;
	AVIRLE   = true;
	
	PosMOVI  = 0;
	
	RiffSize = 0;
	MoviSize = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ビデオキャプチャ開始
//
// 引数:	filename	出力ファイル名
//			sbuf		サブバッファへのポインタ
//			vrate		フレームレート(fps)
//			arate		音声サンプリングレート(Hz)
//			rle			true:RLE使う false:RLE使わない
// 返値:	bool		true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool AVI6::StartAVI( char *filename, VSurface *sbuf, int vrate, int arate, bool rle )
{
	Init();
	
	vfp = FOPENEN( filename, "w+b" );
	if( !vfp ) return false;
	
	switch( sbuf->Bpp() ){
	case 24: ABPP = 24; AVIRLE = false; break;
	case 16: ABPP = 16; AVIRLE = false; break;
	default: ABPP =  8; AVIRLE = rle;
	}
	
	// AVIメインヘッダーの設定
	// フレーム間の間隔をマイクロ秒単位で指定する。この値はファイルの全体のタイミングを示す。
	vmh.dwMicroSecPerFrame = 1000000 / vrate;
	// ファイルの概算最大データレートを指定する。この値は,メインヘッダーおよびストリームヘッダーチャンクに含まれる
	// 他のパラメータに従って AVIシーケンスを表示するために,システムが処理しなければならない毎秒のバイト数を示す。
	// (とりあえず無視)
	vmh.dwMaxBytesPerSec = 0;
	// ファイルに対するフラグを含む。以下のフラグが定義されている。
	//  AVIF_HASINDEX AVI	ファイルの終わりにインデックスを含む 'idx1'チャンクがあることを示す。
	//						良好なパフォーマンスのためには,すべての AVIファイルがインデックスを含む必要がある。
	//  AVIF_MUSTUSEINDEX	データのプレゼンテーションの順序を決定するために,ファイル内のチャンクの物理的な順序ではなく
	//						インデックスを使用することを示す。たとえば,これを使ってフレームのリストを作成して編集することができる。
	//  AVIF_ISINTERLEAVED	AVIファイルがインターリーブされていることを示す。
	//  AVIF_WASCAPTUREFILE	AVIファイルが,リアルタイムビデオのキャプチャ用に特別に割り当てられたファイルであることを示す。
	//						アプリケーションは,このフラグが設定されたファイルをオーバーライドする前に,ユーザーに警告を発する必要がある。
	//						これは,ユーザーがこのファイルをデフラグメントしている可能性が高いからである。
	//  AVIF_COPYRIGHTED	AVIファイルに著作権のあるデータおよびソフトウェアが含まれていることを示す。
	//						このフラグが使用されている場合,ソフトウェアはデータの複製を許可すべきではない。
	vmh.dwFlags = 0x00000110;	// AVIF_HASINDEX | AVIF_ISINTERLEAVED
	// ファイル内のデータのフレームの総数を指定する。
	// (後で)
	vmh.dwTotalFrames = 0;
	// インターリーブされたファイルの開始フレームを指定する。インターリーブされたファイル以外では,ゼロを指定する。
	// インターリーブされたファイルを作成する場合,ファイル内で AVIシーケンスの開始フレームより前にあるフレーム数を
	// このメンバに指定する。このメンバの内容に関する詳細については
	// 『Video for Windows Programmer's Guide』の「Special Information for Interleaved Files」を参照すること。
	vmh.dwInitialFrames = 0;
	// ファイル内のストリーム数を指定する。たとえば,オーディオとビデオを含むファイルには 2つのストリームがある。
	vmh.dwStreams = 2;
	// ファイルを読み取るためのバッファサイズを指定する。一般に,このサイズはファイル内の最大のチャンクを格納するのに
	// 十分な大きさにする。ゼロに設定したり,小さすぎる値に設定した場合,再生ソフトウェアは再生中にメモリを
	// 再割り当てしなければならず,パフォーマンスが低下する。インターリーブされたファイルの場合,バッファサイズはチャンクではなく
	// レコード全体を読み取るのに十分な大きさでなければならない。
	// (とりあえず1フレーム分)
	vmh.dwSuggestedBufferSize = sbuf->Width() * sbuf->Height() * ( ABPP / 8 );
	// AVIファイルの幅をピクセル単位で指定する。
	vmh.dwWidth  = sbuf->Width();
	// AVIファイルの高さをピクセル単位で指定する。
	vmh.dwHeight = sbuf->Height();
	
	// AVIストリームヘッダーの設定
	// ストリームに含まれるデータのタイプを指定する FOURCC を含む。ビデオおよびオーディオに対して以下の標準AVI値が定義されている。
	//  'vids'	ストリームにビデオデータが含まれることを示す。ストリームフォーマットチャンクには,パレット情報を含むことが可能な
	//			BITMAPINFO 構造体が含まれる。
	//  'auds'	ストリームにオーディオデータが含まれることを示す。ストリームフォーマットチャンクには,WAVEFORMATEX
	//			または PCMWAVEFORMAT 構造体が含まれる。
	//  'txts'	ストリームにテキスト データが含まれることを示す。
	vsh.fccType = CIDVIDS;
	ash.fccType = CIDAUDS;
	// 特定のデータハンドラを示す FOURCC を含む (オプション)。データハンドラは,ストリームに対して適切なハンドラである。
	// オーディオおよびビデオストリームの場合は,インストール可能なコンプレッサまたはデコンプレッサを指定する。
	vsh.fccHandler = ( (ABPP==8) && AVIRLE ) ? CIDRLE  : CIDDIB;
	ash.fccHandler = 0x00000001;	//(PCM)
	// データ ストリームに対するフラグを含む。これらのフラグの上位ワードのビットは,ストリームに含まれるデータのタイプに固有である。
	// 以下の標準フラグが定義されている。
	//  AVISF_DISABLED			このストリームをデフォルトで有効にしないことを示す。
	//  AVISF_VIDEO_PALCHANGES	このビデオストリームにパレットの変更が含まれることを示す。このフラグは,再生ソフトウェアに対して
	//							パレットをアニメーションする必要があることを警告する。
	vsh.dwFlags = 0;
	ash.dwFlags = 0;
	// ストリーム タイプの優先順位を指定する。たとえば,複数のオーディオストリームを含むファイルでは,優先順位の最も高いストリームが
	// デフォルトのストリームになる。
	vsh.dwPriority = 0;
	ash.dwPriority = 0;
	// インターリーブされたファイルで,オーディオデータがビデオフレームからどのくらいスキューされているかを指定する。
	// 通常は,約 0.75 秒である。インターリーブされたファイルを作成する場合,ファイル内で AVIシーケンスの開始フレームより
	// 前にあるフレーム数を,このメンバに指定する。このメンバの内容に関する詳細については,『Video for Windows Programmer's Guide』の
	// 「Special Information for Interleaved Files」を参照すること。
	vsh.dwInitialFrames = 0;
	ash.dwInitialFrames = 0;
	// dwRate とともに使用して,このストリームが使用するタイムスケールを指定する。dwRate を dwScale で割ることにより
	// 1 秒あたりのサンプル数が求められる。ビデオストリームの場合,このレートはフレームレートに等しくなる。
	// オーディオストリームの場合,このレートは nBlockAlign バイトのオーディオデータに必要な時間に対応する。
	// これは PCMオーディオの場合はサンプルレートに等しくなる。
	vsh.dwScale = 10000;
	ash.dwScale = 2;
	// dwScale を参照すること。
	vsh.dwRate = vsh.dwScale * vrate;
	ash.dwRate = ash.dwScale * arate;
	// AVIファイルの開始タイムを指定する。単位は,メインファイルヘッダーの dwRate および dwScale メンバによって定義される。
	// 通常,これはゼロであるが,ファイルと同時に開始されないストリームに対しては,遅延時間を指定することができる。
	vsh.dwStart = 0;
	ash.dwStart = 0;
	// このストリームの長さを指定する。単位は,ストリームのヘッダーの dwRate および dwScale メンバによって定義される。
	// (後で)
	vsh.dwLength = 0;
	ash.dwLength = 0;
	// このストリームを読み取るために必要なバッファの大きさを指定する。通常は,ストリーム内の最大のチャンクに対応する値である。
	// 正しいバッファサイズを使用することで,再生の効率が高まる。正しいバッファサイズがわからない場合は,ゼロを指定する。
	// (とりあえず1フレーム分)
	vsh.dwSuggestedBufferSize = sbuf->Width() * sbuf->Height() * ( ABPP / 8 );
	ash.dwSuggestedBufferSize = arate / vrate * 2;
	// ストリーム内のデータの品質を示す値を指定する。品質は,0〜10,000 の範囲の値で示される。圧縮データの場合,これは通常
	// 圧縮ソフトウェアに渡される品質パラメータの値を示す。-1に設定した場合,ドライバはデフォルトの品質値を使用する。
	vsh.dwQuality = 0;
	ash.dwQuality = 0;
	// データの 1 サンプルのサイズを指定する。サンプルのサイズが変化する場合は,ゼロに設定する。この値がゼロでない場合
	// ファイル内で複数のデータサンプルを 1つのチャンクにグループ化できる。ゼロの場合,各データサンプル(ビデオフレームなど)
	// はそれぞれ別のチャンクに含まれなければならない。ビデオストリームの場合,この値は通常ゼロであるが
	// すべてのビデオフレームが同じサイズであれば,ゼロ以外の値にもできる。オーディオストリームの場合,この値はオーディオを
	// 記述する WAVEFORMATEX 構造体の nBlockAlign メンバと同じでなければならない。
	vsh.dwSampleSize = 0;
	ash.dwSampleSize = 2;
	// AVI メインヘッダー構造体の dwWidth および dwHeight メンバによって指定される動画矩形内のテキストまたはビデオストリームに
	// 対する転送先矩形を指定する。通常,rcFrame メンバは,複数のビデオストリームをサポートするために使用される。この矩形は
	// 動画矩形に対応する座標に設定して,動画矩形全体を更新する。このメンバの単位はピクセルである。転送先矩形の左上隅は
	// 動画矩形の左上隅からの相対指定となる。
	vsh.rcFrame.left   = 0;
	vsh.rcFrame.top    = 0;
	vsh.rcFrame.right  = sbuf->Width()  - 1;
	vsh.rcFrame.bottom = sbuf->Height() - 1;
	
	// BMPINFOHEADER6構造体の設定
	// 構造体が必要とするバイト数を指定する。
	vbf.biSize = sizeof(BMPINFOHEADER6);
	// ビットマップの幅をピクセル単位で指定する。
	vbf.biWidth  = sbuf->Width();
	// ビットマップの高さをピクセル単位で指定する。biHeight の値が正である場合,ビットマップはボトムアップDIB
	// (device-independent bitmap : デバイスに依存しないビットマップ) であり,左下隅が原点となる。biHeight の値が負である場合
	// ビットマップはトップダウンDIB であり,左上隅が原点となる。
	vbf.biHeight = sbuf->Height();
	// ターゲット デバイスに対する面の数を指定する。これは必ず 1 に設定する。
	vbf.biPlanes = 1;
	// 1 ピクセルあたりのビット数を指定する。圧縮フォーマットによっては,ピクセルの色を正しくデコードするためにこの情報が必要である。
	vbf.biBitCount = ABPP;
	// 使用されている,または要求されている圧縮のタイプを指定する。既存の圧縮フォーマットと新しい圧縮フォーマットの両方で
	// このメンバを使用する。
	switch( ABPP ){
	case 16:
	case 24: vbf.biCompression = 0; break;	// 0:BI_RGB
	default:
		if( AVIRLE ) vbf.biCompression = 1;	// 1:BI_RLE8
		else         vbf.biCompression = 0; // 0:BI_RGB
	}
	// イメージのサイズをバイト単位で指定する。非圧縮RGBビットマップの場合は,0 に設定できる。
	vbf.biSizeImage = sbuf->Width() * sbuf->Height() * ( ABPP / 8 );
	// ビットマップのターゲットデバイスの水平解像度を 1メートルあたりのピクセル単位で指定する。アプリケーションはこの値を
	// 使用して,リソースグループの中から現在のデバイスの特性に最も適合するビットマップを選択することができる。
	vbf.biXPelsPerMeter = 0;
	// ビットマップのターゲット デバイスの垂直解像度を 1メートルあたりのピクセル単位で指定する。
	vbf.biYPelsPerMeter = 0;
	// カラー テーブル内のカラー インデックスのうち,ビットマップ内で実際に使用されるインデックスの数を指定する。この値が
	// ゼロの場合,ビットマップは,biCompression で指定される圧縮モードに対して,biBitCount メンバの値に対応する最大色数を使用する。
	// Macの場合は256でないと色がおかしくなるらしい。(by 西田さん&Windyさん)
	// でも24bitの時は0にしておかないとちゃんと再生されないみたい。
	switch( ABPP ){
	case 24: vbf.biClrUsed =   0; break;
	case 16: vbf.biClrUsed =   0; break;
	default: vbf.biClrUsed = 256;
	}
	// ビットマップを表示するために重要とみなされるカラーインデックス数を指定する。この値がゼロの場合は,すべての色が重要とみなされる。
	vbf.biClrImportant = 0;
	
	// パレット取得
	if( ABPP == 8 ){
		for( int i=0; i<sbuf->GetPalette()->ncols; i++ ){
			pal[i].b        = sbuf->GetPalette()->colors[i].b;
			pal[i].g        = sbuf->GetPalette()->colors[i].g;
			pal[i].r        = sbuf->GetPalette()->colors[i].r;
			pal[i].reserved = 0;
		}
	}
	
	// WAVEFORMATEX6構造体の設定
	// オーディオストリームのオーディオ波形タイプを定義する。フォーマットタグの完全なリストは,Microsoft Visual C++ および
	// 他の Microsoft 製品に付属する Mmreg.h ヘッダー ファイルにある。
	awf.wFormatTag = 0x0001;	// WAVE_FORMAT_PCM
	// オーディオストリーム内のチャンネル数を指定する。1はモノ,2はステレオを示す。
	awf.nChannels = 1;
	// オーディオストリームのサンプルレート周波数をサンプル/秒 (Hz) で指定する。たとえば,11,025,22,050,または 44,100。
	awf.nSamplesPerSec = arate;
	// 平均データレートを指定する。再生ソフトウェアはこの値を使ってバッファサイズを見積もることができる。
	awf.nAvgBytesPerSec = arate * 2;
	// データのブロック アラインメントをバイト単位で指定する。再生ソフトウェアが 1回に処理するデータのバイト数は
	// nBlockAlign の整数倍でなければならないため,nBlockAlign の値を使用してバッファのアラインメントを行うことができる。
	awf.nBlockAlign = 2;
	// チャンネルデータごとの 1サンプルあたりのビット数を指定する。各チャンネルのサンプル解像度は同じであると仮定される。
	// このフィールドが必要ない場合は,ゼロに設定する。
	awf.wBitsPerSample = 16;
	// フォーマットヘッダー内の追加情報のサイズをバイト単位で指定する。これには,WAVEFORMATEX 構造体のサイズは含まれない。
	// たとえば,wFormatTag WAVE_FORMAT_IMA_ADPCM に対応する waveフォーマットの場合,cbSize は
	// sizeof(IMAADPCMWAVEFORMAT) - sizeof(WAVEFORMATEX) として計算され,結果は 2となる。
	awf.cbSize = 0;
	
	// ヘッダチャンク書出し
	WriteHeader();
	
	// オーディオバッファ作成
	ABuf.InitBuffer( arate / vrate * 2 );
	// カウンタ初期化
	anum = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ビデオキャプチャ停止
//
// 引数:	なし
// 返値:	なし
////////////////////////////////////////////////////////////////
void AVI6::StopAVI( void )
{
	if( !vfp ) return;
	
	// 総フレーム数
	vsh.dwLength = vmh.dwTotalFrames;
	ash.dwLength = anum;
	
	// この時点でファイルポインタは末尾にあるはずだが念のため
	fseek( vfp, 0, SEEK_END );
	
	// Moviチャンクのサイズ取得
	MoviSize = ftell( vfp ) - PosMOVI;
	
	// インデックスチャンク出力
	WriteIndexr();
	
	RiffSize = ftell( vfp ) - 8;
	
	// ヘッダチャンク再書出し
	WriteHeader();
	
	fclose( vfp );
	vfp = NULL;
}


////////////////////////////////////////////////////////////////
// ビデオキャプチャ中?
//
// 引数:	なし
// 返値:	bool	true:キャプチャ中 false:ひま
////////////////////////////////////////////////////////////////
bool AVI6::IsAVI( void )
{
	return vfp ? true : false;
}


////////////////////////////////////////////////////////////////
// AVI1フレーム書出し
//
// 引数:	sbuf	サーフェスへのポインタ
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool AVI6::AVIWriteFrame( VSurface *sbuf )
{
	if( !vfp || !sbuf ) return false;
	
	int xx = min( sbuf->Width(),  vbf.biWidth  );
	int yy = min( sbuf->Height(), vbf.biHeight );
	
	FPUTDWORD( ( (ABPP==8) && AVIRLE ) ? CID00DC : CID00DB, vfp );
	FPUTDWORD( vbf.biSizeImage, vfp );
	
	switch( ABPP ){
	case 8:		// 8bitの場合
		if( AVIRLE ){	// RLEの場合
			long fnum = 0;
			for( int y = yy - 1; y >= 0; y-- ){
				BYTE dat = ((BYTE *)sbuf->GetPixels())[y*sbuf->Pitch()];
				int num = 1;
				for( int x = 1; x < xx; x++ ){
					if( ( ((BYTE *)sbuf->GetPixels())[y*sbuf->Pitch()+x] != dat )||( num == 255 )||( x == xx - 1 ) ){
						FPUTBYTE( num, vfp );
						FPUTBYTE( dat, vfp );
						dat = ((BYTE *)sbuf->GetPixels())[y*sbuf->Pitch()+x];
						num = 1;
						fnum += 2;
					}else
						num++;
				}
				FPUTBYTE( 0, vfp );
				if( y ) FPUTBYTE( 0, vfp );
				else    FPUTBYTE( 1, vfp );
				fnum += 2;
			}
			fseek( vfp, -fnum-4, SEEK_CUR );
			FPUTDWORD( (DWORD)fnum, vfp );
			fseek( vfp, 0, SEEK_END );
		}else{			// ベタの場合
			for( int y = yy - 1; y >= 0; y-- )
				fwrite( (BYTE *)sbuf->GetPixels() + sbuf->Pitch() * y, sizeof(BYTE), xx, vfp );
		}
		break;
		
	case 16:	// 16bitの場合
		for( int y = yy - 1; y >= 0; y-- ){
			WORD *src = (WORD *)sbuf->GetPixels() + sbuf->Pitch() * y/sizeof(WORD);
			for( int x=0; x < xx; x++ ){
				WORD dat = (((((*src)&sbuf->Rmask())>>sbuf->Rshift())<<sbuf->Rloss())&0xf8) << 7 |
						   (((((*src)&sbuf->Gmask())>>sbuf->Gshift())<<sbuf->Gloss())&0xf8) << 2 |
						   (((((*src)&sbuf->Bmask())>>sbuf->Bshift())<<sbuf->Bloss())&0xf8) >> 3;
				FPUTWORD( dat, vfp );
				src++;
			}
		}
		break;
		
	case 24:	// 24bitの場合
		for( int y = yy - 1; y >= 0; y-- ){
			BYTE *src = (BYTE *)sbuf->GetPixels() + sbuf->Pitch() * y;
			for( int x=0; x < xx; x++ ){
				BYTE r,g,b;
				GET3BYTE( b, g, r, src );
				FPUTBYTE( b, vfp ); FPUTBYTE( g, vfp ); FPUTBYTE( r, vfp );
			}
		}
	}
	// 総フレーム数を1増やす
	vmh.dwTotalFrames++;
	
	// オーディオ出力
	if( ABuf.ReadySize() > 0 ){
		anum += ABuf.ReadySize();
		FPUTDWORD( CID01WB, vfp );
		FPUTDWORD( ABuf.ReadySize()*2, vfp );
		while( ABuf.ReadySize() > 0 ){
			short dat = ABuf.Get();
			FPUTWORD( dat, vfp );
		}
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// オーディオバッファ取得
//
// 引数:	なし
// 返値:	cRing *		バッファオブジェクトへのポインタ
////////////////////////////////////////////////////////////////
cRing *AVI6::GetAudioBuffer( void )
{
	return &ABuf;
}


////////////////////////////////////////////////////////////////
// ヘッダチャンク書出し
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool AVI6::WriteHeader( void )
{
	if( !vfp ) return false;
	
	DWORD SIZESTRLV	= sizeof(AVISTRMHEADER6) + sizeof(BMPINFOHEADER6) + sizeof(DWORD)*5 + (ABPP==8 ? (sizeof(RGBPAL6)*256) : 0);
	DWORD SIZESTRLA	= sizeof(AVISTRMHEADER6) + sizeof(WAVEFORMATEX6)  + sizeof(DWORD)*5;
	DWORD SIZEHDRL  = SIZESTRLV + SIZESTRLA  + sizeof(MAINAVIHEADER6) + sizeof(DWORD)*7;
	DWORD SIZEJUNK  = 2048 - SIZEHDRL - sizeof(DWORD)*10;
	
	fseek( vfp, 0, SEEK_SET );
	
	fputs( "RIFF", vfp );
	FPUTDWORD( RiffSize, vfp );
		fputs( "AVI ", vfp );
			fputs( "LIST", vfp );
			FPUTDWORD( SIZEHDRL, vfp );
			fputs( "hdrl", vfp );
				fputs( "avih", vfp );
				FPUTDWORD( sizeof(MAINAVIHEADER6), vfp );
				putMAINAVIHEADER6( &vmh );
				
				// ビデオ
				fputs( "LIST", vfp );
				FPUTDWORD( SIZESTRLV, vfp );
				fputs( "strl", vfp );
					fputs( "strh", vfp );
					FPUTDWORD( sizeof(AVISTRMHEADER6), vfp );
					putAVISTRMHEADER6( &vsh );
					
					fputs( "strf", vfp );
					switch( ABPP ){
					case 8:		// 8bitの場合
						FPUTDWORD( sizeof(BMPINFOHEADER6) + sizeof(RGBPAL6)*256, vfp );
						putBMPINFOHEADER6( &vbf );
						fwrite( &pal, sizeof(BYTE), sizeof(RGBPAL6)*256, vfp );
						break;
						
					case 16:	// 16bitの場合
					case 24:	// 24bitの場合
						FPUTDWORD( sizeof(BMPINFOHEADER6), vfp );
						putBMPINFOHEADER6( &vbf );
					}
					
				// オーディオ
				fputs( "LIST", vfp );
				FPUTDWORD( SIZESTRLA, vfp );
				fputs( "strl", vfp );
					fputs( "strh", vfp );
					FPUTDWORD( sizeof(AVISTRMHEADER6), vfp );
					putAVISTRMHEADER6( &ash );
					
					fputs( "strf", vfp );
					FPUTDWORD( sizeof(WAVEFORMATEX6), vfp );
					putWAVEFORMATEX6( &awf );
				
			fputs( "JUNK", vfp );
			FPUTDWORD( SIZEJUNK, vfp );
			for( DWORD i=0; i<SIZEJUNK; i++ ) FPUTBYTE( 0, vfp );
			
			fputs( "LIST", vfp );
			FPUTDWORD( MoviSize, vfp );
			
			PosMOVI = ftell( vfp );
			
			fputs( "movi", vfp );
	
	fseek( vfp, 0, SEEK_END );
	
	return true;
}


////////////////////////////////////////////////////////////////
// インデックスチャンク書出し
//
// 引数:	なし
// 返値:	bool	true:成功 false:失敗
////////////////////////////////////////////////////////////////
bool AVI6::WriteIndexr( void )
{
	if( !vfp ) return false;
	
	DWORD frames = vmh.dwTotalFrames * 2;
	
	fseek( vfp, 0, SEEK_END );
	
	// インデックスチャンク出力
	fputs( "idx1", vfp );
	FPUTDWORD( sizeof(AVIINDEXENTRY6) * frames, vfp );
	DWORD ipos = 4;
	for( DWORD i=0; i<frames; i++ ){
		AVIINDEXENTRY6 idx;
		
		fseek( vfp, PosMOVI + ipos, SEEK_SET );
		idx.ckid          = FGETDWORD( vfp );
//		if( idx.ckid == CID01WB ) idx.dwFlags = 0x00000000;
//		else                      idx.dwFlags = 0x00000010;	// AVIIF_KEYFRAME
		idx.dwFlags = 0x00000010;	// AVIIF_KEYFRAME
		idx.dwChunkOffset = ipos;
		idx.dwChunkLength = FGETDWORD( vfp );
		fseek( vfp, 0, SEEK_END );
		putAVIINDEXENTRY6( &idx );
		ipos += idx.dwChunkLength + 8;
	}
	
	return true;
}
