#include "pc6001v.h"

#include "common.h"
#include "config.h"
#include "debug.h"
#include "graph.h"
#include "log.h"
#include "osd.h"
#include "status.h"
#include "vdg.h"

#include "p6el.h"


// �ʏ�E�B���h�E�T�C�Y
#define	P6WINW		(vm->vdg->Width()*2/vm->vdg->XScale())
#define	P6WINH		(vm->vdg->Height()*2)

#define	HBBUS		((282-12)*2)	/* 4:3�\������ʍ��� */

// �t���X�N���[���E�B���h�E�T�C�Y
#define	P6WIFW		640
#define	P6WIFH		480

#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// ���j�^���[�h�E�B���h�E�T�C�Y
#define	P6DEBUGW	(max(P6WINW/2,vm->el->monw->Width())+max(vm->el->regw->Width(),vm->el->memw->Width()))
#define	P6DEBUGH	(max(P6WINH/2+vm->el->monw->Height(),vm->el->regw->Height()+vm->el->memw->Height()))
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@


// ���j�^���[�h���̓t���X�N���[���C�X�L�������C���C4:3�\���֎~
// �t���X�N���[�����[�h���̓X�e�[�^�X�o�[�\���֎~
#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define	DISPMON		vm->el->IsMonitor()
#define	DISPFULL	(!DISPMON  && vm->el->cfg->GetFullScreen())
#define	DISPSCAN	(!DISPMON  && vm->el->cfg->GetScanLine())
#define	DISPNTSC	(!DISPMON  && vm->el->cfg->GetDispNTSC())
#else
#define	DISPFULL	vm->el->cfg->GetFullScreen()
#define	DISPSCAN	vm->el->cfg->GetScanLine()
#define	DISPNTSC	vm->el->cfg->GetDispNTSC()
#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define	DISPSTAT	(!DISPFULL && vm->el->cfg->GetDispStat())


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
//DSP6::DSP6( VM6 *pvm ) : vm(pvm), Wh(NULL), SBuf(NULL) {}
DSP6::DSP6( VM6 *pvm ) : vm(pvm), Wh(NULL) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
DSP6::~DSP6( void )
{
//	if( SBuf ) delete SBuf;
	if( Wh ) OSD_DestroyWindow( Wh );
}


////////////////////////////////////////////////////////////////
// ������
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool DSP6::Init( void )
{
	PRINTD( GRP_LOG, "[GRP][Init]\n" );
	
	// �X�N���[���T�[�t�F�X�쐬
	if( !SetScreenSurface() ) return false;
	
	return true;
}


////////////////////////////////////////////////////////////////
// ������
//
// ����:	model	�@�� 60,62,66
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void DSP6::SetIcon( const int model )
{
	OSD_SetIcon( Wh, model );
}


