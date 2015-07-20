#include <time.h>

#include "config.h"
#include "log.h"
#include "cpus.h"
#include "intr.h"
#include "schedule.h"

#include "p6vm.h"


// �C�x���gID
#define	EID_INTCHK	(1)
#define	EID_VECTOR	(2)
#define	EID_DATA	(3)

// �E�F�C�g(us)
#define	WAIT_INTCHK	(300)
//#define	WAIT_INTCHK	(30)
// �E�F�C�g(Z80�X�e�[�g��)
#define	WAIT_DATA	(15)
#define	WAIT_JOY	(9225)

// 8049����X�e�[�^�X
#define	D8049_IDLE		(0)				/* �������Ă��Ȃ� */
#define	D8049_IMASK		(0xff00)		/* �O�������݃X�e�[�^�X�}�X�N */
#define	D8049_INTR		(0x0100)		/* �O�������ݗv�� */

#define	D8049_CMTO		(0x0200)		/* CMT 1�����o�� �f�[�^�҂� */
#define	D8049_TVRW		(0x0300)		/* TV�\�񏑍��� �f�[�^�҂� */

#define	D8049_KEY1		(0x0001)		/* �L�[������1����1 */
#define	D8049_KEY12		(0x0002)		/* �L�[������1����2 */
#define	D8049_KEY2		(0x0003)		/* �L�[������2 */
#define	D8049_KEY3		(0x0004)		/* �L�[������3 */
#define	D8049_CMTR		(0x0005)		/* CMT READ������ */
#define	D8049_CMTE		(0x0006)		/* CMT ERROR������ */
#define	D8049_SIO		(0x0007)		/* RS232C��M������ */
#define	D8049_JOY		(0x0008)		/* �Q�[���p�L�[������ */
#define	D8049_TVRR		(0x0009)		/* TV�\��Ǎ��݊����� */
#define	D8049_DATE		(0x000a)		/* DATE������ */

#define	D8049_NODATA	(0x000f)		/* �f�[�^�o�͂Ȃ� */

// �����ݗv���t���O
#define	IR_KEY1			(0x0001)
#define	IR_KEY12		(0x0002)
#define	IR_CMTR			(0x0004)
#define	IR_CMTE			(0x0008)
#define	IR_KEY2			(0x0010)
#define	IR_KEY3			(0x0020)
#define	IR_SIO			(0x0040)
#define	IR_JOY			(0x0080)
#define	IR_TVR			(0x0100)
#define	IR_DATE			(0x0200)

// �����݃x�N�^
#define	IVEC_NOINTR		(0xff)
#define	IVEC_KEY3		(2)
#define	IVEC_SIO		(4)
#define	IVEC_TIMER		(6)
#define	IVEC_CMT_R		(8)
#define	IVEC_KEY1		(14)
#define	IVEC_KEY12		(16)
#define	IVEC_CMT_E		(18)
#define	IVEC_KEY2		(20)
#define	IVEC_JOY		(22)
#define	IVEC_TVR		(24)
#define	IVEC_DATE		(26)


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
SUB6::SUB6( VM6 *vm, const ID& id ) : Device(vm,id),
	CmtStatus(CMTCLOSE), Status8049(D8049_IDLE),
	IntrFlag(0), KeyCode(0), JoyCode(0), CmtData(0), SioData(0),
	TVRCnt(0), DateCnt(0)
{
	INITARRAY( TVRData,  0xff );
	TVRData[0] = 0xa6;
	INITARRAY( DateData, 0 );
}

SUB60::SUB60( VM6 *vm, const ID& id ) : SUB6(vm,id){}
SUB62::SUB62( VM6 *vm, const ID& id ) : SUB60(vm,id){}
SUB68::SUB68( VM6 *vm, const ID& id ) : SUB62(vm,id){}

////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
SUB6::~SUB6( void ){}
SUB60::~SUB60( void ){}
SUB62::~SUB62( void ){}
SUB68::~SUB68( void ){}


