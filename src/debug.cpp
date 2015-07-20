#include <stdlib.h>
#include <string.h>

#include "pc6001v.h"

#include "breakpoint.h"
#include "debug.h"
#include "osd.h"
#include "schedule.h"

#include "p6el.h"
#include "p6vm.h"


#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define	SCRWINW		(380)
#define	SCRWINH		(250)

#define	REGWINW		(40)
#define	REGWINH		( 8)
#define	MEMWINW		(72)
#define	MEMWINH		(31)
#define	MONWINW		(60)
#define	MONWINH		(30)

#define	PROMPT		"P6V>"

//------------------------------------------------------
//  ���j�^���[�h�E�B���h�E �C���^�[�t�F�[�X(?)�N���X
//------------------------------------------------------
////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
iMon::iMon( VM6 *vm, const ID& id ) : Device(vm,id)
{
	x = y = 0;
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
iMon::~iMon( void ){}


////////////////////////////////////////////////////////////////
// X���W�擾
////////////////////////////////////////////////////////////////
int iMon::X( void )
{
	return x;
}


////////////////////////////////////////////////////////////////
// Y���W�擾
////////////////////////////////////////////////////////////////
int iMon::Y( void )
{
	return y;
}


////////////////////////////////////////////////////////////////
// X���W�ݒ�
////////////////////////////////////////////////////////////////
void iMon::SetX( int xx )
{
	x = xx;
}


////////////////////////////////////////////////////////////////
// Y���W�ݒ�
////////////////////////////////////////////////////////////////
void iMon::SetY( int yy )
{
	x = yy;
}



//------------------------------------------------------
//  �������_���v�E�B���h�E�N���X
//------------------------------------------------------
////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cWndMem::cWndMem( VM6 *vm, const ID& id ) : iMon(vm,id), Addr(0) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cWndMem::~cWndMem( void ){}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool cWndMem::Init( void )
{
	// �\���A�h���X������
	Addr = 0;
	
	return ZCons::Init( MEMWINW, MEMWINH, "MEMORY" );
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�X�V
////////////////////////////////////////////////////////////////
void cWndMem::Update( void )
{
	WORD addr = Addr;
	static DWORD TexCol[] = { FC_GRAY, FC_BLUE, FC_GREEN, FC_CYAN, FC_RED, FC_MAGENTA, FC_YELLOW, FC_WHITE };
	int i,j;
	
	ZCons::Locate( 0, 0 );
	ZCons::SetColor( FC_YELLOW, FC_DYELLOW );
	ZCons::Printf( "MAP " );
	ZCons::SetColor( FC_DCYAN, FC_BLACK );
	ZCons::Printf( "  0000   2000   4000   6000   8000   A000   C000   E000\n" );
	ZCons::SetColor( FC_WHITE );
	
	// Read �������u���b�N���\��
	ZCons::Printf( "Read  " );
	for( i=0; i<8; i++ ){
		ZCons::SetColor( TexCol[i] );
		ZCons::Printf( "%-7s", vm->MemGetReadMemBlk( i ) );
	}
	ZCons::Printf( "\nWrire " );
	
	// Write �������u���b�N���\��
	for( i=0; i<8; i++ ){
		ZCons::SetColor( TexCol[i] );
		ZCons::Printf( "%-7s", vm->MemGetWriteMemBlk( i ) );
	}
	ZCons::Printf( "\n" );
	
	// �������_���v�\��
	ZCons::SetColor( FC_YELLOW, FC_DYELLOW );
	ZCons::Printf( "ADDR" );
	ZCons::SetColor( FC_DCYAN, FC_BLACK );
	ZCons::Printf( " 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" );
	for( i=0; i<(ZCons::GetYline()-5)/2; i++ ){
		ZCons::Printf( "\n" );
		ZCons::SetColor( TexCol[ addr>>13 ] );
		ZCons::Printf( "%04X ", addr );
		ZCons::SetColor( FC_WHITE );
		for( j=0; j<16; j++)
			ZCons::Printf( "%02X ", vm->MemRead(addr+j) );
		for( j=0; j<16; j++)
			ZCons::PutCharH( vm->MemRead(addr+j) );
		addr += 16;
	}
	
	addr = Addr;
	ZCons::Printf( "\n" );
	ZCons::SetColor( FC_YELLOW, FC_DYELLOW );
	ZCons::Printf( "RAM " );
	ZCons::SetColor( FC_DCYAN, FC_BLACK );
	ZCons::Printf( " 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F" );
	for( i=0; i<(ZCons::GetYline()-5)/2; i++ ){
		ZCons::Printf( "\n" );
		ZCons::SetColor( TexCol[ addr>>13 ] );
		ZCons::Printf( "%04X ", addr );
		ZCons::SetColor( FC_WHITE );
		for( j=0; j<16; j++)
			ZCons::Printf( "%02X ", vm->MemReadMainRam(addr+j) );
		for( j=0; j<16; j++)
			ZCons::PutCharH( vm->MemReadMainRam(addr+j) );
		addr += 16;
	}
}


////////////////////////////////////////////////////////////////
// �\���A�h���X�ݒ�
////////////////////////////////////////////////////////////////
void cWndMem::SetAddress( WORD addr )
{
	Addr = addr & 0xfff8;
}


////////////////////////////////////////////////////////////////
// �\���A�h���X�擾
////////////////////////////////////////////////////////////////
WORD cWndMem::GetAddress( void )
{
	return Addr;
}



//------------------------------------------------------
//  ���W�X�^�E�B���h�E�N���X
//------------------------------------------------------
////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cWndReg::cWndReg( VM6 *vm, const ID& id ) : iMon(vm,id) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cWndReg::~cWndReg( void ){}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool cWndReg::Init( void )
{
	return ZCons::Init( REGWINW, REGWINH, "REGISTER" );
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�X�V
////////////////////////////////////////////////////////////////
void cWndReg::Update( void )
{
	const char flags[9] = "SZ.H.PNC";
	char fbuf[9];
	cZ80::Register reg;
	char DisCode[128];
	int i,j;
	
	// ���W�X�^�l�擾
	vm->CpumGetRegister( &reg );
	
	// �t���O�̏�Ԏ擾
	for( j=0,i=reg.AF.B.l; j<8; j++,i<<=1 ) fbuf[j] = i&0x80 ? flags[j] : '.';
	fbuf[8]='\0';
	
	// 1���C���t�A�Z���u��
	vm->CpumDisasm( DisCode, reg.PC.W );
	
	ZCons::Locate( 0, 0 ); ZCons::Print( "AF :%04X  BC :%04X  DE :%04X  HL :%04X", reg.AF.W, reg.BC.W, reg.DE.W, reg.HL.W );
	ZCons::Locate( 0, 1 ); ZCons::Print( "AF':%04X  BC':%04X  DE':%04X  HL':%04X", reg.AF1.W, reg.BC1.W, reg.DE1.W, reg.HL1.W );
	ZCons::Locate( 0, 2 ); ZCons::Print( "IX :%04X  IY :%04X  PC :%04X  SP :%04X", reg.IX.W, reg.IY.W, reg.PC.W, reg.SP.W );
	ZCons::Locate( 0, 3 ); ZCons::Print( "FLAG:[%s] I:%02X IFF:%d IM:%1d HALT:%1d",  fbuf, reg.I, reg.IFF, reg.IM, reg.Halt );
	ZCons::Locate( 0, 4 ); ZCons::Print( "--------------------------------------" );
	ZCons::Locate( 0, 5 ); ZCons::Print( "%-36s", DisCode );
}


//------------------------------------------------------
//  ���j�^�E�B���h�E�N���X
//------------------------------------------------------

////////////////////////////////////////////////////////////////
// ���߂̎�ޔ���e�[�u��
////////////////////////////////////////////////////////////////
enum MonitorJob	// �W���u
{
	MONITOR_NONE = 0,
	
	MONITOR_HELP,
	
	MONITOR_GO,
	MONITOR_TRACE,
	MONITOR_STEP,
	MONITOR_STEPALL,
	MONITOR_BREAK,
	
	MONITOR_READ,
	MONITOR_WRITE,
	MONITOR_FILL,
	MONITOR_MOVE,
	MONITOR_SEARCH,
	MONITOR_OUT,
	MONITOR_LOADMEM,
	MONITOR_SAVEMEM,
	
	MONITOR_RESET,
	MONITOR_REG,
	MONITOR_DISASM,
	
	EndofMONITOR
};

const struct{
	MonitorJob Step;
	const char *cmd;
	const char *HelpMes;
}MonitorCmd[]=
	{
	{ MONITOR_HELP,		"help",		"�w���v��\��" },
	{ MONITOR_HELP,		"?",		"    �V" },
	{ MONITOR_GO,		"go",		"���s" },
	{ MONITOR_GO,		"g",		"    �V" },
	{ MONITOR_TRACE,	"trace",	"�g���[�X���s" },
	{ MONITOR_TRACE,	"t",		"    �V" },
	{ MONITOR_STEP,		"step",		"�X�e�b�v���s" },
	{ MONITOR_STEP,		"s",		"    �V" },
	{ MONITOR_STEPALL,	"S",		"    �V" },
	{ MONITOR_BREAK,	"break",	"�u���[�N�|�C���g�ݒ�" },
	{ MONITOR_BREAK,	"b",		"    �V" },
	{ MONITOR_READ,		"read",		"��������Ǎ���" },
	{ MONITOR_WRITE,	"write",	"�������ɏ�����" },
	{ MONITOR_FILL,		"fill",		"�������𖄂߂�" },
	{ MONITOR_MOVE,		"move",		"���������ړ�" },
	{ MONITOR_SEARCH,	"search",	"������������" },
	{ MONITOR_OUT,		"out",		"�|�[�g�o��" },
	{ MONITOR_LOADMEM,	"loadmem",	"�t�@�C�����烁�����ɓǍ���" },
	{ MONITOR_SAVEMEM,	"savemem",	"����������t�@�C���ɏ�����" },
	{ MONITOR_RESET,	"reset",	"PC6001V�����Z�b�g" },
	{ MONITOR_REG,		"reg",		"CPU���W�X�^���Q��/�ݒ�" },
	{ MONITOR_DISASM,	"disasm",	"�t�A�Z���u��" },

};


////////////////////////////////////////////////////////////////
// �����̎�ޔ���e�[�u��
////////////////////////////////////////////////////////////////
enum ArgvType{
	ARGV_END	= 0x00000,
	ARGV_STR	= 0x00001,	// strings
	ARGV_PORT	= 0x00002,	// 0�`0xff
	ARGV_ADDR	= 0x00004,	// 0�`0xffff
	ARGV_NUM	= 0x00008,	// 0�`0x7fffffff
	ARGV_INT	= 0x00010,	// -0x7fffffff�`0x7fffffff
	ARGV_SIZE	= 0x00080,	// #1�`#0x7fffffff
	ARGV_REG	= 0x00400,	// RegisterName
	ARGV_BREAK	= 0x00800,	// BreakAction
	ARGV_STEP	= 0x04000,	// StepCommand
	
	EndofArgvType
};


enum ArgvName{
	// <reg>
	ARG_AF,		ARG_BC,		ARG_DE,		ARG_HL,
	ARG_IX,		ARG_IY,		ARG_SP,		ARG_PC,
	ARG_AF1,	ARG_BC1,	ARG_DE1,	ARG_HL1,
	ARG_I,		ARG_R,
	ARG_IFF,	ARG_IM,		ARG_HALT,
	
	// <action>
	//ARG_PC,
	ARG_READ,	ARG_WRITE,	ARG_IN,
	ARG_OUT,	ARG_CLEAR,
	
	// step <cmd>
	//ARG_ALL
	ARG_CALL,	ARG_JP,		ARG_REP,
	
	// reg all
	ARG_ALL,
	
	EndofArgName
};


const struct{
	const char *StrL;
	const char *StrU;
	int Type;
	int Val;
}MonitorArgv[]=
{
	// <reg>
	{ "af",		"AF",		ARGV_REG,	ARG_AF,		},
	{ "bc",		"BC",		ARGV_REG,	ARG_BC,		},
	{ "de",		"DE",		ARGV_REG,	ARG_DE,		},
	{ "hl",		"HL",		ARGV_REG,	ARG_HL,		},
	{ "ix",		"IX",		ARGV_REG,	ARG_IX,		},
	{ "iy",		"IY",		ARGV_REG,	ARG_IY,		},
	{ "sp",		"SP",		ARGV_REG,	ARG_SP,		},
	{ "pc",		"PC",		ARGV_REG,	ARG_PC,		},
	{ "af'",	"AF'",		ARGV_REG,	ARG_AF1,	},
	{ "bc'",	"BC'",		ARGV_REG,	ARG_BC1,	},
	{ "de'",	"DE'",		ARGV_REG,	ARG_DE1,	},
	{ "hl'",	"HL'",		ARGV_REG,	ARG_HL1,	},
	{ "i",		"I",		ARGV_REG,	ARG_I,		},
	{ "r",		"R",		ARGV_REG,	ARG_R,		},
	{ "iff",	"IFF",		ARGV_REG,	ARG_IFF,	},
	{ "im",		"IM",		ARGV_REG,	ARG_IM,		},
	{ "halt",	"HALT",		ARGV_REG,	ARG_HALT,	},
	
	//<action>
	{ "pc",		"PC",		ARGV_BREAK,	ARG_PC,		},
	{ "read",	"READ",		ARGV_BREAK,	ARG_READ,	},
	{ "write",	"WRITE",	ARGV_BREAK,	ARG_WRITE,	},
	{ "in",		"IN",		ARGV_BREAK,	ARG_IN,		},
	{ "out",	"OUT",		ARGV_BREAK,	ARG_OUT,	},
	{ "clear",	"CLEAR",	ARGV_BREAK,	ARG_CLEAR,	},
	
	// step
	{ "call",	"CALL",		ARGV_STEP,	ARG_CALL,	},
	{ "jp",		"JP",		ARGV_STEP,	ARG_JP,		},
	{ "rep",	"REP",		ARGV_STEP,	ARG_REP,	},
	{ "all",	"ALL",		ARGV_STEP,	ARG_ALL,	},
};


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cWndMon::cWndMon( VM6 *vm, const ID& id ) : iMon(vm,id), Argc(0), ArgvCounter(0)
{
	INITARRAY( KeyBuf, '\0' );
	for( int i=0; i<MAX_HIS; i++ ) ZeroMemory( HisBuf[i], MAX_CHRS );
	INITARRAY( Argv, NULL );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cWndMon::~cWndMon( void ){}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool cWndMon::Init( void )
{
	if( ZCons::Init( MONWINW, MONWINH, "" ) ){
		// �ŏ��������b�Z�[�W�\��
		ZCons::Printf( "***********************************************\n" );
		ZCons::Printf( "* PC6001V  - monitor mode -                   *\n" );
		ZCons::Printf( "*  help ���� ? �Ɠ��͂���ƃw���v��\�����܂� *\n" );
		ZCons::Printf( "***********************************************\n\n" );
		ZCons::Printf( PROMPT );
		return true;
	}
	
	return false;
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�X�V
////////////////////////////////////////////////////////////////
void cWndMon::Update( void ){}


////////////////////////////////////////////////////////////////
// �L�[���͏���
////////////////////////////////////////////////////////////////
void cWndMon::KeyIn( int kcode, bool shift, int ccode )
{
	static int LastKey  = KVC_ENTER;	// �O��̃L�[
	static int HisLevel = 1;			// �q�X�g�����x��
	
	switch( kcode ){		// �L�[�R�[�h
	case KVC_F6:			// ���j�^���[�h�ύX
		vm->el->ToggleMonitor();
		break;
		
	case KVC_ENTER:			// Enter
	case KVC_P_ENTER:		// Enter(�e���L�[)
		ZCons::Printf( "\n" );
		
		if( strlen( KeyBuf ) > 0 ){						// �L�[�o�b�t�@���L����
			if( stricmp( KeyBuf, HisBuf[1] ) != 0 ){	// ���O�̃q�X�g���ƈقȂ�Ȃ�
				for( int i=MAX_HIS-1; i>1; i-- ) strcpy( HisBuf[i], HisBuf[i-1] );
				strcpy( HisBuf[1], KeyBuf );			// �L�[�o�b�t�@���q�X�g���o�b�t�@�ɃR�s�[
			}
		}
		
		// �����ň�����͏���
		Exec( GetArg() );
		
		*KeyBuf = '\0';						// �L�[�o�b�t�@�N���A
		ZCons::Printf( PROMPT );
		break;
		
	case KVC_UP:			// ����
	case KVC_DOWN:			// �����
		if( LastKey == KVC_UP || LastKey == KVC_DOWN ){
			if( kcode == KVC_UP   && HisLevel < MAX_HIS-1 && strlen( HisBuf[HisLevel+1] ) > 0 ) HisLevel++;
			if( kcode == KVC_DOWN && HisLevel > 0 )                                             HisLevel--;
		}else{
			if( kcode == KVC_UP ) HisLevel = 1;
			else                  HisLevel = 0;
		}
		
		// ���̃R�}���h���C��������
		while( strlen(KeyBuf) > 0 ){
			ZCons::LocateR( -1, 0 );
			ZCons::PutCharH( ' ' );
			ZCons::LocateR( -1, 0 );
			KeyBuf[strlen(KeyBuf)-1] = (BYTE)'\0';
		}
		// �q�X�g���o�b�t�@����L�[�o�b�t�@�ɃR�s�[���ĕ\��
		strcpy( KeyBuf, HisBuf[HisLevel] );
		ZCons::Printf( KeyBuf );
		break;
		
	case KVC_BACKSPACE:		// BackSpace
		if( strlen(KeyBuf) > 0 ){
			ZCons::LocateR( -1, 0 );
			ZCons::PutCharH( ' ' );
			ZCons::LocateR( -1, 0 );
			KeyBuf[strlen(KeyBuf)-1] = (BYTE)'\0';
		}
		break;
		
	// �������E�B���h�E
	case KVC_PAGEDOWN:		// PageDown
		vm->el->memw->SetAddress( vm->el->memw->GetAddress() + ( shift ? 2048 : 16 ) );
		break;
		
	case KVC_PAGEUP:		// PageUp
		vm->el->memw->SetAddress( vm->el->memw->GetAddress() - ( shift ? 2048 : 16 ) );
		break;
		
	default:
		// �L���ȕ����R�[�h���o�b�t�@�����ӂ�Ă��Ȃ����
		if( ( ccode > 0x1f ) && ( ccode < 0x80 ) ){
			if( strlen(KeyBuf) < (MAX_CHRS-1) ){
				char *p = &KeyBuf[strlen(KeyBuf)];
				*p++ = (BYTE)ccode;
				*p   = (BYTE)'\0';
//				ZCons::PutCharH( ccode );
				ZCons::Printf( "%c", ccode );
			}
		}
	}
	
	LastKey = kcode;
	
}


////////////////////////////////////////////////////////////////
// �u���[�N�|�C���g���B
////////////////////////////////////////////////////////////////
void cWndMon::BreakIn( WORD addr )
{
	ZCons::SetColor( FC_YELLOW );
	ZCons::Printf( "\n << Break in %04XH >>", addr );
	switch( vm->BpGetType( vm->BpGetReqBPNum() ) ){
	case BPoint::BP_READ:	ZCons::Printf( " Read Memory %04XH",    vm->BpGetAddr( vm->BpGetReqBPNum() ) );	break;
	case BPoint::BP_WRITE:	ZCons::Printf( " Write Memory %04XH",   vm->BpGetAddr( vm->BpGetReqBPNum() ) );	break;
	case BPoint::BP_IN:		ZCons::Printf( " Read I/O Port %02XH",  vm->BpGetAddr( vm->BpGetReqBPNum() ) );	break;
	case BPoint::BP_OUT:	ZCons::Printf( " Write I/O Port %02XH", vm->BpGetAddr( vm->BpGetReqBPNum() ) );	break;
	default:				break;
	}
	ZCons::SetColor( FC_WHITE );
	ZCons::Printf( "\n" PROMPT );
}


////////////////////////////////////////////////////////////////
// ��������
////////////////////////////////////////////////////////////////
int cWndMon::GetArg( void )
{
	char *p = KeyBuf;
	
	ArgvCounter = 1;	// Shift()�p�J�E���^������
	Argc = 0;			// �����̌�������
	
	// ��������
	while( (*p!='\0') && (Argc<=MAX_ARGS) ){
		if( *p==' ' )
			p++;
		else{
			Argv[Argc++] = p;
			while( (*p!='\0') && (*p!=' ') )
				p++;
			if( *p==' ' ) *p++ = '\0';
		}
	}
	
	int JobNo;
	if( !Argc )	// ��s?
		JobNo = MONITOR_NONE;
	else{		// �L������?
		int i;
		for( i=0; i < COUNTOF(MonitorCmd); i++ )
				if( !strcmp( Argv[0], MonitorCmd[i].cmd ) ) break;
		
		if( i == COUNTOF( MonitorCmd ) ){	// �������߂̏ꍇ
			ZCons::SetColor( FC_RED );
			ZCons::Printf( "�����ȃR�}���h�ł� : %s\n", Argv[0] );
			ZCons::SetColor( FC_WHITE );
			JobNo = MONITOR_NONE;
		}else{								// ������ ? �̏ꍇ
			if( Argc==2 && !strcmp( Argv[1], "?" ) ){
				Help( MonitorCmd[i].Step );
				JobNo = MONITOR_NONE;
			}else{							// �ʏ�̖��߂̏ꍇ
				JobNo = MonitorCmd[i].Step;
				Shift();
			}
		}
	}
	
	return JobNo;
}


////////////////////////////////////////////////////////////////
// �����z��V�t�g
////////////////////////////////////////////////////////////////
void cWndMon::Shift( void )
{
	bool size = false;
	char *p, *chk;
	
	// ����ȏ����������
	if( ArgvCounter > MAX_ARGS || ArgvCounter >= Argc )
		argv.Type = ARGV_END;
	// �܂�����������̂ŉ��
	else{
		p = Argv[ ArgvCounter ];
		if( *p == '#' ){ size = true; p++; }
		
		argv.Type = ARGV_END;
		argv.Val  = strtol( p, &chk, 0 );
		argv.Str  = Argv[ ArgvCounter ];
		
		// ���l�̏ꍇ
		if( p!=chk && *chk=='\0' ){
			if( size ){		// #�Ŏn�܂�
				if( argv.Val <= 0 ) argv.Type = ARGV_STR;
				else                argv.Type = ARGV_SIZE;
			}else{			// ���Ŏn�܂�
				argv.Type |= ARGV_INT;
				if( argv.Val >= 0 )      argv.Type |= ARGV_NUM;
				if( argv.Val <= 0xff )   argv.Type |= ARGV_PORT;
				if( argv.Val <= 0xffff ) argv.Type |= ARGV_ADDR;
			}
		// ������̏ꍇ
		}else{
			if( size ){		// #�Ŏn�܂�
				argv.Type = ARGV_STR;
			}else{			// ���Ŏn�܂�
				for( int i=0; i<COUNTOF( MonitorArgv ); i++ ){
					if( !strcmp( p, MonitorArgv[i].StrL ) || !strcmp( p, MonitorArgv[i].StrU ) ){
						argv.Type |= MonitorArgv[i].Type;
						argv.Val   = MonitorArgv[i].Val;
					}
				}
				if( argv.Type == ARGV_END ) argv.Type = ARGV_STR;
			}
		}
		ArgvCounter++;
	}
}


////////////////////////////////////////////////////////////////
// �����G���[���b�Z�[�W����
////////////////////////////////////////////////////////////////
#define ErrorMes()													\
	do{																\
		ZCons::SetColor( FC_RED );									\
		ZCons::Printf( "�����������ł� (arg %d)\n", ArgvCounter );	\
		ZCons::SetColor( FC_WHITE );								\
		return;														\
	}while(0)


////////////////////////////////////////////////////////////////
// �R�}���h���s
////////////////////////////////////////////////////////////////
void cWndMon::Exec( int cmd )
{
// �������ꂽ�����̎�ނ��`�F�b�N
#define	ArgvIs( x )	(argv.Type & (x))

	switch( cmd ){
	case MONITOR_HELP:
	//--------------------------------------------------------------
	// help [<cmd>]
	//	�w���v��\������
	//--------------------------------------------------------------
	{
		int i;
		char *cmd = NULL;
		
		if( argv.Type != ARGV_END ){				// [cmd]
			cmd = argv.Str;
			Shift();
		}
		if( argv.Type != ARGV_END ) ErrorMes();		// �]�v�Ȉ���������΃G���[
		
		if( !cmd ){	// �����Ȃ��B�S�w���v�\��
			ZCons::Printf( "help\n" );
			for( i=0; i < COUNTOF(MonitorCmd); i++ )
				ZCons::Printf( "  %-7s %s\n", MonitorCmd[i].cmd, MonitorCmd[i].HelpMes );
			ZCons::Printf( "     ��: \"help <�R�}���h��>\" �Ɠ��͂����\n         �X�ɏڍׂȃw���v��\�����܂��B\n" );
		}else{		// �����̃R�}���h�̃w���v�\��
			for( i=0; i < COUNTOF(MonitorCmd); i++ )
				if( !strcmp( cmd, MonitorCmd[i].cmd ) ) break;
			if( i==COUNTOF(MonitorCmd) ) ErrorMes();
			Help( MonitorCmd[i].Step );
		}
		
		break;
	}
	case MONITOR_GO:
	//--------------------------------------------------------------
	//  go
	//	 ���s
	//--------------------------------------------------------------
	{
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->el->ToggleMonitor();
		
		break;
	}
	
	case MONITOR_TRACE:
	//--------------------------------------------------------------
	//  trace <step>
	//  trace #<step>
	//  �w�肵���X�e�b�v���������ς��܂Ŏ��s
	//--------------------------------------------------------------
	{
		int step = 1;
		
		if( argv.Type != ARGV_END ){
			if     ( ArgvIs( ARGV_SIZE ) ) step = argv.Val;	// [<step>]
			else if( ArgvIs( ARGV_NUM )  ) step = argv.Val;	// [#<step>]
			else                            ErrorMes();
			Shift();
		}
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		
		while( step-- ){
			vm->EventUpdate( vm->Emu() );	// 1���ߎ��s�ƃC�x���g�X�V
		}
		
//		if( CheckBreakPointPC() ) set_emumode( TRACE_BP );
//		else                      set_emumode( M_TRACE );
		
		break;
	}
	
	case MONITOR_STEP:
	//--------------------------------------------------------------
	//  step
	//  step [call] [jp] [rep] [all]
	//  1�X�e�b�v,���s
	//  CALL,DJNZ,LDIR etc �̃X�L�b�v����
	//--------------------------------------------------------------
	{
		bool call = false, jp = false, rep = false;
		BYTE code;
		WORD addr;
		char DisCode[128];
		cZ80::Register reg;
		
		while( argv.Type != ARGV_END ){
			if( ArgvIs( ARGV_STEP ) ){
				if( argv.Val == ARG_CALL )	call = true;
				if( argv.Val == ARG_JP )	jp   = true;
				if( argv.Val == ARG_REP )	rep  = true;
				if( argv.Val == ARG_ALL )	call = jp = rep = true;
				Shift();
			}else
				ErrorMes();
		}
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->CpumGetRegister( &reg );
		
		addr = reg.PC.W;
		code = vm->MemRead( addr );
		
		if( call ){
			if( code		== 0xcd ||	// CALL nn    = 11001101B
			  ( code&0xc7 ) == 0xc4 ){	// CALL cc,nn = 11ccc100B
				addr += 3;
			}
		}
		
		if( jp ){
			if( code == 0x10 ){			// DJNZ e     = 00010000B
				addr += 2;
			}
		}
		
	    if( rep ){
			if( code == 0xed ){			// LDIR/LDDR/CPIR/CPDR etc
				code = vm->MemRead( addr+1 );
				if( (code&0xf4) == 0xb0 ){
					addr += 2;
				}
			}
		}
		
		vm->CpumDisasm( DisCode, addr );
		ZCons::Printf( "%s\n", DisCode );
		
		vm->EventUpdate( vm->Emu() );	// 1���ߎ��s�ƃC�x���g�X�V
		
		break;
	}	
	case MONITOR_STEPALL:
	//--------------------------------------------------------------
	//  S
	//  step all �ɓ���
	//--------------------------------------------------------------
	{
		BYTE code;
		WORD addr;
		char DisCode[128];
		cZ80::Register reg;
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->CpumGetRegister( &reg );
		
		addr = reg.PC.W;
		code = vm->MemRead( addr );
		
		if( code		== 0xcd ||	// CALL nn    = 11001101B
		  ( code&0xc7 ) == 0xc4 ){	// CALL cc,nn = 11ccc100B
			addr += 3;
		}
		
		if( code == 0x10 ){			// DJNZ e     = 00010000B
			addr += 2;
		}
		
		if( code == 0xed ){			// LDIR/LDDR/CPIR/CPDR etc
			code = vm->MemRead( addr+1 );
			if( (code&0xf4) == 0xb0 ){
				addr += 2;
			}
		}
		
		vm->CpumDisasm( DisCode, addr );
		ZCons::Printf( "%s\n", DisCode );
		
		vm->EventUpdate( vm->Emu() );	// 1���ߎ��s�ƃC�x���g�X�V
		
		break;
	}	
	case MONITOR_BREAK:
	//--------------------------------------------------------------
	//  break [PC|READ|WRITE|IN|OUT] <addr|port> [#<No>]
	//  break CLEAR [#<No>]
	//  break
	//  �u���[�N�|�C���g�̐ݒ�^�����^�\��
	//--------------------------------------------------------------
	{
		bool show = false;
		int action = ARG_PC;
		WORD addr = 0;
		int number = 0;
		
		if( argv.Type != ARGV_END ){
			// <action>
			if( ArgvIs( ARGV_BREAK ) ){
				action = argv.Val;
				Shift();
			}
			
			// <addr|port>
			switch( action ){
			case ARG_IN:
			case ARG_OUT:
				if( !ArgvIs( ARGV_PORT ) ) ErrorMes();
				addr = argv.Val;
				Shift();
				break;
			case ARG_PC:
			case ARG_READ:
			case ARG_WRITE:
				if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
				addr = argv.Val;
				Shift();
				break;
			}
			
			// [#<No>]
			if( argv.Type != ARGV_END ){
				if( !ArgvIs( ARGV_SIZE ) ) ErrorMes();
				if( argv.Val < 1 || argv.Val > NR_BP ) ErrorMes();
				number = argv.Val - 1;
				Shift();
			}
		}else{
			show = true;
		}
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		
		if( show ){
			for( int i=0; i<NR_BP; i++ ){
				ZCons::Printf( "    #%02d  ", i+1 );
				addr = vm->BpGetAddr( i );
				switch( vm->BpGetType( i ) ){
				case BPoint::BP_NONE:
					ZCons::Printf( "-- �Ȃ� --\n" );
					break;
				case BPoint::BP_PC:
					ZCons::Printf( "PC   reach %04XH\n", addr&0xffff );
					break;
				case BPoint::BP_READ:
					ZCons::Printf( "READ  from %04XH\n", addr&0xffff );
					break;
				case BPoint::BP_WRITE:
					ZCons::Printf( "WRITE   to %04XH\n", addr&0xffff );
					break;
				case BPoint::BP_IN:
					ZCons::Printf( "INPUT from %02XH\n", addr&0xff );
					break;
				case BPoint::BP_OUT:
					ZCons::Printf( "OUTPUT  to %04XH\n", addr&0xff) ;
					break;
				default:
					break;
				}
			}
		}else{
			if( action == ARG_CLEAR ){
				vm->BpSetType( number, BPoint::BP_NONE );
				ZCons::Printf( "�u���[�N�|�C���g #%02d ���������܂��B\n", number+1 );
			}else{
				const char *s = NULL;
				
				switch( action ){
				case ARG_PC:
					vm->BpSetType( number, BPoint::BP_PC );
					s = "PC : %04XH";
					break;
				case ARG_READ:
					vm->BpSetType( number, BPoint::BP_READ );
					s = "READ : %04XH";
					break;
				case ARG_WRITE:
					vm->BpSetType( number, BPoint::BP_WRITE );
					s = "WRITE : %04XH";
					break;
				case ARG_IN:
					vm->BpSetType( number, BPoint::BP_IN );
					s = "IN : %02XH";
					break;
				case ARG_OUT:
					vm->BpSetType( number, BPoint::BP_OUT );
					s = "OUT : %02XH";
					break;
				}
				vm->BpSetAddr( number, addr );
				ZCons::Printf( "�u���[�N�|�C���g #%02d ��ݒ肵�܂��B[ ",number+1 );
				ZCons::Printf( s, addr );
				ZCons::Printf( " ]\n" );
			}
		}
		break;
	}
	case MONITOR_READ:
	//--------------------------------------------------------------
	//  read <addr>
	//  ����̃A�h���X�����[�h
	//--------------------------------------------------------------
		break;
		
	case MONITOR_WRITE:
	//--------------------------------------------------------------
	//  write <addr> <data>
	//  ����̃A�h���X�Ƀ��C�g
	//--------------------------------------------------------------
	{
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR )) ErrorMes();
		WORD addr = argv.Val;
		Shift();
		
		// <data>
		if( !ArgvIs( ARGV_INT )) ErrorMes();
		BYTE data = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->MemWrite( addr, data );
		
		ZCons::Printf( "WRITE memory [ %04XH ] <- %02X  (= %d | %+d | ", addr, (BYTE)data, (BYTE)data, (int8_t)data );
		int i,j;
		for( i=0, j=0x80; i<8; i++, j>>=1 ) ZCons::Printf( "%d", (data & j) ? 1 : 0 );
		ZCons::Printf( "B )\n");
		
		break;
	}
	
	case MONITOR_FILL:
	//--------------------------------------------------------------
	//  fill <start-addr> <end-addr> <value>
	//  fill <start-addr> #<size> <value>
	//  �������𖄂߂�
	//--------------------------------------------------------------
	{
		int start, size, value;
		
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ) size = argv.Val;
		else if( ArgvIs( ARGV_ADDR ) ) size = argv.Val - start +1;
		else                           ErrorMes();
		Shift();
		
		// <data>
		if( !ArgvIs( ARGV_INT )) ErrorMes();
		value = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		for( int i=0; i<size; i++ ) vm->MemWrite( start+i, value );
		
		break;
	}
	
	case MONITOR_MOVE:
	//--------------------------------------------------------------
	//  move <src-addr> <end-addr> <dist-addr>
	//  move <src-addr> #size      <dist-addr>
	//  �������]��
	//--------------------------------------------------------------
	{
		int start, size, dist;
		
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ) size = argv.Val;
		else if( ArgvIs( ARGV_ADDR ) ) size = argv.Val - start +1;
		else                           ErrorMes();
		Shift();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		dist = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		// �]����-�]���悪 �d�Ȃ�Ȃ�
		if( start+size <= dist ) for( int i=0; i<size; i++ )    vm->MemWrite( dist+i, vm->MemRead( start+i ) );
		// �]����-�]���悪 �d�Ȃ�
		else                     for( int i=size-1; i>=0; i-- ) vm->MemWrite( dist+i, vm->MemRead( start+i ) );
		
		break;
	}
	
	case MONITOR_SEARCH:
	//--------------------------------------------------------------
	//  search [<value> [<start-addr> <end-addr>]]
	//  search [<value> [<start-addr> #<size>]]
	//  ����̒萔 (1�o�C�g) ���T�[�`
	//--------------------------------------------------------------
		break;
		
	case MONITOR_OUT:
	//--------------------------------------------------------------
	//  out <port> <data>
	//  ����̃|�[�g�ɏo��
	//--------------------------------------------------------------
	{
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <port>
		if( !ArgvIs( ARGV_PORT )) ErrorMes();
		WORD port = argv.Val;
		Shift();
		
		// <data>
		if( !ArgvIs( ARGV_INT )) ErrorMes();
		BYTE data = argv.Val;
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->IomOut( port, data );
		
		ZCons::Printf( "OUT port [ %02XH ] <- %02X  (= %d | %+d | ", port, (BYTE)data, (BYTE)data, (int8_t)data );
		int i,j;
		for( i=0, j=0x80; i<8; i++, j>>=1 ) ZCons::Printf( "%d", (data & j) ? 1 : 0 );
		ZCons::Printf( "B )\n");
		
		break;
	}
		
	case MONITOR_LOADMEM:
	//--------------------------------------------------------------
	//  loadmem <filename> <start-addr> <end-addr>
	//  loadmem <filename> <start-addr> #<size>
	// �t�@�C�����烁�����Ƀ��[�h
	//--------------------------------------------------------------
	{
		char *fname;
		int start,size;
		FILE *fp;
		
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <filename>
		if( !ArgvIs( ARGV_STR ) ) ErrorMes();
		fname = argv.Str;
		Shift();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ) size = (argv.Val > 0xffff) ? 0xffff : argv.Val;
		else if( ArgvIs( ARGV_ADDR ) ) size = (argv.Val < start ) ? (0x10000 | argv.Val) - start + 1 : argv.Val - start + 1;
		else                           ErrorMes();
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		if( (fp=fopen( fname, "rb" )) != NULL ){
			int addr = start;
			for( int i=0; i<size; i++ ){
				BYTE dat = fgetc( fp );
				vm->MemWrite( (addr++)&0xffff, dat );
			}
			fclose( fp );
			ZCons::Printf( " Loaded [%s] -> %d bytes\n", fname, addr-start );
		}else{
			ZCons::SetColor( FC_RED );
			ZCons::Printf( "Failed : File open error\n" );
			ZCons::SetColor( FC_WHITE );
		}
		break;
	}
		
	case MONITOR_SAVEMEM:
	//--------------------------------------------------------------
	//  savemem <filename> <start-addr> <end-addr>
	//  savemem <filename> <start-addr> #<size>
	//  ���������t�@�C���ɃZ�[�u
	//--------------------------------------------------------------
	{
		char *fname;
		int start,size;
		FILE *fp;
		
		if( argv.Type == ARGV_END ) ErrorMes();
		
		// <filename>
		if( !ArgvIs( ARGV_STR ) ) ErrorMes();
		fname = argv.Str;
		Shift();
		
		// <addr>
		if( !ArgvIs( ARGV_ADDR ) ) ErrorMes();
		start = argv.Val;
		Shift();
		
		// [<addr|#size>]
		if     ( ArgvIs( ARGV_SIZE ) ) size = (argv.Val > 0xffff) ? 0xffff : argv.Val;
		else if( ArgvIs( ARGV_ADDR ) ) size = (argv.Val < start ) ? (0x10000 | argv.Val) - start + 1 : argv.Val - start + 1;
		else                           ErrorMes();
		Shift();
		
		if( argv.Type != ARGV_END ) ErrorMes();
		
		if( (fp=fopen( fname, "wb" )) != NULL ){
			int addr = start;
			for( int i=0; i<size; i++ ){
				if( fputc( vm->MemRead( (addr++)&0xffff ), fp ) == EOF ){
					ZCons::SetColor( FC_RED );
					ZCons::Printf( "Failed : Data write error\n" );
					ZCons::SetColor( FC_WHITE );
					break;
				}
			}
			fclose( fp );
			ZCons::Printf( " Saved [%s] -> %d bytes\n", fname, addr-start );
		}else{
			ZCons::SetColor( FC_RED );
			ZCons::Printf( "Failed : File open error\n" );
			ZCons::SetColor( FC_WHITE );
		}
		break;
	}
		
	case MONITOR_RESET:
	//--------------------------------------------------------------
	//  reset
	//	���Z�b�g
	//--------------------------------------------------------------
		if( argv.Type != ARGV_END ) ErrorMes();
		vm->Reset();
		
		break;
		
	case MONITOR_REG:
	//--------------------------------------------------------------
	//  reg <name> <value>
	//  ���W�X�^�̓��e��ύX
	//--------------------------------------------------------------
	{	int re = -1, val=0, i;
		const char *str;
		cZ80::Register reg;
		
		if( argv.Type != ARGV_END ){
			if( !ArgvIs( ARGV_REG )) ErrorMes();		// <name>
			re = argv.Val;
			Shift();
			if( !ArgvIs( ARGV_INT )) ErrorMes();		// <value>
			val = argv.Val;
			Shift();
		}
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->CpumGetRegister( &reg );
		
		switch( re ){
		case ARG_AF:	reg.AF.W = val;		break;
		case ARG_BC:	reg.BC.W = val;		break;
		case ARG_DE:	reg.DE.W = val;		break;
		case ARG_HL:	reg.HL.W = val;		break;
		case ARG_IX:	reg.IX.W = val;		break;
		case ARG_IY:	reg.IY.W = val;		break;
		case ARG_SP:	reg.SP.W = val;		break;
		case ARG_PC:	reg.PC.W = val;		break;
		case ARG_AF1:	reg.AF1.W = val;	break;
		case ARG_BC1:	reg.BC1.W = val;	break;
		case ARG_DE1:	reg.DE1.W = val;	break;
		case ARG_HL1:	reg.HL1.W = val;	break;
		case ARG_I:		val &= 0xff; reg.I = val;		break;
		case ARG_R:		val &= 0xff; reg.R = val;		break;
		case ARG_IFF:	if(val)   val=1; reg.IFF  = val;	break;
		case ARG_IM:	if(val>3) val=2; reg.IM   = val;	break;
		case ARG_HALT:	if(val)   val=1; reg.Halt = val;	break;
		}
		
		vm->CpumSetRegister( &reg );
		
		for( i=0; i<COUNTOF( MonitorArgv ); i++ )
			if( re == MonitorArgv[i].Val ) break;
		if( i == COUNTOF( MonitorArgv ) ) str = "";
		else                              str = MonitorArgv[i].StrU;
		ZCons::Printf( "reg %s <- %04X\n", str, val );
		
		break;
	}
		
	case MONITOR_DISASM:
	//--------------------------------------------------------------
	//  disasm [[<start-addr>][#<steps>]]
	//  �t�A�Z���u��
	//--------------------------------------------------------------
	{	static int SaveDisasmAddr = -1;
		int i, pc;
		int addr = SaveDisasmAddr;
		int step = 16;
		char DisCode[128];
		cZ80::Register reg;
		
		if( argv.Type != ARGV_END ){
			if( ArgvIs( ARGV_ADDR )){		// [<addr>]
				addr = argv.Val;
				Shift();
			}
			if( ArgvIs( ARGV_SIZE )){		// [#<step>]
				step = argv.Val;
				Shift();
			}
		}
		if( argv.Type != ARGV_END ) ErrorMes();
		
		vm->CpumGetRegister( &reg );
		if( addr == -1 ) addr = reg.PC.W;	// ADDR ���w�莞
		
		pc = 0;
		for( i=0; i<step; i++ ){
			pc += vm->CpumDisasm( DisCode, (WORD)(addr+pc) );
			ZCons::Printf( "%s\n", DisCode );
		}
		SaveDisasmAddr = ( addr + pc ) & 0xffff;
		break;
	}
	
	}
}


