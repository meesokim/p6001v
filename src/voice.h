#ifndef VOICE_H_INCLUDED
#define VOICE_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "sound.h"
#include "device/pd7752.h"

// [OUT]
//  E0H : �����p�����[�^�]��
//  E1H : ?
//  E2H : ���[�h�Z�b�g : 0 0 0 0 F S1 S0
//			F     : ���̓t���[������ 0: 10ms/�t���[��
//									 1: 20ms/�t���[��
//			S1,S0 : �������xBIT     00: NORMAL SPEED
//									01: SLOW SPEED
//									10: FAST SPEED
//									11: �֎~

//  E3H : �R�}���h�Z�b�g
//		�E������I���R�}���h : 0  0 P5 P4 P3 P2 P1 P0
//			P5-P0 : ��I���r�b�g(0-63)
//		�E�X�g�b�v�R�}���h   : 1  1  1  1  1  1  1  1
//		�E�O����I���R�}���h : 1  1  1  1  1  1  1  0
//

// [IN]
//  E0H : �X�e�[�^�X���W�X�^ : BSY REQ ~INT/EXT ERR 0 0 0 0
//			BSY      : �������� 1:������ 0:��~��
//			REQ      : �����p�����[�^ 1:���͗v�� 0:�֎~
//			~INT/EXT : ���b�Z�[�W�f�[�^ 1:�O�� 0:����
//			ERR      : �G���[�t���O 1:�G���[ 0:�G���[�Ȃ�
//  E1H : ?
//  E2H : PortE2�ɏ����񂾒l?
//  E3H : PortE3�ɏ����񂾒l?


// �����^�C�~���O�Ɋւ��鋓��
// [�{���̃X�y�b�N]
// �E�R�}���h���s����1�t���[���ڂ̃p�����[�^���͊����܂�2ms�ȓ�(NORMAL SPEED)
//   �y�^��z�R�}���h���s����REQ�܂ł̎��Ԃ�?
// �E�t���[�����ōŏ���REQ����S�p�����[�^���͂܂�3/4�t���[�����Ԉȓ�
//   �y�l�@�z���͊������甭���܂ł�1/4�t���[�����Ԉȓ��Ŋm���ɏ��������?
//   �y�^��z����REQ�܂ł̎��Ԃ�?
//
// [�����X�y�b�N]
// �E�R�}���h���s����1�t���[���ڂ̃p�����[�^���͊����܂�1�t���[�����Ԉȓ�
// �E1�t���[�����Ԍo�ߎ��_�Ŕ���(1�t���[�����̃T���v������)�C������REQ=1
// �E�������玟�t���[���̃p�����[�^���͊����܂�1�t���[�����Ԉȓ�


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class VCE6 : public Device, public cD7752, public SndDev, public IDoko {
protected:
	char FilePath[PATH_MAX];		// WAVE�t�@�C���i�[�p�X
	
	BYTE io_E0H;
	BYTE io_E2H;
	BYTE io_E3H;
	
	int VStat;						// �X�e�[�^�X
	
	// ������֌W
	int IVLen;						// �T���v����
	int *IVBuf;						// �f�[�^�o�b�t�@
	int IVPos;						// �Ǎ��݃|�C���^
	
	// �p�����[�^�o�b�t�@(1�t���[���̃p�����[�^����7��)
	BYTE ParaBuf[7];				// �p�����[�^�o�b�t�@
	int	Pnum;						// �p�����[�^��
	int Fnum;						// �J��Ԃ��t���[����
	bool PReady;					// �p�����[�^�Z�b�g����
	
	D7752_SAMPLE *Fbuf;				// �t���[���o�b�t�@�|�C���^(10kHz 1�t���[��)
	
	void VSetMode( BYTE );			// ���[�h�Z�b�g
	void VSetCommand( BYTE );		// �R�}���h�Z�b�g
	void VSetData( BYTE );			// �����p�����[�^�]��
	int VGetStatus();				// �X�e�[�^�X���W�X�^�擾
	void AbortVoice();				// ������~
	void UpConvert();				// �T���v�����O���[�g�ϊ�
	bool LoadVoice( int );			// ������WAV�Ǎ���
	void FreeVoice();				// ������WAV�J��
	virtual void ReqIntr();			// �������������ݗv��
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/O�A�N�Z�X�֐�
	void OutE0H( int, BYTE );
	void OutE2H( int, BYTE );
	void OutE3H( int, BYTE );
	BYTE InE0H( int );
	BYTE InE2H( int );
	BYTE InE3H( int );
	
public:
	VCE6( VM6 *, const ID& );		// �R���X�g���N�^
	virtual ~VCE6();				// �f�X�g���N�^
	
	void EventCallback( int, int );	// �C�x���g�R�[���o�b�N�֐�
	
	bool Init( int, const char * );	// ������
	void Reset( void );				// ���Z�b�g
	
	int SoundUpdate( int );			// �X�g���[���X�V
	
	// �f�o�C�XID
	enum IDOut{ outE0H=0, outE2H, outE3H };
	enum IDIn {  inE0H=0,  inE2H,  inE3H };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};

class VCE64 : public VCE6 {
protected:
	void ReqIntr();					// �������������ݗv��
	
public:
	VCE64( VM6 *, const ID& );		// �R���X�g���N�^
	virtual ~VCE64();				// �f�X�g���N�^
};

#endif	// VOICE_H_INCLUDED
