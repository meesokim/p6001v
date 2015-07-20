// OS�ˑ��̔ėp���[�`��(���UI�p)

#include <windows.h>
#include <mmsystem.h>
#include <shlobj.h>
#include <string.h>

#include <shlwapi.h>	// ie�ˑ�

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_endian.h>

#include "../common.h"
#include "../log.h"
#include "../osd.h"
#include "../vsurface.h"

#if SDL_VERSION_ATLEAST(2,0,0)
#include "icons2.h"
#else
#include "icons.h"
#endif


///////////////////////////////////////////////////////////
// SDL�֘A
///////////////////////////////////////////////////////////
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define	AUDIOFORMAT	AUDIO_S16MSB	// 16�r�b�g��������
#else
#define	AUDIOFORMAT	AUDIO_S16		// 16�r�b�g��������
#endif

// �X�N���[���T�[�t�F�X/�E�B���h�E�쐬�p�I�v�V����
#if SDL_VERSION_ATLEAST(2,0,0)
#define SDLOP_SCREEN	SDL_RENDERER_ACCELERATED
#else
#define SDLOP_SCREEN	(SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_HWACCEL)
#endif

// ���[�U�[��`�C�x���g
#define UEV_FPSUPDATE	(SDL_USEREVENT+0)
#define UEV_RESTART		(SDL_USEREVENT+1)
#define UEV_DOKOLOAD	(SDL_USEREVENT+2)
#define UEV_REPLAY		(SDL_USEREVENT+3)
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define UEV_DEBUGMODEBP	(SDL_USEREVENT+4)
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define USESDLTIMER					// SDL�̃^�C�}�g�p




#ifndef USESDLTIMER
#define TIMERRES	10				// �^�C�}�𑜓x
// �^�C�}���
struct TINFO {
	UINT id;			// �^�C�}ID
	DWORD interval;		// �����݊Ԋu(ms)
	CBF_TMR callback;	// �R�[���o�b�N�֐�
	void *param;		// �R�[���o�b�N�֐��ɓn������
	TINFO *next;		// ���̃^�C�}���ւ̃|�C���^
	
	TINFO() : id(0), callback(NULL), param(NULL), next(NULL) {}
};
#endif


///////////////////////////////////////////////////////////
// ���[�J���֐���`
///////////////////////////////////////////////////////////
// OSD_FolderDiaog()�Ŏg�p����R�[���o�b�N�v���V�[�W��
static int CALLBACK OsdBrowseCallbackProc( HWND, UINT, LPARAM, LPARAM );
#ifndef USESDLTIMER
// OSD_AddTimer()�Ŏg�p����R�[���o�b�N�v���V�[�W��
static void CALLBACK OsdTimerCallbackProc( UINT, UINT, DWORD_PTR, DWORD_PTR, DWORD_PTR );
#endif

///////////////////////////////////////////////////////////
// �X�^�e�B�b�N�ϐ�
///////////////////////////////////////////////////////////
static HANDLE hMutex;						// ���d�N���`�F�b�N�p�̃~���[�e�b�N�X
static BROWSEINFO OBI;						// OSD_FolderDiaog()�Ŏg�p��������i�[����\����

#if SDL_VERSION_ATLEAST(2,0,0)
static PCKEYsym VKTable[SDL_NUM_SCANCODES];	// SDL�X�L�����R�[�h  -> ���z�L�[�R�[�h �ϊ��e�[�u��
#else
static PCKEYsym VKTable[SDLK_LAST];			// SDL�L�[�R�[�h  -> ���z�L�[�R�[�h �ϊ��e�[�u��
#endif



#ifndef USESDLTIMER
static TINFO *tif;
#endif
///////////////////////////////////////////////////////////
// �萔
///////////////////////////////////////////////////////////
#if SDL_VERSION_ATLEAST(2,0,0)
static const struct {	// SDL�X�L�����R�[�h -> ���z�L�[�R�[�h��`
	SDL_Scancode InKey;	// SDL�̃X�L�����R�[�h
	PCKEYsym VKey;		// ���z�L�[�R�[�h
} VKeyDef[] = {
	{ SDL_SCANCODE_UNKNOWN,			KVC_UNKNOWN },
	
	{ SDL_SCANCODE_1,				KVC_1 },			// 1	!
	{ SDL_SCANCODE_2,				KVC_2 },			// 2	"
	{ SDL_SCANCODE_3,				KVC_3 },			// 3	#
	{ SDL_SCANCODE_4,				KVC_4 },			// 4	$
	{ SDL_SCANCODE_5,				KVC_5 },			// 5	%
	{ SDL_SCANCODE_6,				KVC_6 },			// 6	&
	{ SDL_SCANCODE_7,				KVC_7 },			// 7	'
	{ SDL_SCANCODE_8,				KVC_8 },			// 8	(
	{ SDL_SCANCODE_9,				KVC_9 },			// 9	)
	{ SDL_SCANCODE_0,				KVC_0 },			// 0
	
	{ SDL_SCANCODE_A,				KVC_A },			// a	A
	{ SDL_SCANCODE_B,				KVC_B },			// b	B
	{ SDL_SCANCODE_C,				KVC_C },			// c	C
	{ SDL_SCANCODE_D,				KVC_D },			// d	D
	{ SDL_SCANCODE_E,				KVC_E },			// e	E
	{ SDL_SCANCODE_F,				KVC_F },			// f	F
	{ SDL_SCANCODE_G,				KVC_G },			// g	G
	{ SDL_SCANCODE_H,				KVC_H },			// h	H
	{ SDL_SCANCODE_I,				KVC_I },			// i	I
	{ SDL_SCANCODE_J,				KVC_J },			// j	J
	{ SDL_SCANCODE_K,				KVC_K },			// k	K
	{ SDL_SCANCODE_L,				KVC_L },			// l	L
	{ SDL_SCANCODE_M,				KVC_M },			// m	M
	{ SDL_SCANCODE_N,				KVC_N },			// n	N
	{ SDL_SCANCODE_O,				KVC_O },			// o	O
	{ SDL_SCANCODE_P,				KVC_P },			// p	P
	{ SDL_SCANCODE_Q,				KVC_Q },			// q	Q
	{ SDL_SCANCODE_R,				KVC_R },			// r	R
	{ SDL_SCANCODE_S,				KVC_S },			// s	S
	{ SDL_SCANCODE_T,				KVC_T },			// t	T
	{ SDL_SCANCODE_U,				KVC_U },			// u	U
	{ SDL_SCANCODE_V,				KVC_V },			// v	V
	{ SDL_SCANCODE_W,				KVC_W },			// w	W
	{ SDL_SCANCODE_X,				KVC_X },			// x	X
	{ SDL_SCANCODE_Y,				KVC_Y },			// y	Y
	{ SDL_SCANCODE_Z,				KVC_Z },			// z	Z
	
	{ SDL_SCANCODE_F1,				KVC_F1 },			// F1
	{ SDL_SCANCODE_F2,				KVC_F2 },			// F2
	{ SDL_SCANCODE_F3,				KVC_F3 },			// F3
	{ SDL_SCANCODE_F4,				KVC_F4 },			// F4
	{ SDL_SCANCODE_F5,				KVC_F5 },			// F5
	{ SDL_SCANCODE_F6,				KVC_F6 },			// F6
	{ SDL_SCANCODE_F7,				KVC_F7 },			// F7
	{ SDL_SCANCODE_F8,				KVC_F8 },			// F8
	{ SDL_SCANCODE_F9,				KVC_F9 },			// F9
	{ SDL_SCANCODE_F10,				KVC_F10 },			// F10
	{ SDL_SCANCODE_F11,				KVC_F11 },			// F11
	{ SDL_SCANCODE_F12,				KVC_F12 },			// F12
	
	{ SDL_SCANCODE_MINUS,			KVC_MINUS },		// -	=
	{ SDL_SCANCODE_APOSTROPHE,		KVC_CARET },		// ^	~
	{ SDL_SCANCODE_BACKSPACE,		KVC_BACKSPACE },	// BackSpace
	{ SDL_SCANCODE_GRAVE,			KVC_AT },			// @	`
	{ SDL_SCANCODE_LEFTBRACKET,		KVC_LBRACKET },		// [	{
	{ SDL_SCANCODE_EQUALS,			KVC_SEMICOLON },	// ;	+
	{ SDL_SCANCODE_SEMICOLON,		KVC_COLON },		// :	*
	{ SDL_SCANCODE_COMMA,			KVC_COMMA },		// ,	<
	{ SDL_SCANCODE_PERIOD,			KVC_PERIOD },		// .	>
	{ SDL_SCANCODE_SLASH,			KVC_SLASH },		// /	?
	{ SDL_SCANCODE_SPACE,			KVC_SPACE },		// Space
	
	{ SDL_SCANCODE_ESCAPE,			KVC_ESC },			// ESC
	{ SDL_SCANCODE_TAB,				KVC_TAB },			// Tab
	{ SDL_SCANCODE_CAPSLOCK,		KVC_CAPSLOCK },		// CapsLock
	{ SDL_SCANCODE_RETURN,			KVC_ENTER },		// Enter
	{ SDL_SCANCODE_LCTRL,			KVC_LCTRL },		// L-Ctrl
	{ SDL_SCANCODE_RCTRL,			KVC_RCTRL },		// R-Ctrl
	{ SDL_SCANCODE_LSHIFT,			KVC_LSHIFT },		// L-Shift
	{ SDL_SCANCODE_RSHIFT,			KVC_RSHIFT },		// R-Shift
	{ SDL_SCANCODE_LALT,			KVC_LALT },			// L-Alt
	{ SDL_SCANCODE_RALT,			KVC_RALT },			// R-Alt
	{ SDL_SCANCODE_PRINTSCREEN,		KVC_PRINT },		// PrintScreen
	{ SDL_SCANCODE_PRINTSCREEN,		KVC_SCROLLLOCK },	// ScrollLock
	{ SDL_SCANCODE_PAUSE,			KVC_PAUSE },		// Pause
	{ SDL_SCANCODE_INSERT,			KVC_INSERT },		// Insert
	{ SDL_SCANCODE_DELETE,			KVC_DELETE },		// Delete
	{ SDL_SCANCODE_END,				KVC_END },			// End
	{ SDL_SCANCODE_HOME,			KVC_HOME },			// Home
	{ SDL_SCANCODE_PAGEUP,			KVC_PAGEUP },		// PageUp
	{ SDL_SCANCODE_PAGEDOWN,		KVC_PAGEDOWN },		// PageDown
	
	{ SDL_SCANCODE_UP,				KVC_UP },			// ��
	{ SDL_SCANCODE_DOWN,			KVC_DOWN },			// ��
	{ SDL_SCANCODE_LEFT,			KVC_LEFT },			// ��
	{ SDL_SCANCODE_RIGHT,			KVC_RIGHT },		// ��
	
	{ SDL_SCANCODE_KP_0,			KVC_P0 },			// [0]
	{ SDL_SCANCODE_KP_1,			KVC_P1 },			// [1]
	{ SDL_SCANCODE_KP_2,			KVC_P2 },			// [2]
	{ SDL_SCANCODE_KP_3,			KVC_P3 },			// [3]
	{ SDL_SCANCODE_KP_4,			KVC_P4 },			// [4]
	{ SDL_SCANCODE_KP_5,			KVC_P5 },			// [5]
	{ SDL_SCANCODE_KP_6,			KVC_P6 },			// [6]
	{ SDL_SCANCODE_KP_7,			KVC_P7 },			// [7]
	{ SDL_SCANCODE_KP_8,			KVC_P8 },			// [8]
	{ SDL_SCANCODE_KP_9,			KVC_P9 },			// [9]
	{ SDL_SCANCODE_NUMLOCKCLEAR,	KVC_NUMLOCK },		// NumLock
	{ SDL_SCANCODE_KP_PLUS,			KVC_P_PLUS },		// [+]
	{ SDL_SCANCODE_KP_MINUS,		KVC_P_MINUS },		// [-]
	{ SDL_SCANCODE_KP_MULTIPLY,		KVC_P_MULTIPLY },	// [*]
	{ SDL_SCANCODE_KP_DIVIDE,		KVC_P_DIVIDE },		// [/]
	{ SDL_SCANCODE_KP_PERIOD,		KVC_P_PERIOD },		// [.]
	{ SDL_SCANCODE_KP_ENTER,		KVC_P_ENTER },		// [Enter]
	
	// ���{��L�[�{�[�h�̂�
	{ SDL_SCANCODE_BACKSLASH,		KVC_YEN },			// ��	|
	{ SDL_SCANCODE_RIGHTBRACKET,	KVC_RBRACKET },		// ]	}
	{ SDL_SCANCODE_NONUSBACKSLASH,	KVC_UNDERSCORE },	// ��	_
//	{ SDL_SCANCODE_LANG5,			KVC_HANZEN },		// ���p/�S�p
//	{ 				,				KVC_MUHENKAN },		// ���ϊ�
//	{ 				,				KVC_HENKAN },		// �ϊ�
//	{ 				,				KVC_HIRAGANA },		// �Ђ炪��
	
	// �ǉ��L�[
	{ SDL_SCANCODE_LGUI,			KVX_LMETA },		// L-Meta
	{ SDL_SCANCODE_RGUI,			KVX_RMETA },		// R-Meta
	{ SDL_SCANCODE_MENU,			KVX_MENU }			// Menu
};
#else
static const struct {	// SDL�L�[�R�[�h -> ���z�L�[�R�[�h��`
	int InKey;			// SDL�̃L�[�R�[�h
	PCKEYsym VKey;		// ���z�L�[�R�[�h
} VKeyDef[] = {
	{ SDLK_UNKNOWN,			KVC_UNKNOWN },
	
	{ SDLK_1,				KVC_1 },			// 1	!
	{ SDLK_2,				KVC_2 },			// 2	"
	{ SDLK_3,				KVC_3 },			// 3	#
	{ SDLK_4,				KVC_4 },			// 4	$
	{ SDLK_5,				KVC_5 },			// 5	%
	{ SDLK_6,				KVC_6 },			// 6	&
	{ SDLK_7,				KVC_7 },			// 7	'
	{ SDLK_8,				KVC_8 },			// 8	(
	{ SDLK_9,				KVC_9 },			// 9	)
	{ SDLK_0,				KVC_0 },			// 0
	
	{ SDLK_a,				KVC_A },			// a	A
	{ SDLK_b,				KVC_B },			// b	B
	{ SDLK_c,				KVC_C },			// c	C
	{ SDLK_d,				KVC_D },			// d	D
	{ SDLK_e,				KVC_E },			// e	E
	{ SDLK_f,				KVC_F },			// f	F
	{ SDLK_g,				KVC_G },			// g	G
	{ SDLK_h,				KVC_H },			// h	H
	{ SDLK_i,				KVC_I },			// i	I
	{ SDLK_j,				KVC_J },			// j	J
	{ SDLK_k,				KVC_K },			// k	K
	{ SDLK_l,				KVC_L },			// l	L
	{ SDLK_m,				KVC_M },			// m	M
	{ SDLK_n,				KVC_N },			// n	N
	{ SDLK_o,				KVC_O },			// o	O
	{ SDLK_p,				KVC_P },			// p	P
	{ SDLK_q,				KVC_Q },			// q	Q
	{ SDLK_r,				KVC_R },			// r	R
	{ SDLK_s,				KVC_S },			// s	S
	{ SDLK_t,				KVC_T },			// t	T
	{ SDLK_u,				KVC_U },			// u	U
	{ SDLK_v,				KVC_V },			// v	V
	{ SDLK_w,				KVC_W },			// w	W
	{ SDLK_x,				KVC_X },			// x	X
	{ SDLK_y,				KVC_Y },			// y	Y
	{ SDLK_z,				KVC_Z },			// z	Z
	
	{ SDLK_F1,				KVC_F1 },			// F1
	{ SDLK_F2,				KVC_F2 },			// F2
	{ SDLK_F3,				KVC_F3 },			// F3
	{ SDLK_F4,				KVC_F4 },			// F4
	{ SDLK_F5,				KVC_F5 },			// F5
	{ SDLK_F6,				KVC_F6 },			// F6
	{ SDLK_F7,				KVC_F7 },			// F7
	{ SDLK_F8,				KVC_F8 },			// F8
	{ SDLK_F9,				KVC_F9 },			// F9
	{ SDLK_F10,				KVC_F10 },			// F10
	{ SDLK_F11,				KVC_F11 },			// F11
	{ SDLK_F12,				KVC_F12 },			// F12
	
	{ SDLK_MINUS,			KVC_MINUS },		// -	=
	{ SDLK_CARET,			KVC_CARET },		// ^	~
	{ SDLK_BACKSPACE,		KVC_BACKSPACE },	// BackSpace
	{ SDLK_AT,				KVC_AT },			// @	`
	{ SDLK_LEFTBRACKET,		KVC_LBRACKET },		// [	{
	{ SDLK_SEMICOLON,		KVC_SEMICOLON },	// ;	+
	{ SDLK_COLON,			KVC_COLON },		// :	*
	{ SDLK_COMMA,			KVC_COMMA },		// ,	<
	{ SDLK_PERIOD,			KVC_PERIOD },		// .	>
	{ SDLK_SLASH,			KVC_SLASH },		// /	?
	{ SDLK_SPACE,			KVC_SPACE },		// Space
	
	{ SDLK_ESCAPE,			KVC_ESC },			// ESC
	{ SDLK_BACKQUOTE,		KVC_HANZEN },		// ���p/�S�p
	{ SDLK_TAB,				KVC_TAB },			// Tab
	{ SDLK_CAPSLOCK,		KVC_CAPSLOCK },		// CapsLock
	{ SDLK_RETURN,			KVC_ENTER },		// Enter
	{ SDLK_LCTRL,			KVC_LCTRL },		// L-Ctrl
	{ SDLK_RCTRL,			KVC_RCTRL },		// R-Ctrl
	{ SDLK_LSHIFT,			KVC_LSHIFT },		// L-Shift
	{ SDLK_RSHIFT,			KVC_RSHIFT },		// R-Shift
	{ SDLK_LALT,			KVC_LALT },			// L-Alt
	{ SDLK_RALT,			KVC_RALT },			// R-Alt
	{ SDLK_PRINT,			KVC_PRINT },		// PrintScreen
	{ SDLK_SCROLLOCK,		KVC_SCROLLLOCK },	// ScrollLock
	{ SDLK_PAUSE,			KVC_PAUSE },		// Pause
	{ SDLK_INSERT,			KVC_INSERT },		// Insert
	{ SDLK_DELETE,			KVC_DELETE },		// Delete
	{ SDLK_END,				KVC_END },			// End
	{ SDLK_HOME,			KVC_HOME },			// Home
	{ SDLK_PAGEUP,			KVC_PAGEUP },		// PageUp
	{ SDLK_PAGEDOWN,		KVC_PAGEDOWN },		// PageDown
	
	{ SDLK_UP,				KVC_UP },			// ��
	{ SDLK_DOWN,			KVC_DOWN },			// ��
	{ SDLK_LEFT,			KVC_LEFT },			// ��
	{ SDLK_RIGHT,			KVC_RIGHT },		// ��
	
	{ SDLK_KP0,				KVC_P0 },			// [0]
	{ SDLK_KP1,				KVC_P1 },			// [1]
	{ SDLK_KP2,				KVC_P2 },			// [2]
	{ SDLK_KP3,				KVC_P3 },			// [3]
	{ SDLK_KP4,				KVC_P4 },			// [4]
	{ SDLK_KP5,				KVC_P5 },			// [5]
	{ SDLK_KP6,				KVC_P6 },			// [6]
	{ SDLK_KP7,				KVC_P7 },			// [7]
	{ SDLK_KP8,				KVC_P8 },			// [8]
	{ SDLK_KP9,				KVC_P9 },			// [9]
	{ SDLK_NUMLOCK,			KVC_NUMLOCK },		// NumLock
	{ SDLK_KP_PLUS,			KVC_P_PLUS },		// [+]
	{ SDLK_KP_MINUS,		KVC_P_MINUS },		// [-]
	{ SDLK_KP_MULTIPLY,		KVC_P_MULTIPLY },	// [*]
	{ SDLK_KP_DIVIDE,		KVC_P_DIVIDE },		// [/]
	{ SDLK_KP_PERIOD,		KVC_P_PERIOD },		// [.]
	{ SDLK_KP_ENTER,		KVC_P_ENTER },		// [Enter]
	
	// ���{��L�[�{�[�h�̂�
	{ SDLK_BACKSLASH,		KVC_YEN },			// ��	|
	{ SDLK_RIGHTBRACKET,	KVC_RBRACKET },		// ]	}
	{ SDLK_LESS,			KVC_UNDERSCORE },	// ��	_
//	{ 				,		KVC_MUHENKAN },		// ���ϊ�
//	{ 				,		KVC_HENKAN },		// �ϊ�
//	{ 				,		KVC_HIRAGANA },		// �Ђ炪��
	
	// �ǉ��L�[
	{ SDLK_LMETA,			KVX_LMETA },		// L-Meta
	{ SDLK_RMETA,			KVX_RMETA },		// R-Meta
	{ SDLK_MENU,			KVX_MENU }			// Menu
};
#endif