////////////////////////////////////////////////////////////////
// �w���v�\��
////////////////////////////////////////////////////////////////
void cWndMon::Help( int cmd )
{
	switch( cmd ){
	case MONITOR_HELP:
		ZCons::Printf(
			"  help [<cmd>]\n"
			"    �w���v��\�����܂�\n"
			"    <cmd> ... �w���v��\���������R�}���h\n"
			"              [omit]... �S�R�}���h�̊ȈՃw���v��\��\n"
		);
		break;
		
	case MONITOR_GO:
		ZCons::Printf(
			"  go\n"
			"    �v���O���������s���܂�\n"
		);
		break;
		
	case MONITOR_TRACE:
		ZCons::Printf(
			"  trace [#<steps>|<steps>]\n"
			"    execute program specityes times\n"
			"    [all omit]        ... trace some steps (previous steps)\n"
			"    #<steps>, <steps> ... step counts of trace  ( you can omit '#' )\n"
		);
		break;
		
	case MONITOR_STEP:
		ZCons::Printf(
			"  step [call][jp][rep]\n"
			"    execute program 1 time\n"
			"    [all omit] ... execute 1 step\n"
			"    call       ... not trace CALL instruction\n"
			"    jp         ... not trace DJNZ instruction\n"
			"    rep        ... not trace LD*R/CP*R/IN*R/OT*R instruction\n"
			"    CAUTION)\n"
			"         call/jp/rep are use break-point #10.\n"
		);
		break;
		
	case MONITOR_STEPALL:
		ZCons::Printf(
			"  S\n"
			"    'step all' �Ɠ���   (step���Q��)\n"
		);
		break;
		
	case MONITOR_BREAK:
		ZCons::Printf(
			"  break [<action>] <addr|port> [#<No>]\n"
			"  break CLEAR [#<No>]\n"
			"  break\n"
			"    �u���[�N�|�C���g��ݒ肵�܂�\n"
			"    [all omit]  ... �S�Ẵu���[�N�|�C���g��\��\n"
			"    <action>    ... set action of conditon PC|READ|WRITE|IN|OUT or CLEAR\n"
			"                    PC    ... break if PC reach addr\n"
			"                    READ  ... break if data is read\n"
			"                    WRITE ... break if data is written\n"
			"                    IN    ... break if data is input\n"
			"                    OUT   ... break if data is output\n"
			"                    CLEAR ... clear all break point\n"
			"                    [omit]... select PC\n"
			"    <addr|port> ... specify address or port\n"
			"                    if <action> is CLEAR, this argument is invalid\n"
			"    #<No>       ... number of break point. (#1..#10)\n"
			"                    [omit]... select #1\n"
			"                    CAUTION).. #10 is used by system\n"
		);
		break;
		
	case MONITOR_READ:
		ZCons::Printf(
			"  read <addr>\n"
			"    ��������Ǎ��݂܂�\n"
			"    <addr> ... �w��A�h���X\n"
		);
		break;
		
	case MONITOR_WRITE:
		ZCons::Printf(
			"  write <addr> <data>\n"
			"    �������ɏ����݂܂�\n"
			"    <addr> ... �w��A�h���X\n"
			"    <data> ... �����ރf�[�^\n"
		);
		break;
		
	case MONITOR_FILL:
		ZCons::Printf(
			"  fill <start-addr> <end-addr> <value>\n"
			"  fill <start-addr> #<size>    <value>\n"
			"    ���������w��l�Ŗ��߂܂�\n"
			"    <start-addr> ... �J�n�A�h���X\n"
			"    <end-addr>   ... �I���A�h���X\n"
			"    #<size>      ... �T�C�Y\n"
			"    <value>      ... �����ޒl\n"
		);
		break;
		
	case MONITOR_MOVE:
		ZCons::Printf(
			"  move <src-addr> <end-addr> <dist-addr>\n"
			"  move <src-addr> #<size>    <dist-addr>\n"
			"    ��������]�����܂�\n"
			"    <src-addr>  ... �]�����J�n�A�h���X\n"
			"    <end-addr>  ... �]�����I���A�h���X\n"
			"    #<size>     ... �]���T�C�Y\n"
			"    <dist-addr> ... �]����A�h���X\n"
		);
		break;
		
	case MONITOR_SEARCH:
		ZCons::Printf(
			"  search [<value> [<start-addr> <end-addr>]]\n"
			"  search [<value> [<start-addr> #<size>]]\n"
			"    ���������������܂�\n"
			"    <value>      ... �����l\n"
			"    <start-addr> ... �����J�n�A�h���X\n"
			"    <end-addr>   ... �����I���A�h���X\n"
			"    #<size>      ... �����T�C�Y\n"
			"    [omit-all]   ... �O��̒l�܂��͕����������\n"
		);
		break;
		
	case MONITOR_OUT:
		ZCons::Printf(
			"  out <port> <data>\n"
			"    I/O�|�[�g�ɏo�͂��܂�\n"
			"    <port> ... I/O�|�[�g�A�h���X\n"
			"    <data> ... �o�̓f�[�^\n"
		);
		break;
		
	case MONITOR_LOADMEM:
		ZCons::Printf(
			"  loadmem <filename> <start-addr> <end-addr>\n"
			"  loadmem <filename> <start-addr> #<size>\n"
			"    �t�@�C�����烁�����Ƀ��[�h���܂�\n"
			"    <filename>   ... �t�@�C����\n"
			"    <start-addr> ... ���[�h�J�n�A�h���X\n"
			"    <end-addr>   ... ���[�h�I���A�h���X\n"
			"    #<size>      ... ���[�h�T�C�Y\n"
		);
		break;
		
	case MONITOR_SAVEMEM:
		ZCons::Printf(
			"  savemem <filename> <start-addr> <end-addr>\n"
			"  savemem <filename> <start-addr> #<size>\n"
			"    �������C���[�W���t�@�C���ɃZ�[�u���܂�\n"
			"    <filename>   ... �t�@�C����\n"
			"    <start-addr> ... �Z�[�u�J�n�A�h���X\n"
			"    <end-addr>   ... �Z�[�u�I���A�h���X\n"
			"    #<size>      ... �Z�[�u�T�C�Y\n"
		);
		break;
		
	case MONITOR_RESET:
		ZCons::Printf(
			"  reset\n"
			"    PC6001V�����Z�b�g���C�A�h���X 0000H ������s���܂�\n"
		);
		break;
		
	case MONITOR_REG:
		ZCons::Printf(
			"  reg <name> <value>\n"
			"    ���W�X�^�̒l���Q�ƁC�ݒ肵�܂�\n"
			"    <name>     ... specity register name.\n"
			"                   AF|BC|DE|HL|AF'|BC'|DE'|HL'|IX|IY|SP|PC|I|R|IFF|IM\n"
			"    <value>    ... set value\n"
		);
		break;
		
	case MONITOR_DISASM:
		ZCons::Printf(
			"  disasm [[<start-addr>][#<steps>]]\n"
			"    �t�A�Z���u�����܂�\n"
			"    [all omit]   ... PC���W�X�^�A�h���X����16�X�e�b�v�����t�A�Z���u��\n"
			"    <start-addr> ... start-addr ����t�A�Z���u�����܂�\n"
			"                     [omit]... PC���W�X�^�A�h���X\n"
			"    #<steps>     ... �t�A�Z���u������X�e�b�v��\n"
			"                     [omit]... 16�X�e�b�v\n"
		);
		break;
		
	}
}