////////////////////////////////////////////////////////////////
// �C�x���g�R�[���o�b�N�֐�
//
// ����:	id		�C�x���gID
//			clock	�N���b�N
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SUB6::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_INTCHK:	// �����݃`�F�b�N
		
		if( (Status8049 == D8049_IDLE) && IntrFlag ){
			int wt = WAIT_DATA;
			
			PRINTD( SUB_LOG, "[8049][EventCallback] EID_INTCHK %02X->", Status8049 );
			
			// �����ݗv���𒲂ׂ�
			if     ( IntrFlag & IR_JOY )  { Status8049 = D8049_JOY;   wt += WAIT_JOY; }
			else if( IntrFlag & IR_KEY1 ) { Status8049 = D8049_KEY1;  }
			else if( IntrFlag & IR_KEY12 ){ Status8049 = D8049_KEY12; }
			else if( IntrFlag & IR_CMTR ) { Status8049 = D8049_CMTR;  }
			else if( IntrFlag & IR_CMTE ) { Status8049 = D8049_CMTE;  }
			else if( IntrFlag & IR_KEY2 ) { Status8049 = D8049_KEY2;  }
			else if( IntrFlag & IR_KEY3 ) { Status8049 = D8049_KEY3;  }
			else if( IntrFlag & IR_SIO )  { Status8049 = D8049_SIO;   }
			else if( IntrFlag & IR_TVR )  { Status8049 = D8049_TVRR;  }
			else if( IntrFlag & IR_DATE ) { Status8049 = D8049_DATE;  }
			
			if( Status8049 != D8049_IDLE )
				vm->EventAdd( this, EID_VECTOR, wt, EV_LOOP|EV_STATE );
			
			PRINTD( SUB_LOG, "%02X\n", Status8049 );
		}
		break;
		
	case EID_VECTOR:	// �����݃x�N�^�o��
		// T0=IBF=H �܂�8255�̃o�b�t�@���󂢂Ă��Ȃ���΃��g���C
		if( GetT0() ) break;
		vm->EventDel( this, EID_VECTOR );
		
		OutVector();	// �����݃x�N�^�o��
		
		// �����ăf�[�^�o��
		vm->EventAdd( this, EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
		break;
		
	case EID_DATA:		// �����݃f�[�^�o��
		// T0=IBF=H �܂�8255�̃o�b�t�@���󂢂Ă��Ȃ���΃��g���C
		if( GetT0() ) break;
		vm->EventDel( this, EID_DATA );
		
		OutData();		// �����݃f�[�^�o��
		break;
	}
}


////////////////////////////////////////////////////////////////
// ���Z�b�g
////////////////////////////////////////////////////////////////
void SUB6::Reset( void )
{
	IntrFlag   = 0;
	Status8049 = D8049_IDLE;
	CmtStatus  = CMTCLOSE;
	
	KeyCode    = 0;
	JoyCode    = 0;
	CmtData    = 0;
	SioData    = 0;
	
	vm->EventAdd( this, EID_INTCHK, WAIT_INTCHK, EV_LOOP|EV_US );
}


////////////////////////////////////////////////////////////////
// I/O�|�[�g�A�N�Z�X
////////////////////////////////////////////////////////////////
BYTE SUB6::ReadIO( int addr )
{
	return vm->IosIn( addr );
}

void SUB6::WriteIO( int addr, BYTE data )
{
	vm->IosOut( addr, data );
}


////////////////////////////////////////////////////////////////
// �O���������A�N�Z�X
////////////////////////////////////////////////////////////////
void SUB6::WriteExt( BYTE data )
{
	vm->IosOut( IO8049_BUS, data );
}

BYTE SUB6::ReadExt( void )
{
	return vm->IosIn( IO8049_BUS );
}


////////////////////////////////////////////////////////////////
// �e�X�g0�X�e�[�^�X�擾
////////////////////////////////////////////////////////////////
bool SUB6::GetT0( void )
{
	return vm->IosIn( IO8049_T0 ) ? true : false;
}


////////////////////////////////////////////////////////////////
// �e�X�g1�X�e�[�^�X�擾
////////////////////////////////////////////////////////////////
bool SUB6::GetT1( void )
{
	return 0;
}


////////////////////////////////////////////////////////////////
// �O�������݃X�e�[�^�X�擾
////////////////////////////////////////////////////////////////
bool SUB6::GetINT( void )
{
	return vm->IosIn( IO8049_INT ) ? true : false;
}


