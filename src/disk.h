#ifndef DISK_H_INCLUDED
#define DISK_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "d88.h"
#include "ini.h"

#define	MAXDRV	4	// �ő�h���C�u�ڑ���


// ���u�^�C�v
enum UnitType
{
	PC6031 = 0,
	PC6031SR,
	PC8031,
	PC80S31,
	PC6601,
	PC6601SR,
	EndofUnitType
};

// �h���C�u�^�C�v
enum FddType
{
	FDD1D = 0,
	FDD1DD,
	FDD2D,
	FDD2DD,
	
	EndofFddType
};

// �R�}���h
// PC-6031����(�����т��񒲍��x�[�X)
enum FddCommand
{
	INITIALIZE			= 0x00,
	WRITE_DATA			= 0x01,
	READ_DATA			= 0x02,
	SEND_DATA			= 0x03,
	FORMAT				= 0x05,
	SEND_RESULT_STATUS	= 0x06,
	SEND_DRIVE_STATUS	= 0x07,
	
	COPY				= 0x04,	// PC-6031SR
	GET_MEMORY			= 0x0b,	// PC-6031SR
	FAST_WRITE_DISK		= 0x11,	// PC-6031SR
	FAST_SEND_DATA		= 0x12,	// PC-6031SR
	SET_MODE			= 0x17,	// PC-6031SR
	
	IDLE				= 0xff,	// �����҂��̏��
	EndofFddCmd
};

// �~�j�t���b�s�[�f�B�X�N �e����
struct DISK60 {
	FddType Type;		// �h���C�u�^�C�v
	
	bool DAC;			// Data Accepted	:�f�[�^��M��
	bool RFD;			// Ready For Data	:�f�[�^��M������
	bool DAV;			// Data Valid		:�f�[�^���M��
	
	int command;		// �󂯎�����R�}���h
	int step;			// �p�����[�^���͑҂��X�e�[�^�X
	
	int blk;			// �]���u���b�N��
	int drv;			// �h���C�u�ԍ�-1
	int trk;			// �g���b�N�ԍ�
	int sct;			// �Z�N�^�ԍ�
	
	int rsize;			// �Ǎ��݃o�b�t�@�̃f�[�^��
	int wsize;			// �����݃o�b�t�@�̃f�[�^��
	int ridx;
	
	int size;			// ��������o�C�g��
	
	bool Fast;			// �����A�N�Z�X�t���O true:�L�� false:����
	bool FastStat;		// �����A�N�Z�X���A�N�Z�X�f�[�^�t���O true:2�o�C�g�� false:1�o�C�g��
	
	BYTE retdat;		// port D0H ����Ԃ��l
	
	BYTE busy;			// �h���C�uBUSY 1:�h���C�u1 2:�h���C�u2
	
	bool error;			// �G���[�t���O true:�G���[���� false:�G���[�Ȃ�
	
	DISK60() :
		Type(FDD1D), DAC(0), RFD(0), DAV(0),
		command(IDLE), step(0),
		blk(0), drv(0), trk(0), sct(0),
		size(0), Fast(false), FastStat(false),
		retdat(0xff), busy(0), error(false) {}
};


enum FdcPhase {
	IDLEP = 0,
	C_PHASE,
	E_PHASE,
	R_PHASE
};

enum FdcSeek{
	SK_STOP = 0,	// �V�[�N�Ȃ�
	SK_SEEK,		// �V�[�N��
	SK_END			// �V�[�N����
};

struct PD765 {
	BYTE command;		// �R�}���h
	
//	FdcPhase phase;		// Phase (C/E/R)
//	int step;			// Phase���̏����菇
	
	BYTE SRT;			// Step Rate Time
	BYTE HUT;			// Head Unloaded Time
	BYTE HLT;			// Head Load Time
	bool ND;			// Non DMA Mode  true:Non DMA false:DMA
	
	FdcSeek SeekSta[4];	// �V�[�N���
	BYTE NCN[4];		// New Cylinder Number
	BYTE PCN[4];		// Present Cylinder Number
	
	
	BYTE MT;			// Multi-track
	BYTE MF;			// MFM/FM Mode
	BYTE SK;			// Skip
	BYTE HD;			// Head
	BYTE US;			// Unit Select
	
