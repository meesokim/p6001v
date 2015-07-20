#include <string.h>

#include "pc6001v.h"

#include "log.h"
#include "intr.h"
#include "osd.h"
#include "schedule.h"
#include "voice.h"

#include "p6el.h"
#include "p6vm.h"


// �C�x���gID
#define	EID_FRAME	(1)



////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
VCE6::VCE6( VM6 *vm, const ID& id ) : Device(vm,id),
	io_E0H(0), io_E2H(0), io_E3H(0), VStat(D7752E_IDL),
	IVLen(0), IVBuf(NULL), IVPos(0), Pnum(0), Fnum(0), PReady(false), Fbuf(NULL)
{
	INITARRAY( FilePath, '\0' );
	INITARRAY( ParaBuf, 0 );
	SndDev::Volume = DEFAULT_VOICEVOL;
}

VCE64::VCE64( VM6 *vm, const ID& id ) : VCE6(vm,id) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
VCE6::~VCE6()
{
	FreeVoice();
	if( Fbuf ) delete [] Fbuf;
}

VCE64::~VCE64(){}


////////////////////////////////////////////////////////////////
// �C�x���g�R�[���o�b�N�֐�
//
// ����:	id		�C�x���gID
//			clock	�N���b�N
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void VCE6::EventCallback( int id, int clock )
{
	PRINTD( VOI_LOG, "[VOICE][EventCallback] -> " );
	
	switch( id ){
	case EID_FRAME:	// 1�t���[��
		if( VStat & D7752E_EXT ){	// �O���唭������
			PRINTD( VOI_LOG, "(EXT)\n" );
			if( PReady ){	// �p�����[�^�Z�b�g�������Ă�?
				// �t���[����=0�Ȃ�ΏI������
				if( !(ParaBuf[0]>>3) )
					AbortVoice();
				else{
					// 1�t���[�����̃T���v������
					cD7752::Synth(	ParaBuf, Fbuf );
					
					// �T���v�����O���[�g��ϊ����ăo�b�t�@�ɏ�����
					UpConvert();
					
					// ���t���[���̃p�����[�^���󂯕t����
					PReady = false;
					VStat |= D7752E_REQ;
					ReqIntr();		// ��������������
				}
			}else{			// �������ĂȂ���΃G���[
				AbortVoice();		// ������~
				VStat = D7752E_ERR;	// �X�e�[�^�X���G���[�����ɃZ�b�g
			}
		}else{						// �����唭������
			PRINTD( VOI_LOG, "(INT) " );
			// 1�t���[�����̃T���v�����o�b�t�@�ɏ�����
			int num = min( IVLen - IVPos, cD7752::GetFrameSize() * SndDev::SampleRate / 10000 );
			PRINTD( VOI_LOG, "%d/%d\n", num, IVPos );
			while( num-- )
				SndDev::cRing::Put( ( IVBuf[IVPos++] * SndDev::Volume ) / 100 );
			if( IVPos >= IVLen ){	// �Ō�܂Ŕ��������甭���I��
				AbortVoice();
			}
		}
		
		break;
	}
}


////////////////////////////////////////////////////////////////
// ���[�h�Z�b�g
////////////////////////////////////////////////////////////////
void VCE6::VSetMode( BYTE mode )
{
	PRINTD( VOI_LOG, "[VOICE][VSetMode]    %02X %dms", mode, (((mode>>2)&1)+1)*10 );
	PRINTD( VOI_LOG, " %s\n", mode&1 && mode&2 ? "X" : mode&1 ? "SLOW" : mode&2 ? "FAST" : "NORMAL" );
	
	// ���������J�n
	cD7752::Start( mode );
	
	// �X�e�[�^�X�N���A
	VStat = D7752E_IDL;
}


