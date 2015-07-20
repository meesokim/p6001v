#ifndef TYPEDEF_H_INCLUDED
#define TYPEDEF_H_INCLUDED

#define __STDC_LIMIT_MACROS

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>


/////////////////////////////////////////////////////////////////////////////
// �^,�萔��`
/////////////////////////////////////////////////////////////////////////////
#if defined(WIN32) || defined(WIN64)
#define	NOMINMAX	1				// max,min������
#include <windows.h>
#else
typedef uint8_t		BYTE;
typedef uint16_t	WORD;
typedef uint32_t	DWORD;
#endif


typedef uint8_t		UINT8;
typedef uint16_t	UINT16;
typedef uint32_t	UINT32;
typedef int8_t		SINT8;
typedef int16_t		SINT16;
typedef int32_t		SINT32;


// OSD�֘A�I�u�W�F�N�g�ւ̃|�C���^
// �ʓ|�Ȃ̂łƂ肠�������ł�void *
typedef void *HTHREAD;				// �X���b�h
typedef void *HCRSECT;				// �N���e�B�J���Z�N�V����
typedef void *HSEMAPHORE;			// �Z�}�t�H
typedef void *HTIMERID;				// �^�C�}ID
typedef void *HWINDOW;				// �E�B���h�E�n���h���I��
typedef void *HSURFACE;				// �T�[�t�F�X�I��
typedef void *HJOYINFO;				// �W���C�X�e�B�b�N

// OSD�֘A�R�[���o�b�N�֐��ւ̃|�C���^
typedef void (*CBF_SND)( void *, BYTE *, int );	// �T�E���h�X�g���[��
typedef DWORD (*CBF_TMR)( DWORD, void * );		// �^�C�}


/////////////////////////////////////////////////////////////////////////////
// �萔�Ȃ�
/////////////////////////////////////////////////////////////////////////////
#define INBPP		8	// �����F�[�x

#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#ifndef PATH_MAX
#define	PATH_MAX	260
#endif

#define LIL_ENDIAN	1234
#define BIG_ENDIAN	4321

#ifndef BYTEORDER
#if defined(__hppa__) || \
    defined(__m68k__) || defined(mc68000) || defined(_M_M68K) || \
    (defined(__MIPS__) && defined(__MISPEB__)) || \
    defined(__ppc__) || defined(__POWERPC__) || defined(_M_PPC) || \
    defined(__sparc__)
#define BYTEORDER	BIG_ENDIAN
#else
#define BYTEORDER	LIL_ENDIAN
#endif
#endif



/////////////////////////////////////////////////////////////////////////////
// �ėp�}�N��
/////////////////////////////////////////////////////////////////////////////
#define STATIC_CAST(t, o)	static_cast<t> (o)
#define CONST_CAST(t, o)	const_cast<t> (o)

#define FGETBYTE(fp)		((BYTE)fgetc(fp))
#define FGETWORD(fp)		((WORD)(((BYTE)fgetc(fp))|((BYTE)fgetc(fp)<<8)))
#define FGETDWORD(fp)		((DWORD)(((BYTE)fgetc(fp))|((BYTE)fgetc(fp)<<8)|((BYTE)fgetc(fp)<<16)|((BYTE)fgetc(fp)<<24)))
#define FPUTBYTE(data,fp)	fputc((data)&0xff,fp)
#define FPUTWORD(data,fp)	{ fputc((data)&0xff,fp); fputc(((data)>>8)&0xff,fp); }
#define FPUTDWORD(data,fp)	{ fputc((data)&0xff,fp); fputc(((data)>>8)&0xff,fp); fputc(((data)>>16)&0xff,fp); fputc(((data)>>24)&0xff,fp); }

#define CTODW(a,b,c,d)		((DWORD)(((BYTE)(a))|(((DWORD)((BYTE)(b)))<<8)|(((DWORD)((BYTE)(c)))<<16)|(((DWORD)((BYTE)(d)))<<24)))

#if BYTEORDER == LIL_ENDIAN
#define GET3BYTE(a,b,c,p)	{ a = *((BYTE *)p++); b = *((BYTE *)p++); c = *((BYTE *)p++); }
#define PUT3BYTE(a,b,c,p)	{ *((BYTE *)p++) = a; *((BYTE *)p++) = b; *((BYTE *)p++) = c; }
#define BTODW(a,b,c,d)		((DWORD)(((BYTE)(a))|(((DWORD)((BYTE)(b)))<<8)|(((DWORD)((BYTE)(c)))<<16)|(((DWORD)((BYTE)(d)))<<24)))
#define DWTOB(s,a,b,c,d)	{ a = (BYTE)((s>>24)&0x000000ff); b = (BYTE)((s>>16)&0x000000ff); c = (BYTE)((s>>8)&0x000000ff); d = (BYTE)(s&0x000000ff); }
#else
#define GET3BYTE(a,b,c,p)	{ c = *((BYTE *)p++); b = *((BYTE *)p++); a = *((BYTE *)p++); }
#define PUT3BYTE(a,b,c,p)	{ *((BYTE *)p++) = c; *((BYTE *)p++) = b; *((BYTE *)p++) = a; }
#define BTODW(a,b,c,d)		((DWORD)(((BYTE)(d))|(((DWORD)((BYTE)(c)))<<8)|(((DWORD)((BYTE)(b)))<<16)|(((DWORD)((BYTE)(a)))<<24)))
#define DWTOB(s,a,b,c,d)	{ d = (BYTE)((s>>24)&0x000000ff); c = (BYTE)((s>>16)&0x000000ff); b = (BYTE)((s>>8)&0x000000ff); a = (BYTE)(s&0x000000ff); }
#endif

#define	COUNTOF(arr)		(int)(sizeof(arr)/sizeof((arr)[0]))

#define INITARRAY(arr,val)	{for(int i=0; i<COUNTOF(arr); i++) arr[i] = val;}

#ifndef ZeroMemory
#define ZeroMemory(d,l)	memset((d), 0, (l))
#endif


// 32bit������NOMINMAX�������Ȃ��݂���?
#undef max
#undef min

template <class T, class S>
T max( T v1, S v2 ){ T v2_(v2); return v1 > v2_ ? v1 : v2_; }

template <class T, class S>
T min( T v1, S v2 ){ T v2_(v2); return v1 < v2_ ? v1 : v2_; }

// Mac�p�����R�[�h�ϊ� ShiftJIS -> UTF-8
#ifdef __APPLE__
#define FOPENEN(a,b)	fopen(Sjis2UTF8(a),b)
#else
#define FOPENEN(a,b)	fopen(a,b)
#endif




#endif	// TYPEDEF_H_INCLUDED