///////////////////////////////////////////////////////////
// ���b�Z�[�W�z��
///////////////////////////////////////////////////////////
// ��ʃ��b�Z�[�W
const char *MsgStr[] = {
	"�I�����Ă�낵���ł���?",
	"�I���m�F",
	"�ċN�����Ă�낵���ł���?",
	"�ύX�͍ċN����ɗL���ƂȂ�܂��B\n�������ċN�����܂���?",
	"�ċN���m�F",
	"�g��ROM��}�����ă��Z�b�g���܂��B",
	"�g��ROM��r�o���ă��Z�b�g���܂��B",
	"���Z�b�g�m�F"
};

// ���j���[�p���b�Z�[�W ------
const char *MsgMen[] = {
	"�r�f�I�L���v�`��...",
	"�r�f�I�L���v�`����~",
	"�L�^...",	// (���v���C)
	"�L�^��~",	// (���v���C)
	"�Đ�...",	// (���v���C)
	"�Đ���~"	// (���v���C)
};

// INI�t�@�C���p���b�Z�[�W ------
const char *MsgIni[] = {
	// [CONFIG]
	"; === PC6001V �����ݒ�t�@�C�� ===\n\n",
//	"�@�� 60:PC-6001 61:PC-6001A 62:PC-6001mk2 66:PC-6601 64:PC-6001mk2SR 68:PC-6601SR",
	"�@�� 60:PC-6001 61:PC-6001A 62:PC-6001mk2 66:PC-6601",
	"FDD�ڑ��䐔 (0-2)",
	"�g��RAM�g�p",
	"Turbo TAPE Yes:�L�� No:����",
	"BoostUp Yes:�L�� No:����",
	"BoostUp �ő�{��(N60���[�h)",
	"BoostUp �ő�{��(N60m/N66���[�h)",
	"�I�[�o�[�N���b�N�� (1-1000)%",
	"CRC�`�F�b�N Yes:�L�� No:����",
	"ROM�p�b�` Yes:���Ă� No:���ĂȂ�",
	// [DISPLAY]
	"MODE4�J���[���[�h 0:���m�N�� 1:��/�� 2:��/�� 3:�s���N/�� 4:��/�s���N",
	"�X�L�������C�� Yes:���� No:�Ȃ�",
	"�X�L�������C���P�x (0-100)%",
	"4:3�\�� Yes:�L�� No:����",
	"�t���X�N���[�����[�h Yes:�L�� No:����",
	"�X�e�[�^�X�o�[ Yes:�\�� No:��\��",
	"�t���[���X�L�b�v",
	// [SOUND]
	"�T���v�����O���[�g (44100/22050/11025)Hz",
	"�T�E���h�o�b�t�@�T�C�Y",
	"�}�X�^�[���� (0-100)",
	"PSG���� (0-100)",
	"������������ (0-100)",
	"TAPE���j�^���� (0-100)",
	"PSG LPF�J�b�g�I�t���g��(0�Ŗ���)",
	// [MOVIE]
	"�r�f�I�L���v�`���F�[�x (16,24,32)",
	// [FILES]
	"�g��ROM�t�@�C����",
	"TAPE(LODE)�t�@�C����(�N�����Ɏ����}�E���g)",
	"TAPE(SAVE)�t�@�C����(SAVE���Ɏ����}�E���g)",
	"DISK1�t�@�C����(�N�����Ɏ����}�E���g)",
	"DISK2�t�@�C����(�N�����Ɏ����}�E���g)",
	"�v�����^�o�̓t�@�C����",
	"�S�p�t�H���g�t�@�C����",
	"���p�t�H���g�t�@�C����",
	// [PATH]
	"ROM�C���[�W�i�[�p�X",
	"TAPE�C���[�W�i�[�p�X",
	"DISK�C���[�W�i�[�p�X",
	"�g��ROM�C���[�W�i�[�p�X",
	"�X�i�b�v�V���b�g�i�[�p�X",
	"WAVE�t�@�C���i�[�p�X",
	"FONT�i�[�p�X",
	"�ǂ��ł�SAVE�i�[�p�X",
	// [CHECK]
	"�I�����m�F Yes:���� No:���Ȃ�",
	"�I����INI�t�@�C����ۑ� Yes:���� No:���Ȃ�",
	// [KEY]
	"�L�[���s�[�g�̊Ԋu(�P��:ms 0�Ŗ���)",
	// [OPTION]
	"��m�̃J�[�g���b�W Yes:�L�� No:����"
	
};

