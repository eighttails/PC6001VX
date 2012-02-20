#include <stdlib.h>
#include <errno.h>
#include <QApplication>
#include <QFrame>
#include <QTextCodec>

#include "../typedef.h"
#include "../config.h"
#include "../console.h"
#include "../error.h"
#include "../osd.h"
#include "../pc60.h"

//SDLMainとのコンフリクト解消
#undef main

#ifdef SDL_VIDEO_DRIVER_X11
#include <X11/Xlib.h>
#endif
///////////////////////////////////////////////////////////
// メイン
///////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
#ifdef SDL_VIDEO_DRIVER_X11
    //X11環境でSDLスレッドを使用するために必要。
    XInitThreads();
#endif

    VM6 *P6Core             = NULL;			// オブジェクトポインタ
    VM6::ReturnCode Restart = VM6::Quit;	// 再起動フラグ
    cConfig *Cfg            = NULL;			// 環境設定オブジェクト

    QApplication app(argc, argv);
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    // 環境変数設定(テスト用)
    //	putenv( "SDL_VIDEODRIVER=windib" );
    //	putenv( "SDL_VIDEODRIVER=directx" );
    //	putenv( "SDL_AUDIODRIVER=dsound" );


    // 二重起動禁止
    if( OSD_IsWorking() ) return FALSE;

    // 設定ファイルフォルダを作成
    if( !OSD_CreateConfigPath() ) return FALSE;

    // SDL初期化
    if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) < 0 ){
        Error::SetError( Error::LibInitFailed );
        OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
        OSD_Finish();	// 終了処理
        return FALSE;
    }

    // INIファイル読込み
    Cfg = new cConfig();
    if( !Cfg ){
        SDL_Quit();		// SDLシャットダウン
        OSD_Finish();	// 終了処理
    }
    if( !Cfg->Init() ){
        switch( Error::GetError() ){
        case Error::IniDefault:
            OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
            Error::SetError( Error::NoError );
            break;

        default:
            OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
            SDL_Quit();				// SDLシャットダウン
            OSD_Finish();			// 終了処理
            if( Cfg ) delete Cfg;	// 環境設定オブジェクト開放
            return FALSE;
        }
    }

    // フォントファイルが無ければ作成する
    if( !OSD_FileExist( Cfg->GetFontFileH() ) ) OSD_CreateFont( Cfg->GetFontFileH(), NULL, FSIZE );
    if( !OSD_FileExist( Cfg->GetFontFileZ() ) ) OSD_CreateFont( NULL, Cfg->GetFontFileZ(), FSIZE );


    // P6オブジェクトを作成して実行
    do{
        Restart = VM6::Quit;

        // ROMファイル存在チェック&機種変更
        char RomSerch[PATH_MAX];
        int IniModel = Cfg->GetModel();
        sprintf( RomSerch, "%s*.%2d", Cfg->GetRomPath(), IniModel );
        if( !OSD_FileExist( RomSerch ) ){
            int models[] = { 60, 62, 66 };
            for( int i=0; i < COUNTOF(models); i++ ){
                sprintf( RomSerch, "%s*.%2d", Cfg->GetRomPath(), models[i] );
                if( OSD_FileExist( RomSerch ) ){
                    Cfg->SetModel( models[i] );
                    break;
                }
            }
            // ROMある?
            if( IniModel != Cfg->GetModel() ){
                Error::SetError( Error::RomChange );
                OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
                Error::SetError( Error::NoError );
            }
        }

        // 機種別P6オブジェクト確保
        const char* iconRes = NULL;
        switch( Cfg->GetModel() ){
        case 62: P6Core = new VM62; iconRes = ":/res/PC-6001mk2.ico"; break;
        case 66: P6Core = new VM66; iconRes = ":/res/PC-6601.ico"; break;
        default: P6Core = new VM60; iconRes = ":/res/PC-6001.ico";
        }

        // アイコン設定
        QImage icon(iconRes);
        SDL_Surface *p6icon = SDL_CreateRGBSurfaceFrom( icon.bits(), icon.width(), icon.height(), icon.depth(), icon.bytesPerLine(), 0, 0, 0, 0 );
        SDL_WM_SetIcon( p6icon, icon.alphaChannel().bits() );
        SDL_FreeSurface( p6icon );

        // VMを初期化して実行
        if( P6Core->Init( Cfg ) ){
            switch( Restart ){
            case VM6::Doko:		// どこでもLOAD?
                P6Core->DokoDemoLoad( Cfg->GetDokoFile() );
                break;

            case VM6::Replay:	// リプレイ再生?
            {
                char temp[PATH_MAX];
                strncpy( temp, Cfg->GetDokoFile(), PATH_MAX );
                P6Core->DokoDemoLoad( temp );
                P6Core->REPLAY::StartReplay( temp );
            }
            break;

            default:
                break;
            }

            P6Core->Start();
            Restart = P6Core->EventLoop();
            P6Core->Stop();
        }else{
            // 失敗した場合
            OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
        }

        // P6オブジェクトを開放
        if( P6Core ) delete P6Core;


        // 再起動ならばINIファイル再読込み
        if( Restart == VM6::Restart ){
            if( !Cfg->Init() ){
                Error::SetError( Error::IniDefault );
                OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
                Error::SetError( Error::NoError );
            }
        }

    }while( Restart != VM6::Quit );



    // 環境設定オブジェクト開放
    if( Cfg ) delete Cfg;

    // SDLシャットダウン
    SDL_Quit();

    // 終了処理
    OSD_Finish();

    return TRUE;
}
