#include <stdlib.h>
#include <new>

#include "pc6001v.h"

#include "config.h"
#include "error.h"
#include "osd.h"

const P6KeyName CFG6::P6KeyNameDef[] = {	// P6�L�[�R�[�h ���̒�`
	{ KP6_UNKNOWN,		"K6_UNKNOWN" },
	
	{ KP6_1,			"K6_1" },
	{ KP6_2,			"K6_2" },
	{ KP6_3,			"K6_3" },
	{ KP6_4,			"K6_4" },
	{ KP6_5,			"K6_5" },
	{ KP6_6,			"K6_6" },
	{ KP6_7,			"K6_7" },
	{ KP6_8,			"K6_8" },
	{ KP6_9,			"K6_9" },
	{ KP6_0,			"K6_0" },
	
	{ KP6_A,			"K6_A" },
	{ KP6_B,			"K6_B" },
	{ KP6_C,			"K6_C" },
	{ KP6_D,			"K6_D" },
	{ KP6_E,			"K6_E" },
	{ KP6_F,			"K6_F" },
	{ KP6_G,			"K6_G" },
	{ KP6_H,			"K6_H" },
	{ KP6_I,			"K6_I" },
	{ KP6_J,			"K6_J" },
	{ KP6_K,			"K6_K" },
	{ KP6_L,			"K6_L" },
	{ KP6_M,			"K6_M" },
	{ KP6_N,			"K6_N" },
	{ KP6_O,			"K6_O" },
	{ KP6_P,			"K6_P" },
	{ KP6_Q,			"K6_Q" },
	{ KP6_R,			"K6_R" },
	{ KP6_S,			"K6_S" },
	{ KP6_T,			"K6_T" },
	{ KP6_U,			"K6_U" },
	{ KP6_V,			"K6_V" },
	{ KP6_W,			"K6_W" },
	{ KP6_X,			"K6_X" },
	{ KP6_Y,			"K6_Y" },
	{ KP6_Z,			"K6_Z" },
	
	{ KP6_F1,			"K6_F1" },
	{ KP6_F2,			"K6_F2" },
	{ KP6_F3,			"K6_F3" },
	{ KP6_F4,			"K6_F4" },
	{ KP6_F5,			"K6_F5" },
	
	{ KP6_MINUS,		"K6_MINUS" },
	{ KP6_CARET,		"K6_CARET" },
	{ KP6_YEN,			"K6_YEN" },
	{ KP6_AT,			"K6_AT" },
	{ KP6_LBRACKET,		"K6_LBRACKET" },
	{ KP6_RBRACKET,		"K6_RBRACKET" },
	{ KP6_SEMICOLON,	"K6_SEMICOLON" },
	{ KP6_COLON,		"K6_COLON" },
	{ KP6_COMMA,		"K6_COMMA" },
	{ KP6_PERIOD,		"K6_PERIOD" },
	{ KP6_SLASH,		"K6_SLASH" },
	{ KP6_UNDERSCORE,	"K6_UNDERSCORE" },
	{ KP6_SPACE,		"K6_SPACE" },
	
	{ KP6_ESC,			"K6_ESC" },
	{ KP6_TAB,			"K6_TAB" },
	{ KP6_CTRL,			"K6_CTRL" },
	{ KP6_SHIFT,		"K6_SHIFT" },
	{ KP6_GRAPH,		"K6_GRAPH" },
	{ KP6_HOME,			"K6_HOME" },
	{ KP6_STOP,			"K6_STOP" },
	{ KP6_PAGE,			"K6_PAGE" },
	{ KP6_RETURN,		"K6_RETURN" },
	{ KP6_KANA,			"K6_KANA" },
	{ KP6_INS,			"K6_INS" },
	{ KP6_DEL,			"K6_DEL" },
	
	{ KP6_UP,			"K6_UP" },
	{ KP6_DOWN,			"K6_DOWN" },
	{ KP6_LEFT,			"K6_LEFT" },
	{ KP6_RIGHT,		"K6_RIGHT" },
	
	{ KP6_MODE,			"K6_MODE" },
	{ KP6_CAPS,			"K6_CAPS" },
	
	
	// �e���L�[���g��
	{ KP6_P0,			"K6_P0" },
	{ KP6_P1,			"K6_P1" },
	{ KP6_P2,			"K6_P2" },
	{ KP6_P3,			"K6_P3" },
	{ KP6_P4,			"K6_P4" },
	{ KP6_P5,			"K6_P5" },
	{ KP6_P6,			"K6_P6" },
	{ KP6_P7,			"K6_P7" },
	{ KP6_P8,			"K6_P8" },
	{ KP6_P9,			"K6_P9" },
	{ KP6_PPLUS,		"K6_PPLUS" },
	{ KP6_PMINUS,		"K6_PMINUS" },
	{ KP6_PMULTIPLY,	"K6_PMULTIPLY" },
	{ KP6_PDIVIDE,		"K6_PDIVIDE" },
	{ KP6_PPERIOD,		"K6_PPERIOD" },
	{ KP6_PRETURN,		"K6_PRETURN" },
	
	
	// �e��@�\�L�[
	{ KFN_1,			"K6_FN1" },
	{ KFN_2,			"K6_FN2" },
	{ KFN_3,			"K6_FN3" },
	{ KFN_4,			"K6_FN4" },
	{ KFN_5,			"K6_FN5" },
	{ KFN_6,			"K6_FN6" },
	{ KFN_7,			"K6_FN7" },
	{ KFN_8,			"K6_FN8" },
	{ KFN_9,			"K6_FN9" }
};


