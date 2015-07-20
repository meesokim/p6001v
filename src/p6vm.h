#ifndef P6VM_H_INCLUDED
#define P6VM_H_INCLUDED

#include "typedef.h"
#include "io.h"
#include "schedule.h"

#include "device/z80.h"

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#include "breakpoint.h"
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

class EL6;
class CFG6;

class CPU6;
class SUB6;
class IO6;
class MEM6;
class IRQ6;
class PIO6;
class VDG6;
class KEY6;
class PSGb;
class VCE6;
class CMTL;
class CMTS;
class DSK6;

class EVSC;
class BPoint;


// ��{���z�}�V���N���X
class VM6 {
	
	friend class EL6;
	friend class DSP6;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	friend class cWndMon;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
protected:
	// �f�o�C�X�R�l�N�^�e�[�u���\����
	struct DEVCONNTABLE {
		const IOBus::Connector *Intr;		// ������
		const IOBus::Connector *Memory;		// ������
		const IOBus::Connector *Vdg;		// VDG
		const IOBus::Connector *Psg;		// PSG/OPN
		const IOBus::Connector *M8255;		// I/O(Z80��)
		const IOBus::Connector *S8255;		// I/O(SUB CPU��)
		const IOBus::Connector *Voice;		// ��������
		const IOBus::Connector *Disk;		// DISK
		const IOBus::Connector *CmtL;		// CMT(LOAD)
		const IOBus::Connector *Soldier;	// ��m�̃J�[�g���b�W
		
		DEVCONNTABLE() : Intr(NULL), Memory(NULL), Vdg(NULL), Psg(NULL), M8255(NULL), S8255(NULL),
						 Voice(NULL), Disk(NULL), CmtL(NULL), Soldier(NULL) {}
	};
	
	
	int cclock;					// CPU�N���b�N
	int pclock;					// PSG/OPN�N���b�N
	
	// �I�u�W�F�N�g�|�C���^
	EL6 *el;					// �G�~�����[�^���C��
	EVSC *evsc;					// �C�x���g�X�P�W���[��
	IO6 *iom;					// I/O(Z80��)
	IO6 *ios;					// I/O(SUB CPU��)
	IRQ6 *intr;					// ������
	CPU6 *cpum;					// CPU
	SUB6 *cpus;					// SUB CPU
	MEM6 *mem;					// ������
	VDG6 *vdg;					// VDG
	PSGb *psg;					// PSG/OPN
	VCE6 *voice;				// ��������
	PIO6 *pio;					// 8255
	KEY6 *key;					// �L�[
	CMTL *cmtl;					// CMT(LOAD)
	CMTS *cmts;					// CMT(SAVE)
	DSK6 *disk;					// DISK
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	BPoint *bp;					// �u���[�N�|�C���g
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	DEVCONNTABLE DevTable;		// �f�o�C�X�R�l�N�^�e�[�u��
	
	virtual bool AllocObjSpecific() = 0;		// �@��ʃI�u�W�F�N�g�m��
	
public:
	VM6( EL6 * );								// �R���X�g���N�^
	virtual ~VM6();								// �f�X�g���N�^
	
	// �f�o�C�X�R�l�N�^
	const static IOBus::Connector c_soldier[];	// ��m�̃J�[�g���b�W
	
