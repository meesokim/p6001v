#ifndef OSD_H_INCLUDED
#define OSD_H_INCLUDED

// OS�ˑ��̔ėp���[�`��(���UI�p)

#include "event.h"
#include "typedef.h"
#include "keydef.h"
#include "vsurface.h"


// �t�@�C���I���_�C�A���O�p
enum FileMode{ FM_Load, FM_Save };
enum FileDlg{ FD_TapeLoad, FD_TapeSave, FD_Disk, FD_ExtRom, FD_Printer, FD_FontZ, FD_FontH,
			  FD_DokoLoad, FD_DokoSave, FD_RepLoad, FD_RepSave, FD_AVISave, FD_LoadAll, EndofFileDlg };

// --- �����񑀍�֐� ---
// �������ɂ�镶�����r
//   stricmp �֐��� not ANSI,not POSIX�ł����BMS�̓Ǝ��g������?
#ifndef stricmp
int stricmp ( const char *, const char * );
#endif

// --- �v���Z�X�Ǘ��֐� ---
// ������
bool OSD_Init();
// �I������
void OSD_Quit();
// ���d�N���`�F�b�N
bool OSD_IsWorking();

// --- �p�X�������֐� ---
// �p�X�̖����Ƀf���~�^��ǉ�
void OSD_AddDelimiter( char * );
// �p�X�̖����̃f���~�^���폜
void OSD_DelDelimiter( char * );
// ���΃p�X��
void OSD_RelativePath( char * );
// ��΃p�X��
void OSD_AbsolutePath( char * );
// �p�X����
void OSD_AddPath( char *, const char *, const char * );

// --- �t�@�C������֐� ---
// ���W���[���p�X�擾
const char *OSD_GetModulePath();
// �t�@�C���̑��݃`�F�b�N
bool OSD_FileExist( const char * );
// �t�@�C���̓ǎ���p�`�F�b�N
bool OSD_FileReadOnly( const char * );
// �p�X����t�@�C�������擾
const char *OSD_GetFileNamePart( const char * );
// �p�X����t�H���_�����擾
const char *OSD_GetFolderNamePart( const char * );
// �p�X����g���q�����擾
const char *OSD_GetFileNameExt( const char * );
// �t�H���_�̎Q��
const char *OSD_FolderDiaog( HWINDOW, char * );
// �t�@�C���̎Q��
const char *OSD_FileDiaog( HWINDOW, FileMode, const char *, const char *, char *, char *, const char * );
// �e��t�@�C���I��
const char *OSD_FileSelect( HWINDOW, FileDlg, char *, char * );

// --- ���b�Z�[�W�\���֐� ---
// ���b�Z�[�W�\��
int OSD_Message( const char *, const char *, int );

// --- �L�[���͏����֐� ---
// �L�[���s�[�g�ݒ�
void OSD_SetKeyRepeat( int );
// OSD�L�[�R�[�h -> ���z�L�[�R�[�h�ϊ�
PCKEYsym OSD_ConvertKeyCode( int );

// --- �W���C�X�e�B�b�N�����֐� ---
// ���p�\�ȃW���C�X�e�B�b�N���擾
int OSD_GetJoyNum();
// �W���C�X�e�B�b�N���擾
const char *OSD_GetJoyName( int );
// �W���C�X�e�B�b�N�I�[�v������Ă�H
bool OSD_OpenedJoy( int );
// �W���C�X�e�B�b�N�I�[�v��
HJOYINFO OSD_OpenJoy( int );
// �W���C�X�e�B�b�N�N���[�Y
void OSD_CloseJoy( HJOYINFO );
// �W���C�X�e�B�b�N�̎��̐��擾
int OSD_GetJoyNumAxes( HJOYINFO );
// �W���C�X�e�B�b�N�̃{�^���̐��擾
int OSD_GetJoyNumButtons( HJOYINFO );
// �W���C�X�e�B�b�N�̎��̏�Ԏ擾
int OSD_GetJoyAxis( HJOYINFO, int );
// �W���C�X�e�B�b�N�̃{�^���̏�Ԏ擾
bool OSD_GetJoyButton( HJOYINFO, int );

