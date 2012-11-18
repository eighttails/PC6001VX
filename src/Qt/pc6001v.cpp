#include <stdlib.h>
#include <errno.h>
#include <QApplication>
#include <QFrame>
#include <QTextCodec>
#include <QDir>

#ifdef WIN32
//Windowsではプラグインを静的リンクする。
#include <QtPlugin>
Q_IMPORT_PLUGIN(qjpcodecs)
Q_IMPORT_PLUGIN(qico)
#endif

#include "../pc6001v.h"
#include "../typedef.h"
#include "../config.h"
#include "../console.h"
#include "../error.h"
#include "../osd.h"
#include "../p6el.h"
#include "../p6vm.h"

//SDLMainとのコンフリクト解消
#undef main

#ifdef SDL_VIDEO_DRIVER_X11
#include <X11/Xlib.h>
#endif


///////////////////////////////////////////////////////////
// フォントファイルチェック(無ければ作成する)
///////////////////////////////////////////////////////////
bool CheckFont( void )
{
    char FontFile[PATH_MAX];
    bool ret = true;

    sprintf( FontFile, "%s%s/%s", OSD_GetConfigPath(), FONT_DIR, FONTH_FILE );
    ret |= ( !OSD_FileExist( FontFile ) && !OSD_CreateFont( FontFile, NULL, FSIZE ) );

    sprintf( FontFile, "%s%s/%s", OSD_GetConfigPath(), FONT_DIR, FONTZ_FILE );
    ret |= ( !OSD_FileExist( FontFile ) && !OSD_CreateFont( NULL, FontFile, FSIZE ) );

    return ret;
}

///////////////////////////////////////////////////////////
// ROMファイル存在チェック&機種変更
///////////////////////////////////////////////////////////
bool SerchRom( CFG6 *cfg )
{
	char RomSerch[PATH_MAX];
	
	int IniModel = cfg->GetModel();
	sprintf( RomSerch, "%s*.%2d", cfg->GetRomPath(), IniModel );
	if( OSD_FileExist( RomSerch ) ){
		Error::SetError( Error::NoError );
		return true;
	}
	
	int models[] = { 60, 62, 66 };
	for( int i=0; i < COUNTOF(models); i++ ){
		sprintf( RomSerch, "%s*.%2d", cfg->GetRomPath(), models[i] );
		if( OSD_FileExist( RomSerch ) ){
            cfg->SetModel( models[i] );
            Error::SetError( Error::RomChange );
			return true;
		}
	}
	Error::SetError( Error::NoRom );
	return false;
}

///////////////////////////////////////////////////////////
// メイン
///////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
#ifdef SDL_VIDEO_DRIVER_X11
    //X11環境でSDLスレッドを使用するために必要。
    XInitThreads();
#endif

    QApplication app(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

	EL6 *P6Core             = NULL;			// オブジェクトポインタ
	EL6::ReturnCode Restart = EL6::Quit;	// 再起動フラグ
	CFG6 Cfg;								// 環境設定オブジェクト

	// 環境変数設定(テスト用)
//	putenv( "SDL_VIDEODRIVER=windib" );
//	putenv( "SDL_VIDEODRIVER=directx" );
//	putenv( "SDL_AUDIODRIVER=waveout" );
//	putenv( "SDL_AUDIODRIVER=dsound" );
	

    setlocale(LC_ALL,"Japanese");

#ifdef WIN32
    // Windowsではアプリに同梱のqjpcodecs4.dllプラグインを読み込むため、そのパスを指定
    QApplication::addLibraryPath(qApp->applicationDirPath() + QDir::separator() + "plugins");
#endif
	
	// 二重起動禁止
#ifndef DEBUG
	if( OSD_IsWorking() ) return false;
#endif

    // 設定ファイルパスを作成
    if(!OSD_CreateConfigPath()) return false;

	// OSD関連初期化
	if( !OSD_Init() ){
		Error::SetError( Error::InitFailed );
		OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		OSD_Quit();	// 終了処理
		return false;
	}
	
	// フォントファイルチェック
	if( !CheckFont() ){
		Error::SetError( Error::FontCreateFailed );
		OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
		Error::SetError( Error::NoError );
	}
	
	// コンソール用フォント読込み
	char FontZ[PATH_MAX], FontH[PATH_MAX];
    sprintf( FontZ, "%s%s/%s", OSD_GetConfigPath(), FONT_DIR, FONTZ_FILE );
    sprintf( FontH, "%s%s/%s", OSD_GetConfigPath(), FONT_DIR, FONTH_FILE );
	if( !JFont::OpenFont( FontZ, FontH ) ){
		Error::SetError( Error::FontLoadFailed );
		OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		Error::SetError( Error::NoError );
	}
	
	
	// INIファイル読込み
	if( !Cfg.Init() ){
		switch( Error::GetError() ){
		case Error::IniDefault:
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
			Error::SetError( Error::NoError );
			break;
			
		default:
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			OSD_Quit();			// 終了処理
			return false;
		}
	}
	
	
	// P6オブジェクトを作成して実行
	do{
		// ROMファイル存在チェック&機種変更
		if( SerchRom( &Cfg ) ){
			if( Error::GetError() != Error::NoError ){
				OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
				Error::SetError( Error::NoError );
			}
		}else{
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			break;	// do 抜ける
		}
		
		// 機種別P6オブジェクト確保
		P6Core = new EL6;
		if( !P6Core ){
			break;	// do 抜ける
		}
		
		// VM初期化
		if( !P6Core->Init( &Cfg ) ){
            if(Error::GetError() == Error::RomCrcNG){
                // CRCが合わない場合
                int ret = OSD_Message( "ROMイメージのCRCが不正です。\n"
                                       "CRCが一致しないROMを使用すると、予期せぬ不具合を引き起こす可能性があります。\n"
                                       "それでも起動しますか?",
                                       MSERR_ERROR, OSDM_YESNO | OSDM_ICONWARNING );
                if(ret == OSDR_YES) {
                    Cfg.SetCheckCRC(false);
                    Cfg.Write();
                    Restart = EL6::Restart;
                }
            } else {
                OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
                break;	// do 抜ける
            }
		}
		
		switch( Restart ){
		case EL6::Dokoload:	// どこでもLOAD?
			P6Core->DokoDemoLoad( Cfg.GetDokoFile() );
			break;
			
		case EL6::Replay:	// リプレイ再生?
		{
			char temp[PATH_MAX];
			strncpy( temp, Cfg.GetDokoFile(), PATH_MAX );
			P6Core->DokoDemoLoad( temp );
			P6Core->REPLAY::StartReplay( temp );
		}
			break;
			
		default:
			break;
		}
		
		// VM実行
		P6Core->Start();
		Restart = P6Core->EventLoop();
		P6Core->Stop();
		
		delete P6Core;	// P6オブジェクトを開放
		
		
		// 再起動ならばINIファイル再読込み
		if( Restart == EL6::Restart ){
			if( !Cfg.Init() ){
				Error::SetError( Error::IniDefault );
				OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
				Error::SetError( Error::NoError );
			}
		}
		
	}while( Restart != EL6::Quit );
	
	
	// 終了処理
	OSD_Quit();
	
	return true;
}
