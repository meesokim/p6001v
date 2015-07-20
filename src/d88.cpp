#include <string.h>

#include "log.h"
#include "d88.h"
#include "common.h"
#include "osd.h"


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cD88::cD88( bool dd ) : DDDrv(dd), Protected(false)
{
	PRINTD( D88_LOG, "[D88][cD88] %s Drive\n", DDDrv ? "1DD" : "1D" )
	
	INITARRAY( FileName, '\0' );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cD88::~cD88( void )
{
	if( d88.fp ) fclose( d88.fp );
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool cD88::Init( const char *fname )
{
	PRINTD( D88_LOG, "[D88][Init] %s\n", fname )
	
	strncpy( FileName, fname, PATH_MAX );
	
	// �ǎ���p�����Ȃ�v���e�N�g��ԂŊJ��
	if( OSD_FileReadOnly( FileName ) ){
		d88.fp = FOPENEN( FileName, "rb" );
		Protected = true;	// �v���e�N�g�V�[������
	}else{
		d88.fp = FOPENEN( FileName, "rb+" );
		Protected = false;	// �v���e�N�g�V�[���Ȃ�
	}
	
	if( !d88.fp ){
		*FileName = 0;
		Protected = false;
		return false;
	}
	
	ReadHeader88();	// D88 �w�b�_�Ǎ���
	
	return true;
}


////////////////////////////////////////////////////////////////
// D88 �w�b�_�Ǎ���
////////////////////////////////////////////////////////////////
void cD88::ReadHeader88( void )
{
	PRINTD( D88_LOG, "[D88][ReadHeader88]\n" )
	
	if( d88.fp ){
		// DISK��
		fread( d88.name, sizeof(BYTE), 17, d88.fp );
		d88.name[16] = '\0';
		
		// ���U�[�u��ǂ�
		fread( d88.reserve, sizeof(BYTE), 9, d88.fp );
		
		// ���C�g�v���e�N�g
		d88.protect = fgetc( d88.fp );
		if( d88.protect ) Protected = true;
		else if( Protected ) d88.protect = 0x10;
		
		// DISK�̎��
		d88.type = fgetc( d88.fp );
		
		// DISK�̃T�C�Y
		d88.size = FGETDWORD( d88.fp );
		
		// �g���b�N���̃I�t�Z�b�g�e�[�u��
		ZeroMemory( &d88.table, sizeof(d88.table) );
		// 1DD�h���C�u��1D�f�B�X�N���g������2�g���b�N��тœǍ���
		if( DDDrv && !(GetType()&FD_DOUBLETRACK) ){
			PRINTD( D88_LOG, " (1D disk on 1DD drive)\n" )
			for( int i=0; i<164; i+=2 )
				d88.table[i] = FGETDWORD( d88.fp );
		}else
			for( int i=0; i<164; i++ )
				d88.table[i] = FGETDWORD( d88.fp );
			
		// �A�N�Z�X���̃g���b�NNo
		d88.trkno = 0;
		
		PRINTD( D88_LOG, " FileName : %s\n", d88.name )
		PRINTD( D88_LOG, " Protect  : %s\n", d88.protect ? "ON" : "OFF" )
		PRINTD( D88_LOG, " Media    : %02X\n", d88.type )
		PRINTD( D88_LOG, " Size     : %d\n", (int)d88.size )
	}
}


////////////////////////////////////////////////////////////////
// D88 �Z�N�^���Ǎ���
////////////////////////////////////////////////////////////////
void cD88::ReadSector88( void )
{
	PRINTD( D88_LOG, "[D88][ReadSector88]\n" );
	
	if( d88.fp && d88.table[d88.trkno] ){
		d88.secinfo.c       = fgetc( d88.fp );		// ID �� C (�V�����_No �Жʂ̏ꍇ��=�g���b�NNo)
		d88.secinfo.h       = fgetc( d88.fp );		// ID �� H (�w�b�_�A�h���X �Жʂ̏ꍇ��=0)
		d88.secinfo.r       = fgetc( d88.fp );		// ID �� R (�g���b�N���̃Z�N�^No)
		d88.secinfo.n       = fgetc( d88.fp );		// ID �� N (�Z�N�^�T�C�Y 0:256 1:256 2:512 3:1024)
		d88.secinfo.sec_nr  = FGETWORD( d88.fp );	// ���̃g���b�N���ɑ��݂���Z�N�^�̐�
		d88.secinfo.density = fgetc( d88.fp );		// �L�^���x     0x00:�{���x   0x40:�P���x
		d88.secinfo.deleted = fgetc( d88.fp );		// DELETED MARK 0x00:�m�[�}�� 0x10:DELETED
		d88.secinfo.status  = fgetc( d88.fp );		// �X�e�[�^�X
		fread( d88.secinfo.reserve, sizeof(BYTE), 5, d88.fp );	// ���U�[�u��ǂ�
		d88.secinfo.size    = FGETWORD( d88.fp );	// ���̃Z�N�^���̃f�[�^�T�C�Y
		d88.secinfo.data    = ftell( d88.fp );		// �f�[�^�ւ̃I�t�Z�b�g
		d88.secinfo.offset  = 0;					// ���ɓǍ��ރf�[�^�̃Z�N�^�擪����̃I�t�Z�b�g
		d88.secinfo.secno++;						// �A�N�Z�X���̃Z�N�^No
		
		// Ditt�̃o�O�Ή�
		// �z�o�����C�f�[�^�T�C�Y=0�̎���00H��256�o�C�g�i�[�����o�O������炵��
		// �f�[�^�T�C�Y��0�������瑱��256�o�C�g�𒲂ׁC�S��00H��������ǂݔ�΂�
		// 00H�ȊO�̃f�[�^�����ꂽ�玟�̃Z�N�^�̃f�[�^�Ɣ��f���C�ǂݔ�΂��Ȃ��B
		// 256�o�C�g�悪�g���b�Nor�f�B�X�N�̖����ɓ��B����ꍇ���ǂݔ�΂��Ȃ�
		if( (d88.secinfo.size == 0) &&
			(((d88.trkno < 163) && (d88.table[d88.trkno+1] >= d88.secinfo.data+256)) ||
			 (d88.size >= d88.secinfo.data+256)	) ){
			
			for( int i=0; i<256; i++ ){
				if( fgetc( d88.fp ) != 0 ){
					fseek( d88.fp, d88.secinfo.data, SEEK_SET );
					break;
				}
			}
		}
	}else{
		ZeroMemory( &d88.secinfo, sizeof(D88SECTOR) );
	}
	
	PRINTD( D88_LOG, " C      : %d\n", d88.secinfo.c )
	PRINTD( D88_LOG, " H      : %d\n", d88.secinfo.h )
	PRINTD( D88_LOG, " R      : %d\n", d88.secinfo.r )
	PRINTD( D88_LOG, " N      : %d\n", d88.secinfo.n )
	PRINTD( D88_LOG, " SectNum: %d/%d\n", d88.secinfo.secno, d88.secinfo.sec_nr )
	PRINTD( D88_LOG, " Density: %s\n", d88.secinfo.density&0x40 ? "S" : "D" )
	PRINTD( D88_LOG, " Del    : %s\n", d88.secinfo.deleted&0x10 ? "DELETED" : "NORMAL" )
	PRINTD( D88_LOG, " Stat   : %02X\n", d88.secinfo.status )
	PRINTD( D88_LOG, " Size   : %d\n", d88.secinfo.size )
	PRINTD( D88_LOG, " Offset : %d\n", (int)d88.secinfo.data )
}


////////////////////////////////////////////////////////////////
// 1byte �Ǎ���
////////////////////////////////////////////////////////////////
BYTE cD88::Get8( void )
{
	BYTE dat;
	
	PRINTD( D88_LOG, "[D88][Get8] -> " )
	
	if( d88.fp && d88.table[d88.trkno] ){
		// �Z�N�^�̏I���ɓ��B�����玟�̃Z�N�^���V�[�N����
		// �ŏI�Z�N�^�̎��͓���g���b�N�̐擪�Z�N�^�Ɉړ�
		// �G���[�Z�N�^�̏ꍇ�͎��̃Z�N�^�Ɉړ����Ȃ�(Ditt!�̃G���[�Ή�)
		if( d88.secinfo.offset >= d88.secinfo.size && !d88.secinfo.status ){
			if( d88.secinfo.secno > d88.secinfo.sec_nr ) Seek( d88.trkno );
			else										 ReadSector88();
		}
		dat = fgetc( d88.fp );
		d88.secinfo.offset++;
		
		PRINTD( D88_LOG, "%02X\n", dat );
		
		return dat;
	}
	PRINTD( D88_LOG, "false(0xff)\n" );
	
	return 0xff;
}


////////////////////////////////////////////////////////////////
// 1byte ������
////////////////////////////////////////////////////////////////
bool cD88::Put8( BYTE dat )
{
	PRINTD( D88_LOG, "[D88][Put8] -> %02X(%02d:%02d:%02d:%02d)", dat, d88.secinfo.c, d88.secinfo.h, d88.secinfo.r, d88.secinfo.n );
	
	if( d88.fp && d88.table[d88.trkno] && !d88.protect ){
		// �Z�N�^�̏I���ɓ��B�����玟�̃Z�N�^���V�[�N����
		// �ŏI�Z�N�^�̎��͓���g���b�N�̐擪�Z�N�^�Ɉړ�
		if( d88.secinfo.offset >= d88.secinfo.size ){
			if( d88.secinfo.secno > d88.secinfo.sec_nr ) Seek( d88.trkno );
			else										 ReadSector88();
		}
		
		// r+,w+,a+ �ŊJ�����t�@�C���ɑ΂��ēǍ��݂Ə����݂�؂�ւ���ꍇ��
		// �K�� fsetpos,fseek,rewind �̂����ꂩ�̊֐������s����K�v������炵��
		fseek( d88.fp, 0, SEEK_CUR );
		fputc( dat, d88.fp );
		fseek( d88.fp, 0, SEEK_CUR );
		
		d88.secinfo.offset++;
		
		PRINTD( D88_LOG, " ->OK\n" );
		
		return true;
	}
	PRINTD( D88_LOG, " ->NG\n" );
	
	return false;
}


////////////////////////////////////////////////////////////////
// �V�[�N
////////////////////////////////////////////////////////////////
bool cD88::Seek( int trackno, int sectno )
{
	PRINTD( D88_LOG, "[D88][Seek] Track : %d Sector : %d ", trackno, sectno );
	
	if( d88.fp ){
		ZeroMemory( &d88.secinfo, sizeof(D88SECTOR) );
		d88.trkno          = trackno;
		d88.secinfo.status = BIOS_MISSING_IAM;
		
		// �g���b�N�������Ȃ�Unformat����
		if( !d88.table[d88.trkno] ){
			PRINTD( D88_LOG, "-> Unformat\n" );
			return false;
		}
		PRINTD( D88_LOG, "-> Track:%d\n", d88.trkno );
		
		// �g���b�N�̐擪���V�[�N
		fseek( d88.fp, d88.table[d88.trkno], SEEK_SET );
		
		// �ŏ��̃Z�N�^���Ǎ���
		ReadSector88();
		
		// �ړI�̃Z�N�^�𓪏o��
		if( sectno > 1 ){
			fseek( d88.fp, (long)d88.secinfo.size, SEEK_CUR );
			ReadSector88();
		}
		
		PRINTD( D88_LOG, "-> OK\n" );
		d88.secinfo.status = BIOS_READY;
		
		return true;
	}
	PRINTD( D88_LOG, "-> false\n" );
	
	return false;
}


////////////////////////////////////////////////////////////////
// �Z�N�^��T��
////////////////////////////////////////////////////////////////
bool cD88::SearchSector( BYTE c, BYTE h, BYTE r, BYTE n )
{
	PRINTD( D88_LOG, "[D88][SearchSector] C:%02X H:%02X R:%02X N:%02X ", c, h, r, n );
	
	if( Seek( d88.trkno ) ){
		// �ړI�̃Z�N�^�������܂ŋ�ǂ�
		while( d88.secinfo.secno <= d88.secinfo.sec_nr ){
			// ID���`�F�b�N
			if( ( d88.secinfo.c == c ) && ( d88.secinfo.h == h )
			 && ( d88.secinfo.r == r ) && ( d88.secinfo.n == n ) ){
				PRINTD( D88_LOG, "-> Found\n" );
				return true;
			}
			// ��v���Ȃ���Ύ��̃Z�N�^���Ǎ���
			fseek( d88.fp, (long)d88.secinfo.size, SEEK_CUR );
			ReadSector88();
		}
	}
	PRINTD( D88_LOG, "-> false\n" );
	
	return false;
}


////////////////////////////////////////////////////////////////
// ���̃Z�N�^�Ɉړ�����
////////////////////////////////////////////////////////////////
bool cD88::NextSector( void )
{
	PRINTD( D88_LOG, "[D88][NextSector] Sector:%d ", d88.secinfo.secno );
	
	if( d88.secinfo.sec_nr ){
		int ssize = d88.secinfo.size - d88.secinfo.offset;	// ���݂̃Z�N�^�I�[�܂ł̃f�[�^��
		
		if( d88.secinfo.secno == d88.secinfo.sec_nr )
			// �ŏI�Z�N�^�̎��͓���g���b�N�̐擪�Z�N�^�Ɉړ�
			Seek( d88.trkno );
		else{
			// ���̃Z�N�^�擪�܂ňړ����ăZ�N�^����Ǎ���
			fseek( d88.fp, (long)ssize, SEEK_CUR );
			ReadSector88();
		}
		PRINTD( D88_LOG, "-> %d\n", d88.secinfo.secno );
		
		return true;
	}
	PRINTD( D88_LOG, "-> false\n" );
	
	return false;
}


////////////////////////////////////////////////////////////////
// ���݂�CHRN�擾
////////////////////////////////////////////////////////////////
void cD88::GetID( BYTE *C, BYTE *H, BYTE *R, BYTE *N ) const
{
	PRINTD( D88_LOG, "[D88][GetID] %02X %02X %02X %02X\n", d88.secinfo.c, d88.secinfo.h, d88.secinfo.r, d88.secinfo.n );
	
	if( C ) *C = d88.secinfo.c;
	if( H ) *H = d88.secinfo.h;
	if( R ) *R = d88.secinfo.r;
	if( N ) *N = d88.secinfo.n;
}


////////////////////////////////////////////////////////////////
// ���݂̃Z�N�^�T�C�Y�擾
////////////////////////////////////////////////////////////////
WORD cD88::GetSecSize( void ) const
{
	PRINTD( D88_LOG, "[D88][GetSecSize] %d\n", d88.secinfo.size );
	
	return d88.secinfo.size;
}


////////////////////////////////////////////////////////////////
// ���݂̃g���b�N�ԍ��擾
////////////////////////////////////////////////////////////////
BYTE cD88::Track( void ) const
{
	PRINTD( D88_LOG, "[D88][Track] %d\n", d88.trkno );
	
	return d88.trkno;
}


////////////////////////////////////////////////////////////////
// ���݂̃Z�N�^�ԍ��擾
////////////////////////////////////////////////////////////////
BYTE cD88::Sector( void ) const
{
	PRINTD( D88_LOG, "[D88][Sector] %d\n", d88.secinfo.secno );
	
	return d88.secinfo.secno;
}


////////////////////////////////////////////////////////////////
// ���݂̃g���b�N���ɑ��݂���Z�N�^���擾
////////////////////////////////////////////////////////////////
WORD cD88::SecNum( void ) const
{
	PRINTD( D88_LOG, "[D88][SecNum] %d\n", d88.secinfo.sec_nr );
	
	return d88.secinfo.sec_nr;
}


////////////////////////////////////////////////////////////////
// ���݂̃X�e�[�^�X�擾
////////////////////////////////////////////////////////////////
BYTE cD88::GetSecStatus( void ) const
{
	PRINTD( D88_LOG, "[D88][GetStatus] %02X\n", d88.secinfo.status );
	
	return d88.secinfo.status;
}


////////////////////////////////////////////////////////////////
// �t�@�C�����擾
////////////////////////////////////////////////////////////////
const char *cD88::GetFileName( void ) const
{
	return FileName;
}


////////////////////////////////////////////////////////////////
// DISK�C���[�W���擾
////////////////////////////////////////////////////////////////
const char *cD88::GetDiskImgName( void ) const
{
	return (const char *)d88.name;
}


////////////////////////////////////////////////////////////////
// �v���e�N�g�V�[����Ԏ擾
////////////////////////////////////////////////////////////////
bool cD88::IsProtect( void ) const
{
	return Protected;
}


////////////////////////////////////////////////////////////////
// ���f�B�A�^�C�v�擾
////////////////////////////////////////////////////////////////
int cD88::GetType( void ) const
{
	int ret = FDUNKNOWN;
	
	switch( d88.type ){
	// �{���͂���������
//	case 0x00: ret = FD2D;  break;	// 2D
//	case 0x10: ret = FD2DD; break;	// 2DD
//	case 0x20: ret = FD2HD; break;	// 2HD
	// P6�̏ꍇ�͂���
	case 0x00: ret = FD1D;  break;	// 1D
	case 0x10: ret = FD1DD; break;	// 1DD
	}
	return ret;
}