// --- �T�E���h�֘A�֐� ---
// �I�[�f�B�I�f�o�C�X�I�[�v��
bool OSD_OpenAudio( void *, CBF_SND, int, int );
// �I�[�f�B�I�f�o�C�X�N���[�Y
void OSD_CloseAudio();
// �Đ��J�n
void OSD_StartAudio();
// �Đ���~
void OSD_StopAudio();
// �Đ���Ԏ擾
bool OSD_AudioPlaying();
// Wave�t�@�C���Ǎ���
bool OSD_LoadWAV( const char *, BYTE **, DWORD *, int * );
// Wave�t�@�C���J��
void OSD_FreeWAV( BYTE * );
// �I�[�f�B�I�����b�N����
void OSD_LockAudio();
// �I�[�f�B�I���A�����b�N����
void OSD_UnlockAudio();

// --- �^�C�}�֘A�֐� ---
// �w�莞�ԑҋ@
void OSD_Delay( DWORD );
// �v���Z�X�J�n����̌o�ߎ��Ԏ擾
DWORD OSD_GetTicks();
// �^�C�}�ǉ�
HTIMERID OSD_AddTimer( DWORD, CBF_TMR, void * );
// �^�C�}�폜
bool OSD_DelTimer( HTIMERID );

// --- �E�B���h�E�֘A�֐� ---
// �L���v�V�����ݒ�
void OSD_SetWindowCaption( HWINDOW, const char * );
// �}�E�X�J�[�\���\��/��\��
void OSD_ShowCursor( bool );
// �E�B���h�E�쐬
bool OSD_CreateWindow( HWINDOW *, int, int, bool );
// �E�B���h�E�j��
void OSD_DestroyWindow( HWINDOW );
// �E�B���h�E�̕����擾
int OSD_GetWindowWidth( HWINDOW );
// �E�B���h�E�̍������擾
int OSD_GetWindowHeight( HWINDOW );
// �E�B���h�E�N���A
void OSD_ClearWindow( HWINDOW );
// �E�B���h�E���f
void OSD_RenderWindow( HWINDOW );
// �E�B���h�E�ɓ]��(���{)
void OSD_BlitToWindow( HWINDOW, VSurface *, const int, const int );
// �E�B���h�E�ɓ]��(�g�哙)
void OSD_BlitToWindowEx( HWINDOW, VSurface *, const int, const int, const int, const bool, const bool, const int );
// �E�B���h�E�̃C���[�W�f�[�^�擾
bool OSD_GetWindowImage( HWINDOW, void **, VRect * );
// �A�C�R���ݒ�
void OSD_SetIcon( HWINDOW, int );
// OS�ˑ��̃E�B���h�E�n���h�����擾
void *OSD_GetWindowHandle( HWINDOW );

// ���ݒ�_�C�A���O�\��
int OSD_ConfigDialog( HWINDOW hwnd );
// �o�[�W�������\��
void OSD_VersionDialog( HWINDOW, int );


// --- �C�x���g�����֘A�֐� ---
// �C�x���g�擾(�C�x���g����������܂ő҂�)
bool OSD_GetEvent( Event * );
// �C�x���g���L���[�Ƀv�b�V������
bool OSD_PushEvent( EventType, ... );


// --- ���̑��̎G�֐� ---
// �F�̖��O�擾
const char *OSD_ColorName( int );
// �L�[�̖��O�擾
const char *OSD_KeyName( PCKEYsym );
// �t�H���g�t�@�C���쐬
bool OSD_CreateFont( const char *, const char *, int );


// ���b�Z�[�W�{�b�N�X�̃^�C�v
#define	OSDM_OK				0x000
#define	OSDM_OKCANCEL		0x001
#define	OSDM_YESNO			0x002
#define	OSDM_YESNOCANCEL	0x003

