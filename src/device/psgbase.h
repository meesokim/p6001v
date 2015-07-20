#ifndef PSGBASE_H_INCLUDED
#define PSGBASE_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class cPSG {
protected:
	BYTE RegisterLatch;
	BYTE LastEnable;
	
	virtual void WriteReg( BYTE, BYTE ){}	// ���W�X�^������
	virtual BYTE ReadReg(){ return 0xff; }	// ���W�X�^�Ǎ���
	virtual BYTE ReadStatus(){ return 0xff; }	// �X�e�[�^�X���W�X�^�Ǎ���
	
	virtual void SetClock( int, int ){}		// �N���b�N�ݒ�
	virtual void SetVolumeTable( int ){}	// ���ʐݒ�(�{�����[���e�[�u���ݒ�)
	
	// �|�[�g�A�N�Z�X�֐�
	virtual BYTE PortAread(){ return 0xff; }
	virtual BYTE PortBread(){ return 0xff; }
	virtual void PortAwrite( BYTE ){}
	virtual void PortBwrite( BYTE ){}
	
	virtual void PreWriteReg(){}		// ���W�X�^�ύX�O�̃X�g���[���X�V
	
	virtual bool Init( int, int ){}		// ������
	virtual void Reset(){}				// ���Z�b�g
	
public:
	cPSG() : RegisterLatch(0), LastEnable(0xff) {};		// �R���X�g���N�^
	virtual ~cPSG(){};					// �f�X�g���N�^
	
	int Update1Sample(){ return 0; }	// �X�g���[��1Sample�X�V
};


#endif	// PSGBASE_H_INCLUDED
