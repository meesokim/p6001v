#include <new>

#include "pc6001v.h"
#include "id_menu.h"

#include "breakpoint.h"
#include "common.h"
#include "config.h"
#include "cpum.h"
#include "cpus.h"
#include "debug.h"
#include "disk.h"
#include "error.h"
#include "graph.h"
#include "intr.h"
#include "joystick.h"
#include "keyboard.h"
#include "log.h"
#include "memory.h"
#include "osd.h"
#include "pio.h"
#include "psgfm.h"
#include "schedule.h"
#include "sound.h"
#include "status.h"
#include "tape.h"
#include "vdg.h"
#include "voice.h"
#include "vsurface.h"

#include "p6el.h"


#define	FRAMERATE	(VSYNC_HZ/(cfg->GetFrameSkip()+1))

int EL6::Speed = 100;

const char resext[] = "resume";	// ���v���C�r���ۑ��p�g���q


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
EL6::EL6( void ) : vm(NULL), cfg(NULL), sche(NULL), graph(NULL), snd(NULL), joy(NULL), staw(NULL),
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	regw(NULL), memw(NULL), monw(NULL),
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	UpDateFPSID(NULL), UDFPSCount(0), FSkipCount(0)
{}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
EL6::~EL6( void )
{
	DeleteAllObject();
}



////////////////////////////////////////////////////////////////
// �X���b�h�֐�(�I�[�o�[���C�h)
////////////////////////////////////////////////////////////////
void EL6::OnThread( void *inst )
{
	EL6 *p6;
	
	p6 = STATIC_CAST( EL6 *, inst );	// �������g�̃I�u�W�F�N�g�|�C���^�擾
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( p6->cfg->GetMonDisp() ){
	// ���j�^���[�h
		while( !this->cThread::IsCancel() ){
			// ��ʍX�V
			if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
			p6->Wait();		// �E�F�C�g
		}
	}else
	// �ʏ탂�[�h
		if( p6->vm->BpExistBreakPoint() ){
			// �u���[�N�|�C���g����
			while( !this->cThread::IsCancel() ){
				// �|�[�Y���Ȃ牽�����Ȃ�
				if( p6->sche->GetPauseEnable() )
					// �E�F�C�g
					p6->Wait();
				else{
					p6->Emu();		// 1���ߎ��s
					
					if( p6->vm->evsc->IsVSYNC() ){
						p6->vm->key->ScanMatrix();	// �L�[�}�g���N�X�X�L����
						
						// �T�E���h�X�V
						p6->SoundUpdate( 0 );
						// ��ʍX�V
						if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
						
						// �����L�[����
						if( IsAutoKey() ){
							BYTE key = GetAutoKey();
							if( key ){
								if( key == 0x14 ) p6->vm->cpus->ReqKeyIntr( 6, GetAutoKey() );
								else			  p6->vm->cpus->ReqKeyIntr( 0, key );
							}
						}
						
						// �E�F�C�g
						p6->Wait();
					}
				}
				
				// �u���[�N�|�C���g�`�F�b�N
				if( p6->vm->BpCheckBreakPoint( BPoint::BP_PC, p6->vm->cpum->GetPC(), 0, NULL ) || p6->vm->BpIsReqBreak() ){
					this->cThread::Cancel();	// �X���b�h�I���t���O���Ă�
					p6->vm->BpResetBreak();
					OSD_PushEvent( EV_DEBUGMODEBP, p6->vm->cpum->GetPC() );
				}
			}
		}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{	// �ʏ���s
		while( !this->cThread::IsCancel() ){
			// �|�[�Y���Ȃ��ʍX�V�̂�
			if( p6->sche->GetPauseEnable() ){
				// ��ʍX�V�������}���Ă������ʍX�V
				// �m�[�E�F�C�g�̎���FPS���ς��Ȃ��悤�ɂ���
				if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
			}else{
				// �L�[�}�g���N�X�X�L����
				bool matchg = p6->vm->key->ScanMatrix();
				
				// ���v���C�L�^��
				if( REPLAY::GetStatus() == REP_RECORD )
					REPLAY::ReplayWriteFrame( p6->vm->key->GetMatrix2(), matchg );
				
				// ���v���C�Đ���
				if( REPLAY::GetStatus() == REP_REPLAY ){
					REPLAY::ReplayReadFrame( p6->vm->key->GetMatrix() );
				}
				
				p6->EmuVSYNC();			// 1��ʕ����s
				
				// �r�f�I�L���v�`����?
				if( AVI6::IsAVI() ){
					// �r�f�I�L���v�`�����Ȃ炱����AVI�ۑ�����
					// �T�E���h�X�V
					p6->SoundUpdate( 0, AVI6::GetAudioBuffer() );
					// ��ʍX�V���ꂽ�� AVI1��ʕۑ�
					if( p6->ScreenUpdate() ) AVI6::AVIWriteFrame( p6->graph->GetWindowHandle() );
				}else{
					// �r�f�I�L���v�`�����łȂ��Ȃ�ʏ�̍X�V
					// �T�E���h�X�V
					p6->SoundUpdate( 0 );
					// ��ʍX�V�������}���Ă������ʍX�V
					// �m�[�E�F�C�g�̎���FPS���ς��Ȃ��悤�ɂ���
					if( p6->sche->IsScreenUpdate() ) p6->ScreenUpdate();
				}
				
				// �����L�[����
				if( IsAutoKey() ){
					BYTE key = GetAutoKey();
					if( key ){
						if( key == 0x14 ) p6->vm->cpus->ReqKeyIntr( 6, GetAutoKey() );
						else			  p6->vm->cpus->ReqKeyIntr( 0, key );
					}
				}
			}
			
			// �E�F�C�g
			p6->Wait();
		}
	}
}


////////////////////////////////////////////////////////////////
// Wait
////////////////////////////////////////////////////////////////
void EL6::Wait( void )
{
	if( sche->GetWaitEnable() && (!cfg->GetTurboTAPE() || (vm->cpus->GetCmtStatus() == CMTCLOSE)) )
		sche->VWait();
	vm->evsc->ReVSYNC();
}


////////////////////////////////////////////////////////////////
// 1���ߎ��s
////////////////////////////////////////////////////////////////
int EL6::Emu( void )
{
	int st = vm->Emu();			// VM 1���ߎ��s
	vm->evsc->Update( st );		// �C�x���g�X�V
	sche->Update( st );
	
	return st;
}


////////////////////////////////////////////////////////////////
// 1��ʕ����s
////////////////////////////////////////////////////////////////
int EL6::EmuVSYNC( void )
{
	int state = 0;
	
	// VSYNC����������܂ŌJ�Ԃ�
	while( !vm->evsc->IsVSYNC() ){
		int st = vm->Emu();		// VM 1���ߎ��s
		vm->evsc->Update( st );	// �C�x���g�X�V
		sche->Update( st );
		state += st;
	}
	
	return state;
}


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
////////////////////////////////////////////////////////////////
// �w��X�e�[�g�����s
////////////////////////////////////////////////////////////////
void EL6::Exec( int st )
{
	int State = st;
	
	while( State > 0 ) State -= Emu();
}


