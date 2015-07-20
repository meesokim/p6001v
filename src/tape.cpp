#include "pc6001v.h"

#include "common.h"
#include "disk.h"
#include "intr.h"
#include "log.h"
#include "osd.h"
#include "schedule.h"
#include "tape.h"

#include "p6el.h"
#include "p6vm.h"


// P6T�`���t�H�[�}�b�gVer.2
//  ��{�I�ɂ́u�x�^�C���[�W+�t�b�^+�x�^�C���[�W�T�C�Y(4byte)�v�Ƃ����\��
//  �t�b�^�ɂ�DATA�u���b�N��񂪊܂܂��B
//  DATA�u���b�N�͖���������؂�Ƃ���1�Ɛ�����B
//  BASIC�Ȃǁu�w�b�_�{�f�[�^�v�Ƃ����\���̏ꍇ�CDATA�u���b�N��2�ƂȂ邪
//  ID�ԍ��𓯈�ɂ��邱�ƂŊ֘A�t���ĊǗ��ł���B
//
// [�t�b�^]
//  header (2byte) : "P6"
//  ver    (1byte) : �o�[�W����
//  dbnum  (1byte) : �܂܂��DATA�u���b�N��(255�܂�)
//  start  (1byte) : �I�[�g�X�^�[�g�t���O(0:���� 1:�L��)
//  basic  (1byte) : BASIC���[�h(PC-6001�̏ꍇ�͖��Ӗ�)
//  page   (1byte) : �y�[�W��
//  askey  (2byte) : �I�[�g�X�^�[�g�R�}���h������
//  ...�R�}���h������ꍇ�͂��̌��askey������
//  exhead (2byte) : �g�����T�C�Y(64KB�܂�)
//  ...�g����񂪂���ꍇ�͂��̌��exhead������
//
// [DATA�u���b�N]
//  header (2byte) : "TI"
//  id     (1byte) : ID�ԍ�(DATA�u���b�N���֘A�t����)
//  name  (16byte) : �f�[�^��(15����+'00H')
//  baud   (2byte) : �{�[���[�g(600/1200)
//  stime  (2byte) : �������̎���(ms)
//  ptime  (2byte) : �ҁ[���̎���(ms)
//  offset (4byte) : �x�^�C���[�W�擪����̃I�t�Z�b�g
//  size   (4byte) : �f�[�^�T�C�Y


// �C�x���gID
#define	EID_TAPE	(1)

// ���g��
#define PG_HI	(0)
#define PG_LO	(1)