////////////////////////////////////////////////////////////////
// �R�}���h�Z�b�g
////////////////////////////////////////////////////////////////
void VCE6::VSetCommand( BYTE comm )
{
	PRINTD( VOI_LOG, "[VOICE][VSetCommand] %02X\n", comm );
	
	// �������Ȃ�~�߂�
	AbortVoice();
	
	switch( comm ){
	case 0x00:		// ������I���R�}���h ----------
	case 0x01:
	case 0x02:
	case 0x03:
	case 0x04:
		// WAV�t�@�C����Ǎ���
		if( !LoadVoice( comm ) ) break;
		
		// �X�e�[�^�X�������Đ����[�h�ɃZ�b�g
		VStat = D7752E_BSY;
		
		// �t���[���C�x���g���Z�b�g����
		vm->EventAdd( this, EID_FRAME, 10000.0/(double)cD7752::GetFrameSize(), EV_LOOP|EV_HZ );
		
		break;
		
	case 0xfe:		// �O����I���R�}���h ----------
		// �t���[���o�b�t�@�m��
		Fbuf = new D7752_SAMPLE[cD7752::GetFrameSize()];
		if( !Fbuf ) break;
		
		// �t���[���C�x���g���Z�b�g����
		vm->EventAdd( this, EID_FRAME, 10000.0/(double)cD7752::GetFrameSize(), EV_LOOP|EV_HZ );
		
		// �X�e�[�^�X���O����Đ����[�h�ɃZ�b�g�C�p�����[�^��t�J�n
		VStat = D7752E_BSY | D7752E_EXT | D7752E_REQ;
		
		ReqIntr();		// ��������������
		break;
		
	case 0xff:		// �X�g�b�v�R�}���h ----------
		break;
		
	default:		// �����R�}���h  ----------
		VStat = D7752E_ERR;	// �z���g?
		break;
	}
}


////////////////////////////////////////////////////////////////
// �����p�����[�^�]��
////////////////////////////////////////////////////////////////
void VCE6::VSetData( BYTE data )
{
	PRINTD( VOI_LOG, "[VOICE][VSetData]    %02X\n", data );
	
	// �Đ����̂݃f�[�^���󂯕t����
	if(	(VStat & D7752E_BSY)&&(VStat & D7752E_REQ) ){
		if( Fnum == 0 || Pnum ){	// �ŏ��̃t���[��?
			// �ŏ��̃p�����[�^��������J��Ԃ�����ݒ�
			if( Pnum == 0 ) Fnum = data>>3;
			// �p�����[�^�o�b�t�@�ɕۑ�
			ParaBuf[Pnum++] = data;
			// ����1�t���[�����̃p�����[�^�����܂����甭����������
			if( Pnum == 7 ){
				VStat &= ~D7752E_REQ;
				Pnum = 0;
				if( Fnum > 0 ) Fnum--;
				PReady = true;
			}else
				ReqIntr();		// ��������������
		}else{						// �J��Ԃ��t���[��?
			// �p�����[�^�o�b�t�@�ɕۑ�
			// PD7752�̎d�l�ɍ��킹��
			for( int i=1; i<6; i++ ) ParaBuf[i] = 0;
			ParaBuf[6] = data;
			VStat &= ~D7752E_REQ;
			Pnum = 0;
			Fnum--;
			PReady = true;
		}
	}
}


////////////////////////////////////////////////////////////////
// �X�e�[�^�X���W�X�^�擾
////////////////////////////////////////////////////////////////
int VCE6::VGetStatus( void )
{
	return VStat;
}


////////////////////////////////////////////////////////////////
// ������~
////////////////////////////////////////////////////////////////
void VCE6::AbortVoice( void )
{
	PRINTD( VOI_LOG, "[VOICE][AbortVoice]\n" );
	
	// �t���[���C�x���g��~
	vm->EventDel( this, EID_FRAME );
	
	// �c��̃p�����[�^�̓L�����Z��
	Pnum = Fnum = 0;
	PReady = false;
	
	// �t���[���o�b�t�@�J��
	if( Fbuf ){
		delete [] Fbuf;
		Fbuf = NULL;
	}
	
	// ������WAV�J��
	FreeVoice();
	
	VStat &= ~D7752E_BSY;
}