////////////////////////////////////////////////////////////////
// �X�N���[���T�[�t�F�X�쐬
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool DSP6::SetScreenSurface( void )
{
	PRINTD( GRP_LOG, "[GRP][SetScreenSurface]\n" );
	
	int x = 0, y = 0;
	bool fsflag = false;
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( DISPMON ){	// ���j�^���[�h?
		x      = P6DEBUGW;
		y      = P6DEBUGH;
		fsflag = false;
		
		PRINTD( GRP_LOG, " -> Monitor Mode ( X:%d Y:%d )\n", x, y );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
		if( DISPFULL ){	// �t���X�N���[��?
			// Mac �̃t���X�N���[���� 640x480, 800x600, 1024x768 �Ɍ�����炵���Bby Windy����
			// �Ƃ肠�����ⓚ���p��640x480�ɌŒ肵�Ă݂�
			x      = P6WIFW;
			y      = P6WIFH;
			fsflag = true;
			
			PRINTD( GRP_LOG, " -> FullScreen ( X:%d Y:%d )\n", x, y );
		}else{
			x      = P6WINW;
			y      = (DISPNTSC ? HBBUS : P6WINH) + ( DISPSTAT ? vm->el->staw->Height() : 0 );
			fsflag = false;
			
			PRINTD( GRP_LOG, " -> Window ( X:%d Y:%d )\n", x, y );
		}
	}
	
	// �X�N���[���T�[�t�F�X�쐬
	OSD_CreateWindow( &Wh, x, y, fsflag );
	if( !Wh ){
		PRINTD( GRP_LOG, " -> Failed\n" );
		return false;
	}else{
		PRINTD( GRP_LOG, " -> OK ( %d x %d )\n", OSD_GetWindowWidth( Wh ), OSD_GetWindowHeight( Wh ) );
	}
	
	if( DISPFULL ){	// �t���X�N���[���̎�
		// �}�E�X�J�[�\��������
		OSD_ShowCursor( false );
	}else{			// �t���X�N���[���łȂ���
		// �}�E�X�J�[�\����\������
		OSD_ShowCursor( true );
		// �E�B���h�E�L���v�V�����ݒ�
		OSD_SetWindowCaption( Wh, vm->el->cfg->GetCaption() );
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// �X�N���[���T�C�Y�ύX
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool DSP6::ResizeScreen( void )
{
	PRINTD( GRP_LOG, "[GRP][ResizeScreen]\n" );
	
	int x, y;
	
	// �E�B���h�E�T�C�Y�`�F�b�N
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( DISPMON ){	// ���j�^���[�h?
		x = P6DEBUGW;
		y = P6DEBUGH;
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
		x = DISPFULL ? P6WIFW : P6WINW;
		y = DISPFULL ? P6WIFH : DISPNTSC ? HBBUS : P6WINH;
		
		// �X�e�[�^�X�o�[�\��?
		if( !DISPFULL && DISPSTAT ) y += vm->el->staw->Height();
	}
	
	// �E�B���h�E�T�C�Y���s�K�؂Ȃ��蒼��
	if( !Wh || (x != OSD_GetWindowWidth( Wh )) || (y != OSD_GetWindowHeight( Wh )) ){
		if( !SetScreenSurface() ) return false;
		vm->el->staw->Init( OSD_GetWindowWidth( Wh ) );	// �X�e�[�^�X�o�[��
	}else
		// ��蒼���Ȃ��ꍇ�͌��݂̃X�N���[���T�[�t�F�X���N���A
		OSD_ClearWindow( Wh );
	
	return true;
}


////////////////////////////////////////////////////////////////
// ��ʍX�V
//
// ����:	�Ȃ�
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void DSP6::DrawScreen( void )
{
	PRINTD( GRP_LOG, "[GRP][DrawScreen]\n" );
	
	VSurface *BBuf = vm->vdg;		// �o�b�N�o�b�t�@�ւ̃|�C���^�擾
	
	if( !Wh || !BBuf ) return;
	
	// �X�N���[���T�[�t�F�X��blit
	PRINTD( GRP_LOG, " -> Blit" );
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	if( DISPMON ){	// ���j�^���[�h?
		PRINTD( GRP_LOG, " -> Monitor" );
		
		// �o�b�N�o�b�t�@
		OSD_BlitToWindow( Wh, BBuf, 0, 0 );
		
		// ���j�^�E�B���h�E
		OSD_BlitToWindow( Wh, vm->el->monw, 0, P6WINH/2 );
		
		// ���W�X�^�E�B���h�E
		OSD_BlitToWindow( Wh, vm->el->regw, max( P6WINW/2, vm->el->monw->Width() ), 0 );
		
		// �������E�B���h�E
		OSD_BlitToWindow( Wh, vm->el->memw, max( P6WINW/2, vm->el->monw->Width() ), vm->el->regw->Height() );
	}else
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	{
		if( DISPFULL ){	// �t���X�N���[���\��
			PRINTD( GRP_LOG, " -> FullScreen" );
			OSD_BlitToWindowEx( Wh, BBuf, ( OSD_GetWindowWidth( Wh ) - ScreenX() ) / 2, ( OSD_GetWindowHeight( Wh ) - ScreenY() ) / 2, ScreenY(), DISPNTSC, DISPSCAN, vm->el->cfg->GetScanLineBr() );
		}else{			// �E�B���h�E�\��
			PRINTD( GRP_LOG, " -> Window" );
			OSD_BlitToWindowEx( Wh, BBuf, 0, 0, ScreenY(), DISPNTSC, DISPSCAN, vm->el->cfg->GetScanLineBr() );
		}
		
		// �X�e�[�^�X�o�[
		if( DISPSTAT ){
			PRINTD( GRP_LOG, " -> Statusbar" );
			
			// �X�e�[�^�X�o�[�X�V
			// �X�N���[���T�[�t�F�X���[�Ɉʒu�����킹��blit
			vm->el->staw->Update();
			OSD_BlitToWindow( Wh, vm->el->staw, 0, OSD_GetWindowHeight( Wh ) - vm->el->staw->Height() );
		}
	}
	
	PRINTD( GRP_LOG, " -> OK\n" );
	
	// �t���b�v
	OSD_RenderWindow( Wh );
}


////////////////////////////////////////////////////////////////
// �L���X�N���[�����擾
//
// ����:	�Ȃ�
// �Ԓl:	int		�L���X�N���[�����s�N�Z����
////////////////////////////////////////////////////////////////
int DSP6::ScreenX( void ) const
{
	return DISPFULL ? P6WIFW : P6WINW;
}


////////////////////////////////////////////////////////////////
// �L���X�N���[�������擾
//
// ����:	�Ȃ�
// �Ԓl:	int		�L���X�N���[�������s�N�Z����
////////////////////////////////////////////////////////////////
int DSP6::ScreenY( void ) const
{
	return DISPFULL ? P6WIFH : DISPNTSC ? HBBUS : P6WINH;
}


////////////////////////////////////////////////////////////////
// �T�u�o�b�t�@�擾
//
// ����:	�Ȃ�
// �Ԓl:	VSurface *	�T�u�o�b�t�@�ւ̃|�C���^
////////////////////////////////////////////////////////////////
VSurface *DSP6::GetSubBuffer( void )
{
	return NULL;
//	return SBuf;
}


////////////////////////////////////////////////////////////////
// �E�B���h�E�n���h���擾
//
// ����:	�Ȃ�
// �Ԓl:	HWINDOW *	�E�B���h�E�n���h��
////////////////////////////////////////////////////////////////
HWINDOW DSP6::GetWindowHandle( void )
{
	return (HWINDOW)Wh;
}


////////////////////////////////////////////////////////////////
// �T�u�o�b�t�@�X�V
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
#define	RESO43		256		/* ���ԐF�v�Z�p����\(1���C����RESO43��������) */
/*
bool DSP6::UpdateSubBuf( void )
{
	PRINTD( GRP_LOG, "[GRP][UpdateSubBuf]\n" );
	
	VSurface *BBuf = vm->vdg;
	
	if( !BBuf ) return false;
	if( !RefreshSubBuf() ) return false;
	
	const bool dntsc = DISPNTSC;
	const bool dscan = DISPSCAN;
	const int brscan = vm->el->cfg->GetScanLineBr();
	const int xsc    = BBuf->XScale();
	const int yy     = SBuf->Height();
	const int xx     = BBuf->Width();
	
	for( int y=0; y<yy; y++ ){
		DWORD y0 = ( y * BBuf->Height() ) / yy;
		DWORD a1 = ( y * BBuf->Height() * RESO43 ) / yy - y0 * RESO43;
		DWORD a2 = RESO43 - a1;
		
		DWORD *sof1 = (DWORD *)BBuf->GetPixels() + BBuf->Pitch()/sizeof(DWORD) * y0;
		DWORD *sof2 = sof1 + ( y0 < (DWORD)BBuf->Height()-1 ? BBuf->Pitch()/sizeof(DWORD) : 0 );
		DWORD *doff = (DWORD *)SBuf->GetPixels() + SBuf->Pitch()/sizeof(DWORD) * y ;
		
		for( int x=0; x<xx; x++ ){
			DWORD r,g,b;
			DWORD d1 = *sof1++;
			DWORD d2 = *sof2++;
			
			if( dntsc ){
				r = ( ( ( (d1>>RSHIFT32)&0xff ) * a2 + ( (d2>>RSHIFT32)&0xff ) * a1 ) / RESO43 ) & 0xff;
				g = ( ( ( (d1>>GSHIFT32)&0xff ) * a2 + ( (d2>>GSHIFT32)&0xff ) * a1 ) / RESO43 ) & 0xff;
				b = ( ( ( (d1>>BSHIFT32)&0xff ) * a2 + ( (d2>>BSHIFT32)&0xff ) * a1 ) / RESO43 ) & 0xff;
			}else{
				r = (d1>>RSHIFT32)&0xff;
				g = (d1>>GSHIFT32)&0xff;
				b = (d1>>BSHIFT32)&0xff;
			}
			
			if( dscan && y&1 ){
				r = ( ( r * brscan ) / 100 ) & 0xff;
				g = ( ( g * brscan ) / 100 ) & 0xff;
				b = ( ( b * brscan ) / 100 ) & 0xff;
			}
			*doff++ = (r<<RSHIFT32) | (g<<GSHIFT32) | (b<<BSHIFT32);
			if( xsc == 1 ) *doff++ = (r<<RSHIFT32) | (g<<GSHIFT32) | (b<<BSHIFT32);
		}
	}
	
	PRINTD( GRP_LOG, " -> %sScanLine -> OK\n", DISPSCAN ? "" : "No " );
	
	return true;
}


////////////////////////////////////////////////////////////////
// �T�u�o�b�t�@���t���b�V��
//
// ����:	�Ȃ�
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool DSP6::RefreshSubBuf( void )
{
	PRINTD( GRP_LOG, "[GRP][RefreshSubBuf]\n" );
	
	if( SBuf ){
		if( SBuf->Width() == ScreenX() && SBuf->Height() == ScreenY() ){
			// �T�C�Y���ς���Ă��Ȃ����OK
			PRINTD( GRP_LOG, " X:%d Y:%d -> OK\n", SBuf->Width(), SBuf->Height() );
			return true;
		}else{
			// �T�u�o�b�t�@��U�폜
			delete SBuf;
			SBuf = NULL;
		}
	}
	
	// �T�u�o�b�t�@�쐬
	PRINTD( GRP_LOG, " Create SubBuffer X:%d Y:%d\n", ScreenX(), ScreenY() );
	SBuf = new VSurface;
	if( !SBuf ) return false;
	
	if( !SBuf->InitSurface( ScreenX(), ScreenY() ) ){
		delete SBuf;
		SBuf = NULL;
		return false;
	}
	
	return true;
}
*/


////////////////////////////////////////////////////////////////
// �X�i�b�v�V���b�g
//
// ����:	path	�X�N���[���V���b�g�i�[�p�X
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void DSP6::SnapShot( const char *path )
{
	PRINTD( GRP_LOG, "[GRP][SnapShot]\n" );
	
	char img_file[PATH_MAX];
	char img_name[PATH_MAX] = "P6V";
	int Index = 0;
	
	// �X�i�b�v�V���b�g�i�[�t�H���_���Ȃ���΃G���[
	if( !OSD_FileExist( path ) ) return;
	
	// �X�i�b�v�V���b�g�t�@�C���������߂�
	do{
		sprintf( img_file, "%s%s%03d.%s", path, img_name, ++Index, IMG_EXT );
	}while( OSD_FileExist( img_file ) || (Index > 999) );
	
	// �A�Ԃ��L���Ȃ�摜�t�@�C���ۑ�
	if( !(Index > 999) ){
		VRect scr;
		
		scr.x = scr.y = 0;
		scr.w = DISPFULL ? P6WIFW : P6WINW;
		scr.h = DISPFULL ? P6WIFH : DISPNTSC ? HBBUS : P6WINH;
		
		BYTE *pixels = new BYTE[scr.w * scr.h * sizeof(DWORD)];
		if( !pixels ) return;
		
		if( !OSD_GetWindowImage( Wh, (void **)(&pixels), &scr ) ) return;
		SaveImgData( img_file, pixels, 32, scr.w, scr.h, NULL );
		
		if( pixels ) delete [] pixels;
	}
}
