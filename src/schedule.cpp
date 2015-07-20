#include "pc6001v.h"

#include "config.h"
#include "log.h"
#include "osd.h"
#include "schedule.h"


#define WRUPDATE	(1000/SPDCNT)	/* �������x�X�V�Ԋu(ms) */


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
EVSC::EVSC( int mclock ) : VSYNC(false), MasterClock(mclock), NextEvent(-1), SaveClock(0) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
EVSC::~EVSC( void ){}


////////////////////////////////////////////////////////////////
// �C�x���g����
//
// ����:	devid		�f�o�C�X�I�u�W�F�N�gID
//			id			�C�x���gID
// �Ԓl:	evinfo *	�C�x���g���|�C���^(���݂��Ȃ����NULL)
////////////////////////////////////////////////////////////////
const EVSC::evinfo *EVSC::Find( Device::ID devid, int id ) const
{
	for( int i=0; i<MAXEVENT; i++ )
		if( ev[i].devid == devid && ev[i].id == id ) return &ev[i];
	
	return NULL;
}


////////////////////////////////////////////////////////////////
// �ڑ�����f�o�C�X����o�^����
//
// ����:	dev		�f�o�C�X�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool EVSC::Entry( Device *dev )
{
	// �ǉ�&�폜���ăf�o�C�X���X�g�Ɍ���o�^����
	if( !dev || !Add( dev, 0, 0, 0 ) || !Del( dev, 0 ) ) return false;
	return true;
}