	bool AllocObject( CFG6 * );					// �S�I�u�W�F�N�g�m��
	bool Init( CFG6 * );						// ������
	void Reset();								// ���Z�b�g
	int Emu();									// 1���ߎ��s
	int GetCPUClock() const;					// CPU�N���b�N�擾
	
	
	// P6�f�o�C�X�p�֐��Q
	// EL
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	bool ElIsMonitor() const;					// ���j�^���[�h?
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// EVSC
	bool EventAdd( Device *, int, double, int );	// �C�x���g�ǉ�
	bool EventDel( Device *, int );				// �C�x���g�폜
	void EventUpdate( int );					// �C�x���g�X�V
	void EventReset( Device::ID, int, double=0 );	// �w��C�x���g�����Z�b�g����
	double EventGetProgress( Device::ID, int );	// �C�x���g�̐i�s�������߂�
	bool EventGetInfo( EVSC::evinfo * );		// �C�x���g���擾
	bool EventSetInfo( EVSC::evinfo * );		// �C�x���g���ݒ�
	void EventOnVSYNC();						// VSYNC��ʒm����
	// IO6
	BYTE IomIn( int, int * = NULL );			// IN�֐�
	BYTE IosIn( int, int * = NULL );			// IN�֐�
	void IomOut( int, BYTE, int * = NULL );		// OUT�֐�
	void IosOut( int, BYTE, int * = NULL );		// OUT�֐�
	// IRQ6
	int	IntIntrCheck();							// �����݃`�F�b�N
	void IntReqIntr( DWORD );					// �����ݗv��
	void IntCancelIntr( DWORD );				// �����ݓP��
	// CPU6
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	int CpumDisasm( char *, WORD );				// 1���C���t�A�Z���u��
	void CpumGetRegister( cZ80::Register * );	// ���W�X�^�l�擾
	void CpumSetRegister( cZ80::Register * );	// ���W�X�^�l�ݒ�
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// SUB6
	void CpusReqKeyIntr( int, BYTE );			// �L�[�����ݗv��
	void CpusExtIntr();							// �O�������ݗv��
	void CpusReqCmtIntr( BYTE );				// CMT READ�����ݗv��
	bool CpusIsCmtIntrReady();					// CMT�����ݔ�����?
	// MEM6
	BYTE MemFetch( WORD, int * = NULL );		// �t�F�b�`(M1)
	BYTE MemRead( WORD, int * = NULL );			// ���������[�h
	void MemWrite( WORD, BYTE, int * = NULL );	// ���������C�g
	void MemSetCGBank( bool );					// CG ROM BANK ��؂�ւ���
	BYTE MemReadMainRom( WORD ) const;			// ���ړǍ���
	BYTE MemReadMainRam( WORD ) const;			// ���ړǍ���
	BYTE MemReadExtRom ( WORD ) const;			// ���ړǍ���
	BYTE MemReadCGrom1 ( WORD ) const;			// ���ړǍ���
	BYTE MemReadCGrom2 ( WORD ) const;			// ���ړǍ���
	BYTE MemReadCGrom3 ( WORD ) const;			// ���ړǍ���
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	const char *MemGetReadMemBlk( int ) const;	// �������u���b�N�擾(Read)
	const char *MemGetWriteMemBlk( int ) const;	// �������u���b�N�擾(Write)
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// VDG6
	bool VdgGetCrtDisp() const;					// CRT�\����Ԏ擾
	void VdgSetCrtDisp( bool );					// CRT�\����Ԑݒ�
	bool VdgGetWinSize() const;					// �E�B���h�E�T�C�Y�擾
	bool VdgIsBusReqStop() const;				// �o�X���N�G�X�g��Ԓ�~�t���O�擾
	bool VdgIsBusReqExec() const;				// �o�X���N�G�X�g��Ԏ��s�t���O�擾
	bool VdgIsSRmode() const;					// SR���[�h�擾
	bool VdgIsSRGVramAccess( WORD, bool ) const;	// SR��G-VRAM�A�N�Z�X?
	WORD VdgSRGVramAddr( WORD ) const;			// SR��G-VRAM�A�h���X�擾
	// PIO6
	BYTE PioReadA();							// PartA ���[�h
	// KEY6
	BYTE KeyGetKeyJoy() const;					// �J�[�\���L�[��Ԏ擾
	BYTE KeyGetJoy( int ) const;				// �W���C�X�e�B�b�N��Ԏ擾
	BYTE KeyGetKeyIndicator() const;			// �L�[�{�[�h�C���W�P�[�^��Ԏ擾
	void KeyChangeKana();						// �p��<->���Ȑ؊�
	void KeyChangeKKana();						// ����<->�J�i�؊�
	// CMTL
	bool CmtlIsMount() const;					// �}�E���g�ς�?
	bool CmtlIsAutoStart() const;				// �I�[�g�X�^�[�g?
	const char *CmtlGetFile() const;			// �t�@�C���p�X�擾
	const char *CmtlGetName() const;			// TAPE���擾
	DWORD CmtlGetSize() const;					// �x�^�C���[�W�T�C�Y�擾
	int CmtlGetCount() const;					// �J�E���^�擾
	// CMTS
	bool CmtsMount();							// TAPE �}�E���g
	void CmtsUnmount();							// TAPE �A���}�E���g
	void CmtsSetBaud( int );					// �{�[���[�g�ݒ�
	void CmtsCmtWrite( BYTE );					// CMT 1����������
	// DSK6
	bool DskIsMount( int ) const;				// �}�E���g�ς�?
	bool DskIsSystem( int ) const;				// �V�X�e���f�B�X�N?
	bool DskIsProtect( int ) const;				// �v���e�N�g?
	bool DskInAccess( int ) const;				// �A�N�Z�X��?
	const char *DskGetFile( int ) const;		// �t�@�C���p�X�擾
	const char *DskGetName( int ) const;		// DISK���擾
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	// BPoint
	bool BpExistBreakPoint() const;				// �u���[�N�|�C���g�̗L�����`�F�b�N
	bool BpCheckBreakPoint( BPoint::BPtype, WORD, BYTE, const char * );	// �u���[�N�|�C���g���`�F�b�N
	void BpClearStatus();						// �u���[�N�|�C���g�̏����N���A
	BPoint::BPtype BpGetType( int ) const;		// �u���[�N�|�C���g�̃^�C�v���擾
	void BpSetType( int, BPoint::BPtype );		// �u���[�N�|�C���g�̃^�C�v��ݒ�
	WORD BpGetAddr( int ) const;				// �u���[�N�|�C���g�̃A�h���X���擾
	void BpSetAddr( int, WORD );				// �u���[�N�|�C���g�̃A�h���X��ݒ�
	bool BpIsReqBreak() const;					// �u���[�N�v������?
	int BpGetReqBPNum() const;					// �u���[�N�v���̂������u���[�N�|�C���gNo.���擾
	void BpResetBreak();						// �u���[�N�v���L�����Z��
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
};


