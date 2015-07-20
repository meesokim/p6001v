#ifndef MEMORY_H_INCLUDED
#define MEMORY_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "ini.h"


// �������u���b�N��
#define MAXRMB	(22)
#define MAXWMB	(16)

////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
// �������u���b�N�N���X
class MemBlock {
protected:
	char Name[33];				// �������u���b�N��
	BYTE *MB;					// �������u���b�N�ւ̃|�C���^
	int RWait;					// �A�N�Z�X�E�F�C�g(�Ǎ���)
	int WWait;					// �A�N�Z�X�E�F�C�g(������)
	bool WPt;					// ���C�g�v���e�N�g�t���O
	
public:
	MemBlock();									// �R���X�g���N�^
	~MemBlock();								// �f�X�g���N�^
	
	void SetMemory( const char *, BYTE *, int, int, bool );	// �������u���b�N�ݒ�
	void SetWait( int, int );					// �A�N�Z�X�E�F�C�g�ݒ�
	int GetWait() const;						// �A�N�Z�X�E�F�C�g�擾
	void SetProtect( bool );					// ���C�g�v���e�N�g�ݒ�
	bool GetProtect() const;					// ���C�g�v���e�N�g�擾
	
	const char *GetName() const;				// �������u���b�N���擾
	
	BYTE Read( WORD, int * = NULL ) const;		// ���������[�h
	void Write( WORD, BYTE, int * = NULL ) const;	// ���������C�g
};


class MEM6 : public Device, public IDoko {
protected:
	// ROM���\����
	typedef struct{
		const char *FileName;	// �t�@�C����
		DWORD Crc;				// CRC32
	} ROMINFO;
	
	// ���������\����
	typedef struct{
		const ROMINFO *Rinf;	// ROM���ւ̃|�C���^
		const int Rnum;			// ROM���̗v�f��
		DWORD Size;				// �T�C�Y
		BYTE Init;				// �������f�[�^
		int WaitR;				// �A�N�Z�X�E�F�C�g(Read)
		int WaitW;				// �A�N�Z�X�E�F�C�g(Write)
	} MEMINFO;
	
	// ROM���e�[�u���\����
	struct ROMINFOTABLE {
		const ROMINFO *System1;
		const ROMINFO *System2;
		const ROMINFO *CGRom1;
		const ROMINFO *CGRom2;
		const ROMINFO *Kanji;
		const ROMINFO *Voice;
		
		ROMINFOTABLE() : System1(NULL), System2(NULL), CGRom1(NULL), CGRom2(NULL),
						 Kanji(NULL), Voice(NULL) {}
	};
	
	// ���������e�[�u���\����
	struct MEMINFOTABLE {
		const MEMINFO *EmptRom;
		const MEMINFO *EmptRam;
		const MEMINFO *ExtRom;
		const MEMINFO *IntRam;
		const MEMINFO *ExtRam;
		const MEMINFO *SolRam;
		
		const MEMINFO *System1;
		const MEMINFO *System2;
		const MEMINFO *CGRom1;
		const MEMINFO *CGRom2;
		const MEMINFO *Kanji;
		const MEMINFO *Voice;
		
		MEMINFOTABLE() : EmptRom(NULL), EmptRam(NULL), ExtRom(NULL), IntRam(NULL), ExtRam(NULL),
						 SolRam(NULL), System1(NULL), System2(NULL), CGRom1(NULL), CGRom2(NULL),
						 Kanji(NULL), Voice(NULL) {}
	};
	
	// ���������
	static const MEMINFO IEMPTROM;
	static const MEMINFO IEMPTRAM;
	static const MEMINFO IEXTROM;
	static const MEMINFO IEXTRAM;
	
	ROMINFOTABLE RomTable;		// ROM���e�[�u��
	MEMINFOTABLE MemTable;		// ���������e�[�u��
	
	bool CGBank;				// CG ROM BANK	true:�L�� false:����
	bool UseExtRom;				// �g��ROM		true:�L�� false:����
	bool UseExtRam;				// �g��RAM		true:�L�� false:����
	
