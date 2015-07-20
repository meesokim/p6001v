// OS�ˑ��̔ėp���[�`��(���UI�p)

#include <windows.h>
#include <shlobj.h>
#include <new>

#include "id_config.h"
#include "id_icon.h"
#include "../id_menu.h"
#include "../pc6001v.h"

#include "../breakpoint.h"
#include "../config.h"
#include "../disk.h"
#include "../error.h"
#include "../graph.h"
#include "../joystick.h"
#include "../memory.h"
#include "../osd.h"
#include "../schedule.h"
#include "../tape.h"
#include "../vdg.h"

#include "../p6el.h"


///////////////////////////////////////////////////////////
// ���[�J���֐���`
///////////////////////////////////////////////////////////
static bool OsdReadINI(  HWND, int );			// �ݒ��Ǎ���
static bool OsdWriteINI( HWND, int );			// �ݒ��ۑ�����

static bool CALLBACK OsdCnfgProc1(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProc2(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProc3(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProc4(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProc5(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProcCol( HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProcEtc( HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProcIn(  HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK VerInfoProc(    HWND, UINT, WPARAM, LPARAM );

static void OsdOwnerDrawBtn( HDC, RECT, COLORREF );	// �I�[�i�[�h���[�{�^�� �`��

///////////////////////////////////////////////////////////
// �d���Ȃ��ɃX�^�e�B�b�N�ϐ�
///////////////////////////////////////////////////////////
static CFG6 *ecfg = NULL;					// ���ݒ�I�u�W�F�N�g�|�C���^(�ҏW�p)
static int model = 60;						// �@��



///////////////////////////////////////////////////////////
// �|�b�v�A�b�v���j���[�\��
///////////////////////////////////////////////////////////
void EL6::ShowPopupMenu( int x, int y )
{
	// MF_BYPOSITION�p
	enum {	MSYSTEM = 0,	// �V�X�e��
			MSEP1,			// ----------
			MTAPE,			// TAPE
			MDISK,			// DISK
			MEXTROM,		// �g��ROM
			MCONT,			// �R���g���[��
			MCONFIG,		// �ݒ�
			MDEBUG,			// �f�o�b�O
			MSEP2,			// ----------
			MHELP			// �w���v
		};
	
	POINT pt;
	
	pt.x = x;
	pt.y = y;
	HWND hwnd = (HWND)OSD_GetWindowHandle( graph->GetWindowHandle() );
	
	if( GetMenu( hwnd ) ) return;
	
	HMENU hm  = LoadMenu( (HINSTANCE)GetWindowLongPtr( hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE( ID_MENU ) );
	HMENU hsm = GetSubMenu( hm, MSYSTEM );
	
	// ���j���[�̑O����
	MENUITEMINFO minfo;
	minfo.cbSize = sizeof(MENUITEMINFO);
	
	// �r�f�I�L���v�`��
	minfo.fMask      = MIIM_TYPE;
	minfo.dwTypeData = NULL;
	GetMenuItemInfo( hsm, ID_AVISAVE, MF_BYCOMMAND, &minfo );
	minfo.dwTypeData = (char *)(AVI6::IsAVI() ? MSMEN_AVI1 : MSMEN_AVI0);
	SetMenuItemInfo( hsm, ID_AVISAVE, MF_BYCOMMAND, &minfo );
	// ���j�^���[�h or �u���[�N�|�C���^���ݒ肳��Ă�����r�f�I�L���v�`������
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( cfg->GetMonDisp() || vm->BpExistBreakPoint() )
		EnableMenuItem( hsm, ID_AVISAVE, MF_BYCOMMAND | MF_GRAYED );
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// ���v���C�L�^
	minfo.fMask      = MIIM_TYPE;
	minfo.dwTypeData = NULL;
	GetMenuItemInfo( hsm, ID_REPLAYSAVE, MF_BYCOMMAND, &minfo );
	minfo.dwTypeData = (char *)(( REPLAY::GetStatus() == REP_RECORD ) ? MSMEN_REP1 : MSMEN_REP0);
	SetMenuItemInfo( hsm, ID_REPLAYSAVE, MF_BYCOMMAND, &minfo );
	// ���j�^���[�h or �u���[�N�|�C���^���ݒ肳��Ă���
	// �܂��̓��v���C�Đ����������烊�v���C�L�^����
	if( 
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		cfg->GetMonDisp() || vm->BpExistBreakPoint() ||
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		( REPLAY::GetStatus() == REP_REPLAY ) ){
		EnableMenuItem( hsm, ID_REPLAYSAVE,   MF_BYCOMMAND | MF_GRAYED );
		EnableMenuItem( hsm, ID_REPLAYRESUME, MF_BYCOMMAND | MF_GRAYED );
	}
	// �r���ۑ��A��蒼���̓��v���C�L�^���̂�
	if(
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		cfg->GetMonDisp() || vm->BpExistBreakPoint() ||
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		( REPLAY::GetStatus() != REP_RECORD ) ){
		EnableMenuItem( hsm, ID_REPLAYDOKOSAVE, MF_BYCOMMAND | MF_GRAYED );
		EnableMenuItem( hsm, ID_REPLAYDOKOLOAD, MF_BYCOMMAND | MF_GRAYED );
	}
	
	// ���v���C�Đ�
	minfo.fMask      = MIIM_TYPE;
	minfo.dwTypeData = NULL;
	GetMenuItemInfo( hsm, ID_REPLAYLOAD, MF_BYCOMMAND, &minfo );
	minfo.dwTypeData = (char *)(( REPLAY::GetStatus() == REP_REPLAY ) ? MSMEN_REP3: MSMEN_REP2);
	SetMenuItemInfo( hsm, ID_REPLAYLOAD, MF_BYCOMMAND, &minfo );
	// ���j�^���[�h or �u���[�N�|�C���^���ݒ肳��Ă���
	// �܂��̓��v���C�L�^���������烊�v���C�Đ�����
	if(
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		cfg->GetMonDisp() || vm->BpExistBreakPoint() ||
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		( REPLAY::GetStatus() == REP_RECORD ) ){
		EnableMenuItem( hsm, ID_REPLAYLOAD,   MF_BYCOMMAND | MF_GRAYED );
		EnableMenuItem( hsm, ID_REPLAYRESUME, MF_BYCOMMAND | MF_GRAYED );
	}
	
	// TAPE
	EnableMenuItem( hsm, ID_TAPEEJECT, MF_BYCOMMAND | *vm->cmtl->GetFile() ? MF_ENABLED : MF_GRAYED );
	
	// DISK
	switch( vm->disk->GetDrives() ){
	case 0:
		DeleteMenu( hsm, MDISK, MF_BYPOSITION );
		break;
	case 1:
		DeleteMenu( GetSubMenu( hsm, MDISK ), 1, MF_BYPOSITION );
		EnableMenuItem( hsm, ID_DISKEJECT1, MF_BYCOMMAND | *vm->disk->GetFile(0) ? MF_ENABLED : MF_GRAYED );
		break;
	default:
		EnableMenuItem( hsm, ID_DISKEJECT1, MF_BYCOMMAND | *vm->disk->GetFile(0) ? MF_ENABLED : MF_GRAYED );
		EnableMenuItem( hsm, ID_DISKEJECT2, MF_BYCOMMAND | *vm->disk->GetFile(1) ? MF_ENABLED : MF_GRAYED );
	}
	
	// �g��ROM
	EnableMenuItem( hsm, ID_ROMEJECT, MF_BYCOMMAND | (*vm->mem->GetFile()  ? MF_ENABLED : MF_GRAYED) );
	
	// �R���g���[��
	for( int i=0; i < 5; i++ ){
		if( i < OSD_GetJoyNum() ){
			EnableMenuItem( hsm, ID_JOY101 + i, MF_BYCOMMAND | MF_ENABLED );
			EnableMenuItem( hsm, ID_JOY201 + i, MF_BYCOMMAND | MF_ENABLED );
			
			minfo.fMask      = MIIM_TYPE;
			minfo.dwTypeData = (char *)OSD_GetJoyName( i );
			SetMenuItemInfo( hsm, ID_JOY101 + i, MF_BYCOMMAND, &minfo );
			SetMenuItemInfo( hsm, ID_JOY201 + i, MF_BYCOMMAND, &minfo );
		}else{
			DeleteMenu( hsm, ID_JOY101 + i, MF_BYCOMMAND );
			DeleteMenu( hsm, ID_JOY201 + i, MF_BYCOMMAND );
		}
	}
	CheckMenuRadioItem( hsm, ID_JOY100, ID_JOY105, (joy->GetID(0) < 0) ? ID_JOY100 : ID_JOY101 + joy->GetID(0), MF_BYCOMMAND );
	CheckMenuRadioItem( hsm, ID_JOY200, ID_JOY205, (joy->GetID(1) < 0) ? ID_JOY200 : ID_JOY201 + joy->GetID(1), MF_BYCOMMAND );
	
	// MODE4�J���[
	CheckMenuRadioItem( hsm, ID_M4MONO, ID_M4GRPK, ID_M4MONO + vm->vdg->GetMode4Color(), MF_BYCOMMAND );
	
	// �t���[���X�L�b�v
	CheckMenuRadioItem( hsm, ID_FSKP0, ID_FSKP5, ID_FSKP0 + cfg->GetFrameSkip(), MF_BYCOMMAND );
	
	// �T���v�����O���[�g
	CheckMenuRadioItem( hsm, ID_SPR44, ID_SPR11, ID_SPR11 - ((cfg->GetSampleRate()/11025)>>1), MF_BYCOMMAND );
	
	CheckMenuItem( hsm, ID_NOWAIT,   sche->GetWaitEnable() ? MF_UNCHECKED : MF_CHECKED   );
	CheckMenuItem( hsm, ID_TURBO,    cfg->GetTurboTAPE()   ? MF_CHECKED   : MF_UNCHECKED );
	CheckMenuItem( hsm, ID_BOOST,    vm->cmtl->IsBoostUp() ? MF_CHECKED   : MF_UNCHECKED );
	CheckMenuItem( hsm, ID_SCANLINE, cfg->GetScanLine()    ? MF_CHECKED   : MF_UNCHECKED );
	CheckMenuItem( hsm, ID_DISP43,   cfg->GetDispNTSC()    ? MF_CHECKED   : MF_UNCHECKED );
	CheckMenuItem( hsm, ID_STATUS,   cfg->GetDispStat()    ? MF_CHECKED   : MF_UNCHECKED );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	CheckMenuItem( hsm, ID_MONITOR,  cfg->GetMonDisp()     ? MF_CHECKED   : MF_UNCHECKED );
	#else
	DeleteMenu( hsm, MDEBUG, MF_BYPOSITION );
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	
	// �|�b�v�A�b�v���j���[�\��
	ClientToScreen( hwnd, &pt );
	int id = TrackPopupMenu( hsm, TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL );
	DestroyMenu( hm );
	
	// ���ڂ��Ƃ̏���
	ExecMenu( id );
}


///////////////////////////////////////////////////////////
// ���ݒ�_�C�A���O�\��
//
// ����:	hwnd		�E�B���h�E�n���h��
// �Ԓl:	int			1:OK 0:CANCEL -1:ERROR
///////////////////////////////////////////////////////////
int OSD_ConfigDialog( HWINDOW hwnd )
{
	// INI�t�@�C�����J��
	try{
		ecfg = new CFG6();
		if( !ecfg->Init() ) throw Error::IniReadFailed;
	}
	// new �Ɏ��s�����ꍇ
	catch( std::bad_alloc ){
		return -1;
	}
	// ��O����
	catch( Error::Errno i ){
		delete ecfg;
		ecfg = NULL;
		return -1;
	}
	
	HWND hhwnd = (HWND)OSD_GetWindowHandle( hwnd );
	
	// �y�[�W���̐ݒ���s�Ȃ�
	PROPSHEETPAGE psp[9];
	PROPSHEETHEADER psh;
	
	// ��{
	psp[0].dwSize      = sizeof(PROPSHEETPAGE);
	psp[0].dwFlags     = PSP_DEFAULT;
	psp[0].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[0].pszTemplate = MAKEINTRESOURCE(ID_CNFG1);
	psp[0].pszIcon     = NULL;
	psp[0].pfnDlgProc  = (DLGPROC)OsdCnfgProc1;
	psp[0].pszTitle    = NULL;
	psp[0].lParam      = 0;
	
	// ���
	psp[1].dwSize      = sizeof(PROPSHEETPAGE);
	psp[1].dwFlags     = PSP_DEFAULT;
	psp[1].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[1].pszTemplate = MAKEINTRESOURCE(ID_CNFG2);
	psp[1].pszIcon     = NULL;
	psp[1].pfnDlgProc  = (DLGPROC)OsdCnfgProc2;
	psp[1].pszTitle    = NULL;
	psp[1].lParam      = 0;
	
	// �T�E���h
	psp[2].dwSize      = sizeof(PROPSHEETPAGE);
	psp[2].dwFlags     = PSP_DEFAULT;
	psp[2].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[2].pszTemplate = MAKEINTRESOURCE(ID_CNFG3);
	psp[2].pszIcon     = NULL;
	psp[2].pfnDlgProc  = (DLGPROC)OsdCnfgProc3;
	psp[2].pszTitle    = NULL;
	psp[2].lParam      = 0;
	
	// ���͊֌W
	psp[3].dwSize      = sizeof(PROPSHEETPAGE);
	psp[3].dwFlags     = PSP_DEFAULT;
	psp[3].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[3].pszTemplate = MAKEINTRESOURCE(ID_CNFGIN);
	psp[3].pszIcon     = NULL;
	psp[3].pfnDlgProc  = (DLGPROC)OsdCnfgProcIn;
	psp[3].pszTitle    = NULL;
	psp[3].lParam      = 0;
	
	// �t�@�C��
	psp[4].dwSize      = sizeof(PROPSHEETPAGE);
	psp[4].dwFlags     = PSP_DEFAULT;
	psp[4].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[4].pszTemplate = MAKEINTRESOURCE(ID_CNFG4);
	psp[4].pszIcon     = NULL;
	psp[4].pfnDlgProc  = (DLGPROC)OsdCnfgProc4;
	psp[4].pszTitle    = NULL;
	psp[4].lParam      = 0;
	
	// �t�H���_
	psp[5].dwSize      = sizeof(PROPSHEETPAGE);
	psp[5].dwFlags     = PSP_DEFAULT;
	psp[5].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[5].pszTemplate = MAKEINTRESOURCE(ID_CNFG5);
	psp[5].pszIcon     = NULL;
	psp[5].pfnDlgProc  = (DLGPROC)OsdCnfgProc5;
	psp[5].pszTitle    = NULL;
	psp[5].lParam      = 0;
	
	// �F1
	psp[6].dwSize      = sizeof(PROPSHEETPAGE);
	psp[6].dwFlags     = PSP_DEFAULT;
	psp[6].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[6].pszTemplate = MAKEINTRESOURCE(ID_CNFGCL1);
	psp[6].pszIcon     = NULL;
	psp[6].pfnDlgProc  = (DLGPROC)OsdCnfgProcCol;
	psp[6].pszTitle    = NULL;
	psp[6].lParam      = 0;
	
	// �F2
	psp[7].dwSize      = sizeof(PROPSHEETPAGE);
	psp[7].dwFlags     = PSP_DEFAULT;
	psp[7].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[7].pszTemplate = MAKEINTRESOURCE(ID_CNFGCL2);
	psp[7].pszIcon     = NULL;
	psp[7].pfnDlgProc  = (DLGPROC)OsdCnfgProcCol;
	psp[7].pszTitle    = NULL;
	psp[7].lParam      = 0;
	
	// ���̑�
	psp[8].dwSize      = sizeof(PROPSHEETPAGE);
	psp[8].dwFlags     = PSP_DEFAULT;
	psp[8].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[8].pszTemplate = MAKEINTRESOURCE(ID_CNFGETC);
	psp[8].pszIcon     = NULL;
	psp[8].pfnDlgProc  = (DLGPROC)OsdCnfgProcEtc;
	psp[8].pszTitle    = NULL;
	psp[8].lParam      = 0;
	
	
	psh.dwSize     = sizeof(PROPSHEETHEADER);
	psh.dwFlags    = PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
	psh.hwndParent = hhwnd;
	psh.hInstance  = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psh.pszIcon    = NULL;
	psh.pszCaption = (LPSTR) "���ݒ�";
	psh.nPages     = sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.nStartPage = 0;
	psh.ppsp       = (LPCPROPSHEETPAGE) &psp;
	
	// �v���p�e�B�V�[�g�\��
	int ret = PropertySheet( &psh );
	
	// OK�{�^���������ꂽ�Ȃ�INI�t�@�C��������
	if( ret > 0) ecfg->Write();
	
	delete ecfg;
	ecfg = NULL;
	
	return ret;
}


enum { PP_BASE, PP_DISP, PP_SOUND, PP_FILE, PP_FOLDER, PP_COL, PP_ETC, PP_INPUT };

///////////////////////////////////////////////////////////
// �ݒ��Ǎ���
///////////////////////////////////////////////////////////
static bool OsdReadINI( HWND hwnd, int page )
{
	int st;					// ��Ԏ擾�p
	bool yn;				// ��Ԏ擾�p
	char str[PATH_MAX];		// ������擾�p
	
	if( !ecfg ) return false;
	
	switch( page ){
	case PP_BASE:	// ��{
		// �@��
		st = ecfg->GetModel();
		SendMessage( GetDlgItem( hwnd, ID_RB01 ), BM_SETCHECK, (st==60)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB02 ), BM_SETCHECK, (st==61)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB03 ), BM_SETCHECK, (st==62)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB04 ), BM_SETCHECK, (st==66)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB05 ), BM_SETCHECK, (st==64)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB06 ), BM_SETCHECK, (st==68)? BST_CHECKED:BST_UNCHECKED, 0 );
		
		// FDD
		st = ecfg->GetFddNum();
		SendMessage( GetDlgItem( hwnd, ID_RB31 ), BM_SETCHECK, (st==0)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB32 ), BM_SETCHECK, (st==1)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB33 ), BM_SETCHECK, (st==2)? BST_CHECKED:BST_UNCHECKED, 0 );
		
		// �g��RAM�g�p
		yn = ecfg->GetUseExtRam();
		SendMessage( GetDlgItem( hwnd, ID_CB1 ), BM_SETCHECK, yn, 0 );
		
		// ��m�̃J�[�g���b�W�g�p
		yn = ecfg->GetUseSoldier();
		SendMessage( GetDlgItem( hwnd, ID_CB13 ), BM_SETCHECK, yn, 0 );
		
		break;
		
	case PP_DISP:	// ���
		// MODE4�J���[
		st = ecfg->GetMode4Color();
		SendMessage( GetDlgItem( hwnd, ID_RB11 ), BM_SETCHECK, (st==0)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB12 ), BM_SETCHECK, (st==1)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB13 ), BM_SETCHECK, (st==2)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB14 ), BM_SETCHECK, (st==3)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB15 ), BM_SETCHECK, (st==4)? BST_CHECKED:BST_UNCHECKED, 0 );
		
		// �r�f�I�L���v�`���F�[�x
		st = ecfg->GetAviBpp();
		SendMessage( GetDlgItem( hwnd, ID_RB07 ), BM_SETCHECK, (st==16)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB08 ), BM_SETCHECK, (st==24)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB09 ), BM_SETCHECK, (st==32)? BST_CHECKED:BST_UNCHECKED, 0 );
		
		// �X�L�������C��
		yn = ecfg->GetScanLine();
		SendMessage( GetDlgItem( hwnd, ID_CB2 ), BM_SETCHECK, yn, 0 );
		
		// �X�L�������C���P�x
		st = ecfg->GetScanLineBr();
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_SCANLINEBR, str );
		
		// 4:3�\��
		yn = ecfg->GetDispNTSC();
		SendMessage( GetDlgItem( hwnd, ID_CB7 ), BM_SETCHECK, yn, 0 );
		
		// �t���X�N���[��
		yn = ecfg->GetFullScreen();
		SendMessage( GetDlgItem( hwnd, ID_CB10 ), BM_SETCHECK, yn, 0 );
		
		// �X�e�[�^�X�o�[�\�����
		yn = ecfg->GetDispStat();
		SendMessage( GetDlgItem( hwnd, ID_CB11 ), BM_SETCHECK, yn, 0 );
		
		// �t���[���X�L�b�v
		st = ecfg->GetFrameSkip();
		SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_SETPOS, true, st );
		SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_SETRANGE, true, MAKELPARAM(0,5) );
		
		break;
		
	case PP_SOUND:	// �T�E���h
		// �T���v�����O���[�g
		st = ecfg->GetSampleRate();
		SendMessage( GetDlgItem( hwnd, ID_RB21 ), BM_SETCHECK, (st==44100)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB22 ), BM_SETCHECK, (st==22050)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB23 ), BM_SETCHECK, (st==11025)? BST_CHECKED:BST_UNCHECKED, 0 );
		
		// �o�b�t�@�T�C�Y
		st = ecfg->GetSoundBuffer();
		SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_SETPOS, true, st );
		SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_SETRANGE, true, MAKELPARAM(1,5) );
		
		// PSG LPF�J�b�g�I�t���g��
		st = ecfg->GetPsgLPF();
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_EDIT1, str );
		
		// �}�X�^�[����
		st = ecfg->GetMasterVol();
		SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_SETPOS, true, st/10 );
		SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_SETRANGE, true, MAKELPARAM(0,10) );
		
		// PSG����
		st = ecfg->GetPsgVol();
		SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_SETPOS, true, st/10 );
		SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_SETRANGE, true, MAKELPARAM(0,10) );
		
		// ������������
		st = ecfg->GetVoiceVol();
		SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_SETPOS, true, st/10 );
		SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_SETRANGE, true, MAKELPARAM(0,10) );
		
		// TAPE���j�^����
		st = ecfg->GetCmtVol();
		SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_SETPOS, true, st/10 );
		SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_SETRANGE, true, MAKELPARAM(0,10) );
		
		break;
		
	case PP_INPUT:	// ���͊֌W
		// �L�[���s�[�g�Ԋu
		st = ecfg->GetKeyRepeat();
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_KEYREP, str );
		
		break;
		
	case PP_FOLDER:	// �t�H���_
		// ROM�p�X
		strncpy( str, ecfg->GetRomPath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH1, str );
		
		// TAPE�p�X
		strncpy( str, ecfg->GetTapePath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH2, str );
		
		// DISK�p�X
		strncpy( str, ecfg->GetDiskPath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH3, str );
		
		// �g��ROM�p�X
		strncpy( str, ecfg->GetExtRomPath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH4, str );
		
		// IMG�p�X
		strncpy( str, ecfg->GetImgPath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH5, str );
		
		// WAVE�p�X
		strncpy( str, ecfg->GetWavePath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH6, str );
		
		// �ǂ��ł�SAVE�p�X
		strncpy( str, ecfg->GetDokoSavePath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH7, str );
		
		break;
		
	case PP_FILE:	// �t�@�C��
		// �g��ROM�t�@�C��
		strncpy( str, ecfg->GetExtRomFile(), PATH_MAX );
		SetDlgItemText( hwnd, ID_FEXROM, str );
		
		// TAPE(LOAD)�t�@�C����
		strncpy( str, ecfg->GetTapeFile(), PATH_MAX );
		SetDlgItemText( hwnd, ID_FTPLD, str );
		
		// TAPE(SAVE)�t�@�C����
		strncpy( str, ecfg->GetSaveFile(), PATH_MAX );
		SetDlgItemText( hwnd, ID_FTPSV, str );
		
		// DISK1�t�@�C����
		strncpy( str, ecfg->GetDiskFile( 1 ), PATH_MAX );
		SetDlgItemText( hwnd, ID_FDISK1, str );
		
		// DISK2�t�@�C����
		strncpy( str, ecfg->GetDiskFile( 2 ), PATH_MAX );
		SetDlgItemText( hwnd, ID_FDISK2, str );
		
		// �v�����^�t�@�C����
		strncpy( str, ecfg->GetPrinterFile(), PATH_MAX );
		SetDlgItemText( hwnd, ID_FPRINT, str );
		
		break;
		
	case PP_COL:	// �F1
		break;
		
	case PP_ETC:	// ���̑�
		// �I�[�o�[�N���b�N��
		st = min( max( 1, ecfg->GetOverClock() ), 1000 );
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_OVERCLK, str );
		
		// CRC�`�F�b�N
		yn = ecfg->GetCheckCRC();
		SendMessage( GetDlgItem( hwnd, ID_CB4 ), BM_SETCHECK, yn, 0 );
		
		// ROM�p�b�`
		yn = ecfg->GetRomPatch();
		SendMessage( GetDlgItem( hwnd, ID_CB9 ), BM_SETCHECK, yn, 0 );
		
		// Turbo TAPE
		yn = ecfg->GetTurboTAPE();
		SendMessage( GetDlgItem( hwnd, ID_CB3 ), BM_SETCHECK, yn, 0 );
		
		// Boost Up
		yn = ecfg->GetBoostUp();
		SendMessage( GetDlgItem( hwnd, ID_CB5 ), BM_SETCHECK, yn, 0 );
		
		// BoostUp �ő�{��(N60���[�h)
		st = min( max( 1, ecfg->GetMaxBoost1() ), 100 );
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_BOOST60, str );
		
		// BoostUp �ő�{��(N60m/N66���[�h)
		st = min( max( 1, ecfg->GetMaxBoost2() ), 100 );
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_BOOST62, str );
		
		// �I���� �m�F����
		yn = ecfg->GetCkQuit();
		SendMessage( GetDlgItem( hwnd, ID_CB8 ), BM_SETCHECK, yn, 0 );
		
		// �I���� INI�t�@�C����ۑ�����
		yn = ecfg->GetSaveQuit();
		SendMessage( GetDlgItem( hwnd, ID_CB12 ), BM_SETCHECK, yn, 0 );
		
		break;
	}
	
	return true;
}


