#include <stdlib.h>

#include "../log.h"
#include "../vsurface.h"
#include "mc6847.h"


#define	P60W		256			// �����L���\������(N60)
#define	P60H		192			// �����L���\�����C��(N60)
#define	P62W		320			// �����L���\������(N60m)
#define	P62H		200			// �����L���\�����C��(N60m)
#define	P64W		320			// �����L���\������(SR)
#define	P64H		((!CharMode && SRLine204) ? 204 : 200)	// �����L���\�����C��(SR)

// �o�b�N�o�b�t�@�T�C�Y
#define	WBBUF		(376-16)	// �K��4�̔{��
#define	HBBUF		(242-12)

// �{�[�_�[�T�C�Y
#define	LB60		((int)((WBBUF-P60W)/2))
#define	RB60		(WBBUF-P60W-LB60)
#define	TB60		((int)((HBBUF-P60H)/2))
#define	BB60		(HBBUF-P60H-TB60)
#define	LB62		((int)((WBBUF-P62W)/2))
#define	RB62		(WBBUF-P62W-LB62)
#define	TB62		((int)((HBBUF-P62H)/2))
#define	BB62		(HBBUF-P62H-TB62)
#define	LB64		((int)((WBBUF-P64W)/2))
#define	RB64		(WBBUF-P64W-LB64)
#define	TB64		((int)((HBBUF-P64H)/2))
#define	BB64		(HBBUF-P64H-TB64)

// �A�g���r���[�g
#define ATTR_AG		0x80
#define ATTR_AS		0x40
#define ATTR_INTEXT	0x20
#define ATTR_GM0	0x10
#define ATTR_GM1	0x08
#define ATTR_GM2	0x04
#define ATTR_CSS	0x02
#define ATTR_INV	0x01

#define ANMODE		( ( AT_AS << 1 ) | AT_IE )
#define AM_AN0		0			// �A���t�@�j���[�����b�N(�����t�H���g)
#define AM_AN1		1			// �A���t�@�j���[�����b�N(�O���t�H���g)
#define AM_SG4		2			// �Z�~�O���t�B�b�N4
#define AM_SG6		3			// �Z�~�O���t�B�b�N6

#define GM_CG1		0			//  64x 64 �J���[
#define GM_CG2		1			// 128x 64 �J���[
#define GM_CG3		2			// 128x 96 �J���[
#define GM_CG6		3			// 128x192 �J���[
#define GM_RG1		4			// 128x 64 ���m�N��
#define GM_RG2		5			// 128x 96 ���m�N��
#define GM_RG3		6			// 128x192 ���m�N��
#define GM_RG6		7			// 256x192 ���m�N��


// VDG Font data
const BYTE MC6847::VDGfont[] = {
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x02, 0x1a, 0x2a, 0x2a, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// @
	0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// A
	0x00, 0x00, 0x00, 0x3c, 0x12, 0x12, 0x1c, 0x12, 0x12, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// B
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x20, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// C
	0x00, 0x00, 0x00, 0x3c, 0x12, 0x12, 0x12, 0x12, 0x12, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// D
	0x00, 0x00, 0x00, 0x3e, 0x20, 0x20, 0x3c, 0x20, 0x20, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// E
	0x00, 0x00, 0x00, 0x3e, 0x20, 0x20, 0x3c, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// F
	0x00, 0x00, 0x00, 0x1e, 0x20, 0x20, 0x26, 0x22, 0x22, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// G
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// H
	0x00, 0x00, 0x00, 0x1c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// I
	0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// J
	0x00, 0x00, 0x00, 0x22, 0x24, 0x28, 0x30, 0x28, 0x24, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// K
	0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// L
	0x00, 0x00, 0x00, 0x22, 0x36, 0x2a, 0x2a, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// M
	0x00, 0x00, 0x00, 0x22, 0x32, 0x2a, 0x26, 0x22, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// N
	0x00, 0x00, 0x00, 0x3e, 0x22, 0x22, 0x22, 0x22, 0x22, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// O
	
	0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// P
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x2a, 0x24, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Q
	0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x3c, 0x28, 0x24, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// R
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x10, 0x08, 0x04, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// S
	0x00, 0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// T
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// U
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// V
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x2a, 0x2a, 0x36, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// W
	0x00, 0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x14, 0x22, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// X
	0x00, 0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Y
	0x00, 0x00, 0x00, 0x3e, 0x02, 0x04, 0x08, 0x10, 0x20, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// Z
	0x00, 0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// [
	0x00, 0x00, 0x00, 0x20, 0x20, 0x10, 0x08, 0x04, 0x02, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// \(BackSlash)
	0x00, 0x00, 0x00, 0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ]
	0x00, 0x00, 0x00, 0x08, 0x1c, 0x2a, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ��
	0x00, 0x00, 0x00, 0x00, 0x08, 0x10, 0x3e, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ��
	
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ' '
	0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// !
	0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// "
	0x00, 0x00, 0x00, 0x14, 0x14, 0x36, 0x00, 0x36, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// #
	0x00, 0x00, 0x00, 0x08, 0x1e, 0x20, 0x1c, 0x02, 0x3c, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// $
	0x00, 0x00, 0x00, 0x32, 0x32, 0x04, 0x08, 0x10, 0x26, 0x26, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// %
	0x00, 0x00, 0x00, 0x10, 0x28, 0x28, 0x10, 0x2a, 0x24, 0x1a, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// &
	0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// '
	0x00, 0x00, 0x00, 0x08, 0x10, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// (
	0x00, 0x00, 0x00, 0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// )
	0x00, 0x00, 0x00, 0x00, 0x08, 0x2a, 0x1c, 0x2a, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// *
	0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// +
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x10, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// -
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// .
	0x00, 0x00, 0x00, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// /
	
	0x00, 0x00, 0x00, 0x18, 0x24, 0x24, 0x24, 0x24, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 0
	0x00, 0x00, 0x00, 0x08, 0x18, 0x08, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 1
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x02, 0x1c, 0x20, 0x20, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 2
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x02, 0x0c, 0x02, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 3
	0x00, 0x00, 0x00, 0x04, 0x0c, 0x14, 0x3e, 0x04, 0x04, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 4
	0x00, 0x00, 0x00, 0x3e, 0x20, 0x3c, 0x02, 0x02, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 5
	0x00, 0x00, 0x00, 0x1c, 0x20, 0x20, 0x3c, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 6
	0x00, 0x00, 0x00, 0x3e, 0x02, 0x04, 0x08, 0x10, 0x20, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 7
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x1c, 0x22, 0x22, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 8
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x1e, 0x02, 0x02, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// 9
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// :
	0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// ;
	0x00, 0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// <
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// =
	0x00, 0x00, 0x00, 0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,	// >
	0x00, 0x00, 0x00, 0x18, 0x24, 0x04, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00	// ?
};

