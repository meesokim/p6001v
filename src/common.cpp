#include "log.h"
#include "common.h"
#include "png.h"

#ifdef __APPLE__
#include <Carbon/Carbon.h>  // add Windy for UTF8 �ϊ�
#endif



////////////////////////////////////////////////////////////////
// CRC32�v�Z
//
// ����:	buf				�f�[�^�o�b�t�@�ւ̃|�C���^
//			num				�f�[�^��(�o�C�g)
// �Ԓl:	DWORD			CRC32�l
////////////////////////////////////////////////////////////////
DWORD CalcCrc32( BYTE *buf, int num )
{
	DWORD crc = 0xffffffff;
	
	for( int i=0; i < num; i++ ){
		crc ^= buf[i];
		for( int j=0; j < 8; j++ )
			if( crc & 1 ) crc   = (crc >> 1) ^ 0xedb88320;
			else		  crc >>= 1;
	}
	return crc ^ 0xffffffff;
}


////////////////////////////////////////////////////////////////
// CRC16�v�Z
//
// ����:	buf				�f�[�^�o�b�t�@�ւ̃|�C���^
//			num				�f�[�^��(�o�C�g)
// �Ԓl:	WORD			CRC16�l
////////////////////////////////////////////////////////////////
WORD CalcCrc16( BYTE *buf, int num )
{
	WORD crc = 0xffff;
	
	for( int i=0; i < num; i++ ){
		crc ^= buf[i];
		for( int j=0; j < 8; j++ )
			if( crc & 1 ) crc   = (crc >> 1) ^ 0x8408;
			else		  crc >>= 1;
	}
	return crc ^ 0xffff;
}


////////////////////////////////////////////////////////////////
// EUC -> JIS
//
// ����:	c1				��1�o�C�g�ւ̃|�C���^
//			c2				��2�o�C�g�ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void Euc2Jis( BYTE *c1, BYTE *c2 )
{
	*c1 &= 0x7f;
	*c2 &= 0x7f;
}


////////////////////////////////////////////////////////////////
// JIS -> EUC
//
// ����:	c1				��1�o�C�g�ւ̃|�C���^
//			c2				��2�o�C�g�ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
/*
void Jis2Euc( BYTE *c1, BYTE *c2 )
{
	*c1 |= 0x80;
	*c2 |= 0x80;
}
*/


////////////////////////////////////////////////////////////////
// EUC -> SJIS
//
// ����:	c1				��1�o�C�g�ւ̃|�C���^
//			c2				��2�o�C�g�ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
/*
void Euc2Sjis( BYTE *c1, BYTE *c2 )
{
	// ��2�o�C�g�̕ϊ�
	if( ( *c1 % 2 ) == 0 )
		*c2 -= 0x02;
	else{
		*c2 -= 0x61;
		if( *c2 > 0x7e ) ++ *c2;
	}
	
	// ��1�o�C�g�̕ϊ�
	if( *c1 < 0xdf ){
		++ *c1;
		*c1 /= 2;
		*c1 += 0x30;
	}else{
		++ *c1;
		*c1 /= 2;
		*c1 += 0x70;
	}
}
*/


////////////////////////////////////////////////////////////////
// SJIS -> EUC
//
// ����:	c1				��1�o�C�g�ւ̃|�C���^
//			c2				��2�o�C�g�ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
/*
void Sjis2Euc( BYTE *c1, BYTE *c2 )
{
	if( *c2 < 0x9f ){
		if( *c1 < 0xa0 ){
			*c1 -= 0x81;
			*c1 *= 2;
			*c1 += 0xa1;
		}else{
			*c1 -= 0xe0;
			*c1 *= 2;
			*c1 += 0xdf;
		}
		if( *c2 > 0x7f ) -- *c2;
		*c2 += 0x61;
	}else{
		if( *c1 < 0xa0 ){
			*c1 -= 0x81;
			*c1 *= 2;
			*c1 += 0xa2;
		}else{
			*c1 -= 0xe0;
			*c1 *= 2;
			*c1 += 0xe0;
		}
		*c2 += 2;
	}
}
*/