const PCKeyName CFG6::PCKeyNameDef[] = {	// ���z�L�[�R�[�h ���̒�`
	{ KVC_UNKNOWN,		"K_UNKNOWN" },
	
	{ KVC_1,			"K_1" },
	{ KVC_2,			"K_2" },
	{ KVC_3,			"K_3" },
	{ KVC_4,			"K_4" },
	{ KVC_5,			"K_5" },
	{ KVC_6,			"K_6" },
	{ KVC_7,			"K_7" },
	{ KVC_8,			"K_8" },
	{ KVC_9,			"K_9" },
	{ KVC_0,			"K_0" },
	
	{ KVC_A,			"K_A" },
	{ KVC_B,			"K_B" },
	{ KVC_C,			"K_C" },
	{ KVC_D,			"K_D" },
	{ KVC_E,			"K_E" },
	{ KVC_F,			"K_F" },
	{ KVC_G,			"K_G" },
	{ KVC_H,			"K_H" },
	{ KVC_I,			"K_I" },
	{ KVC_J,			"K_J" },
	{ KVC_K,			"K_K" },
	{ KVC_L,			"K_L" },
	{ KVC_M,			"K_M" },
	{ KVC_N,			"K_N" },
	{ KVC_O,			"K_O" },
	{ KVC_P,			"K_P" },
	{ KVC_Q,			"K_Q" },
	{ KVC_R,			"K_R" },
	{ KVC_S,			"K_S" },
	{ KVC_T,			"K_T" },
	{ KVC_U,			"K_U" },
	{ KVC_V,			"K_V" },
	{ KVC_W,			"K_W" },
	{ KVC_X,			"K_X" },
	{ KVC_Y,			"K_Y" },
	{ KVC_Z,			"K_Z" },
	
	{ KVC_F1,			"K_F1" },
	{ KVC_F2,			"K_F2" },
	{ KVC_F3,			"K_F3" },
	{ KVC_F4,			"K_F4" },
	{ KVC_F5,			"K_F5" },
	{ KVC_F6,			"K_F6" },
	{ KVC_F7,			"K_F7" },
	{ KVC_F8,			"K_F8" },
	{ KVC_F9,			"K_F9" },
	{ KVC_F10,			"K_F10" },
	{ KVC_F11,			"K_F11" },
	{ KVC_F12,			"K_F12" },
	
	{ KVC_MINUS,		"K_MINUS" },
	{ KVC_CARET,		"K_CARET" },
	{ KVC_BACKSPACE,	"K_BACKSPACE" },
	{ KVC_AT,			"K_AT" },
	{ KVC_LBRACKET,		"K_LBRACKET" },
	{ KVC_SEMICOLON,	"K_SEMICOLON" },
	{ KVC_COLON,		"K_COLON" },
	{ KVC_COMMA,		"K_COMMA" },
	{ KVC_PERIOD,		"K_PERIOD" },
	{ KVC_SLASH,		"K_SLASH" },
	{ KVC_SPACE,		"K_SPACE" },
	
	{ KVC_ESC,			"K_ESC" },
	{ KVC_HANZEN,		"K_HANZEN" },
	{ KVC_TAB,			"K_TAB" },
	{ KVC_CAPSLOCK,		"K_CAPSLOCK" },
	{ KVC_ENTER,		"K_ENTER" },
	{ KVC_LCTRL,		"K_LCTRL" },
	{ KVC_RCTRL,		"K_RCTRL" },
	{ KVC_LSHIFT,		"K_LSHIFT" },
	{ KVC_RSHIFT,		"K_RSHIFT" },
	{ KVC_LALT,			"K_LALT" },
	{ KVC_RALT,			"K_RALT" },
	{ KVC_PRINT,		"K_PRINT" },
	{ KVC_SCROLLLOCK,	"K_SCROLLLOCK" },
	{ KVC_PAUSE,		"K_PAUSE" },
	{ KVC_INSERT,		"K_INSERT" },
	{ KVC_DELETE,		"K_DELETE" },
	{ KVC_HOME,			"K_HOME" },
	{ KVC_END,			"K_END" },
	{ KVC_PAGEUP,		"K_PAGEUP" },
	{ KVC_PAGEDOWN,		"K_PAGEDOWN" },
	
	{ KVC_UP,			"K_UP" },
	{ KVC_DOWN,			"K_DOWN" },
	{ KVC_LEFT,			"K_LEFT" },
	{ KVC_RIGHT,		"K_RIGHT" },
	
	{ KVC_P0,			"K_P_0" },
	{ KVC_P1,			"K_P_1" },
	{ KVC_P2,			"K_P_2" },
	{ KVC_P3,			"K_P_3" },
	{ KVC_P4,			"K_P_4" },
	{ KVC_P5,			"K_P_5" },
	{ KVC_P6,			"K_P_6" },
	{ KVC_P7,			"K_P_7" },
	{ KVC_P8,			"K_P_8" },
	{ KVC_P9,			"K_P_9" },
	{ KVC_NUMLOCK,		"K_NUMLOCK" },
	{ KVC_P_PLUS,		"K_P_PLUS" },
	{ KVC_P_MINUS,		"K_P_MINUS" },
	{ KVC_P_MULTIPLY,	"K_P_MULTIPLY" },
	{ KVC_P_DIVIDE,		"K_P_DIVIDE" },
	{ KVC_P_PERIOD,		"K_P_PERIOD" },
	{ KVC_P_ENTER,		"K_P_ENTER" },
	
	// ���{��L�[�{�[�h�̂�
	{ KVC_YEN,			"K_YEN" },
	{ KVC_RBRACKET,		"K_RBRACKET" },
	{ KVC_UNDERSCORE,	"K_UNDERSCORE" },
	{ KVC_MUHENKAN,		"K_MUHENKAN" },
	{ KVC_HENKAN,		"K_HENKAN" },
	{ KVC_HIRAGANA,		"K_HIRAGANA" },
	
	// �p��L�[�{�[�h�̂�
	{ KVE_BACKSLASH,	"K_BACKSLASH" },
	
	// �ǉ��L�[
	{ KVX_RMETA,		"K_RMETA" },
	{ KVX_LMETA,		"K_LMETA" },
	{ KVX_MENU,			"K_MENU" },
};


const VKeyConv CFG6::KeyIni[] = {	// ���z�L�[�R�[�h -> P6�L�[�R�[�h��`�����l
	{ KVC_1,			KP6_1 },			// 1	!
	{ KVC_2,			KP6_2 },			// 2	"
	{ KVC_3,			KP6_3 },			// 3	#
	{ KVC_4,			KP6_4 },			// 4	$
	{ KVC_5,			KP6_5 },			// 5	%
	{ KVC_6,			KP6_6 },			// 6	&
	{ KVC_7,			KP6_7 },			// 7	'
	{ KVC_8,			KP6_8 },			// 8	(
	{ KVC_9,			KP6_9 },			// 9	)
	{ KVC_0,			KP6_0 },			// 0
	
	{ KVC_A,			KP6_A },			// a	A
	{ KVC_B,			KP6_B },			// b	B
	{ KVC_C,			KP6_C },			// c	C
	{ KVC_D,			KP6_D },			// d	D
	{ KVC_E,			KP6_E },			// e	E
	{ KVC_F,			KP6_F },			// f	F
	{ KVC_G,			KP6_G },			// g	G
	{ KVC_H,			KP6_H },			// h	H
	{ KVC_I,			KP6_I },			// i	I
	{ KVC_J,			KP6_J },			// j	J
	{ KVC_K,			KP6_K },			// k	K
	{ KVC_L,			KP6_L },			// l	L
	{ KVC_M,			KP6_M },			// m	M
	{ KVC_N,			KP6_N },			// n	N
	{ KVC_O,			KP6_O },			// o	O
	{ KVC_P,			KP6_P },			// p	P
	{ KVC_Q,			KP6_Q },			// q	Q
	{ KVC_R,			KP6_R },			// r	R
	{ KVC_S,			KP6_S },			// s	S
	{ KVC_T,			KP6_T },			// t	T
	{ KVC_U,			KP6_U },			// u	U
	{ KVC_V,			KP6_V },			// v	V
	{ KVC_W,			KP6_W },			// w	W
	{ KVC_X,			KP6_X },			// x	X
	{ KVC_Y,			KP6_Y },			// y	Y
	{ KVC_Z,			KP6_Z },			// z	Z
	
	{ KVC_F1,			KP6_F1 },			// F1
	{ KVC_F2,			KP6_F2 },			// F2
	{ KVC_F3,			KP6_F3 },			// F3
	{ KVC_F4,			KP6_F4 },			// F4
	{ KVC_F5,			KP6_F5 },			// F5
	
	{ KVC_MINUS,		KP6_MINUS },		// -	=
	{ KVC_CARET,		KP6_CARET },		// ^	~
	{ KVC_BACKSPACE,	KP6_DEL },			// BackSpace
	{ KVC_AT,			KP6_AT },			// @	`
	{ KVC_LBRACKET,		KP6_LBRACKET },		// [	{
	{ KVC_SEMICOLON,	KP6_SEMICOLON },	// ;	+
	{ KVC_COLON,		KP6_COLON },		// :	*
	{ KVC_COMMA,		KP6_COMMA },		// ,	<
	{ KVC_PERIOD,		KP6_PERIOD },		// .	>
	{ KVC_SLASH,		KP6_SLASH },		// /	?
	{ KVC_SPACE,		KP6_SPACE },		// Space
	
	{ KVC_ESC,			KP6_ESC },			// ESC
	{ KVC_HANZEN,		KP6_UNKNOWN },		// ���p/�S�p
	{ KVC_TAB,			KP6_TAB },			// Tab
	{ KVC_CAPSLOCK,		KP6_UNKNOWN },		// CapsLock
	{ KVC_ENTER,		KP6_RETURN },		// Enter
	{ KVC_LCTRL,		KP6_CTRL },			// L-Ctrl
	{ KVC_RCTRL,		KP6_CTRL },			// R-Ctrl
	{ KVC_LSHIFT,		KP6_SHIFT },		// L-Shift
	{ KVC_RSHIFT,		KP6_SHIFT },		// R-Shift
	{ KVC_LALT,			KP6_GRAPH },		// L-Alt
	{ KVC_RALT,			KP6_GRAPH },		// R-Alt
	{ KVC_PRINT,		KP6_UNKNOWN },		// PrintScreen
	{ KVC_SCROLLLOCK,	KP6_CAPS },			// ScrollLock
	{ KVC_PAUSE,		KP6_KANA },			// Pause
	{ KVC_INSERT,		KP6_INS },			// Insert
	{ KVC_DELETE,		KP6_DEL },			// Delete
	{ KVC_END,			KP6_STOP },			// End
	{ KVC_HOME,			KP6_HOME },			// Home
	{ KVC_PAGEUP,		KP6_PAGE },			// PageUp
	{ KVC_PAGEDOWN,		KP6_MODE },			// PageDown
	
	{ KVC_UP,			KP6_UP },			// ��
	{ KVC_DOWN,			KP6_DOWN },			// ��
	{ KVC_LEFT,			KP6_LEFT },			// ��
	{ KVC_RIGHT,		KP6_RIGHT },		// ��
	
	{ KVC_P0,			KP6_P0 },			// [0]
	{ KVC_P1,			KP6_P1 },			// [1]
	{ KVC_P2,			KP6_P2 },			// [2]
	{ KVC_P3,			KP6_P3 },			// [3]
	{ KVC_P4,			KP6_P4 },			// [4]
	{ KVC_P5,			KP6_P5 },			// [5]
	{ KVC_P6,			KP6_P6 },			// [6]
	{ KVC_P7,			KP6_P7 },			// [7]
	{ KVC_P8,			KP6_P8 },			// [8]
	{ KVC_P9,			KP6_P9 },			// [9]
	{ KVC_NUMLOCK,		KP6_UNKNOWN },		// NumLock
	{ KVC_P_PLUS,		KP6_PPLUS },		// [+]
	{ KVC_P_MINUS,		KP6_PMINUS },		// [-]
	{ KVC_P_MULTIPLY,	KP6_PMULTIPLY },	// [*]
	{ KVC_P_DIVIDE,		KP6_PDIVIDE },		// [/]
	{ KVC_P_PERIOD,		KP6_PPERIOD },		// [.]
	{ KVC_P_ENTER,		KP6_PRETURN },		// [Enter]
	
	// ���{��L�[�{�[�h�̂�
	{ KVC_YEN,			KP6_YEN },			// ��	|
	{ KVC_RBRACKET,		KP6_RBRACKET },		// ]	}
	{ KVC_UNDERSCORE,	KP6_UNDERSCORE },	// \	_
//	{ KVC_MUHENKAN,		KP6_UNKNOWN },		// ���ϊ�
//	{ KVC_HENKAN,		KP6_UNKNOWN },		// �ϊ�
//	{ KVC_HIRAGANA,		KP6_UNKNOWN },		// �Ђ炪��
	
	// �p��L�[�{�[�h�̂�
	{ KVE_BACKSLASH,	KP6_YEN	 },			// 			BackSlash	|
	
	// �ǉ��L�[
	{ KVX_RMETA,		KP6_UNKNOWN },		// L-Meta
	{ KVX_LMETA,		KP6_UNKNOWN },		// R-Meta
	{ KVX_MENU,			KP6_UNKNOWN },		// Menu
	
	// �e��@�\�L�[ (���̂Ƃ��떳��)
//	{ KVC_F6,			KFN_1 },			// F6
//	{ KVC_F7,			KFN_2 },			// F7
//	{ KVC_F8,			KFN_3 },			// F8
//	{ KVC_F9,			KFN_4 },			// F9
//	{ KVC_F10,			KFN_5 },			// F10
//	{ KVC_F11,			KFN_6 },			// F11
//	{ KVC_F12,			KFN_7 },			// F12
	
	{ KVC_LAST,			KP6_LAST }
};