// �F�ɂ��݃J���[�R�[�h�e�[�u��
// ���l��COL_CG��̃C���f�b�N�X
const BYTE MC6847core::NJM_TBL[][2] = {
				{ 0,0 }, { 0,0 }, { 0,7 }, { 0,7 },
				{ 5,4 }, { 5,4 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 6,2 }, { 6,7 },
				{ 6,4 }, { 6,4 }, { 6,3 }, { 6,3 },
				{ 5,0 }, { 5,0 }, { 5,7 }, { 5,7 },
				{ 1,4 }, { 1,1 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 6,7 }, { 6,7 },
				{ 3,4 }, { 3,4 }, { 3,3 }, { 3,3 },
				{ 0,0 }, { 0,0 }, { 0,7 }, { 0,7 },
				{ 5,4 }, { 5,4 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 2,2 }, { 2,7 },
				{ 6,4 }, { 6,4 }, { 6,3 }, { 6,3 },
				{ 5,0 }, { 5,0 }, { 5,7 }, { 5,7 },
				{ 5,4 }, { 5,1 }, { 5,4 }, { 5,4 },
				{ 6,7 }, { 6,7 }, { 6,7 }, { 6,7 },
				{ 3,4 }, { 3,4 }, { 3,3 }, { 3,3 }
			};
//�F�ɂ��݂�2�h�b�g�P�ʂŕ`�悷��B
//�`�悷��Ώۂ�2�h�b�g�Ƃ��̑O��̊e2�h�b�g�A�v6�r�b�g��VRAM����擾��
//���̒l���C���f�b�N�X�Ƃ��āA2�h�b�g���̐F�����擾����B
//�e�[�u���Ɋi�[���ꂽ�l�͂����COL_CG��̃C���f�b�N�X�ƂȂ�B
//���@0
//���@3
//��/���@����6 2 7����
//��/�΁@����4 1 5����



////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
MC6847core::MC6847core( void ) :
	CrtDisp(true), BusReq(false), N60Win(true),
	Mode4Col(0), VAddr(0), HAddr(0), RowCntA(0), RowCntG(0),
	CharMode(true), GraphMode(false), Css1(1), Css2(1), Css3(1),
	SRmode(false), SRBusReq(true), SRBitmap(false), SRBMPage(false), SRLine204(false),
	SRCharLine(true), SRCharWidth(true),
	SRTextAddr(0), SRRollX(0), SRRollY(0), SRVramAddrY(0),
	AT_AG(0), AT_AS(0), AT_IE(0), AT_GM(0), AT_CSS(0), AT_INV(0) {}

MC6847::MC6847( void ){}

PCZ80_07::PCZ80_07( void ){}