////////////////////////////////////////////////////////////////
// SJIS -> JIS
//
// ����:	c1				��1�o�C�g�ւ̃|�C���^
//			c2				��2�o�C�g�ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void Sjis2Jis( BYTE *c1, BYTE *c2 )
{
	if( *c2 < 0x9f ){
		if( *c1 < 0xa0 ){
			*c1 -= 0x81;
			*c1 *= 2;
			*c1 += 0x21;
		}else{
			*c1 -= 0xe0;
			*c1 *= 2;
			*c1 += 0x5f;
		}
		if( *c2 > 0x7f ) --*c2;
		*c2 -= 0x1f;
	}else{
		if( *c1 < 0xa0 ){
			*c1 -= 0x81;
			*c1 *= 2;
			*c1 += 0x22;
		}else{
			*c1 -= 0xe0;
			*c1 *= 2;
			*c1 += 0x60;
		}
		*c2 -= 0x7e;
	}
}


////////////////////////////////////////////////////////////////
// JIS -> SJIS
//
// ����:	c1				��1�o�C�g�ւ̃|�C���^
//			c2				��2�o�C�g�ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void Jis2Sjis( BYTE *c1, BYTE *c2 )
{
	// ��2�o�C�g�̕ϊ�
	if( ( *c1 & 1 ) == 0 ) *c2 += 0x7d;
	else                   *c2 += 0x1f;
	if( *c2 > 0x7e ) ++ *c2;
	
	// ��1�o�C�g�̕ϊ�
	if( *c1 < 0x5f ){
		++ *c1;
		*c1 /= 2;
		*c1 += 0x70;
	}else{
		++ *c1;
		*c1 /= 2;
		*c1 += 0xb0;
	}
}


////////////////////////////////////////////////////////////////
// �����R�[�h�`���ϊ�(SJIS,EUC -> JIS)
//
// ����:	c1				��1�o�C�g�ւ̃|�C���^
//			c2				��2�o�C�g�ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void Convert2Jis( BYTE *c1, BYTE *c2 )
{
	// ShiftJIS?
	if( (((*c1 >= 0x81) && (*c1 <= 0x9f)) || ((*c1 >= 0xe0) && (*c1 <= 0xfc))) && ((*c2 >= 0x40) && (*c2 <= 0xfc) && (*c2 != 0x7f)) )
		Sjis2Jis( c1, c2 );
	// EUC?
	else if( ((*c1 >= 0xa1) && (*c1 <= 0xfe)) && ((*c2 >= 0xa1) && (*c1 <= 0xfe)) )
		Euc2Jis( c1, c2 );
}


////////////////////////////////////////////////////////////////
// SJIS -> UTF8
//
// ����:	str				SJIS������ւ̃|�C���^
// �Ԓl:	char *			UTF8������ւ̃|�C���^
////////////////////////////////////////////////////////////////
#ifdef __APPLE__
const char *Sjis2UTF8( const char *str )
{
	static char dst[PATH_MAX];
	CFStringRef cfstr;
	
	cfstr = CFStringCreateWithCString( NULL, str, kCFStringEncodingShiftJIS_X0213_00 );
	CFStringGetCString( cfstr, dst, PATH_MAX, kCFStringEncodingUTF8 );
	CFRelease( cfstr );
	return dst;
}
#endif


// isio����� txt2bas ���痬�p
/****************************************************/
/* txt2bas                                          */
/* sjis.h                                           */
/* 2000.03.26. by ishioka                           */
/* 2000.05.25., 2001.01.06.                         */
/****************************************************/

