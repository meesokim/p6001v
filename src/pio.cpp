#include "config.h"
#include "pio.h"
#include "intr.h"

#include "p6vm.h"

/*
BASIC ROM�����[�`��
								// 8049����A�Ƀf�[�^���󂯎��
Z0323:	LD	A,0CH		;E78	// WIE(6)��L�ɂ��� �܂菑���ݗv�������݋֎~ INT=RINT�Ƃ��邽��?
		OUT	(093H),A	;E7A
								// ���Ԃ񂱂̕ӂ̃^�C�~���O��8049���f�[�^���o�͂���STB(4)=L�ɂ���B
								// �����PortA�̓��b�`����,�o�b�t�@�����������̂�IBF(5)=H�ƂȂ�B
								// ����ɂ��8049�̓f�[�^�o�͂��~��,STB(4)=H�ɂ���B
								// STB(4)�̗����オ���INT(3)=H�ƂȂ�B
								// ������ IBF(5)=H -> INT(3)=H �̏��ɑ҂̂�������?
Z0319:	IN	A,(092H)	;E7C
		AND	08H			;E7E	// INT(3)��H�ɂȂ�܂ő҂�
		JR	Z,0E7CH		;E80	// �܂�8049����̃f�[�^��PortA�Ƀ��b�`�����܂ő҂�
		IN	A,(092H)	;E82
		AND	020H		;E84	// IBF(5)��H�ɂȂ�܂ő҂� ������INT=H�����H�ɂȂ��Ă�
		JR	Z,0E7CH		;E86	// �܂�PortA�̓��̓o�b�t�@�����������܂ő҂�
		LD	A,0DH		;E88	// WIE(6)��H�ɂ��� �܂菑���ݗv�������݋���
		OUT	(093H),A	;E8A
		IN	A,(090H)	;E8C	// PortA����f�[�^���󂯎��
		RET				;E8E
		
								// A�̃f�[�^��8049�֑���
Z0044:	PUSH	AF		;E8F
		LD	A,08H		;E90	// RIE(4)��L�ɂ��� �܂�Ǎ��ݗv�������݋֎~ INT=WINT�Ƃ��邽��?
		OUT	(093H),A	;E92
Z0320:	IN	A,(092H)	;E94
		AND	08H			;E96	// INT(3)��H�ɂȂ�܂ő҂�
		JR	Z,0E94H		;E98	// �܂�8049���f�[�^���󂯎���ăo�b�t�@����ɂȂ�܂ő҂� �O�񑗂����f�[�^?
		IN	A,(092H)	;E9A
		AND	080H		;E9C	// OBF(7)��H�ɂȂ�܂ő҂� ������INT=H�����H�ɂȂ��Ă�
		JR	Z,0E94H		;E9E	// �܂�8049���f�[�^���󂯎����DAK=H�ɂȂ�܂ő҂� �O�񑗂����f�[�^?
		LD	A,09H		;EA0	// RIE(4)��H�ɂ��� �܂�Ǎ��ݗv�������݋���
		OUT	(093H),A	;EA2
		POP	AF			;EA4
		OUT	(090H),A	;EA5	// PortA�Ƀf�[�^�𑗂�
		RET				;EA7


�y8255 ���[�h2�̋����z
bit3 INT(�o��)	[���͂̏ꍇ]���̏o�͂�8049����̃f�[�^��PortA(����)�Ƀ��b�`���ꂽ�Ƃ���H�ƂȂ�
				CPU�ɑ΂���f�[�^�Ǎ��ݗv�����荞�ݐM���Ƃ��ē����B
				[�o�͂̏ꍇ]���̏o�͂�8049���f�[�^���󂯎��������H�ƂȂ�,CPU�ɑ΂���
				���̃f�[�^�̏����ݗv�����荞�ݐM���Ƃ��ē����B

bit4 STB(����)	���̓��͂�L�ɂ����8049����PortA�ɑ����Ă���M����PortA�����b�`����
				[����]IBF��L�ɂȂ�O��STB��L�ɂȂ�ƃ|�[�g�����b�`�f�[�^���ω����Ă��܂��B
				�@�@�@STB��IBF��L�ɂȂ�܂ł�H�ɕۂ��Ȃ���΂Ȃ�Ȃ��B
				[�⑫]8049��~WR�Ɍq�����Ă���炵���B�܂�8049��PortA�ɏo�͂����瑦���b�`�����B

bit5 IBF(�o��)	���̏o�͂�H�̎���PortA�̓��̓o�b�t�@����������Ă��邱�Ƃ��Ӗ���,8049�ɑ΂��ăf�[�^�̓]���̋֎~��m�点��B
				���̐M����STB�̗����������H�ƂȂ�STB=1�̎��̃f�[�^�Ǎ��ݏI����(~RD�����オ��)��L�ɂȂ�B
				���[�h�ݒ蒼��̏����l��L�ł���B

bit6 DAK(����)	���̓��͂�8049��PortA(�o��)�̃f�[�^���󂯎�����Ƃ������Ƃ�m�点��M���ł���B
				�f�[�^���󂯎��������L�̐M�����o���悤��8049��݌v����B
				[�⑫]8049��~RD�Ɍq�����Ă���炵���B�܂�8049��PortA������͂����瑦DAK=L�ɂȂ�B

bit7 OBF(�o��)	���̐M����CPU����̃f�[�^����PortA(�o��)�Ƀ��b�`���ꂽ����L�ƂȂ�,8049�ɑ΂��Ẵf�[�^�󂯎��v���Ƃ��ē����B
				OBF��DAK=1�̂Ƃ��̃f�[�^�����ݏI����(~WR�����オ��)��L�ɂȂ�8049���f�[�^���󂯎��������DAK�̗����������H�ɂȂ�B
				���[�h�ݒ蒼��̏����l��H�ł���B
				[����]OBF��H�ɂȂ�O�Ƀf�[�^�̏����݂��s����PortA(�o��)���ω����Ă��܂��̂�,OBF��L�̊Ԃ͏����݂͔�����B
				[�⑫]8049��INT�Ɍq�����Ă���炵���B�܂�8049�͊����݂Ńf�[�^���󂯎��B


bit4 RIE		CPU�ɑ΂���f�[�^�Ǎ��ݗv�����荞�݂̋��t���O��,���̃r�b�g���r�b�g���얽�߂�1�ɂ��邱�Ƃɂ���Ċ��荞�݋���,
				0�ɂ��邱�Ƃɂ���Ċ��荞�݋֎~�ɂ���B
				RIE�����������Ă������r�b�g�Ɋ��蓖�Ă��Ă���STB�̋@�\�ɂ͉e����^���Ȃ��B

bit6 WIE		CPU�ɑ΂���f�[�^�����ݗv�����荞�݂̋��t���O��,���̃r�b�g���r�b�g���얽�߂�1�ɂ��邱�Ƃɂ���Ċ��荞�݋���,
				0�ɂ��邱�Ƃɂ���Ċ��荞�݋֎~�ɂ���B
				WIE�����������Ă������r�b�g�Ɋ��蓖�Ă��Ă���DAK�̋@�\�ɂ͉e����^���Ȃ��B
*/


