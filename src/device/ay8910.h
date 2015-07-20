#ifndef AY8910_H_INCLUDED
#define AY8910_H_INCLUDED

#include "../typedef.h"
#include "../ini.h"
#include "psgbase.h"

#ifdef USEFMGEN
#include "fmgen/types.h"
#include "fmgen/psg.h"
#endif


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class cAY8910 : public cPSG, public virtual IDoko {
protected:
	#ifdef USEFMGEN
	PSG	psg;
	#else
	BYTE Regs[16];
	int UpdateStep;
	int PeriodA, PeriodB, PeriodC, PeriodN, PeriodE;
	int CountA, CountB, CountC, CountN, CountE;
	int VolA, VolB, VolC, VolE;
	BYTE EnvelopeA, EnvelopeB, EnvelopeC;
	BYTE OutputA, OutputB, OutputC, OutputN;
	int8_t CountEnv;
	BYTE Hold,Alternate,Attack,Holding;
	int RNG;
	int VolTable[32];
	#endif
	
	void _WriteReg( BYTE, BYTE );		// ���W�X�^�����݃T�u
	void WriteReg( BYTE, BYTE );		// ���W�X�^������
	BYTE ReadReg();						// ���W�X�^�Ǎ���
	
	void SetClock( int, int );			// �N���b�N�ݒ�
	void SetVolumeTable( int );			// ���ʐݒ�(�{�����[���e�[�u���ݒ�)
	
	void Reset();						// ���Z�b�g
	
public:
	cAY8910();							// �R���X�g���N�^
	virtual ~cAY8910();					// �f�X�g���N�^
	
	int Update1Sample();				// �X�g���[��1Sample�X�V
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


#endif	// AY8910_H_INCLUDED
