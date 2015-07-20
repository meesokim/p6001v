#include "common.h"
#include "log.h"
#include "movie.h"
#include "osd.h"


// [�Q�l] MSDN
// AVI �t�@�C�� �t�H�[�}�b�g
// http://msdn.microsoft.com/ja-jp/library/cc352258.aspx
// AVI �t�@�C�� �t�H�[�}�b�g�� DV �f�[�^(�A�[�J�C�u�R���e���c)
// http://msdn.microsoft.com/ja-jp/library/cc354259.aspx

// 16bit���[�h�ɂ��Ẵ���
// SDL�� R:G:B=5:6:5 �炵���̂������̃t�H�[�}�b�g��QuickTime�ŏ�肭�Đ��ł��Ȃ��悤�Ȃ̂�
// R:G:B=5:5:5 �Ƃ��Ă݂�B

#define	CIDVIDS		(CTODW( 'v', 'i', 'd', 's' ))
#define	CIDAUDS		(CTODW( 'a', 'u', 'd', 's' ))
#define	CIDRLE		(CTODW( 'R', 'L', 'E', ' ' ))
#define	CIDDIB		(CTODW( 'D', 'I', 'B', ' ' ))
#define	CID00DC		(CTODW( '0', '0', 'd', 'c' ))
#define	CID00DB		(CTODW( '0', '0', 'd', 'b' ))
#define	CID01WB		(CTODW( '0', '1', 'w', 'b' ))


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
AVI6::AVI6( void ) : vfp(NULL), ABPP(32), Sbuf(NULL), PosMOVI(0),
						RiffSize(0), MoviSize(0), anum(0) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
AVI6::~AVI6( void )
{
	if( IsAVI() ) StopAVI();
}


////////////////////////////////////////////////////////////////
// �e��w�b�_�o��
////////////////////////////////////////////////////////////////
void AVI6::putBMPINFOHEADER6( BMPINFOHEADER6 *p )
{
	FPUTDWORD( p->biSize,			vfp );
	FPUTDWORD( p->biWidth,			vfp );	// LONG
	FPUTDWORD( p->biHeight,			vfp );	// LONG
	FPUTWORD(  p->biPlanes,			vfp );
	FPUTWORD(  p->biBitCount,		vfp );
	FPUTDWORD( p->biCompression,	vfp );
	FPUTDWORD( p->biSizeImage,		vfp );
	FPUTDWORD( p->biXPelsPerMeter,	vfp );	// LONG
	FPUTDWORD( p->biYPelsPerMeter,	vfp );	// LONG
	FPUTDWORD( p->biClrUsed,		vfp );
	FPUTDWORD( p->biClrImportant,	vfp );
}

void AVI6::putMAINAVIHEADER6( MAINAVIHEADER6 *p )
{
	FPUTDWORD( p->dwMicroSecPerFrame,		vfp );
	FPUTDWORD( p->dwMaxBytesPerSec,			vfp );
	FPUTDWORD( p->dwReserved1,				vfp );
	FPUTDWORD( p->dwFlags,					vfp );
	FPUTDWORD( p->dwTotalFrames,			vfp );
	FPUTDWORD( p->dwInitialFrames,			vfp );
	FPUTDWORD( p->dwStreams,				vfp );
	FPUTDWORD( p->dwSuggestedBufferSize,	vfp );
	FPUTDWORD( p->dwWidth,					vfp );
	FPUTDWORD( p->dwHeight,					vfp );
	fwrite( (DWORD *)&p->dwReserved ,sizeof(DWORD), 4, vfp );
}

void AVI6::putAVISTRMHEADER6( AVISTRMHEADER6 *p )
{
	FPUTDWORD( p->fccType,					vfp );
	FPUTDWORD( p->fccHandler,				vfp );
	FPUTDWORD( p->dwFlags,					vfp );
	FPUTDWORD( p->dwPriority,				vfp );
	FPUTDWORD( p->dwInitialFrames,			vfp );
	FPUTDWORD( p->dwScale,					vfp );
	FPUTDWORD( p->dwRate,					vfp );
	FPUTDWORD( p->dwStart,					vfp );
	FPUTDWORD( p->dwLength,					vfp );
	FPUTDWORD( p->dwSuggestedBufferSize,	vfp );
	FPUTDWORD( p->dwQuality,				vfp );
	FPUTDWORD( p->dwSampleSize,				vfp );
	
	FPUTDWORD( p->rcFrame.left,		vfp );	// LONG
	FPUTDWORD( p->rcFrame.top,		vfp );	// LONG
	FPUTDWORD( p->rcFrame.right,	vfp );	// LONG
	FPUTDWORD( p->rcFrame.bottom,	vfp );	// LONG
}