// �n���h�V�F�C�N�p����r�b�g
//#define	HS_INT	(0x08)	/* bit3 */
//#define	HS_STB	(0x10)	/* bit4 */
//#define	HS_IBF	(0x20)	/* bit5 */
//#define	HS_DAK	(0x40)	/* bit6 */
//#define	HS_OBF	(0x80)	/* bit7 */

//#define	HS_RIE	(0x10)	/* bit4 */
//#define	HS_WIE	(0x40)	/* bit6 */

////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cPRT::cPRT() : fp(NULL), pdata(0), strb(false)
{
	INITARRAY( FilePath, '\0' );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cPRT::~cPRT()
{
	if( fp ) fclose( fp );
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
void cPRT::Init( const char *filename )
{
	if( filename && *filename ){
		// �t�@�C���p�X�ۑ�
		strncpy( FilePath, filename, PATH_MAX );
	}
}


////////////////////////////////////////////////////////////////
// �������f�[�^����t
//
// ����:	data	�������f�[�^�f�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void cPRT::SetData( BYTE data )
{
	pdata = ~data;
}


////////////////////////////////////////////////////////////////
// �X�g���[�u��t
//
// ����:	st		�X�g���[�u�M�� true:�L�� false:����
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void cPRT::Strobe( bool st )
{
	if( !strb && st ){
		if( fp || (fp = FOPENEN( FilePath, "ab" )) ){
			fputc( pdata, fp );
		}
	}else{
		if( fp && strb && !st ){
			fclose( fp );
			fp = NULL;
		}
		strb = st;
	}
}




////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
PIO6::PIO6( VM6 *vm, const ID& id ) : Device(vm,id) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
PIO6::~PIO6(){}


////////////////////////////////////////////////////////////////
// PartA ���C�g
//
// ����:	data	�������ރf�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void PIO6::JobWriteA( BYTE data )
{
	// ���[�h2�� DAK=H,OBF=L(�܂�WR0�����) �Ȃ�8049�ɑ΂��Ċ����ݗv��
	if( ModeA == 2 && HSDAK0 && !HSOBF0 )
		vm->CpusExtIntr();
}


////////////////////////////////////////////////////////////////
// PartB ���C�g
//
// ����:	data	�������ރf�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void PIO6::JobWriteB( BYTE data )
{
	// �v�����^�Ƀf�[�^�o��
	cPRT::SetData( data );
}


////////////////////////////////////////////////////////////////
// PartC ���C�g
//
// ����:	data	�������ރf�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void PIO6::JobWriteC1( BYTE data )
{
	// �v�����^�X�g���[�u
	cPRT::Strobe( data&1 ? false : true );
	
	// CRT�\����Ԑݒ�
	vm->VdgSetCrtDisp( data&2 ? true : false );
	
	// CG ROM BANK �ؑ�
	vm->MemSetCGBank( data&4 ? false : true );
}


////////////////////////////////////////////////////////////////
// PartD ���C�g(�R���g���[���|�[�g)
//
// ����:	data	�������ރf�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void PIO6::JobWriteD( BYTE data )
{
	// bit���̑Ή�
	switch( (data>>1)&0x07 ){
	case 0: // �v�����^�X�g���[�u
		cPRT::Strobe( data&1 ? false : true );
		break;
		
	case 1: // CRT�\����Ԑݒ�
		vm->VdgSetCrtDisp( data&1 ? true : false );
		break;
		
	case 2: // CG ROM BANK �ؑ�
		vm->MemSetCGBank( data&1 ? false : true );
		break;
	}
}


////////////////////////////////////////////////////////////////
// I/O�A�N�Z�X�֐�
////////////////////////////////////////////////////////////////
void PIO6::Out90H( int, BYTE data ){ WriteA( data ); }
void PIO6::Out91H( int, BYTE data ){ WriteB( data ); }
void PIO6::Out92H( int, BYTE data ){ WriteC( data ); }
void PIO6::Out93H( int, BYTE data ){ WriteD( data ); }
BYTE PIO6::In90H( int ){ return ReadA(); }
BYTE PIO6::In92H( int ){ return ReadC(); }
BYTE PIO6::In93H( int ){ return 0xff; }

void PIO6::OutPBH( int, BYTE data ){ WriteAE( data ); }
BYTE PIO6::InPBH( int ){ return ReadAE(); }
BYTE PIO6::InIBF( int ){ return GetIBF() ? 1 : 0; }
BYTE PIO6::InOBF( int ){ return GetOBF() ? 1 : 0; }


////////////////////////////////////////////////////////////////
// �ǂ��ł�SAVE
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool PIO6::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "8255", NULL, "PortA",		"0x%02X",	PortA );
	Ini->PutEntry( "8255", NULL, "PortB",		"0x%02X",	PortB );
	Ini->PutEntry( "8255", NULL, "PortC",		"0x%02X",	PortC );
	Ini->PutEntry( "8255", NULL, "PortAbuf",	"0x%02X",	PortAbuf );
	Ini->PutEntry( "8255", NULL, "ModeA",		"%d",		ModeA );
	Ini->PutEntry( "8255", NULL, "ModeB",		"%d",		ModeB );
	Ini->PutEntry( "8255", NULL, "PortAdir",	"%s",		PortAdir ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "PortBdir",	"%s",		PortBdir ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "PortC1dir",	"%s",		PortC1dir ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "PortC2dir",	"%s",		PortC2dir ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "HSINT0",		"%s",		HSINT0 ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "HSWINT0",		"%s",		HSWINT0 ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "HSRINT0",		"%s",		HSRINT0 ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "HSSTB0",		"%s",		HSSTB0 ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "HSIBF0",		"%s",		HSIBF0 ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "HSDAK0",		"%s",		HSDAK0 ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "HSOBF0",		"%s",		HSOBF0 ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "RIE0",		"%s",		RIE0 ? "Yes" : "No" );
	Ini->PutEntry( "8255", NULL, "WIE0",		"%s",		WIE0 ? "Yes" : "No" );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD
//
// ����:	Ini		INI�I�u�W�F�N�g�|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool PIO6::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt(   "8255", "PortA",		&st,		PortA );	PortA = st;
	Ini->GetInt(   "8255", "PortB",		&st,		PortB );	PortB = st;
	Ini->GetInt(   "8255", "PortC",		&st,		PortC );	PortC = st;
	Ini->GetInt(   "8255", "PortAbuf",	&st,		PortAbuf );	PortAbuf = st;
	Ini->GetInt(   "8255", "ModeA",		&ModeA,		ModeA );
	Ini->GetInt(   "8255", "ModeB",		&ModeB,		ModeB );
	Ini->GetTruth( "8255", "PortAdir",	&PortAdir,	PortAdir );
	Ini->GetTruth( "8255", "PortBdir",	&PortBdir,	PortBdir );
	Ini->GetTruth( "8255", "PortC1dir",	&PortC1dir,	PortC1dir );
	Ini->GetTruth( "8255", "PortC2dir",	&PortC2dir,	PortC2dir );
	Ini->GetTruth( "8255", "HSINT0",	&HSINT0,	HSINT0 );
	Ini->GetTruth( "8255", "HSWINT0",	&HSWINT0,	HSWINT0 );
	Ini->GetTruth( "8255", "HSRINT0",	&HSRINT0,	HSRINT0 );
	Ini->GetTruth( "8255", "HSSTB0",	&HSSTB0,	HSSTB0 );
	Ini->GetTruth( "8255", "HSIBF0",	&HSIBF0,	HSIBF0 );
	Ini->GetTruth( "8255", "HSDAK0",	&HSDAK0,	HSDAK0 );
	Ini->GetTruth( "8255", "HSOBF0",	&HSOBF0,	HSOBF0 );
	Ini->GetTruth( "8255", "RIE0",		&RIE0,	RIE0 );
	Ini->GetTruth( "8255", "WIE0",		&WIE0,	WIE0 );
	
	return true;
}


////////////////////////////////////////////////////////////////
//  device description
////////////////////////////////////////////////////////////////
const Device::Descriptor PIO6::descriptor = {
	PIO6::indef, PIO6::outdef
};

const Device::OutFuncPtr PIO6::outdef[] = {
	STATIC_CAST( Device::OutFuncPtr, &PIO6::Out90H ),
	STATIC_CAST( Device::OutFuncPtr, &PIO6::Out91H ),
	STATIC_CAST( Device::OutFuncPtr, &PIO6::Out92H ),
	STATIC_CAST( Device::OutFuncPtr, &PIO6::Out93H ),
	STATIC_CAST( Device::OutFuncPtr, &PIO6::OutPBH )
};

const Device::InFuncPtr PIO6::indef[] = {
	STATIC_CAST( Device::InFuncPtr, &PIO6::In90H ),
	STATIC_CAST( Device::InFuncPtr, &PIO6::In92H ),
	STATIC_CAST( Device::InFuncPtr, &PIO6::In93H ),
	STATIC_CAST( Device::InFuncPtr, &PIO6::InPBH ),
	STATIC_CAST( Device::InFuncPtr, &PIO6::InIBF ),
	STATIC_CAST( Device::InFuncPtr, &PIO6::InOBF )
};
