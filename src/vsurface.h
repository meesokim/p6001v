#ifndef VSURFACE_H_INCLUDED
#define VSURFACE_H_INCLUDED


#include "typedef.h"

// �T�[�t�F�X�̎d�l
//  �F�[�x��32bit�̂�
//  �ő�T�C�Y��65536x65536

#define	AMASK32		0xff000000
#define	RMASK32		0x00ff0000
#define	GMASK32		0x0000ff00
#define	BMASK32		0x000000ff

#define	ASHIFT32	24
#define	RSHIFT32	16
#define	GSHIFT32	8
#define	BSHIFT32	0

struct COLOR24 {
	BYTE r;
	BYTE g;
	BYTE b;
	BYTE reserved;
};

// ��`�̈�\����
struct VRect{
	int x, y;		// �`��̈���W
	int w, h;		// �`��̈�T�C�Y
	
	VRect() : x(0), y(0), w(0), h(0) {}
};

struct VPalette{
	int ncols;
	COLOR24 *colors;
	
	VPalette() : ncols(0), colors(NULL) {}
};

#define COL2DW(c)	(DWORD)(((DWORD)c.reserved<<ASHIFT32)|((DWORD)c.r<<RSHIFT32)|((DWORD)c.g<<GSHIFT32)|((DWORD)c.b<<BSHIFT32))


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class VSurface {
protected:
	int w, h;					// ��,����
	int pitch;					// 1���C����byte��
	void *pixels;				// �s�N�Z���f�[�^�ւ̃|�C���^
	int xscale;					// ���{��(1:���{ 2:2�{)
	VRect rect;					// �`��̈�
	
	static DWORD col32[256];	// 32bit�J���[�e�[�u��
	
public:
	VSurface();										// �R���X�g���N�^
	virtual ~VSurface();							// �f�X�g���N�^
	
	virtual bool InitSurface( int, int, int=1 );	// ������(�T�[�t�F�X�T�C�Y�w��)
	void SetRect( int, int, int, int );				// �`��̈�ݒ�
	VRect *GetRect();								// �`��̈�擾
	
	int Width() const;								// ���擾
	int Height() const;								// �����擾
	int Pitch() const;								// 1���C����byte���擾
	int XScale() const;								// ���{���擾
	void *GetPixels() const;						// �s�N�Z���f�[�^�ւ̃|�C���^�擾
	
	#if INBPP == 8	// 8bit
	void PSet( int, int, BYTE );					// �_��`��
	BYTE PGet( int, int );							// �F���擾
	void Fill( BYTE, VRect * = NULL );				// ��`�̈��h��Ԃ�
	#else			// 32bit
	void PSet( int, int, DWORD );					// �_��`��
	DWORD PGet( int, int );							// �F���擾
	void Fill( DWORD, VRect * = NULL );				// ��`�̈��h��Ԃ�
	#endif
	
	void Blit( VRect *, VSurface *, VRect * );		// �]������
	
	static void SetColor( int, DWORD );				// 32bit�J���[�e�[�u���ݒ�
	static DWORD GetColor( int );					// 32bit�J���[�擾
};


#endif	// VSURFACE_H_INCLUDED