// sjis <-> p6 character set table
int sjistbl[] = {
	0x0000, 0x8c8e, 0x89ce, 0x9085, 0x96d8, 0x8be0, 0x9379, 0x93fa, 
	0x944e, 0x897e, 0x8e9e, 0x95aa, 0x9562, 0x9553, 0x90e7, 0x969c, 
	0x83ce, 0x84b3, 0x84b1, 0x84b2, 0x84b0, 0x84b4, 0x84ab, 0x84aa, 
	0x84ac, 0x84ad, 0x84af, 0x84ae, 0x817e, 0x91e5, 0x9286, 0x8fac, 
	0x8140, 0x8149, 0x8168, 0x8194, 0x8190, 0x8193, 0x8195, 0x8166, 
	0x8169, 0x816a, 0x8196, 0x817b, 0x8143, 0x817c, 0x8144, 0x815e, 
	0x824f, 0x8250, 0x8251, 0x8252, 0x8253, 0x8254, 0x8255, 0x8256, 
	0x8257, 0x8258, 0x8146, 0x8147, 0x8183, 0x8181, 0x8184, 0x8148, 
	0x8197, 0x8260, 0x8261, 0x8262, 0x8263, 0x8264, 0x8265, 0x8266, 
	0x8267, 0x8268, 0x8269, 0x826a, 0x826b, 0x826c, 0x826d, 0x826e, 
	0x826f, 0x8270, 0x8271, 0x8272, 0x8273, 0x8274, 0x8275, 0x8276, 
	0x8277, 0x8278, 0x8279, 0x816b, 0x818f, 0x816c, 0x814f, 0x8151, 
	0x0000, 0x8281, 0x8282, 0x8283, 0x8284, 0x8285, 0x8286, 0x8287, 
	0x8288, 0x8289, 0x828a, 0x828b, 0x828c, 0x828d, 0x828e, 0x828f, 
	0x8290, 0x8291, 0x8292, 0x8293, 0x8294, 0x8295, 0x8296, 0x8297, 
	0x8298, 0x8299, 0x829a, 0x816f, 0x8162, 0x8170, 0x8160, 0x8140, 
	0x81a3, 0x81a5, 0x819a, 0x819f, 0x819b, 0x819c, 0x82f0, 0x829f, 
	0x82a1, 0x82a3, 0x82a5, 0x82a7, 0x82e1, 0x82e3, 0x82e5, 0x82c1, 
	0x0000, 0x82a0, 0x82a2, 0x82a4, 0x82a6, 0x82a8, 0x82a9, 0x82ab, 
	0x82ad, 0x82af, 0x82b1, 0x82b3, 0x82b5, 0x82b7, 0x82b9, 0x82bb, 
	0x0000, 0x8142, 0x8175, 0x8176, 0x8141, 0x8145, 0x8392, 0x8340, 
	0x8342, 0x8344, 0x8346, 0x8348, 0x8383, 0x8385, 0x8387, 0x8362, 
	0x815b, 0x8341, 0x8343, 0x8345, 0x8347, 0x8349, 0x834a, 0x834c, 
	0x834e, 0x8350, 0x8352, 0x8354, 0x8356, 0x8358, 0x835a, 0x835c, 
	0x835e, 0x8360, 0x8363, 0x8365, 0x8367, 0x8369, 0x836a, 0x836b, 
	0x836c, 0x836d, 0x836e, 0x8371, 0x8374, 0x8377, 0x837a, 0x837d, 
	0x837e, 0x8380, 0x8381, 0x8382, 0x8384, 0x8386, 0x8388, 0x8389, 
	0x838a, 0x838b, 0x838c, 0x838d, 0x838f, 0x8393, 0x814a, 0x814b, 
	0x82bd, 0x82bf, 0x82c2, 0x82c4, 0x82c6, 0x82c8, 0x82c9, 0x82ca, 
	0x82cb, 0x82cc, 0x82cd, 0x82d0, 0x82d3, 0x82d6, 0x82d9, 0x82dc, 
	0x82dd, 0x82de, 0x82df, 0x82e0, 0x82e2, 0x82e4, 0x82e6, 0x82e7, 
	0x82e8, 0x82e9, 0x82ea, 0x82eb, 0x82ed, 0x82f1, 0x0000, 0x0000, 
};

