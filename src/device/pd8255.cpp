#include "../log.h"
#include "pd8255.h"


// �n���h�V�F�C�N�p����r�b�g
#define	HS_INT	(0x08)	/* bit3 */
#define	HS_STB	(0x10)	/* bit4 */
#define	HS_IBF	(0x20)	/* bit5 */
#define	HS_DAK	(0x40)	/* bit6 */
#define	HS_OBF	(0x80)	/* bit7 */

#define	HS_RIE	(0x10)	/* bit4 */
#define	HS_WIE	(0x40)	/* bit6 */


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cD8255::cD8255( void )
{
	Reset();
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cD8255::~cD8255(){}


////////////////////////////////////////////////////////////////
// ���Z�b�g
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void cD8255::Reset( void )
{
	PortA    = PortB    = PortC     = PortAbuf  = 0;
	ModeA    = ModeB    = 0;
	PortAdir = PortBdir = PortC1dir = PortC2dir = true;
	
	HSINT0 = HSWINT0 = HSRINT0 = HSIBF0 = false;
	HSSTB0 = HSDAK0  = HSOBF0  = true;
	RIE0   = WIE0    = false;
}


////////////////////////////////////////////////////////////////
// ���[�h�ݒ�
//
// ����:	data	�f�[�^�]������
//				bit6,5: �O���[�vA�̃��[�h	1x:���[�h2 01:���[�h1 00:���[�h0
//				bit4:   PortA				1:����(�ǂ�) 0:�o��(����)
//				bit3:   PortC(bit4-7)		1:����(�ǂ�) 0:�o��(����)
//				bit2:   �O���[�vB�̃��[�h	1:���[�h1 0:���[�h0
//				bit1:   PortB				1:����(�ǂ�) 0:�o��(����)
//				bit0:   PortC(bit0-3)		1:����(�ǂ�) 0:�o��(����)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void cD8255::SetMode( BYTE data )
{
	ModeA     = (data>>6)&1 ? 2    : (data>>5)&1;
	ModeB     = (data>>2)&1;
	PortAdir  = (data>>4)&1 ? true : false;
	PortC2dir = (data>>3)&1 ? true : false;
	PortBdir  = (data>>1)&1 ? true : false;
	PortC1dir = (data   )&1 ? true : false;
	
	PortA = PortB = PortC = 0;
	if( ModeA == 2 ) PortC |= HS_OBF;
	
	PRINTD( PPI_LOG, "[8255][SetMode] GrA:%d GrB:%d\n", ModeA, ModeB );
}


////////////////////////////////////////////////////////////////
// PartA ���C�g(���ӑ�)
//
// ����:	data	�������ރf�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void cD8255::WriteAE( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteAE] %02X", data );
	
	// �Ƃ肠�������[�h2�̏ꍇ�����l����
	if( ModeA == 2 ){
		// �o�b�t�@�Ƀf�[�^�����b�`
		PortAbuf = data;
		// ������
		if( HSSTB0 ){
			// IBF=H
			HSIBF0   = true;
			
			PRINTD( PPI_LOG, " DOWN IBF:%d RINT:%d DATA:%02X", HSIBF0 ? 1 : 0, HSRINT0 ? 1 : 0, data );
		}
		HSSTB0 = false;
		// �����
		if( !HSSTB0 ){
			// IBF=H �Ȃ�RINT=H
			if( HSIBF0 ) HSRINT0 = true;
			
			PRINTD( PPI_LOG, " UP IBF:%d RINT:%d", HSIBF0 ? 1 : 0, HSRINT0 ? 1 : 0 );
		}
		HSSTB0 = true;
	}
	
	PRINTD( PPI_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// PartA ���[�h(���ӑ�)
//
// ����:	�Ȃ�
// �Ԓl:	BYTE	PortA�̒l
////////////////////////////////////////////////////////////////
BYTE cD8255::ReadAE( void )
{
	PRINTD( PPI_LOG, "[8255][ReadAE]" );
	
	// �Ƃ肠�������[�h2�̏ꍇ�����l����
	if( ModeA == 2 ){
		// ������Ȃ�OBF=H
		if( HSDAK0 ){
			HSOBF0 = true;
			
			PRINTD( PPI_LOG, " DN OBF:%d WINT %d", HSOBF0 ? 1 : 0, HSWINT0 ? 1 : 0 );
		}
		HSDAK0 = false;
		// �����
		if( !HSDAK0 ){
			// OBF=H �Ȃ�WRINT=H
			if( HSOBF0 ) HSWINT0 = true;
			
			PRINTD( PPI_LOG, " UP OBF:%d WINT %d", HSOBF0 ? 1 : 0, HSWINT0 ? 1 : 0 );
		}
		HSDAK0 = true;
	}
	
	PRINTD( PPI_LOG, "\n" );
	
	return PortA;
}


////////////////////////////////////////////////////////////////
// PartA ���C�g
//
// ����:	data	�������ރf�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void cD8255::WriteA( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteA] %02X", data );
	
	if( !PortAdir ){
		PortA = data;
		
		// �Ƃ肠�������[�h2�̏ꍇ�����l����
		if( ModeA == 2 ){
			// WR0�������WINT=L
			HSWINT0 = false;
			// WR0������ DAK=H �Ȃ� OBF=L
			if( HSDAK0 ) HSOBF0 = false;
			
			PRINTD( PPI_LOG, " WINT:%d OBF %d\n", HSWINT0 ? 1 : 0, HSOBF0 ? 1 : 0 );
		}
		JobWriteA( PortA );
	}else{
		PRINTD( PPI_LOG, "\n" );
	}
}


////////////////////////////////////////////////////////////////
// PartB ���C�g
//
// ����:	data	�������ރf�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void cD8255::WriteB( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteB] %02X", data );
	
	if( !PortBdir ){
		PortB = data;
		JobWriteB( PortB );
	}
	
	PRINTD( PPI_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// PartC ���C�g
//
// ����:	data	�������ރf�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void cD8255::WriteC( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteC] %02X", data );
	
	// ���ڏ����݂̓��[�h0�̎��ɂ̂݉\
	
	// �O���[�vA�����[�h0  �������� ���ʂ�bit0-3 ��ʂ�bit4-7
	//            ���[�h1,2�������� ���ʂ�bit0-2 ��ʂ�bit3-7
	
	
	if( ModeA == 0 ){
		// �O���[�vA�����[�h0,�O���[�vB�����[�h1�������牺��(bit0-3)���}�X�N
		if( ModeB == 1 ) data = ( PortC & 0x0f ) | ( data & 0xf0 );
	}else{
		// �O���[�vA�����[�h1,2����������(bit3-7)���}�X�N
		PortC = ( PortC & 0xf8 ) | ( data & 0x07 );
		// �O���[�vA�����[�h1,2,�O���[�vB�����[�h1��������S�ă}�X�N
		if( ModeB == 1 ) data = PortC;
	}
	
	// ����
	if( !PortC1dir && ModeB == 0 ){
		if( ModeA == 0 ) PortC = ( PortC & 0xf0 ) | ( data & 0x0f );
		else             PortC = ( PortC & 0xf8 ) | ( data & 0x07 );
		JobWriteC1( PortC );
	}
	// ���
	if( !PortC2dir && ModeA == 0 ){
		PortC = ( PortC & 0x07 ) | ( data & 0xf8 );
		JobWriteC2( PortC );
	}
	
	PRINTD( PPI_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// PartD ���C�g(�R���g���[���|�[�g)
//
// ����:	data	�������ރf�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void cD8255::WriteD( BYTE data )
{
	PRINTD( PPI_LOG, "[8255][WriteD] %02X", data );
	
	if( data&0x80 ){	// �ŏ�ʃr�b�g��1�Ȃ�
		// ���[�h�I��
		SetMode( data );
	}else{				// �ŏ�ʃr�b�g��0�Ȃ�
		// �r�b�g�Z�b�g/���Z�b�g
		// ���[�h2�őΏ�bit��INT(bit3),IBF(bit5),OBF(bit7)�̏ꍇ��(���̓|�[�g�Ȃ̂�)�}�X�N����
		// ���̃��[�h�̓m�[�P�A
		if( ModeA == 2 ){
			// bit���̑Ή�
			switch( (data>>1)&0x07 ){
			case 4: // RIE0
				RIE0 = ( data&1 ) ? true : false;
				break;
			
			case 6: // WIE0
				WIE0 = ( data&1 ) ? true : false;
				break;
			
			case 3: // INT0
			case 5: // IBF0
			case 7: // OBF0
				break;
			
			default:	// �܂�bit0-2
				if( data&1 ) PortC |=   1<<((data>>1)&0x07);
				else         PortC &= ~(1<<((data>>1)&0x07));
			}
		}else{
			if( data&1 ) PortC |=   1<<((data>>1)&0x07);
			else         PortC &= ~(1<<((data>>1)&0x07));
		}
		JobWriteD( data );
	}
	
	PRINTD( PPI_LOG, "\n" );
}


////////////////////////////////////////////////////////////////
// PartA ���[�h
//
// ����:	�Ȃ�
// �Ԓl:	BYTE	PortA�̒l
////////////////////////////////////////////////////////////////
BYTE cD8255::ReadA( void )
{
	// �o�̓|�[�g�ł����̂܂ܓǍ��߂�
	
	JobReadA();
	// �Ƃ肠�������[�h2�̏ꍇ�����l����
	if( ModeA == 2 ){
		// RD0�������RINT=L
		HSRINT0 = false;
		
		// IBF=H�Ȃ�o�b�t�@����f�[�^��Ǎ���
		if( HSIBF0 ){
			PortA = PortAbuf;
		}
		
		// RD0������ STB=H �Ȃ� IBF=L
		if( HSSTB0 ) HSIBF0 = false;
	}
	
	PRINTD( PPI_LOG, "[8255][Read][PortA] %02X STB:%d IBF:%d\n", PortA, HSSTB0, HSIBF0 );
	return PortA;
}


////////////////////////////////////////////////////////////////
// PartB ���[�h
//
// ����:	�Ȃ�
// �Ԓl:	BYTE	PortB�̒l
////////////////////////////////////////////////////////////////
BYTE cD8255::ReadB( void )
{
	// �o�̓|�[�g�ł����̂܂ܓǍ��߂�
	
	JobReadB();
	return PortB;
}


////////////////////////////////////////////////////////////////
// PartC ���[�h
//
// ����:	�Ȃ�
// �Ԓl:	BYTE	PortC�̒l
////////////////////////////////////////////////////////////////
BYTE cD8255::ReadC( void )
{
	// �o�̓|�[�g�ł����̂܂ܓǍ��߂�
	
	JobReadC();
	// �Ƃ肠�������[�h2�̏ꍇ�����l����
	if( ModeA == 2 ){
		HSWINT0 = ( HSOBF0 && WIE0 && HSDAK0 ) ? true : false;
		HSRINT0 = ( HSIBF0 && RIE0 && HSSTB0 ) ? true : false;
		HSINT0  = ( HSWINT0 || HSRINT0 ) ? true : false;
		
		PortC = ( HSOBF0 ? HS_OBF : 0 ) | ( WIE0   ? HS_WIE : 0 ) | ( HSIBF0 ? HS_IBF : 0 ) |
				( RIE0   ? HS_RIE : 0 ) | ( HSINT0 ? HS_INT : 0 ) | ( PortC & 0x07 );
	}
	PRINTD( PPI_LOG, "[8255][Read][PortC] OBF:%d WIE:%d IBF:%d RIE:%d INT:%d ", HSOBF0, WIE0, HSIBF0, RIE0, HSINT0  );
	PRINTD( PPI_LOG, "WINT:%d RINT:%d\n", HSWINT0, HSRINT0 );
	return PortC;
}


////////////////////////////////////////////////////////////////
// IBF�擾
//
// ����:	�Ȃ�
// �Ԓl:	bool	�X�e�[�^�X
////////////////////////////////////////////////////////////////
bool cD8255::GetIBF( void )
{
	// �Ƃ肠�������[�h2�̏ꍇ�����l����
	if( ModeA == 2 ) return HSIBF0;
	
	return false;
}


////////////////////////////////////////////////////////////////
// OBF�擾
//
// ����:	�Ȃ�
// �Ԓl:	bool	�X�e�[�^�X
////////////////////////////////////////////////////////////////
bool cD8255::GetOBF( void )
{
	// �Ƃ肠�������[�h2�̏ꍇ�����l����
	if( ModeA == 2 ) return HSOBF0;
	
	return false;
}