const COLOR24 CFG6::STDColor[] = {	// �W���J���[�f�[�^ ( R,G,B,0  0-255 )
				// �V�X�e���J���[
				{   0,   0,   0, 0 },	// 00:
				{   0,   0, 128, 0 },	// 01:
				{   0, 128,   0, 0 },	// 02:
				{   0, 128, 128, 0 },	// 03:
				{ 128,   0,   0, 0 },	// 04:
				{ 128,   0, 128, 0 },	// 05:
				{ 128, 128,   0, 0 },	// 06:
				{ 128, 128, 128, 0 },	// 07:
				{  64,  64,  64, 0 },	// 08:
				{   0,   0, 255, 0 },	// 09:
				{   0, 255,   0, 0 },	// 10:
				{   0, 255, 255, 0 },	// 11:
				{ 255,   0,   0, 0 },	// 12:
				{ 255,   0, 255, 0 },	// 13:
				{ 255, 255,   0, 0 },	// 14:
				{ 255, 255, 255, 0 },	// 15:
				
				{  30,  30,  30, 0 },	// 16:��(mode 1,2 �{�[�_�[)
				
				// mode 1
				{  50, 238,  15, 0 },	// 17:��(Set1)
				{   4, 106,   4, 0 },	// 18:�[��
				{ 252, 100,  35, 0 },	// 19:��(Set2)
				{ 180,   6,   4, 0 },	// 20:�[��
				// mode 2
				{  50, 238,  15, 0 },	// 21:��
				{ 234, 224,   4, 0 },	// 22:��
				{  68,  10, 244, 0 },	// 23:��
				{ 226,   6,  12, 0 },	// 24:��
				{ 214, 208, 246, 0 },	// 25:��
				{  52, 186, 236, 0 },	// 26:�V�A��
				{ 244,  10, 244, 0 },	// 27:�}�[���^
				{ 252,  90,   4, 0 },	// 28:��
				// mode 3
				{  50, 238,  15, 0 },	// 29:��
				{ 234, 224,   4, 0 },	// 30:��
				{  68,  10, 244, 0 },	// 31:��
				{ 226,   6,  12, 0 },	// 32:��
				{ 214, 208, 246, 0 },	// 33:��
				{  52, 186, 236, 0 },	// 34:�V�A��
				{ 244,  10, 244, 0 },	// 35:�}�[���^
				{ 252,  90,   4, 0 },	// 36:��
				// mode 4
				{   4, 106,   4, 0 },	// 37:�[��(Set1)
				{  83, 242,  55, 0 },	// 38:��
				{  30,  30,  30, 0 },	// 39:��(Set2)
				{ 224, 255, 208, 0 },	// 40:��
				
				{ 146,  82,  13, 0 },	// 41:�ɂ��� ��(Set1)
				{  28, 202, 121, 0 },	// 42:�ɂ��� ��(Set1)
				{ 120,  60,  95, 0 },	// 43:�ɂ��� ��(Set1)
				{  80, 190,  80, 0 },	// 44:�ɂ��� ��(Set1)
				
				{  81, 192,  14, 0 },	// 45:�ɂ��� ����(Set1)
				{  69, 130,   9, 0 },	// 46:�ɂ��� �Ð�(Set1)
				{  21, 221,  47, 0 },	// 47:�ɂ��� ����(Set1)
				{  14, 156,  50, 0 },	// 48:�ɂ��� �Ð�(Set1)
				
				{ 113, 210,  15, 0 },	// 49:�ɂ��� ����(Set1)
				{  52, 132,  38, 0 },	// 50:�ɂ��� �Ó�(Set1)
				{  70, 200,  60, 0 },	// 51:�ɂ��� ����(Set1)
				{   4, 145,  47, 0 },	// 52:�ɂ��� �×�(Set1)
				
				{ 255,  48,   0, 0 },	// 53:�ɂ��� ��(Set2)
				{  38, 201, 255, 0 },	// 54:�ɂ��� ��(Set2)
				{ 255,  0,  236, 0 },	// 55:�ɂ��� ��(Set2)
				{  38, 255,  92, 0 },	// 56:�ɂ��� ��(Set2)
				
				{ 255, 140,  64, 0 },	// 57:�ɂ��� ����(Set2)
				{ 150,  45,  00, 0 },	// 58:�ɂ��� �Ð�(Set2)
				{ 119, 207, 255, 0 },	// 59:�ɂ��� ����(Set2)
				{  30,  80, 150, 0 },	// 60:�ɂ��� �Ð�(Set2)
				
				{ 255,  64, 131, 0 },	// 61:�ɂ��� ����(Set2)
				{ 151,   0, 105, 0 },	// 62:�ɂ��� �Ó�(Set2)
				{ 119, 255, 167, 0 },	// 63:�ɂ��� ����(Set2)
				{  30, 151, 100, 0 },	// 64:�ɂ��� �×�(Set2)
				
				// mk2
				{  20,  20,  20, 0 },	// 65:����(��)
				{ 255, 172,   0, 0 },	// 66:��
				{   0, 255, 172, 0 },	// 67:��
				{ 172, 255,   0, 0 },	// 68:����
				{ 172,   0, 255, 0 },	// 69:��
				{ 255,   0, 172, 0 },	// 70:�Ԏ�
				{   0, 172, 255, 0 },	// 71:��F
				{ 172, 172, 172, 0 },	// 72:�D�F
				{  20,  20,  20, 0 },	// 73:��
				{ 255,   0,   0, 0 },	// 74:��
				{   0, 255,   0, 0 },	// 75:��
				{ 255, 255,   0, 0 },	// 76:��
				{   0,   0, 255, 0 },	// 77:��
				{ 255,   0, 255, 0 },	// 78:�}�[���^
				{   0, 255, 255, 0 },	// 79:�V�A��
				{ 255, 255, 255, 0 },	// 80:��
			};




