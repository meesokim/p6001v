#ifndef TAPE_H_INCLUDED
#define TAPE_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"
#include "p6t2.h"
#include "sound.h"


// �f�[�^�̎��
#define PG_P	(0x0000)
#define PG_S	(0x0100)
#define PG_D	(0x0200)


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class CMTL : public Device, public SndDev, public IDoko {
private:
	char FilePath[PATH_MAX];			// TAPE�t�@�C���t���p�X
	cP6T *p6t;
	bool Relay;							// �����[�̏��
	bool stron;							// �X�g���[�����������p
	
	bool Boost;							// BoostUp�g��? true:�g�� false:�g��Ȃ�
	int MaxBoost60;						// BoostUp �ő�{��(N60���[�h)
	int MaxBoost62;						// BoostUp �ő�{��(N60m/N66���[�h)
	
	bool Remote( bool );				// �����[�g����(PLAY,STOP)
	WORD CmtRead();						// CMT 1�����Ǎ���
	int GetSinCurve( int );				// sin�g�擾
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/O�A�N�Z�X�֐�
	void OutB0H( int, BYTE );
	
public:
	CMTL( VM6 *, const ID& );			// �R���X�g���N�^
	virtual ~CMTL();					// �f�X�g���N�^
	
	void EventCallback( int, int );		// �C�x���g�R�[���o�b�N�֐�
	
	bool Init( int );					// ������
	void Reset();						// ���Z�b�g
	
	bool Mount( const char * );			// TAPE �}�E���g
	void Unmount();						// TAPE �A���}�E���g
	
	WORD Update();						// �X�g���[���X�V(1byte��)
	int SoundUpdate( int );				// �X�g���[���X�V
	
	bool IsMount() const;				// �}�E���g�ς�?
	bool IsAutoStart() const;			// �I�[�g�X�^�[�g?
	
	const char *GetFile() const;		// �t�@�C���p�X�擾
	const char *GetName() const;		// TAPE���擾
	DWORD GetSize() const;				// �x�^�C���[�W�T�C�Y�擾
	int GetCount() const;				// �J�E���^�擾
	bool IsRelay() const;				// �����[�̏�Ԏ擾
	
	void SetBoost( bool );				// BoostUp�ݒ�
	void SetMaxBoost( int, int );		// BoostUp�ő�{���ݒ�
	bool IsBoostUp() const;				// BoostUp��Ԏ擾
	
	const P6TAUTOINFO *GetAutoStartInfo() const;	// �I�[�g�X�^�[�g���擾
	
	// �f�o�C�XID
	enum IDOut{ outB0H=0 };
	enum IDIn {};
	
	// ------------------------------------------
	bool DokoSave( cIni * );		// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );		// �ǂ��ł�LOAD
	// ------------------------------------------
};


class CMTS : public Device {
private:
	char FilePath[PATH_MAX];			// TAPE�t�@�C���t���p�X
	
	FILE *fp;							// FILE �|�C���^
	int Baud;							// �{�[���[�g
	
public:
	CMTS( VM6 *, const ID& );			// �R���X�g���N�^
	~CMTS();							// �f�X�g���N�^
	
	bool Init( const char * );			// ������
	
	bool Mount();						// TAPE �}�E���g
	void Unmount();						// TAPE �A���}�E���g
	
	void SetBaud( int );				// �{�[���[�g�ݒ�
	
	void CmtWrite( BYTE );				// CMT 1����������
};


#endif	// TAPE_H_INCLUDED