// �ǂ��ł�SAVE�p���b�Z�[�W ------
const char *MsgDok[] = {
	"; === PC6001V �ǂ��ł�SAVE�t�@�C�� ===\n\n"
};

// Error�p���b�Z�[�W ------
const char *MsgErr[] = {
	"Error",
	"�G���[�͂���܂���",
	"�����s���̃G���[���������܂���",
	"�������̊m�ۂɎ��s���܂���",
	"�w�肳�ꂽ�@���ROM�C���[�W��������Ȃ����ߋ@���ύX���܂���\n�ݒ���m�F���Ă�������",
	"ROM�C���[�W��������܂���\n�ݒ�ƃt�@�C�������m�F���Ă�������",
	"ROM�C���[�W�̃T�C�Y���s���ł�",
	"ROM�C���[�W��CRC���s���ł�",
	"���C�u�����̏������Ɏ��s���܂���",
	"�������Ɏ��s���܂���\n�ݒ���m�F���Ă�������",
	"�t�H���g�̓Ǎ��݂Ɏ��s���܂���",
	"�t�H���g�t�@�C���̍쐬�Ɏ��s���܂���",
	"INI�t�@�C���̓Ǎ��݂Ɏ��s���܂���\n�f�t�H���g�ݒ�ŋN�����܂�",
	"INI�t�@�C���̓Ǎ��݂Ɏ��s���܂���",
	"INI�t�@�C���̕ۑ��Ɏ��s���܂���",
	"TAPE�C���[�W�̃}�E���g�Ɏ��s���܂���",
	"DISK�C���[�W�̃}�E���g�Ɏ��s���܂���",
	"�g��ROM�C���[�W�̃}�E���g�Ɏ��s���܂���",
	"�ǂ��ł�LOAD�Ɏ��s���܂���",
	"�ǂ��ł�SAVE�Ɏ��s���܂���",
	"�ǂ��ł�LOAD�Ɏ��s���܂���\n�ۑ�����PC6001V�̃o�[�W�������قȂ�܂�",
	"���v���C�Đ��Ɏ��s���܂���",
	"���v���C�L�^�Ɏ��s���܂���",
	"���v���C�f�[�^������܂���"
};


////////////////////////////////////////////////////////////////
// �������ɂ�镶�����r
//
// ����:	s1			��r���镶����ւ̃|�C���^1
//			s2			��r���镶����ւ̃|�C���^2
// �Ԓl:	int			0:������ ����:s1>s2 ����:s1<s2
////////////////////////////////////////////////////////////////
int stricmp ( const char *s1, const char *s2 )
{
	const char *ps1 = s1, *ps2 = s2;
	
	if( !s1 ) return -1;
	if( !s2 ) return  1;
	
	// �擪�̕���
	char c1 = *ps1++;
	char c2 = *ps2++;
	
	// �ǂ��炩�̕�����I�[�܂ŌJ��Ԃ�
	while( c1 && c2 ){
		// �������ɕϊ�
		if( c1 >= 'A' && c1 <= 'Z' ) c1 += 'a' - 'A';
		if( c2 >= 'A' && c2 <= 'Z' ) c2 += 'a' - 'A';
		
		// �������Ȃ��������r�I��
		if( c1 != c2 ) break;
		
		// ���̕����擾
		c1 = *ps1++;
		c2 = *ps2++;
	}
	
	return (int)c1 - (int)c2;
}



////////////////////////////////////////////////////////////////
// ������
//
// ����:	�Ȃ�
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool OSD_Init( void )
{
	if( SDL_Init( SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK ) )
		return false;
	
	// SDL�L�[�R�[�h  -> ���z�L�[�R�[�h �ϊ��e�[�u��������
	ZeroMemory( &VKTable, sizeof(VKTable) );
	for( int i=0; i < COUNTOF(VKeyDef); i++ ) VKTable[VKeyDef[i].InKey] = VKeyDef[i].VKey;
	
	#if SDL_VERSION_ATLEAST(2,0,0)
	// SDL2.0�̓L�[�V���{����Unicode�ɑΉ����Ă���炵��
	#else
	// Unicode�L�� �I�[�o�[�w�b�h�͋C�ɂ��Ȃ����Ƃɂ���
	SDL_EnableUNICODE( 1 );
	#endif
	
	#ifndef USESDLTIMER
	tif = NULL;
	#endif
	
	return true;
}


////////////////////////////////////////////////////////////////
// �I������
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_Quit( void )
{
	PRINTD( OSD_LOG, "[OSD][OSD_Quit]\n" );
	
	#ifndef USESDLTIMER
	// �^�C�}�S�폜
	TINFO *ti    = tif;
	TINFO *tip   = NULL;
	while( ti ){
		timeKillEvent( ti->id );
		timeEndPeriod( TIMERRES );
		tip = ti;
		ti = ti->next;
		delete tip;
	}
	tif = NULL;
	#endif
	
	
	// Mutex ���J������
	ReleaseMutex( hMutex );
	
	SDL_Quit();
}


////////////////////////////////////////////////////////////////
// ���d�N���`�F�b�N
//
// ����:	�Ȃ�
// �Ԓl:	bool		true:�N���ς� false:���N��
////////////////////////////////////////////////////////////////
bool OSD_IsWorking( void )
{
	// ���C���E�B���h�E�N���X�̖��O
	#define	MWCLASS	"p6vmainw"
	
	PRINTD( OSD_LOG, "[OSD][OSD_IsWorking]\n" );
	
	HANDLE hCheckMutex = OpenMutex( MUTEX_ALL_ACCESS, false, MWCLASS );
	if( hCheckMutex ){
		CloseHandle( hCheckMutex );
		return true;
	}
	hMutex = CreateMutex( NULL, 0, MWCLASS );
	return false;
}



////////////////////////////////////////////////////////////////
// �p�X�̖����Ƀf���~�^��ǉ�
//
// ����:	path		�p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_AddDelimiter( char *path )
{
	PathAddBackslash( path );
}


////////////////////////////////////////////////////////////////
// �p�X�̖����̃f���~�^���폜
//
// ����:	path		�p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_DelDelimiter( char *path )
{
	PathRemoveBackslash( path );
}


////////////////////////////////////////////////////////////////
// ���΃p�X��
//
// ����:	path		�p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_RelativePath( char *path )
{
	char tpath[PATH_MAX+1];
	
	if( PathIsRelative( path ) || !strlen( path ) ) return;
	
	strcpy( tpath, path );
	DWORD attr = GetFileAttributes( path ) & FILE_ATTRIBUTE_DIRECTORY;
	PathRelativePathTo( tpath, OSD_GetModulePath(), attr, path, attr );
	
	if( !strncmp( tpath, "..\\", 3 ) ){
		OSD_AbsolutePath( tpath );
		strcpy( path, tpath );
	}else if( !strncmp( tpath, ".\\", 2 ) )
		strcpy( path, &tpath[2] );
}


////////////////////////////////////////////////////////////////
// ��΃p�X��
//
// ����:	path		�p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_AbsolutePath( char *path )
{
	char tpath[PATH_MAX+1];
	
	if( !PathIsRelative( path ) || !strlen( path ) ) return;
	
	PathCombine( tpath, OSD_GetModulePath(), path );
	strcpy( path, tpath );
}


////////////////////////////////////////////////////////////////
// �p�X����
//
// ����:	pdst		������p�X�i�[�o�b�t�@�|�C���^
//			psrc1		�p�X1�i�[�o�b�t�@�|�C���^
//			psrc2		�p�X2�i�[�o�b�t�@�|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_AddPath( char *pdst, const char *psrc1, const char *psrc2 )
{
	PathCombine( pdst, psrc1, psrc2 );
}


////////////////////////////////////////////////////////////////
// ���W���[���p�X�擾
//
// ����:	�Ȃ�
// �Ԓl:	char *		�擾����������ւ̃|�C���^
////////////////////////////////////////////////////////////////
const char *OSD_GetModulePath( void )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetModulePath]\n" );
	
	static char mpath[PATH_MAX] = "";	// ���W���[���p�X�擾�p
	
	if( !GetModuleFileName( NULL, mpath, PATH_MAX ) ) return NULL;
	
	char *fname = PathFindFileName( mpath );
	if( fname != mpath ) *fname = 0;
	OSD_AddDelimiter( mpath );	// �O�̂���
	
	return mpath;
}


////////////////////////////////////////////////////////////////
// �p�X����t�@�C�������擾
//
// ����:	path		�p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	char *		�t�@�C�����̊J�n�|�C���^
////////////////////////////////////////////////////////////////
const char *OSD_GetFileNamePart( const char *path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFileNamePart]\n" );
	
	return PathFindFileName( path );
}


////////////////////////////////////////////////////////////////
// �p�X����t�H���_�����擾
//
// ����:	path		�p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	char *		�t�H���_���̊J�n�|�C���^
////////////////////////////////////////////////////////////////
const char *OSD_GetFolderNamePart( const char *path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFolderNamePart]\n" );
	
	static char fpath[PATH_MAX] = "";	// �t�H���_�p�X�擾�p
	strncpy( fpath, path, PATH_MAX );
	PathRemoveFileSpec( fpath );
	return fpath;
}


////////////////////////////////////////////////////////////////
// �p�X����g���q�����擾
//
// ����:	path		�p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	char *		�g���q���̊J�n�|�C���^
////////////////////////////////////////////////////////////////
const char *OSD_GetFileNameExt( const char *path )
{
	PRINTD( OSD_LOG, "[OSD][OSD_GetFileNameExt]\n" );
	
	const char *ext = PathFindExtension( path );
	if( *ext == '.' ) ext++;
	return ext;
}


////////////////////////////////////////////////////////////////
// �t�@�C���̑��݃`�F�b�N
//
// ����:	path		�p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	bool		true:���݂��� false:���݂��Ȃ�
////////////////////////////////////////////////////////////////
bool OSD_FileExist( const char *fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileExist]\n" );
	
//	return PathFileExists( fullpath );	// ���C���h�J�[�h�͎g���Ȃ��炵��

	char filename[PATH_MAX+1];
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	bool ret = false;
	
	strncpy( filename, fullpath, PATH_MAX );
	OSD_DelDelimiter( filename );
	
	hFind = FindFirstFile( filename, &FindFileData );
	if( hFind != INVALID_HANDLE_VALUE ) ret = true;
	FindClose( hFind );
	
	return ret;
}


////////////////////////////////////////////////////////////////
// �t�@�C���̓ǎ���p�`�F�b�N
//
// ����:	path		�p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	bool		true:�ǎ���p false:�ǂݏ���
///////////////////////////////////////////////////////////////
bool OSD_FileReadOnly( const char *fullpath )
{
	PRINTD( OSD_LOG, "[OSD][OSD_FileReadOnly]\n" );
	
	char filename[PATH_MAX+1];
	
	strncpy( filename, fullpath, PATH_MAX );
	OSD_DelDelimiter( filename );
	
	DWORD fa = GetFileAttributes( filename );
	
	if( fa & FILE_ATTRIBUTE_READONLY ) return true;
	else                               return false;
}


