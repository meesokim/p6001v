#ifndef EVENT_H_INCLUDED
#define EVENT_H_INCLUDED

#include "typedef.h"
#include "keydef.h"


/////////////////////////////////////////////////////////////////////////////
// �C�x���g��`
// �ق�SDL����
/////////////////////////////////////////////////////////////////////////////
// �C�x���gID
typedef enum {
	EV_NOEVENT = 0,			// Unused (do not remove)
	EV_KEYDOWN,				// Keys pressed
	EV_KEYUP,				// Keys released
	EV_MOUSEBUTTONDOWN,		// Mouse button pressed
	EV_MOUSEBUTTONUP,		// Mouse button released
	EV_JOYAXISMOTION,		// Joystick axis motion
	EV_JOYBUTTONDOWN,		// Joystick button pressed
	EV_JOYBUTTONUP,			// Joystick button released
	EV_QUIT,				// User-requested quit
	EV_RESTART,
	EV_DOKOLOAD,
	EV_REPLAY,
	EV_FPSUPDATE,
	EV_DEBUGMODEBP,
	EV_DROPFILE,
	EndofEvent
} EventType;


// �L�[�{�[�h
typedef struct {
	EventType type;		// EV_KEYDOWN or EV_KEYUP
	bool state;			// true:ON false:OFF
	PCKEYsym sym;		// virtual keysym
	PCKEYmod mod;		// current key modifiers
	uint16_t unicode;	// translated character
} Event_Keyboard;

// �}�E�X�{�^��
typedef struct {
	EventType type;		// EV_MOUSEBUTTONDOWN or EV_MOUSEBUTTONUP
	MouseButton button;	// The mouse button index
	bool state;			// true:ON false:OFF
	uint16_t x, y;		// The X/Y coordinates of the mouse at press time
} Event_MouseButton;

// �W���C�X�e�B�b�N ��
typedef struct {
	EventType type;		// EV_JOYAXISMOTION
	uint8_t idx;		// The joystick device index
	uint8_t axis;		// The joystick axis index
	int16_t value;		// The axis value (range: -32768 to 32767)
} Event_JoyAxis;

// �W���C�X�e�B�b�N �{�^��
typedef struct {
	EventType type;		// EV_JOYBUTTONDOWN or EV_JOYBUTTONUP
	uint8_t idx;		// The joystick device index
	uint8_t button;		// The joystick button index
	bool state;			// true:ON false:OFF
} Event_JoyButton;

// �I��
typedef struct {
	EventType type;		// EV_QUIT
} Event_Quit;

// �ċN��
typedef struct {
	EventType type;		// EV_RESTART
} Event_Restart;

// �ǂ��ł�LOAD
typedef struct {
	EventType type;		// EV_DOKOLOAD
} Event_DOKOLoad;

// ���v���C
typedef struct {
	EventType type;		// EV_REPLAY
} Event_Replay;

// FPS�\��
typedef struct {
	EventType type;		// EV_FPSUPDATE
	int fps;
} Event_FPSUpdate;

// �u���[�N�|�C���g
typedef struct {
	EventType type;		// EV_DEBUGMODEBP
	uint16_t addr;
} Event_BreakPoint;

// Drag & Drop
typedef struct {
	EventType type;		// EV_DROPFILE
	char *file;			// �C�x���g������ɕK�����������J������
} Event_Drop;


// �C�x���g��񋤗p��
typedef union Event {
	EventType type;
	Event_Keyboard key;
	Event_MouseButton mousebt;
	Event_JoyAxis joyax;
	Event_JoyButton joybt;
	Event_Quit quit;
	Event_Restart restart;
	Event_DOKOLoad doko;
	Event_Replay replay;
	Event_FPSUpdate fps;
	Event_BreakPoint bp;
	Event_Drop drop;
} Event;


#endif	// EVENT_H_INCLUDED
