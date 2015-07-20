#ifndef CPUM_H_INCLUDED
#define CPUM_H_INCLUDED

#include "device.h"
#include "ini.h"
#include "device/z80.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class CPU6 : public Device, public cZ80, public IDoko {
private:
	BYTE Fetch( WORD, int * );		// �t�F�b�`(M1)
	BYTE ReadMemNW( WORD );			// �������A�N�Z�X(�E�F�C�g�Ȃ�)
	BYTE ReadMem( WORD );			// �������A�N�Z�X(�E�F�C�g����) Read
	void WriteMem( WORD, BYTE );	// �������A�N�Z�X(�E�F�C�g����) Write
	BYTE ReadIO( int );				// I/O�|�[�g�A�N�Z�X Read
	void WriteIO( int, BYTE );		// I/O�|�[�g�A�N�Z�X Write
	int GetIntrVector();			// �����݃x�N�^�擾
	bool IsBUSREQ();				// �o�X���N�G�X�g��Ԓ�~�t���O�擾
	
public:
	CPU6( VM6 *, const ID& );		// �R���X�g���N�^
	~CPU6();						// �f�X�g���N�^
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};

#endif		// CPUM_H_INCLUDED