PCZ80_12::PCZ80_12( void )
{
	SRmode    = true;
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
MC6847core::~MC6847core( void ){}

MC6847::~MC6847( void ){}

PCZ80_07::~PCZ80_07( void ){}

PCZ80_12::~PCZ80_12( void ){}


////////////////////////////////////////////////////////////////
// �o�b�N�o�b�t�@���擾(�K��l)
////////////////////////////////////////////////////////////////
int MC6847core::GetW( void ) const
{
	return WBBUF;
}


////////////////////////////////////////////////////////////////
// �o�b�N�o�b�t�@�����擾(�K��l)
////////////////////////////////////////////////////////////////
int MC6847core::GetH( void ) const
{
	return HBBUF;
}


////////////////////////////////////////////////////////////////
// �Z�~�O��4�f�[�^�擾
////////////////////////////////////////////////////////////////
BYTE MC6847core::GetSemi4( BYTE data ) const
{
	BYTE bdat = (data<<(4+(RowCntA/6)*2)&0x80) | (data<<(1+(RowCntA/6)*2)&0x08);
	bdat |= bdat>>1;
	bdat |= bdat>>2;
	return bdat;
}


////////////////////////////////////////////////////////////////
// �Z�~�O��6�f�[�^�擾
////////////////////////////////////////////////////////////////
BYTE MC6847core::GetSemi6( BYTE data ) const
{
	BYTE bdat = (data<<(2+(RowCntA/4)*2)&0x80) | (((data<<(3+(RowCntA/4)*2))>>4)&0x08);
	bdat |= bdat>>1;
	bdat |= bdat>>2;
	return bdat;
}

BYTE PCZ80_07::GetSemi6( BYTE data ) const
{
	return GetFont1( 0x1000 + (data&0x3f)*16 + RowCntA );	// �Z�~�O���t�H���g�� 1000H-13FFH
}

BYTE PCZ80_12::GetSemi6( BYTE data ) const
{
	return MC6847core::GetSemi6( data );	// SR�͏���Ɠ��l�ɐ���
}


////////////////////////////////////////////////////////////////
// �Z�~�O��8�f�[�^�擾
////////////////////////////////////////////////////////////////
BYTE PCZ80_12::GetSemi8( BYTE data ) const
{
	BYTE bdat = ((data<<(RowCntA&0xe))&0x80) | (((data<<(RowCntA&0xe))>>3)&0x08);
	bdat |= bdat>>1;
	bdat |= bdat>>2;
	return bdat;
}


////////////////////////////////////////////////////////////////
// CRT�\����Ԏ擾
////////////////////////////////////////////////////////////////
bool MC6847core::GetCrtDisp( void ) const
{
	return CrtDisp;
}


////////////////////////////////////////////////////////////////
// CRT�\����Ԑݒ�
////////////////////////////////////////////////////////////////
void MC6847core::SetCrtDisp( bool st )
{
	CrtDisp = st;
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�T�C�Y�擾
////////////////////////////////////////////////////////////////
bool MC6847core::GetWinSize( void ) const
{
	return N60Win;
}


////////////////////////////////////////////////////////////////
// ���[�h4�J���[���[�h�擾
////////////////////////////////////////////////////////////////
int MC6847core::GetMode4Color( void ) const
{
	return Mode4Col;
}


////////////////////////////////////////////////////////////////
// ���[�h4�J���[���[�h�ݒ�
////////////////////////////////////////////////////////////////
void MC6847core::SetMode4Color( int col )
{
	Mode4Col = col;
}


////////////////////////////////////////////////////////////////
// �F�̑g�����w��
////////////////////////////////////////////////////////////////
void MC6847core::SetCss( BYTE data )
{
	PRINTD( VDG_LOG, "[VDG][SetCss] -> %02X : ", data );
	
	Css1 =  data    &1;
	Css2 = (data>>1)&1;
	Css3 = (data>>2)&1;
	
	PRINTD( VDG_LOG, "1:%d 2:%d 3:%d\n", Css1, Css2, Css3 );
}


////////////////////////////////////////////////////////////////
// CRT�R���g���[�����[�h�ݒ�
////////////////////////////////////////////////////////////////
void MC6847core::SetCrtControler( BYTE data )
{
	PRINTD( VDG_LOG, "[VDG][62][SetCrtControler] -> %02X\n", data );
	
	N60Win    = data&2 ? true : false;	// �E�B���h�E�T�C�Y		true:N60		false:N60m
	CharMode  = data&4 ? true : false;	// �\�����[�h			true:�L�����N�^	false:�O���t�B�b�N
	GraphMode = data&8 ? true : false;	// �O���t�B�b�N�𑜓x	true:160*200	false:320*200
	
	PRINTD( VDG_LOG, " DATA      :%02X\n", data );
	PRINTD( VDG_LOG, " N60Win    :%s\n", N60Win    ? "N60"  : "N60m" );
	PRINTD( VDG_LOG, " CharMode  :%s\n", CharMode  ? "Char" : "Graph" );
	PRINTD( VDG_LOG, " GraphMode :%d\n", GraphMode ? 160    : 320 );
}

void PCZ80_12::SetCrtControler( BYTE data )
{
	PRINTD( VDG_LOG, "[VDG][64][SetCrtControler]" );
	
	if( SRmode ){	// SR���[�h
		PRINTD( VDG_LOG, "[SRmode] -> %02X\n", data );
		
		SRLine204   = data&1 ? false : true;	// �O���t�B�b�N���C����	true:204���C��	false:200���C��
		SRCharWidth = data&2 ? true : false;	// �e�L�X�g������		true:40����		false:80����
		CharMode    = data&4 ? true : false;	// �e�L�X�g�\�����[�h	true:�L�����N�^	false:�O���t�B�b�N
		GraphMode   = data&8 ? true : false;	// �O���t�B�b�N�𑜓x	true:320*200	false:640*200
		
		PRINTD( VDG_LOG, " Lines       :%d\n", SRLine204   ? 204    : 200 );
		PRINTD( VDG_LOG, " SRTextWidth :%d\n", SRCharWidth ? 40     : 80 );
		PRINTD( VDG_LOG, " CharMode    :%s\n", CharMode    ? "Char" : "Graph" );
		PRINTD( VDG_LOG, " GraphMode   :%d\n", GraphMode   ? 640    : 320 );
	}else{			// �����[�h
		PRINTD( VDG_LOG, "[66mode] -> %02X\n", data );
		
		N60Win    = data&2 ? true : false;	// �E�B���h�E�T�C�Y		true:N60		false:N60m
		CharMode  = data&4 ? true : false;	// �e�L�X�g�\�����[�h	true:�L�����N�^	false:�O���t�B�b�N
		GraphMode = data&8 ? true : false;	// �O���t�B�b�N�𑜓x	true:160*200	false:320*200
		
		PRINTD( VDG_LOG, " N60Win    :%s\n", N60Win    ? "N60"  : "N60m" );
		PRINTD( VDG_LOG, " CharMode  :%s\n", CharMode  ? "Char" : "Graph" );
		PRINTD( VDG_LOG, " GraphMode :%d\n", GraphMode ? 160    : 320 );
	}
}


////////////////////////////////////////////////////////////////
// CRT�R���g���[���^�C�v�ݒ�
////////////////////////////////////////////////////////////////
void PCZ80_12::SetCrtCtrlType( BYTE data )
{
	PRINTD( VDG_LOG, "[VDG][64][SetCrtCtrlType] -> %02X\n", data );
	
	SRmode     = data&0x01 ? false : true;	// SR���[�h�t���O 		true:SR-BASIC	false:��BASIC
	SRBusReq   = data&0x02 ? false : true;	// �o�X���N�G�X�g		true:ON			false:OFF
	SRCharLine = data&0x04 ? true  : false;	// SR�e�L�X�g�s��		true:20�s		false:25�s
	SRBitmap   = data&0x08 ? false : true;	// SR�r�b�g�}�b�v�t���O	true:�L��		false:����
	SRBMPage   = data&0x10 ? true  : false;	// SR�r�b�g�}�b�v�y�[�W	true:���32KB	false:����32KB
	
	PRINTD( VDG_LOG, " BasicMode    :%s\n",   SRmode     ? "SR"     : "66" );
	PRINTD( VDG_LOG, " SRBusReqest  :%s\n",   SRBusReq   ? "ON"     : "OFF" );
	PRINTD( VDG_LOG, " SRTextLines  :%d\n",   SRCharLine ? 20       : 25 );
	PRINTD( VDG_LOG, " SRBitmapMode :%s\n",   SRBitmap   ? "Bitmap" : "Text" );
	PRINTD( VDG_LOG, " SRBitmapPage :%04X\n", SRBMPage   ? 0x8000   : 0x0000 );
}


////////////////////////////////////////////////////////////////
// �o�b�N�o�b�t�@�X�V
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void MC6847::UpdateBackBuf( void )
{
	PRINTD( VDG_LOG, "[VDG][60][UpdateBackBuf]\n" );
	
	#if INBPP == 8	// 8bit
	BYTE bcol;
	BYTE *doff   = (BYTE *)GetBufAddr();
	const int pt = GetBufPitch();
	#else			// 32bit
	DWORD bcol;
	DWORD *doff  = (DWORD *)GetBufAddr();
	const int pt = GetBufPitch() / sizeof(DWORD);
	#endif
	
	VAddr = HAddr = RowCntA = RowCntG = 0;
	
	const int tbline = TB60 * pt;
	const int bbline = BB60 * pt;
	
	// �㑤�{�[�_�`��
	bcol = GetBcol();
	for( int i=0; i<tbline; i++ ) *doff++ = bcol;
	
	// �\���G���A�`��
	for( int i=0; i<P60H; i++ ) Draw1line1( i );
	
	// �����{�[�_�`��
	doff += P60H * pt;
	bcol = GetBcol();
	for( int i=0; i<bbline; i++ ) *doff++ = bcol;
}

void PCZ80_07::UpdateBackBuf( void )
{
	PRINTD( VDG_LOG, "[VDG][62][UpdateBackBuf]\n" );
	
	#if INBPP == 8	// 8bit
	BYTE bcol;
	BYTE *doff   = (BYTE *)GetBufAddr();
	const int pt = GetBufPitch();
	#else			// 32bit
	DWORD bcol;
	DWORD *doff  = (DWORD *)GetBufAddr();
	const int pt = GetBufPitch() / sizeof(DWORD);
	#endif
	
	VAddr = HAddr = RowCntA = 0;
	
	bcol = GetBcol();
	if( N60Win ){	// N60
		const int tbline = TB60 * pt;
		const int bbline = BB60 * pt;
		
		// �㑤�{�[�_�`��
		for( int i=0; i<tbline; i++ ) *doff++ = bcol;
		
		// �\���G���A�`��
		for( int i=0; i<P60H; i++ ) Draw1line1( i );
		
		// �����{�[�_�`��
		doff += P60H * pt;
		bcol = GetBcol();
		for( int i=0; i<bbline; i++ ) *doff++ = bcol;
	}else{			// N60m
		const int tbline = TB62 * pt;
		const int bbline = BB62 * pt;
		
		// �㑤�{�[�_�`��
		for( int i=0; i<tbline; i++ ) *doff++ = bcol;
		
		// �\���G���A�`��
		for( int i=0; i<P62H; i++ ) Draw1line2( i );
		
		// �����{�[�_�`��
		doff += P62H * pt;
		bcol = GetBcol();
		for( int i=0; i<bbline; i++ ) *doff++ = bcol;
	}
}

void PCZ80_12::UpdateBackBuf( void )
{
	PRINTD( VDG_LOG, "[VDG][64][UpdateBackBuf]\n" );
	
	#if INBPP == 8	// 8bit
	BYTE bcol;
	BYTE *doff   = (BYTE *)GetBufAddr();
	const int pt = GetBufPitch();
	#else			// 32bit
	DWORD bcol;
	DWORD *doff  = (DWORD *)GetBufAddr();
	const int pt = GetBufPitch() / sizeof(DWORD);
	#endif
	
	VAddr = HAddr = RowCntA = 0;
	
	bcol = GetBcol();
	if( SRmode ){	// SR���[�h
		const int tbline = TB64 * pt;
		const int bbline = BB64 * pt;
		
		// �㑤�{�[�_�`��
		for( int i=0; i<tbline; i++ ) *doff++ = bcol;
		
		// �\���G���A�`��
		for( int i=0; i<P64H; i++ ) Draw1line3( i );
		
		// �����{�[�_�`��
		doff += P64H * pt;
		bcol = GetBcol();
		for( int i=0; i<bbline; i++ ) *doff++ = bcol;
	}else{			// �����[�h
		if( N60Win ){	// N60
			const int tbline  = TB60 * pt;
			const int bbline  = BB60 * pt;
			
			// �㑤�{�[�_�`��
			for( int i=0; i<tbline; i++ ) *doff++ = bcol;
			
			// �\���G���A�`��
			for( int i=0; i<P60H; i++ ) Draw1line1( i );
			
			// �����{�[�_�`��
			doff += P60H * pt;
			bcol = GetBcol();
			for( int i=0; i<bbline; i++ ) *doff++ = bcol;
		}else{			// N60m
			const int tbline  = TB62 * pt;
			const int bbline  = BB62 * pt;
			
			// �㑤�{�[�_�`��
			for( int i=0; i<tbline; i++ ) *doff++ = bcol;
			
			// �\���G���A�`��
			for( int i=0; i<P62H; i++ ) Draw1line2( i );
			
			// �����{�[�_�`��
			doff += P62H * pt;
			bcol = GetBcol();
			for( int i=0; i<bbline; i++ ) *doff++ = bcol;
		}
	}
}


////////////////////////////////////////////////////////////////
// 1���C���`��(N60)
//
// ����:	line	�`�悷�郉�C���ԍ�(0-191)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void MC6847::Draw1line1( int line )
{
	if( !line ){ PRINTD( VDG_LOG, "[VDG][Draw1line1] %d\n", line ); }
	
	BYTE data=0xff, prevdata=0, nextdata=0, bdat=0;
	BYTE LAT_AG=0, LAT_GM=0;
	
	#if INBPP == 8	// 8bit
	BYTE wdat, fg=0, bg=0;
	BYTE *doff  = (BYTE *)GetBufAddr()  + ( TB60 + line ) * GetBufPitch();
	#else			// 32bit
	DWORD wdat, fg=0, bg=0;
	DWORD *doff = (DWORD *)GetBufAddr() + ( TB60 + line ) * GetBufPitch() / sizeof(DWORD);
	#endif
	
	// �����{�[�_�[�`��
	wdat = GetBcol();
	for( int i=0; i<LB60; i++ ) *doff++ = wdat;
	
	// �\���G���A�`��
	for( int x=0; x<( P60W / 8 ); x++ ){
		if( !CrtDisp ){
			data = AT_AG ? rand() : 0;	// �z���g�H ��łǂ��ɂ�����
		}else{
			LatchAttr();
			// 1�o�C�g�O�̃f�[�^(�F�ɂ��ݎZ�o�Ɏg�p)
			prevdata = data;
			data = GetVram();
			HAddr++;
			// 1�o�C�g��̃f�[�^(�F�ɂ��ݎZ�o�Ɏg�p)
			nextdata = ( x < (P60W/8-1) ) ? GetVram() : 0xff;
			LAT_AG |= AT_AG;				// �Ƃ肠����
			if( x == 2 ) LAT_GM = AT_GM;	// �Ƃ肠����
			
			// 1byte��̃O���t�B�b�N���[�h�擾
			if( LAT_AG ) LatchGMODE();
				// ����@��VDG(���Ӊ�H?)�ɂ̓o�O������炵��
				// �A�g���r���[�g�A�h���X�ɑ΂��J���[�Z�b�g�͐������ǂ߂邪
				// ���[�h����̎��ɂ�1byte��̃A�h���X����ǂ�ł��܂��悤��
				// �E�[�̏ꍇ�͂��̃��C���̐擪(���[)�̃f�[�^��ǂނ炵��
				// mk2,66�ł͂ǂ�����������ǂ߂�
		}
		
		if( AT_AG ){	// �O���t�B�b�N
			switch( AT_GM ){
			case GM_CG1:	//  64x 64 �J���[   (CG1)
				for( int i=3; i>=0; i-- ){
					#if INBPP == 8	// 8bit
					wdat = COL_CG[AT_CSS][(data>>(i*2))&3];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_CG[AT_CSS][(data>>(i*2))&3] );
					#endif
					for( int j=0; j<4; j++ ) *doff++ = wdat;
				}
				x++;
				break;
				
			case GM_CG2:	// 128x 64 �J���[   (CG2)
			case GM_CG3:	// 128x 96 �J���[   (CG3)
			case GM_CG6:	// 128x192 �J���[   (CG6)
				for( int i=6; i>=0; i-=2 ){
					#if INBPP == 8	// 8bit
					wdat = COL_CG[AT_CSS][(data>>i)&3];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_CG[AT_CSS][(data>>i)&3] );
					#endif
					*doff++ = wdat;
					*doff++ = wdat;
				}
				break;
				
			case GM_RG1:	// 128x 64 ���m�N�� (RG1)
			case GM_RG2:	// 128x 96 ���m�N�� (RG2)
			case GM_RG3:	// 128x192 ���m�N�� (RG3)
				for( int i=7; i>=0; i-- ){
					#if INBPP == 8	// 8bit
					wdat = COL_RG[AT_CSS][(data>>i)&1];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_RG[AT_CSS][(data>>i)&1] );
					#endif
					*doff++ = wdat;
					*doff++ = wdat;
				}
				x++;
				break;
				
			case GM_RG6:	// 256x192 ���m�N�� (RG6)
				if( Mode4Col ){	// 128x192 �J���[(�ɂ���)
					// VRAM��̘A������3�o�C�g
					DWORD data3 = prevdata << 16 | data << 8 | nextdata;
					int CsC = AT_CSS + Mode4Col*2;
					for( int i=6; i>=0; i-=2 ){
						// �ɂ��݃e�[�u����̃C���f�b�N�X
						DWORD nijimiIdx = (data3 & (0x0000003f << (i + 6))) >> (i + 6);
						#if INBPP == 8	// 8bit
						*doff++ = COL_CG[CsC][NJM_TBL[nijimiIdx][0]];
						*doff++ = COL_CG[CsC][NJM_TBL[nijimiIdx][1]];
						#else			// 32bit
						*doff++ = VSurface::GetColor( COL_CG[CsC][NJM_TBL[nijimiIdx][0]] );
						*doff++ = VSurface::GetColor( COL_CG[CsC][NJM_TBL[nijimiIdx][1]] );
						#endif
					}
				}else{			// 256x192 ���m�N��
					for( int i=7; i>=0; i-- )
						#if INBPP == 8	// 8bit
						*doff++ = COL_RG[AT_CSS][(data>>i)&1];
						#else			// 32bit
						*doff++ = VSurface::GetColor( COL_RG[AT_CSS][(data>>i)&1] );
						#endif
				}
			}
			
		}else{			// �A���t�@�j���[�����b�N
			switch( ANMODE ){
			case AM_AN0:		// �A���t�@�j���[�����b�N(�����t�H���g)
				#if INBPP == 8	// 8bit
				fg   = COL_AN[(AT_CSS<<1) |   AT_INV];
				bg   = COL_AN[(AT_CSS<<1) | (~AT_INV&1)];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) |   AT_INV] );
				bg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) | (~AT_INV&1)] );
				#endif
				bdat = VDGfont[ (data&0x3f)*16 + RowCntA ];
				break;
				
			case AM_AN1:		// �A���t�@�j���[�����b�N(�O���t�H���g)
				#if INBPP == 8	// 8bit
				fg   = COL_AN[(AT_CSS<<1) |   AT_INV];
				bg   = COL_AN[(AT_CSS<<1) | (~AT_INV&1)];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) |   AT_INV] );
				bg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) | (~AT_INV&1)] );
				#endif
				bdat = GetFont1( data*16 + RowCntA );
				break;
				
			case AM_SG4:		// �Z�~�O���t�B�b�N4
				#if INBPP == 8	// 8bit
				fg   = COL_SG[(data>>4)&7];
				bg   = COL_SG[8];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_SG[(data>>4)&7] );
				bg   = VSurface::GetColor( COL_SG[8] );
				#endif
				bdat = GetSemi4( data );
				break;
				
			case AM_SG6:		// �Z�~�O���t�B�b�N6
				#if INBPP == 8	// 8bit
				fg   = COL_SG[(AT_CSS<<2) | ((data>>6)&3)];
				bg   = COL_SG[8];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_SG[(AT_CSS<<2) | ((data>>6)&3)] );
				bg   = VSurface::GetColor( COL_SG[8] );
				#endif
				bdat = GetSemi6( data );
			}
			for( int i=7; i>=0; i-- )
				*doff++ = (bdat>>i)&1 ? fg : bg;
		};
	}
	
	
	// �������A�h���X�I�t�Z�b�g���߂�
	//   �ǂ����e���C��3byte�ڂ̕\�����[�h�Ō��܂��Ă���炵��
	//   ���������ꃉ�C���ɈقȂ郂�[�h�����݂���Ƃ��͋������ς��
	if( LAT_AG ){	// �O���t�B�b�N
		switch( LAT_GM ){
		case GM_CG1:	//  64x 64 �J���[   (CG1)
		case GM_CG2:	// 128x 64 �J���[   (CG2)
		case GM_RG1:	// 128x 64 ���m�N�� (RG1)
			if( RowCntG++ == 2 ){
				VAddr++;
				RowCntG = 0;
			}
			break;
		case GM_CG3:	// 128x 96 �J���[   (CG3)
		case GM_RG2:	// 128x 96 ���m�N�� (RG2)
			if( RowCntG++ == 1 ){
				VAddr++;
				RowCntG = 0;
			}
			break;
		case GM_CG6:	// 128x192 �J���[   (CG6)
		case GM_RG3:	// 128x192 ���m�N�� (RG3)
		case GM_RG6:	// 256x192 ���m�N�� (RG6)
			VAddr++;
		}
		RowCntA = 0;
	}else{			// �A���t�@�j���[�����b�N
		if( ++RowCntA == 12 ){
			VAddr++;
			RowCntA = 0;
		}
		RowCntG = 0;
	}
	HAddr = 0;
	
	// �E���{�[�_�[�`��
	wdat = GetBcol();
	for( int i=0; i<RB60; i++ ) *doff++ = wdat;
}

