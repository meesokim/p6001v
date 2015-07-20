#ifndef JOYSTICK_H_INCLUDED
#define JOYSTICK_H_INCLUDED

#include "typedef.h"

#define	MAX_JOY			(8)			// �W���C�X�e�B�b�N�ő�ڑ���


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class JOY6 {
protected:
	int JID[2];						// �C���f�b�N�X
	HJOYINFO Jinfo[MAX_JOY];		// �W���C�X�e�B�b�N���
	
public:
	JOY6();							// �R���X�g���N�^
	virtual ~JOY6();				// �f�X�g���N�^
	
	bool Init();					// ������
	
	bool Connect( int, int );		// �W���C�X�e�B�b�N�ڑ�
	int GetID( int );				// �C���f�b�N�X�擾
	BYTE GetJoyState( int );		// �W���C�X�e�B�b�N�̏�Ԏ擾
};

#endif	// JOYSTICK_H_INCLUDED
