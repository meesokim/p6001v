#include <stdarg.h>
#include <ctype.h>

#include "log.h"
#include "console.h"
#include "common.h"


#define	BLNKW	(2)	/* �������̗]�� */
#define	BLNKH	(2)	/* �c�����̗]�� */


VSurface *JFont::ZFont = NULL;	// �S�p�t�H���g�f�[�^�T�[�t�F�X�ւ̃|�C���^
VSurface *JFont::HFont = NULL;	// ���p�t�H���g�f�[�^�T�[�t�F�X�ւ̃|�C���^
int JFont::zWidth  = 0;			// �S�p�����̕�
int JFont::zHeight = 0;			//           ����
int JFont::hWidth  = 0;			// ���p�����̕�
int JFont::hHeight = 0;			//           ����


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
JFont::JFont( void ){}

////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
JFont::~JFont( void ){}


////////////////////////////////////////////////////////////////
// �t�H���g�t�@�C�����J��
////////////////////////////////////////////////////////////////
bool JFont::OpenFont( char *zfilename, char *hfilename )
{
	VRect ff;
	
	// ���ɓǍ��܂�Ă�����j������
	CloseFont();
	
	// �Ƃ肠�����T�C�Y�w��
	hWidth  = FSIZE;
	hHeight = FSIZE * 2;
	zWidth  = FSIZE * 2;
	zHeight = FSIZE * 2;
	
	// �t�H���g�t�@�C���ǂݍ���
	// �t�H���g�t�@�C����������΃_�~�[�쐬
	if( !(HFont = LoadImg( hfilename )) ){
		HFont = new VSurface;
		HFont->InitSurface( hWidth*192, hHeight*2 );
		HFont->Fill( FC_BLACK );
		ff.w = hWidth-1;
		ff.h = hHeight-1;
		for( int y=0; y<2; y++ ){
			ff.y = hHeight*y+1;
			for( int x=0; x<192; x++ ){
				ff.x = hWidth*x+1;
				HFont->Fill( FC_WHITE, &ff );
			}
		}
	}
	if( !(ZFont = LoadImg( zfilename )) ){
		ZFont = new VSurface;
		ZFont->InitSurface( zWidth*96, zHeight*96 );
		ZFont->Fill( FC_BLACK );
		ff.w = zWidth-1;
		ff.h = zHeight-1;
		for( int y=0; y<96; y++ ){
			ff.y = zHeight*y+1;
			for( int x=0; x<96; x++ ){
				ff.x = zWidth*x+1;
				ZFont->Fill( FC_WHITE, &ff );
			}
		}
	}
	
	// ���p�ƑS�p�ŃT�C�Y���قȂ����ꍇ�͏������ق��ɍ��킹��(���R�\�����Y����)
	hWidth  = min( HFont->Width(), ZFont->Width() ) / 96 / 2;
	hHeight = min( HFont->Height() / 2, ZFont->Height() / 96 );
	zWidth  = hWidth * 2;
	zHeight = hHeight;
	
	return true;
}


////////////////////////////////////////////////////////////////
// �t�H���g��j������
////////////////////////////////////////////////////////////////
void JFont::CloseFont( void )
{
	if( HFont ){
		delete HFont;
		HFont = NULL;
	}
	if( ZFont ){
		delete ZFont;
		ZFont = NULL;
	}
}


////////////////////////////////////////////////////////////////
// ���p�����`��
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
void JFont::PutCharh( VSurface *dst, int dx, int dy, BYTE txt, BYTE  fg, BYTE  bg )
#else			// 32bit
void JFont::PutCharh( VSurface *dst, int dx, int dy, BYTE txt, DWORD fg, DWORD bg )
#endif