	BYTE *MainRom;				// BASIC ROM	ALL (64,68�̎���System ROM1)
	BYTE *SysRom2;				// System ROM2	64,68
	BYTE *ExtRom;				// �g�� ROM		ALL
	BYTE *CGRom1;				// CG ROM1		ALL
	BYTE *CGRom2;				// CG ROM2		62,66
	BYTE *KanjiRom;				// ���� ROM		62,66
	BYTE *VoiceRom;				// �������� ROM	62,66
	
	BYTE *IntRam;				// ���� RAM		ALL
	BYTE *ExtRam;				// �O�� RAM		ALL
	BYTE *EmptyRom;				// ������ROM	ALL
	BYTE *EmptyRam;				// ������RAM	ALL
	
	MemBlock RomB[MAXRMB];		// ROM�u���b�N
	MemBlock RamB[MAXWMB];		// RAM�u���b�N
	
	MemBlock *Rm_blk[8];		// ���[�h���������u���b�N�|�C���^(8KB*8)
	MemBlock *Wm_blk[8];		// ���C�g���������u���b�N�|�C���^(8KB*8)
	
	char FilePath[PATH_MAX];	// �g��ROM�t�@�C���t���p�X
	int M1Wait;					// M1�E�F�C�g
	bool EnableChkCRC;			// CRC�`�F�b�N  true:�L�� false:����
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	bool cgrom;					// CG ROM �I��p true:N60���[�h false:N60m���[�h
	bool kj_rom;				// ����ROM,��������ROM �I��p true:����ROM false:��������ROM
	bool kj_LR;					// ����ROM ���E�I��p true:�E false:��
	bool cgenable;				// CG ROM�A�N�Z�X�t���O true:�A�N�Z�X�� false:�A�N�Z�X�s��
	BYTE cgaddr;				// CG ROM�A�h���X A13,14,15
	BYTE Rf[3];					// �������R���g���[���������W�X�^
	// ---------------------------------------------------------------------------------------
	
	// for 64,68 -----------------------------------------------------------------------------
	MemBlock *Rm_blkSR[8];		// ���[�h���������u���b�N�|�C���^(8KB*8)	SR���[�h�p
	MemBlock *Wm_blkSR[8];		// ���C�g���������u���b�N�|�C���^(8KB*8)	SR���[�h�p
	BYTE RfSR[16];				// �������R���g���[���������W�X�^			SR���[�h�p
	// ---------------------------------------------------------------------------------------
	
	
	bool AllocMemory( BYTE **, const MEMINFO *, const char * );	// �������m�ۂ�ROM�t�@�C���Ǎ���
	virtual void SetRamValue() = 0;			// RAM�̏����l��ݒ�
	virtual bool InitSpecific() = 0;		// �@��ʏ�����
	virtual void SetMemBlockR( BYTE, BYTE ) = 0;	// ���������[�h���̃������u���b�N�w��(62,66)
	virtual void SetMemBlockW( BYTE ) = 0;	// ���������C�g���̃������u���b�N�w��(62,66)
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	void SetWait( BYTE );					// �������A�N�Z�X�E�F�C�g�ݒ�
	BYTE GetWait() const;					// �������A�N�Z�X�E�F�C�g�擾
	void SetCGrom( BYTE );					// CG ROM �A�h���X���ݒ�(62,66)
	void SelectCGrom( int );				// CG ROM �I��(62,66)
	void SelectKanjiRom( BYTE );			// ����ROM ����� ��������ROM �؂�ւ�(62,66)
	// ---------------------------------------------------------------------------------------
	
	// ��m�̃J�[�g���b�W --------------------------------------------------------------------
	static const MEMINFO ISOLRAM;	// ���������
	bool UseSol;					// true:�L�� false:����
	int SolBank;					// ROM�o���N(0-15)
	void SetSolBank( BYTE );				// ROM�o���N�ݒ�
	// ---------------------------------------------------------------------------------------
	
	// I/O�A�N�Z�X�֐�
	void Out7FH( int, BYTE );
	
