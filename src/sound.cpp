#include <new>

#include "pc6001v.h"

#include "error.h"
#include "log.h"
#include "osd.h"
#include "sound.h"


// �o�b�t�@�T�C�Y�̔{��
#define	MULTI		(2)
//   �����I�ɂ̓T�C�Y��MULTI�{����邪�ΊO�I�ɂ͓��{�̂悤�ɐU����
//   �ǂݏ����͑S�̈�ɑ΂��čs�Ȃ��邪�T�C�Y�擾�͓��{
//   �܂�I�[�o�[�t���[��h�~�����X�|���X��ۂĂ�...�͂�

////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cRing::cRing( void ) : Buffer(NULL), Size(0), Wpt(0), Rpt(0), Num(0) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cRing::~cRing( void )
{
	if( Buffer ) delete [] Buffer;
}


////////////////////////////////////////////////////////////////
// �o�b�t�@������
//
// ����:	size		�T���v����
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool cRing::InitBuffer( int size )
{
	PRINTD( SND_LOG, "[cRing][Init] Size:%d\n", size );
	
	if( Buffer ) delete [] Buffer;
	
	Size = size;
	Wpt = Rpt = Num = 0;
	
	try{
		Buffer = new int[Size * MULTI];	// ���ۂ̃o�b�t�@�T�C�Y��MULTI�{
		memset( Buffer, 0, Size * MULTI );
	}
	// new �Ɏ��s�����ꍇ
	catch( std::bad_alloc ){
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// �Ǎ���
//
// ����:	�Ȃ�
// �Ԓl:	int		�f�[�^
////////////////////////////////////////////////////////////////
int cRing::Get( void )
{
	if( Num ){
		cCritical::Lock();
		int data = Buffer[Rpt++];
		if( Rpt == Size * MULTI ) Rpt = 0;
		Num--;
		cCritical::UnLock();
		return data;
	}else{
		return 0;
	}
}


////////////////////////////////////////////////////////////////
// ������
//
// ����:	data		�f�[�^
// �Ԓl:	bool		true:���� false:���s(�o�b�t�@�����ς�)
////////////////////////////////////////////////////////////////
bool cRing::Put( int data )
{
	if( Num < Size * MULTI ){
		cCritical::Lock();
		Buffer[Wpt++] = data;
		if( Wpt == Size * MULTI ) Wpt = 0;
		Num++;
		cCritical::UnLock();
		return true;
	}else
		return false;
}


////////////////////////////////////////////////////////////////
// ���ǃf�[�^���擾
//
// ����:	�Ȃ�
// �Ԓl:	int			���ǃT���v����
////////////////////////////////////////////////////////////////
int cRing::ReadySize( void )
{
	return Num > Size ? Size : Num;
}


////////////////////////////////////////////////////////////////
// �c��o�b�t�@�擾
//
// ����:	�Ȃ�
// �Ԓl:	int			�c��o�b�t�@�T���v����
////////////////////////////////////////////////////////////////
int cRing::FreeSize( void )
{
	return Size - Num;
}


////////////////////////////////////////////////////////////////
// �o�b�t�@�T�C�Y�擾
//
// ����:	�Ȃ�
// �Ԓl:	int			�S�o�b�t�@�T���v����
////////////////////////////////////////////////////////////////
int cRing::GetSize( void )
{
	return Size;
}




////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
SndDev::SndDev( void ) : SampleRate(DEFAULT_SAMPLE_RATE),
							Volume(0), LPF_Mem(0), LPF_fc(0) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
SndDev::~SndDev( void ){}


////////////////////////////////////////////////////////////////
// ������
// ����:	rate	�T���v�����O���[�g
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool SndDev::Init( int rate )
{
	SampleRate = rate;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ���[�p�X�t�B���^ �J�b�g�I�t���g���ݒ�
//
// ����:	fc		�J�b�g�I�t���g��
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SndDev::SetLPF( int fc )
{
	LPF_fc = fc;
}


////////////////////////////////////////////////////////////////
// ���[�p�X�t�B���^
//
// [�Q�l] �����̃f�B�W�^���t�B���^
// http://kmkz.jp/mtm/mag/lab/digitalfilter.htm
//
// ����:	src		���f�[�^
// �Ԓl:	int		�t�B���^�K�p��̃f�[�^
////////////////////////////////////////////////////////////////
int SndDev::LPF( int src )
{
	int lpf_k = LPF_fc ? (int)((2.0*M_PI*(double)LPF_fc*(double)0x8000)/(double)SampleRate) : 0x8000;
	
	LPF_Mem += ( ( src - LPF_Mem ) * lpf_k ) / 0x8000;
	return LPF_Mem;
}


////////////////////////////////////////////////////////////////
// �Ǎ���(�I�[�o�[���C�h)
//
// ����:	�Ȃ�
// �Ԓl:	int		�f�[�^
////////////////////////////////////////////////////////////////
int SndDev::Get( void )
{
	return LPF( this->cRing::Get() );
}


////////////////////////////////////////////////////////////////
// �T���v�����O���[�g�ݒ�
//
// ����:	rate	�T���v�����O���[�g
//			size	�o�b�t�@�T�C�Y
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool SndDev::SetSampleRate( int rate, int size )
{
	if( SampleRate == rate ) return false;
	
	SampleRate = rate;
	
	if( !InitBuffer( size ) ) return false;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ���ʐݒ�
//
// ����:	vol		����(0-100)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SndDev::SetVolume( int vol )
{
	Volume = min( max( vol, 0 ), 100 );
}


////////////////////////////////////////////////////////////////
// �X�g���[���X�V
//
// ����:	samples	�V����T���v����
// �Ԓl:	int		�X�V�����T���v����
////////////////////////////////////////////////////////////////
int SndDev::SoundUpdate( int samples )
{
	return 0;
}




////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
SND6::SND6( void ) : Volume(DEFAULT_MASTERVOL), SampleRate(DEFAULT_SAMPLE_RATE), BSize(SOUND_BUFFER_SIZE),
						CbFunc(NULL), CbData(NULL)
{
	INITARRAY( RB, NULL );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
SND6::~SND6( void )
{
	Pause();
	// �I�[�f�B�I�f�o�C�X�����
	OSD_CloseAudio();
}


////////////////////////////////////////////////////////////////
// ������
//
// ����:	cbdata		�R�[���o�b�N�֐��ɓn���f�[�^
//			callback	�R�[���o�b�N�֐��ւ̃|�C���^
//			rate		�T���v�����O���[�g
//			size		�o�b�t�@�T�C�Y(�{��)
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool SND6::Init( void *cbdata, void (*callback)(void *, BYTE *, int ), int rate, int size )
{
	PRINTD( SND_LOG, "[SND6][Init]\n" );
	
	for( int i=0; i<MAXSTREAM; i++ ) RB[i] = NULL;
	
	// �o�b�t�@�T�C�Y(�T���v����)
	int samples = rate / VSYNC_HZ;
	
	// �o�b�t�@������
	if( !this->cRing::InitBuffer( samples * size ) ) return false;
	
	// �I�[�f�B�I�f�o�C�X���J��
	if( !OSD_OpenAudio( cbdata, callback, rate, samples ) ) return false;
	
	CbData     = cbdata;
	CbFunc     = callback;
	SampleRate = rate;
	BSize      = size;
	
	PRINTD( SND_LOG, " SampleRate : %d\n", rate );
	PRINTD( SND_LOG, " BufferSize : %d\n", samples * size );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �X�g���[���ڑ�
//
// ����:	SndDev *	�o�b�t�@�|�C���^
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool SND6::ConnectStream( SndDev *buf )
{
	PRINTD( SND_LOG, "[SND6][ConnectStream]\n" );
	
	if( !buf ) return false;
	
	// �o�b�t�@�T�C�Y(�T���v����)
	int samples = SampleRate * BSize / VSYNC_HZ;
	
	for( int i=0; i<MAXSTREAM; i++ ){
		if( !RB[i] ){
			// �o�b�t�@������
			if( !buf->InitBuffer( samples ) ) return false;
			RB[i] = buf;
			return true;
		}
	}
	
	return false;
}


////////////////////////////////////////////////////////////////
// �Đ�
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SND6::Play( void )
{
	PRINTD( SND_LOG, "[SND6][Play]\n" );
	
	OSD_StartAudio();
}


////////////////////////////////////////////////////////////////
// ��~
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SND6::Pause( void )
{
	PRINTD( SND_LOG, "[SND6][Pause]\n" );
	
	OSD_StopAudio();
}


////////////////////////////////////////////////////////////////
// �T���v�����O���[�g�ݒ�
//
// ����:	rate	�T���v�����O���[�g
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool SND6::SetSampleRate( int rate )
{
	SampleRate = rate;
	
	bool pflag = OSD_AudioPlaying();
	OSD_CloseAudio();
	
	// �o�b�t�@�T�C�Y(�T���v����)
	int samples = rate / VSYNC_HZ;
	
	// �o�b�t�@������
	if( !this->cRing::InitBuffer( samples * BSize ) ) return false;
	
	for( int i=0; i<MAXSTREAM; i++ )
		if( RB[i] && !RB[i]->SetSampleRate( rate, samples * BSize ) ) return false;
	
	// �I�[�f�B�I�f�o�C�X���J��
	if( !OSD_OpenAudio( CbData, CbFunc, rate, samples ) ) return false;
	if( pflag ) Play();
	
	return true;
}


////////////////////////////////////////////////////////////////
// �T���v�����O���[�g�擾
//
// ����:	�Ȃ�
// �Ԓl:	int		�T���v�����O���[�g
////////////////////////////////////////////////////////////////
int SND6::GetSampleRate( void )
{
	return SampleRate;
}


////////////////////////////////////////////////////////////////
// �}�X�^�[���ʐݒ�
//
// ����:	vol		����(0-100)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SND6::SetVolume( int vol )
{
	Volume = min( max( vol, 0 ), 100 );
}


////////////////////////////////////////////////////////////////
// �o�b�t�@�T�C�Y(�{��)�擾
//
// ����:	�Ȃ�
// �Ԓl:	int		�o�b�t�@�T�C�Y(�{��)
////////////////////////////////////////////////////////////////
int SND6::GetBufferSize( void )
{
	return BSize;
}


////////////////////////////////////////////////////////////////
// �T�E���h���O�X�V�֐�
//
// ����:	samples		�T���v����
//			exbuf		�O���o�b�t�@�|�C���^
// �Ԓl:	int			�X�V�����T���v����
////////////////////////////////////////////////////////////////
int SND6::PreUpdate( int samples, cRing *exbuf )
{
	int exsam = 0;
	
	PRINTD( SND_LOG,"PreUpdate" );
	
	for( int i=0; i<MAXSTREAM; i++ )
		if( RB[i] ){
			PRINTD( SND_LOG," [%d]%d", i, RB[i]->ReadySize() );
			exsam = min( max( exsam, RB[i]->ReadySize() ), samples );
		}
	PRINTD( SND_LOG,"\n" );
	
	for( int i=0; i<exsam; i++ ){
		int dat = 0;
		for( int j=0; j<MAXSTREAM; j++ )
			if( RB[j] ) dat += RB[j]->Get();
		
		dat = ( dat * Volume ) / 100;
		dat = min( max( dat, INT16_MIN ), INT16_MAX );
		
		this->cRing::Put( (int16_t)dat );
		// �O���o�b�t�@�����݂���Ώ�����
		if( exbuf ) exbuf->Put( (int16_t)dat );
	}
	
	return exsam;
}


////////////////////////////////////////////////////////////////
// �T�E���h�X�V�֐�
//
// ����:	stream		�X�g���[�������݃o�b�t�@�ւ̃|�C���^
//			samples		�T���v����
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void SND6::Update( BYTE *stream, int samples )
{
	int16_t *str = (int16_t *)stream;
	
	PRINTD( SND_LOG, "[SND6][Update] Stream:%p Samples:%d / %d\n", stream, samples, this->cRing::ReadySize() );
	
	for( int i=0; i<samples; i++ ){
		*(str++) = (int16_t)this->cRing::Get();
	}
}