////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
CFG6::CFG6( void ) : Ini(NULL)
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	, MonDisp(false)
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
{
	// INI�t�@�C���̃p�X��ݒ�
	OSD_AddPath( IniPath, OSD_GetModulePath(), CONF_FILE );
	
	INITARRAY( Caption, '\0' );		// �E�B���h�E�L���v�V����
	INITARRAY( DokoFile, '\0' );	// �ǂ��ł�SAVE�t�@�C���p�X
	INITARRAY( RomPath, '\0' );		// ROM�p�X
	INITARRAY( ExtRomPath, '\0' );	// �g��ROM�p�X
	INITARRAY( ExtRomFile, '\0' );	// �g��ROM�t�@�C����
	INITARRAY( TapePath, '\0' );	// TAPE�p�X
	INITARRAY( TapeFile, '\0' );	// TAPE�t�@�C����
	INITARRAY( SaveFile, '\0' );	// TAPE(SAVE)�t�@�C����
	INITARRAY( DiskPath, '\0' );	// DISK�p�X
	INITARRAY( DiskFile1, '\0' );	// DISK1�t�@�C����
	INITARRAY( DiskFile2, '\0' );	// DISK2�t�@�C����
	INITARRAY( ImgPath, '\0' );		// �X�N���[���V���b�g�i�[�p�X
	INITARRAY( WavePath, '\0' );	// WAVE�i�[�p�X
	INITARRAY( PrinterFile, '\0' );	// �v�����^�t�@�C����
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
CFG6::~CFG6( void )
{
	if( Ini ){
		if( GetSaveQuit() ) Ini->Write();	// INI�t�@�C���ɏ�����
		delete Ini;
	}
}


////////////////////////////////////////////////////////////////
// ������(INI�t�@�C���Ǎ���)
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool CFG6::Init( void )
{
	// INI�I�u�W�F�N�g�����݂��Ă������U�J��
	if( Ini ) delete Ini;
	
	try{
		// INI�t�@�C�����Ȃ��Ȃ�f�t�H���g�ō쐬
		if( !OSD_FileExist( IniPath ) ){
			FILE *fp = FOPENEN( IniPath, "wt" );
			if( !fp ) throw Error::IniWriteFailed;
			// �^�C�g���s���o�͂��Ĉ�U����
			fprintf( fp, MSINI_TITLE );
			fclose( fp );
			
			// INI�t�@�C�����J��
			Ini = new cIni();
			if( !Ini->Init( IniPath ) ) throw Error::IniDefault;
			InitIni( Ini, true );	// INI�I�u�W�F�N�g�����l�ݒ�
			Ini->Write();
		}else{
			// INI�t�@�C�����J��
			Ini = new cIni();
			if( !Ini->Init( IniPath ) ) throw Error::IniDefault;
			InitIni( Ini, false );	// INI�I�u�W�F�N�g�����l�ݒ�
		}
	}
	catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
		Error::SetError( Error::MemAllocFailed );
		return false;
	}
	catch( Error::Errno i ){	// ��O����
		Error::SetError( i );
		if( Ini ) delete Ini;
		Ini = NULL;
		return false;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// INI�t�@�C��������
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool CFG6::Write( void )
{
	if( Ini ){
		// INI�t�@�C���ɏ�����
		return Ini->Write();
	}
	return false;
}


////////////////////////////////////////////////////////////////
// �����o�A�N�Z�X�֐�
////////////////////////////////////////////////////////////////
// �@��擾
int CFG6::GetModel( void )
{
	int st = DEFAULT_MODEL;
	Ini->GetInt( "CONFIG", "Model", &st, st );
	return st;
}

// �@��ݒ�
void CFG6::SetModel( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_Model, "Model", "%02d", data );
}

// �E�B���h�E�L���v�V�����擾
char *CFG6::GetCaption( void )
{
	switch( GetModel() ){	// �@��擾
	case 61: sprintf( Caption, APPNAME " (" P61NAME ") Ver." VERSION ); break;
	case 62: sprintf( Caption, APPNAME " (" P62NAME ") Ver." VERSION ); break;
	case 66: sprintf( Caption, APPNAME " (" P66NAME ") Ver." VERSION ); break;
	case 64: sprintf( Caption, APPNAME " (" P64NAME ") Ver." VERSION ); break;
	case 68: sprintf( Caption, APPNAME " (" P68NAME ") Ver." VERSION ); break;
	default: sprintf( Caption, APPNAME " (" P60NAME ") Ver." VERSION );
	}
	return Caption;
}

// �ǂ��ł�SAVE�t�@�C�����擾
char *CFG6::GetDokoFile( void )
{
	return DokoFile;
}

// �ǂ��ł�SAVE�t�@�C�����ݒ�
void CFG6::SetDokoFile( const char *str )
{
	strncpy( DokoFile, str, PATH_MAX );
	OSD_DelDelimiter( DokoFile );
}

// �I�[�o�[�N���b�N���擾
int CFG6::GetOverClock( void )
{
	int st = 100;
	Ini->GetInt( "CONFIG", "OverClock", &st, st );
	return st;
}

// �I�[�o�[�N���b�N���ݒ�
void CFG6::SetOverClock( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_OverClock, "OverClock", "%d", data );
}

// ROM�p�X�擾
char *CFG6::GetRomPath( void )
{
	Ini->GetString( "PATH", "RomPath", RomPath, RomPath );
	OSD_AddDelimiter( RomPath );
	OSD_AbsolutePath( RomPath );
	return RomPath;
}

// ROM�p�X�ݒ�
void CFG6::SetRomPath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_RomPath, "RomPath", temp );
}

// �g��RAM���g���擾
bool CFG6::GetUseExtRam( void )
{
	bool st = DEFAULT_EXTRAM;
	Ini->GetTruth( "CONFIG", "ExtRam", &st, st );
	return st;
}

// �g��RAM���g���ݒ�
void CFG6::SetUseExtRam( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_ExtRam, "ExtRam", "%s", yn ? "Yes" : "No" );
}

// �g��ROM�p�X�擾
char *CFG6::GetExtRomPath( void )
{
	Ini->GetString( "PATH", "ExtRomPath", ExtRomPath, ExtRomPath );
	OSD_AddDelimiter( ExtRomPath );
	OSD_AbsolutePath( ExtRomPath );
	return ExtRomPath;
}
// �g��ROM�p�X�ݒ�
void CFG6::SetExtRomPath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_ExtRomPath, "ExtRomPath", temp );
}

// �g��ROM�t�@�C�����擾
char *CFG6::GetExtRomFile( void )
{
	Ini->GetString( "FILES", "ExtRom", ExtRomFile, ExtRomFile );
	OSD_AbsolutePath( ExtRomFile );
	return ExtRomFile;
}

// �g��ROM�t�@�C�����ݒ�
void CFG6::SetExtRomFile( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_RelativePath( temp );
	Ini->PutEntry( "FILES", MSINI_ExtRom, "ExtRom", temp );
}

// CRC�`�F�b�N�擾
bool CFG6::GetCheckCRC( void )
{
	bool st = true;
	Ini->GetTruth( "CONFIG", "CheckCRC", &st, st );
	return st;
}
// CRC�`�F�b�N�ݒ�
void CFG6::SetCheckCRC( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_CheckCRC, "CheckCRC", "%s", yn ? "Yes" : "No" );
}

