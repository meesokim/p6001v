#include <stdlib.h>
#include <new>

#include "log.h"
#include "replay.h"
#include "common.h"
#include "error.h"


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
REPLAY::REPLAY( void ) : Ini(NULL), RepST(REP_IDLE), Matrix(NULL),
							MSize(0), RepFrm(0), EndFrm(0) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
REPLAY::~REPLAY( void )
{
	switch( RepST ){
	case REP_RECORD:	StopRecord(); break;
	case REP_REPLAY:	StopReplay(); break;
	}
	if( Matrix ) delete [] Matrix;
	if( Ini ) delete Ini;
}


////////////////////////////////////////////////////////////////
// ������
//
// ����:	msize	�}�g���N�X�T�C�Y
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool REPLAY::Init( int msize )
{
	PRINTD( GRP_LOG, "[REPLAY][Init]\n" );
	
	if( Ini ) delete Ini;
	Ini = NULL;
	
	if( Matrix ) delete [] Matrix;
	Matrix = new BYTE[msize];
	if( !Matrix ) return false;
	
	RepST   = REP_IDLE;
	MSize   = msize;
	RepFrm  = 0;
	EndFrm  = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// �X�e�[�^�X�擾
//
// ����:	�Ȃ�
// �Ԓl:	int		�X�e�[�^�X
////////////////////////////////////////////////////////////////
int REPLAY::GetStatus( void ) const
{
	return RepST;
}


////////////////////////////////////////////////////////////////
// ���v���C�L�^�J�n
//
// ����:	filename	�o�̓t�@�C����
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool REPLAY::StartRecord( const char *filename )
{
	// �Ƃ肠�����G���[�ݒ�
	Error::SetError( Error::ReplayPlayError );
	try{
		if( RepST != REP_IDLE ) throw Error::ReplayRecError;
		
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::ReplayRecError;
	}
	catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// ��O����
		Error::SetError( i );
		if( Ini ) delete Ini;
		Ini = NULL;
		return false;
	}
	
	memset( Matrix, 0xff, MSize );		// �L�[�}�g���N�X�o�b�t�@�N���A
	
	RepFrm = 0;
	RepST  = REP_RECORD;
	
	// �����������̂ŃG���[�Ȃ�
	Error::Reset();
	
	return true;
}


////////////////////////////////////////////////////////////////
// ���v���C�L�^�ĊJ
//
// ����:	filename	�o�̓t�@�C����
// ����:	frame       �r���ĊJ����t���[��
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool REPLAY::ResumeRecord( const char *filename, int frame )
{
	if( !StartRecord( filename ) ) return false;
	char buf[16];
	sprintf( buf, "%08lX", frame );
	// �w�肳�ꂽ�t���[���ȍ~�̃��v���C���폜���A��������ĊJ
	Ini->DeleteAfter( "REPLAY", buf );
	
	RepFrm = frame;
	return true;
}


////////////////////////////////////////////////////////////////
// ���v���C�L�^��~
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void REPLAY::StopRecord( void )
{
	if( RepST != REP_RECORD ) return;
	
	if( Ini ){
		Ini->PutEntry( "REPLAY", NULL, "EndFrm", "0x%08lX", RepFrm );
		
		Ini->Write();
		
		delete Ini;
		Ini = NULL;
	}
	
	RepST = REP_IDLE;
}


////////////////////////////////////////////////////////////////
// ���v���C1�t���[�����o��
//
// ����:	mt		�L�[�}�g���N�X�|�C���^
// 			chg		�L�[�}�g���N�X�ω� true:���� false:���Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool REPLAY::ReplayWriteFrame( const BYTE *mt, bool chg )
{
	char stren[16],strva[256];
	
	if( ( RepST != REP_RECORD ) || !mt || !Ini ) return false;
	
	// �}�g���N�X�ɕω����������珑�o��
//	if( chg ){	// ��ɏ��o�����Ƃɂ���
		sprintf( stren, "%08lX ", RepFrm );
		for( int i=0; i<MSize; i++ )
			sprintf( strva+i*2, "%02X", mt[i] );
		Ini->PutEntry( "REPLAY", NULL, stren, "%s", strva );
//	}
	
	RepFrm++;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ���v���C�Đ��J�n
//
// ����:	filename	���̓t�@�C����
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool REPLAY::StartReplay( const char *filename )
{
	// �Ƃ肠�����G���[�ݒ�
	Error::SetError( Error::ReplayPlayError );
	try{
		int st;	// �z���g��DWORD
		
		if( RepST != REP_IDLE ) throw Error::ReplayPlayError;
		
		Ini = new cIni();
		if( !Ini->Init( filename ) ) throw Error::ReplayPlayError;
		
		if( !Ini->GetInt( "REPLAY", "EndFrm", &st, EndFrm ) ) throw Error::NoReplayData;
		else                                                  EndFrm = st;
	}
	catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// ��O����
		Error::SetError( i );
		if( Ini ) delete Ini;
		Ini = NULL;
		return false;
	}
	
	memset( Matrix, 0xff, MSize );	// �L�[�}�g���N�X�o�b�t�@�N���A
	
	RepFrm = 1;
	RepST  = REP_REPLAY;
	
	// �����������̂ŃG���[�Ȃ�
	Error::Reset();
	
	return true;
}


////////////////////////////////////////////////////////////////
// ���v���C�Đ���~
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void REPLAY::StopReplay( void )
{
	if( RepST != REP_REPLAY ) return;
	
	if( Ini ){
		delete Ini;
		Ini = NULL;
	}
	
	RepST = REP_IDLE;
}


////////////////////////////////////////////////////////////////
// ���v���C1�t���[���Ǎ���
//
// ����:	mt		�L�[�}�g���N�X�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool REPLAY::ReplayReadFrame( BYTE *mt )
{
	char stren[16],strva[256];
	
	if( ( RepST != REP_REPLAY ) || !mt || !Ini ) return false;
	
	sprintf( stren, "%08lX", RepFrm );
	if( Ini->GetString( "REPLAY", stren, strva, "" ) ){
		int stl = strlen( strva ) / 2;
		for( int i=0; i<stl; i++ ){
			char dt[3] = "FF";
			strncpy( dt, &strva[i*2], 2 );
			Matrix[i] = strtol( dt, NULL, 16 );
		}
		memcpy( mt, Matrix, MSize );
	}
	
	if( ++RepFrm >= EndFrm ){
		// �f�[�^�I�[�ɒB�����烊�v���C�I��
		StopReplay();
	}
	
	return true;
}