// add 2000.05.25.
int sjistbl2[] = {
	// sjis dakuon list
	0x82aa, 0x82ac, 0x82ae, 0x82b0, 0x82b2, 
	0x82b4, 0x82b6, 0x82b8, 0x82ba, 0x82bc, 
	0x82be, 0x82c0, 0x82c3, 0x82c5, 0x82c7, 
	0x82ce, 0x82d1, 0x82d4, 0x82d7, 0x82da, 
	0x834b, 0x834d, 0x834f, 0x8351, 0x8353, 
	0x8355, 0x8357, 0x8359, 0x835b, 0x835d, 
	0x835f, 0x8361, 0x8364, 0x8366, 0x8368, 
	0x836f, 0x8372, 0x8375, 0x8378, 0x837b, 
	// sjis han-dakuon list
	0x82cf, 0x82d2, 0x82d5, 0x82d8, 0x82db, 
	0x8370, 0x8373, 0x8376, 0x8379, 0x837c, 
	// for "vu" (2001.01.06)
	0x8394,
};


////////////////////////////////////////////////////////////////
// SJIS -> P6
//
// ����:	dstr			P6������i�[�|�C���^
//			sstr			SJIS������ւ̃|�C���^
// �Ԓl:	int				P6������̒���
////////////////////////////////////////////////////////////////
int Sjis2P6( char *dstr, const char *sstr )
{
	const char *ss = sstr;
	char *ds       = dstr;
	int c;
	
	while( (c = (BYTE)(*ss++)) != '\0' ){
		if( (c >= 0x80) && (c <= 0x9f) ){
			c = c * 256 + (BYTE)(*ss++);
			
			// for dakuon/han-dakuon 2000.05.25.
			int opt = 0;
			for( int j=0; j<COUNTOF(sjistbl2); j++ ){
				if( sjistbl2[j] == c ){
					if( j == 50 ){
						// for "vu" (2001.01.06)
						opt = 1;
						c = 0x8345;
					}else{
						opt = (j < 40) ? 1 : 2;
						c = c - opt;
						break;
					}
				}
			}
			
			for( int j=0; j<256; j++ ){
				if( sjistbl[j] == c ){
					if( j <= 0x1f ){
						// if graphic character
						*ds++ = (char)0x14;
						*ds++ = (char)(j + 0x30);
					}else{
						*ds++ = (char)j;
						if( opt != 0 )
							*ds++ = (char)(0xde + opt - 1);
					}
					break;
				}
			}
		}else if( c <= 0x1f ){
			switch( c ){
			case 0x09:
				*ds++ = 0x09;
				break;
			case 0x0a:
				*ds++ = 0x0d;
				break;
			case 0x0d:
				if( *ss == 0x0a ) ss++;
				*ds++ = 0x0d;
				break;
			}
		}else
			*ds++ = (char)c;
	}
	*ds++ = '\0';
	
	return ds-dstr;
}