{
	PRINTD( GRP_LOG, "[JFont][PutCharh]\n" );
	
	int index = txt;
	
	// �N���b�s���O
	VRect sr,dr;
	sr.x = ( index % 128 ) * hWidth;
	sr.y = ( index / 128 ) * hHeight;
	dr.x = dx;
	dr.y = dy;
	sr.w = dr.w = hWidth;
	sr.h = dr.h = hHeight;
	
	// �]��
	for( int y=0; y<sr.h; y++ )
		for( int x=0; x<sr.w; x++ )
			#if INBPP == 8	// 8bit
			dst->PSet( dr.x + x, dr.y + y, HFont && HFont->PGet( sr.x + x, sr.y + y )                           ? fg : bg );
			#else			// 32bit
			dst->PSet( dr.x + x, dr.y + y, HFont && HFont->PGet( sr.x + x, sr.y + y )&(RMASK32|GMASK32|BMASK32) ? fg : bg );
			#endif
}


////////////////////////////////////////////////////////////////
// �S�p�����`��
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
void JFont::PutCharz( VSurface *dst, int dx, int dy, WORD txt, BYTE  fg, BYTE  bg )
#else			// 32bit
void JFont::PutCharz( VSurface *dst, int dx, int dy, WORD txt, DWORD fg, DWORD bg )
#endif
{
	PRINTD( GRP_LOG, "[JFont][PutCharz]\n" );
	
	BYTE high = (txt>>8) & 0xff;
	BYTE low  =  txt     & 0xff;
	
	Convert2Jis( &high, &low );
	int index = ( high - 0x20 ) * 96 + low - 0x20;
	
	// �N���b�s���O
	VRect sr,dr;
	sr.x = ( index % 96 ) * zWidth;
	sr.y = ( index / 96 ) * zHeight;
	dr.x = dx;
	dr.y = dy;
	sr.w = dr.w = zWidth;
	sr.h = dr.h = zHeight;
	
	// �]��
	for( int y=0; y<sr.h; y++ )
		for( int x=0; x<sr.w; x++ )
			#if INBPP == 8	// 8bit
			dst->PSet( dr.x + x, dr.y + y, ZFont && ZFont->PGet( sr.x + x, sr.y + y )                           ? fg : bg );
			#else			// 32bit
			dst->PSet( dr.x + x, dr.y + y, ZFont && ZFont->PGet( sr.x + x, sr.y + y )&(RMASK32|GMASK32|BMASK32) ? fg : bg );
			#endif
}







////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
ZCons::ZCons( void ) : Xmax(0), Ymax(0), x(0), y(0), fgc(FC_WHITE), bgc(FC_BLACK)
{
	INITARRAY( Caption, '\0' );
}

////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
ZCons::~ZCons( void ){}



////////////////////////////////////////////////////////////////
// �R���\�[���쐬(�������ŃT�C�Y�w��)
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
bool ZCons::Init( int winx, int winy, const char *caption, BYTE  fcol, BYTE  bcol )
#else			// 32bit
bool ZCons::Init( int winx, int winy, const char *caption, DWORD fcol, DWORD bcol )
#endif
{
	int winxr = winx * hWidth  + BLNKW * 2;
	int winyr = winy * hHeight + BLNKH * 2;
	
	return InitRes( winxr, winyr, caption, fcol, bcol );
}


