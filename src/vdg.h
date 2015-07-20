#ifndef VDG_H_INCLUDED
#define VDG_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "vsurface.h"
#include "device/mc6847.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class VDG6 : public Device, public VSurface, public virtual MC6847core, public IDoko {
protected:
	// �J���[�R�[�h
	// for 60,61 -----------------------------------------------------------------------------
	static const BYTE COL60_AN[];
	static const BYTE COL60_SG[];
	static const BYTE COL60_CG[][8];
	static const BYTE COL60_RG[][2];
	// ---------------------------------------------------------------------------------------
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	static const BYTE COL62_AN[];
	static const BYTE COL62_SG[];
	static const BYTE COL62_CG[][8];
	static const BYTE COL62_RG[][2];
	static const BYTE COL62_AN2[];
	static const BYTE COL62_CG2[][16];
	// ---------------------------------------------------------------------------------------
	
	WORD AddrOff;								// VRAM,ATTR�I�t�Z�b�g
	
	// �����M�����[�N
	bool VSYNC;									// ���������t���O
	bool HSYNC;									// ���������t���O
	int VLcnt;									// �\�����C���J�E���^
	int HSdclk;									// �����g�[�^������(�h�b�g�N���b�N)
	int Hclk60;									// �����\������(N60)
	
	bool CreateBuffer( int=1 );					// �o�b�N�o�b�t�@�쐬
	BYTE *GetBufAddr() const;					// �o�b�t�@�A�h���X�擾
	int GetBufPitch() const;					// �o�b�t�@�s�b�`(1Line�o�C�g��)�擾
	
	void LatchAttr();							// �A�g���r���[�g�f�[�^���b�`
	void LatchGMODE();							// �A�g���r���[�g�f�[�^���b�`(�O���t�B�b�N���[�h�̂�)
	BYTE GetAttr() const;						// �A�g���r���[�g�f�[�^�擾
	virtual BYTE GetVram() const;				// VRAM�f�[�^�擾
	BYTE GetFont1( WORD ) const;				// Font1�f�[�^�擾
	BYTE GetFont2( WORD ) const;				// Font2�f�[�^�擾
	BYTE GetFont3( WORD ) const;				// Font3�f�[�^�擾
	
	virtual WORD GetVramAddr() const = 0;		// VRAM�A�h���X�擾
	virtual WORD GerAttrAddr() const = 0;		// ATTR�A�h���X�擾
	virtual void SetAttrAddr( BYTE );			// ATTR�A�h���X�ݒ�
	
	// I/O�A�N�Z�X�֐�
	void OutB0H( int, BYTE );
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	void OutC0H( int, BYTE );
	void OutC1H( int, BYTE );
	BYTE InA2H( int );
	// ---------------------------------------------------------------------------------------
	
public:
	VDG6( VM6 *, const ID& );					// �R���X�g���N�^
	virtual ~VDG6();							// �f�X�g���N�^
	
	virtual void EventCallback( int, int );		// �C�x���g�R�[���o�b�N�֐�
	
	virtual bool Init();						// ������
	virtual void Reset();						// ���Z�b�g
	
	bool IsBusReqStop() const;					// �o�X���N�G�X�g��Ԓ�~�t���O�擾
	bool IsBusReqExec() const;					// �o�X���N�G�X�g��Ԏ��s�t���O�擾
	bool IsSRmode() const;						// SR���[�h�擾
	bool IsSRGVramAccess( WORD ) const;			// SR��G-VRAM�A�N�Z�X?
	WORD SRGVramAddr( WORD ) const;				// SR��G-VRAM�A�h���X�擾
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class VDG60 : public VDG6, public MC6847 {
protected:
	WORD GetVramAddr() const;					// VRAM�A�h���X�擾
	WORD GerAttrAddr() const;					// ATTR�A�h���X�擾
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	VDG60( VM6 *, const ID& );					// �R���X�g���N�^
	virtual  ~VDG60();							// �f�X�g���N�^
	
	// �f�o�C�XID
	enum IDOut{ outB0H=0 };
	enum IDIn {};
};


class VDG62 : public VDG6, public virtual PCZ80_07 {
protected:
	WORD GetVramAddr() const;					// VRAM�A�h���X�擾
	WORD GerAttrAddr() const;					// ATTR�A�h���X�擾
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	VDG62( VM6 *, const ID& );					// �R���X�g���N�^
	virtual ~VDG62();							// �f�X�g���N�^
	
	// �f�o�C�XID
	enum IDOut{ outB0H=0, outC0H, outC1H };
	enum IDIn {  inA2H=0 };
};


class VDG64 : public VDG6, public PCZ80_12 {
protected:
	BYTE GetVram() const;						// VRAM�f�[�^�擾
	WORD GetVramAddr() const;					// VRAM�A�h���X�擾
	WORD GerAttrAddr() const;					// ATTR�A�h���X�擾
	void SetAttrAddr( BYTE );					// ATTR�A�h���X�ݒ�
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/O�A�N�Z�X�֐�
	void Out4xH( int, BYTE );
	void OutC8H( int, BYTE );
	void OutC9H( int, BYTE );
	void OutCAH( int, BYTE );
	void OutCBH( int, BYTE );
	void OutCCH( int, BYTE );
	void OutCEH( int, BYTE );
	void OutCFH( int, BYTE );
	
public:
	VDG64( VM6 *, const ID& );					// �R���X�g���N�^
	virtual ~VDG64();							// �f�X�g���N�^
	
	void EventCallback( int, int );				// �C�x���g�R�[���o�b�N�֐�
	
	void Reset();								// ���Z�b�g
	
	// �f�o�C�XID
	enum IDOut{ out4xH=0, outB0H, outC0H, outC1H, outC8H, outC9H,
				outCAH,   outCBH, outCCH, outCEH, outCFH };
	enum IDIn {  inA2H=0 };
};


#endif	// VDG_H_INCLUDED
