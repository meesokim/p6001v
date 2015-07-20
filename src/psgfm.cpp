#include "pc6001v.h"

#include "log.h"
#include "osd.h"
#include "psgfm.h"
#include "schedule.h"

#include "p6el.h"
#include "p6vm.h"

// �C�x���gID
#define	EID_PSG		(1)
#define	EID_TIMERA	(2)
#define	EID_TIMERB	(3)

// �|�[�g�A�N�Z�X�E�F�C�g�N���b�N��
#define PAWAIT		(1)


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
PSGb::PSGb( VM6 *vm, const ID& id ) : Device(vm,id), JoyNo(0), Clock(0) {}
PSG6::PSG6( VM6 *vm, const ID& id ) : PSGb(vm,id) {}
OPN6::OPN6( VM6 *vm, const ID& id ) : PSGb(vm,id) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
PSGb::~PSGb( void ){}
PSG6::~PSG6( void ){}
OPN6::~OPN6( void ){}


////////////////////////////////////////////////////////////////
// �C�x���g�R�[���o�b�N�֐�
//
// ����:	id		�C�x���gID
//			clock	�N���b�N
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void PSGb::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_PSG:
		break;
	}
}


void OPN6::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_PSG:
		break;
	
	case EID_TIMERA:
	case EID_TIMERB:
		cYM2203::TimerIntr();
		break;
	}
}


////////////////////////////////////////////////////////////////
// �X�V�T���v�����擾
//   �኱�̌덷���܂ނ����p��(����)���Ȃ�
////////////////////////////////////////////////////////////////
int PSGb::GetUpdateSamples( void )
{
	int samples = (int)( (double)SndDev::SampleRate * vm->EventGetProgress( this->Device::GetID(), EID_PSG ) + 0.5 );
	vm->EventReset( this->Device::GetID(), EID_PSG );
	return samples;
}


////////////////////////////////////////////////////////////////
// ���W�X�^�ύX�O�̃X�g���[���X�V
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void PSG6::PreWriteReg( void )
{
	SoundUpdate( 0 );
}


void OPN6::PreWriteReg( void )
{
	SoundUpdate( 0 );
}


////////////////////////////////////////////////////////////////
// TimerA�ݒ�
////////////////////////////////////////////////////////////////
void OPN6::SetTimerA( int cnt )
{
	double ct = 72. * (1024. - (double)cnt) / (double)Clock * 1000000.;
	
	if( cnt ) vm->EventAdd( this, EID_TIMERA, ct, EV_LOOP|EV_US );
	else	  vm->EventDel( this, EID_TIMERA );
}


