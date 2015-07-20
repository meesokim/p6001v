#ifndef SCHEDULE_H_INCLUDED
#define SCHEDULE_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "thread.h"


// �ő�C�x���g��
#define	MAXEVENT	(32)

// �C�x���g�X�^�C���t���O
// bit0: �J��Ԃ��w��
#define	EV_ONETIME	(0x00)	/* �����^�C�� */
#define	EV_LOOP		(0x01)	/* ���[�v */
// bit1,2: �����w��P��
#define	EV_HZ		(0x00)	/* Hz */
#define	EV_US		(0x02)	/* us */
#define	EV_MS		(0x04)	/* ms */
#define	EV_STATE	(0x08)	/* CPU�X�e�[�g�� */

// �������x�v�Z�p�ۑ��̈�T�C�Y
#define	SPDCNT		(5)


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
// VM�C�x���g�X�P�W���[���N���X
class EVSC : public IDoko {
public:
	// �C�x���g���\����
	struct evinfo {
		Device::ID devid;		// �C�x���g�f�o�C�X�I�u�W�F�N�gID
		int id;					// �C�x���gID
		bool Active;			// �C�x���g�L���t���O true:�L�� false:��~
		int Period;				// 1�����̃N���b�N��
		int Clock;				// �c��N���b�N��
		double nps;				// �C�x���g�������g��(Hz)
		
		evinfo() : devid(0), id(0), Active(false), Period(0), Clock(0), nps(0) {}
	};
	
protected:
	evinfo ev[MAXEVENT];		// �C�x���g���
	DeviceList devlist;			// �f�o�C�X���X�g
	
	bool VSYNC;					// VSYNC�t���O true:VSYNC�ɒB���� false:�B���ĂȂ�
	
	int MasterClock;			// �}�X�^�N���b�N��(��{�I�ɂ�CPU�X�e�[�g��/�b�ɂȂ�͂�)
	int NextEvent;				// ���̃C�x���g����������܂ł̃N���b�N��
	int SaveClock;				// �N���b�N�𗭂ߍ���
	
	const evinfo *Find( Device::ID, int ) const;	// �C�x���g����
	
public:
	EVSC( int );								// �R���X�g���N�^
	virtual ~EVSC();							// �f�X�g���N�^
	
	bool Entry( Device * );						// �ڑ�����f�o�C�X����o�^����
	bool Add( Device *, int, double, int );		// �C�x���g�ǉ�
	bool Del( Device *, int );					// �C�x���g�폜
	
	bool SetHz( Device::ID, int, double );		// �C�x���g�������g���ݒ�
	
	void Update( int );							// �C�x���g�X�V
	void Reset( Device::ID, int, double=0 );	// �w��C�x���g�����Z�b�g����
	
	int Rest( Device::ID, int ) const;			// �C�x���g�����܂ł̎c��N���b�N�������߂�
	double GetProgress( Device::ID, int ) const;	// �C�x���g�̐i�s�������߂�
	
	bool GetEvinfo( evinfo * ) const;			// �C�x���g���擾
	bool SetEvinfo( evinfo * );					// �C�x���g���ݒ�
	
	void SetMasterClock( int );					// �}�X�^�N���b�N�ݒ�
	int GetMasterClock() const;					// �}�X�^�N���b�N�擾
	
	bool IsVSYNC() const;						// VSYNC�ɒB����?
	void OnVSYNC();								// VSYNC��ʒm����
	void ReVSYNC();								// VSYNC�t���O�L�����Z��
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


// �G�~�����[�^�X�P�W���[���N���X
class SCH6 : public cThread, public cSemaphore {
protected:
	bool WaitEnable;		// Wait�L���t���O
	bool PauseEnable;		// �|�[�Y�L���t���O
	int EnableScrUpdate;	// ��ʍX�V�t���O
	
	int SpeedRatio;			// ���s���x
	int SpeedCnt1;			// ���s���x�����p�J�E���^1
	int SpeedCnt2;			// ���s���x�����p�J�E���^2
	
	// �������x�v�Z�p
	int MasterClock;		// �}�X�^�N���b�N��
	DWORD WRClock[SPDCNT];	// �����N���b�N�ۑ�
	
	void OnThread( void * );					// �X���b�h�֐�
	
public:
	SCH6( int );								// �R���X�g���N�^
	virtual ~SCH6();							// �f�X�g���N�^
	
//	void CalcCpuState();						// ��ʂ̕\����Ԃɉ�����CPU�X�e�[�g�����v�Z
	
	bool Start();								// ����J�n
	void Stop();								// �����~
	
	bool GetWaitEnable() const;					// Wait�L���t���O�擾
	void SetWaitEnable( bool );					//               �ݒ�
	
	bool GetPauseEnable() const;				// �|�[�Y�L���t���O�擾
	void SetPauseEnable( bool );				//                 �ݒ�
	
	void VWait();								// VSYNC Wait
	void WaitReset();							// Wait����������
	
	void SetSpeedRatio( int );					// ���s���x�ݒ�
	int GetSpeedRatio() const;					//         �擾
	
	void Update( int );							// �C�x���g�X�V
	int GetRatio() const;						// ���s���x��擾
	
	bool IsScreenUpdate();						// ��ʍX�V�������}����?
};

#endif	// SCHEDULE_H_INCLUDED