////////////////////////////////////////////////////////////////
// ���j�^���[�h�ؑւ�
////////////////////////////////////////////////////////////////
bool EL6::ToggleMonitor( void )
{
	// VM��~
	Stop();
	
	// ���j�^�E�B���h�E�\����Ԑ؊���
	cfg->SetMonDisp( cfg->GetMonDisp() ? false : true );
	
	// �u���[�N�|�C���g�̏����N���A
	vm->BpClearStatus();
	
	// �X�N���[���T�C�Y�ύX
	graph->ResizeScreen();
	
	// VM�ĊJ
	Start();
	
	return cfg->GetMonDisp();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool EL6::Init( const CFG6 *config )
{
	// �G���[���b�Z�[�W�����l
	Error::SetError( Error::InitFailed );
	
	if( !config ) return false;
	cfg = (CFG6 *)config;
	
	// �S�I�u�W�F�N�g�폜
	DeleteAllObject();
	
	// �p���b�g
	GPal.colors = new COLOR24[256];
	if( !GPal.colors ) return false;
	GPal.ncols = 256;
	
	for( int i=0; i<128; i++ ){
		cfg->GetColor( i, &GPal.colors[i] );
		VSurface::SetColor( i, COL2DW( GPal.colors[i] ) );
	}
	
	// �@��� VM�m��
	switch( cfg->GetModel() ){
	case 61: vm = new VM61( this ); break;
	case 62: vm = new VM62( this ); break;
	case 66: vm = new VM66( this ); break;
	case 64: vm = new VM64( this ); break;
	case 68: vm = new VM68( this ); break;
	default: vm = new VM60( this );
	}
	// VM������
	if( !vm || !vm->AllocObject( cfg ) || !vm->Init( cfg ) ) return false;
	
	// �e��I�u�W�F�N�g�m��
	try{
		sche   = new SCH6( vm->GetCPUClock() * cfg->GetOverClock() / 100 );	// �X�P�W���[��
		snd    = new SND6;											// �T�E���h
		graph  = new DSP6( vm );									// ��ʕ`��
		joy    = new JOY6;											// �W���C�X�e�B�b�N
		staw   = new cWndStat( vm );								// �X�e�[�^�X�o�[
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		regw   = new cWndReg( vm, DEV_ID("REGW") );					// ���W�X�^�E�B���h�E
		memw   = new cWndMem( vm, DEV_ID("MEMW") );					// �������E�B���h�E
		monw   = new cWndMon( vm, DEV_ID("MONW") );					// ���j�^�E�B���h�E
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	}
	// new �Ɏ��s�����ꍇ
	catch( std::bad_alloc ){
		if( sche ) { delete sche;  sche  = NULL; }
		if( snd )  { delete snd;   snd   = NULL; }
		if( graph ){ delete graph; graph = NULL; }
		if( staw ) { delete staw;  staw  = NULL; }
		if( joy )  { delete joy;   joy   = NULL; }
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		if( regw ) { delete regw;  regw  = NULL; }
		if( memw ) { delete memw;  memw  = NULL; }
		if( monw ) { delete monw;  monw  = NULL; }
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		
		return false;
	}
	
	// �T�E���h -----
	if( !snd->Init( this, EL6::StreamUpdate, cfg->GetSampleRate(), cfg->GetSoundBuffer() ) ) return false;
	snd->SetVolume( cfg->GetMasterVol() );
	
	// ��ʕ`�� -----
	if( !graph->Init() ) return false;
	graph->SetIcon( cfg->GetModel() );	// �A�C�R���ݒ�
	
	// �W���C�X�e�B�b�N -----
	if( !joy->Init() ) return false;
	
	// �X�e�[�^�X�o�[ -----
	if( !staw->Init( graph->ScreenX(), cfg->GetFddNum() ) ) return false;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// ���W�X�^�E�B���h�E�@-----
	if( !regw->Init() ) return false;
	
	// �������E�B���h�E�@-----
	if( !memw->Init() ) return false;
	
	// ���j�^�E�B���h�E�@-----
	if( !monw->Init() ) return false;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// FPS�\���^�C�}�ݒ�
	if( !SetFPSTimer( 1 ) ){
		Stop();
		return false;
	}
	
	// �r�f�I�L���v�`�� -----
	if( !AVI6::Init() ) return false;
	
	// ���v���C -----
	if( !REPLAY::Init( vm->key->GetMatrixSize() ) ) return false;
	
	// �X�N���[���T�C�Y�ύX
	graph->ResizeScreen();
	
	// �X�g���[���ڑ�
	snd->ConnectStream( vm->psg );						// PSG
	snd->ConnectStream( vm->cmtl );						// CMT(LOAD)
	if( vm->voice ) snd->ConnectStream( vm->voice );	// ��������
	
	
	// TAPE�}��
	if( *(cfg->GetTapeFile()) ) TapeMount( cfg->GetTapeFile() );
	// �h���C�u1,2��DISK�}��
	if( *(cfg->GetDiskFile( 1 )) ) DiskMount( 0, cfg->GetDiskFile( 1 ) );
	if( *(cfg->GetDiskFile( 2 )) ) DiskMount( 1, cfg->GetDiskFile( 2 ) );
	
	
	UI_Reset();
	
	
	// �G���[�Ȃ�
	Error::Reset();
	
	return true;
}


////////////////////////////////////////////////////////////////
// ����J�n
////////////////////////////////////////////////////////////////
bool EL6::Start( void )
{
	// ���s���x�𕜌�
	while( sche->GetSpeedRatio() != Speed ){
		sche->SetSpeedRatio( Speed > 100 ? 1 : -1 );
	}
	
	FSkipCount = 0;
	
	// �X���b�h����
	if( !this->cThread::BeginThread( this ) ) return false;
	
	sche->Start();
	snd->Play();
	
	return true;
}


////////////////////////////////////////////////////////////////
// �����~
////////////////////////////////////////////////////////////////
void EL6::Stop( void )
{
	// ���s���x��ޔ�
	Speed = sche->GetSpeedRatio();
	
	if( !this->cThread::IsCancel() ){
		this->cThread::Cancel();	// �X���b�h�I���t���O���Ă�
		this->cThread::Waiting();	// �X���b�h�I���܂ő҂�
	}
	snd->Pause();
	sche->Stop();
}


////////////////////////////////////////////////////////////////
// �C�x���g���[�v
////////////////////////////////////////////////////////////////
EL6::ReturnCode EL6::EventLoop( void )
{
	Event event;
	
	while( OSD_GetEvent( &event ) ){
		switch( event.type ){
		case EV_FPSUPDATE:		// FPS�\��
			{
			char str[256];
			if( sche->GetPauseEnable() )
				sprintf( str, "%s === PAUSE ===", cfg->GetCaption() );
			else
				sprintf( str, "%s (%3d%%  %2d/%2d fps)", cfg->GetCaption(), sche->GetRatio(), event.fps.fps, FRAMERATE );
				if( sche->GetSpeedRatio() != 100 )
					sprintf( str+strlen(str), " [x%3.1f]", (double)sche->GetSpeedRatio()/100 );
					
			OSD_SetWindowCaption( graph->GetWindowHandle(), str );
			}
			break;
			
		case EV_KEYDOWN:		// �L�[����
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			// ���j�^���[�h?
			if( cfg->GetMonDisp() ){
				monw->KeyIn( event.key.sym, event.key.mod & KVM_SHIFT, event.key.unicode );
				break;
			}else
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			// �e��@�\�L�[�`�F�b�N
			if( CheckFuncKey( event.key.sym,
							  event.key.mod & KVM_ALT  ? true : false,
							  event.key.mod & KVM_META ? true : false ) )
				break;
			// ���v���C�Đ��� or �����L�[���͎��s���łȂ����
			if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() )
				// �L�[�}�g���N�X�X�V(�L�[)
				vm->key->UpdateMatrixKey( event.key.sym, true );
			break;
			
		case EV_KEYUP:
			// ���v���C�Đ��� or �����L�[���͎��s���łȂ����
			if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() )
				// �L�[�}�g���N�X�X�V(�L�[)
				vm->key->UpdateMatrixKey( event.key.sym, false );
			break;
			
		case EV_JOYAXISMOTION:
		case EV_JOYBUTTONDOWN:
		case EV_JOYBUTTONUP:
			// ���v���C�Đ��� or �����L�[���͎��s���łȂ����
			if( REPLAY::GetStatus() != REP_REPLAY && !IsAutoKey() )
				// �L�[�}�g���N�X�X�V(�W���C�X�e�B�b�N)
				vm->key->UpdateMatrixJoy( joy->GetJoyState( 0 ), joy->GetJoyState( 1 ) );
			break;
			
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		case EV_DEBUGMODEBP:	// ���j�^���[�h�ύX(�u���[�N�|�C���g���B��)
			monw->BreakIn( event.bp.addr );		// �u���[�N�|�C���g�̏���\��
			ToggleMonitor();					// ���j�^���[�h�ύX
			
			break;
			
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		case EV_MOUSEBUTTONUP:	// �}�E�X�{�^���N���b�N(��������)
			if( event.mousebt.state != false ) break;
			
			if( event.mousebt.button == MBT_WHEELUP )
				// �X�s�[�h�A�b�v
				sche->SetSpeedRatio( 1 );
			
			if( event.mousebt.button == MBT_WHEELDOWN )
				// �X�s�[�h�_�E��
				sche->SetSpeedRatio( -1 );
			
			if( event.mousebt.button == MBT_LEFT )
				// ����
				sche->SetSpeedRatio( 0 );
			
//			if( event.mousebt.button == MBT_MIDDLE ){}
			
			if( event.mousebt.button == MBT_RIGHT ){
				// �|�b�v�A�b�v���j���[�\��
				Stop();
				ShowPopupMenu( event.mousebt.x, event.mousebt.y );
				Start();
			}
			break;
			
		case EV_QUIT:			// �I��
			if( cfg->GetCkQuit() )
				if( OSD_Message( MSG_QUIT, MSG_QUITC, OSDM_YESNO | OSDM_ICONQUESTION ) != OSDR_YES )
					break;
			return Quit;
			
		case EV_RESTART:		// �ċN��
			return Restart;
			
		case EV_DOKOLOAD:		// �ǂ��ł�LOAD
			return Dokoload;
			
		case EV_REPLAY:			// ���v���C�Đ�
			return Replay;
			
		case EV_DROPFILE:		// Drag & Drop
			if( !stricmp( "p6",  OSD_GetFileNameExt( event.drop.file ) ) ||
				!stricmp( "cas", OSD_GetFileNameExt( event.drop.file ) ) ||
				!stricmp( "p6t", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_TapeInsert( event.drop.file );
			}else if( !stricmp( "d88", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_DiskInsert( 0, event.drop.file );
			}else if( !stricmp( "rom",  OSD_GetFileNameExt( event.drop.file ) ) ||
				!stricmp( "bin", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_RomInsert( event.drop.file );
			}else if( !stricmp( "dds", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_DokoLoad( event.drop.file );
			}else if( !stricmp( "ddr", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_ReplayLoad( event.drop.file );
			}else if( !stricmp( "bas",  OSD_GetFileNameExt( event.drop.file ) ) ||
				!stricmp( "txt", OSD_GetFileNameExt( event.drop.file ) ) ){
				UI_AutoType( event.drop.file );
			}
			
			// �t�@�C�������J��
			delete [] event.drop.file;
			break;
			
		default:
			break;
		}
		
		// �G���[����
		switch( Error::GetError() ){
		case Error::NoError:
			break;
			
		default:
			OSD_Message( (char *)Error::GetErrorText(), MSERR_ERROR, OSDR_OK | OSDM_ICONERROR );
			Error::Reset();
		}
	}
	return Quit;
}


////////////////////////////////////////////////////////////////
// �e��@�\�L�[�`�F�b�N
////////////////////////////////////////////////////////////////
bool EL6::CheckFuncKey( int kcode, bool OnALT, bool OnMETA )
{
	switch( kcode ){	// �L�[�R�[�h
	case KVC_F6:		// ���j�^���[�h�ύX or �X�N���[�����[�h�ύX
		// �r�f�I�L���v�`�����͖���
		if( AVI6::IsAVI() ) return false;
		
		if( OnALT ){
			Stop();
			cfg->SetFullScreen( cfg->GetFullScreen() ? false : true );
			graph->ResizeScreen();	// �X�N���[���T�C�Y�ύX
			Start();
		}else{
			#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			ToggleMonitor();
			#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		}
		break;
		
	case KVC_F7:			// �X�L�������C�����[�h�ύX
		// �r�f�I�L���v�`�����͖���
		if( AVI6::IsAVI() ) return false;
		
		Stop();
		if( OnALT ){
			cfg->SetDispNTSC( cfg->GetDispNTSC() ? false : true );
		}else{
			cfg->SetScanLine( cfg->GetScanLine() ? false : true );
		}
		graph->ResizeScreen();	// �X�N���[���T�C�Y�ύX
		Start();
		break;
		
	case KVC_F8:			// ���[�h4�J���[�ύX or �X�e�[�^�X�o�[�\����ԕύX
		if( OnALT ){
			Stop();
			cfg->SetDispStat( cfg->GetDispStat() ? false : true );
			graph->ResizeScreen();	// �X�N���[���T�C�Y�ύX
			Start();
		}else{
			int c = vm->vdg->GetMode4Color();
			if( ++c > 4 ) c = 0;
			vm->vdg->SetMode4Color( c );
		}
		break;
		
	case KVC_F9:			// �|�[�Y�L�������ύX
		if( OnALT ){
		}else{
			sche->SetPauseEnable( sche->GetPauseEnable() ? false : true );
		}
		break;
		
	case KVC_F10:			// Wait�L�������ύX
		if( OnALT ){
		}else{
			sche->SetWaitEnable( sche->GetWaitEnable() ? false : true );
		}
		break;
		
	case KVC_F11:			// ���Z�b�g or �ċN��
		if( OnALT ){
			OSD_PushEvent( EV_RESTART );
		}else{
			UI_Reset();
		}
		break;
		
	case KVC_F12:			// �X�i�b�v�V���b�g
		if( OnALT ){
		}else{
			graph->SnapShot( cfg->GetImgPath() );
		}
		break;
		
	case KVX_MENU:			// �|�b�v�A�b�v���j���[�\��
		Stop();
		ShowPopupMenu( 0, 0 );
		Start();
		break;
		
/*
	case KVC_MUHENKAN:      // �ǂ��ł�SAVE
		Stop();
		if( REPLAY::GetStatus() == REP_RECORD ){
			UI_ReplayDokoSave();
		}else{
			char str[PATH_MAX];
			snprintf( str, PATH_MAX, "%s/.1.dds", cfg->GetDokoSavePath() );
			DokoDemoSave( str );
			
			cIni save;
			if( save.Init( str ) ){
				// ��U�L�[���͂𖳌�������(LOAD���ɃL�[���������ςȂ��ɂȂ�̂�h������)
				save.PutEntry( "KEY", NULL, "P6Matrix", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
				save.PutEntry( "KEY", NULL, "P6Mtrx",   "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
			}
		}
		Start();
		break;
		
	case KVC_HENKAN:      // �ǂ��ł�LOAD
		Stop();
		if( REPLAY::GetStatus() == REP_RECORD ){
			UI_ReplayDokoLoad();
		} else {
			char str[PATH_MAX];
			snprintf( str, PATH_MAX, "%s/.1.dds", cfg->GetDokoSavePath() );
			if( OSD_FileExist( str ) ){
				cfg->SetModel( GetDokoModel( str ) );
				cfg->SetDokoFile( str );
				OSD_PushEvent( EV_DOKOLOAD );
			}
		}
		Start();
		break;
*/
		
	default:				// �ǂ�ł��Ȃ�
		return false;
	}
	return true;
}




////////////////////////////////////////////////////////////////
// �S�I�u�W�F�N�g�폜
////////////////////////////////////////////////////////////////
void EL6::DeleteAllObject( void )
{
	if( UpDateFPSID ) OSD_DelTimer( UpDateFPSID );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( monw ) { delete monw;	monw = NULL; }
	if( memw ) { delete memw;	memw = NULL; }
	if( regw ) { delete regw;	regw = NULL; }
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	if( staw ) { delete staw;	staw  = NULL; }
	if( joy )  { delete joy;	joy   = NULL; }
	if( snd )  { delete snd;	snd   = NULL; }
	if( graph ){ delete graph;	graph = NULL; }
	if( sche ) { delete sche;	sche  = NULL; }
	if( vm )   { delete vm;		vm    = NULL; }
	
	// �����L�[���̓o�b�t�@
	if( ak.Buffer ){   delete [] ak.Buffer;		ak.Buffer = NULL; }
	
	// �p���b�g
	if( GPal.colors ){ delete [] GPal.colors;	GPal.colors = NULL; }
}


////////////////////////////////////////////////////////////////
// ��ʍX�V
////////////////////////////////////////////////////////////////
bool EL6::ScreenUpdate( void )
{
	// �t���[���X�L�b�v�`�F�b�N
	if( ++FSkipCount > cfg->GetFrameSkip() ){
		// �X�e�[�^�X�o�[�X�V
		staw->SetReplayStatus( REPLAY::GetStatus() );	// ���v���C�X�e�[�^�X
		vm->vdg->UpdateBackBuf();	// �o�b�N�o�b�t�@�X�V
		graph->DrawScreen();		// ��ʕ`��
		
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		if( cfg->GetMonDisp() ){
			regw->Update();
			memw->Update();
			monw->Update();
		}
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		
		UDFPSCount++;
		FSkipCount = 0;
		
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// �T�E���h�X�V
//
// ����:	samples		�X�V����T���v����(-1:�c��o�b�t�@�S�� 0:�����N���b�N��)
//			exbuf		�O���o�b�t�@
// �Ԓl:	int			�X�V�����T���v����
////////////////////////////////////////////////////////////////
int EL6::SoundUpdate( int samples, cRing *exbuf )
{
	// PSG�X�V
	vm->psg->SoundUpdate( samples );
	int size = vm->psg->SndDev::cRing::ReadySize();
	
	// CMT(LOAD)�X�V
	vm->cmtl->SoundUpdate( size );
	
	// ���������X�V
	if( vm->voice ) vm->voice->SoundUpdate( size );
	
	// �T�E���h�o�b�t�@�X�V
	int ret = snd->PreUpdate( size, exbuf );
	
	return ret;
}


////////////////////////////////////////////////////////////////
// �X�g���[���X�V �R�[���o�b�N�֐�
//
// ����:	userdata	�R�[���o�b�N�֐��ɓn���f�[�^(�������g�ւ̃I�u�W�F�N�g�|�C���^)
//			stream		�X�g���[�������݃o�b�t�@�ւ̃|�C���^
//			len			�o�b�t�@�T�C�Y(�o�C�g�P��)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::StreamUpdate( void *userdata, BYTE *stream, int len )
{
	EL6 *p6 = STATIC_CAST( EL6 *, userdata );	// �������g�̃I�u�W�F�N�g�|�C���^�擾
	
	// �T�E���h�o�b�t�@�X�V
	//  �����T���v����������Ȃ���΂����Œǉ�
	//  �������r�f�I�L���v�`����,�|�[�Y��,���j�^���[�h�̏ꍇ�͖���
	int addsam = len/sizeof(int16_t) - p6->snd->cRing::ReadySize();
	
	p6->snd->Update( stream, min( (int)(len/sizeof(int16_t)), p6->snd->cRing::ReadySize() ) );
	
	if( addsam > 0 && !p6->AVI6::IsAVI() && !p6->sche->GetPauseEnable()
		#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		&& !p6->cfg->GetMonDisp()
		#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
																		){
		p6->SoundUpdate( addsam );
		p6->snd->Update( stream, addsam );
	}
	
	// �^�C�~���O�����p�E�F�C�g����
	p6->sche->WaitReset();
}


////////////////////////////////////////////////////////////////
// FPS�\���^�C�} �R�[���o�b�N�֐�
////////////////////////////////////////////////////////////////
DWORD EL6::UpDateFPS( DWORD interval, void *obj )
{
	EL6 *p6 = STATIC_CAST( EL6 *, obj );	// �������g�̃I�u�W�F�N�g�|�C���^�擾
	
	OSD_PushEvent( EV_FPSUPDATE, p6->UDFPSCount );
	p6->UDFPSCount = 0;
	
	return interval;
}


////////////////////////////////////////////////////////////////
// FPS�\���^�C�}�ݒ�
////////////////////////////////////////////////////////////////
bool EL6::SetFPSTimer( int fps )
{
	// �^�C�}�ғ����Ȃ��~
	if( UpDateFPSID ){
		OSD_DelTimer( UpDateFPSID );
		UpDateFPSID = NULL;
	}
	
	// �^�C�}�ݒ�
	if( fps > 0 )
		UpDateFPSID = OSD_AddTimer( 1000/fps, EL6::UpDateFPS, this );
	
	return UpDateFPSID ? true : false;
}


////////////////////////////////////////////////////////////////
// �����L�[���͎��s��?
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���s�� false:���s���łȂ�
////////////////////////////////////////////////////////////////
bool EL6::IsAutoKey( void )
{
	return ak.Num ? true : false;
}


////////////////////////////////////////////////////////////////
// �����L�[����1�����擾
//   (VSYNC=1/60sec���ɌĂ΂��)
//
// ����:	�Ȃ�
// �Ԓl:	BYTE	P6�̃L�[�R�[�h
////////////////////////////////////////////////////////////////
char EL6::GetAutoKey( void )
{
	// �����[ON�҂�
	if( ak.RelayOn ){
		if( vm->cmtl->IsRelay() ) ak.RelayOn = false;
		else                      return 0;
	}
	
	// �����[OFF�҂�
	if( ak.Relay ){
		if( !vm->cmtl->IsRelay() ) ak.Relay = false;
		else                       return 0;
	}
	
	// �҂�?
	if( ak.Wait > 0 ){
		ak.Wait--;
		return 0;
	}
	
	
	// ���̕������擾
	if( ak.Num-- > 0 ){
		BYTE dat = ak.Buffer[ak.Seek++];
		switch( dat ){
		case 0x17:	// '\w' �E�F�C�g�ݒ�
			if( ak.Num-- > 0 ) ak.Wait += (int)((BYTE)ak.Buffer[ak.Seek++]);	// �҂��񐔐ݒ�
			dat = 0;
			break;
			
		case 0x0a:	// '\r' �����[OFF�҂�
			ak.Relay   = true;
			ak.RelayOn = true;
			dat = 0x0d;
		case 0x0d:	// '\n' ���s?
			ak.Wait = 9;	// �҂�9��(=150msec)
			break;
			
		default:	// ��ʂ̕���
			ak.Wait = 0;	// �҂��Ȃ�
		}
		return dat;
	}else{
		// �o�b�t�@����Ȃ�I��
		delete [] ak.Buffer;
		ak.Buffer = NULL;
		ak.Seek   = 0;
	}
	return 0;
}


////////////////////////////////////////////////////////////////
// �����L�[���͕�����ݒ�
//
// ����:	str		������
//			num		������̒���
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::SetAutoKey( const char *str, int num )
{
	// �L�[�o�b�t�@����U�폜
	
	// �L�[�o�b�t�@�m��
	if( ak.Buffer ) delete [] ak.Buffer;
	ak.Buffer = new char[num+1];
	if( !ak.Buffer ){
		ak.Num = 0;
		return false;
	}
	
	// ��������R�s�[
	strncpy( ak.Buffer, str, num );
	
	ak.Num     = num;	// �c�蕶����
	ak.Wait    = 60;	// �҂��񐔃J�E���^(�����1sec)
	ak.Relay   = false;	// �����[�X�C�b�`OFF�҂��t���O
	ak.RelayOn = false;	// �����[�X�C�b�`ON�҂��t���O
	ak.Seek    = 0;		// �Ǎ��݃|�C���^
	
	return true;
}


////////////////////////////////////////////////////////////////
// �����L�[���͕�����ݒ�(�t�@�C������)
//
// ����:	filepath	���̓t�@�C���ւ̃p�X
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::SetAutoKeyFile( const char *filepath )
{
	FILE *fp;
	char lbuf[1024];
	
	fp = FOPENEN( filepath, "rb" );
	if( !fp ) return false;
	
	fseek( fp, 0, SEEK_END );
	int tsize = ftell( fp ) + 1;
	fseek( fp, 0, SEEK_SET );
	
	// �L�[�o�b�t�@�m��
	if( ak.Buffer ) delete [] ak.Buffer;
	ak.Buffer = new char[tsize+1];
	if( !ak.Buffer ){
		fclose( fp );
		ak.Num = 0;
		return false;
	}
	ZeroMemory( ak.Buffer, sizeof(ak.Buffer) );
	
	// �������Ǎ���
	// �f�[�^�������Ȃ�܂ŌJ��Ԃ�
	while( fgets( lbuf, 1024, fp ) ){
		Sjis2P6( lbuf, lbuf );	// SJIS -> P6
		strcat( ak.Buffer, lbuf );
	}
	fclose( fp );
	tsize = strlen( ak.Buffer );
	
	ak.Num     = tsize;	// �c�蕶����
	ak.Wait    = 60;	// �҂��񐔃J�E���^(�����1sec)
	ak.Relay   = false;	// �����[�X�C�b�`OFF�҂��t���O
	ak.RelayOn = false;	// �����[�X�C�b�`ON�҂��t���O
	ak.Seek    = 0;		// �Ǎ��݃|�C���^
	
	return true;
}


////////////////////////////////////////////////////////////////
// �I�[�g�X�^�[�g������ݒ�
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::SetAutoStart( void )
{
	char kbuf[256] = "";
	char *buf = kbuf;
	
	if( !(vm->cmtl->IsMount() && vm->cmtl->IsAutoStart()) ) return;
	
	const P6TAUTOINFO *ainf = vm->cmtl->GetAutoStartInfo();
	
	// �L�[�o�b�t�@�ɏ�����
	switch( cfg->GetModel() ){
	case 60:	// PC-6001
	case 61:	// PC-6001A
		sprintf( buf, "%c%c", ainf->Page+'0', 0x0d );
		break;
		
	case 64:	// PC-6001mk2SR
		if( ainf->BASIC == 6 ){
			if( vm->disk->GetDrives() )	// ??? ���ۂ�?
				sprintf( buf, "%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 20, 0x0d, 0x17, 10 );
			else
				sprintf( buf, "%c%c%c%c%c%c%c",   0x17, 10, ainf->BASIC+'0', 0x17, 20,       0x17, 10 );
			break;
		}
		
	case 62:	// PC-6001mk2
		switch( ainf->BASIC ){
		case 3:
		case 4:
		case 5:
			if( vm->disk->GetDrives() )	// ??? ���ۂ�?
				sprintf( buf, "%c%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 30, 0x0d, ainf->Page+'0', 0x0d, 0x17, 120 );
			else
				sprintf( buf, "%c%c%c%c%c%c%c%c%c",   0x17, 50, ainf->BASIC+'0', 0x17, 30,       ainf->Page+'0', 0x0d, 0x17, 120 );
			break;
		default:
			sprintf( buf, "%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 30, ainf->Page+'0', 0x0d, 0x17, 20 );
		}
		break;
		
	case 68:	// PC-6601SR
		if( ainf->BASIC == 6 ){
			if( vm->disk->IsMount( 0 ) )
				sprintf( buf, "%c%c%c%c%c%c%c%c%c%c%c", 0x17, 240, 0x17, 60, 0x14, 0xf4, 0x17, 30, 0x0d, 0x17, 10 );
			else
				sprintf( buf, "%c%c%c%c%c%c%c%c",   0x17, 240, 0x14, 0xf4, 0x17, 30,       0x17, 10 );
			break;
		}else{
			sprintf( buf, "%c%c%c%c%c%c%c%c", 0x17, 240, 0x17, vm->disk->IsMount( 0 ) ? 60 : 1, 0x17, vm->disk->IsMount( 1 ) ? 60 : 1, 0x14, 0xf3 );
		}
		buf += strlen(kbuf);
		
	case 66:	// PC-6601
		switch( ainf->BASIC ){
		case 3:
		case 4:
		case 5:
			if( vm->disk->IsMount( 0 ) )
				sprintf( buf, "%c%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 30, 0x0d, ainf->Page+'0', 0x0d, 0x17, 110 );
			else
				sprintf( buf, "%c%c%c%c%c%c%c%c%c",   0x17, 50, ainf->BASIC+'0', 0x17, 30,       ainf->Page+'0', 0x0d, 0x17, 110 );
			break;
		default:
			sprintf( buf, "%c%c%c%c%c%c%c%c%c", 0x17, 50, ainf->BASIC+'0', 0x17, 30, ainf->Page+'0', 0x0d, 0x17, 10 );
		}
		break;
		
	}
	strcat( kbuf, ainf->ask );
		
	// �����L�[���͐ݒ�
	if( strlen(kbuf) ) SetAutoKey( kbuf, strlen(kbuf) );
}




////////////////////////////////////////////////////////////////
// ���j�^���[�h?
//
// ����:	�Ȃ�
// �Ԓl:	bool		true:���j�^���[�h false:���s��
////////////////////////////////////////////////////////////////
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
bool EL6::IsMonitor( void ) const
{
	return cfg->GetMonDisp();
}
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


////////////////////////////////////////////////////////////////
// �ǂ��ł�SAVE
//
// ����:	filename	�t�@�C����
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::DokoDemoSave( const char *filename )
{
	PRINTD( VM_LOG, "[EL6][DokoDemoLoad]\n" );
	
	cIni *Ini = NULL;
	
	// �Ƃ肠�����G���[�����Z�b�g
	Error::Reset();
	try{
		FILE *fp = FOPENEN( filename, "wt" );
		if( !fp ) throw Error::DokoWriteFailed;
		// �^�C�g���s���o�͂��Ĉ�U����
		fprintf( fp, MSDOKO_TITLE );
		fclose( fp );
		
		// �ǂ��ł�SAVE�t�@�C�����J��
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::DokoWriteFailed;
		
		// �e�I�u�W�F�N�g�̃p�����[�^������
		if( !cfg->DokoSave( Ini )      ||
			!vm->evsc->DokoSave( Ini ) ||
			!vm->intr->DokoSave( Ini ) ||
			!vm->cpum->DokoSave( Ini ) ||
			!vm->cpus->DokoSave( Ini ) ||
			!vm->mem->DokoSave( Ini )  ||
			!vm->vdg->DokoSave( Ini )  ||
			!vm->psg->DokoSave( Ini )  ||
			!vm->pio->DokoSave( Ini )  ||
			!vm->key->DokoSave( Ini )  ||
			!vm->cmtl->DokoSave( Ini ) ||
			!vm->disk->DokoSave( Ini )
		) throw Error::GetError();
		if( vm->voice && !vm->voice->DokoSave( Ini ) ) throw Error::GetError();
		
		
		Ini->PutEntry( "KEY", NULL, "AK_Num",		"%d",	ak.Num );
		Ini->PutEntry( "KEY", NULL, "AK_Wait",		"%d",	ak.Wait );
		Ini->PutEntry( "KEY", NULL, "AK_Relay",		"%s",	ak.Relay   ? "Yes" : "No" );
		Ini->PutEntry( "KEY", NULL, "AK_RelayOn",	"%s",	ak.RelayOn ? "Yes" : "No" );
		Ini->PutEntry( "KEY", NULL, "AK_Seek",		"%d",	ak.Seek );
		
		char stren[16],strva[256];
		int bnum = ak.Num + ak.Seek;
		
		if( ak.Buffer && bnum ){
			int i=0, nn=0;
			
			do{
				sprintf( &strva[(i&63)*2], "%02X", ak.Buffer[i] );
				i++;
				if( !(i&63) ){
					sprintf( stren, "AKBuf_%02X", nn++ );
					Ini->PutEntry( "KEY", NULL, stren, "%s", strva );
				}
			}while( --bnum );
			
			sprintf( stren, "AKBuf_%02X", nn );
			Ini->PutEntry( "KEY", NULL, stren, "%s", strva );
		}
	}
	catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// ��O����
		Error::SetError( i );
		if( Ini ) delete Ini;
		return false;
	}
	
	Ini->Write();
	
	delete Ini;
	
	return true;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD
//
// ����:	filename	�t�@�C����
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::DokoDemoLoad( const char *filename )
{
	PRINTD( VM_LOG, "[EL6][DokoDemoLoad]\n" );
	
	cIni *Ini = NULL;
	
	// �Ƃ肠�����G���[�����Z�b�g
	Error::Reset();
	try{
		// �ǂ��ł�LOAD�t�@�C�����J��
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::DokoReadFailed;
		
		// PC6001V�̃o�[�W�����m�F�Ǝ�v�\������Ǎ���
		// (�@��,FDD�䐔,�g��RAM,ROM�p�b�`,��m�̃J�[�g���b�W)
		if( !cfg->DokoLoad( Ini ) ) throw Error::GetError();
		
		// VM�ď�����
		Init( cfg );
		
		// �e�I�u�W�F�N�g�̃p�����[�^�Ǎ���
		if(	!vm->evsc->DokoLoad( Ini ) ||
			!vm->intr->DokoLoad( Ini ) ||
			!vm->cpum->DokoLoad( Ini ) ||
			!vm->cpus->DokoLoad( Ini ) ||
			!vm->mem->DokoLoad( Ini )  ||
			!vm->vdg->DokoLoad( Ini )  ||
			!vm->psg->DokoLoad( Ini )  ||
			!vm->pio->DokoLoad( Ini )  ||
			!vm->key->DokoLoad( Ini )  ||
			!vm->cmtl->DokoLoad( Ini ) ||
			!vm->disk->DokoLoad( Ini )
		) throw Error::GetError();
		if( vm->voice && !vm->voice->DokoLoad( Ini ) ) throw Error::GetError();
		
		Ini->GetInt(   "KEY", "AK_Num",		&ak.Num,		ak.Num );
		Ini->GetInt(   "KEY", "AK_Wait",	&ak.Wait,		ak.Wait );
		Ini->GetTruth( "KEY", "AK_Relay",	&ak.Relay,		ak.Relay );
		Ini->GetTruth( "KEY", "AK_RelayOn",	&ak.RelayOn,	ak.RelayOn );
		Ini->GetInt(   "KEY", "AK_Seek",	&ak.Seek,		ak.Seek );
		
		if( ak.Buffer ){
			delete [] ak.Buffer;
			ak.Buffer = NULL;
		}
		int bnum = ak.Num + ak.Seek;
		if( bnum ){
			ak.Buffer = new char[bnum+1];
			
			for( int i=0; i<bnum; i+=64 ){
				char stren[16],strva[256];
				sprintf( stren, "AKBuf_%02X", i/64 );
				memset( strva, '0', 64*2 );
				if( Ini->GetString( "KEY", stren, strva, strva ) ){
					for( int j=0; j<min(64,bnum-i); j++ ){
						char dt[5] = "0x";
						strncpy( &dt[2], &strva[j*2], 2 );
						ak.Buffer[i+j] = strtol( dt, NULL, 16 );
					}
				}
			}
		}
		
		// �f�B�X�N�h���C�u���ɂ���ăX�N���[���T�C�Y�ύX
		if( !staw->Init( -1, vm->disk->GetDrives() ) ) throw Error::GetError();
		if( !graph->ResizeScreen() ) throw Error::GetError();
	}
	catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// ��O����
		Error::SetError( i );
		if( Ini ) delete Ini;
		return false;
	}
	
	delete Ini;
	
	return true;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD�t�@�C������@�햼�Ǎ�
//
// ����:	filename	�t�@�C����
// �Ԓl:	int			�@�햼(60,61,62,66)
////////////////////////////////////////////////////////////////
int EL6::GetDokoModel( const char *filename )
{
	cIni *Ini = NULL;
	int st;
	
	try{
		// �ǂ��ł�LOAD�t�@�C�����J��
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::DokoReadFailed;
		
		// �@��擾
		Ini->GetInt( "GLOBAL", "Model",	&st, 0 );
	}
	catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
		Error::SetError( Error::MemAllocFailed );
		return 0;
	}
	catch( Error::Errno i ){	// ��O����
		Error::SetError( i );
		if( Ini ) delete Ini;
		return 0;
	}
	
	delete Ini;
	
	return st;
}







////////////////////////////////////////////////////////////////
// TAPE �}�E���g
//
// ����:	filename	�t�@�C����
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::TapeMount( const char *filename )
{
	if( !vm->cmtl->Mount( filename ) ) return false;
	return true;
}


////////////////////////////////////////////////////////////////
// TAPE �A���}�E���g
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::TapeUnmount( void )
{
	vm->cmtl->Unmount();
}


////////////////////////////////////////////////////////////////
// DISK �}�E���g
//
// ����:	drv			�h���C�u�ԍ�
//			filename	�t�@�C����
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::DiskMount( int drv, const char *filename )
{
	if( !vm->disk->Mount( drv, filename ) ) return false;
	return true;
}


////////////////////////////////////////////////////////////////
// DISK �A���}�E���g
//
// ����:	drv			�h���C�u�ԍ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::DiskUnmount( int drv )
{
	vm->disk->Unmount( drv );
}






////////////////////////////////////////////////////////////////
// ���v���C�ۑ��J�n
//
// ����:	filename	�t�@�C����
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::ReplayRecStart( const char *filename )
{
	return REPLAY::StartRecord( filename );
}


////////////////////////////////////////////////////////////////
// ���v���C�ۑ��ĊJ
//
// ����:	filename	�t�@�C����
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::ReplayRecResume( const char *filename )
{
	// �r���Z�[�u�t�@�C����T��
	char strsave[PATH_MAX];
	strncpy( strsave, filename, PATH_MAX );
	strncpy( (char *)OSD_GetFileNameExt( strsave ), resext, sizeof(resext) );	// �g���q�����ւ�
	
	if( OSD_FileExist( strsave ) ){
		cIni save;
		save.Init( strsave );
		int frame = 0;
		save.GetInt( "REPLAY", "frame", &frame, frame );
		if( frame == 0 ) return false;
		
		DokoDemoLoad( strsave );
		return REPLAY::ResumeRecord( filename, frame );
	}
	return false;
}


////////////////////////////////////////////////////////////////
// ���v���C���ǂ��ł�LOAD
//
// ����:	�Ȃ�
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::ReplayRecDokoLoad( void )
{
	if( REPLAY::GetStatus() == REP_RECORD ){
		char filename[PATH_MAX];
		strncpy( filename, REPLAY::Ini->GetFileName(), PATH_MAX );
		REPLAY::StopRecord();
		return ReplayRecResume( filename );
	}else{
		return false;
	}
}


////////////////////////////////////////////////////////////////
// ���v���C���ǂ��ł�SAVE
//
// ����:	�Ȃ�
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EL6::ReplayRecDokoSave( void )
{
	if( REPLAY::GetStatus() == REP_RECORD ){
		// �r���Z�[�u�t�@�C����ۑ�
		char strsave[PATH_MAX];
		strncpy( strsave, REPLAY::Ini->GetFileName(), PATH_MAX );
		strncpy( (char *)OSD_GetFileNameExt( strsave ), resext, sizeof(resext) );	// �g���q�����ւ�
		if( !DokoDemoSave( strsave ) ) return false;
		
		// �r���Z�[�u����ǋL
		cIni save;
		if( !save.Init( strsave ) ) return false;
		save.PutEntry( "REPLAY", NULL, "frame", "%d", REPLAY::RepFrm );
		// ��U�L�[���͂𖳌�������(LOAD���ɃL�[���������ςȂ��ɂȂ�̂�h������)
		save.PutEntry( "KEY", NULL, "P6Matrix", "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
		save.PutEntry( "KEY", NULL, "P6Mtrx",   "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF" );
		
		save.Write();
		
		return true;
	}else{
		return false;
	}
}


////////////////////////////////////////////////////////////////
// ���v���C�ۑ���~
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::ReplayRecStop( void )
{
	ReplayRecDokoSave();
	REPLAY::StopRecord();
}


////////////////////////////////////////////////////////////////
// ���v���C�Đ��J�n
//
// ����:	filename	�t�@�C����
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::ReplayPlayStart( const char *filename )
{
	cfg->SetModel( GetDokoModel( filename ) );
	cfg->SetDokoFile( filename );
	OSD_PushEvent( EV_REPLAY );
}


////////////////////////////////////////////////////////////////
// ���v���C�Đ���~
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::ReplayPlayStop( void )
{
	REPLAY::StopReplay();
}







////////////////////////////////////////////////////////////////
// UI:TAPE �}��
//
// ����:	path		�t�@�C����
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_TapeInsert( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path ){
		if( !OSD_FileExist( TapePathUI ) )
			strncpy( TapePathUI, cfg->GetTapePath(), PATH_MAX );
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_TapeLoad, str, TapePathUI ) )
			fpath = str;
	}
	if( !fpath ) return;
	
	if( !TapeMount( fpath ) ) Error::SetError( Error::TapeMountFailed );
}



////////////////////////////////////////////////////////////////
// UI:DISK �}��
//
// ����:	drv			�h���C�u�ԍ�
//			path		�t�@�C����
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_DiskInsert( int drv, const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path ){
		if( !OSD_FileExist( DiskPathUI ) )
			strncpy( DiskPathUI, cfg->GetDiskPath(), PATH_MAX );
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_Disk, str, DiskPathUI ) )
			fpath = str;
	}
	if( !fpath ) return;
	
	if( !DiskMount( drv, fpath ) ) Error::SetError( Error::DiskMountFailed );
}


////////////////////////////////////////////////////////////////
// UI:�g��ROM �}��
//
// ����:	path		�t�@�C����
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_RomInsert( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path ){
		if( !OSD_FileExist( ExRomPathUI ) )
			strncpy( ExRomPathUI, cfg->GetExtRomPath(), PATH_MAX );
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_ExtRom, str, ExRomPathUI ) )
			fpath = str;
	}
	if( !fpath ) return;
	
	// ���Z�b�g�𔺂��̂Ń��b�Z�[�W�\��
	OSD_Message( MSG_RESETI, MSG_RESETC, OSDM_OK | OSDM_ICONINFO );
	if( !vm->mem->MountExtRom( fpath ) )
		Error::SetError( Error::ExtRomMountFailed );
	else
		UI_Reset();
}


////////////////////////////////////////////////////////////////
// UI:�g��ROM �r�o
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_RomEject( void )
{
	// ���Z�b�g�𔺂��̂Ń��b�Z�[�W�\��
	OSD_Message( MSG_RESETE, MSG_RESETC, OSDM_OK | OSDM_ICONINFO );
	vm->mem->UnmountExtRom();
	UI_Reset();
}


////////////////////////////////////////////////////////////////
// UI:�ǂ��ł�SAVE
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_DokoSave( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path ){
		if( !OSD_FileExist( DokoPathUI ) )
			strncpy( DokoPathUI, cfg->GetDokoSavePath(), PATH_MAX );
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_DokoSave, str, DokoPathUI ) )
			fpath = str;
	}
	if( !fpath ) return;
	
	if( !DokoDemoSave( fpath ) ) Error::SetError( Error::DokoWriteFailed );
}


