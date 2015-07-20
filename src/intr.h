#ifndef INTR_H_INCLUDED
#define INTR_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"

// �����ݗv���t���O
//  0 �\��
//  1 �L�[������3
//  2 RS232S ��M ������
//  3 �^�C�}������
//  4 CMT READ ������
//  5 �\��
//  6 �\��
//  7 �L�[������1 ����1
//  8 �L�[������1 ����2
//  9 CMT ERROR ������
// 10 �L�[������2
// 11 �Q�[���p�L�[������
// xx 8049������(��L1,2,4-11)

#define	IREQ_KEY3	(0x0001)
#define	IREQ_SIO	(0x0002)
#define	IREQ_TIMER	(0x0004)
#define	IREQ_CMT_R	(0x0008)
#define	IREQ_KEY1	(0x0010)
#define	IREQ_KEY12	(0x0020)
#define	IREQ_CMT_E	(0x0040)
#define	IREQ_KEY2	(0x0080)
#define	IREQ_JOYKEY	(0x0100)
#define	IREQ_8049	(IREQ_KEY3|IREQ_SIO|IREQ_CMT_R|IREQ_KEY1|IREQ_KEY12|IREQ_CMT_E|IREQ_KEY2|IREQ_JOYKEY)

// 12 TVR
// 13 DATE
// 14 �\��
// 15 �\��
// 16 ��������
// 17 VRTC
// 18 �\��
// 19 �\��
#define	IREQ_JOYSTK	(0x0200)

#define	IREQ_VOICE	(0x0400)
#define	IREQ_VRTC	(0x0800)
#define	IREQ_PRINT	(0x1000)
#define	IREQ_EXTINT	(0x2000)



////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class IRQ6 : public Device, public IDoko {
protected:
	DWORD IntrFlag;			// �����ݗv���t���O
	
	bool TimerIntrEnable;	// �^�C�}�����݋��t���O
	BYTE TimerCntUp;		// �^�C�}�����݃J�E���g�A�b�v�l
	
	bool IntEnable[8];		// �����݋��t���O
	bool VecOutput[8];		// �����݃A�h���X�o�̓t���O
	BYTE IntVector[8];		// �����݃A�h���X
	
	BYTE Timer1st;			// �^�C�}�����ݏ�������䗦
	
	virtual void SetIntrEnable( BYTE );		// �����݋��t���O�ݒ�
	void SetTimerIntr( bool );				// �^�C�}�����݃X�C�b�`
	void SetTimerIntrHz( BYTE, BYTE=0 );	// �^�C�}�����ݎ��g���ݒ�
	
	// I/O�A�N�Z�X�֐�
	void OutB0H( int, BYTE );
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	void OutF3H( int, BYTE );
	void OutF4H( int, BYTE );
	void OutF5H( int, BYTE );
	void OutF6H( int, BYTE );
	void OutF7H( int, BYTE );
	BYTE InF3H( int );
	BYTE InF4H( int );
	BYTE InF5H( int );
	BYTE InF6H( int );
	BYTE InF7H( int );
	// ---------------------------------------------------------------------------------------
	
public:
	IRQ6( VM6 *, const ID& );				// �R���X�g���N�^
	virtual ~IRQ6();						// �f�X�g���N�^
	
	void EventCallback( int, int );			// �C�x���g�R�[���o�b�N�֐�
	
	virtual void Reset() = 0;				// ���Z�b�g
	
	int	IntrCheck();						// �����݃`�F�b�N
	
	void ReqIntr( DWORD );					// �����ݗv��
	void CancelIntr( DWORD );				// �����ݓP��
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class IRQ60 : public IRQ6 {
private:
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	IRQ60( VM6 *, const ID& );				// �R���X�g���N�^
	~IRQ60();								// �f�X�g���N�^
	
	void Reset();							// ���Z�b�g
	
	// �f�o�C�XID
	enum IDOut{ outB0H=0 };
	enum IDIn {};
};


class IRQ62 : public IRQ6 {
private:
	void SetIntrEnable( BYTE );				// �����݋��t���O�ݒ�
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	IRQ62( VM6 *, const ID& );				// �R���X�g���N�^
	~IRQ62();								// �f�X�g���N�^
	
	void Reset();							// ���Z�b�g
	
	// �f�o�C�XID
	enum IDOut{ outB0H=0, outF3H,  outF4H, outF5H, outF6H, outF7H };
	enum IDIn {            inF3H=0, inF4H,  inF5H,  inF6H,  inF7H };
};



class IRQ64 : public IRQ6 {
private:
	void SetIntrEnable( BYTE );				// �����݋��t���O�ݒ�
	void SetIntrVectorEnable( BYTE );		// �����݃x�N�^�A�h���X�o�̓t���O�ݒ�
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/O�A�N�Z�X�֐�
	void OutBxH( int, BYTE );
	void OutFAH( int, BYTE );
	void OutFBH( int, BYTE );
	BYTE InFAH( int );
	BYTE InFBH( int );
	
public:
	IRQ64( VM6 *, const ID& );				// �R���X�g���N�^
	~IRQ64();								// �f�X�g���N�^
	
	void Reset();							// ���Z�b�g
	
	// �f�o�C�XID
	enum IDOut{ outB0H=0, outBxH,
				outF3H,   outF4H, outF5H, outF6H, outF7H, outFAH, outFBH };
	enum IDIn {  inF3H=0,  inF4H,  inF5H,  inF6H,  inF7H,  inFAH,  inFBH };
};


#endif	// INTR_H_INCLUDED