////////////////////////////////////////////////////////////////
// �O�������ݗv��
////////////////////////////////////////////////////////////////
void SUB6::ExtIntr( void )
{
	// �O������������(8255��PortA)
	BYTE comm = ReadExt();
	
	PRINTD( SUB_LOG, "[8049][ExtIntr] -> " );
	
	// �O�������ݏ�����?
	switch( Status8049 ){
	case D8049_CMTO:	// CMT 1�����o��
		PRINTD( SUB_LOG, "CmtPut %02X\n", comm );
		
		vm->CmtsCmtWrite( comm );
		Status8049 = D8049_IDLE;
		break;
		
	case D8049_TVRW:	// TVR ������
		PRINTD( SUB_LOG, "TV reserve write %02X\n", comm );
		
		TVRData[TVRCnt++] = comm;
		if( comm == 0xff || TVRCnt >= (int)sizeof(TVRData) )
			Status8049 = D8049_IDLE;
		break;
		
	default:			// �q�}�Ȃ�V���ȊO�������ݎ�t
		PRINTD( SUB_LOG, "Command %02X\n", comm );
		ExtIntrExec( comm );
		// �{���̓x�N�^(+�f�[�^)�o�͒��͊O�������݋֎~
		// ��ōl����C����

//	case D8049_IDLE:	// �q�}�Ȃ�V���ȊO�������ݎ�t
//		PRINTD( SUB_LOG, "Command %02X\n", comm );
		
//		ExtIntrExec( comm );
//		break;
		
//	default:			// ���炩������
//		PRINTD( SUB_LOG, " %02X pass\n", comm );
	}
}


////////////////////////////////////////////////////////////////
// �O�������ݏ���
////////////////////////////////////////////////////////////////
void SUB6::ExtIntrExec( BYTE comm )
{
	// �R�}���h
	switch( comm ){
	case 0x0c:	// ------ RS-232C ----------
	case 0x2c:
		break;
		
	case 0x06:	// ------ STICK,STRIG ----------
		// �Q�[���p�L�[�����ݗv��
		ReqJoyIntr();
		break;
		
	case 0x19:	// ------ CMT(LOAD) OPEN ----------
		CmtStatus = LOADOPEN;
		break;
		
	case 0x1a:	// ------ CMT(LOAD) CLOSE ----------
		CmtStatus = CMTCLOSE;
		break;
		
	case 0x1d:	// ------ CMT(LOAD) 600�{�[ ----------
//		vm->cmtl->SetBaud( 600 );
		break;
		
	case 0x1e:	// ------ CMT(LOAD) 1200�{�[ ----------
//		vm->cmtl->SetBaud( 1200 );
		break;
		
	case 0x38:	// ------ CMT 1�����o�� ----------
		Status8049 |= D8049_CMTO;
		break;
		
	case 0x39:	// ------ CMT(SAVE) OPEN ----------
		vm->CmtsMount();
		CmtStatus = SAVEOPEN;
		break;
		
	case 0x3a:	// ------ CMT(SAVE) CLOSE ----------
		CmtStatus = CMTCLOSE;
		vm->CmtsUnmount();
		break;
		
	case 0x3d:	// ------ CMT(SAVE) 600�{�[ ----------
		vm->CmtsSetBaud( 600 );
		break;
		
	case 0x3e:	// ------ CMT(SAVE) 1200�{�[ ----------
		vm->CmtsSetBaud( 1200 );
		break;
	}
}

void SUB62::ExtIntrExec( BYTE comm )
{
	// �R�}���h
	switch( comm ){
	case 0x04:	// ------ �p��<->���Ȑ؊� ----------
		vm->KeyChangeKana();
		break;
		
	case 0x05:	// ------ ����<->�J�i�؊� ----------
		vm->KeyChangeKKana();
		break;
		
	default:
		SUB6::ExtIntrExec( comm );
	}
}

void SUB68::ExtIntrExec( BYTE comm )
{
	// �R�}���h
	switch( comm ){
	case 0x30:	// ------ TV�\�񏑍��� ----------
		Status8049 |= D8049_TVRW;
		TVRCnt = 0;
		break;
		
	case 0x31:	// ------ TV�\��Ǎ��� ----------
		ReqTVRReadIntr();
		break;
		
	case 0x32:	// ------ DATE ----------
		ReqDateIntr();
		break;
		
	default:
		SUB62::ExtIntrExec( comm );
	}
}