////////////////////////////////////////////////////////////////
// UI:�ǂ��ł�LOAD
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_DokoLoad( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path ){
		if( !OSD_FileExist( DokoPathUI ) )
			strncpy( DokoPathUI, cfg->GetDokoSavePath(), PATH_MAX );
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_DokoLoad, str, DokoPathUI ) )
			fpath = str;
	}
	if( !fpath ) return;
	
	cfg->SetModel( GetDokoModel( fpath ) );
	cfg->SetDokoFile( fpath );
	OSD_PushEvent( EV_DOKOLOAD );
}


////////////////////////////////////////////////////////////////
// UI:���v���C�ۑ�
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_ReplaySave( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( REPLAY::GetStatus() == REP_IDLE ){
		if( !path ){
			if( !OSD_FileExist( DokoPathUI ) )
				strncpy( DokoPathUI, cfg->GetDokoSavePath(), PATH_MAX );
			if( OSD_FileSelect( graph->GetWindowHandle(), FD_RepSave, str, DokoPathUI ) )
				fpath = str;
		}
		if( !fpath ) return;
		
		if( !DokoDemoSave( fpath ) || !ReplayRecStart( fpath ) )
			Error::SetError( Error::ReplayRecError );
		
	}else if( REPLAY::GetStatus() == REP_RECORD ){
		ReplayRecStop();
	}
}