// ���b�Z�[�W�{�b�N�X�̃A�C�R���^�C�v
#define	OSDM_ICONERROR		0x010
#define	OSDM_ICONQUESTION	0x020
#define	OSDM_ICONWARNING	0x030
#define	OSDM_ICONINFO		0x040

// ���b�Z�[�W�{�b�N�X�̖߂�l
#define	OSDR_OK				0x00
#define	OSDR_CANCEL			0x01
#define	OSDR_YES			0x02
#define	OSDR_NO				0x03


// --- ���b�Z�[�W�z�� ---
// ��ʃ��b�Z�[�W
extern const char *MsgStr[];
#define	MSG_QUIT			MsgStr[0]	// "�I�����Ă�낵���ł���?"
#define	MSG_QUITC			MsgStr[1]	// "�I���m�F"
#define	MSG_RESTART0		MsgStr[2]	// "�ċN�����Ă�낵���ł���?"
#define	MSG_RESTART			MsgStr[3]	// "�ύX�͍ċN����ɗL���ƂȂ�܂��B\n�������ċN�����܂���?"
#define	MSG_RESTARTC		MsgStr[4]	// "�ċN���m�F"
#define	MSG_RESETI			MsgStr[5]	// "�g��ROM��}�����ă��Z�b�g���܂��B"
#define	MSG_RESETE			MsgStr[6]	// "�g��ROM��r�o���ă��Z�b�g���܂��B"
#define	MSG_RESETC			MsgStr[7]	// "���Z�b�g�m�F"


// ���j���[�p���b�Z�[�W ------
extern const char *MsgMen[];
// [�V�X�e��]
#define	MSMEN_AVI0			MsgMen[0]	// "�r�f�I�L���v�`��..."
#define	MSMEN_AVI1			MsgMen[1]	// "�r�f�I�L���v�`����~"
#define	MSMEN_REP0			MsgMen[2]	// "�L�^..."  (���v���C)
#define	MSMEN_REP1			MsgMen[3]	// "�L�^��~" (���v���C)
#define	MSMEN_REP2			MsgMen[4]	// "�Đ�..."  (���v���C)
#define	MSMEN_REP3			MsgMen[5]	// "�Đ���~" (���v���C)


