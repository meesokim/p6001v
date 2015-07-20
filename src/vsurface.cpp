#include <new>
#include <limits.h>

#include "common.h"
#include "vsurface.h"


DWORD VSurface::col32[] = {};	// 32bit�J���[�e�[�u��


////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
VSurface::VSurface( void ) : w(0), h(0), pitch(0), pixels(NULL),xscale(1) {}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
VSurface::~VSurface( void )
{
	if( pixels ) delete [] (BYTE *)pixels;
}



////////////////////////////////////////////////////////////////
// ������(�T�[�t�F�X�T�C�Y�w��)
//
// ����:	ww		��
//			hh		����
//			xsc		���{��(1:���{ 2:2�{)
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool VSurface::InitSurface( int ww, int hh, int xsc )
{
	// �쐬�ς� ���� �T�C�Y�����Ȃ炻�̂܂ܖ߂�
	if( pixels && w == ww && h == hh && xscale == xsc ) return true;
	
	// �T�C�Y�`�F�b�N
	if( ww <= 0 || ww*xsc > 0xffff || hh <= 0 || hh > 0xffff ) return false;
	
	// �쐬�ς݂Ȃ炢������J��
	if( pixels ) delete [] (BYTE *)pixels;
	
	// �������m��
	#if INBPP == 8	// 8bit
	pitch = (ww*xsc)&0xfffffffc;
	#else			// 32bit
	pitch = (ww*xsc*sizeof(DWORD))&0xfffffffc;
	#endif
	pixels = new BYTE[pitch*hh];
	if( !pixels ){
		w = h = pitch = rect.x = rect.y = rect.w = rect.h = 0;
		return false;
	}
	
	w = ww * xsc;
	h = hh;
	rect.x = rect.y = 0;
	rect.w = w;
	rect.h = h;
	xscale = xsc;
	
	return true;
}


////////////////////////////////////////////////////////////////
// �`��̈�ݒ�
// ����:	xx,yy	���W
//			ww,hh	��,����
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void VSurface::SetRect( int xx, int yy, int ww, int hh )
{
	VRect r1,r2;
	
	r1.x = 0;
	r1.y = 0;
	r1.w = w;
	r1.h = h;
	
	r2.x = xx;
	r2.y = xx;
	r2.w = ww > 0 ? ww : 0;
	r2.h = hh > 0 ? hh : 0;
	
	RectAdd( &rect, &r1, &r2 );
}


////////////////////////////////////////////////////////////////
// �`��̈�擾
// ����:	�Ȃ�
// �Ԓl:	VRect *	�`��̈�ւ̃|�C���^
////////////////////////////////////////////////////////////////
VRect *VSurface::GetRect( void )
{
	return &rect;
}


////////////////////////////////////////////////////////////////
// ���擾
// ����:	�Ȃ�
// �Ԓl:	int		��
////////////////////////////////////////////////////////////////
int VSurface::Width( void ) const
{
	return w;
}


////////////////////////////////////////////////////////////////
// �����擾
// ����:	�Ȃ�
// �Ԓl:	int		����
////////////////////////////////////////////////////////////////
int VSurface::Height( void ) const
{
	return h;
}


////////////////////////////////////////////////////////////////
// 1���C����byte���擾
// ����:	�Ȃ�
// �Ԓl:	int		1���C����byte��
////////////////////////////////////////////////////////////////
int VSurface::Pitch( void ) const
{
	return pitch;
}


////////////////////////////////////////////////////////////////
// ���{���擾
// ����:	�Ȃ�
// �Ԓl:	int		���{��
////////////////////////////////////////////////////////////////
int VSurface::XScale( void ) const
{
	return xscale;
}


////////////////////////////////////////////////////////////////
// �s�N�Z���f�[�^�ւ̃|�C���^�擾
// ����:	�Ȃ�
// �Ԓl:	BYTE *	�s�N�Z���f�[�^�ւ̃|�C���^
////////////////////////////////////////////////////////////////
void *VSurface::GetPixels( void ) const
{
	return pixels;
}


////////////////////////////////////////////////////////////////
// �_��`��
//
// ����:	x,y				���W
//			col				�J���[�R�[�h
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
void VSurface::PSet( int x, int y, BYTE col )
#else			// 32bit
void VSurface::PSet( int x, int y, DWORD col )
#endif
{
	if( pixels && x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h ){
		#if INBPP == 8	// 8bit
		BYTE *p  = (BYTE *)pixels  + y * pitch               + x;
		#else			// 32bit
		DWORD *p = (DWORD *)pixels + y * pitch/sizeof(DWORD) + x;
		#endif
		*p = col;
	}
}


