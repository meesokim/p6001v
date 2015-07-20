#include "pc6001v.h"

#include "common.h"
#include "log.h"
#include "osd.h"
#include "p6t2.h"


// P6T�`���t�H�[�}�b�gVer.2
//  ��{�I�ɂ́u�x�^�C���[�W+�t�b�^+�x�^�C���[�W�T�C�Y(4byte)�v�Ƃ����\��
//  �t�b�^�ɂ�DATA�u���b�N��񂪊܂܂��B
//  DATA�u���b�N�͖���������؂�Ƃ���1�Ɛ�����B
//  BASIC�Ȃǁu�w�b�_�{�f�[�^�v�Ƃ����\���̏ꍇ�CDATA�u���b�N��2�ƂȂ邪
//  ID�ԍ��𓯈�ɂ��邱�ƂŊ֘A�t���ĊǗ��ł���B
//
// [�t�b�^]
//  header (2byte) : "P6"
//  ver    (1byte) : �o�[�W����
//  dbnum  (1byte) : �܂܂��DATA�u���b�N��(255�܂�)
//  start  (1byte) : �I�[�g�X�^�[�g�t���O(0:���� 1:�L��)
//  basic  (1byte) : BASIC���[�h(PC-6001�̏ꍇ�͖��Ӗ�)
//  page   (1byte) : �y�[�W��
//  askey  (2byte) : �I�[�g�X�^�[�g�R�}���h������
//  ...�R�}���h������ꍇ�͂��̌��askey������
//  exhead (2byte) : �g�����T�C�Y(64KB�܂�)
//  ...�g����񂪂���ꍇ�͂��̌��exhead������
//
// [DATA�u���b�N]
//  header (2byte) : "TI"
//  id     (1byte) : ID�ԍ�(DATA�u���b�N���֘A�t����)
//  name  (16byte) : �f�[�^��(15����+'00H')
//  baud   (2byte) : �{�[���[�g(600/1200)
//  stime  (2byte) : �������̎���(ms)
//  ptime  (2byte) : �ҁ[���̎���(ms)
//  offset (4byte) : �x�^�C���[�W�擪����̃I�t�Z�b�g
//  size   (4byte) : �f�[�^�T�C�Y



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cP6DATA::cP6DATA( void ) : 	Data(NULL), next(NULL), before(NULL) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cP6DATA::~cP6DATA( void )
{
	if( Data ) delete [] Data;
	if( next ) delete next;
	if( before ) before->next = NULL;
}


////////////////////////////////////////////////////////////////
// �擪�u���b�N�ւ̃|�C���^��Ԃ�
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::TopBlock( void )
{
	cP6DATA *top = this;
	while( top->before ) top = top->before;
	
	return top;
}


////////////////////////////////////////////////////////////////
// �����u���b�N�ւ̃|�C���^��Ԃ�
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::LastBlock( void )
{
	cP6DATA *last = this;
	while( last->next ) last = last->next;
	
	return last;
}


////////////////////////////////////////////////////////////////
// ���̃u���b�N�ւ̃|�C���^��Ԃ�
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::Next( void )
{
	return next;
}


////////////////////////////////////////////////////////////////
// �O�̃u���b�N�ւ̃|�C���^��Ԃ�
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::Before( void )
{
	return before;
}


////////////////////////////////////////////////////////////////
// �V�KDATA�u���b�N�ǉ�
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::New( void )
{
	LastBlock()->next = new cP6DATA;
	LastBlock()->before = this;
	
	return LastBlock();
}


////////////////////////////////////////////////////////////////
// �R�s�[�쐬
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::Clone( void )
{
	cP6DATA *newb = new cP6DATA;
	*newb = *this;
	newb->next = newb->before = NULL;
	
	if( this->Data ){
		newb->Data = new BYTE[Info.DNum];
		memcpy( newb->Data, this->Data, Info.DNum );
	}
	
	return newb;
}


