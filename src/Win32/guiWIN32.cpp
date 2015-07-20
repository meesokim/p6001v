// OS依存の汎用ルーチン(主にUI用)

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
// ローカル関数定義
///////////////////////////////////////////////////////////
static bool OsdReadINI(  HWND, int );			// 設定を読込む
static bool OsdWriteINI( HWND, int );			// 設定を保存する

static bool CALLBACK OsdCnfgProc1(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProc2(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProc3(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProc4(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProc5(   HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProcCol( HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProcEtc( HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK OsdCnfgProcIn(  HWND, UINT, WPARAM, LPARAM );
static bool CALLBACK VerInfoProc(    HWND, UINT, WPARAM, LPARAM );

static void OsdOwnerDrawBtn( HDC, RECT, COLORREF );	// オーナードローボタン 描画

///////////////////////////////////////////////////////////
// 仕方なしにスタティック変数
///////////////////////////////////////////////////////////
static CFG6 *ecfg = NULL;					// 環境設定オブジェクトポインタ(編集用)
static int model = 60;						// 機種



///////////////////////////////////////////////////////////
// ポップアップメニュー表示
///////////////////////////////////////////////////////////
void EL6::ShowPopupMenu( int x, int y )
{
	// MF_BYPOSITION用
	enum {	MSYSTEM = 0,	// システム
			MSEP1,			// ----------
			MTAPE,			// TAPE
			MDISK,			// DISK
			MEXTROM,		// 拡張ROM
			MCONT,			// コントローラ
			MCONFIG,		// 設定
			MDEBUG,			// デバッグ
			MSEP2,			// ----------
			MHELP			// ヘルプ
		};
	
	POINT pt;
	
	pt.x = x;
	pt.y = y;
	HWND hwnd = (HWND)OSD_GetWindowHandle( graph->GetWindowHandle() );
	
	if( GetMenu( hwnd ) ) return;
	
	HMENU hm  = LoadMenu( (HINSTANCE)GetWindowLongPtr( hwnd, GWLP_HINSTANCE), MAKEINTRESOURCE( ID_MENU ) );
	HMENU hsm = GetSubMenu( hm, MSYSTEM );
	
	// メニューの前処理
	MENUITEMINFO minfo;
	minfo.cbSize = sizeof(MENUITEMINFO);
	
	// ビデオキャプチャ
	minfo.fMask      = MIIM_TYPE;
	minfo.dwTypeData = NULL;
	GetMenuItemInfo( hsm, ID_AVISAVE, MF_BYCOMMAND, &minfo );
	minfo.dwTypeData = (char *)(AVI6::IsAVI() ? MSMEN_AVI1 : MSMEN_AVI0);
	SetMenuItemInfo( hsm, ID_AVISAVE, MF_BYCOMMAND, &minfo );
	// モニタモード or ブレークポインタが設定されていたらビデオキャプチャ無効
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( cfg->GetMonDisp() || vm->BpExistBreakPoint() )
		EnableMenuItem( hsm, ID_AVISAVE, MF_BYCOMMAND | MF_GRAYED );
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// リプレイ記録
	minfo.fMask      = MIIM_TYPE;
	minfo.dwTypeData = NULL;
	GetMenuItemInfo( hsm, ID_REPLAYSAVE, MF_BYCOMMAND, &minfo );
	minfo.dwTypeData = (char *)(( REPLAY::GetStatus() == REP_RECORD ) ? MSMEN_REP1 : MSMEN_REP0);
	SetMenuItemInfo( hsm, ID_REPLAYSAVE, MF_BYCOMMAND, &minfo );
	// モニタモード or ブレークポインタが設定されている
	// またはリプレイ再生中だったらリプレイ記録無効
	if( 
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		cfg->GetMonDisp() || vm->BpExistBreakPoint() ||
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		( REPLAY::GetStatus() == REP_REPLAY ) ){
		EnableMenuItem( hsm, ID_REPLAYSAVE,   MF_BYCOMMAND | MF_GRAYED );
		EnableMenuItem( hsm, ID_REPLAYRESUME, MF_BYCOMMAND | MF_GRAYED );
	}
	// 途中保存、やり直しはリプレイ記録中のみ
	if(
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		cfg->GetMonDisp() || vm->BpExistBreakPoint() ||
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		( REPLAY::GetStatus() != REP_RECORD ) ){
		EnableMenuItem( hsm, ID_REPLAYDOKOSAVE, MF_BYCOMMAND | MF_GRAYED );
		EnableMenuItem( hsm, ID_REPLAYDOKOLOAD, MF_BYCOMMAND | MF_GRAYED );
	}
	
	// リプレイ再生
	minfo.fMask      = MIIM_TYPE;
	minfo.dwTypeData = NULL;
	GetMenuItemInfo( hsm, ID_REPLAYLOAD, MF_BYCOMMAND, &minfo );
	minfo.dwTypeData = (char *)(( REPLAY::GetStatus() == REP_REPLAY ) ? MSMEN_REP3: MSMEN_REP2);
	SetMenuItemInfo( hsm, ID_REPLAYLOAD, MF_BYCOMMAND, &minfo );
	// モニタモード or ブレークポインタが設定されている
	// またはリプレイ記録中だったらリプレイ再生無効
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
	
	// 拡張ROM
	EnableMenuItem( hsm, ID_ROMEJECT, MF_BYCOMMAND | (*vm->mem->GetFile()  ? MF_ENABLED : MF_GRAYED) );
	
	// コントローラ
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
	
	// MODE4カラー
	CheckMenuRadioItem( hsm, ID_M4MONO, ID_M4GRPK, ID_M4MONO + vm->vdg->GetMode4Color(), MF_BYCOMMAND );
	
	// フレームスキップ
	CheckMenuRadioItem( hsm, ID_FSKP0, ID_FSKP5, ID_FSKP0 + cfg->GetFrameSkip(), MF_BYCOMMAND );
	
	// サンプリングレート
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
	
	
	// ポップアップメニュー表示
	ClientToScreen( hwnd, &pt );
	int id = TrackPopupMenu( hsm, TPM_RETURNCMD | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, NULL );
	DestroyMenu( hm );
	
	// 項目ごとの処理
	ExecMenu( id );
}


///////////////////////////////////////////////////////////
// 環境設定ダイアログ表示
//
// 引数:	hwnd		ウィンドウハンドル
// 返値:	int			1:OK 0:CANCEL -1:ERROR
///////////////////////////////////////////////////////////
int OSD_ConfigDialog( HWINDOW hwnd )
{
	// INIファイルを開く
	try{
		ecfg = new CFG6();
		if( !ecfg->Init() ) throw Error::IniReadFailed;
	}
	// new に失敗した場合
	catch( std::bad_alloc ){
		return -1;
	}
	// 例外発生
	catch( Error::Errno i ){
		delete ecfg;
		ecfg = NULL;
		return -1;
	}
	
	HWND hhwnd = (HWND)OSD_GetWindowHandle( hwnd );
	
	// ページ毎の設定を行なう
	PROPSHEETPAGE psp[9];
	PROPSHEETHEADER psh;
	
	// 基本
	psp[0].dwSize      = sizeof(PROPSHEETPAGE);
	psp[0].dwFlags     = PSP_DEFAULT;
	psp[0].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[0].pszTemplate = MAKEINTRESOURCE(ID_CNFG1);
	psp[0].pszIcon     = NULL;
	psp[0].pfnDlgProc  = (DLGPROC)OsdCnfgProc1;
	psp[0].pszTitle    = NULL;
	psp[0].lParam      = 0;
	
	// 画面
	psp[1].dwSize      = sizeof(PROPSHEETPAGE);
	psp[1].dwFlags     = PSP_DEFAULT;
	psp[1].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[1].pszTemplate = MAKEINTRESOURCE(ID_CNFG2);
	psp[1].pszIcon     = NULL;
	psp[1].pfnDlgProc  = (DLGPROC)OsdCnfgProc2;
	psp[1].pszTitle    = NULL;
	psp[1].lParam      = 0;
	
	// サウンド
	psp[2].dwSize      = sizeof(PROPSHEETPAGE);
	psp[2].dwFlags     = PSP_DEFAULT;
	psp[2].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[2].pszTemplate = MAKEINTRESOURCE(ID_CNFG3);
	psp[2].pszIcon     = NULL;
	psp[2].pfnDlgProc  = (DLGPROC)OsdCnfgProc3;
	psp[2].pszTitle    = NULL;
	psp[2].lParam      = 0;
	
	// 入力関係
	psp[3].dwSize      = sizeof(PROPSHEETPAGE);
	psp[3].dwFlags     = PSP_DEFAULT;
	psp[3].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[3].pszTemplate = MAKEINTRESOURCE(ID_CNFGIN);
	psp[3].pszIcon     = NULL;
	psp[3].pfnDlgProc  = (DLGPROC)OsdCnfgProcIn;
	psp[3].pszTitle    = NULL;
	psp[3].lParam      = 0;
	
	// ファイル
	psp[4].dwSize      = sizeof(PROPSHEETPAGE);
	psp[4].dwFlags     = PSP_DEFAULT;
	psp[4].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[4].pszTemplate = MAKEINTRESOURCE(ID_CNFG4);
	psp[4].pszIcon     = NULL;
	psp[4].pfnDlgProc  = (DLGPROC)OsdCnfgProc4;
	psp[4].pszTitle    = NULL;
	psp[4].lParam      = 0;
	
	// フォルダ
	psp[5].dwSize      = sizeof(PROPSHEETPAGE);
	psp[5].dwFlags     = PSP_DEFAULT;
	psp[5].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[5].pszTemplate = MAKEINTRESOURCE(ID_CNFG5);
	psp[5].pszIcon     = NULL;
	psp[5].pfnDlgProc  = (DLGPROC)OsdCnfgProc5;
	psp[5].pszTitle    = NULL;
	psp[5].lParam      = 0;
	
	// 色1
	psp[6].dwSize      = sizeof(PROPSHEETPAGE);
	psp[6].dwFlags     = PSP_DEFAULT;
	psp[6].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[6].pszTemplate = MAKEINTRESOURCE(ID_CNFGCL1);
	psp[6].pszIcon     = NULL;
	psp[6].pfnDlgProc  = (DLGPROC)OsdCnfgProcCol;
	psp[6].pszTitle    = NULL;
	psp[6].lParam      = 0;
	
	// 色2
	psp[7].dwSize      = sizeof(PROPSHEETPAGE);
	psp[7].dwFlags     = PSP_DEFAULT;
	psp[7].hInstance   = (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE);
	psp[7].pszTemplate = MAKEINTRESOURCE(ID_CNFGCL2);
	psp[7].pszIcon     = NULL;
	psp[7].pfnDlgProc  = (DLGPROC)OsdCnfgProcCol;
	psp[7].pszTitle    = NULL;
	psp[7].lParam      = 0;
	
	// その他
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
	psh.pszCaption = (LPSTR) "環境設定";
	psh.nPages     = sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.nStartPage = 0;
	psh.ppsp       = (LPCPROPSHEETPAGE) &psp;
	
	// プロパティシート表示
	int ret = PropertySheet( &psh );
	
	// OKボタンが押されたならINIファイル書込み
	if( ret > 0) ecfg->Write();
	
	delete ecfg;
	ecfg = NULL;
	
	return ret;
}


enum { PP_BASE, PP_DISP, PP_SOUND, PP_FILE, PP_FOLDER, PP_COL, PP_ETC, PP_INPUT };

///////////////////////////////////////////////////////////
// 設定を読込む
///////////////////////////////////////////////////////////
static bool OsdReadINI( HWND hwnd, int page )
{
	int st;					// 状態取得用
	bool yn;				// 状態取得用
	char str[PATH_MAX];		// 文字列取得用
	
	if( !ecfg ) return false;
	
	switch( page ){
	case PP_BASE:	// 基本
		// 機種
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
		
		// 拡張RAM使用
		yn = ecfg->GetUseExtRam();
		SendMessage( GetDlgItem( hwnd, ID_CB1 ), BM_SETCHECK, yn, 0 );
		
		// 戦士のカートリッジ使用
		yn = ecfg->GetUseSoldier();
		SendMessage( GetDlgItem( hwnd, ID_CB13 ), BM_SETCHECK, yn, 0 );
		
		break;
		
	case PP_DISP:	// 画面
		// MODE4カラー
		st = ecfg->GetMode4Color();
		SendMessage( GetDlgItem( hwnd, ID_RB11 ), BM_SETCHECK, (st==0)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB12 ), BM_SETCHECK, (st==1)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB13 ), BM_SETCHECK, (st==2)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB14 ), BM_SETCHECK, (st==3)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB15 ), BM_SETCHECK, (st==4)? BST_CHECKED:BST_UNCHECKED, 0 );
		
		// ビデオキャプチャ色深度
		st = ecfg->GetAviBpp();
		SendMessage( GetDlgItem( hwnd, ID_RB07 ), BM_SETCHECK, (st==16)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB08 ), BM_SETCHECK, (st==24)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB09 ), BM_SETCHECK, (st==32)? BST_CHECKED:BST_UNCHECKED, 0 );
		
		// スキャンライン
		yn = ecfg->GetScanLine();
		SendMessage( GetDlgItem( hwnd, ID_CB2 ), BM_SETCHECK, yn, 0 );
		
		// スキャンライン輝度
		st = ecfg->GetScanLineBr();
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_SCANLINEBR, str );
		
		// 4:3表示
		yn = ecfg->GetDispNTSC();
		SendMessage( GetDlgItem( hwnd, ID_CB7 ), BM_SETCHECK, yn, 0 );
		
		// フルスクリーン
		yn = ecfg->GetFullScreen();
		SendMessage( GetDlgItem( hwnd, ID_CB10 ), BM_SETCHECK, yn, 0 );
		
		// ステータスバー表示状態
		yn = ecfg->GetDispStat();
		SendMessage( GetDlgItem( hwnd, ID_CB11 ), BM_SETCHECK, yn, 0 );
		
		// フレームスキップ
		st = ecfg->GetFrameSkip();
		SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_SETPOS, true, st );
		SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_SETRANGE, true, MAKELPARAM(0,5) );
		
		break;
		
	case PP_SOUND:	// サウンド
		// サンプリングレート
		st = ecfg->GetSampleRate();
		SendMessage( GetDlgItem( hwnd, ID_RB21 ), BM_SETCHECK, (st==44100)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB22 ), BM_SETCHECK, (st==22050)? BST_CHECKED:BST_UNCHECKED, 0 );
		SendMessage( GetDlgItem( hwnd, ID_RB23 ), BM_SETCHECK, (st==11025)? BST_CHECKED:BST_UNCHECKED, 0 );
		
		// バッファサイズ
		st = ecfg->GetSoundBuffer();
		SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_SETPOS, true, st );
		SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_SETRANGE, true, MAKELPARAM(1,5) );
		
		// PSG LPFカットオフ周波数
		st = ecfg->GetPsgLPF();
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_EDIT1, str );
		
		// マスター音量
		st = ecfg->GetMasterVol();
		SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_SETPOS, true, st/10 );
		SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_SETRANGE, true, MAKELPARAM(0,10) );
		
		// PSG音量
		st = ecfg->GetPsgVol();
		SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_SETPOS, true, st/10 );
		SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_SETRANGE, true, MAKELPARAM(0,10) );
		
		// 音声合成音量
		st = ecfg->GetVoiceVol();
		SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_SETPOS, true, st/10 );
		SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_SETRANGE, true, MAKELPARAM(0,10) );
		
		// TAPEモニタ音量
		st = ecfg->GetCmtVol();
		SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_SETPOS, true, st/10 );
		SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_SETTHUMBLENGTH, 13, 0 );
		SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_SETRANGE, true, MAKELPARAM(0,10) );
		
		break;
		
	case PP_INPUT:	// 入力関係
		// キーリピート間隔
		st = ecfg->GetKeyRepeat();
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_KEYREP, str );
		
		break;
		
	case PP_FOLDER:	// フォルダ
		// ROMパス
		strncpy( str, ecfg->GetRomPath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH1, str );
		
		// TAPEパス
		strncpy( str, ecfg->GetTapePath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH2, str );
		
		// DISKパス
		strncpy( str, ecfg->GetDiskPath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH3, str );
		
		// 拡張ROMパス
		strncpy( str, ecfg->GetExtRomPath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH4, str );
		
		// IMGパス
		strncpy( str, ecfg->GetImgPath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH5, str );
		
		// WAVEパス
		strncpy( str, ecfg->GetWavePath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH6, str );
		
		// どこでもSAVEパス
		strncpy( str, ecfg->GetDokoSavePath(), PATH_MAX );
		OSD_DelDelimiter( str );
		SetDlgItemText( hwnd, ID_PATH7, str );
		
		break;
		
	case PP_FILE:	// ファイル
		// 拡張ROMファイル
		strncpy( str, ecfg->GetExtRomFile(), PATH_MAX );
		SetDlgItemText( hwnd, ID_FEXROM, str );
		
		// TAPE(LOAD)ファイル名
		strncpy( str, ecfg->GetTapeFile(), PATH_MAX );
		SetDlgItemText( hwnd, ID_FTPLD, str );
		
		// TAPE(SAVE)ファイル名
		strncpy( str, ecfg->GetSaveFile(), PATH_MAX );
		SetDlgItemText( hwnd, ID_FTPSV, str );
		
		// DISK1ファイル名
		strncpy( str, ecfg->GetDiskFile( 1 ), PATH_MAX );
		SetDlgItemText( hwnd, ID_FDISK1, str );
		
		// DISK2ファイル名
		strncpy( str, ecfg->GetDiskFile( 2 ), PATH_MAX );
		SetDlgItemText( hwnd, ID_FDISK2, str );
		
		// プリンタファイル名
		strncpy( str, ecfg->GetPrinterFile(), PATH_MAX );
		SetDlgItemText( hwnd, ID_FPRINT, str );
		
		break;
		
	case PP_COL:	// 色1
		break;
		
	case PP_ETC:	// その他
		// オーバークロック率
		st = min( max( 1, ecfg->GetOverClock() ), 1000 );
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_OVERCLK, str );
		
		// CRCチェック
		yn = ecfg->GetCheckCRC();
		SendMessage( GetDlgItem( hwnd, ID_CB4 ), BM_SETCHECK, yn, 0 );
		
		// ROMパッチ
		yn = ecfg->GetRomPatch();
		SendMessage( GetDlgItem( hwnd, ID_CB9 ), BM_SETCHECK, yn, 0 );
		
		// Turbo TAPE
		yn = ecfg->GetTurboTAPE();
		SendMessage( GetDlgItem( hwnd, ID_CB3 ), BM_SETCHECK, yn, 0 );
		
		// Boost Up
		yn = ecfg->GetBoostUp();
		SendMessage( GetDlgItem( hwnd, ID_CB5 ), BM_SETCHECK, yn, 0 );
		
		// BoostUp 最大倍率(N60モード)
		st = min( max( 1, ecfg->GetMaxBoost1() ), 100 );
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_BOOST60, str );
		
		// BoostUp 最大倍率(N60m/N66モード)
		st = min( max( 1, ecfg->GetMaxBoost2() ), 100 );
		sprintf( str, "%d", st );
		SetDlgItemText( hwnd, ID_BOOST62, str );
		
		// 終了時 確認する
		yn = ecfg->GetCkQuit();
		SendMessage( GetDlgItem( hwnd, ID_CB8 ), BM_SETCHECK, yn, 0 );
		
		// 終了時 INIファイルを保存する
		yn = ecfg->GetSaveQuit();
		SendMessage( GetDlgItem( hwnd, ID_CB12 ), BM_SETCHECK, yn, 0 );
		
		break;
	}
	
	return true;
}