// INI�t�@�C���p���b�Z�[�W ------
extern const char *MsgIni[];
// [CONFIG]
#define	MSINI_TITLE			MsgIni[0]	// "; === PC6001V �����ݒ�t�@�C�� ===\n\n"
#define	MSINI_Model			MsgIni[1]	// " �@�� 60:PC-6001 61:PC-6001A 62:PC-6001mk2 66:PC-6601"
#define	MSINI_FDD			MsgIni[2]	// " FDD�ڑ��䐔 (0-2)"
#define	MSINI_ExtRam		MsgIni[3]	// " �g��RAM�g�p"
#define	MSINI_TurboTAPE		MsgIni[4]	// " Turbo TAPE Yes:�L�� No:����"
#define	MSINI_BoostUp		MsgIni[5]	// " BoostUp Yes:�L�� No:����"
#define	MSINI_MaxBoost60	MsgIni[6]	// " BoostUp �ő�{��(N60���[�h)
#define	MSINI_MaxBoost62	MsgIni[7]	// " BoostUp �ő�{��(N60m/N66���[�h)
#define	MSINI_OverClock		MsgIni[8]	// " �I�[�o�[�N���b�N�� (1-1000)%"
#define	MSINI_CheckCRC		MsgIni[9]	// " CRC�`�F�b�N Yes:�L�� No:����"
#define	MSINI_RomPatch		MsgIni[10]	// " ROM�p�b�` Yes:���Ă� No:���ĂȂ�"
// [DISPLAY]
#define	MSINI_Mode4Color	MsgIni[11]	// " MODE4�J���[���[�h 0:���m�N�� 1:��/�� 2:��/�� 3:�s���N/�� 4:��/�s���N"
#define	MSINI_ScanLine		MsgIni[12]	// " �X�L�������C�� Yes:���� No:�Ȃ�"
#define	MSINI_ScanLineBr	MsgIni[13]	// " �X�L�������C���P�x (0-100)%"
#define	MSINI_DispNTSC		MsgIni[14]	// " 4:3�\�� Yes:�L�� No:����"
#define	MSINI_FullScreen	MsgIni[15]	// " �t���X�N���[�����[�h Yes:�L�� No:����"
#define	MSINI_DispStatus	MsgIni[16]	// " �X�e�[�^�X�o�[ Yes:�\�� No:��\��"
#define	MSINI_FrameSkip		MsgIni[17]	// " �t���[���X�L�b�v"
// [SOUND]
#define	MSINI_SampleRate	MsgIni[18]	// " �T���v�����O���[�g (44100/22050/11025)Hz"
#define	MSINI_SoundBuffer	MsgIni[19]	// " �T�E���h�o�b�t�@�T�C�Y"
#define	MSINI_MasterVolume	MsgIni[29]	// " �}�X�^�[���� (0-100)"
#define	MSINI_PsgVolume		MsgIni[21]	// " PSG���� (0-100)"
#define	MSINI_VoiceVolume	MsgIni[22]	// " ������������ (0-100)"
#define	MSINI_TapeVolume	MsgIni[23]	// " TAPE���j�^���� (0-100)"
#define	MSINI_PsgLPF		MsgIni[24]	// " PSG LPF�J�b�g�I�t���g��(0�Ŗ���)"
// [MOVIE]
#define	MSINI_AviBpp		MsgIni[25]	// " �r�f�I�L���v�`���F�[�x (16,24,32)"
// [FILES]
#define	MSINI_ExtRom		MsgIni[26]	// " �g��ROM�t�@�C����"
#define	MSINI_tape			MsgIni[27]	// " TAPE(LODE)�t�@�C����(�N�����Ɏ����}�E���g)"
#define	MSINI_save			MsgIni[28]	// " TAPE(SAVE)�t�@�C����(SAVE���Ɏ����}�E���g)"
#define	MSINI_disk1			MsgIni[29]	// " DISK1�t�@�C����(�N�����Ɏ����}�E���g)"
#define	MSINI_disk2			MsgIni[30]	// " DISK2�t�@�C����(�N�����Ɏ����}�E���g)"
#define	MSINI_printer		MsgIni[31]	// " �v�����^�o�̓t�@�C����"
#define	MSINI_fontz			MsgIni[32]	// " �S�p�t�H���g�t�@�C����"
#define	MSINI_fonth			MsgIni[33]	// " ���p�t�H���g�t�@�C����"
// [PATH]
#define	MSINI_RomPath		MsgIni[34]	// " ROM�C���[�W�i�[�p�X"
#define	MSINI_TapePath		MsgIni[35]	// " TAPE�C���[�W�i�[�p�X"
#define	MSINI_DiskPath		MsgIni[36]	// " DISK�C���[�W�i�[�p�X"
#define	MSINI_ExtRomPath	MsgIni[37]	// " �g��ROM�C���[�W�i�[�p�X"
#define	MSINI_ImgPath		MsgIni[38]	// " �X�i�b�v�V���b�g�i�[�p�X"
#define	MSINI_WavePath		MsgIni[39]	// " WAVE�t�@�C���i�[�p�X"
#define	MSINI_FontPath		MsgIni[40]	// " FONT�i�[�p�X"
#define	MSINI_DokoSavePath	MsgIni[41]	// " �ǂ��ł�SAVE�i�[�p�X"
// [CHECK]
#define	MSINI_CkQuit		MsgIni[42]	// " �I�����m�F Yes:���� No:���Ȃ�"
#define	MSINI_SaveQuit		MsgIni[43]	// " �I����INI�t�@�C����ۑ� Yes:���� No:���Ȃ�"
// [KEY]
#define	MSINI_KeyRepeat		MsgIni[44]	// " �L�[���s�[�g�̊Ԋu(�P��:ms 0�Ŗ���)"
// [OPTION]
#define	MSINI_UseSoldier	MsgIni[45]	// " ��m�̃J�[�g���b�W Yes:�L�� No:����"


