#ifndef CONSOLE_H_INCLUDED
#define CONSOLE_H_INCLUDED

#include "typedef.h"
#include "vsurface.h"

#define	FSIZE		(6)	// ���p�t�H���g�̕�

// �F��`
#if INBPP == 8	// 8bit

#define	FC_BLACK	0
#define	FC_DBLUE	1
#define	FC_DGREEN	2
#define	FC_DCYAN	3
#define	FC_DRED		4
#define	FC_DMAGENTA	5
#define	FC_DYELLOW	6
#define	FC_GRAY		7
#define	FC_DGRAY	8
#define	FC_BLUE		9
#define	FC_GREEN	10
#define	FC_CYAN		11
#define	FC_RED		12
#define	FC_MAGENTA	13
#define	FC_YELLOW	14
#define	FC_WHITE	15

#else			// 32bit

#define	FC_BLACK	0x00000000
#define	FC_DBLUE	(0x7f7f7f7f &  BMASK32)
#define	FC_DGREEN	(0x7f7f7f7f &  GMASK32)
#define	FC_DCYAN	(0x7f7f7f7f & (GMASK32|BMASK32))
#define	FC_DRED		(0x7f7f7f7f &  RMASK32)
#define	FC_DMAGENTA	(0x7f7f7f7f & (RMASK32|BMASK32))
#define	FC_DYELLOW	(0x7f7f7f7f & (RMASK32|GMASK32))
#define	FC_GRAY		(0x7f7f7f7f & (RMASK32|GMASK32|BMASK32))
#define	FC_DGRAY	(0x3f3f3f3f & (RMASK32|GMASK32|BMASK32))
#define	FC_BLUE		(0xffffffff &  BMASK32)
#define	FC_GREEN	(0xffffffff &  GMASK32)
#define	FC_CYAN		(0xffffffff & (GMASK32|BMASK32))
#define	FC_RED		(0xffffffff &  RMASK32)
#define	FC_MAGENTA	(0xffffffff & (RMASK32|BMASK32))
#define	FC_YELLOW	(0xffffffff & (RMASK32|GMASK32))
#define	FC_WHITE	(0xffffffff & (RMASK32|GMASK32|BMASK32))

#endif



////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class JFont {
protected:
	static VSurface *ZFont;		// �S�p�t�H���g�f�[�^�T�[�t�F�X�ւ̃|�C���^
	static VSurface *HFont;		// ���p�t�H���g�f�[�^�T�[�t�F�X�ւ̃|�C���^
	
	static int zWidth, zHeight;	// �S�p�����̕�,����
	static int hWidth, hHeight;	// ���p�����̕�,����
	
public:
	JFont();									// �R���X�g���N�^
	~JFont();									// �f�X�g���N�^
	
	static bool OpenFont( char *, char * );		// �t�@�C���t�@�C�����J��
	static void CloseFont();					// �t�H���g��j������
	
	static int FontWidth(){ return hWidth; }	// �t�H���g�̕��擾(���p)
	static int FontHeight(){ return hHeight; }	// �t�H���g�̍����擾
	
	#if INBPP == 8	// 8bit
	void PutCharh( VSurface *, int, int, BYTE, BYTE,  BYTE  );	// ���p�����`��
	void PutCharz( VSurface *, int, int, WORD, BYTE,  BYTE  );	// �S�p�����`��
	#else			// 32bit
	void PutCharh( VSurface *, int, int, BYTE, DWORD, DWORD );	// ���p�����`��
	void PutCharz( VSurface *, int, int, WORD, DWORD, DWORD );	// �S�p�����`��
	#endif

};


class ZCons : public JFont, public VSurface {
protected:
	VRect con;					// �`��͈�
	int Xmax,Ymax;				// �c���ő啶����(���p)
	int x,y;					// �J�[�\���ʒu
	#if INBPP == 8	// 8bit
	BYTE fgc,bgc;				// �`��F�Ɣw�i�F
	#else			// 32bit
	DWORD fgc,bgc;				// �`��F�Ɣw�i�F
	#endif
	char Caption[129];			// �L���v�V����
	
	void DrawFrame();							// �g�`��
	void ScrollUp();							// �X�N���[���A�b�v
	
public:
	ZCons();									// �R���X�g���N�^
	virtual ~ZCons();							// �f�X�g���N�^
	
	#if INBPP == 8	// 8bit
	bool Init   ( int, int, const char *, BYTE =FC_WHITE, BYTE =FC_BLACK );	// ������(�������ŃT�C�Y�w��)
	bool InitRes( int, int, const char *, BYTE =FC_WHITE, BYTE =FC_BLACK );	// ������(�𑜓x�ŃT�C�Y�w��)
	void SetColor( BYTE, BYTE );				// �`��F�ݒ�
	void SetColor( BYTE );
	#else			// 32bit
	bool Init   ( int, int, const char *, DWORD=FC_WHITE, DWORD=FC_BLACK );	// ������(�������ŃT�C�Y�w��)
	bool InitRes( int, int, const char *, DWORD=FC_WHITE, DWORD=FC_BLACK );	// ������(�𑜓x�ŃT�C�Y�w��)
	void SetColor( DWORD, DWORD );				// �`��F�ݒ�
	void SetColor( DWORD );
	#endif
	
	void Locate( int, int );					// �J�[�\���ʒu�ݒ�
	void LocateR( int, int );					// �J�[�\���ʒu�ݒ�(���΍��W)
	void Cls();									// ��ʏ���
	
	void PutCharH( BYTE );						// ���p1�����`��
	void PutCharZ( WORD );						// �S�p1�����`��
	
	void Print( const char *, ... );			// �����t������`��(���䕶����Ή�)
	void Printf( const char *, ... );			// �����t������`��(���䕶���Ή�)
	void Printfr( const char *, ... );			// �����t������`��(�E�l��)
	
	int GetXline();								// ���ő啶�����擾
	int GetYline();								// �c�ő啶�����擾
};


#endif	// CONSOLE_H_INCLUDED
