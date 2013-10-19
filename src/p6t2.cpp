#include "pc6001v.h"
#include "common.h"
#include "log.h"
#include "osd.h"
#include "p6t2.h"


// P6T形式フォーマットVer.2
//  基本的には「ベタイメージ+フッタ+ベタイメージサイズ(4byte)」という構造
//  フッタにはDATAブロック情報が含まれる。
//  DATAブロックは無音部を区切りとして1と数える。
//  BASICなど「ヘッダ＋データ」という構造の場合，DATAブロックは2個となるが
//  ID番号を同一にすることで関連付けて管理できる。
//
// [フッタ]
//  header (2byte) : "P6"
//  ver    (1byte) : バージョン
//  dbnum  (1byte) : 含まれるDATAブロック数(255個まで)
//  start  (1byte) : オートスタートフラグ(0:無効 1:有効)
//  basic  (1byte) : BASICモード(PC-6001の場合は無意味)
//  page   (1byte) : ページ数
//  askey  (2byte) : オートスタートコマンド文字数
//  ...コマンドがある場合はこの後にaskey分続く
//  exhead (2byte) : 拡張情報サイズ(64KBまで)
//  ...拡張情報がある場合はこの後にexhead分続く
//
// [DATAブロック]
//  header (2byte) : "TI"
//  id     (1byte) : ID番号(DATAブロックを関連付ける)
//  name  (16byte) : データ名(15文字+'00H')
//  baud   (2byte) : ボーレート(600/1200)
//  stime  (2byte) : 無音部の時間(ms)
//  ptime  (2byte) : ぴー音の時間(ms)
//  offset (4byte) : ベタイメージ先頭からのオフセット
//  size   (4byte) : データサイズ



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cP6DATA::cP6DATA( void ) : 	Data(NULL), next(NULL), before(NULL) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cP6DATA::~cP6DATA( void )
{
	if( Data ) delete [] Data;
	if( next ) delete next;
	if( before ) before->next = NULL;
}


////////////////////////////////////////////////////////////////
// 先頭ブロックへのポインタを返す
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::TopBlock( void )
{
	cP6DATA *top = this;
	while( top->before ) top = top->before;
	
	return top;
}


////////////////////////////////////////////////////////////////
// 末尾ブロックへのポインタを返す
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::LastBlock( void )
{
	cP6DATA *last = this;
	while( last->next ) last = last->next;
	
	return last;
}


////////////////////////////////////////////////////////////////
// 次のブロックへのポインタを返す
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::Next( void )
{
	return next;
}


////////////////////////////////////////////////////////////////
// 前のブロックへのポインタを返す
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::Before( void )
{
	return before;
}


////////////////////////////////////////////////////////////////
// 新規DATAブロック追加
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::New( void )
{
	LastBlock()->next = new cP6DATA;
	LastBlock()->before = this;
	
	return LastBlock();
}


////////////////////////////////////////////////////////////////
// コピー作成
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::Clone( void )
{
	cP6DATA *newb = new cP6DATA;
	*newb = *this;
	newb->next = newb->before = NULL;
	
	if( this->Data ){
		newb->Data = new BYTE[Info.DNum];
		memcpy( newb->Data, this->Data, Info.DNum );
	}
	
	return newb;
}


////////////////////////////////////////////////////////////////
// 全コピー作成
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::Clones( void )
{
	cP6DATA *oldb = TopBlock();
	cP6DATA *newb = TopBlock()->Clone();
	
	while( oldb->next ){
		oldb = oldb->next;
		newb->next         = oldb->Clone();
		newb->next->before = newb;
		newb = newb->LastBlock();
	}
	
	return newb->TopBlock();
}


////////////////////////////////////////////////////////////////
// P6T ブロック情報取得
////////////////////////////////////////////////////////////////
P6TBLKINFO *cP6DATA::GetInfo( void )
{
	return &Info;
}


////////////////////////////////////////////////////////////////
// データセット
////////////////////////////////////////////////////////////////
int cP6DATA::SetData( FILE *fp, int num )
{
	// セット済みなら開放
	if( Data ) delete Data;
	
	// メモリ確保
	Data = new BYTE[num];
	if( Data ) Info.DNum = num;
	else       return 0;
	
	Info.Offset = ftell( fp );
	
	if( fp ) fread( Data, sizeof(BYTE), num, fp );
	else     ZeroMemory( Data, num );
	
	return Info.DNum;
}