////////////////////////////////////////////////////////////////
// �T���v�����O���[�g�ϊ�
////////////////////////////////////////////////////////////////
void VCE6::UpConvert( void )
{
	PRINTD( VOI_LOG, "[VOICE][UpConvert]\n" );
	
	// 10kHz -> ���ۂɍĐ�����T���v�����O���[�g�ɕϊ��������̃T���v����
	int samples = cD7752::GetFrameSize() * SndDev::SampleRate / 10000;
	PRINTD( VOI_LOG, "UpConvert %d \n", samples );
	
	for( int i=0; i<samples; i++ ){
		int dat = Fbuf[i * cD7752::GetFrameSize() / samples] * 4;	// * 4 �� 16bit<-14bit �̂���
//		SndDev::cRing::Put( ( dat * SndDev::Volume ) / 100 );
		SndDev::cRing::Put( ( dat * SndDev::Volume * 2 ) / 100 );		// �o�̓��x�����Ⴂ�̂łƂ肠����2�{
	}
	PRINTD( VOI_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// ������WAV�Ǎ���
////////////////////////////////////////////////////////////////
bool VCE6::LoadVoice( int index )
{
	PRINTD( VOI_LOG, "[VOICE][LoadVoice]  No.%d -> ", index );
	
	DWORD len;		// ���f�[�^�T�C�Y
	BYTE *buf;		// ���f�[�^�o�b�t�@
	int freq;		// ���f�[�^�T���v�����O���[�g
	
	// WAV�t�@�C����Ǎ���
	char filepath[PATH_MAX];
	sprintf( filepath, "%sf4%d.wav", FilePath, index );
	
	PRINTD( VOI_LOG, "%s ->", filepath );
	
	if( !OSD_LoadWAV( filepath, &buf, &len, &freq ) ){
		PRINTD( VOI_LOG, "Error!\n" );
		return false;
	}
	
	PRINTD( VOI_LOG, "OK\n" );
	
	// �������x�ϊ���̃T�C�Y���v�Z���ăo�b�t�@���m��
	// �������x4�̎�,1�t���[���̃T���v������160
	IVLen = (int)( (double)SndDev::SampleRate * (double)(len/2) / (double)freq
					* (double)cD7752::GetFrameSize() / (double)160 );
	
	PRINTD( VOI_LOG, "Len:%d/%d ->", IVLen, (int)len );
	
	IVBuf = new int[IVLen];
	if( !IVBuf ){
		OSD_FreeWAV( buf );
		IVLen = 0;
		return false;
	}
	
	// �������x�ϊ�
	// �P�Ȃ�Ԉ����Ȃ̂Ńs�b�`���ς���Ă��܂��̂����
	// FFT���g����?
	signed short *sbuf = (signed short *)buf;
	for( int i=0; i<IVLen; i++ ){
		IVBuf[i] = sbuf[(int)(( (double)i * (double)(len/2) ) / (double)IVLen)];
	}
	
	// WAV�J��
	OSD_FreeWAV( buf );
	
	// �Ǎ��݃|�C���^������
	IVPos = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ������WAV�J��
////////////////////////////////////////////////////////////////
void VCE6::FreeVoice( void )
{
	if( IVBuf ){
		delete [] IVBuf;
		IVBuf = NULL;
		IVLen = IVPos= 0;
	}
}


////////////////////////////////////////////////////////////////
// �������������ݗv��
////////////////////////////////////////////////////////////////
void VCE6::ReqIntr( void ){}

void VCE64::ReqIntr( void )
{
	vm->IntReqIntr( IREQ_VOICE );
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool VCE6::Init( int srate, const char *path )
{
	PRINTD( VOI_LOG, "[VOICE][Init] BufferSize:" );
	
	// WAVE�t�@�C���i�[�p�X�ۑ�
	strncpy( FilePath, path, PATH_MAX );
	
	PRINTD( VOI_LOG, ":%d\n", SndDev::cRing::GetSize() );
	
	return SndDev::Init( srate );
}


////////////////////////////////////////////////////////////////
// ���Z�b�g
////////////////////////////////////////////////////////////////
void VCE6::Reset( void )
{
	AbortVoice();
	VStat = D7752E_IDL;
}


////////////////////////////////////////////////////////////////
// �X�g���[���X�V
//
// ����:	samples	�X�V�T���v����(-1:�c��o�b�t�@�S�� 0:�����N���b�N��)
// �Ԓl:	int		�X�V�T���v����
////////////////////////////////////////////////////////////////
int VCE6::SoundUpdate( int samples )
{
	PRINTD( VOI_LOG, "[VOICE][SoundUpdate] Samples: %d(%d)", samples, SndDev::cRing::FreeSize() );
	
	int length = 0;
	
	if( samples == 0 ){
		// ���Ƃ�
	}else if( samples > 0 ) length = min( samples - SndDev::cRing::ReadySize(), SndDev::cRing::FreeSize() );
	else                    length = SndDev::cRing::FreeSize();
	
	PRINTD( VOI_LOG, " -> %d\n", length );
	
	if( length <= 0 ) return 0;
	
	
	for( int i=0; i<length; i++ ){
		// �o�b�t�@�ɏ�����
		SndDev::cRing::Put( 0 );	// �蔲��
	}
	
	return length;
}


////////////////////////////////////////////////////////////////
// I/O�A�N�Z�X�֐�
////////////////////////////////////////////////////////////////
void VCE6::OutE0H( int, BYTE data ){ VSetData( data ); }
void VCE6::OutE2H( int, BYTE data ){ VSetMode( data ); }
void VCE6::OutE3H( int, BYTE data ){ VSetCommand( data ); }
BYTE VCE6::InE0H( int ){ return VGetStatus(); }
BYTE VCE6::InE2H( int ){ return io_E2H; }
BYTE VCE6::InE3H( int ){ return io_E3H; }


////////////////////////////////////////////////////////////////
// �ǂ��ł�SAVE
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool VCE6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "VOICE", NULL, "io_E0H",		"0x%02X",	io_E0H );
	Ini->PutEntry( "VOICE", NULL, "io_E2H",		"0x%02X",	io_E2H );
	Ini->PutEntry( "VOICE", NULL, "io_E3H",		"0x%02X",	io_E3H );
	Ini->PutEntry( "VOICE", NULL, "VStat",		"%d",		VStat );
	
	// ������֌W
//	int IVLen;						// �T���v����
//	int *IVBuf;						// �f�[�^�o�b�t�@
//	int IVPos;						// �Ǎ��݃|�C���^
	
	Ini->PutEntry( "VOICE", NULL, "ParaBuf0",	"0x%02X",	ParaBuf[0] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf1",	"0x%02X",	ParaBuf[1] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf2",	"0x%02X",	ParaBuf[2] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf3",	"0x%02X",	ParaBuf[3] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf4",	"0x%02X",	ParaBuf[4] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf5",	"0x%02X",	ParaBuf[5] );
	Ini->PutEntry( "VOICE", NULL, "ParaBuf6",	"0x%02X",	ParaBuf[6] );
	Ini->PutEntry( "VOICE", NULL, "Pnum",		"%d",		Pnum );
	Ini->PutEntry( "VOICE", NULL, "Fnum",		"%d",		Fnum );
	Ini->PutEntry( "VOICE", NULL, "PReady",		"%s",		PReady ? "Yes" : "No" );
	
//	D7752_SAMPLE *Fbuf;				// �t���[���o�b�t�@�|�C���^(10kHz 1�t���[��)
	
	return true;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool VCE6::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt(   "VOICE", "io_E0H",		&st,		io_E0H );	io_E0H = st;
	Ini->GetInt(   "VOICE", "io_E2H",		&st,		io_E2H );	io_E2H = st;
	Ini->GetInt(   "VOICE", "io_E3H",		&st,		io_E3H );	io_E3H = st;
	Ini->GetInt(   "VOICE", "VStat",		&VStat,		VStat );
	
	// ������֌W(�Ƃ肠��������)
	FreeVoice();
	
	Ini->GetInt(   "VOICE", "ParaBuf0",		&st,		ParaBuf[0] );	ParaBuf[0] = st;
	Ini->GetInt(   "VOICE", "ParaBuf1",		&st,		ParaBuf[1] );	ParaBuf[1] = st;
	Ini->GetInt(   "VOICE", "ParaBuf2",		&st,		ParaBuf[2] );	ParaBuf[2] = st;
	Ini->GetInt(   "VOICE", "ParaBuf3",		&st,		ParaBuf[3] );	ParaBuf[3] = st;
	Ini->GetInt(   "VOICE", "ParaBuf4",		&st,		ParaBuf[4] );	ParaBuf[4] = st;
	Ini->GetInt(   "VOICE", "ParaBuf5",		&st,		ParaBuf[5] );	ParaBuf[5] = st;
	Ini->GetInt(   "VOICE", "ParaBuf6",		&st,		ParaBuf[6] );	ParaBuf[6] = st;
	Ini->GetInt(   "VOICE", "Pnum",			&Pnum,		Pnum );
	Ini->GetInt(   "VOICE", "Fnum",			&Fnum,		Fnum );
	Ini->GetTruth( "VOICE", "PReady",		&PReady,	PReady );
	
//	D7752_SAMPLE *Fbuf;				// �t���[���o�b�t�@�|�C���^(10kHz 1�t���[��)
	
	return true;
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor VCE6::descriptor = {
	VCE6::indef, VCE6::outdef
};

const Device::OutFuncPtr VCE6::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &VCE6::OutE0H ),
	STATIC_CAST( Device::OutFuncPtr, &VCE6::OutE2H ),
	STATIC_CAST( Device::OutFuncPtr, &VCE6::OutE3H )
};

const Device::InFuncPtr VCE6::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &VCE6::InE0H ),
	STATIC_CAST( Device::InFuncPtr, &VCE6::InE2H ),
	STATIC_CAST( Device::InFuncPtr, &VCE6::InE3H )
};