void PCZ80_07::Draw1line1( int line )
{
	if( !line ){ PRINTD( VDG_LOG, "[VDG][Draw1line1] %d\n", line ) };
	
	BYTE data=0, prevdata=0, nextdata=0, bdat=0;
	BYTE LAT_AG=0;
	
	#if INBPP == 8	// 8bit
	BYTE wdat, fg=0, bg=0;
	BYTE *doff  = (BYTE *)GetBufAddr()  + ( TB60 + line ) * GetBufPitch();
	#else			// 32bit
	DWORD wdat, fg=0, bg=0;
	DWORD *doff = (DWORD *)GetBufAddr() + ( TB60 + line ) * GetBufPitch() / sizeof(DWORD);
	#endif
	
	// �����{�[�_�[�`��
	wdat = GetBcol();
	for( int i=0; i<LB60; i++ ) *doff++ = wdat;
	
	// �\���G���A�`��
	for( int x=0; x<( P60W / 8 ); x++ ){
		if( !CrtDisp ){
			data = 0;	// �z���g�HRGB�o�͂ƃr�f�I�o�͂ňقȂ�͂�
		}else{
			LatchAttr();
			// 1�o�C�g�O�̃f�[�^(�F�ɂ��ݎZ�o�Ɏg�p)
			prevdata = data;
			data = GetVram();
			HAddr++;
			// 1�o�C�g��̃f�[�^(�F�ɂ��ݎZ�o�Ɏg�p)
			nextdata = ( x < (P60W/8-1) ) ? GetVram() : 0;
			LAT_AG |= AT_AG;				// �Ƃ肠����
		}
		
		if( AT_AG ){	// �O���t�B�b�N
			switch( AT_GM ){
			case GM_CG1:	//  64x 64 �J���[   (CG1)(����)
			case GM_CG2:	// 128x 64 �J���[   (CG2)(����)
			case GM_CG3:	// 128x 96 �J���[   (CG3)(����)
			case GM_CG6:	// 128x192 �J���[   (CG6)
				for( int i=6; i>=0; i-=2 ){
					#if INBPP == 8	// 8bit
					wdat = COL_CG[AT_CSS][(data>>i)&3];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_CG[AT_CSS][(data>>i)&3] );
					#endif
					*doff++ = wdat;
					*doff++ = wdat;
				}
				break;
				
			case GM_RG1:	// 128x 64 ���m�N�� (RG1)(����)
			case GM_RG2:	// 128x 96 ���m�N�� (RG2)(����)
			case GM_RG3:	// 128x192 ���m�N�� (RG3)(����)
			case GM_RG6:	// 256x192 ���m�N�� (RG6)
				if( Mode4Col ){	// 128x192 �J���[(�ɂ���)
					// VRAM��̘A������3�o�C�g
					DWORD data3 = prevdata << 16 | data << 8 | nextdata;
					int CsC = AT_CSS + Mode4Col*2;
					for( int i=6; i>=0; i-=2 ){
						// �ɂ��݃e�[�u����̃C���f�b�N�X
						DWORD nijimiIdx = (data3 & (0x0000003f << (i + 6))) >> (i + 6);
						#if INBPP == 8	// 8bit
						*doff++ = COL_CG[CsC][NJM_TBL[nijimiIdx][0]];
						*doff++ = COL_CG[CsC][NJM_TBL[nijimiIdx][1]];
						#else			// 32bit
						*doff++ = VSurface::GetColor( COL_CG[CsC][NJM_TBL[nijimiIdx][0]] );
						*doff++ = VSurface::GetColor( COL_CG[CsC][NJM_TBL[nijimiIdx][1]] );
						#endif
					}
				}else{			// 256x192 ���m�N��
					for( int i=7; i>=0; i-- )
						#if INBPP == 8	// 8bit
						*doff++ = COL_RG[AT_CSS][(data>>i)&1];
						#else			// 32bit
						*doff++ = VSurface::GetColor( COL_RG[AT_CSS][(data>>i)&1] );
						#endif
				}
			}
		}else{			// �A���t�@�j���[�����b�N
			switch( ANMODE ){
			case AM_AN0:		// �A���t�@�j���[�����b�N(�����t�H���g)(����)
			case AM_AN1:		// �A���t�@�j���[�����b�N(�O���t�H���g)
				#if INBPP == 8	// 8bit
				fg   = COL_AN[(AT_CSS<<1) |   AT_INV];
				bg   = COL_AN[(AT_CSS<<1) | (~AT_INV&1)];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) |   AT_INV] );
				bg   = VSurface::GetColor( COL_AN[(AT_CSS<<1) | (~AT_INV&1)] );
				#endif
				bdat = GetFont1( data*16 + RowCntA );
				break;
				
			case AM_SG4:		// �Z�~�O���t�B�b�N4(����)
			case AM_SG6:		// �Z�~�O���t�B�b�N6
				#if INBPP == 8	// 8bit
				fg   = COL_SG[(AT_CSS<<2) | ((data>>6)&3)];
				bg   = COL_SG[8];
				#else			// 32bit
				fg   = VSurface::GetColor( COL_SG[(AT_CSS<<2) | ((data>>6)&3)] );
				bg   = VSurface::GetColor( COL_SG[8] );
				#endif
				bdat = GetSemi6( data );
			}
			for( int i=7; i>=0; i-- )
				*doff++ = (bdat>>i)&1 ? fg : bg;
		};
	}
	
	
	// �������A�h���X�I�t�Z�b�g���߂�
	//   �ǂ����e���C��3byte�ڂ̕\�����[�h�Ō��܂��Ă���炵��
	//   ���������ꃉ�C���ɈقȂ郂�[�h�����݂���Ƃ��͋������ς��
	//   mk,66���������ǂ����͕s��������������ƃV���v�����Ǝv����
	if( LAT_AG ){	// �O���t�B�b�N
		VAddr++;
		RowCntA = 0;
	}else{			// �A���t�@�j���[�����b�N
		if( ++RowCntA == 12 ){
			VAddr++;
			RowCntA = 0;
		}
	}
	HAddr = 0;
	
	// �E���{�[�_�[�`��
	wdat = GetBcol();
	for( int i=0; i<RB60; i++ ) *doff++ = wdat;
}


