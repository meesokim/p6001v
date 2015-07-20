#ifndef MC6847_H_INCLUDED
#define MC6847_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class MC6847core {
protected:
	BYTE COL_AN[5];			// �J���[�R�[�h(�A���t�@�j���[�����b�N)
	BYTE COL_SG[9];			// �J���[�R�[�h(�Z�~�O���t�B�b�N)
	BYTE COL_CG[10][8];		// �J���[�R�[�h(�J���[�O���t�B�b�N)
	BYTE COL_RG[2][2];		// �J���[�R�[�h(���m�N���O���t�B�b�N)
	// for 62,66,64,68 -----------------------------------------------------------------------
	BYTE COL_AN2[16];		// �J���[�R�[�h(�A���t�@�j���[�����b�N 60m)
	BYTE COL_CG2[2][16];	// �J���[�R�[�h(�J���[�O���t�B�b�N 60m)
	// ---------------------------------------------------------------------------------------
	
	static const BYTE NJM_TBL[][2];	// �F�ɂ��݃J���[�R�[�h�e�[�u��
	
	bool CrtDisp;			// CRT�\�����			true:�\��	false:��\��
	bool BusReq;			// �o�X���N�G�X�g		true:ON		false:OFF
	bool N60Win;			// �E�B���h�E�T�C�Y		true:N60	false:N60m
	int Mode4Col;			// ���[�h4�J���[���[�h	0:���m 1:��/�� 2:��/�� 3:�s���N/�� 4:��/�s���N
	
	WORD VAddr;				// �����A�h���X�J�E���^
	WORD HAddr;				// �����A�h���X�J�E���^
	int RowCntA;			// �\�����C���J�E���^(�A���t�@�j���[�����b�N,�Z�~�O��)
	int RowCntG;			// �\�����C���J�E���^(�O���t�B�b�N)
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	bool CharMode;			// �e�L�X�g�\�����[�h	true:�L�����N�^				false:�O���t�B�b�N
	bool GraphMode;			// �O���t�B�b�N�𑜓x	true:160*200(SR:320*200)	false:320*200(SR:640*200)
	int Css1;				// �F�̑g�ݍ��킹
	int Css2;
	int Css3;
	// ---------------------------------------------------------------------------------------
	
	// for 64,68 -----------------------------------------------------------------------------
	bool SRmode;			// SR���[�h�t���O 			true:SR-BASIC	false:��BASIC
	bool SRBusReq;			// SR�o�X���N�G�X�g�t���O	true:�L��		false:����
	bool SRBitmap;			// SR�r�b�g�}�b�v�t���O		true:�L��		false:����
	bool SRBMPage;			// SR�r�b�g�}�b�v�y�[�W		true:���32KB	false:����32KB
	bool SRLine204;			// SR�O���t�B�b�N���C����	true:204���C��	false:200���C��
	bool SRCharLine;		// SR�e�L�X�g�s��			true:20�s		false:25�s
	bool SRCharWidth;		// SR�e�L�X�g������			true:40����		false:80����
	
	BYTE SRTextAddr;		// SR�e�L�X�gVRAM�A�h���X(0H-FH)
	WORD SRRollX;			// �h�b�g�X�N���[�� X���W
	WORD SRRollY;			// �h�b�g�X�N���[�� Y���W
	WORD SRVramAddrY;		// �O���t�B�b�NVRAM Y���W
	// ---------------------------------------------------------------------------------------
	
	// �A�g���r���[�g�f�[�^
	BYTE AT_AG;
	BYTE AT_AS;
	BYTE AT_IE;
	BYTE AT_GM;
	BYTE AT_CSS;
	BYTE AT_INV;
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	void SetCss( BYTE );						// �F�̑g�����w��
	virtual void SetCrtControler( BYTE );		// CRT�R���g���[�����[�h�ݒ�
	// ---------------------------------------------------------------------------------------
	
	int GetW() const;							// �o�b�N�o�b�t�@���擾(�K��l)
	int GetH() const;							// �o�b�N�o�b�t�@�����擾(�K��l)
	
	virtual BYTE *GetBufAddr() const = 0;		// �o�b�t�@�A�h���X�擾
	virtual int GetBufPitch() const = 0;		// �o�b�t�@�s�b�`(1Line�o�C�g��)�擾
	
	virtual void LatchGMODE() = 0;				// �A�g���r���[�g�f�[�^���b�`(�O���t�B�b�N���[�h�̂�)
	virtual void LatchAttr() = 0;				// �A�g���r���[�g�f�[�^���b�`
	virtual BYTE GetAttr() const = 0;			// �A�g���r���[�g�f�[�^�擾
	virtual BYTE GetVram() const = 0;			// VRAM�f�[�^�擾
	virtual BYTE GetFont1( WORD ) const = 0;	// Font1�f�[�^�擾
	virtual BYTE GetFont2( WORD ) const = 0;	// Font2�f�[�^�擾
	virtual BYTE GetFont3( WORD ) const = 0;	// Font3�f�[�^�擾
	BYTE GetSemi4( BYTE ) const;				// �Z�~�O��4�f�[�^�擾
	virtual BYTE GetSemi6( BYTE ) const;		// �Z�~�O��6�f�[�^�擾
	
