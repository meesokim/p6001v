#ifndef MOVIE_H_INCLUDED
#define MOVIE_H_INCLUDED

#include "sound.h"
#include "vsurface.h"

// RECT����
typedef struct {
	long left;		// LONG
	long top;		// LONG
	long right;		// LONG
	long bottom;	// LONG
} RECT6;

// RGBQUAD����
typedef struct {
	BYTE	b;
	BYTE	g;
	BYTE	r;
	BYTE	reserved;
} RGBPAL6;

// BITMAPINFOHEADER����
typedef struct {
	DWORD  biSize;
	long   biWidth;			// LONG
	long   biHeight;		// LONG
	WORD   biPlanes;
	WORD   biBitCount;
	DWORD  biCompression;
	DWORD  biSizeImage;
	long   biXPelsPerMeter;	// LONG
	long   biYPelsPerMeter;	// LONG
	DWORD  biClrUsed;
	DWORD  biClrImportant;
} BMPINFOHEADER6;

// MainAVIHeader����
typedef struct {
	DWORD dwMicroSecPerFrame;
	DWORD dwMaxBytesPerSec;
	DWORD dwReserved1;
	DWORD dwFlags;
	DWORD dwTotalFrames;
	DWORD dwInitialFrames;
	DWORD dwStreams;
	DWORD dwSuggestedBufferSize;
	DWORD dwWidth;
	DWORD dwHeight;
	DWORD dwReserved[4];
} MAINAVIHEADER6;

// AVIStreamHeader����
typedef struct {
	DWORD fccType;		// FOURCC
	DWORD fccHandler;	// FOURCC
	DWORD dwFlags;
	DWORD dwPriority;
	DWORD dwInitialFrames;
	DWORD dwScale;
	DWORD dwRate;
	DWORD dwStart;
	DWORD dwLength;
	DWORD dwSuggestedBufferSize;
	DWORD dwQuality;
	DWORD dwSampleSize;
	RECT6 rcFrame;
} AVISTRMHEADER6;

// AVIINDEXENTRY����
typedef struct {
	DWORD ckid;
	DWORD dwFlags;
	DWORD dwChunkOffset;
	DWORD dwChunkLength;
} AVIINDEXENTRY6;

// WAVEFORMATEX����
typedef struct {
	WORD  wFormatTag;
	WORD  nChannels;
	DWORD nSamplesPerSec;
	DWORD nAvgBytesPerSec;
	WORD nBlockAlign;
	WORD  wBitsPerSample;
	WORD  cbSize;
} WAVEFORMATEX6;



////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class AVI6 {
protected:
	FILE *vfp;
	MAINAVIHEADER6 vmh;
	AVISTRMHEADER6 vsh, ash;
	BMPINFOHEADER6 vbf;
	WAVEFORMATEX6 awf;
	
	int ABPP;					// �F�[�x (16,24,32)
	BYTE *Sbuf;					// �C���[�W�f�[�^�o�b�t�@�|�C���^
	
	DWORD PosMOVI;
	
	DWORD RiffSize;
	DWORD MoviSize;
	
	cRing ABuf;					// �I�[�f�B�I�o�b�t�@
	DWORD anum;					// �I�[�f�B�I�T���v�����J�E���g�p
	
	bool WriteHeader();						// �w�b�_�`�����N���o��
	bool WriteIndexr();						// �C���f�b�N�X�`�����N���o��
	
	void putBMPINFOHEADER6( BMPINFOHEADER6 * );
	void putMAINAVIHEADER6( MAINAVIHEADER6 * );
	void putAVISTRMHEADER6( AVISTRMHEADER6 * );
	void putAVIINDEXENTRY6( AVIINDEXENTRY6 * );
	void putWAVEFORMATEX6( WAVEFORMATEX6 * );
	
public:
	AVI6();									// �R���X�g���N�^
	~AVI6();								// �f�X�g���N�^
	
	bool Init();							// ������
	
	bool StartAVI( const char *, int, int, int, int, int );	// �r�f�I�L���v�`���J�n
	
	void StopAVI();							// �r�f�I�L���v�`����~
	bool IsAVI();							// �r�f�I�L���v�`����?
	
	bool AVIWriteFrame( HWINDOW );			// AVI1�t���[�����o��
	
	cRing *GetAudioBuffer();				// �I�[�f�B�I�o�b�t�@�擾
};


#endif	// MOVIE_H_INCLUDED
