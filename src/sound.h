#ifndef SOUND_H_INCLUDED
#define SOUND_H_INCLUDED

#include "semaphore.h"


// �X�g���[���̍ő吔
// PSG/OPN
// TAPE
// VOICE
// FM�J�[�g���b�W
#define	MAXSTREAM	(4)

////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
// �����O�o�b�t�@�I�u�W�F�N�g
class cRing : public cCritical {
private:
	int *Buffer;			// �o�b�t�@�ւ̃|�C���^
	
	int Size;				// �o�b�t�@�T�C�Y(�f�[�^��)
	int Wpt;				// �����݃|�C���^
	int Rpt;				// �Ǎ��݃|�C���^
	int Num;				// ���ǃf�[�^��
	
public:
	cRing();								// �R���X�g���N�^
	virtual ~cRing();						// �f�X�g���N�^
	
	bool InitBuffer( int );					// �o�b�t�@������
	
	virtual int Get();						// �Ǎ���
	virtual bool Put( int );				// ������
	
	int ReadySize();						// ���ǃf�[�^���擾
	int FreeSize();							// �c��o�b�t�@�擾
	int GetSize();							// �o�b�t�@�T�C�Y�擾
};


// �T�E���h�f�o�C�X�I�u�W�F�N�g
class SndDev : public cRing {
protected:
	int SampleRate;			// �T���v�����[�g
	int Volume;				// ����
	int LPF_Mem;			// ���[�p�X�t�B���^�p���[�N
	int LPF_fc;				// ���[�p�X�t�B���^�J�b�g�I�t���g��
	
	int LPF( int );							// ���[�p�X�t�B���^
	
public:
	SndDev();								// �R���X�g���N�^
	virtual ~SndDev();						// �f�X�g���N�^
	
	virtual bool Init( int );				// ������
	
	int Get();								// �Ǎ���
	
	void SetLPF( int );						// ���[�p�X�t�B���^ �J�b�g�I�t���g���ݒ�
	virtual bool SetSampleRate( int, int );	// �T���v�����O���[�g�ݒ�
	virtual void SetVolume( int );			// ���ʐݒ�
	virtual int SoundUpdate( int );			// �X�g���[���X�V
};


// �T�E���h�I�u�W�F�N�g
class SND6 : public cRing {
private:
	SndDev *RB[MAXSTREAM];	// �X�g���[���|�C���^�z��
	int Volume;				// �}�X�^�[����
	int SampleRate;			// �T���v�����O���[�g
	int BSize;				// �o�b�t�@�T�C�Y(�{��)
	CBF_SND CbFunc;			// �R�[���o�b�N�֐��ւ̃|�C���^
	void *CbData;			// �R�[���o�b�N�֐��ɓn���f�[�^
	
public:
	SND6();									// �R���X�g���N�^
	~SND6();								// �f�X�g���N�^
	
	bool Init( void *, CBF_SND, int, int );	// ������
	
	bool ConnectStream( SndDev * );			// �X�g���[���ڑ�
	
	void Play();							// �Đ�
	void Pause();							// ��~
	
	bool SetSampleRate( int );				// �T���v�����O���[�g�ݒ�
	int GetSampleRate();					// �T���v�����O���[�g�擾
	void SetVolume( int );					// �}�X�^�[���ʐݒ�
	
	int GetBufferSize();					// �o�b�t�@�T�C�Y(�{��)�擾
	
	int PreUpdate( int, cRing * = NULL );	// �T�E���h���O�X�V�֐�
	void Update( BYTE *, int );				// �T�E���h�X�V�֐�
};

#endif	// SOUND_H_INCLUDED