////////////////////////////////////////////////////////////////
// �L�[�����ݗv��
////////////////////////////////////////////////////////////////
void SUB6::ReqKeyIntr( int flag, BYTE data )
{
	// flag
	// bit 0 : ON_STOP
	// bit 1 : ON_GRAPH
	// bit 2 : ON_FUNC
	
	// �O��̃L�[�����݂��������������疳��
	if( IntrFlag & ( IR_KEY1 | IR_KEY12 | IR_KEY2 | IR_KEY3 ) ) return;
	
	if( flag&1 ){	// STOP ?
		// CMT OPEN ?
		if( CmtStatus ){
			PRINTD( SUB_LOG, "[8049][ReqKeyIntr] KEY1 03\n" );
			IntrFlag |= IR_KEY1;
			KeyCode   = 0x03;
		}else{
			PRINTD( SUB_LOG, "[8049][ReqKeyIntr] KEY2(STOP) FA\n" );
			IntrFlag |= IR_KEY2;
			KeyCode   = 0xfa;
		}
	}else{			// STOP�ȊO
		// GRAPH or FUNCTION KEY ?
		if( flag&6 ){
			PRINTD( SUB_LOG, "[8049][ReqKeyIntr] KEY2 %02X\n", data );
			IntrFlag |= IR_KEY2;
			KeyCode   = data;
		}else{
			PRINTD( SUB_LOG, "[8049][ReqKeyIntr] KEY3 %02X\n", data );
			IntrFlag |= IR_KEY3;
			KeyCode   = data;
		}
	}
}


////////////////////////////////////////////////////////////////
// CMT READ�����ݗv��
////////////////////////////////////////////////////////////////
void SUB6::ReqCmtIntr( BYTE data )
{
	PRINTD( SUB_LOG, "[8049][ReqCmtIntr]\n" );
	
	// LOADOPEN�Ȃ�CMT�����ݔ���
	if( CmtStatus == LOADOPEN ){
		// �O���CMT READ�����݂��������������疳��
		if( IntrFlag & IR_CMTR ) return;
		
		IntrFlag |= IR_CMTR;
		CmtData   = data;
	}
}


////////////////////////////////////////////////////////////////
// �Q�[���p�L�[�����ݗv��
////////////////////////////////////////////////////////////////
void SUB6::ReqJoyIntr( void )
{
	PRINTD( SUB_LOG, "[8049][ReqJoyIntr]\n" );
	
	// �O��̃Q�[���p�L�[�����݂��������������疳��
	if( IntrFlag & IR_JOY ) return;
	
	IntrFlag |= IR_JOY;
	JoyCode   = vm->KeyGetKeyJoy();
}


////////////////////////////////////////////////////////////////
// TV�\��Ǎ��݊����ݗv��
////////////////////////////////////////////////////////////////
void SUB68::ReqTVRReadIntr( void )
{
	PRINTD( SUB_LOG, "[8049][ReqTVRReadIntr]\n" );
	
	// �O���TV�\��Ǎ��݊����݂��������������疳��
	if( IntrFlag & IR_TVR ) return;
	
	IntrFlag |= IR_TVR;
	TVRCnt    = 0;
}


////////////////////////////////////////////////////////////////
// DATE�����ݗv��
////////////////////////////////////////////////////////////////
void SUB68::ReqDateIntr( void )
{
	PRINTD( SUB_LOG, "[8049][ReqDateIntr]\n" );
	
	// �O���DATE�����݂��������������疳��
	if( IntrFlag & IR_DATE ) return;
	
	IntrFlag |= IR_DATE;
	DateCnt   = 0;
	
	time_t timer;
	tm *t_st;
	
	time( &timer );
	t_st = localtime( &timer );
	if( !t_st->tm_wday ) t_st->tm_wday = 7;	// �j���␳
	PRINTD( SUB_LOG, "Localtime : %d/%d/%d(%d) %d:%d:%d\n",t_st->tm_year+1900,t_st->tm_mon+1,t_st->tm_mday,t_st->tm_wday-1,t_st->tm_hour,t_st->tm_min,t_st->tm_sec );
	
	DateData[0] = ((t_st->tm_mon  +  1)<<4) | (t_st->tm_wday -  1);
	DateData[1] = ((t_st->tm_mday / 10)<<4) | (t_st->tm_mday % 10);
	DateData[2] = ((t_st->tm_hour / 10)<<4) | (t_st->tm_hour % 10);
	DateData[3] = ((t_st->tm_min  / 10)<<4) | (t_st->tm_min  % 10);
	DateData[4] = ((t_st->tm_sec  / 10)<<4) | (t_st->tm_sec  % 10);
}