////////////////////////////////////////////////////////////////
// �C�x���g�ǉ�
//
// ����:	dev		�f�o�C�X�I�u�W�F�N�g�|�C���^
//			id		�C�x���gID
//			hz		�C�x���g�������g��
//			loop	���[�v�w�� true:���[�v false:�����^�C��
//			flag	�C�x���g�X�^�C���w��
//				bit0  : �J��Ԃ��w�� 0:�����^�C�� 1:���[�v
//				bit2,3,4: �����w��P�� 000:Hz 001:us 010:ms 100:CPU�X�e�[�g��
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool EVSC::Add( Device *dev, int id, double hz, int flag )
{
	// �o�^�ς݂̏ꍇ�͈�U�폜���čēo�^
	const evinfo *e = Find( dev->GetID(), id );
	if( e ) Del( (Device *)devlist.Find( e->devid ), e->id );
	
	for( int i=0; i<MAXEVENT; i++ ){
		if( !ev[i].devid ){
			devlist.Add( dev );
			
			ev[i].devid  = dev->GetID();
			ev[i].id     = id;
			ev[i].Active = true;
			
			// �C�x���g�������g���̐ݒ�
			switch( flag&(EV_US|EV_MS|EV_STATE) ){
			case EV_US:		// us�Ŏw��
				ev[i].nps    = (double)1000000 / hz;
				ev[i].Clock  = (int)((double)MasterClock / ev[i].nps);
				break;
			case EV_MS:		// ms�Ŏw��
				ev[i].nps    = (double)1000 / hz;
				ev[i].Clock  = (int)((double)MasterClock / ev[i].nps);
				break;
			case EV_STATE:	// CPU�X�e�[�g���Ŏw��
				ev[i].nps    = (double)MasterClock / hz;
				ev[i].Clock  = (int)hz;
				break;
			default:		// Hz�Ŏw��
				ev[i].nps    = hz;
				ev[i].Clock  = (int)((double)MasterClock / hz);
			}
			
			// �����̐ݒ�
			if( flag&EV_LOOP ){	// ���[�v�C�x���g
				ev[i].Period = ev[i].Clock;
				if( ev[i].Period < 1 ) ev[i].Period = 1;
			}else				// �����^�C���C�x���g
				ev[i].Period = 0;
			
			// ���̃C�x���g�܂ł̃N���b�N���X�V
			if( NextEvent < 0 ) NextEvent = ev[i].Clock;
			else                NextEvent = min( NextEvent, ev[i].Clock );
			
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////
// �C�x���g�폜
//
// ����:	dev		�f�o�C�X�I�u�W�F�N�g�|�C���^
//			id		�C�x���gID
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool EVSC::Del( Device *dev, int id )
{
	evinfo *e = (evinfo *)Find( dev->GetID(), id );
	if( e ){
		devlist.Del( dev );
		
		e->devid  = 0;
		e->id     = 0;
		e->Active = false;
		return true;
	}else
		return false;
}


////////////////////////////////////////////////////////////////
// �C�x���g�������g���ݒ�
//
// ����:	devid	�f�o�C�X�I�u�W�F�N�gID
//			id		�C�x���gID
//			hz		�C�x���g�������g��
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool EVSC::SetHz( Device::ID devid, int id, double hz )
{
	evinfo *e = (evinfo *)Find( devid, id );
	if( e ){
		e->nps    = hz;
		e->Period = (int)((double)MasterClock / hz);
		return true;
	}else
		return false;
}


////////////////////////////////////////////////////////////////
// �C�x���g�X�V
//
// ����:	clk		�i�߂�N���b�N��
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EVSC::Update( int clk )
{
	PRINTD( TIC_LOG, "[SCHE][Update] %d clock\n", clk );
	
	// �N���b�N�𗭂ߍ���
	SaveClock  += clk;	// ���̃C�x���g�����p
	
	// ���̃C�x���g�����N���b�N�ɒB���Ă��Ȃ�������߂�
	// ������ clk=0 �̏ꍇ�͍X�V���s��
	if( NextEvent > SaveClock && clk ) return;
	NextEvent = -1;
	
	int cnt;
	do{
		cnt = 0;
		for( int i=0; i<MAXEVENT; i++ ){
			// �L���ȃC�x���g?
			if( ev[i].Active ){
				ev[i].Clock -= SaveClock;
				// �X�V�Ԋu�������ꍇ�͕����񔭐�����\������
				// �Ƃ肠�����S�Ă��Ȃ��܂ŌJ��Ԃ����Ă��Ƃł����̂�?
				if( ev[i].Clock <= 0 ){
					// �C�x���g�R�[���o�b�N�����s
					devlist.Find( ev[i].devid )->EventCallback( ev[i].id, ev[i].Clock );
					
					if( ev[i].Period > 0 ){	// ���[�v�C�x���g
						ev[i].Clock += ev[i].Period;
						if( ev[i].Clock <= 0 ) cnt++;	// ���̃C�x���g���������Ă�����J�E���g
					}else{					// �����^�C���C�x���g
						Del( (Device *)devlist.Find( ev[i].devid ), ev[i].id );
						break;
					}
				}
				// ���̃C�x���g�܂ł̃N���b�N���X�V
				if( NextEvent < 0 ) NextEvent = ev[i].Clock;
				else                NextEvent = min( NextEvent, ev[i].Clock );
			}
		}
		SaveClock = 0;
	}while( cnt > 0 );
}


////////////////////////////////////////////////////////////////
// �w��C�x���g�����Z�b�g����
//
// ����:	devid	�f�o�C�X�I�u�W�F�N�gID
//			id		�C�x���gID
//			ini		�J�E���^�����l(�i�s���Ŏw�� 1.0=100%)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EVSC::Reset( Device::ID devid, int id, double ini )
{
	PRINTD( TIC_LOG, "[SCHE][Reset] ID:%d %3f%%", id, ini );
	
	evinfo *e = (evinfo *)Find( devid, id );
	if( e ){
		// ���ߍ��񂾃N���b�N�����l��
		e->Clock = (int)((double)e->Period * ( 1.0 - min( max( 0.0, ini ), 1.0 ) )) - SaveClock;
		
		PRINTD( TIC_LOG, " -> %d/%d clock", e->Clock, e->Period );
	}
	
	PRINTD( TIC_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// �C�x���g�����܂ł̎c��N���b�N�������߂�
//
// ����:	devid	�f�o�C�X�I�u�W�F�N�gID
//			id		�C�x���gID
// �Ԓl:	int		�c��X�e�[�g��(ID�������Ȃ�0)
////////////////////////////////////////////////////////////////
int EVSC::Rest( Device::ID devid, int id ) const
{
	const evinfo *e = Find( devid, id );
	if( e ) return e->Clock;
	else    return 0;
}


////////////////////////////////////////////////////////////////
// �C�x���g�̐i�s�������߂�
//
// ����:	devid	�f�o�C�X�I�u�W�F�N�gID
//			id		�C�x���gID
// �Ԓl:	double	�C�x���g�i�s��(1.0=100%)
////////////////////////////////////////////////////////////////
double EVSC::GetProgress( Device::ID devid, int id ) const
{
	PRINTD( TIC_LOG, "[SCHE][GetProgress] ID:%d", id );
	
	const evinfo *e = Find( devid, id );
	// �C�x���g�����݂�1�����̃N���b�N�����ݒ肳��Ă���?
	if( e && e->Period > 0 ){
		PRINTD( TIC_LOG, " %d/%d SAVE:%d\n", e->Clock, e->Period, SaveClock );
		
		// ���ߍ��񂾃N���b�N���l��
		double sc = (double)( (double)( e->Period - max( e->Clock - SaveClock, 0 ) ) / (double)e->Period );
		return min( max( 0.0, sc ), 1.0 );
	}else{
		PRINTD( TIC_LOG, " Disable\n" );
	}
	
	return 0;
}


////////////////////////////////////////////////////////////////
// �C�x���g���擾
//
// ����:	evinfo *	�C�x���g���\���̂ւ̃|�C���^(devid,id ���Z�b�g���Ă���)
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EVSC::GetEvinfo( evinfo *info ) const
{
	if( !info ) return false;
	
	const evinfo *e = Find( info->devid, info->id );
	if( e ){
		info->Active = e->Active;
		info->Period = e->Period;
		info->Clock  = e->Clock;
		info->nps    = e->nps;
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// �C�x���g���ݒ�
//
// ����:	evinfo *	�C�x���g���\���̂ւ̃|�C���^(devid,id ���Z�b�g���Ă���)
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool EVSC::SetEvinfo( evinfo *info )
{
	if( !info ) return false;
	
	// �ⓚ���p�Œǉ�
	if( !Add( (Device *)devlist.Find( info->devid ), info->id, 1, EV_HZ ) ) return false;
	
	evinfo *e = (evinfo *)Find( info->devid, info->id );
	if( e ){
		e->Active = info->Active;
		e->Period = info->Period;
		e->Clock  = info->Clock;
		e->nps    = info->nps;
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// �}�X�^�N���b�N�ݒ�
//
// ����:	clock	�}�X�^�N���b�N��
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EVSC::SetMasterClock( int clock )
{
	MasterClock = clock;
	for( int i=0; i<MAXEVENT; i++ )
		if( ev[i].devid && ev[i].nps > 0 && ev[i].Period > 0 ){
			ev[i].Period = (int)((double)clock / ev[i].nps);
			if( ev[i].Period < 1 ) ev[i].Period = 1;
		}

}


////////////////////////////////////////////////////////////////
// �}�X�^�N���b�N�擾
//
// ����:	�Ȃ�
// �Ԓl:	int		�}�X�^�N���b�N��
////////////////////////////////////////////////////////////////
int EVSC::GetMasterClock( void ) const
{
	return MasterClock;
}


////////////////////////////////////////////////////////////////
// VSYNC�ɒB����?
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
bool EVSC::IsVSYNC( void ) const
{
	return VSYNC;
}


////////////////////////////////////////////////////////////////
// VSYNC��ʒm����
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EVSC::OnVSYNC( void )
{
	VSYNC = true;
}


////////////////////////////////////////////////////////////////
// VSYNC�t���O�L�����Z��
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void EVSC::ReVSYNC( void )
{
	VSYNC = false;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�SAVE
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool EVSC::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "SCHEDULE", NULL, "MasterClock",	"%d",	MasterClock );
	Ini->PutEntry( "SCHEDULE", NULL, "VSYNC",		"%s",	VSYNC ? "Yes" : "No" );
	Ini->PutEntry( "SCHEDULE", NULL, "NextEvent",	"%d",	NextEvent );
	Ini->PutEntry( "SCHEDULE", NULL, "SaveClock",	"%d",	SaveClock );
	
	
	// �C�x���g
	char stren[16];
	int i = 0;
	
	while( ev[i].devid ){
		BYTE id1,id2,id3,id4;
		DWTOB( ev[i].devid, id4, id3, id2, id1 );
		sprintf( stren, "Event%02X", i );
		Ini->PutEntry( "SCHEDULE", NULL, stren, "%c%c%c%c %d %d %d %d %lf", id1, id2, id3, id4, ev[i].id, ev[i].Active ? 1 : 0, ev[i].Period, ev[i].Clock, ev[i].nps );
		i++;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool EVSC::DokoLoad( cIni *Ini )
{
	if( !Ini ) return false;
	
	// �S�ẴC�x���g���ЂƂ܂������ɂ���
	for( int i=0; i<MAXEVENT; i++ ){
		ev[i].devid  = 0;
		ev[i].id     = 0;
		ev[i].Active = false;
		ev[i].Period = 0;
		ev[i].Clock  = 0;
		ev[i].nps    = 0;
	}
	
	Ini->GetInt(    "SCHEDULE", "MasterClock",	&MasterClock,	MasterClock );
	Ini->GetTruth(  "SCHEDULE", "VSYNC",		&VSYNC, 		VSYNC );
	Ini->GetInt(    "SCHEDULE", "NextEvent",	&NextEvent,		NextEvent );	// �C�x���g�Đݒ莞�ɍČ������?
	Ini->GetInt(    "SCHEDULE", "SaveClock",	&SaveClock,		SaveClock );
	
	
	// �C�x���g
	char stren[16];
	char strrs[64];
	
	for( int i=0; i<MAXEVENT; i++ ){
		sprintf( stren, "Event%02X", i );
		if( Ini->GetString( "SCHEDULE", stren, strrs, "" ) ){
			evinfo e;
			BYTE id1,id2,id3,id4;
			int yn;
			
			sscanf( strrs,"%c%c%c%c %d %d %d %d %lf", &id1, &id2, &id3, &id4, &e.id, &yn, &e.Period, &e.Clock, &e.nps );
			e.devid = BTODW( id1, id2, id3, id4 );
			e.Active = yn ? true : false;
			if( !SetEvinfo( &e ) ) return false;
		}else
			break;
	}
	
	return true;
}









////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
SCH6::SCH6( int mclock ) : WaitEnable(true), PauseEnable(false),
	EnableScrUpdate(0),	SpeedRatio(100), SpeedCnt1(1), SpeedCnt2(1), MasterClock(mclock)
{
	INITARRAY( WRClock, 0 );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
SCH6::~SCH6( void )
{
	Stop();
}


////////////////////////////////////////////////////////////////
// �X���b�h�֐�(�I�[�o�[���C�h)
//   OSD_Delay()�̐��x�ɍ��E�����̂ł�������������������
//   49.7���ȏ�A���ғ�����OS�ł͕s��������邯�ǁD�D�D�����ł���
//
// ����:	inst	�������g�̃I�u�W�F�N�g�|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SCH6::OnThread( void *inst )
{
//	SCH6 *ti;
	int Vint[VSYNC_HZ];
	int VintCnt = 0;
	DWORD now,last;
	
	EnableScrUpdate = 0;
	
//	ti = STATIC_CAST( SCH6 *, inst );	// �������g�̃I�u�W�F�N�g�|�C���^�擾
	
	// 1�b�Ԃ̃C���^�[�o���ݒ�
	for( int i=0; i<VSYNC_HZ; i++ ) Vint[i] = (int)( 1000 / VSYNC_HZ );
	int Vrem = 1000 - (int)( 1000 / VSYNC_HZ ) * VSYNC_HZ;
	for( int i=0; i<Vrem; i++ ) Vint[(int)(VSYNC_HZ * i / Vrem)]++;
	
	// �ŏ��̑҂����Ԃ�ݒ�
	now  = OSD_GetTicks();
	last = now;
	DWORD NextWait = now + Vint[VintCnt++];
	
	while( !this->cThread::IsCancel() ){
		now = OSD_GetTicks();
		if( now >= NextWait ){
			NextWait += Vint[VintCnt++];
			if( VintCnt >= VSYNC_HZ ){
				VintCnt -= VSYNC_HZ;
// �^�C�~���O�����̓T�E���h�X�V�X���b�h�Ɉړ�
//				ti->WaitReset();
			}
			// ��ʍX�V�t���O�𗧂Ă�
			EnableScrUpdate++;
		}else
			OSD_Delay( 0 );
		
		if( now - last >= WRUPDATE ){
			for( int i=SPDCNT-1; i>0; i-- )
				WRClock[i] = WRClock[i-1];
			WRClock[0] = 0;
			last       += WRUPDATE;
		}
	}
}


////////////////////////////////////////////////////////////////
// ����J�n
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
bool SCH6::Start( void )
{
	// �X���b�h����
	if( !this->cThread::BeginThread( this ) ) return false;
	return true;
}


////////////////////////////////////////////////////////////////
// �����~
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SCH6::Stop( void )
{
	this->cThread::Cancel();	// �X���b�h�I���t���O���Ă�
	this->cThread::Waiting();	// �X���b�h�I���܂ő҂�
	
	INITARRAY( WRClock, 0 );
}


////////////////////////////////////////////////////////////////
// Wait�L���t���O�擾
//
// ����:	�Ȃ�
// �Ԓl:	�L��:true ����:false
////////////////////////////////////////////////////////////////
bool SCH6::GetWaitEnable( void ) const
{
	return WaitEnable;
}


////////////////////////////////////////////////////////////////
// Wait�L���t���O�ݒ�
//
// ����:	en		Wait�L���t���O �L��:true ����:false
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SCH6::SetWaitEnable( bool en )
{
	WaitEnable = en;
}


////////////////////////////////////////////////////////////////
// �|�[�Y�L���t���O�擾
//
// ����:	�Ȃ�
// �Ԓl:	�L��:true ����:false
////////////////////////////////////////////////////////////////
bool SCH6::GetPauseEnable( void ) const
{
	return PauseEnable;
}


////////////////////////////////////////////////////////////////
// �|�[�Y�L���t���O�ݒ�
//
// ����:	en		�|�[�Y�L���t���O �L��:true ����:false
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SCH6::SetPauseEnable( bool en )
{
	PauseEnable = en;
}


////////////////////////////////////////////////////////////////
// VSYNC Wait
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SCH6::VWait( void )
{
	if( !WaitEnable ) return;
	
	SpeedCnt1++;
	
	if( (SpeedRatio > 100 ) && ( (SpeedCnt1 * 100) / SpeedCnt2 < SpeedRatio ) )
		return;
	
	cSemaphore::Wait();
}


////////////////////////////////////////////////////////////////
// Wait����������
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SCH6::WaitReset( void )
{
	if( (SpeedCnt1 > 0xffffff) || (SpeedCnt2 > 0xffffff) ){
		SpeedCnt1 /= 2;
		SpeedCnt2 /= 2;
	}
	
	SpeedCnt2++;
	
	if( (SpeedRatio < 100 ) && ( (SpeedCnt1 * 100) / SpeedCnt2 >= SpeedRatio ) )
		return;
	
	if( !cSemaphore::Value() ) cSemaphore::Post();
	
}


////////////////////////////////////////////////////////////////
// ���s���x�ݒ�
//
// ����:	spd		+:UP -:DOWN 0:����
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SCH6::SetSpeedRatio( int spd )
{
	if     ( spd > 0 && SpeedRatio < 2000 ) SpeedRatio += SpeedRatio <  200 ? 10 : 100;
	else if( spd < 0 && SpeedRatio >   10 ) SpeedRatio -= SpeedRatio <= 200 ? 10 : 100;
	else if( spd == 0 )                     SpeedRatio = 100;
	
	SpeedCnt2 = VSYNC_HZ * 1000;
	SpeedCnt1 = (SpeedCnt2 * SpeedRatio) / 100;
}


////////////////////////////////////////////////////////////////
// ���s���x�擾
//
// ����:	�Ȃ�
// �Ԓl:	int		���s���x(%)
////////////////////////////////////////////////////////////////
int SCH6::GetSpeedRatio( void ) const
{
	return SpeedRatio;
}


////////////////////////////////////////////////////////////////
// ���s���x��擾
//
// ����:	�Ȃ�
// �Ԓl:	int		���s���x��(%)(���{��100)
////////////////////////////////////////////////////////////////
int SCH6::GetRatio( void ) const
{
	DWORD sum = 0;
	
	for( int i=1; i<SPDCNT; i++ )
		sum += WRClock[i];
	
	return (int)(double)( sum * 100.0 / MasterClock * 1000.0 / (WRUPDATE*(SPDCNT-1)) + 0.5 );
}


////////////////////////////////////////////////////////////////
// �C�x���g�X�V
//
// ����:	clk		�i�߂�N���b�N��
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SCH6::Update( int clk )
{
	WRClock[0] += clk;	// ���s���x�v�Z�p
}


////////////////////////////////////////////////////////////////
// ��ʍX�V�������}����?
//
// ����:	�Ȃ�
// �Ԓl:	bool		true:�X�V���� false:�X�V���Ȃ�
////////////////////////////////////////////////////////////////
bool SCH6::IsScreenUpdate( void )
{
	if( EnableScrUpdate ){
		EnableScrUpdate--;
		return true;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// ** ���ǎg��Ȃ��Ȃ�܂��� **
// ��ʂ̕\����Ԃɉ�����CPU�X�e�[�g�����v�Z
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
// PC-6001mk2SR�̃}�j���A��������p (�Ȃ���SR? ���܂��܎茳�ɂ��ꂵ���Ȃ���������)

// �\�����g��
// ���� 15.70kHz(63.70usec)
// ���� 59.92 Hz(16.69msec)

// �����^�C�~���O (�P��:usec)
// �\�����[�h �������� �o�b�N�|�[�` �\������ �t�����g�|�[�`  �v
// 32����		4.87	13.74		35.76		9.33		63.70
// 40,80����	4.87	 9.27		44.69		4.86		63.69

// �����^�C�~���O (�P��:msec)
// �\�����[�h �������� �o�b�N�|�[�` �\������ �t�����g�|�[�`  �v
// 192H			0.25	 2.49		12.23		1.72		16.69
// 200H			0.25	 2.23		12.74		1.47		16.69
// 204H			0.25	 2.11		12.99		1.34		16.69

// �h�b�g�N���b�N,���C�����Œu��������
// �����^�C�~���O
// �\�����[�h �������� �o�b�N�|�[�` �\������ �t�����g�|�[�`  �v
// 32����		34			98			256		67			455
// 40,80����	34	 		66			320		35			455

// �����^�C�~���O
// �\�����[�h �������� �o�b�N�|�[�` �\������ �t�����g�|�[�`  �v
// 192H			3			34			192		33			262
// 200H			3		 	30			200		29			262
// 204H			4(?)		33			204		21			262


// �܂Ƃ߂�Ƃ���Ȋ���
// �����g�[�^������   455 Clk (�h�b�g�N���b�N 7.16MHz? 3.58MHz�̗��G�b�W?)
// �����\������       256 320 Clk
// ������\������     199 135 Clk
// �����g�[�^�����C�� 262 Line
// �����\�����C��     192 200 204 Line
// ������\�����C��    70  62  58 Line

// [�\������]�̂�CPU��~�ƍl����ƁC�ғ����͂���Ȋ���
// �ł�SR�͂���ȂɎ~�܂�Ȃ��炵��
// N60   : 58.769% ( 199*192 + 455*70 ) / ( 262*455 )
// N60m  : 46.313% ( 135*200 + 455*62 ) / ( 262*455 )
// N66SR : 45.239% ( 135*204 + 455*58 ) / ( 262*455 )

// CPU�N���b�N�ōl�����
// CPU�N���b�N/s      3993600
// CPU�N���b�N/VSYNC    66560 (VSYNC:60Hz�Ƃ���)
// CPU�N���b�N/1Line     1109 (66560/262)
//   �ғ�:��~/1Line  486:623 331:778
// CPU�N���b�N/VBlank  146368  155240

/*
#define	Ref1	(double)( (double)( 199*192 + 455*70 ) / (double)( 262*455 ) )
#define	Ref2	(double)( (double)( 135*200 + 455*62 ) / (double)( 262*455 ) )

void SCH6::CalcCpuState( void )
{
	bool disp = vm->vdg->GetCrtDisp();
	bool win  = vm->vdg->GetWinSize();
	
	// 1�b������̃X�e�[�g�������߂�
	int CpuState;
	if( disp ) CpuState = (int)( (win ? Ref1 : Ref2) * (double)CpuClock );
	else       CpuState = CpuClock;

CpuState = CpuClock;
	
	// ���̂܂܂��Ƒ�������̂œK���ɒ���
//	CpuState = CpuState * 86 / 100;
	
	// �}�X�^�N���b�N����ݒ�
	EVSC::SetMasterClock( CpuState );
}
*/