///////////////////////////////////////////////////////////
// �t�H���_�̎Q��
//
// ����:	hwnd		�e�̃E�B���h�E�n���h��
//			Result		�t�H���_�ւ̃p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	char *		�擾����������ւ̃|�C���^
///////////////////////////////////////////////////////////
const char *OSD_FolderDiaog( HWINDOW hwnd, char *Result )
{
	LPMALLOC Memory;
	LPITEMIDLIST Ret, Root;
	char Buffer[PATH_MAX];
	
	// �E�B���h�E�n���h���ϊ�
	HWND hhwnd = (HWND)OSD_GetWindowHandle( hwnd );
	
	// �V�F����IMalloc�C���^�t�F�[�X�Ƀ|�C���^���擾
	SHGetMalloc( &Memory );
	
	//�u�}�C�R���s���[�^�v�t�H���_�̈ʒu���擾����
	SHGetSpecialFolderLocation( hhwnd, CSIDL_DRIVES, &Root );
	
	//BrowseInfo�\���̂̏����ݒ�
	ZeroMemory( &OBI, sizeof(BROWSEINFO) );
	OBI.hwndOwner = hhwnd;				// �e�E�C���h�E�̃n���h��
	OBI.pidlRoot  = Root;				// ���[�g�t�H���_(CSIDL_�~�~)
	OBI.ulFlags   = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS;	// �t�H���_�̃^�C�v�������t���O�(BIF_�~�~)
	OBI.lpszTitle = "�t�H���_��I�����Ă��������B";			// �^�C�g���i������j
	OBI.pszDisplayName = Buffer;		// (�߂�l)�t�H���_��
	OBI.lpfn = OsdBrowseCallbackProc;	// �R�[���o�b�N�֐��̃G���g���|�C���g
	OBI.lParam = (LPARAM)Result;		// �R�[���o�b�N�֐��ɓn������
    	
	//�u�t�H���_�̎Q�Ɓv�_�C�A���O��\��
	Ret = SHBrowseForFolder( &OBI );
	
	if( SHGetPathFromIDList( Ret, Buffer ) ){
		//�������̉��
		Memory->Free( Ret );
		Memory->Free( Root );
		
		lstrcpyn( Result, Buffer, PATH_MAX-1 );
		OSD_DelDelimiter( Result );
		return Result;
	}else{
		//�������̉��
		Memory->Free( Ret );
		Memory->Free( Root );
		
		return NULL;
	}
}

// OSD_FolderDiaog()�Ŏg�p����R�[���o�b�N�v���V�[�W��
static int CALLBACK OsdBrowseCallbackProc( HWND hhwnd, UINT msg, LPARAM lp1, LPARAM lp2 )
{
	char Path[PATH_MAX*2];	// �O�̂���2�{?
	
	// BIF_STATUSTEXT�ȊO�͉������Ȃ�
	if( OBI.ulFlags & BIF_STATUSTEXT ){
		switch( msg ){
		case BFFM_INITIALIZED:	// ������
			// �����t�H���_��ݒ�
			SendMessage( hhwnd, BFFM_SETSELECTION, (WPARAM)true, lp2 );
			break;
		case BFFM_SELCHANGED:	// ���[�U�[���t�H���_��ύX����
			// ITEMIDLIST�\���̂���p�X�������o��
			SHGetPathFromIDList( (LPCITEMIDLIST)lp1, Path );
			// �ύX���ꂽ�t�H���_�̃p�X��\������
			SendMessage( hhwnd, BFFM_SETSTATUSTEXT, 0, (LPARAM)Path );
		}
	}
	return 0;
}


///////////////////////////////////////////////////////////
// �t�@�C���̎Q��
//
// ����:	hwnd		�e�̃E�B���h�E�n���h��
//			mode		���[�h FM_Load:�t�@�C�����J�� FM_Save:���O��t���ĕۑ�
//			title		�E�B���h�E�L���v�V����������ւ̃|�C���^
//			filter		�t�@�C���t�B���^������ւ̃|�C���^
//			fullpath	�t���p�X�i�[�o�b�t�@�|�C���^
//			path		�t�@�C�������p�X�i�[�o�b�t�@�|�C���^
//			ext			�g���q������ւ̃|�C���^
// �Ԓl:	char *		�擾�����t�@�C�����ւ̃|�C���^(�t���p�X�ł͂Ȃ�)
///////////////////////////////////////////////////////////
const char *OSD_FileDiaog( HWINDOW hwnd, FileMode mode, const char *title, const char *filter, char *fullpath, char *path, const char *ext )
{
	OPENFILENAME fname;
	char File[PATH_MAX] = "";
	char Path[PATH_MAX] = "";
	bool ret = false;
	
	if( !fullpath ) return NULL;
	
	if( OSD_FileExist( fullpath ) )
		strncpy( File, fullpath, PATH_MAX );
	else if( path )
		strncpy( Path, path,     PATH_MAX );
	
	// �E�B���h�E�n���h���ϊ�
	HWND hhwnd = (HWND)OSD_GetWindowHandle( hwnd );
	
	ZeroMemory( &fname, sizeof(OPENFILENAME) );
	
	fname.lStructSize     = sizeof(OPENFILENAME);
	fname.hwndOwner       = hhwnd;			// �e�̃E�B���h�E�n���h��
	fname.lpstrFilter     = filter;			// �t�@�C���t�B���^
	fname.nFilterIndex    = 1;				// 1�Ԗڂ̃t�@�C���t�B���^���g��
	fname.lpstrFile       = File;			// �I�����ꂽ�t���p�X�̊i�[��
	fname.nMaxFile        = sizeof(File);	// ���̃T�C�Y
	fname.lpstrInitialDir = Path;			// �����t�H���_
	fname.lpstrTitle      = title;			// �^�C�g��
	fname.lpstrDefExt     = ext;			// �g���q���ȗ�����Ă���ꍇ�ɒǉ�����g���q
//	fname.Flags           = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES;
	
	if( mode == FM_Save ){
		fname.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_LONGNAMES;
		ret = GetSaveFileName( &fname );
	}else{
		fname.Flags = OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_LONGNAMES;
		ret = GetOpenFileName( &fname );
	}
	
	if( ret ){
		// �p�X��ۑ�
		if( path ){
			strncpy( path, File, fname.nFileOffset );
			path[fname.nFileOffset] = 0;
		}
		// �t���p�X��ۑ�
		if( fullpath ) strncpy( fullpath, File, PATH_MAX );
		// �t�@�C�����ւ̃|�C���^��Ԃ�
		return fullpath + fname.nFileOffset;
	}else
		return NULL;
}


////////////////////////////////////////////////////////////////
// �e��t�@�C���I��
//
// ����:	hwnd		�e�̃E�B���h�E�n���h��(NULL�̏ꍇ�̓X�N���[���T�[�t�F�X�̃n���h���擾�����݂�)
//			type		�_�C�A���O�̎��(FileDlg�Q��)
//			fullpath	�t���p�X�i�[�o�b�t�@�|�C���^
//			path		�t�@�C�������p�X�i�[�o�b�t�@�|�C���^
// �Ԓl:	char *		�擾�����t�@�C�����ւ̃|�C���^(�t���p�X�ł͂Ȃ�)
////////////////////////////////////////////////////////////////
const char *OSD_FileSelect( HWINDOW hwnd, FileDlg type, char *fullpath, char *path )
{
	FileMode mode = FM_Load;
	const char *title   = NULL;
	const char *filter  = NULL;
	const char *ext     = NULL;
	
	switch( type ){
	case FD_TapeLoad:	// TAPE(LOAD)�I��
		mode   = FM_Load;
		title  = "TAPE�C���[�W�I��(LOAD)";
		filter = "TAPE�C���[�W\0*.p6t;*.cas;*.p6\0"
    			 "P6T�`�� (*.p6t)\0*.p6t\0"
    			 "�x�^�`�� (*.cas;*.p6)\0*.cas\0"
    			 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "p6t";
		break;
		
	case FD_TapeSave:	// TAPE(SAVE)�I��
		mode   = FM_Save;
		title  = "TAPE�C���[�W�I��(SAVE)";
		filter = "TAPE�C���[�W\0*.p6t;*.cas;*.p6\0"
    			 "P6T�`�� (*.p6t)\0*.p6t\0"
    			 "�x�^�`�� (*.cas;*.p6)\0*.cas\0"
    			 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "p6t";
		break;
		
	case FD_Disk:		// DISK�I��
		mode   = FM_Load;
		title  = "DISK�C���[�W�I��";
		filter = "DISK�C���[�W\0*.d88\0"
				 "D88�`�� (*.d88)\0*.d88\0"
				 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "d88";
		break;
		
	case FD_ExtRom:		// �g��ROM�I��
		mode   = FM_Load;
		title  = "�g��ROM�C���[�W�I��";
		filter = "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		break;
		
	case FD_Printer:	// �v�����^�[�o�̓t�@�C���I��
		mode   = FM_Save;
		title  = "�v�����^�[�o�̓t�@�C���I��";
		filter = "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "txt";
		break;
		
	case FD_FontZ:		// �S�p�t�H���g�t�@�C���I��
		mode   = FM_Load;
		title  = "�S�p�t�H���g�t�@�C���I��";
		filter = "PNG�t�@�C�� (*.png)\0*.png\0"
				 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "png";
		break;
		
	case FD_FontH:		// ���p�t�H���g�t�@�C���I��
		mode   = FM_Load;
		title  = "���p�t�H���g�t�@�C���I��";
		filter = "PNG�t�@�C�� (*.png)\0*.png\0"
				 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "png";
		break;
		
	case FD_DokoLoad:	// �ǂ��ł�LOAD�t�@�C���I��
		mode   = FM_Load;
		title  = "�ǂ��ł�LOAD�t�@�C���I��";
		filter = "�ǂ��ł�SAVE�t�@�C��\0*.dds;*.ddr\0"
				 "�ǂ��ł�SAVE�`�� (*.dds)\0*.dds\0"
				 "���v���C�t�@�C�� (*.ddr)\0*.ddr\0"
				 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "dds";
		break;
		
	case FD_DokoSave:	// �ǂ��ł�SAVE�t�@�C���I��
		mode   = FM_Save;
		title  = "�ǂ��ł�SAVE�t�@�C���I��";
		filter = "�ǂ��ł�SAVE�`�� (*.dds)\0*.dds\0"
				 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "dds";
		break;
		
	case FD_RepLoad:	// ���v���C�Đ��t�@�C���I��
		mode   = FM_Load;
		title  = "���v���C�Đ��t�@�C���I��";
		filter = "���v���C�t�@�C�� (*.ddr)\0*.ddr\0"
				 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "ddr";
		break;
		
	case FD_RepSave:	// ���v���C�ۑ��t�@�C���I��
		mode   = FM_Save;
		title  = "���v���C�ۑ��t�@�C���I��";
		filter = "���v���C�t�@�C�� (*.ddr)\0*.ddr\0"
				 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "ddr";
		break;
		
	case FD_AVISave:	// �r�f�I�L���v�`���o�̓t�@�C���I��
		mode   = FM_Save;
		title  = "�r�f�I�L���v�`���o�̓t�@�C���I��";
		filter = "AVI�t�@�C�� (*.avi)\0*.avi\0"
				 "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		ext    = "avi";
		break;
		
	case FD_LoadAll:	// �ėpLOAD
	default:
		mode   = FM_Load;
		title  = "�t�@�C���I��";
		filter = "�S�Ẵt�@�C�� (*.*)\0*.*\0\0";
		break;
	}
	
	return OSD_FileDiaog( hwnd, mode, title, filter, fullpath, path, ext );
}


////////////////////////////////////////////////////////////////
// ���b�Z�[�W�\��
//
// ����:	mes			���b�Z�[�W������ւ̃|�C���^
//			cap			�E�B���h�E�L���v�V����������ւ̃|�C���^
//			type		�\���`���w���̃t���O
// �Ԓl:	int			�����ꂽ�{�^���̎��
//							OSDR_OK:     OK�{�^��
//							OSDR_CANCEL: CANCEL�{�^��
//							OSDR_YES:    YES�{�^��
//							OSDR_NO:     NO�{�^��
////////////////////////////////////////////////////////////////
int OSD_Message( const char *mes, const char *cap, int type )
{
	int Type = MB_OK;
	
	// ���b�Z�[�W�{�b�N�X�̃^�C�v
	switch( type&0x000f ){
	case OSDM_OK:			Type = MB_OK;			break;
	case OSDM_OKCANCEL:		Type = MB_OKCANCEL;		break;
	case OSDM_YESNO:		Type = MB_YESNO;		break;
	case OSDM_YESNOCANCEL:	Type = MB_YESNOCANCEL;	break;
	}
	
	// ���b�Z�[�W�{�b�N�X�̃A�C�R���^�C�v
	switch( type&0x00f0 ){
	case OSDM_ICONERROR:	Type |= MB_ICONERROR;		break;
	case OSDM_ICONQUESTION:	Type |= MB_ICONQUESTION;	break;
	case OSDM_ICONWARNING:	Type |= MB_ICONWARNING;		break;
	case OSDM_ICONINFO:		Type |= MB_ICONINFORMATION;	break;
	}
	
	// �e�E�B���h�E�̓X�N���[���T�[�t�F�X�ƌ��ߑł�
//	SDL_SysWMinfo WinInfo;
//	SDL_VERSION( &WinInfo.version );
//	HWND hwnd = SDL_GetWMInfo( &WinInfo ) ? WinInfo.window : NULL;
	
//	int res = MessageBox( hwnd, mes, cap, Type | MB_TOPMOST );
	int res = MessageBox( NULL, mes, cap, Type | MB_TOPMOST );
	
	switch( res ){
	case IDOK:	return OSDR_OK;
	case IDYES:	return OSDR_YES;
	case IDNO:	return OSDR_NO;
	default:	return OSDR_CANCEL;
	}
}




