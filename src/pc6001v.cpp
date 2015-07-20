#include <stdlib.h>
#include <errno.h>
#include <locale.h>

#include <SDL_main.h>

#include "pc6001v.h"
#include "typedef.h"

#include "config.h"
#include "console.h"
#include "error.h"
#include "osd.h"
#include "vsurface.h"

#include "p6el.h"
#include "p6vm.h"



///////////////////////////////////////////////////////////
// フォントファイルチェック(無ければ作成する)
///////////////////////////////////////////////////////////
bool CheckFont( void )
{
	char FontFile[PATH_MAX];
	bool ret = true;
	
	OSD_AddPath( FontFile, FONT_DIR, FONTH_FILE );
	OSD_AddPath( FontFile, OSD_GetModulePath(), (const char *)FontFile );
	ret |= ( !OSD_FileExist( FontFile ) && !OSD_CreateFont( FontFile, NULL, FSIZE ) );
	
	OSD_AddPath( FontFile, FONT_DIR, FONTZ_FILE );
	OSD_AddPath( FontFile, OSD_GetModulePath(), (const char *)FontFile );
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
	sprintf( RomSerch, "*.%2d", IniModel );
	OSD_AddPath( RomSerch, cfg->GetRomPath(), RomSerch );
	if( OSD_FileExist( RomSerch ) ){
		Error::Reset();
		return true;
	}
	
	int models[] = { 60, 61, 62, 66, 64, 68 };
	for( int i=0; i < COUNTOF(models); i++ ){
		sprintf( RomSerch, "*.%2d", models[i] );
		OSD_AddPath( RomSerch, cfg->GetRomPath(), RomSerch );
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
	EL6 *P6Core             = NULL;			// オブジェクトポインタ
	EL6::ReturnCode Restart = EL6::Quit;	// 再起動フラグ
	CFG6 Cfg;								// 環境設定オブジェクト
	
	// 32bitカラーテーブルクリア
	for( int i=0; i<256; i++ )
		VSurface::SetColor( i, 0 );
	
	
	// 環境変数設定(テスト用)
//	putenv( "SDL_VIDEODRIVER=windib" );
//	putenv( "SDL_VIDEODRIVER=directx" );
//	putenv( "SDL_AUDIODRIVER=waveout" );
//	putenv( "SDL_AUDIODRIVER=dsound" );
	
	setlocale( LC_CTYPE, "" );
	
	// 二重起動禁止
	if( OSD_IsWorking() ) return false;
	
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
		Error::Reset();
	}
	
	// コンソール用フォント読込み
	char FontZ[PATH_MAX], FontH[PATH_MAX];
	OSD_AddPath( FontZ, FONT_DIR, FONTZ_FILE );
	OSD_AddPath( FontZ, OSD_GetModulePath(), (const char *)FontZ );
	OSD_AddPath( FontH, FONT_DIR, FONTH_FILE );
	OSD_AddPath( FontH, OSD_GetModulePath(), (const char *)FontH );
	if( !JFont::OpenFont( FontZ, FontH ) ){
		Error::SetError( Error::FontLoadFailed );
		OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		Error::Reset();
	}
	
	// INIファイル読込み
	if( !Cfg.Init() ){
		switch( Error::GetError() ){
		case Error::IniDefault:
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
			Error::Reset();
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
				Error::Reset();
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
			// 失敗した場合
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			delete P6Core;	// P6オブジェクトを開放して
			break;			// do 抜ける
		}
		
		switch( Restart ){
		case EL6::Dokoload:	// どこでもLOAD?
			if( !P6Core->DokoDemoLoad( Cfg.GetDokoFile() ) ){
				// 失敗した場合
				OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			}
			Cfg.SetDokoFile( "" );
			break;
			
		case EL6::Replay:	// リプレイ再生?
			if( !P6Core->DokoDemoLoad( Cfg.GetDokoFile() ) ){
				// 失敗した場合
				OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			}
			P6Core->REPLAY::StartReplay( Cfg.GetDokoFile() );
			Cfg.SetDokoFile( "" );
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
				Error::Reset();
			}
		}
		
	}while( Restart != EL6::Quit );
	
	
	// 終了処理
	OSD_Quit();
	
	return true;
}
