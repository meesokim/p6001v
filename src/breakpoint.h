#ifndef BREAKPOINTL_H_INCLUDED
#define BREAKPOINTL_H_INCLUDED

#include "typedef.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// �u���[�N�|�C���g�̐�
#define	NR_BP	(10)

class BPoint {
public:
	enum BPtype{ BP_NONE, BP_PC, BP_READ, BP_WRITE, BP_IN, BP_OUT, EndofBPtype };
	
private:
	struct BreakPoint {
		BPtype Type;
		WORD Addr;
		
		BreakPoint() : Type(BP_NONE), Addr(0) {}
	};
	BreakPoint BP[NR_BP];	// �u���[�N�|�C���g
	
	// �u���[�N�|�C���g�̏��
	int BPNo;
	WORD BPAddr;
	BYTE BPData;
	const char *BPStr;
	
	// �u���[�N�v��
	bool ReqBreak;
	int ReqBPNum;
	
public:
	BPoint();
	~BPoint();
	
	bool ExistBreakPoint() const;						// �u���[�N�|�C���g�̗L�����`�F�b�N
	bool CheckBreakPoint( BPtype, WORD, BYTE, const char * );	// �u���[�N�|�C���g���`�F�b�N
	
	void ClearStatus();									// �u���[�N�|�C���g�̏����N���A
	
	BPtype GetType( int ) const;						// �u���[�N�|�C���g�̃^�C�v���擾
	void SetType( int, BPtype );						// �u���[�N�|�C���g�̃^�C�v��ݒ�
	WORD GetAddr( int ) const;							// �u���[�N�|�C���g�̃A�h���X���擾
	void SetAddr( int, WORD );							// �u���[�N�|�C���g�̃A�h���X��ݒ�
	
	bool IsReqBreak() const;							// �u���[�N�v������?
	int GetReqBPNum() const;							// �u���[�N�v���̂������u���[�N�|�C���gNo.���擾
	void ResetBreak();									// �u���[�N�v���L�����Z��
};

#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#endif	// BREAKPOINTL_H_INCLUDED