// ROM�p�b�`�擾
bool CFG6::GetRomPatch( void )
{
	bool st = true;
	Ini->GetTruth( "CONFIG", "RomPatch", &st, st );
	return st;
}
// ROM�p�b�`�ݒ�
void CFG6::SetRomPatch( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_RomPatch, "RomPatch", "%s", yn ? "Yes" : "No" );
}

// �L�[���s�[�g�擾
int CFG6::GetKeyRepeat( void )
{
	int st = DEFAULT_REPEAT;
	Ini->GetInt( "KEY", "KeyRepeat", &st, st );
	return st;
}

// �L�[���s�[�g�ݒ�
void CFG6::SetKeyRepeat( int data )
{
	Ini->PutEntry( "KEY", MSINI_KeyRepeat, "KeyRepeat", "%d", data );
}

// �T���v�����O���[�g�擾
int CFG6::GetSampleRate( void )
{
	int st = DEFAULT_SAMPLE_RATE;
	Ini->GetInt( "SOUND", "SampleRate", &st, st );
	return st;
}

// �T���v�����O���[�g�ݒ�
void CFG6::SetSampleRate( int data )
{
	Ini->PutEntry( "SOUND", MSINI_SampleRate, "SampleRate", "%d", data );
}

// �T�E���h�o�b�t�@���{���擾
int CFG6::GetSoundBuffer( void )
{
	int st = SOUND_BUFFER_SIZE;
	Ini->GetInt( "SOUND", "SoundBuffer", &st, st );
	return st;
}

// �T�E���h�o�b�t�@���{���ݒ�
void CFG6::SetSoundBuffer( int data )
{
	Ini->PutEntry( "SOUND", MSINI_SoundBuffer, "SoundBuffer", "%d", data );
}

// �}�X�^�[���ʎ擾
int CFG6::GetMasterVol( void )
{
	int st = DEFAULT_MASTERVOL;
	Ini->GetInt( "SOUND", "MasterVolume", &st, st );
	return st;
}

// �}�X�^�[���ʐݒ�
void CFG6::SetMasterVol( int data )
{
	Ini->PutEntry( "SOUND", MSINI_MasterVolume, "MasterVolume", "%d", data );
}


// PSG���ʎ擾
int CFG6::GetPsgVol( void )
{
	int st = DEFAULT_PSGVOL;
	Ini->GetInt( "SOUND", "PsgVolume", &st, st );
	return st;
}

// PSG���ʐݒ�
void CFG6::SetPsgVol( int data )
{
	Ini->PutEntry( "SOUND", MSINI_PsgVolume, "PsgVolume", "%d", data );
}

// PSG LPF�J�b�g�I�t���g���擾
int CFG6::GetPsgLPF( void )
{
	int st = DEFAULT_PSGLPF;
	Ini->GetInt( "SOUND", "PsgLPF", &st, st );
	return st;
}

// PSG LPF�J�b�g�I�t���g���ݒ�
void CFG6::SetPsgLPF( int data )
{
	Ini->PutEntry( "SOUND", MSINI_PsgLPF,  "PsgLPF",  "%d", data );
}

// WAVE�p�X�擾
char *CFG6::GetWavePath( void )
{
	Ini->GetString( "PATH", "WavePath", WavePath, WavePath );
	OSD_AddDelimiter( WavePath );
	OSD_AbsolutePath( WavePath );
	return WavePath;
}

// WAVE�p�X�ݒ�
void CFG6::SetWavePath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_WavePath, "WavePath", temp );
}

// �ǂ��ł�SAVE�p�X�擾
char *CFG6::GetDokoSavePath( void )
{
	Ini->GetString( "PATH", "DokoSavePath", DokoSavePath, DokoSavePath );
	OSD_AddDelimiter( DokoSavePath );
	OSD_AbsolutePath( DokoSavePath );
	return DokoSavePath;
}

// �ǂ��ł�SAVE�p�X�ݒ�
void CFG6::SetDokoSavePath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_DokoSavePath, "DokoSavePath", temp );
}

// �����������ʎ擾
int CFG6::GetVoiceVol( void )
{
	int st = DEFAULT_VOICEVOL;
	Ini->GetInt( "SOUND", "VoiceVolume", &st, st );
	return st;
}

// �����������ʐݒ�
void CFG6::SetVoiceVol( int data )
{
	Ini->PutEntry( "SOUND", MSINI_VoiceVolume, "VoiceVolume", "%d", data );
}

// TAPE�p�X�擾
char *CFG6::GetTapePath( void )
{
	Ini->GetString( "PATH", "TapePath", TapePath, TapePath );
	OSD_AddDelimiter( TapePath );
	OSD_AbsolutePath( TapePath );
	return TapePath;
}

// TAPE�p�X�ݒ�
void CFG6::SetTapePath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_TapePath, "TapePath", temp );
}

// TAPE�t�@�C�����擾
char *CFG6::GetTapeFile( void )
{
	Ini->GetString( "FILES", "tape", TapeFile, TapeFile );
	OSD_AbsolutePath( TapeFile );
	return TapeFile;
}

// TAPE�t�@�C�����ݒ�
void CFG6::SetTapeFile( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "FILES", MSINI_tape, "tape", temp );
}

// TAPE���j�^���ʎ擾
int CFG6::GetCmtVol( void )
{
	int st = DEFAULT_TAPEVOL;
	Ini->GetInt( "SOUND", "TapeVolume", &st, st );
	return st;
}

// TAPE���j�^���ʐݒ�
void CFG6::SetCmtVol( int data )
{
	Ini->PutEntry( "SOUND", MSINI_TapeVolume, "TapeVolume", "%d", data );
}

// Turbo TAPE �L���t���O�擾
bool CFG6::GetTurboTAPE( void )
{
	bool st = DEFAULT_TURBO;
	Ini->GetTruth( "CONFIG", "TurboTAPE", &st, st );
	return st;
}

// Turbo TAPE �L���t���O�ݒ�
void CFG6::SetTurboTAPE( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_TurboTAPE, "TurboTAPE", "%s", yn ? "Yes" : "No" );
}

// Boost Up �L���t���O�擾
bool CFG6::GetBoostUp( void )
{
	bool st = DEFAULT_BOOST;
	Ini->GetTruth( "CONFIG", "BoostUp", &st, st );
	return st;
}

// BoostUp �L���t���O�ݒ�
void CFG6::SetBoostUp( bool yn )
{
	Ini->PutEntry( "CONFIG", MSINI_BoostUp, "BoostUp", "%s", yn ? "Yes" : "No" );
}

// BoostUp �ő�{��(N60���[�h)�擾
int CFG6::GetMaxBoost1( void )
{
	int st = DEFAULT_MAXBOOST60;
	Ini->GetInt( "CONFIG", "MaxBoost60", &st, st );
	return st;
}

// BoostUp �ő�{��(N60���[�h)�ݒ�
void CFG6::SetMaxBoost1( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_MaxBoost60, "MaxBoost60", "%d", data );
}

// BoostUp �ő�{��(N60m/N66���[�h)�擾
int CFG6::GetMaxBoost2( void )
{
	int st = DEFAULT_MAXBOOST62;
	Ini->GetInt( "CONFIG", "MaxBoost62", &st, st );
	return st;
}
// BoostUp �ő�{��(N60m/N66���[�h)�ݒ�
void CFG6::SetMaxBoost2( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_MaxBoost62, "MaxBoost62", "%d", data );
}

// TAPE(SAVE)�t�@�C�����擾
char *CFG6::GetSaveFile( void )
{
	Ini->GetString( "FILES", "save", SaveFile, SaveFile );
	OSD_AbsolutePath( SaveFile );
	return SaveFile;
}

// TAPE(SAVE)�t�@�C�����ݒ�
void CFG6::SetSaveFile( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "FILES", MSINI_save, "save", temp );
}

// DISK�p�X�擾
char *CFG6::GetDiskPath( void )
{
	Ini->GetString( "PATH", "DiskPath", DiskPath, DiskPath );
	OSD_AddDelimiter( DiskPath );
	OSD_AbsolutePath( DiskPath );
	return DiskPath;
}

// DISK�p�X�ݒ�
void CFG6::SetDiskPath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_DiskPath, "DiskPath", temp );
}