////////////////////////////////////////////////////////////////
// �L�[���s�[�g�ݒ�
//
// ����:	repeat	�L�[���s�[�g�̊Ԋu(ms) 0�Ŗ���
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_SetKeyRepeat( int repeat )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	// SDL2.0�ɂ̓L�[���s�[�g�̐ݒ肪�Ȃ��炵��
	#else
	SDL_EnableKeyRepeat( 500, repeat );
	#endif
}


////////////////////////////////////////////////////////////////
// OSD�L�[�R�[�h -> ���z�L�[�R�[�h�ϊ�
//
// ����:	scode		���ˑ��̃L�[�R�[�h
// �Ԓl:	PCKEYsym	���z�L�[�R�[�h
////////////////////////////////////////////////////////////////
PCKEYsym OSD_ConvertKeyCode( int scode )
{
	return VKTable[scode];
}


////////////////////////////////////////////////////////////////
// ���p�\�ȃW���C�X�e�B�b�N���擾
//
// ����:	�Ȃ�
// �Ԓl:	int			���p�\�ȃW���C�X�e�B�b�N��
////////////////////////////////////////////////////////////////
int OSD_GetJoyNum( void )
{
	return SDL_NumJoysticks();
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N���擾
//
// ����:	int			�C���f�b�N�X
// �Ԓl:	char *		�W���C�X�e�B�b�N��������ւ̃|�C���^
////////////////////////////////////////////////////////////////
const char *OSD_GetJoyName( int index )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	return SDL_JoystickNameForIndex( index );
	#else
	return SDL_JoystickName( index );
	#endif
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N�I�[�v������Ă�H
//
// ����:	int			�C���f�b�N�X
// �Ԓl:	bool		true:OPEN false:CLOSE
////////////////////////////////////////////////////////////////
bool OSD_OpenedJoy( int index )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	// SDL_JoystickOpened ������???
	return true;
	#else
	return SDL_JoystickOpened( index ) ? true : false;
	#endif
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N�I�[�v��
//
// ����:	int			�C���f�b�N�X
// �Ԓl:	HJOYINFO	�W���C�X�e�B�b�N���ւ̃|�C���^
////////////////////////////////////////////////////////////////
HJOYINFO OSD_OpenJoy( int index )
{
	return (HJOYINFO)SDL_JoystickOpen( index );
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N�N���[�Y
//
// ����:	int			�C���f�b�N�X
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_CloseJoy( HJOYINFO jinfo )
{
	SDL_JoystickClose( (SDL_Joystick *)jinfo );
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N�̎��̐��擾
//
// ����:	HJOYINFO	�W���C�X�e�B�b�N���ւ̃|�C���^
// �Ԓl:	int			���̐�
////////////////////////////////////////////////////////////////
int OSD_GetJoyNumAxes( HJOYINFO jinfo )
{
	return SDL_JoystickNumAxes( (SDL_Joystick *)jinfo );
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N�̃{�^���̐��擾
//
// ����:	HJOYINFO	�W���C�X�e�B�b�N���ւ̃|�C���^
// �Ԓl:	int			�{�^���̐�
////////////////////////////////////////////////////////////////
int OSD_GetJoyNumButtons( HJOYINFO jinfo )
{
	return SDL_JoystickNumButtons( (SDL_Joystick *)jinfo );
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N�̎��̏�Ԏ擾
//
// ����:	HJOYINFO	�W���C�X�e�B�b�N���ւ̃|�C���^
//			int			���̔ԍ�
// �Ԓl:	int			���̏��(-32768�`32767)
////////////////////////////////////////////////////////////////
int OSD_GetJoyAxis( HJOYINFO jinfo, int num )
{
	return SDL_JoystickGetAxis( (SDL_Joystick *)jinfo, num );
}


////////////////////////////////////////////////////////////////
// �W���C�X�e�B�b�N�̃{�^���̏�Ԏ擾
//
// ����:	HJOYINFO	�W���C�X�e�B�b�N���ւ̃|�C���^
//			int			�{�^���̔ԍ�
// �Ԓl:	bool		�{�^���̏�� true:ON false:OFF
////////////////////////////////////////////////////////////////
bool OSD_GetJoyButton( HJOYINFO jinfo, int num )
{
	return SDL_JoystickGetButton( (SDL_Joystick *)jinfo, num ) ? true : false;
}


////////////////////////////////////////////////////////////////
// �I�[�f�B�I�f�o�C�X�I�[�v��
//
// ����:	obj			�������g�ւ̃I�u�W�F�N�g�|�C���^
//			callback	�R�[���o�b�N�֐��ւ̃|�C���^
//			rate		�T���v�����O���[�g
//			sample		�o�b�t�@�T�C�Y(�T���v����)
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool OSD_OpenAudio( void *obj, CBF_SND callback, int rate, int samples )
{
	SDL_AudioSpec ASpec;				// �I�[�f�B�I�X�y�b�N
	
	ASpec.freq     = rate;				// �T���v�����O���[�g
	ASpec.format   = AUDIOFORMAT;		// �t�H�[�}�b�g
	ASpec.channels = 1;					// ���m����
	ASpec.samples  = samples;			// �o�b�t�@�T�C�Y(�T���v����)
	ASpec.callback = callback;			// �R�[���o�b�N�֐��̎w��
	ASpec.userdata = obj;				// �R�[���o�b�N�֐��Ɏ������g�̃I�u�W�F�N�g�|�C���^��n��
	
	// �I�[�f�B�I�f�o�C�X���J��
	if( SDL_OpenAudio( &ASpec, NULL ) < 0 ) return false;
	
	return true;
}


////////////////////////////////////////////////////////////////
// �I�[�f�B�I�f�o�C�X�N���[�Y
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_CloseAudio( void )
{
	SDL_CloseAudio();
}


////////////////////////////////////////////////////////////////
// �Đ��J�n
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_StartAudio( void )
{
	SDL_PauseAudio( 0 );
}


////////////////////////////////////////////////////////////////
// �Đ���~
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_StopAudio( void )
{
	SDL_PauseAudio( 1 );
}


////////////////////////////////////////////////////////////////
// �Đ���Ԏ擾
//
// ����:	�Ȃ�
// �Ԓl:	bool		true:�Đ��� false:��~��
////////////////////////////////////////////////////////////////
bool OSD_AudioPlaying( void )
{
	return SDL_GetAudioStatus() == SDL_AUDIO_PLAYING ? true : false;
}


////////////////////////////////////////////////////////////////
// Wave�t�@�C���Ǎ���
// �@�Ή��`���� 22050Hz�ȏ�,�����t��16bit,1ch
//
// ����:	filepath	�t�@�C���p�X
//			buf			�o�b�t�@�|�C���^�i�[�|�C���^
//			len			�t�@�C�������i�[�|�C���^
//			freq		�T���v�����O���[�g�i�[�|�C���^
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool OSD_LoadWAV( const char *filepath, BYTE **buf, DWORD *len, int *freq )
{
	SDL_AudioSpec ws;
	
	if( !SDL_LoadWAV( filepath, &ws, buf, (Uint32 *)len ) ) return false;
	
	if( ws.freq < 22050 || ws.format != AUDIO_S16 || ws.channels != 1 ){
		SDL_FreeWAV( *buf );
		return false;
	}
	
	*freq    = ws.freq;
	
	return true;
}


////////////////////////////////////////////////////////////////
// Wave�t�@�C���J��
//
// ����:	buf			�o�b�t�@�ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_FreeWAV( BYTE *buf )
{
	SDL_FreeWAV( buf );
}


////////////////////////////////////////////////////////////////
// �I�[�f�B�I�����b�N����
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_LockAudio( void )
{
	SDL_LockAudio();
}


////////////////////////////////////////////////////////////////
// �I�[�f�B�I���A�����b�N����
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_UnlockAudio( void )
{
	SDL_UnlockAudio();
}


////////////////////////////////////////////////////////////////
// �w�莞�ԑҋ@
//
// ����:	tms			�ҋ@����(ms)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_Delay( DWORD tms )
{
	#ifdef USESDLTIMER
	SDL_Delay( tms );
	#else
	Sleep( tms );
	#endif
}


////////////////////////////////////////////////////////////////
// �v���Z�X�J�n����̌o�ߎ��Ԏ擾
//
// ����:	�Ȃ�
// �Ԓl:	DWORD		�o�ߎ���(ms)
////////////////////////////////////////////////////////////////
DWORD OSD_GetTicks( void )
{
	#ifdef USESDLTIMER
	return SDL_GetTicks();
	#else
	return GetTickCount();
	#endif
}


////////////////////////////////////////////////////////////////
// �^�C�}�ǉ�
//
// ����:	interval	�����݊Ԋu(ms)
//			callback	�R�[���o�b�N�֐�
//			param		�R�[���o�b�N�֐��ɓn������
// �Ԓl:	HTIMERID	�^�C�}ID(���s������NULL)
////////////////////////////////////////////////////////////////
HTIMERID OSD_AddTimer( DWORD interval, CBF_TMR callback, void *param )
{
	#ifdef USESDLTIMER
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_TimerID *pid = NULL;
	SDL_TimerID tid = SDL_AddTimer( interval, (SDL_TimerCallback)callback, param );
	if( tid ){
		pid = new SDL_TimerID;
		if( pid ) *pid = tid;
	}
	return (HTIMERID)pid;
	#else
	return (HTIMERID)SDL_AddTimer( interval, (SDL_NewTimerCallback)callback, param );
	#endif
	#else
	TINFO *tinfo = new TINFO;
	if( !tinfo ) return NULL;
	
	timeBeginPeriod( TIMERRES );
	tinfo->interval = interval;
	tinfo->callback = callback;
	tinfo->param    = param;
	tinfo->id       = timeSetEvent( interval, 1, OsdTimerCallbackProc, 0, TIME_PERIODIC );
	if( !tinfo->id ){
		delete tinfo;
		timeEndPeriod( TIMERRES );
		return NULL;
	}
	
	TINFO *ti = tif;
	if( !ti ) tif = tinfo;
	else{
		while( ti->next ) ti = ti->next;
		ti->next = tinfo;
	}
	
	return (HTIMERID)tinfo;
	#endif
}

#ifndef USESDLTIMER
// OSD_AddTimer()�Ŏg�p����R�[���o�b�N�v���V�[�W��
static void CALLBACK OsdTimerCallbackProc( UINT id,  UINT, DWORD_PTR data, DWORD_PTR, DWORD_PTR )
{
	TINFO *ti = tif;
	
	while( ti && (ti->id != id) ) ti = ti->next;
	if( ti ) ti->callback( ti->interval, ti->param );
}
#endif


////////////////////////////////////////////////////////////////
// �^�C�}�폜
//
// ����:	id			�^�C�}ID
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool OSD_DelTimer( HTIMERID id )
{
	#ifdef USESDLTIMER
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( SDL_RemoveTimer( *((SDL_TimerID *)id) ) ){
		delete (SDL_TimerID *)id;
		return true;
	}else
		return false;
	#else
	return (bool)SDL_RemoveTimer( (SDL_TimerID)id );
	#endif
	#else
	if( !id ) return false;
	
	TINFO *tinfo = (TINFO *)id;
	TINFO *ti    = tif;
	TINFO *tip   = NULL;
	
	while( ti && (ti->id != tinfo->id) ){
		tip = ti;
		ti  = ti->next;
	}
	if( !ti ) return false;
	
	if( timeKillEvent( ti->id ) != TIMERR_NOERROR )
		return false;
	timeEndPeriod( TIMERRES );
	
	delete ti;
	if( tip ) tip->next = NULL;
	else      tif = NULL;
	
	return true;
	#endif
}


////////////////////////////////////////////////////////////////
// �L���v�V�����ݒ�
//
// ����:	wh			�E�B���h�E�n���h��
//			str			�L���v�V����������ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_SetWindowCaption( HWINDOW wh, const char *str )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( wh && str ) SDL_SetWindowTitle( (SDL_Window*)wh, str );
	#else
	if( str ) SDL_WM_SetCaption( str, "" );
	#endif
}


////////////////////////////////////////////////////////////////
// �}�E�X�J�[�\���\��/��\��
//
// ����:	disp		true:�\�� false:��\��
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_ShowCursor( bool disp )
{
	SDL_ShowCursor( disp ? SDL_ENABLE : SDL_DISABLE );
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�쐬
//
// ����:	pwh			�E�B���h�E�n���h���ւ̃|�C���^
//			w			��
//			h			����
//			fsflag		true:�t���X�N���[�� false:�E�B���h�E
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool OSD_CreateWindow( HWINDOW *pwh, int w, int h, bool fsflag )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_Renderer *rend;
	
	if( !*pwh ){
		*pwh = (HWINDOW)SDL_CreateWindow( "", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h,
											SDL_WINDOW_SHOWN );
		if( !*pwh ) return false;
	}else{
		rend = SDL_GetRenderer( (SDL_Window *)*pwh );
		if( rend ) SDL_DestroyRenderer( rend );
	}
	
	if( fsflag ){
		SDL_DisplayMode mode;
		
		mode.format = SDL_PIXELFORMAT_ARGB8888;
		mode.w = w;
		mode.h = h;
		mode.refresh_rate = 0;
		mode.driverdata   = 0;
		SDL_SetWindowDisplayMode( (SDL_Window *)*pwh, &mode );
		SDL_SetWindowFullscreen( (SDL_Window *)*pwh, SDL_TRUE );
	}else{
		SDL_SetWindowFullscreen( (SDL_Window *)*pwh, SDL_FALSE );
		SDL_SetWindowSize( (SDL_Window *)*pwh, w, h );
	}
	
	rend = SDL_CreateRenderer( (SDL_Window *)*pwh, -1, SDLOP_SCREEN );
	SDL_SetRenderDrawColor( rend, 255, 255, 255, 255 );
	SDL_RenderSetViewport( rend, NULL );
	SDL_RenderClear( rend );
	SDL_RenderPresent( rend );
	
	#else
	*pwh = (HWINDOW)SDL_SetVideoMode( w, h, 32, SDLOP_SCREEN | (fsflag ? SDL_FULLSCREEN : 0) );
	#endif
	return *pwh ? true : false;
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�j��
//
// ����:	wh			�E�B���h�E�n���h��
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_DestroyWindow( HWINDOW wh )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( wh ){
		SDL_Renderer *rend = SDL_GetRenderer( (SDL_Window *)wh );
		if( rend ) SDL_DestroyRenderer( rend );
		SDL_DestroyWindow( (SDL_Window *)wh );
	}
	#else
	// SDL1.2�͖���
	#endif
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�̕����擾
//
// ����:	wh			�E�B���h�E�n���h��
// �Ԓl:	int			��
////////////////////////////////////////////////////////////////
int OSD_GetWindowWidth( HWINDOW wh )
{
	int res = 0;
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( wh ) SDL_GetWindowSize( (SDL_Window *)wh, &res, NULL );
	#else
	if( wh ) res = ((SDL_Surface *)wh)->w;
	#endif
	return res;
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�̍������擾
//
// ����:	wh			�E�B���h�E�n���h��
// �Ԓl:	int			����
////////////////////////////////////////////////////////////////
int OSD_GetWindowHeight( HWINDOW wh )
{
	int res = 0;
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( wh ) SDL_GetWindowSize( (SDL_Window *)wh, NULL, &res );
	#else
	if( wh ) res = ((SDL_Surface *)wh)->h;
	#endif
	return res;
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�N���A
//  �F��0(��)�Ō��ߑł�
//
// ����:	wh			�E�B���h�E�n���h��
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_ClearWindow( HWINDOW wh )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_Renderer *rend = SDL_GetRenderer( (SDL_Window *)wh );
	SDL_SetRenderDrawColor( rend, 0, 0, 0, 0xff );
	SDL_RenderClear( rend );
	#else
	SDL_FillRect( (SDL_Surface *)wh, NULL, 0 );
	#endif
}


////////////////////////////////////////////////////////////////
// �E�B���h�E���f
//
// ����:	wh			�E�B���h�E�n���h��
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_RenderWindow( HWINDOW wh )
{
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_Renderer *rend = SDL_GetRenderer( (SDL_Window *)wh );
	SDL_RenderPresent( rend );
	#else
 	SDL_Flip( (SDL_Surface *)wh );
	#endif

}


////////////////////////////////////////////////////////////////
// �E�B���h�E�ɓ]��(���{)
//
// ����:	wh			�E�B���h�E�n���h��
//			src			�]�����T�[�t�F�X
//			x			�]����x���W
//			y			�]����y���W
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void OSD_BlitToWindow( HWINDOW wh, VSurface *src, const int x, const int y )
{
	VRect src1,drc1;
	
	if( !src || !wh ) return;
	
	#if SDL_VERSION_ATLEAST(2,0,0)
	SDL_Renderer *rend = SDL_GetRenderer( (SDL_Window *)wh );
	if( !rend ) return;
	
	
	#else
	SDL_Surface *dst  = SDL_GetVideoSurface();
	if( !dst ) return;
	
	// �]�����͈͐ݒ�
	src1.x = max( 0, -x );
	src1.y = max( 0, -y );
	src1.w = min( src->Width()  - src1.x, dst->w );
	src1.h = min( src->Height() - src1.y, dst->h );
	
	if( src1.w <= 0 || src1.h <= 0 ) return;
	
	// �]����͈͐ݒ�
	drc1.x = max( 0, x );
	drc1.y = max( 0, y );
	
	
	#if INBPP == 8	// 8bit
	BYTE *psrc    = (BYTE *)src->GetPixels()  + src->Pitch()               * src1.y + src1.x;
	const int pp  = src->Pitch();
	#else			// 32bit
	DWORD *psrc   = (DWORD *)src->GetPixels() + src->Pitch()/sizeof(DWORD) * src1.y + src1.x;
	const int pp  = src->Pitch() / sizeof(DWORD);
	#endif
	const int dpp = dst->pitch   / sizeof(DWORD);
	
	if( SDL_MUSTLOCK( dst ) ) SDL_LockSurface( dst );
	
	DWORD *pdst = (DWORD *)dst->pixels + dst->pitch  /sizeof(DWORD) * drc1.y + drc1.x;
	
	for( int y=0; y < src1.h; y++ ){
		#if INBPP == 8	// 8bit
		BYTE  *tps = psrc;
		DWORD *tpd = pdst;
		for( int x=0; x < src1.w; x++ )
			*tpd++ = VSurface::GetColor( *tps++ );
		#else			// 32bit
		memcpy( pdst, psrc, src1.w * sizeof(DWORD) );
		#endif
		psrc += pp;
		pdst += dpp;
	}
	
	if( SDL_MUSTLOCK( dst ) ) SDL_UnlockSurface( dst );
	#endif
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�ɓ]��(�g�哙)
//
// ����:	wh			�E�B���h�E�n���h��
//			src			�]�����T�[�t�F�X
//			dx			�]����x���W
//			dy			�]����y���W
//			dh			�]���捂��
//			ntsc		4:3�\���t���O
//			scan		�X�L�������C���t���O
//			brscan		�X�L�������C���P�x
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
#define	RESO		256		/* ���ԐF�v�Z�p����\(1���C����RESO��������) */
void OSD_BlitToWindowEx( HWINDOW wh, VSurface *src, const int dx, const int dy, const int dh,
						const bool ntsc, const bool scan, const int brscan )
{
	VRect src1,drc1;
	
	if( !src || !wh ) return;
	
	SDL_Surface *dst = SDL_GetVideoSurface();
	if( !dst ) return;
	
	#if INBPP == 8	// 8bit
	const BYTE *spt  = (BYTE *)src->GetPixels();
	const int pp     = src->Pitch();
	#else			// 32bit
	const DWORD *spt = (DWORD *)src->GetPixels();
	const int pp     = src->Pitch() / sizeof(DWORD);
	#endif
	const int xsc    = src->XScale();
	const int ww     = src->Width();
	const int hh     = src->Height();
	
	const DWORD *dpt = (DWORD *)dst->pixels;
	const int dpp    = dst->pitch / sizeof(DWORD);
	
	// �]�����͈͐ݒ�
	src1.x = max( 0, -dx / 2 * xsc );
	src1.y = max( 0, -dy );
	src1.w = min( ww * xsc - src1.x, dst->w );
	src1.h = min( hh       - src1.y, dst->h );
	
	if( src1.w <= 0 || src1.h <= 0 ) return;
	
	// �]����͈͐ݒ�
	drc1.x = max( 0, dx );
	drc1.y = max( 0, dy );
	drc1.w = min( ww, (dst->w - drc1.x)/2*xsc );
	drc1.h = min( dh, dst->h - drc1.y );
	
	if( SDL_MUSTLOCK( dst ) ) SDL_LockSurface( dst );
	
	for( int y=0; y<drc1.h; y++ ){
		int y0 = ( (y+src1.y) * hh ) / dh;
		int a1 = ( (y+src1.y) * hh * RESO ) / dh - y0 * RESO;
		int a2 = RESO - a1;
		
		#if INBPP == 8	// 8bit
		BYTE *sof1  = (BYTE *)spt  + pp * y0 + src1.x;
		BYTE *sof2  = sof1 + ( y0 < hh-1 ? pp : 0 );
		#else			// 32bit
		DWORD *sof1 = (DWORD *)spt + pp * y0 + src1.x;
		DWORD *sof2 = sof1 + ( y0 < hh-1 ? pp : 0 );
		#endif
		DWORD *doff = (DWORD *)dpt + dpp * (y+drc1.y) + drc1.x;
		
		for( int x=0; x<drc1.w; x++ ){
			DWORD r,g,b;
			#if INBPP == 8	// 8bit
			DWORD d1 = VSurface::GetColor( *sof1++ );
			DWORD d2 = VSurface::GetColor( *sof2++ );
			#else			// 32bit
			DWORD d1 = *sof1++;
			DWORD d2 = *sof2++;
			#endif
			
			if( ntsc ){
				r = ( ( ( (d1>>RSHIFT32)&0xff ) * a2 + ( (d2>>RSHIFT32)&0xff ) * a1 ) / RESO ) & 0xff;
				g = ( ( ( (d1>>GSHIFT32)&0xff ) * a2 + ( (d2>>GSHIFT32)&0xff ) * a1 ) / RESO ) & 0xff;
				b = ( ( ( (d1>>BSHIFT32)&0xff ) * a2 + ( (d2>>BSHIFT32)&0xff ) * a1 ) / RESO ) & 0xff;
			}else{
				r = (d1>>RSHIFT32)&0xff;
				g = (d1>>GSHIFT32)&0xff;
				b = (d1>>BSHIFT32)&0xff;
			}
			
			if( scan && y&1 ){
				r = ( ( r * brscan ) / 100 ) & 0xff;
				g = ( ( g * brscan ) / 100 ) & 0xff;
				b = ( ( b * brscan ) / 100 ) & 0xff;
			}
			*doff++ = (r<<RSHIFT32) | (g<<GSHIFT32) | (b<<BSHIFT32);
			if( xsc == 1 ) *doff++ = (r<<RSHIFT32) | (g<<GSHIFT32) | (b<<BSHIFT32);
		}
	}
	
	if( SDL_MUSTLOCK( dst ) ) SDL_UnlockSurface( dst );
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�̃C���[�W�f�[�^�擾
//
// ����:	wh			�E�B���h�E�n���h��
//			pixels		�]����z��|�C���^�ւ̃|�C���^
//			pos			�ۑ�����̈���ւ̃|�C���^
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool OSD_GetWindowImage( HWINDOW wh, void **pixels, VRect *pos )
{
	VRect src1;
	
	if( !wh || !pixels ) return false;
	
	#if SDL_VERSION_ATLEAST(2,0,0)
	
	#else
	
	SDL_Surface *src = SDL_GetVideoSurface();
	if( !src ) return false;
	
	// �]�����͈͐ݒ�
	if( pos ){
		src1.x = max( 0, min( pos->x, src->w ) );
		src1.y = max( 0, min( pos->y, src->h ) );
		src1.w = min( pos->w, src->w - src1.x );
		src1.h = min( pos->h, src->h - src1.y );
		if( src1.w <= 0 || src1.h <= 0 ) return false;
		
		*pos = src1;
		
	}else{
		src1.x = 0;
		src1.y = 0;
		src1.w = src->w;
		src1.h = src->h;
	}
	
//	BYTE *pixels = new BYTE[src1.w * src1.h * sizeof(DWORD)];
//	if( !pixels ) return false;
	
	const int spp = src->pitch / sizeof(DWORD);
	const int dpp = src1.w;
	DWORD *spt = (DWORD *)src->pixels + spp * src1.y + src1.x / sizeof(DWORD);
//	DWORD *dpt = (DWORD *)pixels;
	DWORD *dpt = (DWORD *)(*pixels);
	
	if( SDL_MUSTLOCK( src ) ) SDL_LockSurface( src );
	
	for( int y=0; y<src1.h; y++ ){
		memcpy( (BYTE *)dpt, (BYTE *)spt, src1.w * sizeof(DWORD) );
		spt += spp;
		dpt += dpp;
	}
	
	if( SDL_MUSTLOCK( src ) ) SDL_UnlockSurface( src );
	
	#endif
	
	return true;
}


///////////////////////////////////////////////////////////
// �A�C�R���ݒ�
//
// ����:	wh			�E�B���h�E�n���h��
//			model		�@�� 60,61,62,66,64,68
// �Ԓl:	�Ȃ�
///////////////////////////////////////////////////////////
void OSD_SetIcon( HWINDOW wh, int model )
{
	BYTE *ipix;
	#if SDL_VERSION_ATLEAST(2,0,0)
	switch( model ){
	case 61: ipix = (BYTE *)p61pix; break;
	case 62: ipix = (BYTE *)p62pix; break;
	case 66: ipix = (BYTE *)p66pix; break;
	case 64: ipix = (BYTE *)p64pix; break;
	case 68: ipix = (BYTE *)p68pix; break;
	default: ipix = (BYTE *)p60pix;
	}
	SDL_Surface *p6icon = SDL_CreateRGBSurfaceFrom( ipix, 32, 32, 32, 32*4, RMASK32, GMASK32, BMASK32, AMASK32 );
	SDL_SetWindowIcon( (SDL_Window *)wh, p6icon );
	#else
	BYTE *imsk;
	switch( model ){
	case 61: ipix = (BYTE *)p61pix; imsk = (BYTE *)p61msk; break;
	case 62: ipix = (BYTE *)p62pix; imsk = (BYTE *)p62msk; break;
	case 66: ipix = (BYTE *)p66pix; imsk = (BYTE *)p66msk; break;
	case 64: ipix = (BYTE *)p64pix; imsk = (BYTE *)p64msk; break;
	case 68: ipix = (BYTE *)p68pix; imsk = (BYTE *)p68msk; break;
	default: ipix = (BYTE *)p60pix; imsk = (BYTE *)p60msk;
	}
	SDL_Surface *p6icon = SDL_CreateRGBSurfaceFrom( ipix, 32, 32, 24, 32*3, 0, 0, 0, 0 );
	SDL_WM_SetIcon( p6icon, imsk );
	#endif
	SDL_FreeSurface( p6icon );
}


////////////////////////////////////////////////////////////////
// OS�ˑ��̃E�B���h�E�n���h�����擾
//
// ����:	wh			�E�B���h�E�n���h��
// �Ԓl:	void *		OS�ˑ��̃E�B���h�E�n���h��
////////////////////////////////////////////////////////////////
void *OSD_GetWindowHandle( HWINDOW wh )
{
	SDL_SysWMinfo WinInfo;
	
	SDL_VERSION( &WinInfo.version );
	#if SDL_VERSION_ATLEAST(2,0,0)
	if( SDL_GetWindowWMInfo( (SDL_Window *)wh, &WinInfo ) )
		return WinInfo.info.win.window;
	#else
	if( SDL_GetWMInfo( &WinInfo ) )
		return WinInfo.window;
	#endif
	else
		return NULL;
}


////////////////////////////////////////////////////////////////
// �C�x���g�擾(�C�x���g����������܂ő҂�)
//
// ����:	ev			�C�x���g��񋤗p�̂ւ̃|�C���^
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool OSD_GetEvent( Event *ev )
{
	SDL_Event event;
	
	if( !SDL_WaitEvent( &event ) ) return false;
	
	switch( event.type ){
	case SDL_KEYDOWN:
		ev->type			= EV_KEYDOWN;
		ev->key.state		= true;
		#if SDL_VERSION_ATLEAST(2,0,0)
		ev->key.sym			= OSD_ConvertKeyCode( event.key.keysym.scancode );
		ev->key.unicode		= event.key.keysym.scancode;
		#else
		ev->key.sym			= OSD_ConvertKeyCode( event.key.keysym.sym );
		ev->key.unicode		= event.key.keysym.unicode;
		#endif
		ev->key.mod			= (PCKEYmod)(
							  ( event.key.keysym.mod & KMOD_LSHIFT ? KVM_LSHIFT : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RSHIFT ? KVM_RSHIFT : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_LCTRL  ? KVM_LCTRL  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RCTRL  ? KVM_RCTRL  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_LALT   ? KVM_LALT   : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RALT   ? KVM_RALT   : KVM_NONE )
							#if SDL_VERSION_ATLEAST(2,0,0)
							| ( event.key.keysym.mod & KMOD_LGUI   ? KVM_LMETA  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RGUI   ? KVM_RMETA  : KVM_NONE )
							#else
							| ( event.key.keysym.mod & KMOD_LMETA  ? KVM_LMETA  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RMETA  ? KVM_RMETA  : KVM_NONE )
							#endif
							| ( event.key.keysym.mod & KMOD_NUM    ? KVM_NUM    : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_CAPS   ? KVM_CAPS   : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_MODE   ? KVM_MODE   : KVM_NONE ) );
		break;
		
	case SDL_KEYUP:
		ev->type			= EV_KEYUP;
		ev->key.state		= false;
		#if SDL_VERSION_ATLEAST(2,0,0)
		ev->key.sym			= OSD_ConvertKeyCode( event.key.keysym.scancode );
		ev->key.unicode		= event.key.keysym.scancode;
		#else
		ev->key.sym			= OSD_ConvertKeyCode( event.key.keysym.sym );
		ev->key.unicode		= event.key.keysym.unicode;
		#endif
		ev->key.mod			= (PCKEYmod)(
							  ( event.key.keysym.mod & KMOD_LSHIFT ? KVM_LSHIFT : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RSHIFT ? KVM_RSHIFT : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_LCTRL  ? KVM_LCTRL  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RCTRL  ? KVM_RCTRL  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_LALT   ? KVM_LALT   : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RALT   ? KVM_RALT   : KVM_NONE )
							#if SDL_VERSION_ATLEAST(2,0,0)
							| ( event.key.keysym.mod & KMOD_LGUI   ? KVM_LMETA  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RGUI   ? KVM_RMETA  : KVM_NONE )
							#else
							| ( event.key.keysym.mod & KMOD_LMETA  ? KVM_LMETA  : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_RMETA  ? KVM_RMETA  : KVM_NONE )
							#endif
							| ( event.key.keysym.mod & KMOD_NUM    ? KVM_NUM    : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_CAPS   ? KVM_CAPS   : KVM_NONE )
							| ( event.key.keysym.mod & KMOD_MODE   ? KVM_MODE   : KVM_NONE ) );
		break;
		
	case SDL_MOUSEBUTTONDOWN:
		ev->type			= EV_MOUSEBUTTONDOWN;
		ev->mousebt.button	= event.button.button == SDL_BUTTON_LEFT      ? MBT_LEFT      :
							  event.button.button == SDL_BUTTON_MIDDLE    ? MBT_MIDDLE    :
							  event.button.button == SDL_BUTTON_RIGHT     ? MBT_RIGHT     :
							  event.button.button == SDL_BUTTON_WHEELUP   ? MBT_WHEELUP   :
							  event.button.button == SDL_BUTTON_WHEELDOWN ? MBT_WHEELDOWN :
							  MBT_NONE;
		ev->mousebt.state	= true;
		ev->mousebt.x		= event.button.x;
		ev->mousebt.y		= event.button.y;
		break;
		
	case SDL_MOUSEBUTTONUP:
		ev->type			= EV_MOUSEBUTTONUP;
		ev->mousebt.button	= event.button.button == SDL_BUTTON_LEFT      ? MBT_LEFT      :
							  event.button.button == SDL_BUTTON_MIDDLE    ? MBT_MIDDLE    :
							  event.button.button == SDL_BUTTON_RIGHT     ? MBT_RIGHT     :
							  event.button.button == SDL_BUTTON_WHEELUP   ? MBT_WHEELUP   :
							  event.button.button == SDL_BUTTON_WHEELDOWN ? MBT_WHEELDOWN :
							  MBT_NONE;
		ev->mousebt.state	= false;
		ev->mousebt.x		= event.button.x;
		ev->mousebt.y		= event.button.y;
		break;
		
	case SDL_JOYAXISMOTION:
		ev->type			= EV_JOYAXISMOTION;
		ev->joyax.idx		= event.jaxis.which;
		ev->joyax.axis		= event.jaxis.axis;
		ev->joyax.value		= event.jaxis.value;
		break;
		
	case SDL_JOYBUTTONDOWN:
		ev->type			= EV_JOYBUTTONDOWN;
		ev->joybt.idx		= event.jbutton.which;
		ev->joybt.button	= event.jbutton.button;
		ev->joybt.state		= true;
		break;
		
	case SDL_JOYBUTTONUP:
		ev->type			= EV_JOYBUTTONUP;
		ev->joybt.idx		= event.jbutton.which;
		ev->joybt.button	= event.jbutton.button;
		ev->joybt.state		= false;
		break;
		
	case SDL_QUIT:
		ev->type			= EV_QUIT;
		break;
		
	case UEV_RESTART:
		ev->type			= EV_RESTART;
		break;
		
	case UEV_DOKOLOAD:
		ev->type			= EV_DOKOLOAD;
		break;
		
	case UEV_REPLAY:
		ev->type			= EV_REPLAY;
		break;
		
	case UEV_FPSUPDATE:
		ev->type			= EV_FPSUPDATE;
		ev->fps.fps			= event.user.code;
		break;
		
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	case UEV_DEBUGMODEBP:
		ev->type			= EV_DEBUGMODEBP;
		ev->bp.addr			= event.user.code;
		break;
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
		
	case SDL_DROPFILE:
		ev->type			= EV_DROPFILE;
		ev->drop.file		= new char[strlen( event.drop.file ) + 1];
		
		if( ev->drop.file ) strcpy( ev->drop.file, event.drop.file );
		else                ev->type = EV_NOEVENT;
		
		SDL_free( event.drop.file );
		break;
		
	default:
		ev->type			= EV_NOEVENT;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// �C�x���g���L���[�Ƀv�b�V������
//
// ����:	ev			�C�x���g�^�C�v
//			...			�C�x���g�^�C�v�ɉ���������
// �Ԓl:	bool		true:���� false:���s
////////////////////////////////////////////////////////////////
bool OSD_PushEvent( EventType ev, ... )
{
	SDL_Event event;
	
	va_list args;
	va_start( args, ev );
	
	#if SDL_VERSION_ATLEAST(2,0,0)
	event.type = SDL_FIRSTEVENT;
	#else
	event.type = SDL_NOEVENT;
	#endif
	
	switch( ev ){
	case EV_QUIT:
		event.type		= SDL_QUIT;
		break;
		
	case EV_RESTART:
		event.type		= UEV_RESTART;
		break;
		
	case EV_DOKOLOAD:
		event.type		= UEV_DOKOLOAD;
		break;
		
	case EV_REPLAY:
		event.type		= UEV_REPLAY;
		break;
		
	case EV_FPSUPDATE:
		event.type		= UEV_FPSUPDATE;
		event.user.code	= va_arg( args, int );
		break;
		
	case EV_DEBUGMODEBP:
		event.type		= UEV_DEBUGMODEBP;
		event.user.code	= va_arg( args, int );
		break;
		
	default:
		return false;
	}
	
	va_end( args );
	
	return SDL_PushEvent( &event ) ? false : true;
}



////////////////////////////////////////////////////////////////
// �F�̖��O�擾
//
// ����:	num			�F�R�[�h
// �Ԓl:	char *		�F�̖��O������ւ̃|�C���^
////////////////////////////////////////////////////////////////
const char *OSD_ColorName( int num )
{
	static const char *JColorName[] = {
		"MODE1,2 ��(�{�[�_�[)",
		"MODE1 Set1 ��",		"MODE1 Set1 �[��",		"MODE1 Set2 ��",		"MODE1 Set2 �[��",
		"MODE2 ��",				"MODE2 ��",				"MODE2 ��",				"MODE2 ��",
		"MODE2 ��",				"MODE2 �V�A��",			"MODE2 �}�[���^",		"MODE2 ��",
		"MODE3 Set1 ��",		"MODE3 Set1 ��",		"MODE3 Set1 ��",		"MODE3 Set1 ��",
		"MODE3 Set2 ��",		"MODE3 Set2 �V�A��",	"MODE3 Set2 �}�[���^",	"MODE3 Set2 ��",
		"MODE4 Set1 �[��",		"MODE4 Set1 ��",		"MODE4 Set2 ��",		"MODE4 Set2 ��",
		"MODE4 Set1 �ɂ��� ��",		"MODE4 Set1 �ɂ��� ��",		"MODE4 Set1 �ɂ��� ��",		"MODE4 Set1 �ɂ��� ��",
		"MODE4 Set1 �ɂ��� ����",	"MODE4 Set1 �ɂ��� �Ð�",	"MODE4 Set1 �ɂ��� ����",	"MODE4 Set1 �ɂ��� �Ð�",
		"MODE4 Set1 �ɂ��� ����",	"MODE4 Set1 �ɂ��� �Ó�",	"MODE4 Set1 �ɂ��� ����",	"MODE4 Set1 �ɂ��� �×�",
		"MODE4 Set2 �ɂ��� ��",		"MODE4 Set2 �ɂ��� ��",		"MODE4 Set2 �ɂ��� ��",		"MODE4 Set2 �ɂ��� ��",
		"MODE4 Set2 �ɂ��� ����",	"MODE4 Set2 �ɂ��� �Ð�",	"MODE4 Set2 �ɂ��� ����",	"MODE4 Set2 �ɂ��� �Ð�",
		"MODE4 Set2 �ɂ��� ����",	"MODE4 Set2 �ɂ��� �Ó�",	"MODE4 Set2 �ɂ��� ����",	"MODE4 Set2 �ɂ��� �×�",
		"mk2�` ����(��)",		"mk2�` ��",				"mk2�` ��",			"mk2�` ����",
		"mk2�` ��",			"mk2�` �Ԏ�",			"mk2�` ��F",			"mk2�` �D�F",
		"mk2�` ��",				"mk2�` ��",				"mk2�` ��",				"mk2�` ��",
		"mk2�` ��",				"mk2�` �}�[���^",		"mk2�` �V�A��",			"mk2�` ��"
	};
	
	if( num < 0 || num >= COUNTOF( JColorName ) ) return NULL;
	else                                          return JColorName[num];

}


////////////////////////////////////////////////////////////////
// �L�[�̖��O�擾
//
// ����:	sym			���z�L�[�R�[�h
// �Ԓl:	char *		�L�[���O������ւ̃|�C���^
////////////////////////////////////////////////////////////////
const char *OSD_KeyName( PCKEYsym sym )
{
	static const PCKeyName kname[] = {
		{ KVC_1,			"1" },
		{ KVC_2,			"2" },
		{ KVC_3,			"3" },
		{ KVC_4,			"4" },
		{ KVC_5,			"5" },
		{ KVC_6,			"6" },
		{ KVC_7,			"7" },
		{ KVC_8,			"8" },
		{ KVC_9,			"9" },
		{ KVC_0,			"0" },
		
		{ KVC_A,			"A" },
		{ KVC_B,			"B" },
		{ KVC_C,			"C" },
		{ KVC_D,			"D" },
		{ KVC_E,			"E" },
		{ KVC_F,			"F" },
		{ KVC_G,			"G" },
		{ KVC_H,			"H" },
		{ KVC_I,			"I" },
		{ KVC_J,			"J" },
		{ KVC_K,			"K" },
		{ KVC_L,			"L" },
		{ KVC_M,			"M" },
		{ KVC_N,			"N" },
		{ KVC_O,			"O" },
		{ KVC_P,			"P" },
		{ KVC_Q,			"Q" },
		{ KVC_R,			"R" },
		{ KVC_S,			"S" },
		{ KVC_T,			"T" },
		{ KVC_U,			"U" },
		{ KVC_V,			"V" },
		{ KVC_W,			"W" },
		{ KVC_X,			"X" },
		{ KVC_Y,			"Y" },
		{ KVC_Z,			"Z" },
		
		{ KVC_F1,			"F1" },
		{ KVC_F2,			"F2" },
		{ KVC_F3,			"F3" },
		{ KVC_F4,			"F4" },
		{ KVC_F5,			"F5" },
		{ KVC_F6,			"F6" },
		{ KVC_F7,			"F7" },
		{ KVC_F8,			"F8" },
		{ KVC_F9,			"F9" },
		{ KVC_F10,			"F10" },
		{ KVC_F11,			"F11" },
		{ KVC_F12,			"F12" },
		
		{ KVC_MINUS,		"-" },
		{ KVC_CARET,		"^" },
		{ KVC_BACKSPACE,	"BackSpace" },
		{ KVC_AT,			"@" },
		{ KVC_LBRACKET,		"[" },
		{ KVC_SEMICOLON,	";" },
		{ KVC_COLON,		":" },
		{ KVC_COMMA,		"," },
		{ KVC_PERIOD,		"." },
		{ KVC_SLASH,		"/" },
		{ KVC_SPACE,		"Space" },
		
		{ KVC_ESC,			"ESC" },
		{ KVC_HANZEN,		"���p/�S�p" },
		{ KVC_TAB,			"Tab" },
		{ KVC_CAPSLOCK,		"CapsLock" },
		{ KVC_ENTER,		"Enter" },
		{ KVC_LCTRL,		"L-Ctrl" },
		{ KVC_RCTRL,		"R-Ctrl" },
		{ KVC_LSHIFT,		"L-Shift" },
		{ KVC_RSHIFT,		"R-Shift" },
		{ KVC_LALT,			"L-Alt" },
		{ KVC_RALT,			"R-Alt" },
		{ KVC_PRINT,		"PrintScreen" },
		{ KVC_SCROLLLOCK,	"ScrollLock" },
		{ KVC_PAUSE,		"Pause" },
		{ KVC_INSERT,		"Insert" },
		{ KVC_DELETE,		"Delete" },
		{ KVC_END,			"End" },
		{ KVC_HOME,			"Home" },
		{ KVC_PAGEUP,		"PageUp" },
		{ KVC_PAGEDOWN,		"PageDown" },
		
		{ KVC_UP,			"��" },
		{ KVC_DOWN,			"��" },
		{ KVC_LEFT,			"��" },
		{ KVC_RIGHT,		"��" },
		
		{ KVC_P0,			"0(�e���L�[)" },
		{ KVC_P1,			"1(�e���L�[)" },
		{ KVC_P2,			"2(�e���L�[)" },
		{ KVC_P3,			"3(�e���L�[)" },
		{ KVC_P4,			"4(�e���L�[)" },
		{ KVC_P5,			"5(�e���L�[)" },
		{ KVC_P6,			"6(�e���L�[)" },
		{ KVC_P7,			"7(�e���L�[)" },
		{ KVC_P8,			"8(�e���L�[)" },
		{ KVC_P9,			"9(�e���L�[)" },
		{ KVC_NUMLOCK,		"NumLock" },
		{ KVC_P_PLUS,		"+(�e���L�[)" },
		{ KVC_P_MINUS,		"-(�e���L�[)" },
		{ KVC_P_MULTIPLY,	"*(�e���L�[)" },
		{ KVC_P_DIVIDE,		"/(�e���L�[)" },
		{ KVC_P_PERIOD,		".(�e���L�[)" },
		{ KVC_P_ENTER,		"Enter(�e���L�[)" },
		
		// ���{��L�[�{�[�h�̂�
		{ KVC_YEN,			"\\" },
		{ KVC_RBRACKET,		"]" },
		{ KVC_UNDERSCORE,	"_" },
		{ KVC_MUHENKAN,		"���ϊ�" },
		{ KVC_HENKAN,		"�ϊ�" },
		{ KVC_HIRAGANA,		"�Ђ炪��" },
		
		// �p��L�[�{�[�h�̂�
		{ KVE_BACKSLASH,	"BackSlash" },
		
		// �ǉ��L�[
		{ KVX_RMETA,		"L-Windows" },
		{ KVX_LMETA,		"R-Windows" },
		{ KVX_MENU,			"Menu" }
	};
	
	const char *str = NULL;
	for( int i=0; i<(int)(sizeof(kname)/sizeof(PCKeyName)); i++ ){
		if( kname[i].PCKey == sym ){
			str = kname[i].Name;
			break;
		}
	}
	return str;
}


////////////////////////////////////////////////////////////////
// �t�H���g�t�@�C���쐬
//
// ����:	hfile		���p�t�H���g�t�@�C���p�X
//			zfile		�S�p�t�H���g�t�@�C���p�X
//			size		�����T�C�Y(���p�������s�N�Z����)
// �Ԓl:	bool		true:�쐬���� false:�쐬���s
////////////////////////////////////////////////////////////////
bool OSD_CreateFont( const char *hfile, const char *zfile, int size )
{
	int ret = 0;
	int Wscr = size * 96 * 2;
	int Hscr = size * 96 * 2;
	
	
	// �f�o�C�X�R���e�L�X�g�擾
	HDC hBmpDC = CreateCompatibleDC( NULL );
	
	// �t�H���g�쐬
	HFONT NewFont = CreateFont( size * 2, size,
								FW_DONTCARE, FW_DONTCARE, FW_REGULAR,
								false, false, false,
								SHIFTJIS_CHARSET,
								OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								DEFAULT_QUALITY,
								FF_MODERN | FIXED_PITCH, "" );
	HFONT OldFont = (HFONT)SelectObject( hBmpDC, NewFont );
	SetTextColor( hBmpDC, RGB(255,255,255) );
	SetBkColor(   hBmpDC, RGB(  0,  0,  0) );
	
	// ��Ɨp�r�b�g�}�b�v�쐬
	BYTE *ppixel;	// �s�N�Z���f�[�^
	char bmbuf[sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*2];
	BITMAPINFO &bi        = *(BITMAPINFO *)&bmbuf;
	BITMAPINFOHEADER &bih = bi.bmiHeader;
	ZeroMemory( &bih, sizeof(bmbuf) );
	
	RGBQUAD *bc       = bi.bmiColors;
	bc[1].rgbBlue     = 255;
	bc[1].rgbGreen    = 255;
	bc[1].rgbRed      = 255;
	bc[1].rgbReserved = 0;
	
	bih.biSize        = sizeof(BITMAPINFOHEADER);
	bih.biWidth       = Wscr;
	bih.biHeight      = -Hscr;
	bih.biPlanes      = 1;
	bih.biBitCount    = 1;
	bih.biCompression = BI_RGB;
	
	HBITMAP hBmp = CreateDIBSection( NULL, (BITMAPINFO*)&bi, DIB_RGB_COLORS, (void **)&ppixel, 0, 0 );
	ZeroMemory( ppixel, Wscr * Hscr * bih.biBitCount / 8 );
	
	SelectObject( hBmpDC, hBmp );
	SelectObject( hBmpDC, GetStockObject( BLACK_BRUSH ) );
	
	// �t�H���g�`��
	BYTE strk[2] = { 0x00, 0x00 };
	// ���p
	if( hfile ){
		Rectangle( hBmpDC, 0, 0, Wscr, Hscr );
		for( int y=0; y<2; y++ ){
			for( int x=32; x<128; x++ ){
				strk[0] = x + y * 128;
				TextOut( hBmpDC, x*size, y*size*2, (char *)strk, 1 );
			}
		}
		if( !SaveImgData( hfile, ppixel, 1, Wscr, size*2*2, NULL ) ) ret++;
	}
	
	// �S�p
	if( zfile ){
		Rectangle( hBmpDC, 0, 0, Wscr, Hscr );
		for( int y=1; y<95; y++ ){
			for( int x=1; x<95; x++ ){
				strk[0] = y + 0x20;
				strk[1] = x + 0x20;
				Jis2Sjis( &strk[0], &strk[1] );
				TextOut( hBmpDC, x*size*2, y*size*2, (char *)strk, 2 );
			}
		}
		if( !SaveImgData( zfile, ppixel, 1, Wscr, Hscr, NULL ) ) ret++;
	}
	
	SelectObject( hBmpDC, OldFont );
	DeleteObject( NewFont );
	DeleteObject( hBmp );
	DeleteDC( hBmpDC );
	
	if( ret ) return false;
	else	  return true;
}

