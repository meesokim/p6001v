#ifndef P6EL_H_INCLUDED
#define P6EL_H_INCLUDED

#include "typedef.h"
#include "movie.h"
#include "replay.h"
#include "thread.h"
#include "vsurface.h"


class VM6;
class CFG6;
class DSP6;
class SND6;
class SCH6;
class JOY6;
class cWndStat;

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
class cWndReg;
class cWndMem;
class cWndMon;
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
// �G�~�����[�^���C���[�N���X
class EL6 : public cThread, public AVI6, public REPLAY {
	
	friend class VM6;
	friend class DSP6;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	friend class cWndMon;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
public:
	enum ReturnCode {
		Quit = 0,
		Restart,
		Dokoload,
		Replay,
		Error,
		
		EndofReturnCode
	};
	
protected:
	// �I�u�W�F�N�g�|�C���^
	VM6 *vm;					// VM
	CFG6 *cfg;					// ���ݒ�I�u�W�F�N�g
	SCH6 *sche;					// �X�P�W���[��
	DSP6 *graph;				// ��ʕ`��
	SND6 *snd;					// �T�E���h
	JOY6 *joy;					// �W���C�X�e�B�b�N
	
	cWndStat *staw;				// �X�e�[�^�X�o�[
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	cWndReg *regw;				// ���W�X�^�E�B���h�E
	cWndMem *memw;				// �������E�B���h�E
	cWndMon *monw;				// ���j�^�E�B���h�E
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	HTIMERID UpDateFPSID;		// FPS�\���^�C�}ID
	VPalette GPal;				// �p���b�g
	int UDFPSCount;				// FPS�\���^�C�}�J�E���^
	int FSkipCount;				// �t���[���X�L�b�v�J�E���^
	
	static int Speed;           // ��~���̑��x�ޔ�p
	
	void DeleteAllObject();					// �S�I�u�W�F�N�g�폜
	
	bool ScreenUpdate();					// ��ʍX�V
	int SoundUpdate( int, cRing * = NULL );	// �T�E���h�X�V
	static void StreamUpdate( void *, BYTE *, int);		// �X�g���[���X�V �R�[���o�b�N�֐�
	static DWORD UpDateFPS( DWORD, void * );	// FPS�\���^�C�} �R�[���o�b�N�֐�
	bool SetFPSTimer( int );				// FPS�\���^�C�}�ݒ�
	
	void OnThread( void * );				// �X���b�h�֐�
	
	int Emu();								// 1���ߎ��s
	int EmuVSYNC();							// 1��ʕ����s
	void Wait();							// Wait
	
	bool CheckFuncKey( int, bool, bool );	// �e��@�\�L�[�`�F�b�N
	
	// �����L�[���͏��\����
	struct AKEY{
		char *Buffer;			// �L�[�o�b�t�@�|�C���^
		int Num;				// �c�蕶����
		int Wait;				// �҂��񐔃J�E���^
		bool Relay;				// �����[�X�C�b�`OFF�҂��t���O
		bool RelayOn;			// �����[�X�C�b�`ON�҂��t���O
		int Seek;
		
		AKEY() : Buffer(NULL), Num(0), Wait(0), Relay(false), RelayOn(false), Seek(0) {}
	};
	AKEY ak;								// �����L�[���͏��
	
	char GetAutoKey();						// �����L�[����1�����擾
	
	
	// UI�֘A
	char TapePathUI[PATH_MAX];	// TAPE�p�X
	char DiskPathUI[PATH_MAX];	// DISK�p�X
	char ExRomPathUI[PATH_MAX];	// �g��ROM�p�X
	char DokoPathUI[PATH_MAX];	// �ǂ��ł�SAVE�p�X
	
