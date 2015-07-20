#ifndef KEYBOARD_H_INCLUDED
#define KEYBOARD_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "keydef.h"


// �L�[�{�[�h�C���W�P�[�^���
#define	KI_KANA		(1)
#define	KI_KKANA	(2)
#define	KI_CAPS		(4)


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class KEY6 : public Device, public IDoko {
protected:
	P6KEYsym K6Table[KVC_LAST];		// ���z�L�[�R�[�h -> P6�L�[�R�[�h �ϊ��e�[�u��
	BYTE MatTable[KP6_LAST];		// P6�L�[�R�[�h -> �}�g���N�X �ϊ��e�[�u��
	
	// ����L�[ �t���O
	bool ON_SHIFT;	// SHIFT
	bool ON_GRAPH;	// GRAPH
	bool ON_KANA;	// ����
	bool ON_KKANA;	// �J�^�J�i
	bool ON_CTRL;	// CTRL
	bool ON_STOP;	// STOP
	bool ON_CAPS;	// CAPS
	
	BYTE P6Matrix[16*2];		// �L�[�}�g���N�X (�O��:���� �㔼:�O��)
	BYTE P6Mtrx[16*2];			// �L�[�}�g���N�X�ۑ��p
								//  �������������2byte�̓W���C�X�e�B�b�N�̏�ԕۑ��p

public:
	KEY6( VM6 *, const ID& );			// �R���X�g���N�^
	virtual ~KEY6();					// �f�X�g���N�^
	
	bool Init( int );					// ������
	
	void UpdateMatrixKey( int, bool );	// �L�[�}�g���N�X�X�V(�L�[)
	void UpdateMatrixJoy( BYTE, BYTE );	// �L�[�}�g���N�X�X�V(�W���C�X�e�B�b�N)
	bool ScanMatrix();					// �L�[�}�g���N�X�X�L����
	int GetMatrixSize() const;			// �L�[�}�g���N�X�T�C�Y�擾
	BYTE *GetMatrix();					// �L�[�}�g���N�X�|�C���^�擾
	const BYTE *GetMatrix2() const;		// �L�[�}�g���N�X�|�C���^(�ۑ��p)�擾
	
	BYTE GetKeyJoy() const;				// �J�[�\���L�[��Ԏ擾
	BYTE GetKeyIndicator() const;		// �L�[�{�[�h�C���W�P�[�^��Ԏ擾
	
	void SetVKeySymbols( VKeyConv * );	// ���z�L�[�R�[�h -> P6�L�[�R�[�h �ݒ�
	
	BYTE GetJoy( int ) const;			// �W���C�X�e�B�b�N��Ԏ擾
	
	void ChangeKana();					// �p��<->���Ȑ؊�
	void ChangeKKana();					// ����<->�J�i�؊�
	
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class KEY60 : public KEY6 {
public:
	KEY60( VM6 *, const ID& );		// �R���X�g���N�^
	~KEY60();							// �f�X�g���N�^
};


#endif	// KEYBOARD_H_INCLUDED
