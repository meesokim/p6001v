#ifndef PSGFM_H_INCLUDED
#define PSGFM_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "sound.h"
#include "device/ay8910.h"
#include "device/ym2203.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
// PSG�n�T�E���h�f�o�C�X�̃x�[�X�N���X
class PSGb : public Device, public SndDev, public virtual IDoko {
protected:
	BYTE JoyNo;							// �ǎ��W���C�X�e�B�b�N�̔ԍ�(0-1)
	int Clock;							// �N���b�N
	
	int GetUpdateSamples();				// �X�V�T���v�����擾
	
public:
	PSGb( VM6 *, const ID& );			// �R���X�g���N�^
	virtual ~PSGb();					// �f�X�g���N�^
	
	virtual bool Init( int, int ) = 0;	// ������
	virtual void Reset(){};				// ���Z�b�g
	
	virtual void EventCallback( int, int );		// �C�x���g�R�[���o�b�N�֐�
	virtual bool SetSampleRate( int, int ) = 0;	// �T���v�����O���[�g�ݒ�
};


class PSG6 : public PSGb, public cAY8910 {
protected:
	void PreWriteReg();					// ���W�X�^�ύX�O�̃X�g���[���X�V
	
	// �|�[�g�A�N�Z�X�֐�(�W���C�X�e�B�b�N����)
	BYTE PortAread();
	void PortBwrite( BYTE );
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/O�A�N�Z�X�֐�
	void OutA0H( int, BYTE );			// PSG���W�X�^�A�h���X���b�`
	void OutA1H( int, BYTE );			// PSG���C�g�f�[�^
	void OutA3H( int, BYTE );			// PSG�C���A�N�e�B�u
	BYTE InA2H( int );					// PSG���[�h�f�[�^
	
public:
	PSG6( VM6 *, const ID& );			// �R���X�g���N�^
	virtual ~PSG6();					// �f�X�g���N�^
	
	bool Init( int, int );				// ������
	
	bool SetSampleRate( int, int );		// �T���v�����O���[�g�ݒ�
	
	int SoundUpdate( int );				// �X�g���[���X�V
	
	// �f�o�C�XID
	enum IDOut{ outA0H=0, outA1H, outA3H };
	enum IDIn {  inA2H=0 };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class OPN6 : public PSGb, public cYM2203 {
protected:
	void PreWriteReg();					// ���W�X�^�ύX�O�̃X�g���[���X�V
	
	void SetTimerA( int );				// TimerA�ݒ�
	void SetTimerB( int );				// TimerB�ݒ�
	
	// �|�[�g�A�N�Z�X�֐�(�W���C�X�e�B�b�N����)
	BYTE PortAread();
	void PortBwrite( BYTE );
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	void OutA0H( int, BYTE );			// OPN���W�X�^�A�h���X���b�`
	void OutA1H( int, BYTE );			// OPN���C�g�f�[�^
	void OutA3H( int, BYTE );			// OPN�C���A�N�e�B�u
	BYTE InA2H( int );					// OPN���[�h�f�[�^
	BYTE InA3H( int );					// OPN�X�e�[�^�X���[�h
	
public:
	OPN6( VM6 *, const ID& );			// �R���X�g���N�^
	~OPN6();							// �f�X�g���N�^
	
	bool Init( int, int );				// ������
	
	void EventCallback( int, int );		// �C�x���g�R�[���o�b�N�֐�
	bool SetSampleRate( int, int );		// �T���v�����O���[�g�ݒ�
	
	int SoundUpdate( int );				// �X�g���[���X�V
	
	// �f�o�C�XID
	enum IDOut{ outA0H=0, outA1H, outA3H };
	enum IDIn {  inA2H=0,  inA3H };
};


#endif	// PSGFM_H_INCLUDED