////////////////////////////////////////////////////////////////
// �S�R�s�[�쐬
////////////////////////////////////////////////////////////////
cP6DATA *cP6DATA::Clones( void )
{
	cP6DATA *oldb = TopBlock();
	cP6DATA *newb = TopBlock()->Clone();
	
	while( oldb->next ){
		oldb = oldb->next;
		newb->next         = oldb->Clone();
		newb->next->before = newb;
		newb = newb->LastBlock();
	}
	
	return newb->TopBlock();
}


////////////////////////////////////////////////////////////////
// P6T �u���b�N���擾
////////////////////////////////////////////////////////////////
P6TBLKINFO *cP6DATA::GetInfo( void )
{
	return &Info;
}


////////////////////////////////////////////////////////////////
// �f�[�^�Z�b�g
////////////////////////////////////////////////////////////////
int cP6DATA::SetData( FILE *fp, int num )
{
	// �Z�b�g�ς݂Ȃ�J��
	if( Data ) delete Data;
	
	// �������m��
	Data = new BYTE[num];
	if( Data ) Info.DNum = num;
	else       return 0;
	
	Info.Offset = ftell( fp );
	
	if( fp ) fread( Data, sizeof(BYTE), num, fp );
	else     ZeroMemory( Data, num );
	
	return Info.DNum;
}


////////////////////////////////////////////////////////////////
// ������,�ҁ[�����ԃZ�b�g
////////////////////////////////////////////////////////////////
void cP6DATA::SetPeriod( int stime, int ptime )
{
	PRINTD( P6T2_LOG, "[cP6DATA][SetPeriod] s:%d p:%d\n", stime, ptime );
	
	Info.STime = stime;	// �������̎���(ms)�Z�b�g
	Info.PTime = ptime;	// �ҁ[���̎���(ms)�Z�b�g
}


////////////////////////////////////////////////////////////////
// 1Byte�Ǎ���
////////////////////////////////////////////////////////////////
BYTE cP6DATA::Read( int num )
{
	if( !Data || num > Info.DNum ) return 0;
	return Data[num];
}