	void UI_TapeInsert( const char * = NULL );		// TAPE �}��
	void UI_DiskInsert( int, const char * = NULL );	// DISK �}��
	void UI_RomInsert( const char * = NULL );		// �g��ROM �}��
	void UI_RomEject();								// �g��ROM �r�o
	void UI_DokoSave( const char * = NULL );		// �ǂ��ł�SAVE
	void UI_DokoLoad( const char * = NULL );		// �ǂ��ł�LOAD
	void UI_ReplaySave( const char * = NULL );		// ���v���C�ۑ�
	void UI_ReplayResumeSave( const char * = NULL );	// ���v���C�ۑ��ĊJ
	void UI_ReplayDokoLoad();						// ���v���C���ǂ��ł�LOAD
	void UI_ReplayDokoSave();						// ���v���C���ǂ��ł�SAVE
	void UI_ReplayLoad( const char * = NULL );		// ���v���C�Đ�
	void UI_AVISave();								// �r�f�I�L���v�`��
	void UI_AutoType( const char * = NULL );		// �ō��ݑ�s
	void UI_Reset();						// ���Z�b�g
	void UI_Restart();						// �ċN��
	void UI_Quit();							// �I��
	void UI_NoWait();						// Wait�ύX
	void UI_TurboTape();					// Turbo TAPE�ύX
	void UI_BoostUp();						// Boost Up�ύX
	void UI_ScanLine();						// �X�L�������C�����[�h�ύX
	void UI_Disp43();						// 4:3�\���ύX
	void UI_StatusBar();					// �X�e�[�^�X�o�[�\����ԕύX
	void UI_Mode4Color( int );				// MODE4�J���[�ύX
	void UI_FrameSkip( int );				// �t���[���X�L�b�v�ύX
	void UI_SampleRate( int );				// �T���v�����O���[�g�ύX
	void UI_Config();						// ���ݒ�
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	void Exec( int );						// �w��X�e�[�g�����s
	bool ToggleMonitor();					// ���j�^���[�h�ؑւ�
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	void ShowPopupMenu( int, int );			// �|�b�v�A�b�v���j���[�\��
	void ExecMenu( int );					// ���j���[�I�����ڎ��s
	
	bool TapeMount( const char * );			// TAPE �}�E���g
	void TapeUnmount();						// TAPE �A���}�E���g
	bool DiskMount( int, const char * );	// DISK �}�E���g
	void DiskUnmount( int );				// DISK �A���}�E���g
	
	bool ReplayRecStart( const char * );	// ���v���C�ۑ��J�n
	bool ReplayRecResume(const char *);     // ���v���C�ۑ��ĊJ
	bool ReplayRecDokoLoad();               // ���v���C���ǂ��ł�LOAD
	bool ReplayRecDokoSave();               // ���v���C���ǂ��ł�SAVE
	void ReplayRecStop();					// ���v���C�ۑ���~
	void ReplayPlayStart( const char * );	// ���v���C�Đ��J�n
	void ReplayPlayStop();					// ���v���C�Đ���~
	
	bool IsAutoKey();						// �����L�[���͎��s��?
	bool SetAutoKeyFile( const char * );	// �����L�[���͕�����ݒ�(�t�@�C������)
	bool SetAutoKey( const char *, int );	// �����L�[���͕�����ݒ�
	void SetAutoStart();					// �I�[�g�X�^�[�g������ݒ�
	
public:
	EL6();									// �R���X�g���N�^
	~EL6();									// �f�X�g���N�^
	
	bool Init( const CFG6 * );				// ������
	
	bool Start();							// ����J�n
	void Stop();							// �����~
	
	ReturnCode EventLoop();					// �C�x���g���[�v
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	bool IsMonitor() const;					// ���j�^���[�h?
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// ------------------------------------------
	bool DokoDemoSave( const char * );	// �ǂ��ł�SAVE
	bool DokoDemoLoad( const char * );	// �ǂ��ł�LOAD
	int GetDokoModel( const char * );	// �ǂ��ł�LOAD�t�@�C������@�햼�Ǎ�
	// ------------------------------------------
};


#endif		// P6EL_H_INCLUDED