////////////////////////////////////////////////////////////////
// UI:���v���C�ĊJ
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_ReplayResumeSave( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( REPLAY::GetStatus() == REP_IDLE ){
		if( !path ){
			if( !OSD_FileExist( DokoPathUI ) )
				strncpy( DokoPathUI, cfg->GetDokoSavePath(), PATH_MAX );
			if( OSD_FileSelect( graph->GetWindowHandle(), FD_RepSave, str, DokoPathUI ) )
				fpath = str;
		}
		if( !fpath ) return;
		
		if( !ReplayRecResume( fpath ) ) Error::SetError( Error::ReplayRecError );
	}
}


////////////////////////////////////////////////////////////////
// UI:���v���C���ǂ��ł�LOAD
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_ReplayDokoLoad()
{
	ReplayRecDokoLoad();
}


////////////////////////////////////////////////////////////////
// UI:���v���C���ǂ��ł�SAVE
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_ReplayDokoSave()
{
	ReplayRecDokoSave();
}


////////////////////////////////////////////////////////////////
// UI:���v���C�Đ�
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_ReplayLoad( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( REPLAY::GetStatus() == REP_IDLE ){
		if( !path ){
			if( !OSD_FileExist( DokoPathUI ) )
				strncpy( DokoPathUI, cfg->GetDokoSavePath(), PATH_MAX );
			if( OSD_FileSelect( graph->GetWindowHandle(), FD_RepLoad, str, DokoPathUI ) )
				fpath = str;
		}
	}else if( REPLAY::GetStatus() == REP_REPLAY ){
		ReplayPlayStop();
	}
	
	if( !fpath ) return;
	
	ReplayPlayStart( fpath );
}


