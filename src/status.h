#ifndef STATUS_H_INCLUDED
#define STATUS_H_INCLUDED

#include "typedef.h"
#include "console.h"
#include "p6vm.h"


//------------------------------------------------------
//  �X�e�[�^�X�o�[�N���X
//------------------------------------------------------
class cWndStat : public ZCons {
private:
	VM6* vm;
	
	int DrvNum;							// �h���C�u��
	int	ReplayStatus;					// ���v���C�X�e�[�^�X
	
public:
	cWndStat( VM6 * );					// �R���X�g���N�^
	~cWndStat();						// �f�X�g���N�^
	
	bool Init( int, int = -1 );			// ������
	void Update();						// �E�B���h�E�X�V
	
	void SetReplayStatus( int );		// ���v���C�X�e�[�^�X�Z�b�g
};

#endif	// STATUS_H_INCLUDED