// PC-6001 ���z�}�V���N���X
class VM60 : public VM6 {
private:
	bool AllocObjSpecific();					// �@��ʃI�u�W�F�N�g�m��
	
public:
	VM60( EL6 * );								// �R���X�g���N�^
	~VM60();									// �f�X�g���N�^
	
	// �f�o�C�X�R�l�N�^
	const static IOBus::Connector c_intr[];		// ������
	const static IOBus::Connector c_vdg[];		// VDG
	const static IOBus::Connector c_psg[];		// PSG
	const static IOBus::Connector c_8255m[];	// I/O(Z80��)
	const static IOBus::Connector c_8255s[];	// I/O(SUB CPU��)
	const static IOBus::Connector c_disk[];		// DISK
	const static IOBus::Connector c_cmtl[];		// CMT(LOAD)
};


// PC-6001A ���z�}�V���N���X
class VM61 : public VM6 {
private:
	bool AllocObjSpecific();					// �@��ʃI�u�W�F�N�g�m��
	
public:
	VM61( EL6 * );								// �R���X�g���N�^
	~VM61();									// �f�X�g���N�^
};


// PC-6001mk2 ���z�}�V���N���X
class VM62 : public VM6 {
private:
	bool AllocObjSpecific();					// �@��ʃI�u�W�F�N�g�m��
	
public:
	VM62( EL6 * );								// �R���X�g���N�^
	~VM62();									// �f�X�g���N�^
	
	// �f�o�C�X�R�l�N�^
	const static IOBus::Connector c_intr[];		// ������
	const static IOBus::Connector c_memory[];	// ������
	const static IOBus::Connector c_vdg[];		// VDG
	const static IOBus::Connector c_psg[];		// PSG
	const static IOBus::Connector c_8255m[];	// I/O(Z80��)
	const static IOBus::Connector c_8255s[];	// I/O(SUB CPU��)
	const static IOBus::Connector c_voice[];	// ��������
	const static IOBus::Connector c_disk[];		// DISK
	const static IOBus::Connector c_cmtl[];		// CMT(LOAD)
};


// PC-6601 ���z�}�V���N���X
class VM66 : public VM6 {
private:
	bool AllocObjSpecific();					// �@��ʃI�u�W�F�N�g�m��
	
public:
	VM66( EL6 * );								// �R���X�g���N�^
	~VM66();									// �f�X�g���N�^
	
	// �f�o�C�X�R�l�N�^
	const static IOBus::Connector c_disk[];		// DISK
};


// PC-6001mk2SR ���z�}�V���N���X
class VM64 : public VM6 {
private:
	bool AllocObjSpecific();					// �@��ʃI�u�W�F�N�g�m��
	
public:
	VM64( EL6 * );								// �R���X�g���N�^
	~VM64();									// �f�X�g���N�^
	
	// �f�o�C�X�R�l�N�^
	const static IOBus::Connector c_intr[];		// ������
	const static IOBus::Connector c_memory[];	// ������
	const static IOBus::Connector c_vdg[];		// VDG
	const static IOBus::Connector c_psg[];		// OPN
	const static IOBus::Connector c_8255m[];	// I/O(Z80��)
	const static IOBus::Connector c_8255s[];	// I/O(SUB CPU��)
	const static IOBus::Connector c_voice[];	// ��������
	const static IOBus::Connector c_disk[];		// DISK
	const static IOBus::Connector c_cmtl[];		// CMT(LOAD)
};


// PC-6601SR ���z�}�V���N���X
class VM68 : public VM6 {
private:
	bool AllocObjSpecific();					// �@��ʃI�u�W�F�N�g�m��
	
public:
	VM68( EL6 * );								// �R���X�g���N�^
	~VM68();									// �f�X�g���N�^
	
	// �f�o�C�X�R�l�N�^
	const static IOBus::Connector c_disk[];		// DISK
};


#endif		// P6VM_H_INCLUDED
