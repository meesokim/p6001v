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
// �t�H���g�t�@�C���`�F�b�N(������΍쐬����)
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
// ROM�t�@�C�����݃`�F�b�N&�@��ύX
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
// ���C��
///////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
	EL6 *P6Core             = NULL;			// �I�u�W�F�N�g�|�C���^
	EL6::ReturnCode Restart = EL6::Quit;	// �ċN���t���O
	CFG6 Cfg;								// ���ݒ�I�u�W�F�N�g
	
	// 32bit�J���[�e�[�u���N���A
	for( int i=0; i<256; i++ )
		VSurface::SetColor( i, 0 );
	
	
	// ���ϐ��ݒ�(�e�X�g�p)
//	putenv( "SDL_VIDEODRIVER=windib" );
//	putenv( "SDL_VIDEODRIVER=directx" );
//	putenv( "SDL_AUDIODRIVER=waveout" );
//	putenv( "SDL_AUDIODRIVER=dsound" );
	
	setlocale( LC_CTYPE, "" );
	
	// ��d�N���֎~
	if( OSD_IsWorking() ) return false;
	
	// OSD�֘A������
	if( !OSD_Init() ){
		Error::SetError( Error::InitFailed );
		OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		OSD_Quit();	// �I������
		return false;
	}
	
	// �t�H���g�t�@�C���`�F�b�N
	if( !CheckFont() ){
		Error::SetError( Error::FontCreateFailed );
		OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
		Error::Reset();
	}
	
	// �R���\�[���p�t�H���g�Ǎ���
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
	
	// INI�t�@�C���Ǎ���
	if( !Cfg.Init() ){
		switch( Error::GetError() ){
		case Error::IniDefault:
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
			Error::Reset();
			break;
			
		default:
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			OSD_Quit();			// �I������
			return false;
		}
	}
	
	
	// P6�I�u�W�F�N�g���쐬���Ď��s
	do{
		// ROM�t�@�C�����݃`�F�b�N&�@��ύX
		if( SerchRom( &Cfg ) ){
			if( Error::GetError() != Error::NoError ){
				OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
				Error::Reset();
			}
		}else{
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			break;	// do ������
		}
		
		// �@���P6�I�u�W�F�N�g�m��
		P6Core = new EL6;
		if( !P6Core ){
			break;	// do ������
		}
		
		// VM������
		if( !P6Core->Init( &Cfg ) ){
			// ���s�����ꍇ
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			delete P6Core;	// P6�I�u�W�F�N�g���J������
			break;			// do ������
		}
		
		switch( Restart ){
		case EL6::Dokoload:	// �ǂ��ł�LOAD?
			if( !P6Core->DokoDemoLoad( Cfg.GetDokoFile() ) ){
				// ���s�����ꍇ
				OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			}
			Cfg.SetDokoFile( "" );
			break;
			
		case EL6::Replay:	// ���v���C�Đ�?
			if( !P6Core->DokoDemoLoad( Cfg.GetDokoFile() ) ){
				// ���s�����ꍇ
				OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			}
			P6Core->REPLAY::StartReplay( Cfg.GetDokoFile() );
			Cfg.SetDokoFile( "" );
			break;
			
		default:
			break;
		}
		
		// VM���s
		P6Core->Start();
		Restart = P6Core->EventLoop();
		P6Core->Stop();
		
		delete P6Core;	// P6�I�u�W�F�N�g���J��
		
		
		// �ċN���Ȃ��INI�t�@�C���ēǍ���
		if( Restart == EL6::Restart ){
			if( !Cfg.Init() ){
				Error::SetError( Error::IniDefault );
				OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONWARNING );
				Error::Reset();
			}
		}
		
	}while( Restart != EL6::Quit );
	
	
	// �I������
	OSD_Quit();
	
	return true;
}