////////////////////////////////////////////////////////////////
// Img SAVE from Data
//   ���Q�ƁuPNG���p�p�v http://gmoon.jp/png/
//			 �u�v���O���~���O�E���C�u�����v http://dencha.ojaru.jp/
//
// ����:	filename		�ۑ��t�@�C����
//			pixels			�ۑ�����f�[�^�̈�ւ̃|�C���^
//			bpp				�F�[�x
//			ww				��
//			hh				����
//			pos				�ۑ�����̈���ւ̃|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool SaveImgData( const char *filename, BYTE *pixels, const int bpp, const int ww, const int hh, VRect *pos )
{
	PRINTD( GRP_LOG, "[COMMON][SaveImg] -> %s\n", filename );
	
	FILE *fp;
	png_structp PngPtr;
	png_infop InfoPtr;
	png_bytepp image = NULL;	// image[HEIGHT][WIDTH]�̌`��
	VRect rec;
	int pitch = ww * bpp / 8;
	
	// �̈�ݒ�
	if( pos ){
		rec.x = pos->x;	rec.y = pos->y;
		rec.w = pos->w;	rec.h = pos->h;
	}else{
		rec.x =     rec.y = 0;
		rec.w = ww; rec.h = hh;
	}
	
	// �e�\���̂��m�ہE����������
	if( !(PngPtr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) )
		return false;
	
	if( !(InfoPtr = png_create_info_struct( PngPtr )) ){
		png_destroy_write_struct( &PngPtr, NULL );
		return false;
	}
	
	// �摜�t�@�C�����J��
	if( !(fp = FOPENEN( filename, "wb" )) ){
		png_destroy_write_struct( &PngPtr, &InfoPtr );
		return false;
	}
	
	// �G���[����
	if( setjmp( png_jmpbuf( PngPtr ) ) ){
		png_destroy_write_struct( &PngPtr, &InfoPtr );
		fclose( fp );
		if( image ){
			for( int i=0; i<rec.h; i++ ) delete [] image[i];
			delete [] image;
		}
		return false;
	}
	
	// libpng��fp��m�点��
	png_init_io( PngPtr, fp );
	
	
	// ���k���̐ݒ�
	png_set_compression_level( PngPtr, 9 );
	
	// IHDR�`�����N����ݒ肷��
	png_set_IHDR( PngPtr, InfoPtr, rec.w, rec.h, bpp == 1 ? 1 : 8,
					bpp == 1 ? PNG_COLOR_TYPE_GRAY :
					bpp == 8 ? PNG_COLOR_TYPE_PALETTE :
							   PNG_COLOR_TYPE_RGB_ALPHA,
					PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT,
					PNG_FILTER_TYPE_DEFAULT );
	
	// �C���[�W�f�[�^��2�����z��ɔz�u����
	image = new png_bytep[rec.h];
	
	BYTE *doff = pixels + rec.x + rec.y * pitch;
	for( int i=0; i<rec.h; i++ ){
		image[i] = new png_byte[rec.w * bpp / 8];
		memcpy( image[i], doff, rec.w * bpp / 8 );
		doff += pitch;
	}
	
	if( bpp == 8 ){	// 8bit�̏ꍇ
		// �p���b�g�����
		png_colorp Palette = ( png_colorp )png_malloc( PngPtr, 256 * sizeof(png_color) );
		// �T�[�t�F�X����p���b�g���擾
		for( int i=0; i<256; i++ ){
			Palette[i].red   = (VSurface::GetColor( i )>>RSHIFT32)&0xff;
			Palette[i].green = (VSurface::GetColor( i )>>GSHIFT32)&0xff;
			Palette[i].blue  = (VSurface::GetColor( i )>>BSHIFT32)&0xff;
		}
		png_set_PLTE( PngPtr, InfoPtr, Palette, 256 );
	}
	
	// �C���[�W�f�[�^��libpng�ɒm�点��
	png_set_rows( PngPtr, InfoPtr, image );
	
	// �摜�f�[�^��������
	if     ( bpp == 1 ) png_write_png( PngPtr, InfoPtr, PNG_TRANSFORM_IDENTITY, NULL );
	else if( bpp == 8 ) png_write_png( PngPtr, InfoPtr, PNG_TRANSFORM_IDENTITY, NULL );
	else				png_write_png( PngPtr, InfoPtr, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_BGR | PNG_TRANSFORM_INVERT_ALPHA, NULL );
	
	
	// �摜�t�@�C�������
	fclose( fp );
	
	// �C���[�W�f�[�^���J������
	for( int i=0; i<rec.h; i++ ) delete [] image[i];
	delete [] image;
	
	// 2�̍\���̂̃��������������
	png_destroy_write_struct( &PngPtr, &InfoPtr );
	
	return true;
}