///////////////////////////////////////////////////////////
// 設定を保存する
///////////////////////////////////////////////////////////
static bool OsdWriteINI( HWND hwnd, int page )
{
	int st;					// 状態取得用
	char str[PATH_MAX];		// 文字列取得用
	
	if( !ecfg ) return false;
	
	switch( page ){
	case PP_BASE:	// 基本
		// 機種
		if     ( IsDlgButtonChecked( hwnd, ID_RB02 ) ) st = 61;	// PC-6001A
		else if( IsDlgButtonChecked( hwnd, ID_RB03 ) ) st = 62;	// PC-6001mk2
		else if( IsDlgButtonChecked( hwnd, ID_RB04 ) ) st = 66;	// PC-6601
		else if( IsDlgButtonChecked( hwnd, ID_RB05 ) ) st = 64;	// PC-6001mk2SR
		else if( IsDlgButtonChecked( hwnd, ID_RB06 ) ) st = 68;	// PC-6601SR
		else                                           st = 60;	// 上記以外ならPC-6001
		ecfg->SetModel( st );
		
		// FDD
		if     ( IsDlgButtonChecked( hwnd, ID_RB32 ) ) st = 1;		// 2台
		else if( IsDlgButtonChecked( hwnd, ID_RB33 ) ) st = 2;		// 1台
		else                                           st = 0;		// 上記以外ならなし
		ecfg->SetFddNum( st );
		
		// 拡張RAM使用
		ecfg->SetUseExtRam( IsDlgButtonChecked( hwnd, ID_CB1 ) );
		
		// 戦士のカートリッジ使用
		ecfg->SetUseSoldier( IsDlgButtonChecked( hwnd, ID_CB13 ) );
		
		break;
		
	case PP_DISP:	// 画面
		// MODE4カラー
		if     ( IsDlgButtonChecked( hwnd, ID_RB12 ) ) st = 1;	// 赤/青
		else if( IsDlgButtonChecked( hwnd, ID_RB13 ) ) st = 2;	// 青/赤
		else if( IsDlgButtonChecked( hwnd, ID_RB14 ) ) st = 3;	// ピンク/緑
		else if( IsDlgButtonChecked( hwnd, ID_RB15 ) ) st = 4;	// 緑/ピンク
		else                                           st = 0;	// 上記以外ならモノクロ
		ecfg->SetMode4Color( st );
		
		// ビデオキャプチャ色深度
		if     ( IsDlgButtonChecked( hwnd, ID_RB07 ) ) st = 16;	// 16bit
		else if( IsDlgButtonChecked( hwnd, ID_RB08 ) ) st = 24;	// 24bit
		else                                           st = 32;	// 上記以外なら32bit
		ecfg->SetAviBpp( st );
		
		// スキャンライン
		ecfg->SetScanLine( IsDlgButtonChecked( hwnd, ID_CB2 ) );
		
		// スキャンライン輝度
		GetDlgItemText( hwnd, ID_SCANLINEBR, str, sizeof(str) );
		st = strtol( str, NULL, 0 );
		ecfg->SetScanLineBr( st );
		
		// 4:3表示
		ecfg->SetDispNTSC( IsDlgButtonChecked( hwnd, ID_CB7 ) );
		
		// フルスクリーン
		ecfg->SetFullScreen( IsDlgButtonChecked( hwnd, ID_CB10 ) );
		
		// ステータスバー表示状態
		ecfg->SetDispStat( IsDlgButtonChecked( hwnd, ID_CB11 ) );
		
		// フレームスキップ
		st = SendMessage( GetDlgItem( hwnd, ID_TBFPS ), TBM_GETPOS, 0, 0 );
		ecfg->SetFrameSkip( st );
		
		break;
		
	case PP_SOUND:	// サウンド
		// サンプリングレート
		if     ( IsDlgButtonChecked( hwnd, ID_RB22 ) ) st = 22050;	// 22050
		else if( IsDlgButtonChecked( hwnd, ID_RB23 ) ) st = 11025;	// 11025
		else                                           st = 44100;	// 上記以外なら44100
		ecfg->SetSampleRate( st );
		
		// バッファサイズ
		st = SendMessage( GetDlgItem( hwnd, ID_TBBUF ), TBM_GETPOS, 0, 0 );
		ecfg->SetSoundBuffer( st );
		
		// PSG LPFカットオフ周波数
		GetDlgItemText( hwnd, ID_EDIT1, str, sizeof(str) );
		st = strtol( str, NULL, 0 );
		ecfg->SetPsgLPF( st );
		
		// マスター音量
		st = SendMessage( GetDlgItem( hwnd, ID_TBMST ), TBM_GETPOS, 0, 0 );
		ecfg->SetMasterVol( st*10 );
		
		// PSG音量
		st = SendMessage( GetDlgItem( hwnd, ID_TBPSG ), TBM_GETPOS, 0, 0 );
		ecfg->SetPsgVol( st*10 );
		
		// 音声合成音量
		st = SendMessage( GetDlgItem( hwnd, ID_TBVCE ), TBM_GETPOS, 0, 0 );
		ecfg->SetVoiceVol( st*10 );
		
		// TAPEモニタ音量
		st = SendMessage( GetDlgItem( hwnd, ID_TBTAPE ), TBM_GETPOS, 0, 0 );
		ecfg->SetCmtVol( st*10 );
		
		break;
		
	case PP_INPUT:	// 入力関係
		// キーリピート間隔
		GetDlgItemText( hwnd, ID_KEYREP, str, sizeof(str) );
		st = strtol( str, NULL, 0 );
		ecfg->SetKeyRepeat( st );
		
		break;
		
	case PP_FOLDER:	// フォルダ
		// ROMパス
		GetDlgItemText( hwnd, ID_PATH1, str, sizeof(str) );
		ecfg->SetRomPath( str );
		
		// TAPEパス
		GetDlgItemText( hwnd, ID_PATH2, str, sizeof(str) );
		ecfg->SetTapePath( str );
		
		// DISKパス
		GetDlgItemText( hwnd, ID_PATH3, str, sizeof(str) );
		ecfg->SetDiskPath( str );
		
		// 拡張ROMパス
		GetDlgItemText( hwnd, ID_PATH4, str, sizeof(str) );
		ecfg->SetExtRomPath( str );
		
		// IMGパス
		GetDlgItemText( hwnd, ID_PATH5, str, sizeof(str) );
		ecfg->SetImgPath( str );
		
		// WAVEパス
		GetDlgItemText( hwnd, ID_PATH6, str, sizeof(str) );
		ecfg->SetWavePath( str );
		
		// どこでもSAVEパス
		GetDlgItemText( hwnd, ID_PATH7, str, sizeof(str) );
		ecfg->SetDokoSavePath( str );
		
		break;
		
	case PP_FILE:	// ファイル
		// 拡張ROMファイル
		GetDlgItemText( hwnd, ID_FEXROM, str, sizeof(str) );
		ecfg->SetExtRomFile( str );
		
		// TAPE(LOAD)ファイル名
		GetDlgItemText( hwnd, ID_FTPLD, str, sizeof(str) );
		ecfg->SetTapeFile( str );
		
		// TAPE(SAVE)ファイル名
		GetDlgItemText( hwnd, ID_FTPSV, str, sizeof(str) );
		ecfg->SetSaveFile( str );
		
		// DISK1ファイル名
		GetDlgItemText( hwnd, ID_FDISK1, str, sizeof(str) );
		ecfg->SetDiskFile( 1, str );
		
		// DISK2ファイル名
		GetDlgItemText( hwnd, ID_FDISK2, str, sizeof(str) );
		ecfg->SetDiskFile( 2, str );
		
		// プリンタファイル名
		GetDlgItemText( hwnd, ID_FPRINT, str, sizeof(str) );
		ecfg->SetPrinterFile( str );
		
		break;
		
	case PP_COL:	// 色1
		break;
		
	case PP_ETC:	// その他
		// オーバークロック率
		GetDlgItemText( hwnd, ID_OVERCLK, str, sizeof(str) );
		st = min( max( 1, strtol( str, NULL, 0 ) ), 1000 );
		ecfg->SetOverClock( st );
		
		// CRCチェック
		ecfg->SetCheckCRC( IsDlgButtonChecked( hwnd, ID_CB4 ) );
		
		// ROMパッチ
		ecfg->SetRomPatch( IsDlgButtonChecked( hwnd, ID_CB9 ) );
		
		// Turbo TAPE
		ecfg->SetTurboTAPE( IsDlgButtonChecked( hwnd, ID_CB3 ) );
		
		// Boost Up
		ecfg->SetBoostUp( IsDlgButtonChecked( hwnd, ID_CB5 ) );
		
		// BoostUp 最大倍率(N60モード)
		GetDlgItemText( hwnd, ID_BOOST60, str, sizeof(str) );
		st = min( max( 1, strtol( str, NULL, 0 ) ), 100 );
		ecfg->SetMaxBoost1( st );
		
		// BoostUp 最大倍率(N60m/N66モード)
		GetDlgItemText( hwnd, ID_BOOST62, str, sizeof(str) );
		st = min( max( 1, strtol( str, NULL, 0 ) ), 100 );
		ecfg->SetMaxBoost2( st );
		
		// 終了時 確認する
		ecfg->SetCkQuit( IsDlgButtonChecked( hwnd, ID_CB8 ) );
		
		// 終了時 INIファイルを保存する
		ecfg->SetSaveQuit( IsDlgButtonChecked( hwnd, ID_CB12 ) );
		
		break;
	}
	
	return true;
}