///////////////////////////////////////////////////////////
// �ݒ��ۑ�����
///////////////////////////////////////////////////////////
static bool OsdWriteINI( HWND hwnd, int page )
{
	int st;					// ��Ԏ擾�p
	char str[PATH_MAX];		// ������擾�p
	
	if( !ecfg ) return false;
	
	switch( page ){
	case PP_BASE:	// ��{
		// �@��
		if     ( IsDlgButtonChecked( hwnd, ID_RB02 ) ) st = 61;	// PC-6001A
		else if( IsDlgButtonChecked( hwnd, ID_RB03 ) ) st = 62;	// PC-6001mk2
		else if( IsDlgButtonChecked( hwnd, ID_RB04 ) ) st = 66;	// PC-6601
		else if( IsDlgButtonChecked( hwnd, ID_RB05 ) ) st = 64;	// PC-6001mk2SR
		else if( IsDlgButtonChecked( hwnd, ID_RB06 ) ) st = 68;	// PC-6601SR
		else                                           st = 60;	// ��L�ȊO�Ȃ�PC-6001
		ecfg->SetModel( st );
		
		// FDD
		if     ( IsDlgButtonChecked( hwnd, ID_RB32 ) ) st = 1;		// 2��
		else if( IsDlgButtonChecked( hwnd, ID_RB33 ) ) st = 2;		// 1��
		else                                           st = 0;		// ��L�ȊO�Ȃ�Ȃ�
		ecfg->SetFddNum( st );
		
		// �g��RAM�g�p
		ecfg->SetUseExtRam( IsDlgButtonChecked( hwnd, ID_CB1 ) );
		
		// ��m�̃J�[�g���b�W�g�p
		ecfg->SetUseSoldier( IsDlgButtonChecked( hwnd, ID_CB13 ) );
		
		break;
		
	case PP_DISP:	// ���
		// MODE4�J���[
		if     ( IsDlgButtonChecked( hwnd, ID_RB12 ) ) st = 1;	// ��/��
		else if( IsDlgButtonChecked( hwnd, ID_RB13 ) ) st = 2;	// ��/��
		else if( IsDlgButtonChecked( hwnd, ID_RB14 ) ) st = 3;	// �s���N/��
		else if( IsDlgButtonChecked( hwnd, ID_RB15 ) ) st = 4;	// ��/�s���N
		else                                           st = 0;	// ��L�ȊO�Ȃ烂�m�N��
		ecfg->SetMode4Color( st );
		
		// �r�f�I�L���v�`���F�[�x
		if     ( IsDlgButtonChecked( hwnd, ID_RB07 ) ) st = 16;	// 16bit
		else if( IsDlgButtonChecked( hwnd, ID_RB08 ) ) st = 24;	// 24bit
		else                                           st = 32;	// ��L�ȊO�Ȃ�32bit
		ecfg->SetAviBpp( st );
		
		// �X�L�������C��
		ecfg->SetScanLine( IsDlgButtonChecked( hwnd, ID_CB2 ) );
		
		// �X�L�������C���P�x
		GetDlgItemText( hwnd, ID_SCANLINEBR, str, sizeof(str) );
		st = strtol( str, NULL, 0 );
		ecfg->SetScanLineBr( st );
		
		// 4:3�\��
		ecfg->SetDispNTSC( IsDlgButtonChecked( hwnd, ID_CB7 ) );
		
		// �t���X�N���[��
		ecfg->SetFullScreen( IsDlgButtonChecked( hwnd, ID_CB10 ) );
		
		// �X�e�[�^�X�o�[�\�����
		ecfg->SetDispStat( IsDlgButtonChecked( hwnd, ID_CB11 ) );
		
		// �t���[���X�L�b�v
		st = SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_GETPOS, 0, 0 );
		ecfg->SetFrameSkip( st );
		
		break;
		
	case PP_SOUND:	// �T�E���h
		// �T���v�����O���[�g
		if     ( IsDlgButtonChecked( hwnd, ID_RB22 ) ) st = 22050;	// 22050
		else if( IsDlgButtonChecked( hwnd, ID_RB23 ) ) st = 11025;	// 11025
		else                                           st = 44100;	// ��L�ȊO�Ȃ�44100
		ecfg->SetSampleRate( st );
		
		// �o�b�t�@�T�C�Y
		st = SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_GETPOS, 0, 0 );
		ecfg->SetSoundBuffer( st );
		
		// PSG LPF�J�b�g�I�t���g��
		GetDlgItemText( hwnd, ID_EDIT1, str, sizeof(str) );
		st = strtol( str, NULL, 0 );
		ecfg->SetPsgLPF( st );
		
		// �}�X�^�[����
		st = SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_GETPOS, 0, 0 );
		ecfg->SetMasterVol( st*10 );
		
		// PSG����
		st = SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_GETPOS, 0, 0 );
		ecfg->SetPsgVol( st*10 );
		
		// ������������
		st = SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_GETPOS, 0, 0 );
		ecfg->SetVoiceVol( st*10 );
		
		// TAPE���j�^����
		st = SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_GETPOS, 0, 0 );
		ecfg->SetCmtVol( st*10 );
		
		break;
		
	case PP_INPUT:	// ���͊֌W
		// �L�[���s�[�g�Ԋu
		GetDlgItemText( hwnd, ID_KEYREP, str, sizeof(str) );
		st = strtol( str, NULL, 0 );
		ecfg->SetKeyRepeat( st );
		
		break;
		
	case PP_FOLDER:	// �t�H���_
		// ROM�p�X
		GetDlgItemText( hwnd, ID_PATH1, str, sizeof(str) );
		ecfg->SetRomPath( str );
		
		// TAPE�p�X
		GetDlgItemText( hwnd, ID_PATH2, str, sizeof(str) );
		ecfg->SetTapePath( str );
		
		// DISK�p�X
		GetDlgItemText( hwnd, ID_PATH3, str, sizeof(str) );
		ecfg->SetDiskPath( str );
		
		// �g��ROM�p�X
		GetDlgItemText( hwnd, ID_PATH4, str, sizeof(str) );
		ecfg->SetExtRomPath( str );
		
		// IMG�p�X
		GetDlgItemText( hwnd, ID_PATH5, str, sizeof(str) );
		ecfg->SetImgPath( str );
		
		// WAVE�p�X
		GetDlgItemText( hwnd, ID_PATH6, str, sizeof(str) );
		ecfg->SetWavePath( str );
		
		// �ǂ��ł�SAVE�p�X
		GetDlgItemText( hwnd, ID_PATH7, str, sizeof(str) );
		ecfg->SetDokoSavePath( str );
		
		break;
		
	case PP_FILE:	// �t�@�C��
		// �g��ROM�t�@�C��
		GetDlgItemText( hwnd, ID_FEXROM, str, sizeof(str) );
		ecfg->SetExtRomFile( str );
		
		// TAPE(LOAD)�t�@�C����
		GetDlgItemText( hwnd, ID_FTPLD, str, sizeof(str) );
		ecfg->SetTapeFile( str );
		
		// TAPE(SAVE)�t�@�C����
		GetDlgItemText( hwnd, ID_FTPSV, str, sizeof(str) );
		ecfg->SetSaveFile( str );
		
		// DISK1�t�@�C����
		GetDlgItemText( hwnd, ID_FDISK1, str, sizeof(str) );
		ecfg->SetDiskFile( 1, str );
		
		// DISK2�t�@�C����
		GetDlgItemText( hwnd, ID_FDISK2, str, sizeof(str) );
		ecfg->SetDiskFile( 2, str );
		
		// �v�����^�t�@�C����
		GetDlgItemText( hwnd, ID_FPRINT, str, sizeof(str) );
		ecfg->SetPrinterFile( str );
		
		break;
		
	case PP_COL:	// �F1
		break;
		
	case PP_ETC:	// ���̑�
		// �I�[�o�[�N���b�N��
		GetDlgItemText( hwnd, ID_OVERCLK, str, sizeof(str) );
		st = min( max( 1, strtol( str, NULL, 0 ) ), 1000 );
		ecfg->SetOverClock( st );
		
		// CRC�`�F�b�N
		ecfg->SetCheckCRC( IsDlgButtonChecked( hwnd, ID_CB4 ) );
		
		// ROM�p�b�`
		ecfg->SetRomPatch( IsDlgButtonChecked( hwnd, ID_CB9 ) );
		
		// Turbo TAPE
		ecfg->SetTurboTAPE( IsDlgButtonChecked( hwnd, ID_CB3 ) );
		
		// Boost Up
		ecfg->SetBoostUp( IsDlgButtonChecked( hwnd, ID_CB5 ) );
		
		// BoostUp �ő�{��(N60���[�h)
		GetDlgItemText( hwnd, ID_BOOST60, str, sizeof(str) );
		st = min( max( 1, strtol( str, NULL, 0 ) ), 100 );
		ecfg->SetMaxBoost1( st );
		
		// BoostUp �ő�{��(N60m/N66���[�h)
		GetDlgItemText( hwnd, ID_BOOST62, str, sizeof(str) );
		st = min( max( 1, strtol( str, NULL, 0 ) ), 100 );
		ecfg->SetMaxBoost2( st );
		
		// �I���� �m�F����
		ecfg->SetCkQuit( IsDlgButtonChecked( hwnd, ID_CB8 ) );
		
		// �I���� INI�t�@�C����ۑ�����
		ecfg->SetSaveQuit( IsDlgButtonChecked( hwnd, ID_CB12 ) );
		
		break;
	}
	
	return true;
}