////////////////////////////////////////////////////////////////
// Img SAVE
//   ���Q�ƁuPNG���p�p�v http://gmoon.jp/png/
//			 �u�v���O���~���O�E���C�u�����v http://dencha.ojaru.jp/
//
// ����:	filename		�ۑ��t�@�C����
//			sur				�ۑ�����T�[�t�F�X�ւ̃|�C���^
//			pos				�ۑ�����̈���ւ̃|�C���^
// �Ԓl:	bool	true:���� false:���s
////////////////////////////////////////////////////////////////
bool SaveImg( const char *filename, VSurface *sur, VRect *pos )
{
	return SaveImgData( filename, (BYTE *)sur->GetPixels(), INBPP, sur->Width(), sur->Height(), pos );
}

/*
bool SaveImg( const char *filename, VSurface *sur, VRect *pos )
{
	PRINTD( GRP_LOG, "[COMMON][SaveImg] -> %s\n", filename );
	
	FILE *fp;
	png_structp PngPtr;
	png_infop InfoPtr;
	png_bytepp image = NULL;	// image[HEIGHT][WIDTH]�̌`��
	VRect rec;
	
	// �̈�ݒ�
	if( pos ){
		rec.x = pos->x;	rec.y = pos->y;
		rec.w = pos->w;	rec.h = pos->h;
	}else{
		rec.x =               rec.y = 0;
		rec.w = sur->Width(); rec.h = sur->Height();
	}
	
	// �e�\���̂��m�ہE����������
	if( !(PngPtr = png_create_write_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) )
		return false;
	
	if( !(InfoPtr = png_create_info_struct( PngPtr )) ){
		png_destroy_write_struct( &PngPtr, NULL );
		return false;
	}
	
	// �摜�t�@�C�����J��
	if( !(fp = FOPENEN( filename, "wb" )) ){
		png_destroy_write_struct( &PngPtr, &InfoPtr );
		return false;
	}
	
	// �G���[����
	if( setjmp( png_jmpbuf( PngPtr ) ) ){
		png_destroy_write_struct( &PngPtr, &InfoPtr );
		fclose( fp );
		if( image ){
			for( int i=0; i<rec.h; i++ ) delete [] image[i];
			delete [] image;
		}
		return false;
	}
	
	// libpng��fp��m�点��
	png_init_io( PngPtr, fp );
	
	
	// ���k���̐ݒ�
	png_set_compression_level( PngPtr, 9 );
	
	// IHDR�`�����N����ݒ肷��
	png_set_IHDR( PngPtr, InfoPtr, rec.w, rec.h, 8,
					#if INBPP == 8	// 8bit
					PNG_COLOR_TYPE_PALETTE,
					#else			// 32bit
					PNG_COLOR_TYPE_RGB_ALPHA,
					#endif
					PNG_INTERLACE_NONE,
					PNG_COMPRESSION_TYPE_DEFAULT,
					PNG_FILTER_TYPE_DEFAULT );
	
	// �C���[�W�f�[�^��2�����z��ɔz�u����
	image = new png_bytep[rec.h];
	
	BYTE *doff = (BYTE *)sur->GetPixels() + rec.x + rec.y * sur->Pitch();
	for( int i=0; i<rec.h; i++ ){
		#if INBPP == 8	// 8bit
		image[i] = new png_byte[rec.w];
		memcpy( image[i], doff, rec.w );
		#else			// 32bit
		image[i] = new png_byte[rec.w * sizeof(DWORD)];
		memcpy( image[i], doff, rec.w * sizeof(DWORD) );
		#endif
		doff += sur->Pitch();
	}
	
	#if INBPP == 8	// 8bit
	// �p���b�g�����
	png_colorp Palette = ( png_colorp )png_malloc( PngPtr, 256 * sizeof(png_color) );
	// �T�[�t�F�X����p���b�g���擾
	for( int i=0; i<256; i++ ){
		Palette[i].red   = (VSurface::GetColor( i )>>RSHIFT32)&0xff;
		Palette[i].green = (VSurface::GetColor( i )>>GSHIFT32)&0xff;
		Palette[i].blue  = (VSurface::GetColor( i )>>BSHIFT32)&0xff;
	}
	png_set_PLTE( PngPtr, InfoPtr, Palette, 256 );
	#endif
	
	// �C���[�W�f�[�^��libpng�ɒm�点��
	png_set_rows( PngPtr, InfoPtr, image );
	
	// �摜�f�[�^��������
	#if INBPP == 8	// 8bit
	png_write_png( PngPtr, InfoPtr, PNG_TRANSFORM_IDENTITY, NULL );
	#else			// 32bit
	png_write_png( PngPtr, InfoPtr, PNG_TRANSFORM_PACKING | PNG_TRANSFORM_BGR | PNG_TRANSFORM_INVERT_ALPHA, NULL );
	#endif
	
	// �摜�t�@�C�������
	fclose( fp );
	
	// �C���[�W�f�[�^���J������
	for( int i=0; i<rec.h; i++ ) delete [] image[i];
	delete [] image;
	
	// 2�̍\���̂̃��������������
	png_destroy_write_struct( &PngPtr, &InfoPtr );
	
	return true;
}
*/


