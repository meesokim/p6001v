#include "joystick.h"
#include "osd.h"


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
JOY6::JOY6( void )
{
	INITARRAY( JID, -1 );
	INITARRAY( Jinfo, NULL );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
JOY6::~JOY6( void )
{
	// �I�[�v������Ă�����N���[�Y����
	for( int i=0; i < MAX_JOY; i++ )
		if( Jinfo[i] && OSD_OpenedJoy( i ) ) OSD_CloseJoy( Jinfo[i] );
}


////////////////////////////////////////////////////////////////
// ������
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool JOY6::Init( void )
{
	// �I�[�v������Ă�����N���[�Y����
	for( int i=0; i < MAX_JOY; i++ )
		if( Jinfo[i] && OSD_OpenedJoy( i ) ) OSD_CloseJoy( Jinfo[i] );
	
	Connect( 0, 0 );
	Connect( 1, 1 );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N�ڑ�
//
// ����:	jno		�W���C�X�e�B�b�N�ԍ�(0-1)
//			index	�C���f�b�N�X
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool JOY6::Connect( int jno, int index )
{
	if( jno == 0 || jno == 1 ){
		if( index >= 0 && index < min( OSD_GetJoyNum(), MAX_JOY ) ){
			if( !OSD_OpenedJoy( index ) )
				Jinfo[index] = OSD_OpenJoy( index );
			
			if( OSD_OpenedJoy( index ) ){
				JID[jno] = index;
				return true;
			}
		}
		JID[jno] = -1;
	}
	return false;
}


////////////////////////////////////////////////////////////////
// �C���f�b�N�X�擾
//
// ����:	jno		�W���C�X�e�B�b�N�ԍ�(0-1)
// �Ԓl:	int		�C���f�b�N�X
////////////////////////////////////////////////////////////////
int JOY6::GetID( int jno )
{
	return 	( jno == 0 || jno == 1 ) ? JID[jno] : -1;
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N��Ԏ擾
//
// ����:	jno		�W���C�X�e�B�b�N�ԍ�(0-1)
// �Ԓl:	BYTE	�W���C�X�e�B�b�N�̏�� 1:OFF 0:ON <-����!
//				bit7: �{�^��4
//				bit6: �{�^��3
//				bit5: �{�^��2
//				bit4: �{�^��1
//				bit3: �E
//				bit2: ��
//				bit1: ��
//				bit0: ��
////////////////////////////////////////////////////////////////
BYTE JOY6::GetJoyState( int jno )
{
	BYTE ret = 0;
	
	if( jno == 0 || jno == 1 ){
		if( JID[jno] >= 0 ){
			int Xmove, Ymove;
			
			// ��
			Xmove = OSD_GetJoyAxis( Jinfo[JID[jno]], 0 );
			Ymove = OSD_GetJoyAxis( Jinfo[JID[jno]], 1 );
			
			if( Xmove < INT16_MIN/2 ) ret |= 4;	// ��
			if( Xmove > INT16_MAX/2 ) ret |= 8;	// �E
			if( Ymove < INT16_MIN/2 ) ret |= 1;	// ��
			if( Ymove > INT16_MAX/2 ) ret |= 2;	// ��
			
			// �{�^��
			for( int i=0; i < min( OSD_GetJoyNumButtons( Jinfo[JID[jno]] ), 4 ); i++ )
				if( OSD_GetJoyButton( Jinfo[JID[jno]], i ) ) ret |= 0x10<<i;
		}
	}
	
	return ~ret;
}
