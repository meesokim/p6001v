#include "breakpoint.h"


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
BPoint::BPoint( void ) : BPNo(BP_NONE), BPData(0), BPStr(NULL), ReqBreak(false), ReqBPNum(0) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
BPoint::~BPoint( void ){}


////////////////////////////////////////////////////////////////
// �u���[�N�|�C���g�̗L�����`�F�b�N
////////////////////////////////////////////////////////////////
bool BPoint::ExistBreakPoint( void ) const
{
	for( int i=0; i<NR_BP; i++ )
		if( BP[i].Type != BP_NONE ) return true;
	
	return false;
}


////////////////////////////////////////////////////////////////
// �u���[�N�|�C���g���`�F�b�N
////////////////////////////////////////////////////////////////
bool BPoint::CheckBreakPoint( BPtype type, WORD addr, BYTE data, const char *str )
{
	for( int i=0; i<NR_BP; i++ ){
		if( BP[i].Type == type && BP[i].Addr == addr ){
			BPNo   = i+1;
			BPAddr = addr;
			BPData = data;
			BPStr  = str;
			
			ReqBreak = true;
			ReqBPNum = i;
			
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////
// �u���[�N�|�C���g�̏����N���A
////////////////////////////////////////////////////////////////
void BPoint::ClearStatus( void )
{
	BPNo   = BP_NONE;
	BPAddr = 0;
	BPData = 0;
	BPStr  = NULL;
}


////////////////////////////////////////////////////////////////
// �u���[�N�|�C���g�̃^�C�v���擾
////////////////////////////////////////////////////////////////
BPoint::BPtype BPoint::GetType( int num ) const
{
	return BP[num].Type;
}


////////////////////////////////////////////////////////////////
// �u���[�N�|�C���g�̃^�C�v��ݒ�
////////////////////////////////////////////////////////////////
void BPoint::SetType( int num, BPtype type )
{
	BP[num].Type = type;
}


////////////////////////////////////////////////////////////////
// �u���[�N�|�C���g�̃A�h���X���擾
////////////////////////////////////////////////////////////////
WORD BPoint::GetAddr( int num ) const
{
	return BP[num].Addr;
}


////////////////////////////////////////////////////////////////
// �u���[�N�|�C���g�̃A�h���X��ݒ�
////////////////////////////////////////////////////////////////
void BPoint::SetAddr( int num, WORD addr )
{
	BP[num].Addr = addr;
}


////////////////////////////////////////////////////////////////
// �u���[�N�v������?
////////////////////////////////////////////////////////////////
bool BPoint::IsReqBreak( void ) const
{
	return ReqBreak;
}


////////////////////////////////////////////////////////////////
// �u���[�N�v���̂������u���[�N�|�C���gNo.���擾
////////////////////////////////////////////////////////////////
int BPoint::GetReqBPNum( void ) const
{
	return ReqBPNum;
}


////////////////////////////////////////////////////////////////
// �u���[�N�v���L�����Z��
////////////////////////////////////////////////////////////////
void BPoint::ResetBreak( void )
{
	ReqBreak = false;
	ReqBPNum = 0;
}


#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@