////////////////////////////////////////////////////////////////
// 1���C���`��(N60m)
//
// ����:	line	�`�悷�郉�C���ԍ�(0-199)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void PCZ80_07::Draw1line2( int line )
{
	if( !line ){ PRINTD( VDG_LOG, "[VDG][Draw1line2] %d %s\n", line, CharMode ? "Char" : GraphMode ? "Graph3" : "Graph4" ); }
	
	BYTE attr, data, bdat;
	
	#if INBPP == 8	// 8bit
	BYTE wdat, fg, bg;
	BYTE *doff  = (BYTE *)GetBufAddr()  + ( TB62 + line ) * GetBufPitch();
	#else			// 32bit
	DWORD wdat, fg, bg;
	DWORD *doff = (DWORD *)GetBufAddr() + ( TB62 + line ) * GetBufPitch() / sizeof(DWORD);
	#endif
	
	// �����{�[�_�[�`��
	wdat = GetBcol();
	for( int i=0; i<LB62; i++ ) *doff++ = wdat;
	
	// �\���G���A�`��
	for( int x=0; x<( P62W / 8 ); x++){
		if( !CrtDisp ){
			attr = 0;	// �z���g�H
			data = 0;	// �z���g�H
		}else{
			attr = GetAttr();
			data = GetVram();
			HAddr++;
		}
		
		if( CharMode ){	// �L�����N�^���[�h
			#if INBPP == 8	// 8bit
			fg   = COL_AN2[attr&0x0f];
			bg   = COL_AN2[((attr>>4)&0x07)|((Css2&1)<<3)];
			#else			// 32bit
			fg   = VSurface::GetColor( COL_AN2[attr&0x0f] );
			bg   = VSurface::GetColor( COL_AN2[((attr>>4)&0x07)|((Css2&1)<<3)] );
			#endif
			bdat = GetFont2( (data+((attr&0x80)?256:0))*16 + RowCntA );
			for( int i=7; i>=0; i-- )
				*doff++ = (bdat>>i)&1 ? fg : bg;
		}else{				// �O���t�B�b�N���[�h
			if( GraphMode ){	// �O���t�B�b�N ���[�h3
				for( int i=6; i>=0; i-=2 ){
					#if INBPP == 8	// 8bit
					wdat = COL_CG2[Css3][(((data<<2)>>i)&0x0c)|((attr>>i)&3)];
					#else			// 32bit
					wdat = VSurface::GetColor( COL_CG2[Css3][(((data<<2)>>i)&0x0c)|((attr>>i)&3)] );
					#endif
					*doff++ = wdat;
					*doff++ = wdat;
				}
			}else{				// �O���t�B�b�N ���[�h4
				BYTE c = (Css2<<3)|(Css1<<2);
				for( int i=7; i>=0; i-- )
					#if INBPP == 8	// 8bit
					*doff++ = COL_CG2[Css3][c|(((data<<1)>>i)&2)|((attr>>i)&1)];
					#else			// 32bit
					*doff++ = VSurface::GetColor( COL_CG2[Css3][c|(((data<<1)>>i)&2)|((attr>>i)&1)] );
					#endif
			}
		}
	}
	
	
	// �������A�h���X�I�t�Z�b�g���߂�
	if( CharMode ){	// �A���t�@�j���[�����b�N
		if( ++RowCntA == 10 ){
			VAddr++;
			RowCntA = 0;
		}
	}else{				// �O���t�B�b�N
		VAddr++;
		RowCntA = 0;
	}
	HAddr = 0;
	
	// �E���{�[�_�[�`��
	wdat = GetBcol();
	for( int i=0; i<RB62; i++ ) *doff++ = wdat;
}