////////////////////////////////////////////////////////////////
// UI:�r�f�I�L���v�`��
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_AVISave( void )
{
	char str[PATH_MAX];
	
	if( !AVI6::IsAVI() ){
		if( OSD_FileSelect( graph->GetWindowHandle(), FD_AVISave, str, (char *)OSD_GetModulePath() ) ){
			AVI6::StartAVI( str, graph->ScreenX(), graph->ScreenY(), FRAMERATE, cfg->GetSampleRate(), cfg->GetAviBpp() );
		}
	}else{
		AVI6::StopAVI();
	}
}


////////////////////////////////////////////////////////////////
// UI:�ō��ݑ�s
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_AutoType( const char *path )
{
	char str[PATH_MAX];
	const char *fpath = path;
	
	if( !path && OSD_FileSelect( graph->GetWindowHandle(), FD_LoadAll, str, (char *)OSD_GetModulePath() ) )
		fpath = str;
	
	if( !fpath ) return;
	
	if( !SetAutoKeyFile( fpath ) ) Error::SetError( Error::Unknown );
}


////////////////////////////////////////////////////////////////
// UI:���Z�b�g
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_Reset( void )
{
	bool can = this->cThread::IsCancel();	// �X���b�h��~�ς�?
	
	if( !can ) Stop();	// �X���b�h�����Ă����U�~�߂�
	
	// �V�X�e���f�B�X�N�������Ă�����TAPE�̃I�[�g�X�^�[�g����
	if( !vm->disk->IsSystem(0) && !vm->disk->IsSystem(1) )
		SetAutoStart();
	
	vm->Reset();
	
	if( !can ) Start();	// ���X�X���b�h�������Ă�����Ďn��
}