// DISK�t�@�C�����擾
char *CFG6::GetDiskFile( int drv )
{
	switch( drv ){
	case 1:	Ini->GetString( "FILES", "disk1", DiskFile1, DiskFile1 );
			OSD_AbsolutePath( DiskFile1 );
			return DiskFile1;
	case 2: Ini->GetString( "FILES", "disk2", DiskFile2, DiskFile2 );
			OSD_AbsolutePath( DiskFile2 );
			return DiskFile2;
	}
	return NULL;
}

// DISK�t�@�C�����ݒ�
void CFG6::SetDiskFile( int drv, const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	switch( drv ){
	case 1: Ini->PutEntry( "FILES", MSINI_disk1, "disk1", temp ); break;
	case 2: Ini->PutEntry( "FILES", MSINI_disk2, "disk2", temp ); break;
	}
}

// FDD�ڑ��䐔�擾
int CFG6::GetFddNum( void )
{
	int st = DEFAULT_FDD;
	Ini->GetInt( "CONFIG", "FDD", &st, st );
	return st;
}

// FDD�ڑ��䐔�ݒ�
void CFG6::SetFddNum( int data )
{
	Ini->PutEntry( "CONFIG", MSINI_FDD, "FDD", "%d", data );
}

// �X�N���[���V���b�g�i�[�p�X�擾
const char *CFG6::GetImgPath( void )
{
	Ini->GetString( "PATH", "ImgPath", ImgPath, ImgPath );
	OSD_AddDelimiter( ImgPath );
	OSD_AbsolutePath( ImgPath );
	return ImgPath;
}

// �X�N���[���V���b�g�i�[�p�X�ݒ�
void CFG6::SetImgPath( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "PATH", MSINI_ImgPath, "ImgPath", temp );
}

// ���[�h4�J���[���[�h�擾
int CFG6::GetMode4Color( void ) const
{
	int st = DEFAULT_MODE4_COLOR;
	Ini->GetInt( "DISPLAY", "Mode4Color", &st, st );
	return st;
}

// ���[�h4�J���[���[�h�ݒ�
void CFG6::SetMode4Color( int data )
{
	Ini->PutEntry( "DISPLAY", MSINI_Mode4Color, "Mode4Color", "%d", data );
}

// �X�L�������C���擾
bool CFG6::GetScanLine( void ) const
{
	bool st = DEFAULT_SCANLINE;
	Ini->GetTruth( "DISPLAY", "ScanLine", &st, st );
	return st;
}

// �X�L�������C���ݒ�
void CFG6::SetScanLine( bool yn )
{
	Ini->PutEntry( "DISPLAY", MSINI_ScanLine, "ScanLine", "%s", yn ? "Yes" : "No" );
}

// �X�L�������C���P�x�擾
int CFG6::GetScanLineBr( void ) const
{
	int st = DEFAULT_SCANLINEBR;
	Ini->GetInt( "DISPLAY", "ScanLineBr", &st, st );
	return st;
}

// �X�L�������C���P�x�ݒ�
void CFG6::SetScanLineBr( int data )
{
	Ini->PutEntry( "DISPLAY", MSINI_ScanLineBr, "ScanLineBr", "%d", data );
}

// 4:3�\���擾
bool CFG6::GetDispNTSC( void ) const
{
	bool st = DEFAULT_DISPNTSC;
	Ini->GetTruth( "DISPLAY", "DispNTSC", &st, st );
	return st;
}

// 4:3�\���ݒ�
void CFG6::SetDispNTSC( bool yn )
{
	Ini->PutEntry( "DISPLAY", MSINI_DispNTSC, "DispNTSC", "%s", yn ? "Yes" : "No" );
}

// �t���X�N���[���擾
bool CFG6::GetFullScreen( void ) const
{
	bool st = false;
	Ini->GetTruth( "DISPLAY", "FullScreen", &st, st );
	return st;
}

// �t���X�N���[���ݒ�
void CFG6::SetFullScreen( bool yn )
{
	Ini->PutEntry( "DISPLAY", MSINI_FullScreen, "FullScreen", "%s", yn ? "Yes" : "No" );
}

// �X�e�[�^�X�o�[�\����Ԏ擾
bool CFG6::GetDispStat( void ) const
{
	bool st = true;
	Ini->GetTruth( "DISPLAY", "DispStatus", &st, st );
	return st;
}

// �X�e�[�^�X�o�[�\����Ԑݒ�
void CFG6::SetDispStat( bool yn )
{
	Ini->PutEntry( "DISPLAY", MSINI_DispStatus, "DispStatus", "%s", yn ? "Yes" : "No" );
}

// �t���[���X�L�b�v�擾
int CFG6::GetFrameSkip( void ) const
{
	int st = DEFAULT_FRAMESKIP;
	Ini->GetInt( "DISPLAY", "FrameSkip", &st, st );
	return st;
}

// �t���[���X�L�b�v�ݒ�
void CFG6::SetFrameSkip( int data )
{
	Ini->PutEntry( "DISPLAY", MSINI_FrameSkip, "FrameSkip", "%d", data );
}

// �r�f�I�L���v�`���F�[�x�擾
int CFG6::GetAviBpp()
{
	int st = 24;
	Ini->GetInt( "MOVIE", "AviBpp", &st, st );
	return st;
}

// �r�f�I�L���v�`���F�[�x�ݒ�
void CFG6::SetAviBpp( int bpp )
{
	Ini->PutEntry( "MOVIE", MSINI_AviBpp, "AviBpp", "%d", bpp );
}

// �v�����^�t�@�C�����擾
char *CFG6::GetPrinterFile( void )
{
	Ini->GetString( "FILES", "printer", PrinterFile, PrinterFile );
	OSD_AbsolutePath( PrinterFile );
	return PrinterFile;
}

// �v�����^�t�@�C�����ݒ�
void CFG6::SetPrinterFile( const char *str )
{
	char temp[PATH_MAX];
	strncpy( temp, str, PATH_MAX );
	OSD_DelDelimiter( temp );
	OSD_RelativePath( temp );
	Ini->PutEntry( "FILES", MSINI_printer, "printer", temp );
}

// �I�����m�F�擾
bool CFG6::GetCkQuit( void )
{
	bool st = false;
	Ini->GetTruth( "CHECK", "CkQuit", &st, st );
	return st;
}

// �I�����m�F�ݒ�
void CFG6::SetCkQuit( bool yn )
{
	Ini->PutEntry( "CHECK", MSINI_CkQuit, "CkQuit", "%s", yn ? "Yes" : "No" );
}

// �I����INI�ۑ��擾
bool CFG6::GetSaveQuit( void )
{
	bool st = false;
	Ini->GetTruth( "CHECK", "SaveQuit", &st, st );
	return st;
}

// �I����INI�ۑ��ݒ�
void CFG6::SetSaveQuit( bool yn )
{
	Ini->PutEntry( "CHECK", MSINI_SaveQuit, "SaveQuit", "%s", yn ? "Yes" : "No" );
}

// ��m�̃J�[�g���b�W�g���t���O�擾
bool CFG6::GetUseSoldier()
{
	bool st = true;
	Ini->GetTruth( "OPTION", "UseSoldier", &st, st );
	return st;
}

// ��m�̃J�[�g���b�W�g���t���O�ݒ�
void CFG6::SetUseSoldier( bool yn )
{
	Ini->PutEntry( "OPTION", MSINI_UseSoldier, "UseSoldier", "%s", yn ? "Yes" : "No" );
}

// �J���[�f�[�^�擾
COLOR24 *CFG6::GetColor( int num, COLOR24 *col )
{
	if( num < (int)(sizeof(STDColor)/sizeof(STDColor[0])) ){
		char strin[16], str[PATH_MAX];
		sprintf( strin, "COL%03d", num );
		sprintf( str,   "%02X%02X%02X", STDColor[num].r, STDColor[num].g, STDColor[num].b );
		Ini->GetString( "COLOR", strin, str, str );
		int st = strtol( str, NULL, 16 );
		col->r        = (st>>16)&0xff;
		col->g        = (st>> 8)&0xff;
		col->b        = (st    )&0xff;
		col->reserved = 0;
	}else{
		col->r        = 0;
		col->g        = 0;
		col->b        = 0;
		col->reserved = 0;
	}
	return col;
}