////////////////////////////////////////////////////////////////
// 1���C���`��(SR)
//
// ����:	line	�`�悷�郉�C���ԍ�(0-204)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void PCZ80_12::Draw1line3( int line )
{
	if( !line ){ PRINTD( VDG_LOG, "[VDG][Draw1line3] %d %s\n", line, CharMode ? "Char" : GraphMode ? "Graph2" : "Graph3" ); }
	
	BYTE attr, data, bdat;
	BYTE attr1=0, data1=0, scrl1=0, scrl2=0;
	
	#if INBPP == 8	// 8bit
	BYTE wdat, fg, bg;
	BYTE *doff  = (BYTE *)GetBufAddr()  + ( TB64 + line ) * GetBufPitch();
	#else			// 32bit
	DWORD wdat, fg, bg;
	DWORD *doff = (DWORD *)GetBufAddr() + ( TB64 + line ) * GetBufPitch() / sizeof(DWORD);
	#endif
	
	const int xsc = ((CharMode && !SRCharWidth) || (!CharMode && !GraphMode)) ? 2 : 1;
	
	// �����{�[�_�[�`��
	wdat = GetBcol();
	for( int i=0; i<(LB64*xsc); i++ ) *doff++ = wdat;
	
	// �\���G���A�`��
	for( int x=0; x<( P64W / 8 * xsc ); x++){
		if( CharMode ){	// �e�L�X�g���[�h
			if( !CrtDisp ){
				attr = 0;	// �z���g�H
				data = 0;	// �z���g�H
			}else{
				data = GetVram();
				HAddr++;
				attr = GetVram();
				HAddr++;
			}
			
			#if INBPP == 8	// 8bit
			fg   = COL_AN2[attr&0x0f];
			bg   = COL_AN2[((attr>>4)&0x07)|((Css2&1)<<3)];
			#else			// 32bit
			fg   = VSurface::GetColor( COL_AN2[attr&0x0f] );
			bg   = VSurface::GetColor( COL_AN2[((attr>>4)&0x07)|((Css2&1)<<3)] );
			#endif
			bdat = SRCharLine  ? GetFont2( (data+((attr&0x80)?256:0))*16 + RowCntA ) :	// 20�s
				   attr & 0x80 ? GetSemi8( data ) :										// 25�s �Z�~�O�����[�h
								 GetFont3( (data+((attr&0x80)?256:0))*16 + RowCntA );	// 25�s �L�����N�^���[�h
			
			for( int i=7; i>=0; i-- )
				*doff++ = (bdat>>i)&1 ? fg : bg;
			
		}else{			// �r�b�g�}�b�v���[�h
			if( GraphMode ){	// �O���t�B�b�N ���[�h2
				BYTE d1;
				if( (SRRollX&1) && (x == 0) ){
					data1 = GetVram();
					HAddr += 2;
					
					scrl1 = data1<<4;
					data1 >>= 4;
				}
				
				for( int i=0; i<4; i++ ){
					if( !CrtDisp ){
						data = 0;	// �z���g�H
					}else if( SRRollX&1 ){
						if( HAddr < 320 ){
							d1 = GetVram();
							HAddr += 2;
							
							data  = data1 | (d1<<4);
							data1 = d1>>4;
						}else{
							data = data1 | scrl1;
						}
					}else{
						data = GetVram();
						HAddr += 2;
					}
					#if INBPP == 8	// 8bit
					*doff++ = COL_CG2[Css3][ data    &0x0f];
					*doff++ = COL_CG2[Css3][(data>>4)&0x0f];
					#else			// 32bit
					*doff++ = VSurface::GetColor( COL_CG2[Css3][ data    &0x0f] );
					*doff++ = VSurface::GetColor( COL_CG2[Css3][(data>>4)&0x0f] );
					#endif
				}
			}else{				// �O���t�B�b�N ���[�h3
				if( !CrtDisp ){
					attr = 0;	// �z���g�H
					data = 0;	// �z���g�H
				}else if( SRRollX&3 ){	// �X�N���[������
					BYTE d1, d2;
					if( x == 0 ){
						data1 = GetVram(); HAddr += 2;
						attr1 = GetVram(); HAddr += 2;
						
						scrl1 = data1 >> (8-((SRRollX&3)<<1));
						scrl2 = attr1 >> (8-((SRRollX&3)<<1));
						
						data1 <<= ((SRRollX&3)<<1);
						attr1 <<= ((SRRollX&3)<<1);
					}
					if( HAddr < 320 ){
						d1 = GetVram(); HAddr += 2;
						d2 = GetVram(); HAddr += 2;
						
						data = data1 | (d1>>(8-((SRRollX&3)<<1)));
						attr = attr1 | (d2>>(8-((SRRollX&3)<<1)));
						
						data1 = d1 << ((SRRollX&3)<<1);
						attr1 = d2 << ((SRRollX&3)<<1);
					}else{
						data = data1 | scrl1;
						attr = attr1 | scrl2;
					}
				}else{				// �X�N���[���Ȃ�
					data = GetVram(); HAddr += 2;
					attr = GetVram(); HAddr += 2;
				}
				
				for( int i=0; i<8; i++ )
					#if INBPP == 8	// 8bit
					*doff++ =                     COL_CG2[Css3][((Css2&1)<<3)|((Css1&1)<<2)|(((attr>>(7-i))<<1)&2)|((data>>(7-i))&1)];
					#else			// 32bit
					*doff++ = VSurface::GetColor( COL_CG2[Css3][((Css2&1)<<3)|((Css1&1)<<2)|(((attr>>(7-i))<<1)&2)|((data>>(7-i))&1)] );
					#endif
			}
		}
	}
	
	// �������A�h���X�I�t�Z�b�g���߂�
	if( CharMode ){	// �A���t�@�j���[�����b�N
		if( ++RowCntA == (SRCharLine ? 10 : 8) ){
			VAddr++;
			RowCntA = 0;
		}
	}else{				// �O���t�B�b�N
		VAddr++;
		RowCntA = 0;
	}
	HAddr = 0;
	
	// �E���{�[�_�[�`��
	wdat = GetBcol();
	for( int i=0; i<(RB64*xsc); i++ ) *doff++ = wdat;
}