////////////////////////////////////////////////////////////////
// TimerB�ݒ�
////////////////////////////////////////////////////////////////
void OPN6::SetTimerB( int cnt )
{
	double ct = 1152. * (256. - (double)cnt) / (double)Clock * 1000000.;
	
	if( cnt ) vm->EventAdd( this, EID_TIMERB, ct, EV_LOOP|EV_US );
	else	  vm->EventDel( this, EID_TIMERB );
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool PSG6::Init( int clock, int srate )
{
	PRINTD( PSG_LOG, "[PSG][Init]\n" );
	
	Clock = clock;
	
	// PSG�N���b�N�ݒ�
	cAY8910::SetClock( Clock, srate );
	
	// PSG �{�����[���e�[�u���ݒ�
	cAY8910::SetVolumeTable( DEFAULT_PSGVOL );
	
	// ���Z�b�g
	cAY8910::Reset();
	
	// ���Ȃ��Ƃ�1�b��1�񂭂炢�͍X�V���邾�낤�Ƃ����O��
	if( !vm->EventAdd( this, EID_PSG, 1000, EV_LOOP|EV_MS ) ) return false;
	
	return SndDev::Init( srate );
}


bool OPN6::Init( int clock, int srate )
{
	PRINTD( PSG_LOG, "[OPN][Init]\n" );
	
	// ���̂��N���b�N2�{
	Clock = clock * 2;
	
	// OPN�N���b�N�ݒ�
	cYM2203::Init( Clock, srate );
	
	// OPN �{�����[���e�[�u���ݒ�
	cYM2203::SetVolumeTable( DEFAULT_PSGVOL );
	
	// ���Z�b�g
	cYM2203::Reset();
	
	// ���Ȃ��Ƃ�1�b��1�񂭂炢�͍X�V���邾�낤�Ƃ����O��
	if( !vm->EventAdd( this, EID_PSG, 1000, EV_LOOP|EV_MS ) ) return false;
	
	return SndDev::Init( srate );
}



////////////////////////////////////////////////////////////////
// �T���v�����O���[�g�ݒ�
//
// ����:	rate	�T���v�����O���[�g
//			size	�o�b�t�@�T�C�Y
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool PSG6::SetSampleRate( int rate, int size )
{
	cAY8910::SetClock( Clock, rate );
	
	return SndDev::SetSampleRate( rate, size );
}


bool OPN6::SetSampleRate( int rate, int size )
{
	cYM2203::SetClock( Clock, rate );
	
	return SndDev::SetSampleRate( rate, size );
}


////////////////////////////////////////////////////////////////
// �X�g���[���X�V
//
// ����:	samples	�X�V�T���v����(-1:�c��o�b�t�@�S�� 0:�����N���b�N��)
// �Ԓl:	int		�X�V�T���v����
////////////////////////////////////////////////////////////////
int PSG6::SoundUpdate( int samples )
{
	PRINTD( PSG_LOG, "[PSG][SoundUpdate] Samples: %d(%d)", samples, SndDev::cRing::FreeSize() );
	
	int length = 0;
	
	if( samples == 0 ){
		int sam = GetUpdateSamples();
		length = min( sam, SndDev::cRing::FreeSize() );
	}else if( samples > 0 ) length = min( samples, SndDev::cRing::FreeSize() );
	else					length = SndDev::cRing::FreeSize();
	
	PRINTD( PSG_LOG, " -> %d\n", length );
	
	if( length <= 0 ) return 0;
	
	
	// buffering loop
	for( int i=0; i<length; i++ ){
		// �o�b�t�@�ɏ�����
		SndDev::cRing::Put( ( this->Update1Sample() * SndDev::Volume ) / 100 );
	}
	
	return length;
}


int OPN6::SoundUpdate( int samples )
{
//	PRINTD( PSG_LOG, "[OPN][SoundUpdate] Samples: %d(%d)", samples, SndDev::cRing::FreeSize() );
	
	int length = 0;
	
	if( samples == 0 ){
		int sam = GetUpdateSamples();
		length = min( sam, SndDev::cRing::FreeSize() );
	}else if( samples > 0 ) length = min( samples, SndDev::cRing::FreeSize() );
	else					length = SndDev::cRing::FreeSize();
	
//	PRINTD( PSG_LOG, " -> %d\n", length );
	
	if( length <= 0 ) return 0;
	
	
	// buffering loop
	for( int i=0; i<length; i++ ){
		// �o�b�t�@�ɏ�����
		SndDev::cRing::Put( ( this->Update1Sample() * SndDev::Volume ) / 100 );
	}
	
	return length;
}


////////////////////////////////////////////////////////////////
// �|�[�g�A�N�Z�X�֐�
////////////////////////////////////////////////////////////////
BYTE PSG6::PortAread( void ){ return vm->KeyGetJoy( JoyNo ); }
void PSG6::PortBwrite( BYTE data ){ JoyNo = (~data>>6)&1; }

BYTE OPN6::PortAread( void ){ return vm->KeyGetJoy( JoyNo ); }
void OPN6::PortBwrite( BYTE data ){ JoyNo = (~data>>6)&1; }


////////////////////////////////////////////////////////////////
// I/O�A�N�Z�X�֐�
////////////////////////////////////////////////////////////////
// PSG���W�X�^�A�h���X���b�`
void PSG6::OutA0H( int, BYTE data )
{
	this->WriteReg( 0, data );
}

// PSG���C�g�f�[�^
void PSG6::OutA1H( int, BYTE data )
{
	this->WriteReg( 1, data );
}

// PSG�C���A�N�e�B�u
void PSG6::OutA3H( int, BYTE data ){}

// PSG���[�h�f�[�^
BYTE PSG6::InA2H( int )
{
	return this->ReadReg();
}


// OPN���W�X�^�A�h���X���b�`
void OPN6::OutA0H( int, BYTE data )
{
	this->WriteReg( 0, data );
}

// OPN���C�g�f�[�^
void OPN6::OutA1H( int, BYTE data )
{
	this->WriteReg( 1, data );
}

// OPN�C���A�N�e�B�u
void OPN6::OutA3H( int, BYTE data ){}

// OPN���[�h�f�[�^
BYTE OPN6::InA2H( int )
{
	return this->ReadReg();
}

// OPN�X�e�[�^�X���[�h
BYTE OPN6::InA3H( int )
{
	return this->ReadStatus();
}



////////////////////////////////////////////////////////////////
// �ǂ��ł�SAVE
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool PSG6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	// cAY8910
	if( !cAY8910::DokoSave( Ini ) ) return false;
	
	Ini->PutEntry( "PSG", NULL, "BufSize",		"%d",		SndDev::FreeSize() );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool PSG6::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	// cAY8910
	if( !cAY8910::DokoLoad( Ini ) ) return false;
	
	Ini->GetInt( "PSG", "BufSize",		&st,            SndDev::FreeSize() );
	while( SndDev::FreeSize() > st )
		SndDev::Put(0);
	
	return true;
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor PSG6::descriptor = {
	PSG6::indef, PSG6::outdef
};

const Device::OutFuncPtr PSG6::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &PSG6::OutA0H ),
	STATIC_CAST( Device::OutFuncPtr, &PSG6::OutA1H ),
	STATIC_CAST( Device::OutFuncPtr, &PSG6::OutA3H )
};

const Device::InFuncPtr PSG6::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &PSG6::InA2H )
};

const Device::Descriptor OPN6::descriptor = {
	OPN6::indef, OPN6::outdef
};

const Device::OutFuncPtr OPN6::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &OPN6::OutA0H ),
	STATIC_CAST( Device::OutFuncPtr, &OPN6::OutA1H ),
	STATIC_CAST( Device::OutFuncPtr, &OPN6::OutA3H )
};

const Device::InFuncPtr OPN6::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &OPN6::InA2H ),
	STATIC_CAST( Device::InFuncPtr, &OPN6::InA3H )
};