////////////////////////////////////////////////////////////////
// 無音部,ぴー音時間セット
////////////////////////////////////////////////////////////////
void cP6DATA::SetPeriod( int stime, int ptime )
{
	PRINTD( P6T2_LOG, "[cP6DATA][SetPeriod] s:%d p:%d\n", stime, ptime );
	
	Info.STime = stime;	// 無音部の時間(ms)セット
	Info.PTime = ptime;	// ぴー音の時間(ms)セット
}


////////////////////////////////////////////////////////////////
// 1Byte読込み
////////////////////////////////////////////////////////////////
BYTE cP6DATA::Read( int num )
{
	if( !Data || num > Info.DNum ) return 0;
	return Data[num];
}


////////////////////////////////////////////////////////////////
// ファイルに書込み(データ)
////////////////////////////////////////////////////////////////
int cP6DATA::Writefd( FILE *fp )
{
	Info.Offset = ftell( fp );
	
	return fwrite( Data, sizeof(BYTE), Info.DNum, fp );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cP6PART::cP6PART( void ) : Data(NULL), next(NULL), before(NULL) {}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cP6PART::~cP6PART( void )
{
	if( Data ) delete Data;
	if( next ) delete next;
	if( before ) before->next = NULL;
}


////////////////////////////////////////////////////////////////
// 先頭PARTへのポインタを返す
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::TopPart( void )
{
	cP6PART *top = this;
	while( top->before ) top = top->before;
	
	return top;
}


////////////////////////////////////////////////////////////////
// 末尾PARTへのポインタを返す
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::LastPart( void )
{
	cP6PART *last = this;
	while( last->next ) last = last->next;
	
	return last;
}


////////////////////////////////////////////////////////////////
// ID番号リナンバー
////////////////////////////////////////////////////////////////
BYTE cP6PART::Renumber( void )
{
	cP6PART *part = TopPart();
	
	part->Info.ID = 0;
	while( part->next ){
		part->next->Info.ID = part->Info.ID + 1;
		part = part->next;
	}
	
	return LastPart()->Info.ID;
}


////////////////////////////////////////////////////////////////
// 次のPARTへのポインタを返す
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Next( void )
{
	return next;
}


////////////////////////////////////////////////////////////////
// 前のPARTへのポインタを返す
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Before( void )
{
	return before;
}


////////////////////////////////////////////////////////////////
// 任意のPARTへのポインタを返す
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Part( int num )
{
	cP6PART *part = TopPart();
	
	for( int i=0; i< num; i++ ){
		if( part->next ) part = part->next;
		else             break;
	}
	
	return part;
}


////////////////////////////////////////////////////////////////
// 新規PART追加
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::New( void )
{
	LastPart()->next = new cP6PART;
	LastPart()->before = this;
	
	return LastPart();
}


////////////////////////////////////////////////////////////////
// コピー作成
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Clone( void )
{
	cP6PART *newp = new cP6PART;
	*newp = *this;
	newp->next = newp->before = NULL;
	
	if( this->Data ) newp->Data = this->Data->Clones();
	
	return newp;
}


////////////////////////////////////////////////////////////////
// 末尾にPART連結
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Link( cP6PART *src )
{
	cP6PART *last = LastPart();
	
	last->next  = src;
	src->before = last;
	
	Renumber();
	
	return src;
}


////////////////////////////////////////////////////////////////
// 新規DATAブロック追加
////////////////////////////////////////////////////////////////
cP6DATA *cP6PART::NewBlock( void )
{
	if( Data ) return Data->New();
	else       return Data = new cP6DATA;
}


////////////////////////////////////////////////////////////////
// 全DATAブロック数を取得
////////////////////////////////////////////////////////////////
int cP6PART::GetBlocks( void )
{
	int num = 0;
	cP6DATA *data = Data;
	
	do{
		num ++;
		data = data->Next();
	}while( data );
	
	return num;
}


////////////////////////////////////////////////////////////////
// PARTサイズ取得
////////////////////////////////////////////////////////////////
int cP6PART::GetSize( void )
{
	int size = 0;
	
	cP6DATA *data = Data;
	while( data ){
		size += data->GetInfo()->DNum;
		data  = data->Next();
	}
	
	return size;
}

////////////////////////////////////////////////////////////////
// 先頭DATAブロックへのポインタ取得
////////////////////////////////////////////////////////////////
cP6DATA *cP6PART::FirstData( void )
{
	return Data;
}


////////////////////////////////////////////////////////////////
// P6T PART情報取得
////////////////////////////////////////////////////////////////
P6TPRTINFO *cP6PART::GetInfo( void )
{
	return &Info;
}
////////////////////////////////////////////////////////////////
// データ名設定
////////////////////////////////////////////////////////////////
int cP6PART::SetName( const char *name )
{
	ZeroMemory( Info.Name, sizeof(Info.Name) );
	strncpy( Info.Name, name, sizeof(Info.Name)-1 );
	
	return strlen( Info.Name );
}


////////////////////////////////////////////////////////////////
// ボーレート設定
////////////////////////////////////////////////////////////////
void cP6PART::SetBaud( int baud )
{
	Info.Baud = baud;
}


////////////////////////////////////////////////////////////////
// ファイルから全PARTを読込み
////////////////////////////////////////////////////////////////
bool cP6PART::Readf( FILE *fp )
{
	PRINTD( P6T2_LOG, "[cP6PART][Readf]\n" );
	
	// ファイルポインタは有効?
	if( !fp ) return false;
	
	// 識別子 "TI"(0x4954) ?
	while( ( FGETWORD( fp ) ) == 0x4954 ){
		
		BYTE NextID = fgetc( fp );	// ID番号
		
		// 既にDATAブロックがあり,IDが異なるなら新規PARTを追加
		if( LastPart()->Data && ( LastPart()->Info.ID != NextID ) ){
			New();
			LastPart()->Info.ID = NextID;
		}
		
		// 既にDATAブロックがある?
		if( LastPart()->Data )
			// 継続PARTならデータ名とボーレートを読み飛ばす
			fseek( fp, sizeof(Info.Name)+sizeof(Info.Baud)-1, SEEK_CUR );
		else{
			// データ名
			fread( LastPart()->Info.Name, sizeof(BYTE), sizeof(Info.Name)-1, fp );
			// ボーレート
			LastPart()->Info.Baud = FGETWORD( fp );
		}
		// DATAブロック追加
		cP6DATA *NewBlk = LastPart()->NewBlock();
		
		WORD stime = FGETWORD( fp );	// 無音部の時間(ms)
		WORD ptime = FGETWORD( fp );	// ぴー音の時間(ms)
		DWORD doff = FGETDWORD( fp );	// ベタイメージ先頭からのオフセット
		DWORD dnum = FGETDWORD( fp );	// データサイズ
		
		// データを読込む
		DWORD fps = ftell( fp );
		fseek( fp, doff, SEEK_SET );
		NewBlk->SetData( fp, dnum );
		NewBlk->SetPeriod( stime, ptime );
		fseek( fp, fps, SEEK_SET );	// ファイルポインタを次のPART情報に戻す
	}
	
	Renumber();
	
	return true;
}


////////////////////////////////////////////////////////////////
// ファイルに書込み(データ)
////////////////////////////////////////////////////////////////
int cP6PART::Writefd( FILE *fp )
{
	cP6DATA *data = Data;
	int size = 0;
	
	do{
		size += data->Writefd( fp );
		data = data->Next();
	}while( data );
	
	return size;
}


////////////////////////////////////////////////////////////////
// ファイルに書込み(フッタ)
////////////////////////////////////////////////////////////////
bool cP6PART::Writeff( FILE *fp )
{
// [DATAブロック]
//  header (2byte) : "TI"
//  id     (1byte) : ID番号(DATAブロックを関連付ける)
//  name  (16byte) : データ名(15文字+'00H')
//  baud   (2byte) : ボーレート(600/1200)
//  stime  (2byte) : 無音部の時間(ms)
//  ptime  (2byte) : ぴー音の時間(ms)
//  offset (4byte) : ベタイメージ先頭からのオフセット
//  size   (4byte) : データサイズ
	
	cP6DATA *data = Data;
	
	do{
		fputc( 'T', fp );
		fputc( 'I', fp );
		fputc( Info.ID, fp );
		fwrite( Info.Name, sizeof(BYTE), 16, fp );
		FPUTWORD( Info.Baud, fp )
		
		P6TBLKINFO *info = data->GetInfo();
		FPUTWORD( info->STime, fp );
		FPUTWORD( info->PTime, fp );
		FPUTDWORD( info->Offset, fp );
		FPUTDWORD( info->DNum, fp );
		
		data = data->Next();
	}while( data );
	
	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// コンストラクタ
////////////////////////////////////////////////////////////////
cP6T::cP6T( void ) : Version(0), EHead(0), exh(NULL), Part(NULL),
						rpart(NULL), rdata(NULL), rpt(0), swait(0), pwait(0)
{
	INITARRAY( Name, 0 );
}


////////////////////////////////////////////////////////////////
// デストラクタ
////////////////////////////////////////////////////////////////
cP6T::~cP6T( void )
{
	if( Part ) delete Part;
	if( Ainfo.ask ) delete [] Ainfo.ask;
	if( exh ) delete [] exh;
}


////////////////////////////////////////////////////////////////
// 全DATAブロック数を取得
////////////////////////////////////////////////////////////////
int cP6T::GetBlocks( void )
{
	int num = 0;
	cP6PART *part = Part;
	
	do{
		num += part->GetBlocks();
		part = part->Next();
	}while( part );
	
	return num;
}


////////////////////////////////////////////////////////////////
// 全PART数を取得
////////////////////////////////////////////////////////////////
int cP6T::GetParts( void )
{
	int num = 0;
	cP6PART *part = Part;
	
	do{
		num ++;
		part = part->Next();
	}while( part );
	
	return num;
}


////////////////////////////////////////////////////////////////
// 任意PARTへのポインタ取得
////////////////////////////////////////////////////////////////
cP6PART *cP6T::GetPart( int num )
{
	return Part->Part( num );
}


////////////////////////////////////////////////////////////////
// ベタイメージサイズ取得
////////////////////////////////////////////////////////////////
int cP6T::GetSize( void )
{
	int size = 0;
	cP6PART *part = Part;
	
	while( part ){
		size += part->GetSize();
		part  = part->Next();
	}
	
	return size;
}


////////////////////////////////////////////////////////////////
// データ名設定
////////////////////////////////////////////////////////////////
int cP6T::SetName( const char *name )
{
	ZeroMemory( Name, 16 );
	strncpy( Name, name, 16 );
	
	return strlen( Name );
}


////////////////////////////////////////////////////////////////
// データ名取得
////////////////////////////////////////////////////////////////
const char *cP6T::GetName( void )
{
	return Name;
}


////////////////////////////////////////////////////////////////
// オートスタート情報取得
////////////////////////////////////////////////////////////////
P6TAUTOINFO *cP6T::GetAutoStartInfo( void )
{
	return &Ainfo;
}


////////////////////////////////////////////////////////////////
// 1文字読込み
////////////////////////////////////////////////////////////////
BYTE cP6T::ReadOne( void )
{
	BYTE data = rdata->Read( rpt++ );
	
	PRINTD( P6T2_LOG, "[cP6T][ReadOne] -> %02X\n", data );
	
	// ブロック情報取得
	P6TBLKINFO *binfo = rdata->GetInfo();
	if( rpt >= binfo->DNum ){	// データ最後?
		rpt = 0;
		if( !( rdata = rdata->Next() ) ){	// DATAブロック最後?
			if( !( rpart = rpart->Next() ) ){	// PART最後?
				rpart = Part;
			}
			rdata = rpart->FirstData();
		}
		// 次のブロック情報取得
		binfo = rdata->GetInfo();
		swait = ( binfo->STime * DEFAULT_CMT_HZ  )/1000;
		pwait = ( binfo->PTime * DEFAULT_CMT_HZ  )/1000;
	}
	
	return data;
}


////////////////////////////////////////////////////////////////
// 無音部待ち?
////////////////////////////////////////////////////////////////
bool cP6T::IsSWaiting( void )
{
	if( !swait ) return false;
	swait--;
	return true;
}


////////////////////////////////////////////////////////////////
// ぴー音待ち?
////////////////////////////////////////////////////////////////
bool cP6T::IsPWaiting( void )
{
	if( !pwait ) return false;
	pwait--;
	return true;
}


////////////////////////////////////////////////////////////////
// リセット(読込み関係ワーク初期化)
////////////////////////////////////////////////////////////////
void cP6T::Reset( void )
{
	PRINTD( P6T2_LOG, "[cP6T][Reset]\n" );
	
	// 読込み関係ワーク初期化
	rpart = Part;				// 現在の読込みPART
	rdata = Part->FirstData();	// 現在の読込みDATAブロック
	rpt   = 0;					// 現在の読込みポインタ
	
	P6TBLKINFO *binfo = rdata->GetInfo();
	swait = ( binfo->STime * DEFAULT_CMT_HZ  )/1000;	// 無音部の待ち回数
	pwait = ( binfo->PTime * DEFAULT_CMT_HZ  )/1000;	// ぴー音の待ち回数
}


////////////////////////////////////////////////////////////////
// ファイルから読込み
////////////////////////////////////////////////////////////////
bool cP6T::Readf( const char *filename )
{
	PRINTD( P6T2_LOG, "[cP6T][Readf] [%s]\n", filename );
	
	if( !ReadP6T( filename ) ){		// P6Tを読込み
		// 失敗したらベタとみなしP6Tに変換して読込み
		if( !ConvP6T( filename ) ) return false;
	}
	
	Reset();	// リセット(読込み関係ワーク初期化)
	
	return true;
}


////////////////////////////////////////////////////////////////
// ファイルに書込み
////////////////////////////////////////////////////////////////
bool cP6T::Writef( const char *filename )
{
	PRINTD( P6T2_LOG, "[cP6T][Writef] [%s]\n", filename );
	
	FILE *fp;
	
	fp = FOPENEN( filename, "wb" );
	if( !fp ) return false;
	
	// データ書込み&ベタイメージサイズ取得
	DWORD BetaSize = 0;
	cP6PART *part = Part;
	do{
		BetaSize += part->Writefd( fp );
		part = part->Next();
	}while( part );
	

// [フッタ]
//  header (2byte) : "P6"
//  ver    (1byte) : バージョン
//  dbnum  (1byte) : 含まれるDATAブロック数(255個まで)
//  start  (1byte) : オートスタートフラグ(0:無効 1:有効)
//  basic  (1byte) : BASICモード(PC-6001の場合は無意味)
//  page   (1byte) : ページ数
//  askey  (2byte) : オートスタートコマンド文字数
//  ...コマンドがある場合はこの後にaskey分続く
//  exhead (2byte) : 拡張情報サイズ(64KBまで)
//  ...拡張情報がある場合はこの後にexhead分続く

	// P6T フッタ書込み
	fputc( 'P', fp );
	fputc( '6', fp );
	fputc( Version, fp );
	fputc( GetBlocks(), fp );
	fputc( Ainfo.Start ? 1 : 0, fp );
	fputc( Ainfo.BASIC, fp );
	fputc( Ainfo.Page, fp );
	FPUTWORD( Ainfo.ASKey, fp );
	if( Ainfo.ASKey ) fwrite( Ainfo.ask, sizeof(char), Ainfo.ASKey, fp );
	FPUTWORD( EHead, fp );
	if( EHead ) fwrite( exh, sizeof(char), EHead, fp );
	
	// 全DATAブロックフッタ書込み
	part = Part;
	do{
		part->Writeff( fp );
		part = part->Next();
	}while( part );
	
	// ベタイメージサイズ書込み
	FPUTDWORD( BetaSize, fp );
	
	return true;
}


////////////////////////////////////////////////////////////////
// P6Tを読込み
////////////////////////////////////////////////////////////////
bool cP6T::ReadP6T( const char *filename )
{
	PRINTD( P6T2_LOG, "[cP6T][ReadP6T] [%s]\n", filename );
	
	FILE *fp;
	
	fp = FOPENEN( filename, "rb" );
	if( !fp ) return false;
	
	// ベタイメージサイズ取得
	fseek( fp, -4, SEEK_END );
	DWORD BetaSize = FGETDWORD( fp );
	// ベタイメージサイズがファイルサイズを超えていたらエラー
	if( BetaSize > (DWORD)ftell( fp ) ){ fclose( fp ); return false; }
	
	// フッタの先頭に移動
	fseek( fp, BetaSize, SEEK_SET );
	
	// 識別子 "P6"(0x3650) でなければエラー
	WORD Header = FGETWORD( fp );
	if( Header != 0x3650 ){ fclose( fp ); return false; }
	
	Version = fgetc( fp );						// バージョン
	fgetc( fp );								// 含まれるDATAブロック数
	Ainfo.Start   = fgetc( fp ) ? true : false;	// オートスタートフラグ
	Ainfo.BASIC   = fgetc( fp );				// BASICモード
	Ainfo.Page    = fgetc( fp );				// ページ数
	
	// オートスタートコマンド
	Ainfo.ASKey = FGETWORD( fp );
	if( Ainfo.ASKey ){
		Ainfo.ask = new char[Ainfo.ASKey];
		fread( Ainfo.ask, sizeof(char), Ainfo.ASKey, fp );
	}
	
	// 拡張情報
	EHead = FGETWORD( fp );
	if( EHead ){
		exh = new char[EHead];
		fread( exh, sizeof(char), EHead, fp );
	}
	
	// PARTを読込み
	Part = new cP6PART;
	if( !Part->Readf( fp ) ){ fclose( fp ); return false; }
	
	fclose( fp );
	
	return true;
}


////////////////////////////////////////////////////////////////
// ベタをP6Tに変換して読込み
////////////////////////////////////////////////////////////////
bool cP6T::ConvP6T( const char *filename )
{
	PRINTD( P6T2_LOG, "[cP6T][ConvP6T] [%s]\n", filename );
	
	FILE *fp;
	
	fp = FOPENEN( filename, "rb" );
	if( !fp ) return false;
	
	// ベタイメージサイズ取得
	fseek( fp, 0, SEEK_END );
	DWORD BetaSize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	
	// P6T情報設定
	SetName( OSD_GetFileNamePart( filename ) );	// データ名(16文字+'00H')はファイル名
	Version = 2;								// バージョン(とりあえず2)
	Ainfo.Start = false;						// オートスタートフラグ(無効)
	Ainfo.BASIC = 1;							// BASICモード(PC-6001の場合は無意味)(とりあえず1だが無意味)
	Ainfo.Page  = 1;							// ページ数(とりあえず1だが無意味)
	Ainfo.ASKey = 0;							// オートスタートコマンドサイズ(0:無効)
	Ainfo.ask   = NULL;							// オートスタートコマンド格納領域へのポインタ
	EHead       = 0;							// 拡張情報サイズ(0:なし)
	exh         = NULL;							// 拡張情報格納領域へのポインタ
	
	// PARTを作成
	Part = new cP6PART;
	Part->SetName( "BetaImage" );	// データ名(16文字+'00H')はとりあえず"BetaImage"
	Part->SetBaud( 1200 );			// ボーレート(1200)
	
	// DATAブロックを作成
	Part->NewBlock();
	Part->FirstData()->SetData( fp, BetaSize );	// データセット
	Part->FirstData()->SetPeriod( 3400, 6800 );	// 無音部(3400ms),ぴー音(6800ms)時間セット
	
	fclose( fp );
	
	return true;
}


////////////////////////////////////////////////////////////////
// TAPEカウンタ取得
////////////////////////////////////////////////////////////////
int cP6T::GetCount( void )
{
	if( !rdata ) return 0;
	
	return rdata->GetInfo()->Offset + rpt;
}


////////////////////////////////////////////////////////////////
// TAPEカウンタ設定
////////////////////////////////////////////////////////////////
void cP6T::SetCount( int cnt )
{
	if( GetSize() <= cnt ) return;
	
	int off = 0;
	
	// まずPARTをチェック
	rpart = Part;
	while( off+rpart->GetSize() <= cnt ){
		off   += rpart->GetSize();
		rpart  = rpart->Next();
	}
	// 次にDATAをチェック
	rdata = rpart->FirstData();
	while( off+rdata->GetInfo()->DNum <= cnt ){
		off   += rdata->GetInfo()->DNum;
		rdata  = rdata->Next();
	}
	
	rpt = cnt - rdata->GetInfo()->Offset;
}


////////////////////////////////////////////////////////////////
// どこでもSAVE
////////////////////////////////////////////////////////////////
bool cP6T::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "P6T", NULL, "Counter",	"%d",	GetCount() );
	Ini->PutEntry( "P6T", NULL, "swait",	"%d",	swait );
	Ini->PutEntry( "P6T", NULL, "pwait",	"%d",	pwait );
	
	return true;
}


////////////////////////////////////////////////////////////////
// どこでもLOAD
////////////////////////////////////////////////////////////////
bool cP6T::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt( "P6T",	"Counter",	&st,	0 );
	SetCount( st );
	Ini->GetInt( "P6T",	"swait",	&swait,	swait );
	Ini->GetInt( "P6T",	"pwait",	&pwait,	pwait );
	
	return true;
}