////////////////////////////////////////////////////////////////
// UI:�ċN��
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_Restart( void )
{
	OSD_PushEvent( EV_RESTART );
}


////////////////////////////////////////////////////////////////
// UI:�I��
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_Quit( void )
{
	OSD_PushEvent( EV_QUIT );
}


////////////////////////////////////////////////////////////////
// UI:Wait�ύX
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_NoWait( void )
{
	sche->SetWaitEnable( sche->GetWaitEnable() ? false : true );
}


////////////////////////////////////////////////////////////////
// UI:Turbo TAPE�ύX
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_TurboTape( void )
{
	cfg->SetTurboTAPE( cfg->GetTurboTAPE() ? false : true );
}


////////////////////////////////////////////////////////////////
// UI:Boost Up�ύX
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_BoostUp( void )
{
	cfg->SetBoostUp( cfg->GetBoostUp() ? false : true );
	vm->cmtl->SetBoost( vm->cmtl->IsBoostUp() ? false : true );
}


////////////////////////////////////////////////////////////////
// UI:�X�L�������C�����[�h�ύX
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_ScanLine( void )
{
	// �r�f�I�L���v�`�����͖���
	if( !AVI6::IsAVI() ){
		cfg->SetScanLine( cfg->GetScanLine() ? false : true );
		graph->ResizeScreen();	// �X�N���[���T�C�Y�ύX
	}
}


