#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "console.h"


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define MAX_ARGS	(8)
#define	MAX_CHRS	(256)
#define	MAX_HIS		(11)

//------------------------------------------------------
//  ���j�^���[�h�E�B���h�E �C���^�[�t�F�[�X(?)�N���X
//------------------------------------------------------
class iMon : public Device, public ZCons {
private:
	int x;								// X���W(���j�^���[�h���ł̕\���ʒu)
	int y;								// Y���W(���j�^���[�h���ł̕\���ʒu)
	
public:
	iMon( VM6 *, const ID& );			// �R���X�g���N�^
	~iMon();							// �f�X�g���N�^
	
	int X();							// X���W�擾
	int Y();							// Y���W�擾
	void SetX( int );					// X���W�ݒ�
	void SetY( int );					// Y���W�ݒ�
	
	virtual bool Init() = 0;			// ������
	virtual void Update() = 0;			// �E�B���h�E�X�V
};


//------------------------------------------------------
//  �������_���v�E�B���h�E�N���X
//------------------------------------------------------
class cWndMem : public iMon {
private:
	WORD Addr;							// �\���A�h���X
	
public:
	cWndMem( VM6 *, const ID& );		// �R���X�g���N�^
	~cWndMem();							// �f�X�g���N�^
	
	bool Init();						// ������
	void Update();						// �E�B���h�E�X�V
	
	void SetAddress( WORD );			// �\���A�h���X�ݒ�
	WORD GetAddress();					// �\���A�h���X�擾
};


//------------------------------------------------------
//  ���W�X�^�E�B���h�E�N���X
//------------------------------------------------------
class cWndReg : public iMon {
private:
	
public:
	cWndReg( VM6 *, const ID& );		// �R���X�g���N�^
	~cWndReg();							// �f�X�g���N�^
	
	bool Init();						// ������
	void Update();						// �E�B���h�E�X�V
};


//------------------------------------------------------
//  ���j�^�E�B���h�E�N���X
//------------------------------------------------------
class cWndMon : public iMon {
private:
	char KeyBuf[MAX_CHRS];				// �L�[���̓o�b�t�@
	char HisBuf[MAX_HIS][MAX_CHRS];		// �L�[���̓q�X�g���o�b�t�@
	char *Argv[MAX_ARGS];				// �����ւ̃|�C���^�z��
	int Argc;							// �����̌�
	int ArgvCounter;					// Shift()�p�J�E���^
	
	struct argv{						// ���������p
		int Type;	// �����̎��		ARGV_xxx
		int Val;	// �����̒l			ARG_xxx �܂��́A��
		char *Str;	// �����̕�����		Argv[xxx]�Ɠ���
		argv() : Type(0), Val(0), Str(NULL) {}
	} argv;
	
	int GetArg();						// ��������
	void Shift();						// �����z��V�t�g
	void Exec( int );					// �R�}���h���s
	void Help( int );					// �w���v�\��
	
public:
	cWndMon( VM6 *, const ID& );		// �R���X�g���N�^
	~cWndMon();							// �f�X�g���N�^
	
	bool Init();						// ������
	void Update();						// �E�B���h�E�X�V
	
	void KeyIn( int, bool, int );		// �L�[���͏���
	
	void BreakIn( WORD );				// �u���[�N�|�C���g���B
};







//------------------------------------------------------
//  ���j�^���[�h�N���X
//------------------------------------------------------
class Monitor {
private:
	VM6 *vm;
	iMon *dcn[3];
	
public:
	Monitor( VM6 * );					// �R���X�g���N�^
	~Monitor();							// �f�X�g���N�^
	
	bool Init();						// ������
	void Update();						// �E�B���h�E�X�V
	
	int Width();						// ���j�^���[�h �E�B���h�E���擾
	int Height();						// ���j�^���[�h �E�B���h�E�����擾
	
	// �������E�B���h�E
	void SetAddress( WORD );			// �\���A�h���X�ݒ�
	WORD GetAddress();					// �\���A�h���X�擾
	
	// ���j�^�E�B���h�E
	void KeyIn( int, bool, int );		// �L�[���͏���
	void BreakIn( WORD );				// �u���[�N�|�C���g���B
};




#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#endif	// DEBUG_H_INCLUDED