////////////////////////////////////////////////////////////////
// �����݃x�N�^�o��
////////////////////////////////////////////////////////////////
void SUB6::OutVector( void )
{
	BYTE IntrVector = IVEC_NOINTR;
	
	switch( Status8049 ){
	case D8049_JOY:		// �Q�[���p�L�[������ �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector JOY]\n" );
		IntrVector = IVEC_JOY;
		break;
		
	case D8049_KEY1:	// �L�[������1 ����1 �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector KEY1]\n" );
		IntrVector = IVEC_KEY1;
		IntrFlag  &= ~IR_KEY1;
		Status8049 = D8049_NODATA;
		break;
		
	case D8049_KEY12:	// �L�[������1 ����2 �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector KEY12]\n" );
		IntrVector = IVEC_KEY12;
		IntrFlag  &= ~IR_KEY12;
		Status8049 = D8049_NODATA;
		break;
		
	case D8049_CMTR:	// CMT READ������ �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector CMTR]\n" );
		IntrVector = IVEC_CMT_R;
		break;
		
	case D8049_CMTE:	// CMT ERROR������ �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector CMTE]\n" );
		IntrVector = IVEC_CMT_E;
		IntrFlag  &= ~IR_CMTE;
		Status8049 = D8049_NODATA;
		break;
		
	case D8049_KEY2:	// �L�[������2 �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector KEY2]\n" );
		IntrVector = IVEC_KEY2;
		break;
		
	case D8049_KEY3:	// �L�[������3 �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector KEY3]\n" );
		IntrVector = IVEC_KEY3;
		break;
		
	case D8049_SIO:		// RS232C��M������ �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector SIO]\n" );
		IntrVector = IVEC_SIO;
		break;
		
	case D8049_TVRR:	// TV�\�񊄍��� �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector TVR]\n" );
		IntrVector = IVEC_TVR;
		break;
		
	case D8049_DATE:	// DATE������ �����݃x�N�^�o��
		PRINTD( SUB_LOG, "[OutVector DATE]\n" );
		IntrVector = IVEC_DATE;
		break;
		
	default:			// �ǂ�ł��Ȃ���΃x�N�^�o�͂Ȃ�
		return;
	}
	
	WriteExt( IntrVector );
	vm->IntReqIntr( IREQ_8049 );
}