////////////////////////////////////////////////////////////////
// �{�[�_�[�J���[�擾
//
// ����:	�Ȃ�
// �Ԓl:	BYTE	�{�[�_�[�J���[�R�[�h
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
BYTE MC6847::GetBcol( void ) const
{
	BYTE bcol;
	
	if( AT_AG ){	// �O���t�B�b�N���[�h
		if( AT_GM & 4 )	bcol = COL_RG[AT_CSS][1];	// ���m�N��
		else			bcol = COL_CG[AT_CSS][0];	// �J���[
	}else{			// �A���t�@�j���[�����b�N���[�h
		if( AT_AS )		bcol = COL_SG[8];			// �Z�~�O���t�B�b�N
		else			bcol = COL_AN[4];			// �A���t�@�j���[�����b�N
	}
	
	return bcol;
}

BYTE PCZ80_07::GetBcol( void ) const
{
	BYTE bcol;
	
	// ���ۂ͑S����������
	if( SRmode ){
		if( CharMode )		bcol = COL_AN2[0];		// �L�����N�^���[�h
		else				bcol = COL_CG2[0][0];	// �O���t�B�b�N
	}else{
		if( N60Win )		bcol = COL_AN[4];		// N60
		else{										// N60m
			if( CharMode )	bcol = COL_AN2[0];		// �L�����N�^���[�h
			else			bcol = COL_CG2[0][0];	// �O���t�B�b�N
		}
	}
	
	return bcol;
}
#else			// 32bit
DWORD MC6847::GetBcol( void ) const
{
	DWORD bcol;
	
	if( AT_AG ){	// �O���t�B�b�N���[�h
		if( AT_GM & 4 )	bcol = VSurface::GetColor( COL_RG[AT_CSS][1] );	// ���m�N��
		else			bcol = VSurface::GetColor( COL_CG[AT_CSS][0] );	// �J���[
	}else{			// �A���t�@�j���[�����b�N���[�h
		if( AT_AS )		bcol = VSurface::GetColor( COL_SG[8] );			// �Z�~�O���t�B�b�N
		else			bcol = VSurface::GetColor( COL_AN[4] );			// �A���t�@�j���[�����b�N
	}
	
	return bcol;
}

