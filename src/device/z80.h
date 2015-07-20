#ifndef Z80_H_INCLUDED
#define Z80_H_INCLUDED

#include "../typedef.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class cZ80 {
public:
	// �y�A���W�X�^ �f�[�^�^��`
	typedef union {
	#if BYTEORDER == BIG_ENDIAN
	 	struct { BYTE h,l; } B;
	#else
		struct { BYTE l,h; } B;
	#endif
				 WORD        W;
	} PAIR;
	
	// ���W�X�^�Q �\���̒�`
	struct Register {
		PAIR AF, BC, DE, HL;	// �ėp���W�X�^
		PAIR IX, IY, PC, SP;	// ��p���W�X�^
		PAIR AF1,BC1,DE1,HL1;	//  �� ���W�X�^
		BYTE I, R;				// ���ꃌ�W�X�^
		BYTE R_saved;			// R reg �ۑ��p
		BYTE IFF,IFF2;			// IFF,IFF2
		BYTE IM;				// �������[�h
		BYTE Halt;				// HALT �t���O
	};
	
protected:
	typedef int8_t	offset;
	
	PAIR AF, BC, DE, HL;	// �ėp���W�X�^
	PAIR IX, IY, PC, SP;	// ��p���W�X�^
	PAIR AF1,BC1,DE1,HL1;	//  �� ���W�X�^
	BYTE I, R;				// ���ꃌ�W�X�^
	BYTE R_saved;			// R reg �ۑ��p
	BYTE IFF,IFF2;			// IFF,IFF2
	BYTE IM;				// �������[�h
	BYTE Halt;				// HALT �t���O
	
	int mstate;				// �������A�N�Z�X�E�F�C�g �X�e�[�g��
	
	virtual BYTE Fetch( WORD, int * )   = 0;	// �t�F�b�`(M1)
	virtual BYTE ReadMemNW( WORD )      = 0;	// �������A�N�Z�X(�E�F�C�g�Ȃ�)
	virtual BYTE ReadMem( WORD )        = 0;	// �������A�N�Z�X(�E�F�C�g����) Read
	virtual void WriteMem( WORD, BYTE ) = 0;	// �������A�N�Z�X(�E�F�C�g����) Write
	virtual BYTE ReadIO( int )          = 0;	// I/O�|�[�g�A�N�Z�X Read
	virtual void WriteIO( int, BYTE )   = 0;	// I/O�|�[�g�A�N�Z�X Write
	virtual int GetIntrVector()         = 0;	// �����݃x�N�^�擾
	virtual bool IsBUSREQ()             = 0;	// �o�X���N�G�X�g��Ԓ�~�t���O�擾
	
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void PrintfHead( char *, WORD, int );	// �A�h���X/�C���X�g���N�V�����R�[�h�̕\��
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
public:
	cZ80();						// �R���X�g���N�^
	virtual ~cZ80();			// �f�X�g���N�^
	
	void Reset();				// ���Z�b�g
	int Exec();					// 1���ߎ��s
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int Disasm( char *, WORD );		// 1���C���t�A�Z���u��
	void GetRegister( Register * );	// ���W�X�^�l�擾
	void SetRegister( Register * );	// ���W�X�^�l�ݒ�
	WORD GetPC();					// PC���W�X�^�l�擾
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
};


#endif		// Z80_H_INCLUDED
