#ifndef REPLAY_H_INCLUDED
#define REPLAY_H_INCLUDED

#include "typedef.h"
#include "ini.h"

#define	REP_IDLE	(0)
#define	REP_RECORD	(1)
#define	REP_REPLAY	(2)


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class REPLAY {
protected:
	cIni *Ini;
	
	int RepST;					// �X�e�[�^�X
	
	BYTE *Matrix;				// �L�[�}�g���N�X�o�b�t�@�|�C���^
	int MSize;					// �L�[�}�g���N�X�T�C�Y
	
	DWORD RepFrm;				// �t���[��No.�J�E���^
	DWORD EndFrm;				// ���v���C�I���t���[��
	
public:
	REPLAY();								// �R���X�g���N�^
	~REPLAY();								// �f�X�g���N�^
	
	bool Init( int );						// ������
	
	int GetStatus() const;					// �X�e�[�^�X�擾
	
	bool StartRecord( const char * );		// ���v���C�L�^�J�n
	bool ResumeRecord( const char *, int );	// ���v���C�L�^�ĊJ
	void StopRecord();						// ���v���C�L�^��~
	bool ReplayWriteFrame( const BYTE *, bool );	// ���v���C1�t���[�����o��
	
	bool StartReplay( const char * );		// ���v���C�Đ��J�n
	void StopReplay();						// ���v���C�Đ��~
	bool ReplayReadFrame( BYTE * );			// ���v���C1�t���[���Ǎ���
	
};


#endif	// REPLAY_H_INCLUDED