	// for 62,66,64,68 -----------------------------------------------------------------------
	void OutC1H( int, BYTE );
	void OutC2H( int, BYTE );
	void OutC3H( int, BYTE );
	void OutF0H( int, BYTE );
	void OutF1H( int, BYTE );
	void OutF2H( int, BYTE );
	void OutF3H( int, BYTE );
	void OutF8H( int, BYTE );
	BYTE InF0H( int );
	BYTE InF1H( int );
	BYTE InF2H( int );
	BYTE InF3H( int );
	// ---------------------------------------------------------------------------------------
	
public:
	MEM6( VM6 *, const ID& );				// �R���X�g���N�^
	virtual ~MEM6();						// �f�X�g���N�^
	
	bool AllocAllMemory( const char *, bool );	// �S�������m�ۂ�ROM�t�@�C���Ǎ���
	bool Init( bool, bool );				// ������
	virtual void Patch();					// �p�b�`
	virtual void Reset();					// ���Z�b�g
	
	BYTE Fetch( WORD, int * = NULL ) const;	// �t�F�b�`(M1)
	BYTE Read( WORD, int * = NULL ) const;	// ���������[�h
	void Write( WORD, BYTE, int * = NULL ) const;	// ���������C�g
	
	bool MountExtRom( const char * );		// �g��ROM �}�E���g
	void UnmountExtRom();					// �g��ROM �A���}�E���g
	const char *GetFile() const;			// �g��ROM�t�@�C���p�X�擾
	
	// 8255���o�͊֘A�֐�
	virtual void SetCGBank( bool );			// CG ROM BANK ��؂�ւ���
	