// �J���[�f�[�^�ݒ�
void CFG6::SetColor( int num, const COLOR24 *col )
{
	char stren[16];
	sprintf( stren, "COL%03d", num );
	if( col )
		Ini->PutEntry( "COLOR", OSD_ColorName( num-16 ), stren, "%02X%02X%02X", col->r, col->g, col->b );
}

// �L�[��`�擾
P6KEYsym CFG6::GetVKey( PCKEYsym pcs )
{
	char str[PATH_MAX] = "";
	
	// �L�[�R�[�h���疼�̎擾
	const char *k1 = GetPCKeyName( pcs );
	
	if( k1 ){
		Ini->GetString( "KEY", k1, str, str );
		return GetP6KeyCode( str );
	}
	return KP6_UNKNOWN;
}

// �L�[��`�ݒ�
void CFG6::SetVKey( PCKEYsym pcs, P6KEYsym p6s )
{
	const char *k1 = GetPCKeyName( pcs );
	const char *k2 = GetP6KeyName( p6s );
	if( k1 && k2 ){
		Ini->PutEntry( "KEY", OSD_KeyName( pcs ), k1, k2 );
	}
}

// �L�[��`�z��擾
int CFG6::GetVKeyDef( VKeyConv **kdef )
{
	VKeyConv *key = NULL;
	
	try{
		// �L�[��`�e�[�u���쐬
		key = new VKeyConv[COUNTOF(KeyIni)];
		for( int i=0; i<COUNTOF(KeyIni); i++ ){
			key[i].PCKey = KeyIni[i].PCKey;
			key[i].P6Key = GetVKey( key[i].PCKey );
		}
	}
	catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
		Error::SetError( Error::MemAllocFailed );
		*kdef = NULL;
		return 0;
	}
	catch( Error::Errno i ){	// ��O����
		Error::SetError( i );
		if( key ) delete [] key;
		*kdef = NULL;
		return 0;
	}
	
	*kdef = key;
	return COUNTOF(KeyIni);
}

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ���j�^�E�B���h�E�\����Ԏ擾
bool CFG6::GetMonDisp( void ) const
{
	return MonDisp;
}

// ���j�^�E�B���h�E�\����Ԑݒ�
void CFG6::SetMonDisp( bool yn )
{
	MonDisp = yn;
}


#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