////////////////////////////////////////////////////////////////
// �F���擾
//
// ����:	x,y				���W
// �Ԓl:	BYTE			�J���[�R�[�h
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
BYTE VSurface::PGet( int x, int y )
{
	BYTE res = 0;
#else			// 32bit
DWORD VSurface::PGet( int x, int y )
{
	DWORD res = 0;
#endif
	if( pixels && x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h ){
		#if INBPP == 8	// 8bit
		BYTE *p  = (BYTE *)pixels  + y * pitch               + x;
		#else			// 32bit
		DWORD *p = (DWORD *)pixels + y * pitch/sizeof(DWORD) + x;
		#endif
		res  = *p;
	}
	return res;
}


////////////////////////////////////////////////////////////////
// ��`�̈��h��Ԃ�
// ����:	col		�F
//			rc		�h��Ԃ��͈�(NULL�Ȃ����l)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
#if INBPP == 8	// 8bit
void VSurface::Fill( BYTE col, VRect *rc )
#else			// 32bit
void VSurface::Fill( DWORD col, VRect *rc )
#endif
{
	VRect rr;
	
	if( !pixels ) return;
	
	if( rc ){
		RectAdd( &rr, &rect, rc );
	}else{
		rr.x = rect.x;
		rr.y = rect.y;
		rr.w = rect.w;
		rr.h = rect.h;
	}
	
	if( rr.w && rr.h ){
		for( int i=0; i < rr.h; i++ ){
			#if INBPP == 8	// 8bit
			BYTE *p  = (BYTE *)pixels  + (rr.y + i) * pitch               + rr.x;
			#else			// 32bit
			DWORD *p = (DWORD *)pixels + (rr.y + i) * pitch/sizeof(DWORD) + rr.x;
			#endif
			for( int j=0; j < rr.w; j++ )
				*p++ = col;
		}
	}
}


////////////////////////////////////////////////////////////////
// �]������
//
// ����:	srect			�]�����͈�
//			dst				�]����T�[�t�F�X�ւ̃|�C���^
//			drect			�]����͈�(x,y�̂ݗL��)
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void VSurface::Blit( VRect *srect, VSurface *dst, VRect *drect )
{
	VRect src1,drc1,src2,drc2;
	
	if( !dst ) return;
	
	// �]�����͈͐ݒ�
	src1.x = srect ? srect->x : 0;
	src1.y = srect ? srect->y : 0;
	src1.w = srect ? srect->w : w;
	src1.h = srect ? srect->h : h;
	RectAdd( &src2, &src1, &rect );
	
	// �]����ݒ�
	drc1.x = drect ? drect->x : 0;
	drc1.y = drect ? drect->y : 0;
	drc1.w = src2.w;
	drc1.h = src2.h;
	RectAdd( &drc2, &drc1, dst->GetRect() );
	
	if( src2.w > drc2.w ) src2.w = drc2.w;
	if( src2.h > drc2.h ) src2.h = drc2.h;
	
	if( !src2.w || !src2.h || !drc2.w || !drc2.h ) return;
	
	#if INBPP == 8	// 8bit
	BYTE *psrc = (BYTE *)pixels           + src2.y * pitch        + src2.x;
	BYTE *pdst = (BYTE *)dst->GetPixels() + drc2.y * dst->Pitch() + drc2.x;
	
	for( int i=0; i < src2.h; i++ ){
		memcpy( pdst, psrc, src2.w );
		psrc += pitch;
		pdst += dst->Pitch();
	}
	#else			// 32bit
	DWORD *psrc = (DWORD *)pixels           + src2.y * pitch/sizeof(DWORD)        + src2.x;
	DWORD *pdst = (DWORD *)dst->GetPixels() + drc2.y * dst->Pitch()/sizeof(DWORD) + drc2.x;
	
	for( int i=0; i < src2.h; i++ ){
		memcpy( pdst, psrc, src2.w * sizeof(DWORD) );
		psrc += pitch/sizeof(DWORD);
		pdst += dst->Pitch()/sizeof(DWORD);
	}
	#endif
}


////////////////////////////////////////////////////////////////
// 32bit�J���[�e�[�u���ݒ�
//
// ����:	num				�C���f�b�N�X(0-255)
//			col				32bit�J���[�f�[�^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void VSurface::SetColor( int num, DWORD col )
{
	col32[num&0xff] = col;
}


////////////////////////////////////////////////////////////////
// 32bit�J���[�擾
//
// ����:	num				�C���f�b�N�X(0-255)
// �Ԓl:	DWORD			32bit�J���[�f�[�^
////////////////////////////////////////////////////////////////
DWORD VSurface::GetColor( int num )
{
	return col32[num&0xff];
}