	// ���ڃA�N�Z�X�֐�
	virtual BYTE ReadMainRom( WORD ) const;
	virtual BYTE ReadMainRam( WORD ) const;
	virtual BYTE ReadExtRom ( WORD ) const;
	virtual BYTE ReadCGrom1 ( WORD ) const;
	virtual BYTE ReadCGrom2 ( WORD ) const;
	virtual BYTE ReadCGrom3 ( WORD ) const;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	const char *GetReadMemBlk( int ) const ;
	const char *GetWriteMemBlk( int ) const ;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// �f�o�C�XID
	enum IDOut{ out7FH=0 };
	enum IDIn {};
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class MEM60 : public MEM6 {
private:
	// ROM���
	static const ROMINFO AMAINROM[];
	static const ROMINFO ACGROM1[];
	
	// ���������
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM0;
	static const MEMINFO ICGROM1;
	static const MEMINFO IINTRAM;
	
	void SetRamValue();						// RAM�̏����l��ݒ�
	bool InitSpecific();					// �@��ʏ�����
	void SetMemBlockR( BYTE, BYTE );		// ���������[�h���̃������u���b�N�w��
	void SetMemBlockW( BYTE );				// ���������C�g���̃������u���b�N�w��
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	MEM60( VM6 *, const ID& );				// �R���X�g���N�^
	~MEM60();								// �f�X�g���N�^
	
	void Reset();							// ���Z�b�g
	
	// ���ڃA�N�Z�X�֐�
	BYTE ReadMainRam( WORD ) const ;
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class MEM61 : public MEM60 {
private:
	// ROM���
	static const ROMINFO AMAINROM[];
	static const ROMINFO ACGROM1[];
	
	// ���������
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM0;
	static const MEMINFO ICGROM1;
	static const MEMINFO IINTRAM;
	
public:
	MEM61( VM6 *, const ID& );				// �R���X�g���N�^
	~MEM61();								// �f�X�g���N�^
};


class MEM62 : public MEM6 {
protected:
	// ROM���
	static const ROMINFO AMAINROM[];
	static const ROMINFO ACGROM1[];
	static const ROMINFO ACGROM2[];
	static const ROMINFO AKANJI[];
	static const ROMINFO AVOICE[];
	
	// ���������
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM1;
	static const MEMINFO ICGROM2;
	static const MEMINFO IKANJI;
	static const MEMINFO IVOICE;
	static const MEMINFO IINTRAM;
	
	virtual void SetRamValue();				// RAM�̏����l��ݒ�
	virtual bool InitSpecific();			// �@��ʏ�����
	virtual void SetMemBlockR( BYTE, BYTE );// ���������[�h���̃������u���b�N�w��
	void SetMemBlockW( BYTE );				// ���������C�g���̃������u���b�N�w��
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
public:
	MEM62( VM6 *, const ID& );				// �R���X�g���N�^
	virtual ~MEM62();						// �f�X�g���N�^
	
	virtual void Reset();					// ���Z�b�g
	
	// 8255���o�͊֘A�֐�
	void SetCGBank( bool );					// CG ROM BANK ��؂�ւ���
	
	// ���ڃA�N�Z�X�֐�
	virtual BYTE ReadMainRom( WORD ) const;
	BYTE ReadMainRam( WORD ) const;
	virtual BYTE ReadCGrom2( WORD ) const;
	virtual BYTE ReadKanjiRom( WORD ) const;
	virtual BYTE ReadVoiceRom( WORD ) const;
	
	// �f�o�C�XID
	enum IDOut{ out7FH=0, outC1H, outC2H, outC3H, outF0H,  outF1H, outF2H, outF3H, outF8H };
	enum IDIn {                                    inF0H=0, inF1H,  inF2H,  inF3H         };
	
	// ------------------------------------------
	bool DokoSave( cIni * );			// �ǂ��ł�SAVE
	virtual bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class MEM66 : public MEM62 {
private:
	// ROM���
	static const ROMINFO AMAINROM[];
	static const ROMINFO ACGROM1[];
	static const ROMINFO ACGROM2[];
	static const ROMINFO AKANJI[];
	static const ROMINFO AVOICE[];
	
	// ���������
	static const MEMINFO IMAINROM;
	static const MEMINFO ICGROM1;
	static const MEMINFO ICGROM2;
	static const MEMINFO IKANJI;
	static const MEMINFO IVOICE;
	static const MEMINFO IINTRAM;
	
public:
	MEM66( VM6 *, const ID& );				// �R���X�g���N�^
	~MEM66();								// �f�X�g���N�^
};


class MEM64 : public MEM62 {
protected:
	// ROM���
	static const ROMINFO ASYSROM1[];
	static const ROMINFO ASYSROM2[];
	static const ROMINFO ACGROM[];
	
	// ���������
	static const MEMINFO ISYSROM1;
	static const MEMINFO ISYSROM2;
	static const MEMINFO ICGROM;
	static const MEMINFO IINTRAM;
	
	virtual void SetRamValue();				// RAM�̏����l��ݒ�
	bool InitSpecific();					// �@��ʏ�����
	void SetMemBlockR( BYTE, BYTE );		// ���������[�h���̃������u���b�N�w��
	void SetMemBlockSR( BYTE, BYTE );		// ���������[�h/���C�g���̃������u���b�N�w��(64,68)
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	void Out6xH( int, BYTE );
	void OutC8H( int, BYTE );
	
	BYTE In6xH( int );
	virtual BYTE InB2H( int );
	
public:
	MEM64( VM6 *, const ID& );				// �R���X�g���N�^
	virtual ~MEM64();						// �f�X�g���N�^
	
	void Patch();							// ** �p�b�`
	void Reset();							// ** ���Z�b�g
	
	// ���ڃA�N�Z�X�֐�
	BYTE ReadMainRom( WORD ) const;
	BYTE ReadCGrom1( WORD ) const;
	BYTE ReadCGrom2( WORD ) const;
	BYTE ReadCGrom3( WORD ) const;
	BYTE ReadKanjiRom( WORD ) const;
	BYTE ReadVoiceRom( WORD ) const;
	
	// �f�o�C�XID
	enum IDOut{ out6xH=0, out7FH, outC1H, outC2H, outC3H, outF0H, outF1H, outF2H, outF3H, outF8H };
	enum IDIn {  in6xH=0,                                  inF0H,  inF1H,  inF2H,  inF3H,
				 inB2H };
	
	// ------------------------------------------
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};


class MEM68 : public MEM64 {
protected:
	void SetRamValue();						// RAM�̏����l��ݒ�
	
	BYTE InB2H( int );
	
public:
	MEM68( VM6 *, const ID& );				// �R���X�g���N�^
	virtual ~MEM68();						// �f�X�g���N�^
};

#endif	// MEMORY_H_INCLUDED