////////////////////////////////////////////////////////////////
// �����݃f�[�^�o��
////////////////////////////////////////////////////////////////
void SUB6::OutData( void )
{
	switch( Status8049 ){
	case D8049_JOY:		// �Q�[���p�L�[������ �f�[�^�o��
		PRINTD( SUB_LOG, "[OutData JOY] Data: %02X\n", JoyCode );
		WriteExt( JoyCode );
		IntrFlag &= ~IR_JOY;
		break;
		
	case D8049_KEY1:	// �L�[������1 ����1 �����݃x�N�^�o��
		IntrFlag &= ~IR_KEY1;
		break;
		
	case D8049_KEY12:	// �L�[������1 ����2 �����݃x�N�^�o��
		IntrFlag &= ~IR_KEY12;
		break;
		
	case D8049_CMTR:	// CMT READ������ �f�[�^�o��
		PRINTD( SUB_LOG, "[OutData CMTR] Data: %02X\n", CmtData );
		WriteExt( CmtData );
		IntrFlag &= ~IR_CMTR;
		break;
		
	case D8049_CMTE:	// CMT ERROR������ �����݃x�N�^�o��
		IntrFlag &= ~IR_CMTE;
		break;
		
	case D8049_KEY2:	// �L�[������2 �f�[�^�o��
		PRINTD( SUB_LOG, "[OutData KEY2] Data: %02X\n", KeyCode );
		WriteExt( KeyCode );
		IntrFlag &= ~IR_KEY2;
		break;
		
	case D8049_KEY3:	// �L�[������3 �f�[�^�o��
		PRINTD( SUB_LOG, "[OutData KEY3] Data: %02X\n", KeyCode );
		WriteExt( KeyCode );
		IntrFlag &= ~IR_KEY3;
		break;
		
	case D8049_SIO:		// RS232C��M������ �f�[�^�o��
		PRINTD( SUB_LOG, "[OutData SIO] Data: %02X\n", SioData );
		WriteExt( SioData );
		IntrFlag &= ~IR_SIO;
		break;
		
	case D8049_TVRR:{	// TV�\��Ǎ��݊����� �f�[�^�o��
		BYTE tvd = TVRCnt >= (int)sizeof(TVRData) ? 0xff : TVRData[TVRCnt++];
		PRINTD( SUB_LOG, "[OutData TVR] Data: %02X\n", tvd );
		WriteExt( tvd );
		if( tvd == 0xff ){	// FFH�Ȃ�I��
			IntrFlag &= ~IR_TVR;
		}else{				// FFH�ȊO�Ȃ�c��̃f�[�^�o��
			vm->EventAdd( this, EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
			return;
		}
		break;
	}
	case D8049_DATE:	// DATE������ �f�[�^�o��
		PRINTD( SUB_LOG, "[OutData DATE] Data%d: %02X\n", DateCnt, DateData[DateCnt] );
		WriteExt( DateData[DateCnt++] );
		if( DateCnt > 4 ){	// 5��o�͂�����I��
			IntrFlag &= ~IR_DATE;
		}else{				// 5�񖢖��Ȃ�c��̃f�[�^�o��
			vm->EventAdd( this, EID_DATA, WAIT_DATA, EV_LOOP|EV_STATE );
			return;
		}
		break;
		
	}
	
	// �����ݏ����I��
	Status8049 = D8049_IDLE;
	// CPU�ɑ΂��銄���ݗv�����L�����Z��(�����݋֎~�΍�)
	vm->IntCancelIntr( IREQ_8049 );
}


////////////////////////////////////////////////////////////////
// CMT�X�e�[�^�X�擾
////////////////////////////////////////////////////////////////
int SUB6::GetCmtStatus( void ) const
{
	return CmtStatus;
}


////////////////////////////////////////////////////////////////
// CMT�����ݔ�����?
////////////////////////////////////////////////////////////////
bool SUB6::IsCmtIntrReady( void )
{
	// �������Ă��Ȃ���LOADOPEN�őO���CMT READ�����݂������ς݂�IBF=L�������甭����
	
	return ( Status8049 == D8049_IDLE ) && ( CmtStatus == LOADOPEN ) &&
			!( IntrFlag & IR_CMTR ) && !GetT0();
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�SAVE
////////////////////////////////////////////////////////////////
bool SUB6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "8049", NULL, "CmtStatus",	"%d",		CmtStatus );
	Ini->PutEntry( "8049", NULL, "Status8049",	"%d",		Status8049 );
	Ini->PutEntry( "8049", NULL, "IntrFlag",	"0x%08X",	IntrFlag );
	Ini->PutEntry( "8049", NULL, "KeyCode",		"0x%02X",	KeyCode );
	Ini->PutEntry( "8049", NULL, "JoyCode",		"0x%02X",	JoyCode );
	Ini->PutEntry( "8049", NULL, "CmtData",		"0x%02X",	CmtData );
	Ini->PutEntry( "8049", NULL, "SioData",		"0x%02X",	SioData );
	Ini->PutEntry( "8049", NULL, "TVRCnt",		"0x%02X",	TVRCnt );
	Ini->PutEntry( "8049", NULL, "DateCnt",		"0x%02X",	DateCnt );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD
////////////////////////////////////////////////////////////////
bool SUB6::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt(   "8049", "CmtStatus",		&CmtStatus,		CmtStatus );
	Ini->GetInt(   "8049", "Status8049",	&Status8049,	Status8049 );
	Ini->GetInt(   "8049", "IntrFlag",		&st,			IntrFlag );	IntrFlag = st;
	Ini->GetInt(   "8049", "KeyCode",		&st,			KeyCode );	KeyCode  = st;
	Ini->GetInt(   "8049", "JoyCode",		&st,			JoyCode );	JoyCode  = st;
	Ini->GetInt(   "8049", "CmtData",		&st,			CmtData );	CmtData  = st;
	Ini->GetInt(   "8049", "SioData",		&st,			SioData );	SioData  = st;
	Ini->GetInt(   "8049", "TVRCnt",		&st,			TVRCnt );	TVRCnt   = st;
	Ini->GetInt(   "8049", "DateCnt",		&st,			DateCnt );	DateCnt  = st;
	
	return true;
}