public:
	MC6847core();								// �R���X�g���N�^
	virtual ~MC6847core();						// �f�X�g���N�^
	
	virtual void UpdateBackBuf() = 0;			// �o�b�N�o�b�t�@�X�V
	
	bool GetCrtDisp() const;					// CRT�\����Ԏ擾
	void SetCrtDisp( bool );					// CRT�\����Ԑݒ�
	
	bool GetWinSize() const;					// �E�B���h�E�T�C�Y�擾
	
	int GetMode4Color() const;					// ���[�h4�J���[���[�h�擾
	void SetMode4Color( int );					// ���[�h4�J���[���[�h�ݒ�
};


class MC6847 : public virtual MC6847core {
protected:
	static const BYTE VDGfont[];				// VDG Font data
	
	void Draw1line1( int );						// 1���C���`��(N60)
	
	#if INBPP == 8	// 8bit
	BYTE GetBcol() const;						// �{�[�_�[�J���[�擾
	#else			// 32bit
	DWORD GetBcol() const;						// �{�[�_�[�J���[�擾
	#endif
	
public:
	MC6847();									// �R���X�g���N�^
	virtual ~MC6847();							// �f�X�g���N�^
	
	void UpdateBackBuf();						// �o�b�N�o�b�t�@�X�V
};


class PCZ80_07 : public virtual MC6847core {
protected:
	BYTE GetSemi6( BYTE ) const;				// �Z�~�O��6�f�[�^�擾
	
	void Draw1line1( int );						// 1���C���`��(N60)
	void Draw1line2( int );						// 1���C���`��(N60m)
	
	#if INBPP == 8	// 8bit
	BYTE GetBcol() const;						// �{�[�_�[�J���[�擾
	#else			// 32bit
	DWORD GetBcol() const;						// �{�[�_�[�J���[�擾
	#endif
	
public:
	PCZ80_07();									// �R���X�g���N�^
	virtual ~PCZ80_07();						// �f�X�g���N�^
	
	virtual void UpdateBackBuf();				// �o�b�N�o�b�t�@�X�V
};


class PCZ80_12 : public virtual PCZ80_07 {
protected:
	BYTE GetSemi6( BYTE ) const;				// �Z�~�O��6�f�[�^�擾
	BYTE GetSemi8( BYTE ) const;				// �Z�~�O��8�f�[�^�擾
	
	void SetCrtControler( BYTE );				// CRT�R���g���[�����[�h�ݒ�
	void SetCrtCtrlType( BYTE );				// CRT�R���g���[���^�C�v�ݒ�
	
	void Draw1line3( int );						// 1���C���`��(SR)
	
public:
	PCZ80_12();									// �R���X�g���N�^
	virtual ~PCZ80_12();						// �f�X�g���N�^
	
	void UpdateBackBuf();						// �o�b�N�o�b�t�@�X�V
	
	void SetPalette( int, BYTE );				// �p���b�g�ݒ�
};


#endif	// MC6847_H_INCLUDED