	BYTE C;				// Cylinder Number
	BYTE H;				// Head Address
	BYTE R;				// Record
	BYTE N;				// Number
	BYTE EOT;			// End of Track
	BYTE GPL;			// Gap Length
	BYTE DTL;			// Data length
	
	BYTE D;				// Format Data
	BYTE SC;			// Sector
	
	BYTE ST0;			// ST0
	BYTE ST1;			// ST1
	BYTE ST2;			// ST2
	BYTE ST3;			// ST3
	
	BYTE Status;		// Status
	bool Intr;			// FDC�����ݔ����t���O
	
		PD765() :
		command(0), // phase(R_PHASE), step(0),
		SRT(32), HUT(0), HLT(0), ND(false),
		MT(0), MF(0), SK(0), HD(0), US(0), C(0), H(0), R(0), N(0),
		EOT(0), GPL(0), DTL(0),
		ST0(0), ST1(0), ST2(0), ST3(0), Status(0), Intr(false)
		{
			INITARRAY( SeekSta, SK_STOP );
			INITARRAY( NCN, 0 );
			INITARRAY( PCN, 0 );
		}
};


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class DSK6 : public Device, public IDoko {
protected:
	UnitType UType;						// ���u�^�C�v
	int DrvNum;							// �h���C�u��
	char FilePath[MAXDRV][PATH_MAX];	// �t�@�C���p�X
	cD88 *Dimg[MAXDRV];					// �f�B�X�N�C���[�W�I�u�W�F�N�g�ւ̃|�C���^
	bool Sys[MAXDRV];					// �V�X�e���f�B�X�N�t���O
	bool DDDrv[MAXDRV];					// 1DD�h���C�u�t���O
	int waitcnt;						// �E�F�C�g�J�E���^
	
	void ResetWait();					// �E�F�C�g�J�E���^���Z�b�g
	void AddWait( int );				// �E�F�C�g�J�E���^���Z
	bool SetWait( int );				// �E�F�C�g�ݒ�
	
public:
	DSK6( VM6 *, const ID& );			// �R���X�g���N�^
	virtual ~DSK6();					// �f�X�g���N�^
	
	virtual void EventCallback( int, int );	// �C�x���g�R�[���o�b�N�֐�
	
	virtual bool Init( int ) = 0;		// ������
	virtual void Reset() = 0;			// ���Z�b�g
	
	bool Mount( int, const char * );	// DISK �}�E���g
	void Unmount( int );				// DISK �A���}�E���g
	
	int GetDrives();					// �h���C�u���擾
	
	bool IsMount( int ) const;			// �}�E���g�ς�?
	bool IsSystem( int ) const;			// �V�X�e���f�B�X�N?
	bool IsProtect( int ) const;		// �v���e�N�g?
	virtual bool InAccess( int ) const = 0;	// �A�N�Z�X��?
	
	const char *GetFile( int ) const;	// �t�@�C���p�X�擾
	const char *GetName( int ) const;	// DISK���擾
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class DSK60 : public DSK6 {
protected:
	DISK60 mdisk;			// �~�j�t���b�s�[�f�B�X�N�e����
	
	BYTE RBuf[4096];		// �Ǎ��݃o�b�t�@
	BYTE WBuf[4096];		// �����݃o�b�t�@
	
	BYTE io_D1H;
	BYTE io_D2H;
	
	BYTE FddIn();			// DISK���j�b�g����̃f�[�^���� 		(port D0H)
	void FddOut( BYTE );	// DISK���j�b�g�ւ̃R�}���h�C�f�[�^�o�� (port D1H)
	BYTE FddCntIn();		// DISK���j�b�g����̐���M������ 		(port D2H)
	void FddCntOut( BYTE );	// DISK���j�b�g�ւ̐���M���o�� 		(port D3H)
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/O�A�N�Z�X�֐�
	void OutD1H( int, BYTE );
	void OutD2H( int, BYTE );
	void OutD3H( int, BYTE );
	BYTE InD0H( int );
	BYTE InD1H( int );
	BYTE InD2H( int );
	
public:
	DSK60( VM6 *, const ID& );			// �R���X�g���N�^
	virtual ~DSK60();					// �f�X�g���N�^
	
	void EventCallback( int, int );		// �C�x���g�R�[���o�b�N�֐�
	
	bool Init( int );					// ������
	void Reset();						// ���Z�b�g
	bool InAccess( int ) const;			// �A�N�Z�X��?
	
	// �f�o�C�XID
	enum IDOut{ outD1H=0, outD2H, outD3H };
	enum IDIn {  inD0H=0,  inD1H,  inD2H };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class DSK64 : public DSK60 {
public:
	DSK64( VM6 *, const ID& );			// �R���X�g���N�^
	virtual ~DSK64();					// �f�X�g���N�^
};


class DSK66 : public DSK6 {
private:
	struct CmdBuffer {
		BYTE Data[10];
		int Index;
		
		CmdBuffer() : Index(0) { *Data = 0; }
	};
	
	BYTE FDDBuf[1024];					// FDD�o�b�t�@
	void BufWrite( int, BYTE );			// FDD�o�b�t�@������
	BYTE BufRead( int );				// FDD�o�b�t�@�Ǎ���
	
	CmdBuffer CmdIn;					// �R�}���h�o�b�t�@
	CmdBuffer CmdOut;					// �X�e�[�^�X�o�b�t�@
	
	// FDC
	PD765 fdc;
	
	// FDCI
	int SendBytes;						// �]����(256Bytes�P��)
	bool ExtDrv;						// FDD�^�C�v true: �O�t false: ����
	bool B2Dir;							// PortB2H �A�N�Z�X���W�X�^ true: �o�� false: ����
	
	void PushStatus( BYTE );			// �X�e�[�^�X�o�b�t�@�Ƀf�[�^������
	BYTE PopStatus();					// �X�e�[�^�X�o�b�t�@����f�[�^�����o��
	
	void OutFDC( BYTE );				// FDC �ɏ�����
	BYTE InFDC();						// FDC ����Ǎ���
	void Exec();						// FDC �R�}���h���s
	
	bool SearchSector( BYTE * );		// �Z�N�^��T��
	
	void ReadDiagnostic();				// Read Diagnostic
	void Specify();						// Specify
	void ReadData();					// Read Data
	void WriteData();					// Write Data
	void Recalibrate();					// Recalibrate
	void Seek();						// Seek
	void SenseInterruptStatus();		// Sense Interrupt Status
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/O�A�N�Z�X�֐�
	void OutB1H( int, BYTE );	// FDCI���[�h�ݒ�
	void OutB3H( int, BYTE );	// PortB2h�̓��o�͐���
	void OutD0H( int, BYTE );	// Buffer
	void OutD1H( int, BYTE );	// Buffer
	void OutD2H( int, BYTE );	// Buffer
	void OutD3H( int, BYTE );	// Buffer
	void OutD6H( int, BYTE );	// FDD���[�^����
	void OutD8H( int, BYTE );	// �������ݕ⏞���� ???
	void OutDAH( int, BYTE );	// �]���ʎw��
	void OutDDH( int, BYTE );	// FDC �f�[�^���W�X�^
	void OutDEH( int, BYTE );	// ?
	
	BYTE InB2H( int );			// FDC INT
	BYTE InD0H( int );			// Buffer
	BYTE InD1H( int );			// Buffer
	BYTE InD2H( int );			// Buffer
	BYTE InD3H( int );			// Buffer
	BYTE InD4H( int );			// FDD���[�^�̏��
	BYTE InDCH( int );			// FDC �X�e�[�^�X���W�X�^
	BYTE InDDH( int );			// FDC �f�[�^���W�X�^

public:
	DSK66( VM6 *, const ID& );			// �R���X�g���N�^
	~DSK66();							// �f�X�g���N�^
	
	void EventCallback( int, int );		// �C�x���g�R�[���o�b�N�֐�
	
	bool Init( int );					// ������
	void Reset();						// ���Z�b�g
	bool InAccess( int ) const;			// �A�N�Z�X��?
	
	// �f�o�C�XID
	enum IDOut{ outB1H=0, outB3H, outD0H, outD1H, outD2H, outD3H, outD6H, outD8H,
				outDAH,   outDDH, outDEH };
	enum IDIn {  inB2H=0,  inD0H,  inD1H,  inD2H,  inD3H,  inD4H,  inDCH,  inDDH };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class DSK68 : public DSK66 {
public:
	DSK68( VM6 *, const ID& );			// �R���X�g���N�^
	virtual ~DSK68();					// �f�X�g���N�^
};

#endif	// DISK_H_INCLUDED
