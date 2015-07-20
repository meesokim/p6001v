#ifndef P6T2_H_INCLUDED
#define P6T2_H_INCLUDED

#include "typedef.h"
#include "ini.h"


// P6T �u���b�N���
struct P6TBLKINFO {
	int STime;				// �������̎���(ms)
	int PTime;				// �ҁ[���̎���(ms)
	int Offset;				// �x�^�C���[�W�擪����̃I�t�Z�b�g
	int DNum;				// �f�[�^�T�C�Y
	
	P6TBLKINFO() : STime(0), PTime(0), Offset(0), DNum(0) {}
};

// DATA�u���b�N�N���X
class cP6DATA {
private:
	P6TBLKINFO Info;					// P6T �u���b�N���
	BYTE *Data;							// �f�[�^�ւ̃|�C���^
	
	cP6DATA *next;						// ����DATA�u���b�N�ւ̃|�C���^
	cP6DATA *before;					// �O��DATA�u���b�N�ւ̃|�C���^
	
	cP6DATA *TopBlock();				// �擪�u���b�N�ւ̃|�C���^��Ԃ�
	cP6DATA *LastBlock();				// �����u���b�N�ւ̃|�C���^��Ԃ�
	
public:
	cP6DATA();							// �R���X�g���N�^
	~cP6DATA();							// �f�X�g���N�^
	
	cP6DATA *Next();					// ���̃u���b�N�ւ̃|�C���^��Ԃ�
	cP6DATA *Before();					// �O�̃u���b�N�ւ̃|�C���^��Ԃ�
	
	cP6DATA *New();						// �V�KDATA�u���b�N�ǉ�
	cP6DATA *Clone();					// �R�s�[�쐬
	cP6DATA *Clones();					// �S�R�s�[�쐬
	
	P6TBLKINFO *GetInfo();				// P6T �u���b�N���擾
	
	int SetData( FILE *, int );			// �f�[�^�Z�b�g
	void SetPeriod( int, int );			// ������,�ҁ[�����ԃZ�b�g
	
	BYTE Read( int );					// 1Byte�Ǎ���
	
	int Writefd( FILE * );				// �t�@�C���ɏ�����(�f�[�^)
};


// P6T PART���
struct P6TPRTINFO {
	BYTE ID;				// ID�ԍ�
	char Name[17];			// �f�[�^��(16����+'00H')
	WORD Baud;				// �{�[���[�g(600/1200)
	
	P6TPRTINFO() : ID(0), Baud(1200)
	{
		INITARRAY( Name, 0 );
	}
};

// PART�N���X
class cP6PART {
private:
	P6TPRTINFO Info;						// P6T PART���
	cP6DATA *Data;							// �擪DATA�u���b�N�ւ̃|�C���^
	
	cP6PART *next;							// ����PART�ւ̃|�C���^
	cP6PART *before;						// �O��PART�ւ̃|�C���^
	
	cP6PART *TopPart();						// �擪PART�ւ̃|�C���^��Ԃ�
	cP6PART *LastPart();					// ����PART�ւ̃|�C���^��Ԃ�
	
	BYTE Renumber();						// ID�ԍ����i���o�[
	
public:
	cP6PART();								// �R���X�g���N�^
	~cP6PART();								// �f�X�g���N�^
	
	cP6PART *Next();						// ����PART�ւ̃|�C���^��Ԃ�
	cP6PART *Before();						// �O��PART�ւ̃|�C���^��Ԃ�
	cP6PART *Part( int );					// �C�ӂ�PART�ւ̃|�C���^��Ԃ�
	
	cP6PART *New();							// �V�KPART�ǉ�
	cP6PART *Clone();						// �R�s�[�쐬
	cP6PART *Link( cP6PART * );				// ������PART�A��
	
	cP6DATA *NewBlock();					// �V�KDATA�u���b�N�ǉ�
	
	int GetBlocks();						// �SDATA�u���b�N�����擾
	int GetSize();							// PART�T�C�Y�擾
	
	cP6DATA *FirstData();					// �擪DATA�u���b�N�ւ̃|�C���^�擾
	
	P6TPRTINFO *GetInfo();					// P6T PART���擾
	
	int SetName( const char * );			// �f�[�^���ݒ�
	void SetBaud( int );					// �{�[���[�g�ݒ�
	
	bool Readf( FILE * );					// �t�@�C������SPART��Ǎ���
	int Writefd( FILE * );					// �t�@�C���ɏ�����(�f�[�^)
	bool Writeff( FILE * );					// �t�@�C���ɏ�����(�t�b�^)
};


// �I�[�g�X�^�[�g���
struct P6TAUTOINFO {
	bool Start;				// �I�[�g�X�^�[�g�t���O
	BYTE BASIC;				// BASIC���[�h(PC-6001�̏ꍇ�͖��Ӗ�)
	BYTE Page;				// �y�[�W��
	WORD ASKey;				// �I�[�g�X�^�[�g�R�}���h�T�C�Y
	char *ask;				// �I�[�g�X�^�[�g�R�}���h�i�[�̈�ւ̃|�C���^
	
	P6TAUTOINFO() : Start(false), BASIC(1), Page(1), ASKey(0), ask(NULL) {}
};

// P6T�N���X
class cP6T : public IDoko {
private:
	char Name[17];							// �f�[�^��(16����+'00H')
	BYTE Version;							// �o�[�W����
	
	P6TAUTOINFO Ainfo;						// �I�[�g�X�^�[�g���
	
	WORD EHead;								// �g�����T�C�Y(64KB�܂�)
	char *exh;								// �g�����i�[�̈�ւ̃|�C���^
	
	cP6PART *Part;							// �擪PART�ւ̃|�C���^
	
	// �Ǎ��݊֌W���[�N
	cP6PART *rpart;							// ���݂̓Ǎ���PART
	cP6DATA *rdata;							// ���݂̓Ǎ���DATA�u���b�N
	int rpt;								// ���݂̓Ǎ��݃|�C���^(DATA�u���b�N��)
	int swait;								// �������̑҂���
	int pwait;								// �ҁ[���̑҂���
	
	bool ReadP6T( const char * );			// P6T��Ǎ���
	bool ConvP6T( const char * );			// �x�^��P6T�ɕϊ����ēǍ���
	
public:
	cP6T();									// �R���X�g���N�^
	virtual ~cP6T();						// �f�X�g���N�^
	
	int GetBlocks() const;					// �SDATA�u���b�N�����擾
	int GetParts() const;					// �SPART�����擾
	
	cP6PART *GetPart( int ) const;			// �C��PART�ւ̃|�C���^�擾
	
	int SetName( const char * );			// �f�[�^���ݒ�
	const char *GetName() const;			//         �擾
	
	const P6TAUTOINFO *GetAutoStartInfo() const;	// �I�[�g�X�^�[�g���擾
	
	int GetSize() const;					// �x�^�C���[�W�T�C�Y�擾
	
	BYTE ReadOne();							// 1�����Ǎ���
	
	bool IsSWaiting();						// �������҂�?
	bool IsPWaiting();						// �ҁ[���҂�?
	
	bool Readf( const char * );				// �t�@�C������Ǎ���
	bool Writef( const char * );			// �t�@�C���ɏ�����
	
	int GetCount() const;					// TAPE�J�E���^�擾
	void SetCount( int );					//             �ݒ�
	
	void Reset();							// ���Z�b�g(�Ǎ��݊֌W���[�N������)
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};



#endif	// P6T2_H_INCLUDED