///////////////////////////////////////////////////////////
// 環境設定ダイアログプロシージャ
///////////////////////////////////////////////////////////
// 基本
static bool CALLBACK OsdCnfgProc1( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// 設定を読込む
		if( !OsdReadINI( hwnd, PP_BASE ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_COMMAND:
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:	// OKボタン処理
			// 設定を保存する
			if( !OsdWriteINI( hwnd, PP_BASE ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// 画面
static bool CALLBACK OsdCnfgProc2( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// 設定を読込む
		if( !OsdReadINI( hwnd, PP_DISP ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_COMMAND:
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// 設定を保存する
			if( !OsdWriteINI( hwnd, PP_DISP ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// サウンド
static bool CALLBACK OsdCnfgProc3( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// 設定を読込む
		if( !OsdReadINI( hwnd, PP_SOUND ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// 設定を保存する
			if( !OsdWriteINI( hwnd, PP_SOUND ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// ファイル
static bool CALLBACK OsdCnfgProc4( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// 設定を読込む
		if( !OsdReadINI( hwnd, PP_FILE ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_COMMAND:
		char Folder[PATH_MAX];
		switch( wp ){
		case ID_B11:	// ファイル参照ボタン(拡張ROMイメージ)
			GetDlgItemText( hwnd, ID_FEXROM, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_ExtRom, Folder, ecfg->GetExtRomPath() ) ){
				SetDlgItemText( hwnd, ID_FEXROM, Folder );
			}
			break;
			
		case ID_B12:	// ファイル参照ボタン(TAPEイメージ(LOAD))
			GetDlgItemText( hwnd, ID_FTPLD, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_TapeLoad, Folder, ecfg->GetTapePath() ) ){
				SetDlgItemText( hwnd, ID_FTPLD, Folder );
			}
			break;
			
		case ID_B13:	// ファイル参照ボタン(TAPEイメージ(SAVE))
			GetDlgItemText( hwnd, ID_FTPSV, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_TapeSave, Folder, ecfg->GetTapePath() ) ){
				SetDlgItemText( hwnd, ID_FTPSV, Folder );
			}
			break;
			
		case ID_B14:	// ファイル参照ボタン(DISK1イメージ)
			GetDlgItemText( hwnd, ID_FDISK1, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_Disk, Folder, ecfg->GetDiskPath() ) ){
				SetDlgItemText( hwnd, ID_FDISK1, Folder );
			}
			break;
			
		case ID_B15:	// ファイル参照ボタン(DISK2イメージ)
			GetDlgItemText( hwnd, ID_FDISK2, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_Disk, Folder, ecfg->GetDiskPath() ) ){
				SetDlgItemText( hwnd, ID_FDISK2, Folder );
			}
			break;
			
		case ID_B16:	// ファイル参照ボタン(プリンタ出力ファイル)
			GetDlgItemText( hwnd, ID_FPRINT, Folder, sizeof(Folder) );
			if( OSD_FileSelect( hwnd, FD_Printer, Folder, NULL ) ){
				SetDlgItemText( hwnd, ID_FPRINT, Folder );
			}
			break;
			
		case ID_B11E:	// EJECTボタン(拡張ROMイメージ)
			SetDlgItemText( hwnd, ID_FEXROM, "" );
			break;
			
		case ID_B12E:	// EJECTボタン(TAPEイメージ(LOAD))
			SetDlgItemText( hwnd, ID_FTPLD, "" );
			break;
			
		case ID_B13E:	// EJECTボタン(TAPEイメージ(SAVE))
			SetDlgItemText( hwnd, ID_FTPSV, "" );
			break;
			
		case ID_B14E:	// EJECTボタン(DISK1イメージ)
			SetDlgItemText( hwnd, ID_FDISK1, "" );
			break;
			
		case ID_B15E:	// EJECTボタン(DISK2イメージ)
			SetDlgItemText( hwnd, ID_FDISK2, "" );
			break;
			
		case ID_B16E:	// EJECTボタン(プリンタ出力ファイル)
			SetDlgItemText( hwnd, ID_FPRINT, "" );
			break;
		}
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// 設定を保存する
			if( !OsdWriteINI( hwnd, PP_FILE ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// フォルダ
static bool CALLBACK OsdCnfgProc5( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// 設定を読込む
		if( !OsdReadINI( hwnd, PP_FOLDER ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_COMMAND:
		char Folder[PATH_MAX];
		switch( wp ){
		case ID_B01:	// フォルダ参照ボタン(ROM)
			GetDlgItemText( hwnd, ID_PATH1, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH1, Folder );
			break;
			
		case ID_B02:	// フォルダ参照ボタン(TAPE)
			GetDlgItemText( hwnd, ID_PATH2, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH2, Folder );
			break;
			
		case ID_B03:	// フォルダ参照ボタン(DISK)
			GetDlgItemText( hwnd, ID_PATH3, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH3, Folder );
			break;
			
		case ID_B04:	// フォルダ参照ボタン(拡張ROM)
			GetDlgItemText( hwnd, ID_PATH4, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH4, Folder );
			break;
			
		case ID_B05:	// フォルダ参照ボタン(IMG)
			GetDlgItemText( hwnd, ID_PATH5, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH5, Folder );
			break;
			
		case ID_B06:	// フォルダ参照ボタン(WAVE)
			GetDlgItemText( hwnd, ID_PATH6, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH6, Folder );
			break;
			
		case ID_B07:	// フォルダ参照ボタン(どこでもSAVE)
			GetDlgItemText( hwnd, ID_PATH7, Folder, sizeof(Folder) );
			if( OSD_FolderDiaog( hwnd, Folder ) ) SetDlgItemText( hwnd, ID_PATH7, Folder );
			break;
		}
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// 設定を保存する
			if( !OsdWriteINI( hwnd, PP_FOLDER ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// 色1
static bool CALLBACK OsdCnfgProcCol( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	static COLORREF Color = 0;
	static COLORREF CustColors[16] = { RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
									   RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
									   RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ),
									   RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ), RGB( 255, 255, 255 ) };
	static CHOOSECOLOR cc;	// 色の選択用
	COLOR24 col;
	
	switch( msg ){
	case WM_INITDIALOG:
		// 設定を読込む
		if( !OsdReadINI( hwnd, PP_COL ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		
		// 色の選択 コモンダイアログ 初期化
		cc.lStructSize  = sizeof(CHOOSECOLOR);
		cc.hwndOwner    = hwnd;
		cc.rgbResult    = Color;
		cc.lpCustColors = CustColors;
		cc.Flags        = CC_FULLOPEN | CC_RGBINIT;
		
		break;
		
	case WM_DRAWITEM:	// オーナードロー
		if( wp >= ID_COL16 && wp <= ID_COL80 && ecfg->GetColor( wp-ID_COL16+16, &col ) ){
			// オーナードローボタン 描画
			OsdOwnerDrawBtn( ((LPDRAWITEMSTRUCT)lp)->hDC,
							 ((LPDRAWITEMSTRUCT)lp)->rcItem,
							 RGB( col.r, col.g, col.b ) );
			return true;
		}
		return false;
		
	case WM_COMMAND:
		if( wp >= ID_COL16 && wp <= ID_COL80 && ecfg->GetColor( wp-ID_COL16+16, &col ) ){
			Color = RGB( col.r, col.g, col.b );
			// 色の選択 コモンダイアログ表示
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
			// 設定を保存する
			if( !OsdWriteINI( hwnd, PP_COL ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// その他
static bool CALLBACK OsdCnfgProcEtc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// 設定を読込む
		if( !OsdReadINI( hwnd, PP_ETC ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// 設定を保存する
			if( !OsdWriteINI( hwnd, PP_ETC ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}

// 入力関係
static bool CALLBACK OsdCnfgProcIn( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	switch( msg ){
	case WM_INITDIALOG:
		// 設定を読込む
		if( !OsdReadINI( hwnd, PP_INPUT ) ) OSD_Message( MSERR_IniReadFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
		break;
		
	case WM_NOTIFY:
		switch( ((NMHDR *)lp)->code ){
		case PSN_APPLY:
			// 設定を保存する
			if( !OsdWriteINI( hwnd, PP_INPUT ) ) OSD_Message( MSERR_IniWriteFailed, MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			return true;
		}
	}
	return false;
}


// オーナードローボタン 描画
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
// バージョン情報表示
//
// 引数:	hwnd		ウィンドウハンドル
//			mdl			機種
// 返値:	なし
///////////////////////////////////////////////////////////
void OSD_VersionDialog( HWINDOW hwnd, int mdl )
{
	model = mdl;
	HWND hhwnd = (HWND)OSD_GetWindowHandle( hwnd );
	DialogBox( (HINSTANCE)GetWindowLongPtr( hhwnd, GWLP_HINSTANCE), MAKEINTRESOURCE(ID_VER), hhwnd, (DLGPROC)VerInfoProc );
}


///////////////////////////////////////////////////////////
// バージョン情報ダイアログプロシージャ
///////////////////////////////////////////////////////////
static bool CALLBACK VerInfoProc( HWND hwnd, UINT msg, WPARAM wp, LPARAM lp )
{
	static HCURSOR hCursor = NULL;
	
	switch( msg ){
	case WM_INITDIALOG:
		{
		// バージョン文字列設定
		SetDlgItemText( hwnd, ID_VERSTR, APPNAME " Ver." VERSION );
		
		// アイコン設定
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
		
		// 指型カーソル取得
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
	case WM_SETCURSOR:	// カーソル変更
		if( (HWND)wp == GetDlgItem( hwnd, ID_URL ) ){
			SetCursor( hCursor );
			GetWindowLongPtr( hwnd, DWLP_MSGRESULT );
			return true;
		}
		return false;
		break;
		
	case WM_CTLCOLORSTATIC:	// URLの色を変える
		{
		HDC hDC = (HDC)wp;
		if( (HWND)lp == GetDlgItem( hwnd, ID_URL ) ){
			SetBkMode( hDC, TRANSPARENT );
			SetTextColor( hDC, RGB(0,0,255) );	// 色を変える
			return GetStockObject( NULL_BRUSH );
		}
		return ( COLOR_BTNFACE + 1 ) ? true : false;
		}
		break;
		
	case WM_COMMAND:
    	switch( LOWORD(wp) ){
		case IDOK:
		case IDCANCEL:
			if( hCursor ) DestroyCursor( hCursor );	// カーソル開放
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