DWORD PCZ80_07::GetBcol( void ) const
{
	DWORD bcol;
	
	// ���ۂ͑S����������
	if( SRmode ){
		if( CharMode )		bcol = VSurface::GetColor( COL_AN2[0] );	// �L�����N�^���[�h
		else				bcol = VSurface::GetColor( COL_CG2[0][0] );	// �O���t�B�b�N
	}else{
		if( N60Win )		bcol = VSurface::GetColor( COL_AN[4] );		// N60
		else{															// N60m
			if( CharMode )	bcol = VSurface::GetColor( COL_AN2[0] );	// �L�����N�^���[�h
			else			bcol = VSurface::GetColor( COL_CG2[0][0] );	// �O���t�B�b�N
		}
	}
	
	return bcol;
}
#endif


////////////////////////////////////////////////////////////////
// �p���b�g�ݒ�
////////////////////////////////////////////////////////////////
void PCZ80_12::SetPalette( int col, BYTE dat )
{
	PRINTD( VDG_LOG, "[VDG][SetPalette] %d ->%d\n", col, dat );
	
	int tpal  =   (col&8) | ((col&4)>>1) | ((col&2)>>1) | ((col&1)<<2);
	BYTE pdat = ( (dat&8) | ((dat&4)>>1) | ((dat&2)>>1) | ((dat&1)<<2) ) + 65;
	
	COL_AN2[tpal]   = pdat;
	COL_CG2[0][col] = pdat;
	COL_CG2[1][col] = pdat;
}