////////////////////////////////////////////////////////////////
// INI�I�u�W�F�N�g�����l�ݒ�
//
// ����:	ini		INI�I�u�W�F�N�g�ւ̃|�C���^
//			over	true:�㏑�� false:�m�[�h�����݂��Ă�����p�X
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void CFG6::InitIni( cIni *ini, bool over )
{
	if( !ini ) return;
	
	char str[PATH_MAX];
	
	// [CONFIG]
	// �@��
	if( over || !ini->GetString( "CONFIG", "Model", str, str ) )
		SetModel( DEFAULT_MODEL );
	
	// FDD
	if( over || !ini->GetString( "CONFIG", "FDD", str, str ) )
		SetFddNum( DEFAULT_FDD );
	
	// �g��RAM�g�p
	if( over || !ini->GetString( "CONFIG", "ExtRam", str, str ) )
		SetUseExtRam( DEFAULT_EXTRAM );
	
	// Turbo TAPE
	if( over || !ini->GetString( "CONFIG", "TurboTAPE", str, str ) )
		SetTurboTAPE( DEFAULT_TURBO );
	
	// Boost Up
	if( over || !ini->GetString( "CONFIG", "BoostUp", str, str ) )
		SetBoostUp( DEFAULT_BOOST );
	
	// BoostUp �ő�{��(N60���[�h)
	if( over || !ini->GetString( "CONFIG", "MaxBoost60", str, str ) )
		SetMaxBoost1( DEFAULT_MAXBOOST60 );
	
	// BoostUp �ő�{��(N60m/N66���[�h)
	if( over || !ini->GetString( "CONFIG", "MaxBoost62", str, str ) )
		SetMaxBoost2( DEFAULT_MAXBOOST62 );
	
	// �I�[�o�[�N���b�N��
	if( over || !ini->GetString( "CONFIG", "OverClock", str, str ) )
		SetOverClock( DEFAULT_OVERCLOCK );
	
	// CRC�`�F�b�N
	if( over || !ini->GetString( "CONFIG", "CheckCRC", str, str ) )
		SetCheckCRC( DEFAULT_CHECKCRC );
	
	// ROM�p�b�`
	if( over || !ini->GetString( "CONFIG", "RomPatch", str, str ) )
		SetRomPatch( DEFAULT_ROMPATCH );
	
	// [DISPLAY]
	// MODE4�J���[
	if( over || !ini->GetString( "DISPLAY", "Mode4Color", str, str ) )
		SetMode4Color( DEFAULT_MODE4_COLOR );
	
	// �X�L�������C��
	if( over || !ini->GetString( "DISPLAY", "ScanLine", str, str ) )
		SetScanLine( DEFAULT_SCANLINE );
	
	// �X�L�������C���P�x
	if( over || !ini->GetString( "DISPLAY", "ScanLineBr", str, str ) )
		SetScanLineBr( DEFAULT_SCANLINEBR );
	
	// 4:3�\��
	if( over || !ini->GetString( "DISPLAY", "DispNTSC", str, str ) )
		SetDispNTSC( DEFAULT_DISPNTSC );
	
	// �t���X�N���[��
	if( over || !ini->GetString( "DISPLAY", "FullScreen", str, str ) )
		SetFullScreen( DEFAULT_FULLSCREEN );
	
	// �X�e�[�^�X�o�[�\�����
	if( over || !ini->GetString( "DISPLAY", "DispStatus", str, str ) )
		SetDispStat( DEFAULT_DISPSTATUS );
	
	// �t���[���X�L�b�v
	if( over || !ini->GetString( "DISPLAY", "FrameSkip", str, str ) )
		SetFrameSkip( DEFAULT_FRAMESKIP );
	
	
	// [SOUND]
	// �T���v�����O���[�g
	if( over || !ini->GetString( "SOUND", "SampleRate", str, str ) )
		SetSampleRate( DEFAULT_SAMPLE_RATE );
	
	// �T�E���h�o�b�t�@���{��
	if( over || !ini->GetString( "SOUND", "SoundBuffer", str, str ) )
		SetSoundBuffer( SOUND_BUFFER_SIZE );
	
	// �}�X�^�[����
	if( over || !ini->GetString( "SOUND", "MasterVolume", str, str ) )
		SetMasterVol( DEFAULT_MASTERVOL );
	
	// PSG����
	if( over || !ini->GetString( "SOUND", "PsgVolume", str, str ) )
		SetPsgVol( DEFAULT_PSGVOL );
	
	// PSG LPF�J�b�g�I�t���g��
	if( over || !ini->GetString( "SOUND", "PsgLPF", str, str ) )
		SetPsgLPF( DEFAULT_PSGLPF );
	
	// ������������
	if( over || !ini->GetString( "SOUND", "VoiceVolume", str, str ) )
		SetVoiceVol( DEFAULT_VOICEVOL );
	
	// TAPE���j�^����
	if( over || !ini->GetString( "SOUND", "TapeVolume", str, str ) )
		SetCmtVol( DEFAULT_TAPEVOL );
	
	
	// [MOVIE]
	// �r�f�I�L���v�`���F�[�x
	if( over || !ini->GetString( "MOVIE", "AviBpp", str, str ) )
		SetAviBpp( DEFAULT_AVIBPP );
	
	
	//[FILES]
	// �g��ROM�t�@�C����(�N�����Ɏ����}�E���g)
	if( over || !ini->GetString( "FILES", "ExtRom", str, str ) )
		SetExtRomFile( "" );
	
	// TAPE�t�@�C����(�N�����Ɏ����}�E���g)
	if( over || !ini->GetString( "FILES", "tape", str, str ) )
		SetTapeFile( "" );
	
	// TAPE(SAVE)�t�@�C����(SAVE���Ɏ����}�E���g)
	if( over || !ini->GetString( "FILES", "save", str, str ) ){
		OSD_AddPath( str, TAPE_DIR, SAVE_FILE );
		OSD_AddPath( str, OSD_GetModulePath(), (const char *)str );
		SetSaveFile( str );
	}
	
	// DISK1�t�@�C����(�N�����Ɏ����}�E���g)
	if( over || !ini->GetString( "FILES", "disk1", str, str ) )
		SetDiskFile( 1, "" );
	
	// DISK2�t�@�C����(�N�����Ɏ����}�E���g)
	if( over || !ini->GetString( "FILES", "disk2", str, str ) )
		SetDiskFile( 2, "" );
	
	// �v�����^�t�@�C����
	if( over || !ini->GetString( "FILES", "printer", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), PRINTER_FILE );
		SetPrinterFile( str );
	}
	
	// [PATH]
	// ROM�p�X
	if( over || !ini->GetString( "PATH", "RomPath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), ROM_DIR );
		SetRomPath( str );
	}
	
	// TAPE�p�X
	if( over || !ini->GetString( "PATH", "TapePath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), TAPE_DIR );
		SetTapePath( str );
	}
	
	// DISK�p�X
	if( over || !ini->GetString( "PATH", "DiskPath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), DISK_DIR );
		SetDiskPath( str );
	}
	
	// �g��ROM�p�X
	if( over || !ini->GetString( "PATH", "ExtRomPath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), EXTROM_DIR );
		SetExtRomPath( str );
	}
	
	// WAVE�p�X
	if( over || !ini->GetString( "PATH", "WavePath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), WAVE_DIR );
		SetWavePath( str );
	}
	
	// IMG�p�X
	if( over || !ini->GetString( "PATH", "ImgPath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), IMAGE_DIR );
		SetImgPath( str );
	}
	
	// �ǂ��ł�SAVE�p�X
	if( over || !ini->GetString( "PATH", "DokoSavePath", str, str ) ){
		OSD_AddPath( str, OSD_GetModulePath(), DOKOSAVE_DIR );
		SetDokoSavePath( str );
	}
	
	// [CHECK]
	// �I�����m�F
	if( over || !ini->GetString( "CHECK", "CkQuit", str, str ) )
		SetCkQuit( DEFAULT_CKQUIT );
	
	// �I����INI�ۑ�
	if( over || !ini->GetString( "CHECK", "SaveQuit", str, str ) )
		SetSaveQuit( DEFAULT_SAVEQUIT );
	
	
	// [OPTION]
	// ��m�̃J�[�g���b�W�g���t���O
	if( over || !ini->GetString( "OPTION", "UseSoldier", str, str ) )
		SetUseSoldier( DEFAULT_SOLDIER );
	
	
	// [COLOR]
	// �p���b�g
	for( int i=16; i<COUNTOF(STDColor); i++ ){
		char stren[16];
		sprintf( stren, "COL%03d", i );
		if( over || !ini->GetString( "COLOR", stren, str, str ) )
			SetColor( i, &STDColor[i] );
	}
	
	// [KEY]
	// �L�[���s�[�g
	if( over || !ini->GetString( "KEY", "KeyRepeat", str, str ) )
		SetKeyRepeat( DEFAULT_REPEAT );
	
	// �L�[��`
	int i = 0;
	while( KeyIni[i].PCKey != KVC_LAST ){
		// �L�[�R�[�h���疼�̎擾
		const char *k1 = GetPCKeyName( KeyIni[i].PCKey );
		if( k1 && ( over || !ini->GetString( "KEY", k1, str, str ) ) )
			SetVKey( KeyIni[i].PCKey, KeyIni[i].P6Key );
		i++;
	}
	
}


////////////////////////////////////////////////////////////////
// ���z�L�[�R�[�h���疼�̎擾
//
// ����:	sym		���z�L�[�R�[�h
// �Ԓl:	char *	���̕�����ւ̃|�C���^(������Ȃ����NULL)
////////////////////////////////////////////////////////////////
const char *CFG6::GetPCKeyName( PCKEYsym sym )
{
	const char *str = NULL;
	for( int i=0; i<(int)(sizeof(PCKeyNameDef)/sizeof(PCKeyName)); i++ ){
		if( PCKeyNameDef[i].PCKey == sym ){
			str = PCKeyNameDef[i].Name;
			break;
		}
	}
	return str;
}


////////////////////////////////////////////////////////////////
// P6�L�[�R�[�h���疼�̎擾
//
// ����:	sym		P6�L�[�R�[�h
// �Ԓl:	char *	���̕�����ւ̃|�C���^(������Ȃ����NULL)
////////////////////////////////////////////////////////////////
const char *CFG6::GetP6KeyName( P6KEYsym sym )
{
	const char *str = NULL;
	for( int i=0; i<(int)(sizeof(P6KeyNameDef)/sizeof(P6KeyName)); i++ ){
		if( P6KeyNameDef[i].P6Key == sym ){
			str = P6KeyNameDef[i].Name;
			break;
		}
	}
	return str;
}


////////////////////////////////////////////////////////////////
// �L�[���̂��牼�z�L�[�R�[�h���擾
//
// ����:	str			���̕�����ւ̃|�C���^
// �Ԓl:	PCKEYsym	���z�L�[�R�[�h
////////////////////////////////////////////////////////////////
PCKEYsym CFG6::GetPCKeyCode( char *str )
{
	PCKEYsym sym = KVC_UNKNOWN;
	for( int i=0; i<(int)(sizeof(PCKeyNameDef)/sizeof(PCKeyName)); i++ ){
		if( !stricmp( PCKeyNameDef[i].Name, str ) ){
			sym = PCKeyNameDef[i].PCKey;
			break;
		}
	}
	return sym;
}


////////////////////////////////////////////////////////////////
// �L�[���̂���P6�L�[�R�[�h���擾
//
// ����:	str			���̕�����ւ̃|�C���^
// �Ԓl:	P6KEYsym	P6�L�[�R�[�h
////////////////////////////////////////////////////////////////
P6KEYsym CFG6::GetP6KeyCode( char *str )
{
	P6KEYsym sym = KP6_UNKNOWN;
	for( int i=0; i<(int)(sizeof(P6KeyNameDef)/sizeof(P6KeyName)); i++ ){
		if( !stricmp( P6KeyNameDef[i].Name, str ) ){
			sym = P6KeyNameDef[i].P6Key;
			break;
		}
	}
	return sym;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�SAVE
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool CFG6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	// ����
	Ini->PutEntry( "GLOBAL", NULL, "Version",		VERSION );
	Ini->PutEntry( "GLOBAL", NULL, "Model",			"%02d",	GetModel() );
	Ini->PutEntry( "GLOBAL", NULL, "FDD",			"%d",	GetFddNum() );
	Ini->PutEntry( "GLOBAL", NULL, "ExtRam",		"%s",	GetUseExtRam()  ? "Yes" : "No" );
	Ini->PutEntry( "GLOBAL", NULL, "RomPatch",		"%s",	GetRomPatch()   ? "Yes" : "No" );
	// OPTION
	Ini->PutEntry( "OPTION", NULL, "UseSoldier",	"%s",	GetUseSoldier() ? "Yes" : "No" );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool CFG6::DokoLoad( cIni *Ini )
{
	int st;
	bool yn;
	char strva[256];
	
	if( !Ini ) return false;
	
	// ����
	Ini->GetString( "GLOBAL", "Version", strva, "" );
	if( strcmp( strva, VERSION ) ){
		Error::SetError( Error::DokoDiffVersion );
		return false;
	}
	
	Ini->GetInt(   "GLOBAL", "Model",      &st, GetModel() );		SetModel( st );
	Ini->GetInt(   "GLOBAL", "FDD",        &st, GetFddNum() );		SetFddNum( st );
	Ini->GetTruth( "GLOBAL", "ExtRam",     &yn, GetUseExtRam() );	SetUseExtRam( yn );
	Ini->GetTruth( "GLOBAL", "RomPatch",   &yn, GetRomPatch() );	SetRomPatch( yn );
	Ini->GetTruth( "OPTION", "UseSoldier", &yn, GetUseSoldier() );	SetUseSoldier( yn );
	
	return true;
}