////////////////////////////////////////////////////////////////
// UI:4:3�\���ύX
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_Disp43( void )
{
	// �r�f�I�L���v�`�����͖���
	if( !AVI6::IsAVI() ){
		cfg->SetDispNTSC( cfg->GetDispNTSC() ? false : true );
		graph->ResizeScreen();	// �X�N���[���T�C�Y�ύX
	}
}


////////////////////////////////////////////////////////////////
// UI:�X�e�[�^�X�o�[�\����ԕύX
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_StatusBar( void )
{
	cfg->SetDispStat( cfg->GetDispStat() ? false : true );
	graph->ResizeScreen();	// �X�N���[���T�C�Y�ύX
}


////////////////////////////////////////////////////////////////
// UI:MODE4�J���[�ύX
//
// ����:	col			0:���m�N�� 1:��/�� 2:��/�� 3:�s���N/�� 4:��/�s���N
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_Mode4Color( int col )
{
	cfg->SetMode4Color( col );
	vm->vdg->SetMode4Color( col );
}


////////////////////////////////////////////////////////////////
// UI:�t���[���X�L�b�v�ύX
//
// ����:	sk			�t���[���X�L�b�v��
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_FrameSkip( int sk )
{
	if( !AVI6::IsAVI() ) cfg->SetFrameSkip( sk );
}