////////////////////////////////////////////////////////////////
// �t�@�C���ɏ�����(�f�[�^)
////////////////////////////////////////////////////////////////
int cP6DATA::Writefd( FILE *fp )
{
	Info.Offset = ftell( fp );
	
	return fwrite( Data, sizeof(BYTE), Info.DNum, fp );
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cP6PART::cP6PART( void ) : Data(NULL), next(NULL), before(NULL) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cP6PART::~cP6PART( void )
{
	if( Data ) delete Data;
	if( next ) delete next;
	if( before ) before->next = NULL;
}


////////////////////////////////////////////////////////////////
// �擪PART�ւ̃|�C���^��Ԃ�
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::TopPart( void )
{
	cP6PART *top = this;
	while( top->before ) top = top->before;
	
	return top;
}


////////////////////////////////////////////////////////////////
// ����PART�ւ̃|�C���^��Ԃ�
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::LastPart( void )
{
	cP6PART *last = this;
	while( last->next ) last = last->next;
	
	return last;
}


////////////////////////////////////////////////////////////////
// ID�ԍ����i���o�[
////////////////////////////////////////////////////////////////
BYTE cP6PART::Renumber( void )
{
	cP6PART *part = TopPart();
	
	part->Info.ID = 0;
	while( part->next ){
		part->next->Info.ID = part->Info.ID + 1;
		part = part->next;
	}
	
	return LastPart()->Info.ID;
}


////////////////////////////////////////////////////////////////
// ����PART�ւ̃|�C���^��Ԃ�
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Next( void )
{
	return next;
}


////////////////////////////////////////////////////////////////
// �O��PART�ւ̃|�C���^��Ԃ�
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Before( void )
{
	return before;
}


////////////////////////////////////////////////////////////////
// �C�ӂ�PART�ւ̃|�C���^��Ԃ�
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Part( int num )
{
	cP6PART *part = TopPart();
	
	for( int i=0; i< num; i++ ){
		if( part->next ) part = part->next;
		else             break;
	}
	
	return part;
}


////////////////////////////////////////////////////////////////
// �V�KPART�ǉ�
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::New( void )
{
	LastPart()->next = new cP6PART;
	LastPart()->before = this;
	
	return LastPart();
}


////////////////////////////////////////////////////////////////
// �R�s�[�쐬
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Clone( void )
{
	cP6PART *newp = new cP6PART;
	*newp = *this;
	newp->next = newp->before = NULL;
	
	if( this->Data ) newp->Data = this->Data->Clones();
	
	return newp;
}


////////////////////////////////////////////////////////////////
// ������PART�A��
////////////////////////////////////////////////////////////////
cP6PART *cP6PART::Link( cP6PART *src )
{
	cP6PART *last = LastPart();
	
	last->next  = src;
	src->before = last;
	
	Renumber();
	
	return src;
}


////////////////////////////////////////////////////////////////
// �V�KDATA�u���b�N�ǉ�
////////////////////////////////////////////////////////////////
cP6DATA *cP6PART::NewBlock( void )
{
	if( Data ) return Data->New();
	else       return Data = new cP6DATA;
}


////////////////////////////////////////////////////////////////
// �SDATA�u���b�N�����擾
////////////////////////////////////////////////////////////////
int cP6PART::GetBlocks( void )
{
	int num = 0;
	cP6DATA *data = Data;
	
	do{
		num ++;
		data = data->Next();
	}while( data );
	
	return num;
}


////////////////////////////////////////////////////////////////
// PART�T�C�Y�擾
////////////////////////////////////////////////////////////////
int cP6PART::GetSize( void )
{
	int size = 0;
	
	cP6DATA *data = Data;
	while( data ){
		size += data->GetInfo()->DNum;
		data  = data->Next();
	}
	
	return size;
}


////////////////////////////////////////////////////////////////
// �擪DATA�u���b�N�ւ̃|�C���^�擾
////////////////////////////////////////////////////////////////
cP6DATA *cP6PART::FirstData( void )
{
	return Data;
}


////////////////////////////////////////////////////////////////
// P6T PART���擾
////////////////////////////////////////////////////////////////
P6TPRTINFO *cP6PART::GetInfo( void )
{
	return &Info;
}


////////////////////////////////////////////////////////////////
// �f�[�^���ݒ�
////////////////////////////////////////////////////////////////
int cP6PART::SetName( const char *name )
{
	ZeroMemory( Info.Name, sizeof(Info.Name) );
	strncpy( Info.Name, name, sizeof(Info.Name)-1 );
	
	return strlen( Info.Name );
}


////////////////////////////////////////////////////////////////
// �{�[���[�g�ݒ�
////////////////////////////////////////////////////////////////
void cP6PART::SetBaud( int baud )
{
	Info.Baud = baud;
}


////////////////////////////////////////////////////////////////
// �t�@�C������SPART��Ǎ���
////////////////////////////////////////////////////////////////
bool cP6PART::Readf( FILE *fp )
{
	PRINTD( P6T2_LOG, "[cP6PART][Readf]\n" );
	
	// �t�@�C���|�C���^�͗L��?
	if( !fp ) return false;
	
	// ���ʎq "TI"(0x4954) ?
	while( ( FGETWORD( fp ) ) == 0x4954 ){
		
		BYTE NextID = fgetc( fp );	// ID�ԍ�
		
		// ����DATA�u���b�N������,ID���قȂ�Ȃ�V�KPART��ǉ�
		if( LastPart()->Data && ( LastPart()->Info.ID != NextID ) ){
			New();
			LastPart()->Info.ID = NextID;
		}
		
		// ����DATA�u���b�N������?
		if( LastPart()->Data )
			// �p��PART�Ȃ�f�[�^���ƃ{�[���[�g��ǂݔ�΂�
			fseek( fp, sizeof(Info.Name)+sizeof(Info.Baud)-1, SEEK_CUR );
		else{
			// �f�[�^��
			fread( LastPart()->Info.Name, sizeof(BYTE), sizeof(Info.Name)-1, fp );
			// �{�[���[�g
			LastPart()->Info.Baud = FGETWORD( fp );
		}
		// DATA�u���b�N�ǉ�
		cP6DATA *NewBlk = LastPart()->NewBlock();
		
		WORD stime = FGETWORD( fp );	// �������̎���(ms)
		WORD ptime = FGETWORD( fp );	// �ҁ[���̎���(ms)
		DWORD doff = FGETDWORD( fp );	// �x�^�C���[�W�擪����̃I�t�Z�b�g
		DWORD dnum = FGETDWORD( fp );	// �f�[�^�T�C�Y
		
		// �f�[�^��Ǎ���
		DWORD fps = ftell( fp );
		fseek( fp, doff, SEEK_SET );
		NewBlk->SetData( fp, dnum );
		NewBlk->SetPeriod( stime, ptime );
		fseek( fp, fps, SEEK_SET );	// �t�@�C���|�C���^������PART���ɖ߂�
	}
	
	Renumber();
	
	return true;
}


////////////////////////////////////////////////////////////////
// �t�@�C���ɏ�����(�f�[�^)
////////////////////////////////////////////////////////////////
int cP6PART::Writefd( FILE *fp )
{
	cP6DATA *data = Data;
	int size = 0;
	
	do{
		size += data->Writefd( fp );
		data = data->Next();
	}while( data );
	
	return size;
}


////////////////////////////////////////////////////////////////
// �t�@�C���ɏ�����(�t�b�^)
////////////////////////////////////////////////////////////////
bool cP6PART::Writeff( FILE *fp )
{
// [DATA�u���b�N]
//  header (2byte) : "TI"
//  id     (1byte) : ID�ԍ�(DATA�u���b�N���֘A�t����)
//  name  (16byte) : �f�[�^��(15����+'00H')
//  baud   (2byte) : �{�[���[�g(600/1200)
//  stime  (2byte) : �������̎���(ms)
//  ptime  (2byte) : �ҁ[���̎���(ms)
//  offset (4byte) : �x�^�C���[�W�擪����̃I�t�Z�b�g
//  size   (4byte) : �f�[�^�T�C�Y
	
	cP6DATA *data = Data;
	
	do{
		fputc( 'T', fp );
		fputc( 'I', fp );
		fputc( Info.ID, fp );
		fwrite( Info.Name, sizeof(BYTE), 16, fp );
		FPUTWORD( Info.Baud, fp )
		
		P6TBLKINFO *info = data->GetInfo();
		FPUTWORD( info->STime, fp );
		FPUTWORD( info->PTime, fp );
		FPUTDWORD( info->Offset, fp );
		FPUTDWORD( info->DNum, fp );
		
		data = data->Next();
	}while( data );
	
	return true;
}



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
cP6T::cP6T( void ) : Version(0), EHead(0), exh(NULL), Part(NULL),
						rpart(NULL), rdata(NULL), rpt(0), swait(0), pwait(0)
{
	INITARRAY( Name, 0 );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
cP6T::~cP6T( void )
{
	if( Part ) delete Part;
	if( Ainfo.ask ) delete [] Ainfo.ask;
	if( exh ) delete [] exh;
}


////////////////////////////////////////////////////////////////
// �SDATA�u���b�N�����擾
////////////////////////////////////////////////////////////////
int cP6T::GetBlocks( void ) const
{
	int num = 0;
	cP6PART *part = Part;
	
	do{
		num += part->GetBlocks();
		part = part->Next();
	}while( part );
	
	return num;
}


////////////////////////////////////////////////////////////////
// �SPART�����擾
////////////////////////////////////////////////////////////////
int cP6T::GetParts( void ) const
{
	int num = 0;
	cP6PART *part = Part;
	
	do{
		num ++;
		part = part->Next();
	}while( part );
	
	return num;
}


////////////////////////////////////////////////////////////////
// �C��PART�ւ̃|�C���^�擾
////////////////////////////////////////////////////////////////
cP6PART *cP6T::GetPart( int num ) const
{
	return Part->Part( num );
}


////////////////////////////////////////////////////////////////
// �x�^�C���[�W�T�C�Y�擾
////////////////////////////////////////////////////////////////
int cP6T::GetSize( void ) const
{
	int size = 0;
	cP6PART *part = Part;
	
	while( part ){
		size += part->GetSize();
		part  = part->Next();
	}
	
	return size;
}


////////////////////////////////////////////////////////////////
// �f�[�^���ݒ�
////////////////////////////////////////////////////////////////
int cP6T::SetName( const char *name )
{
	ZeroMemory( Name, 16 );
	strncpy( Name, name, 16 );
	
	return strlen( Name );
}


////////////////////////////////////////////////////////////////
// �f�[�^���擾
////////////////////////////////////////////////////////////////
const char *cP6T::GetName( void ) const
{
	return Name;
}


////////////////////////////////////////////////////////////////
// �I�[�g�X�^�[�g���擾
////////////////////////////////////////////////////////////////
const P6TAUTOINFO *cP6T::GetAutoStartInfo( void ) const
{
	return &Ainfo;
}


////////////////////////////////////////////////////////////////
// 1�����Ǎ���
////////////////////////////////////////////////////////////////
BYTE cP6T::ReadOne( void )
{
	BYTE data = rdata->Read( rpt++ );
	
	PRINTD( P6T2_LOG, "[cP6T][ReadOne] -> %02X\n", data );
	
	// �u���b�N���擾
	P6TBLKINFO *binfo = rdata->GetInfo();
	if( rpt >= binfo->DNum ){	// �f�[�^�Ō�?
		rpt = 0;
		if( !( rdata = rdata->Next() ) ){	// DATA�u���b�N�Ō�?
			if( !( rpart = rpart->Next() ) ){	// PART�Ō�?
				rpart = Part;
			}
			rdata = rpart->FirstData();
		}
		// ���̃u���b�N���擾
		binfo = rdata->GetInfo();
		swait = ( binfo->STime * DEFAULT_CMT_HZ  )/1000;
		pwait = ( binfo->PTime * DEFAULT_CMT_HZ  )/1000;
	}
	
	return data;
}


////////////////////////////////////////////////////////////////
// �������҂�?
////////////////////////////////////////////////////////////////
bool cP6T::IsSWaiting( void )
{
	if( !swait ) return false;
	swait--;
	return true;
}


////////////////////////////////////////////////////////////////
// �ҁ[���҂�?
////////////////////////////////////////////////////////////////
bool cP6T::IsPWaiting( void )
{
	if( !pwait ) return false;
	pwait--;
	return true;
}


////////////////////////////////////////////////////////////////
// ���Z�b�g(�Ǎ��݊֌W���[�N������)
////////////////////////////////////////////////////////////////
void cP6T::Reset( void )
{
	PRINTD( P6T2_LOG, "[cP6T][Reset]\n" );
	
	// �Ǎ��݊֌W���[�N������
	rpart = Part;				// ���݂̓Ǎ���PART
	rdata = Part->FirstData();	// ���݂̓Ǎ���DATA�u���b�N
	rpt   = 0;					// ���݂̓Ǎ��݃|�C���^
	
	P6TBLKINFO *binfo = rdata->GetInfo();
	swait = ( binfo->STime * DEFAULT_CMT_HZ  )/1000;	// �������̑҂���
	pwait = ( binfo->PTime * DEFAULT_CMT_HZ  )/1000;	// �ҁ[���̑҂���
}


////////////////////////////////////////////////////////////////
// �t�@�C������Ǎ���
////////////////////////////////////////////////////////////////
bool cP6T::Readf( const char *filename )
{
	PRINTD( P6T2_LOG, "[cP6T][Readf] [%s]\n", filename );
	
	if( !ReadP6T( filename ) ){		// P6T��Ǎ���
		// ���s������x�^�Ƃ݂Ȃ�P6T�ɕϊ����ēǍ���
		if( !ConvP6T( filename ) ) return false;
	}
	
	Reset();	// ���Z�b�g(�Ǎ��݊֌W���[�N������)
	
	return true;
}


////////////////////////////////////////////////////////////////
// �t�@�C���ɏ�����
////////////////////////////////////////////////////////////////
bool cP6T::Writef( const char *filename )
{
	PRINTD( P6T2_LOG, "[cP6T][Writef] [%s]\n", filename );
	
	FILE *fp;
	
	fp = FOPENEN( filename, "wb" );
	if( !fp ) return false;
	
	// �f�[�^������&�x�^�C���[�W�T�C�Y�擾
	DWORD BetaSize = 0;
	cP6PART *part = Part;
	do{
		BetaSize += part->Writefd( fp );
		part = part->Next();
	}while( part );
	

// [�t�b�^]
//  header (2byte) : "P6"
//  ver    (1byte) : �o�[�W����
//  dbnum  (1byte) : �܂܂��DATA�u���b�N��(255�܂�)
//  start  (1byte) : �I�[�g�X�^�[�g�t���O(0:���� 1:�L��)
//  basic  (1byte) : BASIC���[�h(PC-6001�̏ꍇ�͖��Ӗ�)
//  page   (1byte) : �y�[�W��
//  askey  (2byte) : �I�[�g�X�^�[�g�R�}���h������
//  ...�R�}���h������ꍇ�͂��̌��askey������
//  exhead (2byte) : �g�����T�C�Y(64KB�܂�)
//  ...�g����񂪂���ꍇ�͂��̌��exhead������

	// P6T �t�b�^������
	fputc( 'P', fp );
	fputc( '6', fp );
	fputc( Version, fp );
	fputc( GetBlocks(), fp );
	fputc( Ainfo.Start ? 1 : 0, fp );
	fputc( Ainfo.BASIC, fp );
	fputc( Ainfo.Page, fp );
	FPUTWORD( Ainfo.ASKey, fp );
	if( Ainfo.ASKey ) fwrite( Ainfo.ask, sizeof(char), Ainfo.ASKey, fp );
	FPUTWORD( EHead, fp );
	if( EHead ) fwrite( exh, sizeof(char), EHead, fp );
	
	// �SDATA�u���b�N�t�b�^������
	part = Part;
	do{
		part->Writeff( fp );
		part = part->Next();
	}while( part );
	
	// �x�^�C���[�W�T�C�Y������
	FPUTDWORD( BetaSize, fp );
	
	return true;
}


////////////////////////////////////////////////////////////////
// P6T��Ǎ���
////////////////////////////////////////////////////////////////
bool cP6T::ReadP6T( const char *filename )
{
	PRINTD( P6T2_LOG, "[cP6T][ReadP6T] [%s]\n", filename );
	
	FILE *fp;
	
	fp = FOPENEN( filename, "rb" );
	if( !fp ) return false;
	
	// �x�^�C���[�W�T�C�Y�擾
	fseek( fp, -4, SEEK_END );
	DWORD BetaSize = FGETDWORD( fp );
	// �x�^�C���[�W�T�C�Y���t�@�C���T�C�Y�𒴂��Ă�����G���[
	if( BetaSize > (DWORD)ftell( fp ) ){ fclose( fp ); return false; }
	
	// �t�b�^�̐擪�Ɉړ�
	fseek( fp, BetaSize, SEEK_SET );
	
	// ���ʎq "P6"(0x3650) �łȂ���΃G���[
	WORD Header = FGETWORD( fp );
	if( Header != 0x3650 ){ fclose( fp ); return false; }
	
	Version = fgetc( fp );						// �o�[�W����
	fgetc( fp );								// �܂܂��DATA�u���b�N��
	Ainfo.Start   = fgetc( fp ) ? true : false;	// �I�[�g�X�^�[�g�t���O
	Ainfo.BASIC   = fgetc( fp );				// BASIC���[�h
	Ainfo.Page    = fgetc( fp );				// �y�[�W��
	
	// �I�[�g�X�^�[�g�R�}���h
	Ainfo.ASKey = FGETWORD( fp );
	if( Ainfo.ASKey ){
		Ainfo.ask = new char[Ainfo.ASKey];
		fread( Ainfo.ask, sizeof(char), Ainfo.ASKey, fp );
	}
	
	// �g�����
	EHead = FGETWORD( fp );
	if( EHead ){
		exh = new char[EHead];
		fread( exh, sizeof(char), EHead, fp );
	}
	
	// PART��Ǎ���
	Part = new cP6PART;
	if( !Part->Readf( fp ) ){ fclose( fp ); return false; }
	
	fclose( fp );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �x�^��P6T�ɕϊ����ēǍ���
////////////////////////////////////////////////////////////////
bool cP6T::ConvP6T( const char *filename )
{
	PRINTD( P6T2_LOG, "[cP6T][ConvP6T] [%s]\n", filename );
	
	FILE *fp;
	
	fp = FOPENEN( filename, "rb" );
	if( !fp ) return false;
	
	// �x�^�C���[�W�T�C�Y�擾
	fseek( fp, 0, SEEK_END );
	DWORD BetaSize = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	
	// P6T���ݒ�
	SetName( OSD_GetFileNamePart( filename ) );	// �f�[�^��(16����+'00H')�̓t�@�C����
	Version = 2;								// �o�[�W����(�Ƃ肠����2)
	Ainfo.Start = false;						// �I�[�g�X�^�[�g�t���O(����)
	Ainfo.BASIC = 1;							// BASIC���[�h(PC-6001�̏ꍇ�͖��Ӗ�)(�Ƃ肠����1�������Ӗ�)
	Ainfo.Page  = 1;							// �y�[�W��(�Ƃ肠����1�������Ӗ�)
	Ainfo.ASKey = 0;							// �I�[�g�X�^�[�g�R�}���h�T�C�Y(0:����)
	Ainfo.ask   = NULL;							// �I�[�g�X�^�[�g�R�}���h�i�[�̈�ւ̃|�C���^
	EHead       = 0;							// �g�����T�C�Y(0:�Ȃ�)
	exh         = NULL;							// �g�����i�[�̈�ւ̃|�C���^
	
	// PART���쐬
	Part = new cP6PART;
	Part->SetName( "BetaImage" );	// �f�[�^��(16����+'00H')�͂Ƃ肠����"BetaImage"
	Part->SetBaud( 1200 );			// �{�[���[�g(1200)
	
	// DATA�u���b�N���쐬
	Part->NewBlock();
	Part->FirstData()->SetData( fp, BetaSize );	// �f�[�^�Z�b�g
	Part->FirstData()->SetPeriod( 3400, 6800 );	// ������(3400ms),�ҁ[��(6800ms)���ԃZ�b�g
	
	fclose( fp );
	
	return true;
}


////////////////////////////////////////////////////////////////
// TAPE�J�E���^�擾
////////////////////////////////////////////////////////////////
int cP6T::GetCount( void ) const
{
	if( !rdata ) return 0;
	
	return rdata->GetInfo()->Offset + rpt;
}


////////////////////////////////////////////////////////////////
// TAPE�J�E���^�ݒ�
////////////////////////////////////////////////////////////////
void cP6T::SetCount( int cnt )
{
	if( GetSize() <= cnt ) return;
	
	int off = 0;
	
	// �܂�PART���`�F�b�N
	rpart = Part;
	while( off+rpart->GetSize() <= cnt ){
		off   += rpart->GetSize();
		rpart  = rpart->Next();
	}
	// ����DATA���`�F�b�N
	rdata = rpart->FirstData();
	while( off+rdata->GetInfo()->DNum <= cnt ){
		off   += rdata->GetInfo()->DNum;
		rdata  = rdata->Next();
	}
	
	rpt = cnt - rdata->GetInfo()->Offset;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�SAVE
////////////////////////////////////////////////////////////////
bool cP6T::DokoSave( cIni *Ini )
{
	if( !Ini ) return false;
	
	Ini->PutEntry( "P6T", NULL, "Counter",	"%d",	GetCount() );
	Ini->PutEntry( "P6T", NULL, "swait",	"%d",	swait );
	Ini->PutEntry( "P6T", NULL, "pwait",	"%d",	pwait );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �ǂ��ł�LOAD
////////////////////////////////////////////////////////////////
bool cP6T::DokoLoad( cIni *Ini )
{
	int st;
	
	if( !Ini ) return false;
	
	Ini->GetInt( "P6T",	"Counter",	&st,	0 );
	SetCount( st );
	Ini->GetInt( "P6T",	"swait",	&swait,	swait );
	Ini->GetInt( "P6T",	"pwait",	&pwait,	pwait );
	
	return true;
}