////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
CMTL::CMTL( VM6 *vm, const ID& id ) : Device(vm,id),
	p6t(NULL), Relay(false), stron(false), Boost(DEFAULT_BOOST),
	MaxBoost60(DEFAULT_MAXBOOST60), MaxBoost62(DEFAULT_MAXBOOST62)
{
	INITARRAY( FilePath, '\0' );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
CMTL::~CMTL( void )
{
	Unmount();
}


////////////////////////////////////////////////////////////////
// �C�x���g�R�[���o�b�N�֐�
//
// ����:	id		�C�x���gID
//			clock	�N���b�N
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void CMTL::EventCallback( int id, int clock )
{
	switch( id ){
	case EID_TAPE:	// CMT�����ݏ���
		// �����ݏ������łȂ�?
		if( vm->CpusIsCmtIntrReady() ){
			// CMT�A�b�v�f�[�g
			WORD rd = Update();
			// �f�[�^�Ȃ�CMT�����ݔ���
			if( rd & PG_D ){
				vm->CpusReqCmtIntr( rd & 0xff );
			}
		}
		break;
	}
}


////////////////////////////////////////////////////////////////
// �����[�g����(PLAY,STOP)
////////////////////////////////////////////////////////////////
bool CMTL::Remote( bool relay )
{
    PRINTD( TAPE_LOG, "[TAPE][Relay] -> %s\n", relay ? "true" : "false" );
	
	stron = false;
	
	// �����[�̏�Ԃ�ۑ�
	Relay = relay;
	
	// �����[�̏�Ԃɂ��C�x���g�ǉ�or�폜
	if( relay ){	// ON
		// ��ʃT�C�Y�ɂ��BoostUp�ő�{���ݒ�
		// �{����BASIC���[�h(N60,N60m/N66)�ɂ���Ĕ��f����ׂ�
		// ���܂茵���ł͂Ȃ����ǂ������Ƃɂ���
		// SR��mk2/66�Ɠ����ɂ��Ă݂�
		int bst = Boost ? ( vm->VdgGetWinSize() ? MaxBoost60 : MaxBoost62 ) : 1;
		
		if( !vm->EventAdd( this, EID_TAPE, DEFAULT_CMT_HZ * bst, EV_LOOP|EV_HZ ) ) return false;
	}else{			// OFF
		if( !vm->EventDel( this, EID_TAPE ) ) return false;
	}
	return true;
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool CMTL::Init( int srate )
{
	// P6T��� �̈揉����(���Z�b�g���ɂ͖���)
	p6t = NULL;
	
	return SndDev::Init( srate );
}


////////////////////////////////////////////////////////////////
// ���Z�b�g
////////////////////////////////////////////////////////////////
void CMTL::Reset( void )
{
	// TAPE�����Z�b�g
	if( p6t ) p6t->Reset();
}


////////////////////////////////////////////////////////////////
// TAPE�}�E���g
////////////////////////////////////////////////////////////////
bool CMTL::Mount( const char *filename )
{
	PRINTD( TAPE_LOG, "[TAPE][Mount] %s\n", filename  );
	
	// �����}�E���g�ς݂ł���΃A���}�E���g����
	if( p6t ){
		delete p6t;
		p6t = NULL;
	}
	
	// P6T�m��
	p6t = new cP6T;
	if( !p6t ) return false;
	
	// �t�@�C������Ǎ���
	if( !p6t->Readf( filename ) ){
		delete p6t;
		p6t = NULL;
		return false;
	}
	
	// �t�@�C���p�X�ۑ�
	strncpy( FilePath, filename, PATH_MAX );
	
	return true;
}


////////////////////////////////////////////////////////////////
// TAPE�A���}�E���g
////////////////////////////////////////////////////////////////
void CMTL::Unmount( void )
{
	PRINTD( TAPE_LOG, "[TAPE][Unmount]\n" );
	
	if( p6t ){
		delete p6t;
		p6t = NULL;
		*FilePath = '\0';
	}
}


////////////////////////////////////////////////////////////////
// CMT 1�����Ǎ���
// �߂�l�F��ʂ̓f�[�^��(�ҁ[,����,�f�[�^) ���ʂ̓f�[�^
// �����[ON & CMT OPEN ���O��
////////////////////////////////////////////////////////////////
WORD CMTL::CmtRead( void )
{
	PRINTD( TAPE_LOG, "[TAPE][CmtRead] " );
	
	// TAPE���}�E���g����Ă��Ȃ���Ζ���
	if( !p6t ) return PG_S;
	
	// �������҂�?
	if( p6t->IsSWaiting() ){
		PRINTD( TAPE_LOG, "swait\n" );
		return PG_S;
	}
	
	// �ҁ[���҂�?
	if( p6t->IsPWaiting() ){
		PRINTD( TAPE_LOG, "pwait\n" );
		return PG_P;
	}
	
	// �f�[�^�Ǎ���
	PRINTD( TAPE_LOG, "read\n" );
	return PG_D | p6t->ReadOne();
}


////////////////////////////////////////////////////////////////
// �}�E���g�ς�?
////////////////////////////////////////////////////////////////
bool CMTL::IsMount( void ) const
{
	if( p6t ) return true;
	else      return false;
}


////////////////////////////////////////////////////////////////
// �I�[�g�X�^�[�g?
////////////////////////////////////////////////////////////////
bool CMTL::IsAutoStart( void ) const
{
	if( p6t ) return p6t->GetAutoStartInfo()->Start;
	else      return false;
}


////////////////////////////////////////////////////////////////
// �t�@�C���p�X�擾
////////////////////////////////////////////////////////////////
const char *CMTL::GetFile() const
{
	return FilePath;
}


////////////////////////////////////////////////////////////////
// TAPE���擾
////////////////////////////////////////////////////////////////
const char *CMTL::GetName( void ) const
{
	if( p6t ) return p6t->GetName();
	else      return (const char *)"";
}


////////////////////////////////////////////////////////////////
// �x�^�C���[�W�T�C�Y�擾
////////////////////////////////////////////////////////////////
DWORD CMTL::GetSize( void ) const
{
	if( p6t ) return p6t->GetSize();
	else      return 0;
}


////////////////////////////////////////////////////////////////
// �J�E���^�擾
////////////////////////////////////////////////////////////////
int CMTL::GetCount( void ) const
{
	if( p6t ) return p6t->GetCount();
	else      return 0;
}


////////////////////////////////////////////////////////////////
// �����[�̏�Ԏ擾
////////////////////////////////////////////////////////////////
bool CMTL::IsRelay( void ) const
{
	return Relay;
}


////////////////////////////////////////////////////////////////
// BoostUp�ݒ�
////////////////////////////////////////////////////////////////
void CMTL::SetBoost( bool boost )
{
	if( Boost != boost ){
		Boost = boost;
		// �����[ON���������U�~�߂čĊJ
		if( Relay ){
			Remote( false );
			Remote( true );
		}
	}
}


////////////////////////////////////////////////////////////////
// BoostUp�ő�{���ݒ�
////////////////////////////////////////////////////////////////
void CMTL::SetMaxBoost( int max60, int max62 )
{
	if( max60 > 0 ) MaxBoost60 = max60;
	if( max62 > 0 ) MaxBoost62 = max62;
}


////////////////////////////////////////////////////////////////
// BoostUp��Ԏ擾
////////////////////////////////////////////////////////////////
bool CMTL::IsBoostUp( void ) const
{
	return Boost;
}


////////////////////////////////////////////////////////////////
// �I�[�g�X�^�[�g���擾
////////////////////////////////////////////////////////////////
const P6TAUTOINFO *CMTL::GetAutoStartInfo( void ) const
{
	return p6t->GetAutoStartInfo();
}


////////////////////////////////////////////////////////////////
// �X�g���[���X�V(1byte��)
////////////////////////////////////////////////////////////////
WORD CMTL::Update( void )
{
	PRINTD( TAPE_LOG, "[TAPE][Update]\n" );
	
	WORD rd = PG_S;
	
	// TAPE�C���[�W�I�[�v��?
	if( p6t ){
		// 1byte���̃f�[�^����� 10ms(��{)
		int length = SndDev::SampleRate / DEFAULT_CMT_HZ;
		int bdata;
		
		rd = CmtRead();		// CMT 1�����Ǎ���
		
		switch( rd & 0xff00 ){	// �f�[�^�`���́H
		case PG_P:	// �ҁ[���̏ꍇ
			// �����ɃZ�b�g
			while( length-- ) SndDev::cRing::Put( GetSinCurve( PG_HI ) );
			stron = true;	// �X�g���[���X�V����
			
			break;
		case PG_S:	// �������̏ꍇ
			while( length-- ) SndDev::cRing::Put( 0 );
			
			break;
		case PG_D:	// �f�[�^�̏ꍇ
			// �X�^�[�g�r�b�g 1bit
			// �f�[�^�r�b�g   8bits
			// �X�g�b�v�r�b�g 3bits
			
			bdata = length/12;	// 1bit������̃f�[�^��
			// �X�^�[�g�r�b�g
			for( int i=0; i<bdata; i++ ){
				SndDev::cRing::Put( GetSinCurve( PG_LO ) );
			}
			// �f�[�^�r�b�g
			for( int j=7; j>=0; j-- ){
				int hilo = (rd>>j)&1 ? PG_HI : PG_LO;
				for( int i=0; i<bdata; i++ ){
					SndDev::cRing::Put( GetSinCurve( hilo ) );
				}
			}
			// �X�g�b�v�r�b�g
			for( length -= bdata*9; length > 0; length-- ){
				SndDev::cRing::Put( GetSinCurve( PG_HI ) );
			}
			break;
		}
	}
	
	return rd;
}


////////////////////////////////////////////////////////////////
// �X�g���[���X�V
//
// ����:	samples	�X�V�T���v����(-1:�c��o�b�t�@�S�� 0:�����N���b�N��)
// �Ԓl:	int		�X�V�T���v����
////////////////////////////////////////////////////////////////
int CMTL::SoundUpdate( int samples )
{
//	PRINTD( TAPE_LOG, "[TAPE][SoundUpdate] Samples: %d(%d)", samples, SndDev::cRing::FreeSize() );
	
	int length = 0;
	
	if( samples == 0 ){
		// ���Ƃ�
	}else if( samples > 0 ) length = min( samples - SndDev::cRing::ReadySize(), SndDev::cRing::FreeSize() );
	else                    length = SndDev::cRing::FreeSize();
	
//	PRINTD( TAPE_LOG, " -> %d\n", length );
	
	if( length <= 0 ) return 0;
	
	for( int i=0; i<length; i++ ){
		// �o�b�t�@�ɏ�����
		SndDev::cRing::Put( stron ? GetSinCurve( PG_HI ) : 0 );	// �蔲��
	}
	
	return length;
}


////////////////////////////////////////////////////////////////
// sin�g�擾
////////////////////////////////////////////////////////////////
int CMTL::GetSinCurve( int fq )
{
	static const int sinc[] = {
		 32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,
		 32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,  32767,
		-32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767,
		-32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767, -32767
 	};
	static int n=0;
	
	// �T���v�����O���[�g�Ǝ��g���ŊԈ����̊Ԋu�����߂�
	// �f�[�^�e�[�u����44100Hz,1200Hz�̐��l
	n += (fq == PG_HI ? 2:1 ) * 44100 / SndDev::SampleRate;
	
	// �e�[�u���T�C�Y��72(sizeof(sinc))
	if( n >= (int)(sizeof(sinc)/sizeof(int)) ) n -= (int)(sizeof(sinc)/sizeof(int));
	
	int ret = ( sinc[n] * SndDev::Volume ) / 100;
	
	return ret;
}




////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
CMTS::CMTS( VM6 *vm, const ID& id ) : Device(vm,id), fp(NULL), Baud(1200)
{
	INITARRAY( FilePath, '\0' );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
CMTS::~CMTS( void )
{
	Unmount();
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool CMTS::Init( const char *filename )
{
	if( *filename ){
		// �t�@�C���p�X�ۑ�
		strncpy( FilePath, filename, PATH_MAX );
	}
	return true;
}


////////////////////////////////////////////////////////////////
// TAPE�}�E���g
////////////////////////////////////////////////////////////////
bool CMTS::Mount( void )
{
	if( fp ) fclose( fp );
	
	fp = FOPENEN( FilePath, "wb" );
	
	if( fp ) return true;
	else     return false;
}


////////////////////////////////////////////////////////////////
// TAPE�A���}�E���g
////////////////////////////////////////////////////////////////
void CMTS::Unmount( void )
{
	if( fp ){
		// �t�b�^��t����(�Ƃ肠�����x�^��)
		fflush( fp );
		int beta = ftell( fp );	// �x�^�C���[�W�T�C�Y�擾
		
		// [�t�b�^]
		fputc( 'P', fp );	// header (2byte) : "P6"
		fputc( '6', fp );
		fputc( 2, fp );		// ver    (1byte) : �o�[�W����
		fputc( 1, fp );		// dbnum  (1byte) : �܂܂��DATA�u���b�N��(255�܂�)
		fputc( 0, fp );		// start  (1byte) : �I�[�g�X�^�[�g�t���O(0:���� 1:�L��)
		fputc( 1, fp );		// basic  (1byte) : BASIC���[�h(PC-6001�̏ꍇ�͖��Ӗ�)
		fputc( 1, fp );		// page   (1byte) : �y�[�W��
		fputc( 0, fp );		// askey  (2byte) : �I�[�g�X�^�[�g�R�}���h������
		fputc( 0, fp );		// ...�R�}���h������ꍇ�͂��̌��askey������
		fputc( 0, fp );		// exhead (2byte) : �g�����T�C�Y(64KB�܂�)
		fputc( 0, fp );		// ...�g����񂪂���ꍇ�͂��̌��exhead������
		
		// [DATA�u���b�N]
		fputc( 'T', fp );				// header (2byte) : "TI"
		fputc( 'I', fp );
		fputc( 0, fp );					// id     (1byte) : ID�ԍ�(DATA�u���b�N���֘A�t����)
		for( int i=0; i<16; i++ ) fputc( 0, fp );	// name  (16byte) : �f�[�^��(15����+'00H')
		FPUTWORD( Baud, fp );			// baud   (2byte) : �{�[���[�g(600/1200)
		fputc( 0x48, fp );				// stime  (2byte) : �������̎���(ms)
		fputc( 0x0d, fp );
		fputc( 0x48, fp );				// ptime  (2byte) : �ҁ[���̎���(ms)
		fputc( 0x0d, fp );
		fputc( 0, fp );					// offset (4byte) : �x�^�C���[�W�擪����̃I�t�Z�b�g
		fputc( 0, fp );
		fputc( 0, fp );
		fputc( 0, fp );
		FPUTDWORD( beta, fp );			// size   (4byte) : �f�[�^�T�C�Y
		
		// [�x�^�C���[�W�T�C�Y]
		FPUTDWORD( beta, fp );
		
		fclose( fp );
		fp = NULL;
		*FilePath = '\0';
	}
}


////////////////////////////////////////////////////////////////
// �{�[���[�g�ݒ�
////////////////////////////////////////////////////////////////
void CMTS::SetBaud( int b )
{
	Baud = b;
}


////////////////////////////////////////////////////////////////
// CMT 1����������
////////////////////////////////////////////////////////////////
void CMTS::CmtWrite( BYTE data )
{
	if( fp ) fputc( data, fp );
}


////////////////////////////////////////////////////////////////
// I/O�A�N�Z�X�֐�
////////////////////////////////////////////////////////////////
void CMTL::OutB0H( int, BYTE data ){ Remote( data&0x08 ? true : false ); }


////////////////////////////////////////////////////////////////
// �ǂ��ł�SAVE
////////////////////////////////////////////////////////////////
bool CMTL::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "TAPE", NULL, "Relay",	"%s",	Relay ? "Yes" : "No" );
	Ini->PutEntry( "TAPE", NULL, "BoostUp",	"%s",	Boost ? "Yes" : "No" );
	
	// TAPE���}�E���g����ĂȂ���Ή������Ȃ��Ŗ߂�
	if( !p6t ) return true;
	
	// �}�E���g����Ă�����P6T�I�u�W�F�N�g��SAVE
	char pathstr[PATH_MAX+1];
	strncpy( pathstr, FilePath, PATH_MAX );
	OSD_RelativePath( pathstr );
	Ini->PutEntry( "TAPE", NULL, "FilePath",	"%s",	pathstr );
	
	return p6t->DokoSave( Ini );
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD
////////////////////////////////////////////////////////////////
bool CMTL::DokoLoad( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->GetTruth( "TAPE", "Relay",		&Relay,	Relay );
	Ini->GetTruth( "TAPE", "BoostUp",	&Boost,	Boost );
	
	Ini->GetString( "TAPE", "FilePath", FilePath, "" );
	if( *FilePath ){
		if( !Mount( FilePath ) ) return false;
		if( !p6t->DokoLoad( Ini ) ) return false;
	}else
		Unmount();
	
	
	return true;
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor CMTL::descriptor = {
	CMTL::indef, CMTL::outdef
};

const Device::OutFuncPtr CMTL::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &CMTL::OutB0H )
};

const Device::InFuncPtr CMTL::indef[] = { NULL };
