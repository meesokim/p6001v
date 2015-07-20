#ifndef CPUS_H_INCLUDED
#define CPUS_H_INCLUDED

#include "device.h"
#include "ini.h"


// CMT�X�e�[�^�X
#define	CMTCLOSE	(0)
#define	LOADOPEN	(1)
#define	SAVEOPEN	(2)

// I/O�|�[�g�ԍ�
#define	IO8049_BUS	(0x00)	/* �o�X�|�[�g */
#define	IO8049_P1	(0x01)	/* Port1 */
#define	IO8049_P2	(0x02)	/* Port2 */
#define	IO8049_T0	(0x03)	/* T0(OUT) */
#define	IO8049_INT	(0x04)	/* ~INT(IN) */


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class SUB6 : public Device, public IDoko {
protected:
	int CmtStatus;						// CMT�X�e�[�^�X
	int Status8049;						// 8049����X�e�[�^�X
	
	DWORD IntrFlag;						// �����ݗv���t���O
	
	BYTE KeyCode;						// �L�[�R�[�h(�����ݗp)
	BYTE JoyCode;						// �Q�[���p�L�[�R�[�h(�����ݗp)
	BYTE CmtData;						// CMT���[�h�f�[�^(�����ݗp)
	BYTE SioData;						// RS232C��M�f�[�^(�����ݗp)
	
	BYTE TVRData[32];					// TV�\��f�[�^
	BYTE DateData[5];					// DATE�f�[�^
	
	int TVRCnt;							// TV�\�񊄍��ݗp�J�E���^
	int DateCnt;						// DATE�����ݗp�J�E���^;
	
	BYTE ReadIO( int );					// I/O�|�[�g�A�N�Z�X Read
	void WriteIO( int, BYTE );			// I/O�|�[�g�A�N�Z�X Write
	
	void WriteExt( BYTE );				// �O���������o��(8255��PortA)
	BYTE ReadExt();						// �O������������(8255��PortA)
	
	bool GetT0();						// �e�X�g0�X�e�[�^�X�擾
	bool GetT1();						// �e�X�g1�X�e�[�^�X�擾
	bool GetINT();						// �O�������݃X�e�[�^�X�擾
	
	void ReqJoyIntr();					// �Q�[���p�L�[�����ݗv��
	
	void OutVector();					// �����݃x�N�^�o��
	void OutData()	;					// �����݃f�[�^�o��
	
	virtual void ExtIntrExec( BYTE );	// �O�������ݏ���
	
public:
	SUB6( VM6 *, const ID& );			// �R���X�g���N�^
	virtual ~SUB6();					// �f�X�g���N�^
	
	void EventCallback( int, int );		// �C�x���g�R�[���o�b�N�֐�
	
	void Reset();						// ���Z�b�g
	
	// �L�[�{�[�h�֘A
	void ReqKeyIntr( int, BYTE );		// �L�[�����ݗv��
	
	// CMT�֘A
	void ReqCmtIntr( BYTE );			// CMT READ�����ݗv��
	int GetCmtStatus() const;			// CMT�X�e�[�^�X�擾
	bool IsCmtIntrReady();				// CMT�����ݔ�����?
	
	void ExtIntr();						// �O�������ݗv��
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class SUB60 : public SUB6 {
protected:
	
public:
	SUB60( VM6 *, const ID& );			// �R���X�g���N�^
	~SUB60();							// �f�X�g���N�^
};


class SUB62 : public SUB60 {
protected:
	virtual void ExtIntrExec( BYTE );	// �O�������ݏ���
	
public:
	SUB62( VM6 *, const ID& );			// �R���X�g���N�^
	~SUB62();							// �f�X�g���N�^
};


class SUB68 : public SUB62 {
protected:
	void ReqTVRReadIntr();				// TV�\��Ǎ��݊����ݗv��
	void ReqDateIntr();					// DATE�����ݗv��
	
	void ExtIntrExec( BYTE );			// �O�������ݏ���
	
public:
	SUB68( VM6 *, const ID& );			// �R���X�g���N�^
	~SUB68();							// �f�X�g���N�^
};



#endif	// CPUS_H_INCLUDED