////////////////////////////////////////////////////////////////
// �R���\�[���쐬(�𑜓x�ŃT�C�Y�w��)
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
bool ZCons::InitRes( int winx, int winy, const char *caption, BYTE  fcol, BYTE  bcol )
#else			// 32bit
bool ZCons::InitRes( int winx, int winy, const char *caption, DWORD fcol, DWORD bcol )
#endif
{
	// �T�[�t�F�X�쐬
	if( !VSurface::InitSurface( winx, winy ) ) return false;
	
	fgc = fcol;
	bgc = bcol;
	
	// �T�[�t�F�X�S�̂�w�i�F�œh��Ԃ�
	VSurface::Fill( bgc );
	
	// �c���ő啶�����ݒ�
	Xmax = ( winx - BLNKW * 2 ) / hWidth;
	Ymax = ( winy - BLNKH * 2 ) / hHeight;
	
	// �`��͈͐ݒ�
	con.x = BLNKW;
	con.y = BLNKH;
	con.w = Xmax * hWidth;
	con.h = Ymax * hHeight;
	
	if( caption ){	// �L���v�V��������(�t���[������)�̏ꍇ
		// �L���v�V�����ۑ�
		strncpy( Caption, caption, min( Xmax-2, (int)sizeof(Caption)-1 ) );
		
		// �t���[���`��
		DrawFrame();
		
		// �c���ő啶�����ݒ�
		Xmax -= 2;
		Ymax -= 2;
		
		// �`��͈͐ݒ�
		con.x += hWidth;
		con.y += hHeight;
		con.w  = Xmax * hWidth;
		con.h  = Ymax * hHeight;
	}
	
	x = y = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// �J�[�\���ʒu�ݒ�
////////////////////////////////////////////////////////////////
void ZCons::Locate( int xx, int yy )
{
	// �E�[�C���[�`�F�b�N
	// ����������E�[�C���[����
	if( ( xx >= 0 )&&( xx < Xmax ) ) x = xx;
	else if( ( xx < 0 )&&( (Xmax-xx)>=0 ) ) x = Xmax + xx;
	
	if( ( yy >= 0 )&&( yy < Ymax ) ) y = yy;
	else if( ( yy < 0 )&&( (Ymax-yy)>=0 ) ) y = Ymax + yy;
}


////////////////////////////////////////////////////////////////
// �J�[�\���ʒu�ݒ�(�Ԑڍ��W)
////////////////////////////////////////////////////////////////
void ZCons::LocateR( int xx, int yy )
{
	x += xx;
	if( x < 0 ){
	    x += Xmax;
	    y--;
	}
	if( x > Xmax ){
		x -= Xmax;
		y++;
	}
	
	y += yy;
	if( y < 0 )    y = 0;
	if( y > Ymax ) y = Ymax;
}


////////////////////////////////////////////////////////////////
// �`��F�ݒ�
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
void ZCons::SetColor( BYTE  fg, BYTE  bg )
#else			// 32bit
void ZCons::SetColor( DWORD fg, DWORD bg )
#endif
{
	fgc = fg;
	bgc = bg;
}

#if INBPP == 8	// 8bit
void ZCons::SetColor( BYTE  fg )
#else			// 32bit
void ZCons::SetColor( DWORD fg )
#endif
{
	fgc = fg;
}


////////////////////////////////////////////////////////////////
// ��ʏ���
////////////////////////////////////////////////////////////////
void ZCons::Cls( void )
{
	// �`��͈͂�w�i�F�œh��Ԃ�
	if( VSurface::pixels ) VSurface::Fill( bgc, &con );
	// �J�[�\�����z�[���ɖ߂�
	x = y = 0;
}


////////////////////////////////////////////////////////////////
// ���p1�����`��
////////////////////////////////////////////////////////////////
void ZCons::PutCharH( BYTE c )
{
	JFont::PutCharh( this, x * hWidth + con.x, y * hHeight + con.y, c, fgc, bgc );
	
	// ���̃J�[�\����ݒ�
	x++;
}


////////////////////////////////////////////////////////////////
// �S�p1�����`��
////////////////////////////////////////////////////////////////
void ZCons::PutCharZ( WORD c )
{
	JFont::PutCharz( this, x * hWidth + con.x, y * hHeight + con.y, c, fgc, bgc );
	
	// ���̃J�[�\����ݒ�
	x += 2;
}


////////////////////////////////////////////////////////////////
// �����t������`��(���䕶����Ή�)
////////////////////////////////////////////////////////////////
void ZCons::Print( const char *text, ... )
{
	BYTE buf[1024];
	int num = 0;
	va_list ap;
	
	// �ϒ������W�J�i������ɕϊ��j
	va_start( ap, text );
	num = vsprintf( (char *)buf, text, ap );
	
	for( int i=0; i<num; i++ ){
		if( isprint( buf[i] ) )
			PutCharH( buf[i] );
		else{
			PutCharZ( buf[i]<<8 | buf[i+1] );
			i++;
		}
	}
}


////////////////////////////////////////////////////////////////
// �����t������`��(���䕶���Ή�)
////////////////////////////////////////////////////////////////
void ZCons::Printf( const char *text, ... )
{
	BYTE buf[1024];
	int num = 0;
	va_list ap;
	
	// �ϒ������W�J�i������ɕϊ��j
	va_start( ap, text );
	num = vsprintf( (char *)buf, text, ap );
	
	for( int i=0; i<num; i++ ){
		switch( buf[i] ){
		case '\n':	// ���s
			x = 0;
			y++;
			break;
			
		default:	// ���ʂ̕���
			if( isprint( buf[i] ) )
				PutCharH( buf[i] );
			else{
				PutCharZ( buf[i]<<8 | buf[i+1] );
				i++;
			}
			
			// ���̃J�[�\����ݒ�
			if( x >= Xmax ){
				x = 0;
				y++;
			}
		}
		
		// �X�N���[��?
		if( y >= Ymax){
			y = Ymax - 1;
			ScrollUp();
		}
	}
}


////////////////////////////////////////////////////////////////
// �����t������`��(�E�l��)
////////////////////////////////////////////////////////////////
void ZCons::Printfr( const char *text, ... )
{
	char buf[1024];
	int num = 0;
	va_list ap;
	
	// �ϒ������W�J�i������ɕϊ��j
	va_start( ap, text );
	num = vsprintf( buf, text, ap );
	
	if( num > Xmax ) num = Xmax;
	Locate( -num, y );
	
	for( int i=0; i<num; i++ ){
		if( isprint( buf[i] ) )
			PutCharH( buf[i] );
		else{
			PutCharZ( buf[i]<<8 | buf[i+1] );
			i++;
		}
	}
}


////////////////////////////////////////////////////////////////
// ���ő啶�����擾
////////////////////////////////////////////////////////////////
int ZCons::GetXline( void )
{
	return Xmax;
}


////////////////////////////////////////////////////////////////
// �c�ő啶�����擾
////////////////////////////////////////////////////////////////
int ZCons::GetYline( void )
{
	return Ymax;
}


////////////////////////////////////////////////////////////////
// �g�`��
////////////////////////////////////////////////////////////////
void ZCons::DrawFrame( void )
{
	VRect frm;
	
	frm.x = con.x;
	frm.y = con.y + 4;
	frm.w = con.w;
	frm.h = con.h - 8;
	VSurface::Fill( fgc, &frm );
	
	frm.x += 1;
	frm.y += 1;
	frm.w -= 2;
	frm.h -= 2;
	VSurface::Fill( bgc, &frm );
	
	// �L���v�V����
	if( strlen( Caption ) > 0 ){
		Locate( 1, 0 );
		Print( " %s ", Caption );
	}
}


////////////////////////////////////////////////////////////////
// �X�N���[���A�b�v
////////////////////////////////////////////////////////////////
void ZCons::ScrollUp( void )
{
	VRect SPos,DPos;
	
	// �]����
	SPos.x = con.x;
	SPos.y = con.y + hHeight;
	SPos.w = con.w;
	SPos.h = con.h - hHeight;
	
	// �]����
	DPos.x = con.x;
	DPos.y = con.y;
	
	// �X�N���[��
	VSurface::Blit( &SPos, this, &DPos );
	
	
	// �`��͈͂�w�i�F�œh��Ԃ�
	DPos.x = con.x;
	DPos.y = con.y + con.h - hHeight;
	DPos.w = con.w;
	DPos.h = hHeight;
	VSurface::Fill( bgc, &DPos );
}
