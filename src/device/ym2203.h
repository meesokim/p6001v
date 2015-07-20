#ifndef YM2203_H_INCLUDED
#define YM2203_H_INCLUDED

#include "../typedef.h"
#include "../ini.h"
#include "psgbase.h"
#include "fmgen/opna.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class cYM2203 : public cPSG, public virtual IDoko {
protected:
	FM::OPN	opn;
	
	BYTE RegTimerA1, RegTimerA2;
	BYTE RegTimerB;
	BYTE RegTimerCtr;
	
	void _WriteReg( BYTE, BYTE );		// ���W�X�^�����݃T�u
	void WriteReg( BYTE, BYTE );		// ���W�X�^������
	BYTE ReadReg();						// ���W�X�^�Ǎ���
	BYTE ReadStatus();					// �X�e�[�^�X���W�X�^�Ǎ���
	
	void SetClock( int, int );			// �N���b�N�ݒ�
	void SetVolumeTable( int );			// ���ʐݒ�(�{�����[���e�[�u���ݒ�)
	
	bool Init( int, int );				// ������
	void Reset();						// ���Z�b�g
	
	virtual void SetTimerA( int ){}		// TimerA�ݒ�
	virtual void SetTimerB( int ){}		// TimerB�ݒ�
	
	bool TimerIntr();					// �^�C�}�����ݔ���
	
public:
	cYM2203();							// �R���X�g���N�^
	virtual ~cYM2203();					// �f�X�g���N�^
	
	int Update1Sample();				// �X�g���[��1Sample�X�V
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


#endif	// YM2203_H_INCLUDED