////////////////////////////////////////////////////////////////
// UI:�T���v�����O���[�g�ύX
//
// ����:	rate		�T���v�����O���[�g(Hz)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_SampleRate( int rate )
{
	cfg->SetSampleRate( rate );
	snd->SetSampleRate( rate );
}


////////////////////////////////////////////////////////////////
// UI:���ݒ�
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::UI_Config( void )
{
	cfg->Write();
	if( OSD_ConfigDialog( graph->GetWindowHandle() ) > 0 )
		// �ċN��?
		if( OSD_Message( MSG_RESTART, MSG_RESTARTC, OSDM_YESNO | OSDM_ICONQUESTION ) == OSDR_YES )
			OSD_PushEvent( EV_RESTART );
}


////////////////////////////////////////////////////////////////
// ���j���[�I�����ڎ��s
//
// ����:	id		�I���������j���[ID
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EL6::ExecMenu( int id )
{
	// ���ڂ��Ƃ̏���
	switch( id ){
	case ID_TAPEINSERT:		UI_TapeInsert();						break;	// TAPE �}��
	case ID_TAPEEJECT:		TapeUnmount();							break;	// TAPE �r�o
	case ID_DISKINSERT1:													// DISK �}��
	case ID_DISKINSERT2:	UI_DiskInsert( id - ID_DISKINSERT1 );	break;
	case ID_DISKEJECT1:														// DISK �r�o
	case ID_DISKEJECT2:		DiskUnmount( id - ID_DISKEJECT1 );		break;
	case ID_ROMINSERT:		UI_RomInsert();							break;	// �g��ROM �}��
	case ID_ROMEJECT:		UI_RomEject();							break;	// �g��ROM �r�o
	case ID_JOY100:															// �W���C�X�e�B�b�N1
	case ID_JOY101:
	case ID_JOY102:
	case ID_JOY103:
	case ID_JOY104:
	case ID_JOY105:			joy->Connect( 0, id - ID_JOY101 );		break;
	case ID_JOY200:															// �W���C�X�e�B�b�N2
	case ID_JOY201:
	case ID_JOY202:
	case ID_JOY203:
	case ID_JOY204:
	case ID_JOY205:			joy->Connect( 1, id - ID_JOY201 );		break;
	case ID_CONFIG:			UI_Config();							break;	// ���ݒ�
	case ID_RESET:			UI_Reset();								break;	// ���Z�b�g
	case ID_RESTART:		UI_Restart();							break;	// �ċN��
	case ID_DOKOSAVE:		UI_DokoSave();							break;	// �ǂ��ł�SAVE
	case ID_DOKOLOAD:		UI_DokoLoad();							break;	// �ǂ��ł�LOAD
	case ID_REPLAYSAVE:		UI_ReplaySave();						break;	// ���v���C�ۑ�
	case ID_REPLAYRESUME:	UI_ReplayResumeSave();					break;	// ���v���C�ۑ��ĊJ
	case ID_REPLAYDOKOLOAD:	UI_ReplayDokoLoad();					break;	// ���v���C���ǂ��ł�LOAD
	case ID_REPLAYDOKOSAVE:	UI_ReplayDokoSave();					break;	// ���v���C���ǂ��ł�SAVE
	case ID_REPLAYLOAD:		UI_ReplayLoad();						break;	// ���v���C�Đ�
	case ID_AVISAVE:		UI_AVISave();							break;	// �r�f�I�L���v�`��
	case ID_AUTOTYPE:		UI_AutoType();							break;	// �ō��ݑ�s
	case ID_QUIT:			UI_Quit();								break;	// �I��
	case ID_NOWAIT:			UI_NoWait();							break;	// Wait�L�������ύX
	case ID_TURBO:			UI_TurboTape();							break;	// Turbo TAPE
	case ID_BOOST:			UI_BoostUp();							break;	// Boost Up
	case ID_SCANLINE:		UI_ScanLine();							break;	// �X�L�������C�����[�h�ύX
	case ID_DISP43:			UI_Disp43();							break;	// 4:3�\���ύX
	case ID_STATUS:			UI_StatusBar();							break;	// �X�e�[�^�X�o�[�\����ԕύX
	case ID_M4MONO:															// MODE4�J���[ ���m�N��
	case ID_M4RDBL:															// MODE4�J���[ ��/��
	case ID_M4BLRD:															// MODE4�J���[ ��/��
	case ID_M4PKGR:															// MODE4�J���[ �s���N/��
	case ID_M4GRPK:			UI_Mode4Color( id - ID_M4MONO );		break;	// MODE4�J���[ ��/�s���N
	case ID_FSKP0:															// �t���[���X�L�b�v �Ȃ�
	case ID_FSKP1:															// �t���[���X�L�b�v 1
	case ID_FSKP2:															// �t���[���X�L�b�v 2
	case ID_FSKP3:															// �t���[���X�L�b�v 3
	case ID_FSKP4:															// �t���[���X�L�b�v 4
	case ID_FSKP5:			UI_FrameSkip( id - ID_FSKP0 );			break;	// �t���[���X�L�b�v 5
	case ID_SPR44:															// �T���v�����O���[�g 44100Hz
	case ID_SPR22:															// �T���v�����O���[�g 22050Hz
	case ID_SPR11:			UI_SampleRate( 44100 >> (id - ID_SPR44 ) );	break;	// �T���v�����O���[�g 11025Hz
	case ID_VERSION:		OSD_VersionDialog( graph->GetWindowHandle(), cfg->GetModel() );	break;	// �o�[�W�������
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	case ID_MONITOR:		ToggleMonitor();						break;	// ���j�^�[���[�h
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	}
}