///////////////////////////////////////////////////////////
// ���ݒ�_�C�A���O�v���V�[�W��
///////////////////////////////////////////////////////////
// ��{
static bool CALLBACK OsdCnfgProc1( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// �ݒ��Ǎ���
		if( !OsdReadINI( hwnd, PP_BASE ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_COMMAND:
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:	// OK�{�^������
			// �ݒ��ۑ�����
			if( !OsdWriteINI( hwnd, PP_BASE ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// ���
static bool CALLBACK OsdCnfgProc2( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// �ݒ��Ǎ���
		if( !OsdReadINI( hwnd, PP_DISP ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_COMMAND:
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// �ݒ��ۑ�����
			if( !OsdWriteINI( hwnd, PP_DISP ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// �T�E���h
static bool CALLBACK OsdCnfgProc3( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// �ݒ��Ǎ���
		if( !OsdReadINI( hwnd, PP_SOUND ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// �ݒ��ۑ�����
			if( !OsdWriteINI( hwnd, PP_SOUND ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// �t�@�C��
static bool CALLBACK OsdCnfgProc4( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// �ݒ��Ǎ���
		if( !OsdReadINI( hwnd, PP_FILE ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_COMMAND:
		char Folder[PATH_MAX];
		switch( wp ){
		case ID_B11:	// �t�@�C���Q�ƃ{�^��(�g��ROM�C���[�W)
			GetDlgItemText( hwnd, ID_FEXROM, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_ExtRom, Folder, ecfg->GetExtRomPath() ) ){
				SetDlgItemText( hwnd, ID_FEXROM, Folder );
			}
			break;
			
		case ID_B12:	// �t�@�C���Q�ƃ{�^��(TAPE�C���[�W(LOAD))
			GetDlgItemText( hwnd, ID_FTPLD, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_TapeLoad, Folder, ecfg->GetTapePath() ) ){
				SetDlgItemText( hwnd, ID_FTPLD, Folder );
			}
			break;
			
		case ID_B13:	// �t�@�C���Q�ƃ{�^��(TAPE�C���[�W(SAVE))
			GetDlgItemText( hwnd, ID_FTPSV, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_TapeSave, Folder, ecfg->GetTapePath() ) ){
				SetDlgItemText( hwnd, ID_FTPSV, Folder );
			}
			break;
			
		case ID_B14:	// �t�@�C���Q�ƃ{�^��(DISK1�C���[�W)
			GetDlgItemText( hwnd, ID_FDISK1, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_Disk, Folder, ecfg->GetDiskPath() ) ){
				SetDlgItemText( hwnd, ID_FDISK1, Folder );
			}
			break;
			
		case ID_B15:	// �t�@�C���Q�ƃ{�^��(DISK2�C���[�W)
			GetDlgItemText( hwnd, ID_FDISK2, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_Disk, Folder, ecfg->GetDiskPath() ) ){
				SetDlgItemText( hwnd, ID_FDISK2, Folder );
			}
			break;
			
		case ID_B16:	// �t�@�C���Q�ƃ{�^��(�v�����^�o�̓t�@�C��)
			GetDlgItemText( hwnd, ID_FPRINT, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_Printer, Folder, NULL ) ){
				SetDlgItemText( hwnd, ID_FPRINT, Folder );
			}
			break;
			
		case ID_B11E:	// EJECT�{�^��(�g��ROM�C���[�W)
			SetDlgItemText( hwnd, ID_FEXROM, "" );
			break;
			
		case ID_B12E:	// EJECT�{�^��(TAPE�C���[�W(LOAD))
			SetDlgItemText( hwnd, ID_FTPLD, "" );
			break;
			
		case ID_B13E:	// EJECT�{�^��(TAPE�C���[�W(SAVE))
			SetDlgItemText( hwnd, ID_FTPSV, "" );
			break;
			
		case ID_B14E:	// EJECT�{�^��(DISK1�C���[�W)
			SetDlgItemText( hwnd, ID_FDISK1, "" );
			break;
			
		case ID_B15E:	// EJECT�{�^��(DISK2�C���[�W)
			SetDlgItemText( hwnd, ID_FDISK2, "" );
			break;
			
		case ID_B16E:	// EJECT�{�^��(�v�����^�o�̓t�@�C��)
			SetDlgItemText( hwnd, ID_FPRINT, "" );
			break;
		}
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// �ݒ��ۑ�����
			if( !OsdWriteINI( hwnd, PP_FILE ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// �t�H���_
static bool CALLBACK OsdCnfgProc5( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// �ݒ��Ǎ���
		if( !OsdReadINI( hwnd, PP_FOLDER ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_COMMAND:
		char Folder[PATH_MAX];
		switch( wp ){
		case ID_B01:	// �t�H���_�Q�ƃ{�^��(ROM)
			GetDlgItemText( hwnd, ID_PATH1, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH1, Folder );
			break;
			
		case ID_B02:	// �t�H���_�Q�ƃ{�^��(TAPE)
			GetDlgItemText( hwnd, ID_PATH2, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH2, Folder );
			break;
			
		case ID_B03:	// �t�H���_�Q�ƃ{�^��(DISK)
			GetDlgItemText( hwnd, ID_PATH3, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH3, Folder );
			break;
			
		case ID_B04:	// �t�H���_�Q�ƃ{�^��(�g��ROM)
			GetDlgItemText( hwnd, ID_PATH4, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH4, Folder );
			break;
			
		case ID_B05:	// �t�H���_�Q�ƃ{�^��(IMG)
			GetDlgItemText( hwnd, ID_PATH5, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH5, Folder );
			break;
			
		case ID_B06:	// �t�H���_�Q�ƃ{�^��(WAVE)
			GetDlgItemText( hwnd, ID_PATH6, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH6, Folder );
			break;
			
		case ID_B07:	// �t�H���_�Q�ƃ{�^��(�ǂ��ł�SAVE)
			GetDlgItemText( hwnd, ID_PATH7, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH7, Folder );
			break;
		}
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// �ݒ��ۑ�����
			if( !OsdWriteINI( hwnd, PP_FOLDER ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// �F1
static bool CALLBACK OsdCnfgProcCol( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	static COLORREF Color = 0;
	static COLORREF CustColors[16] = { RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
									   RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
									   RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
									   RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ) };
	static CHOOSECOLOR cc;	// �F�̑I��p
	COLOR24 col;
	
	switch( msg ){
	case WM_INITDIALOG:
		// �ݒ��Ǎ���
		if( !OsdReadINI( hwnd, PP_COL ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		
		// �F�̑I�� �R�����_�C�A���O ������
		cc.lStructSize  = sizeof(CHOOSECOLOR);
		cc.hwndOwner    = hwnd;
		cc.rgbResult    = Color;
		cc.lpCustColors = CustColors;
		cc.Flags        = CC_FULLOPEN | CC_RGBINIT;
		
		break;
		
	case WM_DRAWITEM:	// �I�[�i�[�h���[
		if( wp >= ID_COL16 && wp <= ID_COL80 && ecfg->GetColor( wp-ID_COL16+16, &col ) ){
			// �I�[�i�[�h���[�{�^�� �`��
			OsdOwnerDrawBtn( ((LPDRAWITEMSTRUCT)lp)->hDC,
							 ((LPDRAWITEMSTRUCT)lp)->rcItem,
							 RGB( col.r, col.g, col.b ) );
			return true;
		}
		return false;
		
	case WM_COMMAND:
		if( wp >= ID_COL16 && wp <= ID_COL80 && ecfg->GetColor( wp-ID_COL16+16, &col ) ){
			Color = RGB( col.r, col.g, col.b );
			// �F�̑I�� �R�����_�C�A���O�\��
			cc.rgbResult = Color;
			if( ChooseColor( &cc ) ){
				Color = cc.rgbResult;
				col.r = GetRValue( Color );
				col.g = GetGValue( Color );
				col.b = GetBValue( Color );
				ecfg->SetColor( wp-ID_COL16+16, &col );
				InvalidateRect( hwnd, NULL, true );
			}
		}
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// �ݒ��ۑ�����
			if( !OsdWriteINI( hwnd, PP_COL ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// ���̑�
static bool CALLBACK OsdCnfgProcEtc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// �ݒ��Ǎ���
		if( !OsdReadINI( hwnd, PP_ETC ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// �ݒ��ۑ�����
			if( !OsdWriteINI( hwnd, PP_ETC ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// ���͊֌W
static bool CALLBACK OsdCnfgProcIn( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// �ݒ��Ǎ���
		if( !OsdReadINI( hwnd, PP_INPUT ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// �ݒ��ۑ�����
			if( !OsdWriteINI( hwnd, PP_INPUT ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}


// �I�[�i�[�h���[�{�^�� �`��
static void OsdOwnerDrawBtn( HDC hdc, RECT rc, COLORREF col )
{
	HPEN hOldPen   = (HPEN)SelectObject( hdc, (HPEN)GetStockObject( BLACK_PEN ) );
	HBRUSH hBrs    = CreateSolidBrush( col );
	HBRUSH hOldBrs = (HBRUSH)SelectObject( hdc, hBrs );
	
	Rectangle( hdc, rc.left, rc.top, rc.right, rc.bottom );
	
	SelectObject( hdc, hOldPen );
	SelectObject( hdc, hOldBrs );
	DeleteObject( hBrs );
}



///////////////////////////////////////////////////////////
// �o�[�W�������\��
//
// ����:	hwnd		�E�B���h�E�n���h��
//			mdl			�@��
// �Ԓl:	�Ȃ�
///////////////////////////////////////////////////////////
void OSD_VersionDialog( HWINDOW hwnd, int mdl )
{
	model = mdl;
	HWND hhwnd = (HWND)OSD_GetWindowHandle( hwnd );
	DialogBox( (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(ID_VER), hhwnd, (DLGPROC)VerInfoProc );
}


///////////////////////////////////////////////////////////
// �o�[�W�������_�C�A���O�v���V�[�W��
///////////////////////////////////////////////////////////
static bool CALLBACK VerInfoProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	static HCURSOR hCursor = NULL;
	
	switch( msg ){
	case WM_INITDIALOG:
		{
		// �o�[�W����������ݒ�
		SetDlgItemText( hwnd, ID_VERSTR, APPNAME " Ver." VERSION );
		
		// �A�C�R���ݒ�
		const char *ilp;
		switch( model ){
		case 61: ilp = MAKEINTRESOURCE(P61ICON); break;
		case 62: ilp = MAKEINTRESOURCE(P62ICON); break;
		case 66: ilp = MAKEINTRESOURCE(P66ICON); break;
		case 64: ilp = MAKEINTRESOURCE(P64ICON); break;
		case 68: ilp = MAKEINTRESOURCE(P68ICON); break;
		default: ilp = MAKEINTRESOURCE(P60ICON);
		}
		HICON hNewIcon = LoadIcon( (HINSTANCE)GetWindowLongPtr( hwnd, GWLP_HINSTANCE), ilp );
		HICON hOldIcon = (HICON)SendMessage( GetDlgItem( hwnd, ID_VERICON ), STM_SETICON, (WPARAM)hNewIcon, 0 );
		if( hOldIcon ) DeleteObject( hOldIcon );
		
		// �w�^�J�[�\���擾
		HINSTANCE hInstHelp;
		char WinDir[PATH_MAX];
		
		GetWindowsDirectory( WinDir, PATH_MAX-13 );
		lstrcat( WinDir, "\\winhlp32.exe");
		hInstHelp = LoadLibrary( WinDir );
		if( hInstHelp ){
			hCursor = (HCURSOR)CopyImage( LoadImage( hInstHelp, MAKEINTRESOURCE(106), IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR ), IMAGE_CURSOR, 0, 0, LR_COPYDELETEORG );
			FreeLibrary( hInstHelp );
		}else
			hCursor = (HCURSOR)LoadImage( NULL, IDC_ARROW, IMAGE_CURSOR, 0, 0, LR_DEFAULTCOLOR );
		break;
		}
	case WM_SETCURSOR:	// �J�[�\���ύX
		if( (HWND)wp == GetDlgItem( hwnd, ID_URL ) ){
			SetCursor( hCursor );
			GetWindowLongPtr( hwnd, DWLP_MSGRESULT );
			return true;
		}
		return false;
		break;
		
	case WM_CTLCOLORSTATIC:	// URL�̐F��ς���
		{
		HDC hDC = (HDC)wp;
		if( (HWND)lp == GetDlgItem( hwnd, ID_URL ) ){
			SetBkMode( hDC, TRANSPARENT );
			SetTextColor( hDC, RGB(0,0,255) );	// �F��ς���
			return GetStockObject( NULL_BRUSH );
		}
		return ( COLOR_BTNFACE + 1 ) ? true : false;
		}
		break;
		
	case WM_COMMAND:
    	switch( LOWORD(wp) ){
		case IDOK:
		case IDCANCEL:
			if( hCursor ) DestroyCursor( hCursor );	// �J�[�\���J��
			EndDialog( hwnd, true );
			break;
		case ID_URL:
			{
			char Url[256];
			GetWindowText( GetDlgItem( hwnd, ID_URL ), Url, 256 );
			ShellExecute( hwnd, "open", Url, NULL, NULL, SW_SHOWNORMAL );
			InvalidateRect( GetDlgItem( hwnd, ID_URL ), NULL, false );
			return true;
			}
		}
	}
	return false;
}