////////////////////////////////////////////////////////////////
// Img LOAD
//   ���Q�ƁuPNG���p�p�v http://gmoon.jp/png/
//			 �u�v���O���~���O�E���C�u�����v http://dencha.ojaru.jp/
//
// ����:	filename		�Ǎ��ރt�@�C����
// �Ԓl:	VSurface *		�Ǎ��܂ꂽ�T�[�t�F�X�ւ̃|�C���^
//							(�ⓚ���p��32bit��)
////////////////////////////////////////////////////////////////
VSurface *LoadImg( const char *filename )
{
	PRINTD( GRP_LOG, "[COMMON][LoadImg] <- %s\n", filename );
	
	FILE *fp;
	png_structp PngPtr;
	png_infop InfoPtr, EndInfo;
	png_uint_32 width, height;
	png_byte BitDepth, ColorType, Channels;
	png_bytep *RowPtrs;
	
	VSurface *sur = NULL;
	
	
	// �e�\���̂��m�ہE����������
	if( !(PngPtr = png_create_read_struct( PNG_LIBPNG_VER_STRING, NULL, NULL, NULL)) )
		return NULL;
	
	if( !(InfoPtr = png_create_info_struct( PngPtr )) ){
		png_destroy_read_struct( &PngPtr, NULL, NULL );
		return NULL;
	}
	
	if( !(EndInfo = png_create_info_struct( PngPtr )) ){
		png_destroy_read_struct( &PngPtr, &InfoPtr, NULL );
		return NULL;
	}
	
	// �摜�t�@�C�����J��
	if( !(fp = FOPENEN( filename, "rb" )) ){
		png_destroy_read_struct( &PngPtr, &InfoPtr, &EndInfo );
		return NULL;
	}
	
	// �G���[����
	if( setjmp( png_jmpbuf( PngPtr ) ) ){
		png_destroy_read_struct( &PngPtr, &InfoPtr, &EndInfo );
		fclose( fp );
		if( sur ) delete sur;
		return NULL;
	}
	
	// libpng��fp��m�点��
	png_init_io( PngPtr, fp );
	
	// �摜�f�[�^��Ǎ���
//	#if INBPP == 8	// 8bit
	png_read_png( PngPtr, InfoPtr, PNG_TRANSFORM_IDENTITY, NULL );
//	#else			// 32bit
//	png_read_png( PngPtr, InfoPtr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL );
//	#endif
	
	
	// �e���񒊏o
	RowPtrs   = png_get_rows( PngPtr, InfoPtr );
	width     = png_get_image_width( PngPtr, InfoPtr );
	height    = png_get_image_height( PngPtr, InfoPtr );
	BitDepth  = png_get_bit_depth( PngPtr, InfoPtr );
	ColorType = png_get_color_type( PngPtr, InfoPtr );
	Channels  = png_get_channels( PngPtr, InfoPtr );
	
	
	// �T�[�t�F�X���쐬
	sur = new VSurface;
	sur->InitSurface( width, height );
	
	// �摜�f�[�^���擾
	#if INBPP == 8	// 8bit
	BYTE *doff  = (BYTE *)sur->GetPixels();
	#else			// 32bit
	DWORD *doff = (DWORD *)sur->GetPixels();
	#endif
	
	if( Channels == 1 && ( ColorType == PNG_COLOR_TYPE_GRAY || ColorType == PNG_COLOR_TYPE_PALETTE ) &&
		( BitDepth == 1 || BitDepth == 8 ) ){
		// GRAY, PALETTE
		int num_palette;
		png_colorp palette;
		
		// �p���b�g������ΓǍ���
		if( ColorType == PNG_COLOR_TYPE_PALETTE )
			png_get_PLTE( PngPtr, InfoPtr, &palette, &num_palette );
		
		for( int i=0; i<(int)height; i++){
			png_bytep rpt = RowPtrs[i];
			#if INBPP == 8	// 8bit
			for( int j=0; j<(int)width; j++ ){
				doff[j] = BitDepth == 1 ? (rpt[j/8]&(0x80>>(j&7))) ? 15 : 0
										: rpt[j];
			}
			doff += sur->Pitch();
			#else			// 32bit
			for( int j=0; j<(int)width; j++ )
				doff[j] = BitDepth == 1 ? (rpt[j/8]&(0x80>>(j&7))) ? RMASK32|GMASK32|BMASK32 : 0
										: (palette[rpt[j]].red<<RSHIFT32)|(palette[rpt[j]].green<<GSHIFT32)|(palette[rpt[j]].blue<<BSHIFT32);
			doff += sur->Pitch()/sizeof(DWORD);
			#endif
		}
	}else if( Channels != 1 && ( ColorType == PNG_COLOR_TYPE_RGB_ALPHA || ColorType == PNG_COLOR_TYPE_RGB ) &&
				BitDepth == 8 ){
		// GRAY_ALPHA, RGB, RGB_ALPHA, RGB+x
		for( int i=0; i<(int)height; i++ ){
//			png_bytep rpt = RowPtrs[i];
			#if INBPP == 8	// 8bit
// ���̂ւ�K���B���Ƃōl����B
			doff += sur->Pitch();
			#else			// 32bit
			png_bytep rpt = RowPtrs[i];
			for( int j=0; j<(int)width; j++ ){
				doff[j]  = *(rpt++)<<RSHIFT32;
				doff[j] |= *(rpt++)<<GSHIFT32;
				doff[j] |= *(rpt++)<<BSHIFT32;
			}
			doff += sur->Pitch()/sizeof(DWORD);
			#endif
		}
	}else{
		delete sur;
		sur = NULL;
	}
	
	// �e�\���̂��J������
	png_destroy_read_struct( &PngPtr, &InfoPtr, &EndInfo );
	
	// �摜�t�@�C�������
	fclose( fp );
	
	return sur;
}


////////////////////////////////////////////////////////////////
// ��`�̈捇��
//
// ����:	rr		�������ʂ��o�͂����`�̈�ւ̃|�C���^
//			r1,r2	���������`�̈�ւ̃|�C���^
// �Ԓl:	�Ȃ�
////////////////////////////////////////////////////////////////
void RectAdd( VRect *rr, VRect *r1, VRect *r2 )
{
	if( !rr || !r1 || !r2 ) return;
	
	int x1 = max( r1->x, r2->x );
	int y1 = max( r1->y, r2->y );
	int x2 = min( r1->x + r1->w - 1, r2->x + r2->w - 1);
	int y2 = min( r1->y + r1->h - 1, r2->y + r2->h - 1);
	
	rr->w = x1 > x2 ? 0 : x2 - x1 + 1;
	rr->h = y1 > y2 ? 0 : y2 - y1 + 1;
	
	rr->x = rr->w ? x1 : 0;
	rr->y = rr->h ? y1 : 0;
}