//------------------------------------------------------
//  ���j�^���[�h�N���X
//------------------------------------------------------

////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
Monitor::Monitor( VM6 *vm ) : vm(vm)
{
	for( int i=0; i<COUNTOF(dcn); i++ )
		dcn[i] = NULL;
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
Monitor::~Monitor( void )
{
	for( int i=0; i<COUNTOF(dcn); i++ )
		if( dcn[i] ) delete dcn[i];
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool Monitor::Init( void )
{
	dcn[0] = new cWndMon( vm, DEV_ID("MONW") );	// ���j�^�E�B���h�E
	dcn[1] = new cWndReg( vm, DEV_ID("REGW") );	// ���W�X�^�E�B���h�E
	dcn[2] = new cWndMem( vm, DEV_ID("MEMW") );	// �������E�B���h�E
	
	for( int i=0; i<COUNTOF(dcn); i++ )
		if( !(dcn[i] && dcn[i]->Init()) ) return false;
	
	// �ʒu���킹
	dcn[0]->SetX( 0 );								dcn[0]->SetY( SCRWINH/12 );
	dcn[1]->SetX( dcn[0]->X() + dcn[0]->Width());	dcn[1]->SetY( 0 );
	dcn[2]->SetX( dcn[1]->X() );					dcn[2]->SetY( dcn[1]->Y() + dcn[1]->Height() );
	return true;
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�X�V
////////////////////////////////////////////////////////////////
void Monitor::Update( void )
{
	for( int i=0; i<COUNTOF(dcn); i++ )
		dcn[i]->Update();
}


////////////////////////////////////////////////////////////////
// ���j�^���[�h �E�B���h�E���擾
////////////////////////////////////////////////////////////////
int Monitor::Width( void )
{
	int ww = 0;
	for( int i=0; i<COUNTOF(dcn); i++ )
		ww = max( ww, dcn[i]->X() + dcn[i]->Width() );
	
	return ww;
}


////////////////////////////////////////////////////////////////
// ���j�^���[�h �E�B���h�E�����擾
////////////////////////////////////////////////////////////////
int Monitor::Height( void )
{
	int hh = 0;
	for( int i=0; i<COUNTOF(dcn); i++ )
		hh = max( hh, dcn[i]->Y() + dcn[i]->Height() );
	
	return hh;
}




#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