// �ǂ��ł�SAVE�p���b�Z�[�W ------
extern const char *MsgDok[];
#define	MSDOKO_TITLE		MsgDok[0]	// "; === PC6001V �ǂ��ł�SAVE�t�@�C�� ===\n\n"


// Error�p���b�Z�[�W ------
extern const char *MsgErr[];
#define	MSERR_ERROR				MsgErr[0]	// "Error"
#define	MSERR_NoError			MsgErr[1]	// "�G���[�͂���܂���"
#define	MSERR_Unknown			MsgErr[2]	// "�����s���̃G���[���������܂���"
#define	MSERR_MemAllocFailed	MsgErr[3]	// "�������̊m�ۂɎ��s���܂���"
#define	MSERR_RomChange			MsgErr[4]	// "�w�肳�ꂽ�@���ROM�C���[�W��������Ȃ����ߋ@���ύX���܂���\n�ݒ���m�F���Ă�������"
#define	MSERR_NoRom				MsgErr[5]	// "ROM�C���[�W��������܂���\n�ݒ�ƃt�@�C�������m�F���Ă�������"
#define	MSERR_RomSizeNG			MsgErr[6]	// "ROM�C���[�W�̃T�C�Y���s���ł�"
#define	MSERR_RomCrcNG			MsgErr[7]	// "ROM�C���[�W��CRC���s���ł�"
#define	MSERR_LibInitFailed		MsgErr[8]	// "���C�u�����̏������Ɏ��s���܂���"
#define	MSERR_InitFailed		MsgErr[9]	// "�������Ɏ��s���܂���\n�ݒ���m�F���Ă�������"
#define	MSERR_FontLoadFailed	MsgErr[10]	// "�t�H���g�̓Ǎ��݂Ɏ��s���܂���"
#define	MSERR_FontCreateFailed	MsgErr[11]	// "�t�H���g�t�@�C���̍쐬�Ɏ��s���܂���"
#define	MSERR_IniDefault		MsgErr[12]	// "INI�t�@�C���̓Ǎ��݂Ɏ��s���܂���\n�f�t�H���g�ݒ�ŋN�����܂�"
#define	MSERR_IniReadFailed		MsgErr[13]	// "INI�t�@�C���̓Ǎ��݂Ɏ��s���܂���"
#define	MSERR_IniWriteFailed	MsgErr[14]	// "INI�t�@�C���̕ۑ��Ɏ��s���܂���"
#define	MSERR_TapeMountFailed	MsgErr[15]	// "TAPE�C���[�W�̃}�E���g�Ɏ��s���܂���"
#define	MSERR_DiskMountFailed	MsgErr[16]	// "DISK�C���[�W�̃}�E���g�Ɏ��s���܂���"
#define	MSERR_ExtRomMountFailed	MsgErr[17]	// "�g��ROM�C���[�W�̃}�E���g�Ɏ��s���܂���"
#define	MSERR_DokoReadFailed	MsgErr[18]	// "�ǂ��ł�LOAD�Ɏ��s���܂���"
#define	MSERR_DokoWriteFailed	MsgErr[19]	// "�ǂ��ł�SAVE�Ɏ��s���܂���"
#define	MSERR_DokoDiffVersion	MsgErr[20]	// "�ǂ��ł�LOAD�Ɏ��s���܂���\n�ۑ�����PC6001V�̃o�[�W�������قȂ�܂�"
#define	MSERR_ReplayPlayError	MsgErr[21]	// "���v���C�Đ��Ɏ��s���܂���"
#define	MSERR_ReplayRecError	MsgErr[22]	// "���v���C�L�^�Ɏ��s���܂���"
#define	MSERR_NoReplayData		MsgErr[23]	// "���v���C�f�[�^������܂���"



#endif	// OSD_H_INCLUDED