// AVIINDEXENTRY����
void AVI6::putAVIINDEXENTRY6( AVIINDEXENTRY6 *p )
{
	FPUTDWORD( p->ckid,				vfp );
	FPUTDWORD( p->dwFlags,			vfp );
	FPUTDWORD( p->dwChunkOffset,	vfp );
	FPUTDWORD( p->dwChunkLength,	vfp );
} 

// WAVEFORMATEX����
void AVI6::putWAVEFORMATEX6( WAVEFORMATEX6 *p )
{
	FPUTWORD( p->wFormatTag,		vfp );
	FPUTWORD( p->nChannels,			vfp );
	FPUTDWORD(p->nSamplesPerSec,	vfp );
	FPUTDWORD(p->nAvgBytesPerSec,	vfp );
	FPUTWORD( p->nBlockAlign,		vfp );
	FPUTWORD( p->wBitsPerSample,	vfp );
	FPUTWORD( p->cbSize,			vfp );
	
	FPUTWORD( 0,            		vfp );
}




////////////////////////////////////////////////////////////////
// ������
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool AVI6::Init( void )
{
	PRINTD( GRP_LOG, "[MOVIE][Init]\n" );
	
	if( vfp ) fclose( vfp );
	vfp = NULL;
	
	ZeroMemory( &vmh, sizeof(MAINAVIHEADER6) );
	ZeroMemory( &vsh, sizeof(AVISTRMHEADER6) );
	ZeroMemory( &ash, sizeof(AVISTRMHEADER6) );
	ZeroMemory( &vbf, sizeof(BMPINFOHEADER6) );
	ZeroMemory( &awf, sizeof(WAVEFORMATEX6) );
	
	ABPP     = 32;
	
	if( Sbuf ) delete [] Sbuf;
	Sbuf     = NULL;
	
	PosMOVI  = 0;
	
	RiffSize = 0;
	MoviSize = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// �r�f�I�L���v�`���J�n
//
// ����:	filename	�o�̓t�@�C����
//			sw			�X�N���[���̕�
//			sh			�X�N���[���̍���
//			vrate		�t���[�����[�g(fps)
//			arate		�����T���v�����O���[�g(Hz)
//			bpp			�F�[�x(16,24,32)
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool AVI6::StartAVI( const char *filename, int sw, int sh, int vrate, int arate, int bpp )
{
	Init();
	
	// �C���[�W�f�[�^�o�b�t�@�擾
	Sbuf = new BYTE[sw*sh*sizeof(DWORD)];
	if( !Sbuf ) return false;
	
	vfp = FOPENEN( filename, "w+b" );
	if( !vfp ) return false;
	
	ABPP = bpp;
	
	// AVI���C���w�b�_�[�̐ݒ�
	// �t���[���Ԃ̊Ԋu���}�C�N���b�P�ʂŎw�肷��B���̒l�̓t�@�C���̑S�̂̃^�C�~���O�������B
	vmh.dwMicroSecPerFrame = 1000000 / vrate;
	// �t�@�C���̊T�Z�ő�f�[�^���[�g���w�肷��B���̒l��,���C���w�b�_�[����уX�g���[���w�b�_�[�`�����N�Ɋ܂܂��
	// ���̃p�����[�^�ɏ]���� AVI�V�[�P���X��\�����邽�߂�,�V�X�e�����������Ȃ���΂Ȃ�Ȃ����b�̃o�C�g���������B
	// (�Ƃ肠��������)
	vmh.dwMaxBytesPerSec = 0;
	// �t�@�C���ɑ΂���t���O���܂ށB�ȉ��̃t���O����`����Ă���B
	//  AVIF_HASINDEX AVI	�t�@�C���̏I���ɃC���f�b�N�X���܂� 'idx1'�`�����N�����邱�Ƃ������B
	//						�ǍD�ȃp�t�H�[�}���X�̂��߂ɂ�,���ׂĂ� AVI�t�@�C�����C���f�b�N�X���܂ޕK�v������B
	//  AVIF_MUSTUSEINDEX	�f�[�^�̃v���[���e�[�V�����̏��������肷�邽�߂�,�t�@�C�����̃`�����N�̕����I�ȏ����ł͂Ȃ�
	//						�C���f�b�N�X���g�p���邱�Ƃ������B���Ƃ���,������g���ăt���[���̃��X�g���쐬���ĕҏW���邱�Ƃ��ł���B
	//  AVIF_ISINTERLEAVED	AVI�t�@�C�����C���^�[���[�u����Ă��邱�Ƃ������B
	//  AVIF_WASCAPTUREFILE	AVI�t�@�C����,���A���^�C���r�f�I�̃L���v�`���p�ɓ��ʂɊ��蓖�Ă�ꂽ�t�@�C���ł��邱�Ƃ������B
	//						�A�v���P�[�V������,���̃t���O���ݒ肳�ꂽ�t�@�C�����I�[�o�[���C�h����O��,���[�U�[�Ɍx���𔭂���K�v������B
	//						�����,���[�U�[�����̃t�@�C�����f�t���O�����g���Ă���\������������ł���B
	//  AVIF_COPYRIGHTED	AVI�t�@�C���ɒ��쌠�̂���f�[�^����у\�t�g�E�F�A���܂܂�Ă��邱�Ƃ������B
	//						���̃t���O���g�p����Ă���ꍇ,�\�t�g�E�F�A�̓f�[�^�̕����������ׂ��ł͂Ȃ��B
	vmh.dwFlags = 0x00000110;	// AVIF_HASINDEX | AVIF_ISINTERLEAVED
	// �t�@�C�����̃f�[�^�̃t���[���̑������w�肷��B
	// (���)
	vmh.dwTotalFrames = 0;
	// �C���^�[���[�u���ꂽ�t�@�C���̊J�n�t���[�����w�肷��B�C���^�[���[�u���ꂽ�t�@�C���ȊO�ł�,�[�����w�肷��B
	// �C���^�[���[�u���ꂽ�t�@�C�����쐬����ꍇ,�t�@�C������ AVI�V�[�P���X�̊J�n�t���[�����O�ɂ���t���[������
	// ���̃����o�Ɏw�肷��B���̃����o�̓��e�Ɋւ���ڍׂɂ��Ă�
	// �wVideo for Windows Programmer's Guide�x�́uSpecial Information for Interleaved Files�v���Q�Ƃ��邱�ƁB
	vmh.dwInitialFrames = 0;
	// �t�@�C�����̃X�g���[�������w�肷��B���Ƃ���,�I�[�f�B�I�ƃr�f�I���܂ރt�@�C���ɂ� 2�̃X�g���[��������B
	vmh.dwStreams = 2;
	// AVI�t�@�C���̕����s�N�Z���P�ʂŎw�肷��B
	vmh.dwWidth  = sw;
	// AVI�t�@�C���̍������s�N�Z���P�ʂŎw�肷��B
	vmh.dwHeight = sh;
	// �t�@�C����ǂݎ�邽�߂̃o�b�t�@�T�C�Y���w�肷��B��ʂ�,���̃T�C�Y�̓t�@�C�����̍ő�̃`�����N���i�[����̂�
	// �\���ȑ傫���ɂ���B�[���ɐݒ肵����,����������l�ɐݒ肵���ꍇ,�Đ��\�t�g�E�F�A�͍Đ����Ƀ�������
	// �Ċ��蓖�Ă��Ȃ���΂Ȃ炸,�p�t�H�[�}���X���ቺ����B�C���^�[���[�u���ꂽ�t�@�C���̏ꍇ,�o�b�t�@�T�C�Y�̓`�����N�ł͂Ȃ�
	// ���R�[�h�S�̂�ǂݎ��̂ɏ\���ȑ傫���łȂ���΂Ȃ�Ȃ��B
	// (�Ƃ肠����1�t���[����)
	vmh.dwSuggestedBufferSize = vmh.dwWidth * vmh.dwHeight * ( ABPP / 8 );
	
	// AVI�X�g���[���w�b�_�[�̐ݒ�
	// �X�g���[���Ɋ܂܂��f�[�^�̃^�C�v���w�肷�� FOURCC ���܂ށB�r�f�I����уI�[�f�B�I�ɑ΂��Ĉȉ��̕W��AVI�l����`����Ă���B
	//  'vids'	�X�g���[���Ƀr�f�I�f�[�^���܂܂�邱�Ƃ������B�X�g���[���t�H�[�}�b�g�`�����N�ɂ�,�p���b�g�����܂ނ��Ƃ��\��
	//			BITMAPINFO �\���̂��܂܂��B
	//  'auds'	�X�g���[���ɃI�[�f�B�I�f�[�^���܂܂�邱�Ƃ������B�X�g���[���t�H�[�}�b�g�`�����N�ɂ�,WAVEFORMATEX
	//			�܂��� PCMWAVEFORMAT �\���̂��܂܂��B
	//  'txts'	�X�g���[���Ƀe�L�X�g �f�[�^���܂܂�邱�Ƃ������B
	vsh.fccType = CIDVIDS;
	ash.fccType = CIDAUDS;
	// ����̃f�[�^�n���h�������� FOURCC ���܂� (�I�v�V����)�B�f�[�^�n���h����,�X�g���[���ɑ΂��ēK�؂ȃn���h���ł���B
	// �I�[�f�B�I����уr�f�I�X�g���[���̏ꍇ��,�C���X�g�[���\�ȃR���v���b�T�܂��̓f�R���v���b�T���w�肷��B
	vsh.fccHandler = CIDDIB;
	ash.fccHandler = 0x00000001;	//(PCM)
	// �f�[�^ �X�g���[���ɑ΂���t���O���܂ށB�����̃t���O�̏�ʃ��[�h�̃r�b�g��,�X�g���[���Ɋ܂܂��f�[�^�̃^�C�v�ɌŗL�ł���B
	// �ȉ��̕W���t���O����`����Ă���B
	//  AVISF_DISABLED			���̃X�g���[�����f�t�H���g�ŗL���ɂ��Ȃ����Ƃ������B
	//  AVISF_VIDEO_PALCHANGES	���̃r�f�I�X�g���[���Ƀp���b�g�̕ύX���܂܂�邱�Ƃ������B���̃t���O��,�Đ��\�t�g�E�F�A�ɑ΂���
	//							�p���b�g���A�j���[�V��������K�v�����邱�Ƃ��x������B
	vsh.dwFlags = 0;
	ash.dwFlags = 0;
	// �X�g���[�� �^�C�v�̗D�揇�ʂ��w�肷��B���Ƃ���,�����̃I�[�f�B�I�X�g���[�����܂ރt�@�C���ł�,�D�揇�ʂ̍ł������X�g���[����
	// �f�t�H���g�̃X�g���[���ɂȂ�B
	vsh.dwPriority = 0;
	ash.dwPriority = 0;
	// �C���^�[���[�u���ꂽ�t�@�C����,�I�[�f�B�I�f�[�^���r�f�I�t���[������ǂ̂��炢�X�L���[����Ă��邩���w�肷��B
	// �ʏ��,�� 0.75 �b�ł���B�C���^�[���[�u���ꂽ�t�@�C�����쐬����ꍇ,�t�@�C������ AVI�V�[�P���X�̊J�n�t���[�����
	// �O�ɂ���t���[������,���̃����o�Ɏw�肷��B���̃����o�̓��e�Ɋւ���ڍׂɂ��Ă�,�wVideo for Windows Programmer's Guide�x��
	// �uSpecial Information for Interleaved Files�v���Q�Ƃ��邱�ƁB
	vsh.dwInitialFrames = 0;
	ash.dwInitialFrames = 0;
	// dwRate �ƂƂ��Ɏg�p����,���̃X�g���[�����g�p����^�C���X�P�[�����w�肷��BdwRate �� dwScale �Ŋ��邱�Ƃɂ��
	// 1 �b������̃T���v���������߂���B�r�f�I�X�g���[���̏ꍇ,���̃��[�g�̓t���[�����[�g�ɓ������Ȃ�B
	// �I�[�f�B�I�X�g���[���̏ꍇ,���̃��[�g�� nBlockAlign �o�C�g�̃I�[�f�B�I�f�[�^�ɕK�v�Ȏ��ԂɑΉ�����B
	// ����� PCM�I�[�f�B�I�̏ꍇ�̓T���v�����[�g�ɓ������Ȃ�B
	vsh.dwScale = 10000;
	ash.dwScale = 2;
	// dwScale ���Q�Ƃ��邱�ƁB
	vsh.dwRate = vsh.dwScale * vrate;
	ash.dwRate = ash.dwScale * arate;
	// AVI�t�@�C���̊J�n�^�C�����w�肷��B�P�ʂ�,���C���t�@�C���w�b�_�[�� dwRate ����� dwScale �����o�ɂ���Ē�`�����B
	// �ʏ�,����̓[���ł��邪,�t�@�C���Ɠ����ɊJ�n����Ȃ��X�g���[���ɑ΂��Ă�,�x�����Ԃ��w�肷�邱�Ƃ��ł���B
	vsh.dwStart = 0;
	ash.dwStart = 0;
	// ���̃X�g���[���̒������w�肷��B�P�ʂ�,�X�g���[���̃w�b�_�[�� dwRate ����� dwScale �����o�ɂ���Ē�`�����B
	// (���)
	vsh.dwLength = 0;
	ash.dwLength = 0;
	// ���̃X�g���[����ǂݎ�邽�߂ɕK�v�ȃo�b�t�@�̑傫�����w�肷��B�ʏ��,�X�g���[�����̍ő�̃`�����N�ɑΉ�����l�ł���B
	// �������o�b�t�@�T�C�Y���g�p���邱�Ƃ�,�Đ��̌��������܂�B�������o�b�t�@�T�C�Y���킩��Ȃ��ꍇ��,�[�����w�肷��B
	// (�Ƃ肠����1�t���[����)
	vsh.dwSuggestedBufferSize = vmh.dwWidth * vmh.dwHeight * ( ABPP / 8 );
	ash.dwSuggestedBufferSize = arate / vrate * 2;
	// �X�g���[�����̃f�[�^�̕i���������l���w�肷��B�i����,0�`10,000 �͈̔͂̒l�Ŏ������B���k�f�[�^�̏ꍇ,����͒ʏ�
	// ���k�\�t�g�E�F�A�ɓn�����i���p�����[�^�̒l�������B-1�ɐݒ肵���ꍇ,�h���C�o�̓f�t�H���g�̕i���l���g�p����B
	vsh.dwQuality = 0;
	ash.dwQuality = 0;
	// �f�[�^�� 1 �T���v���̃T�C�Y���w�肷��B�T���v���̃T�C�Y���ω�����ꍇ��,�[���ɐݒ肷��B���̒l���[���łȂ��ꍇ
	// �t�@�C�����ŕ����̃f�[�^�T���v���� 1�̃`�����N�ɃO���[�v���ł���B�[���̏ꍇ,�e�f�[�^�T���v��(�r�f�I�t���[���Ȃ�)
	// �͂��ꂼ��ʂ̃`�����N�Ɋ܂܂�Ȃ���΂Ȃ�Ȃ��B�r�f�I�X�g���[���̏ꍇ,���̒l�͒ʏ�[���ł��邪
	// ���ׂẴr�f�I�t���[���������T�C�Y�ł����,�[���ȊO�̒l�ɂ��ł���B�I�[�f�B�I�X�g���[���̏ꍇ,���̒l�̓I�[�f�B�I��
	// �L�q���� WAVEFORMATEX �\���̂� nBlockAlign �����o�Ɠ����łȂ���΂Ȃ�Ȃ��B
	vsh.dwSampleSize = 0;
	ash.dwSampleSize = 2;
	// AVI ���C���w�b�_�[�\���̂� dwWidth ����� dwHeight �����o�ɂ���Ďw�肳��铮���`���̃e�L�X�g�܂��̓r�f�I�X�g���[����
	// �΂���]�����`���w�肷��B�ʏ�,rcFrame �����o��,�����̃r�f�I�X�g���[�����T�|�[�g���邽�߂Ɏg�p�����B���̋�`��
	// �����`�ɑΉ�������W�ɐݒ肵��,�����`�S�̂��X�V����B���̃����o�̒P�ʂ̓s�N�Z���ł���B�]�����`�̍������
	// �����`�̍��������̑��Ύw��ƂȂ�B
	vsh.rcFrame.left   = 0;
	vsh.rcFrame.top    = 0;
	vsh.rcFrame.right  = vmh.dwWidth  - 1;
	vsh.rcFrame.bottom = vmh.dwHeight - 1;
	
	// BMPINFOHEADER6�\���̂̐ݒ�
	// �\���̂��K�v�Ƃ���o�C�g�����w�肷��B
	vbf.biSize = sizeof(BMPINFOHEADER6);
	// �r�b�g�}�b�v�̕����s�N�Z���P�ʂŎw�肷��B
	vbf.biWidth  = vmh.dwWidth;
	// �r�b�g�}�b�v�̍������s�N�Z���P�ʂŎw�肷��BbiHeight �̒l�����ł���ꍇ,�r�b�g�}�b�v�̓{�g���A�b�vDIB
	// (device-independent bitmap : �f�o�C�X�Ɉˑ����Ȃ��r�b�g�}�b�v) �ł���,�����������_�ƂȂ�BbiHeight �̒l�����ł���ꍇ
	// �r�b�g�}�b�v�̓g�b�v�_�E��DIB �ł���,����������_�ƂȂ�B
	vbf.biHeight = vmh.dwHeight;
	// �^�[�Q�b�g �f�o�C�X�ɑ΂���ʂ̐����w�肷��B����͕K�� 1 �ɐݒ肷��B
	vbf.biPlanes = 1;
	// 1 �s�N�Z��������̃r�b�g�����w�肷��B���k�t�H�[�}�b�g�ɂ���Ă�,�s�N�Z���̐F�𐳂����f�R�[�h���邽�߂ɂ��̏�񂪕K�v�ł���B
	vbf.biBitCount = ABPP;
	// �g�p����Ă���,�܂��͗v������Ă��鈳�k�̃^�C�v���w�肷��B�����̈��k�t�H�[�}�b�g�ƐV�������k�t�H�[�}�b�g�̗�����
	// ���̃����o���g�p����B
	vbf.biCompression = 0;	// 0:BI_RGB
	// �C���[�W�̃T�C�Y���o�C�g�P�ʂŎw�肷��B�񈳏kRGB�r�b�g�}�b�v�̏ꍇ��,0 �ɐݒ�ł���B
	vbf.biSizeImage = vmh.dwWidth * vmh.dwHeight * ( ABPP / 8 );
	// �r�b�g�}�b�v�̃^�[�Q�b�g�f�o�C�X�̐����𑜓x�� 1���[�g��������̃s�N�Z���P�ʂŎw�肷��B�A�v���P�[�V�����͂��̒l��
	// �g�p����,���\�[�X�O���[�v�̒����猻�݂̃f�o�C�X�̓����ɍł��K������r�b�g�}�b�v��I�����邱�Ƃ��ł���B
	vbf.biXPelsPerMeter = 0;
	// �r�b�g�}�b�v�̃^�[�Q�b�g �f�o�C�X�̐����𑜓x�� 1���[�g��������̃s�N�Z���P�ʂŎw�肷��B
	vbf.biYPelsPerMeter = 0;
	// �J���[ �e�[�u�����̃J���[ �C���f�b�N�X�̂���,�r�b�g�}�b�v���Ŏ��ۂɎg�p�����C���f�b�N�X�̐����w�肷��B���̒l��
	// �[���̏ꍇ,�r�b�g�}�b�v��,biCompression �Ŏw�肳��鈳�k���[�h�ɑ΂���,biBitCount �����o�̒l�ɑΉ�����ő�F�����g�p����B
	// Mac�̏ꍇ��256�łȂ��ƐF�����������Ȃ�炵���B(by ���c����&Windy����)
	// �ł�24bit�̎���0�ɂ��Ă����Ȃ��Ƃ����ƍĐ�����Ȃ��݂����B
	vbf.biClrUsed = 0;
	// �r�b�g�}�b�v��\�����邽�߂ɏd�v�Ƃ݂Ȃ����J���[�C���f�b�N�X�����w�肷��B���̒l���[���̏ꍇ��,���ׂĂ̐F���d�v�Ƃ݂Ȃ����B
	vbf.biClrImportant = 0;
	
	// WAVEFORMATEX6�\���̂̐ݒ�
	// �I�[�f�B�I�X�g���[���̃I�[�f�B�I�g�`�^�C�v���`����B�t�H�[�}�b�g�^�O�̊��S�ȃ��X�g��,Microsoft Visual C++ �����
	// ���� Microsoft ���i�ɕt������ Mmreg.h �w�b�_�[ �t�@�C���ɂ���B
	awf.wFormatTag = 0x0001;	// WAVE_FORMAT_PCM
	// �I�[�f�B�I�X�g���[�����̃`�����l�������w�肷��B1�̓��m,2�̓X�e���I�������B
	awf.nChannels = 1;
	// �I�[�f�B�I�X�g���[���̃T���v�����[�g���g�����T���v��/�b (Hz) �Ŏw�肷��B���Ƃ���,11,025,22,050,�܂��� 44,100�B
	awf.nSamplesPerSec = arate;
	// ���σf�[�^���[�g���w�肷��B�Đ��\�t�g�E�F�A�͂��̒l���g���ăo�b�t�@�T�C�Y�����ς��邱�Ƃ��ł���B
	awf.nAvgBytesPerSec = arate * 2;
	// �f�[�^�̃u���b�N �A���C�������g���o�C�g�P�ʂŎw�肷��B�Đ��\�t�g�E�F�A�� 1��ɏ�������f�[�^�̃o�C�g����
	// nBlockAlign �̐����{�łȂ���΂Ȃ�Ȃ�����,nBlockAlign �̒l���g�p���ăo�b�t�@�̃A���C�������g���s�����Ƃ��ł���B
	awf.nBlockAlign = 2;
	// �`�����l���f�[�^���Ƃ� 1�T���v��������̃r�b�g�����w�肷��B�e�`�����l���̃T���v���𑜓x�͓����ł���Ɖ��肳���B
	// ���̃t�B�[���h���K�v�Ȃ��ꍇ��,�[���ɐݒ肷��B
	awf.wBitsPerSample = 16;
	// �t�H�[�}�b�g�w�b�_�[���̒ǉ����̃T�C�Y���o�C�g�P�ʂŎw�肷��B����ɂ�,WAVEFORMATEX �\���̂̃T�C�Y�͊܂܂�Ȃ��B
	// ���Ƃ���,wFormatTag WAVE_FORMAT_IMA_ADPCM �ɑΉ����� wave�t�H�[�}�b�g�̏ꍇ,cbSize ��
	// sizeof(IMAADPCMWAVEFORMAT) - sizeof(WAVEFORMATEX) �Ƃ��Čv�Z����,���ʂ� 2�ƂȂ�B
	awf.cbSize = 0;
	
	// �w�b�_�`�����N���o��
	WriteHeader();
	
	// �I�[�f�B�I�o�b�t�@�쐬
	ABuf.InitBuffer( arate / vrate * 2 );
	// �J�E���^������
	anum = 0;
	
	
	return true;
}


////////////////////////////////////////////////////////////////
// �r�f�I�L���v�`����~
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void AVI6::StopAVI( void )
{
	if( !vfp ) return;
	
	// ���t���[����
	vsh.dwLength = vmh.dwTotalFrames;
	ash.dwLength = anum;
	
	// ���̎��_�Ńt�@�C���|�C���^�͖����ɂ���͂������O�̂���
	fseek( vfp, 0, SEEK_END );
	
	// Movi�`�����N�̃T�C�Y�擾
	MoviSize = ftell( vfp ) - PosMOVI;
	
	// �C���f�b�N�X�`�����N�o��
	WriteIndexr();
	
	RiffSize = ftell( vfp ) - 8;
	
	// �w�b�_�`�����N�ď��o��
	WriteHeader();
	
	fclose( vfp );
	vfp = NULL;
	
	if( Sbuf ) delete [] Sbuf;
	Sbuf = NULL;
}


////////////////////////////////////////////////////////////////
// �r�f�I�L���v�`����?
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:�L���v�`���� false:�Ђ�
////////////////////////////////////////////////////////////////
bool AVI6::IsAVI( void )
{
	return vfp ? true : false;
}


////////////////////////////////////////////////////////////////
// AVI1�t���[�����o��
//
// ����:	wh		�E�B���h�E�n���h��
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool AVI6::AVIWriteFrame( HWINDOW wh )
{
	if( !vfp || !wh ) return false;
	
//	int xx = min( sbuf->Width(),  vbf.biWidth  );
//	int yy = min( sbuf->Height(), vbf.biHeight );
	int xx = vbf.biWidth;
	int yy = vbf.biHeight;
	
	FPUTDWORD( CID00DB, vfp );
	FPUTDWORD( vbf.biSizeImage, vfp );
	
	VRect ss;
	ss.x = 0;
	ss.y = 0;
	ss.w = xx;
	ss.h = yy;
	if( !OSD_GetWindowImage( wh, (void **)&Sbuf, &ss ) ) return false;
	
	switch( ABPP ){
	case 16:	// 16bit�̏ꍇ
		for( int y = yy - 1; y >= 0; y-- ){
			DWORD *src = (DWORD *)Sbuf + vmh.dwWidth * y;
			for( int x=0; x < xx; x++ ){
				WORD dat = (((*src)&RMASK32)>>(RSHIFT32+3))<<10 |
						   (((*src)&GMASK32)>>(GSHIFT32+3))<<5 |
						   (((*src)&BMASK32)>>(BSHIFT32+3));
				FPUTWORD( dat, vfp );
				src++;
			}
		}
		break;
		
	case 24:	// 24bit�̏ꍇ
		for( int y = yy - 1; y >= 0; y-- ){
			DWORD *src = (DWORD *)Sbuf + vmh.dwWidth * y;
			for( int x=0; x < xx; x++ ){
				FPUTBYTE( ((*src)&BMASK32)>>BSHIFT32, vfp );
				FPUTBYTE( ((*src)&GMASK32)>>GSHIFT32, vfp );
				FPUTBYTE( ((*src)&RMASK32)>>RSHIFT32, vfp );
				src++;
			}
		}
		break;
		
	case 32:	// 32bit�̏ꍇ
		for( int y = yy - 1; y >= 0; y-- )
			fwrite( (BYTE *)((DWORD *)Sbuf + vmh.dwWidth * y), sizeof(DWORD), xx, vfp );
	}
	
	// ���t���[������1���₷
	vmh.dwTotalFrames++;
	
	// �I�[�f�B�I�o��
	if( ABuf.ReadySize() > 0 ){
		anum += ABuf.ReadySize();
		FPUTDWORD( CID01WB, vfp );
		FPUTDWORD( ABuf.ReadySize()*2, vfp );
		while( ABuf.ReadySize() > 0 ){
			short dat = ABuf.Get();
			FPUTWORD( dat, vfp );
		}
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// �I�[�f�B�I�o�b�t�@�擾
//
// ����:	�Ȃ�
// �Ԓl:	cRing *		�o�b�t�@�I�u�W�F�N�g�ւ̃|�C���^
////////////////////////////////////////////////////////////////
cRing *AVI6::GetAudioBuffer( void )
{
	return &ABuf;
}


////////////////////////////////////////////////////////////////
// �w�b�_�`�����N���o��
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool AVI6::WriteHeader( void )
{
	if( !vfp ) return false;
	
	DWORD SIZESTRLV	= sizeof(AVISTRMHEADER6) + sizeof(BMPINFOHEADER6) + sizeof(DWORD)*5 + (ABPP==8 ? (sizeof(RGBPAL6)*256) : 0);
	DWORD SIZESTRLA	= sizeof(AVISTRMHEADER6) + sizeof(WAVEFORMATEX6)  + sizeof(DWORD)*5;
	DWORD SIZEHDRL  = SIZESTRLV + SIZESTRLA  + sizeof(MAINAVIHEADER6) + sizeof(DWORD)*7;
	DWORD SIZEJUNK  = 2048 - SIZEHDRL - sizeof(DWORD)*10;
	
	fseek( vfp, 0, SEEK_SET );
	
	fputs( "RIFF", vfp );
	FPUTDWORD( RiffSize, vfp );
		fputs( "AVI ", vfp );
			fputs( "LIST", vfp );
			FPUTDWORD( SIZEHDRL, vfp );
			fputs( "hdrl", vfp );
				fputs( "avih", vfp );
				FPUTDWORD( sizeof(MAINAVIHEADER6), vfp );
				putMAINAVIHEADER6( &vmh );
				
				// �r�f�I
				fputs( "LIST", vfp );
				FPUTDWORD( SIZESTRLV, vfp );
				fputs( "strl", vfp );
					fputs( "strh", vfp );
					FPUTDWORD( sizeof(AVISTRMHEADER6), vfp );
					putAVISTRMHEADER6( &vsh );
					
					fputs( "strf", vfp );
					FPUTDWORD( sizeof(BMPINFOHEADER6), vfp );
					putBMPINFOHEADER6( &vbf );
					
				// �I�[�f�B�I
				fputs( "LIST", vfp );
				FPUTDWORD( SIZESTRLA, vfp );
				fputs( "strl", vfp );
					fputs( "strh", vfp );
					FPUTDWORD( sizeof(AVISTRMHEADER6), vfp );
					putAVISTRMHEADER6( &ash );
					
					fputs( "strf", vfp );
					FPUTDWORD( sizeof(WAVEFORMATEX6), vfp );
					putWAVEFORMATEX6( &awf );
				
			fputs( "JUNK", vfp );
			FPUTDWORD( SIZEJUNK, vfp );
			for( DWORD i=0; i<SIZEJUNK; i++ ) FPUTBYTE( 0, vfp );
			
			fputs( "LIST", vfp );
			FPUTDWORD( MoviSize, vfp );
			
			PosMOVI = ftell( vfp );
			
			fputs( "movi", vfp );
	
	fseek( vfp, 0, SEEK_END );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �C���f�b�N�X�`�����N���o��
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool AVI6::WriteIndexr( void )
{
	if( !vfp ) return false;
	
	DWORD frames = vmh.dwTotalFrames * 2;
	
	fseek( vfp, 0, SEEK_END );
	
	// �C���f�b�N�X�`�����N�o��
	fputs( "idx1", vfp );
	FPUTDWORD( sizeof(AVIINDEXENTRY6) * frames, vfp );
	DWORD ipos = 4;
	for( DWORD i=0; i<frames; i++ ){
		AVIINDEXENTRY6 idx;
		
		fseek( vfp, PosMOVI + ipos, SEEK_SET );
		idx.ckid          = FGETDWORD( vfp );
//		if( idx.ckid == CID01WB ) idx.dwFlags = 0x00000000;
//		else                      idx.dwFlags = 0x00000010;	// AVIIF_KEYFRAME
		idx.dwFlags = 0x00000010;	// AVIIF_KEYFRAME
		idx.dwChunkOffset = ipos;
		idx.dwChunkLength = FGETDWORD( vfp );
		fseek( vfp, 0, SEEK_END );
		putAVIINDEXENTRY6( &idx );
		ipos += idx.dwChunkLength + 8;
	}
	
	return true;
}
