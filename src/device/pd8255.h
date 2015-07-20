#ifndef PD8255_H_INCLUDED
#define PD8255_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class cD8255 {
protected:
	BYTE PortA;						// �|�[�gA
	BYTE PortB;						// �|�[�gB
	BYTE PortC;						// �|�[�gC
	
	BYTE PortAbuf;					// �|�[�gA�o�b�t�@(�|�[�gA�̓��͂��ꎞ�I�Ƀ��b�`)
	
	int ModeA;						// �O���[�vA���[�h
	int ModeB;						// �O���[�vB���[�h
	
	bool PortAdir;					// �|�[�gA         ���o�͐��� true:����(�ǂ�) false:�o��(����)
	bool PortBdir;					// �|�[�gB         ���o�͐��� true:����(�ǂ�) false:�o��(����)
	bool PortC1dir;					// �|�[�gC(bit0-3) ���o�͐��� true:����(�ǂ�) false:�o��(����)
	bool PortC2dir;					// �|�[�gC(bit4-7) ���o�͐��� true:����(�ǂ�) false:�o��(����)
	
	bool HSINT0;					// INT0
	bool HSWINT0;					// WINT0
	bool HSRINT0;					// RINT0
	bool HSSTB0;					// STB0
	bool HSIBF0;					// IBF0
	bool HSDAK0;					// DAK0
	bool HSOBF0;					// OBF0
	
	bool RIE0;						// RIE0
	bool WIE0;						// WIE0
	
	// ���o�͏����֐�
	virtual void JobWriteA( BYTE ){}
	virtual void JobWriteB( BYTE ){}
	virtual void JobWriteC1( BYTE ){}	// ����
	virtual void JobWriteC2( BYTE ){}	// ���
	virtual void JobWriteD( BYTE ){}	// �r�b�g�Z�b�g/���Z�b�g�̂�
	virtual void JobReadA(){}
	virtual void JobReadB(){}
	virtual void JobReadC(){}
	
	void SetMode( BYTE );			// ���[�h�ݒ�
	
public:
	cD8255();						// �R���X�g���N�^
	virtual ~cD8255();				// �f�X�g���N�^
	
	void Reset();					// ���Z�b�g
	
	// ���o�͊֐�
	void WriteA( BYTE );
	void WriteB( BYTE );
	void WriteC( BYTE );
	void WriteD( BYTE );			// �R���g���[���|�[�g�o��
	BYTE ReadA();
	BYTE ReadB();
	BYTE ReadC();
	
	// ���[�h2�n���h�V�F�C�N�p
	void WriteAE( BYTE );			// PartA ���C�g(���ӑ�)
	BYTE ReadAE();					// PartA ���[�h(���ӑ�)
	bool GetIBF();					// IBF�擾
	bool GetOBF();					// OBF�擾
};


#endif	// PD8255_H_INCLUDED
