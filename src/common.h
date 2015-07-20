#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

#include "typedef.h"
#include "vsurface.h"


////////////////////////////////////////////////////////////////
// �ėp�v�Z�֐�
////////////////////////////////////////////////////////////////
DWORD CalcCrc32( BYTE *, int );						// CRC32�v�Z
WORD CalcCrc16( BYTE *, int );						// CRC16�v�Z


////////////////////////////////////////////////////////////////
// �����R�[�h����֐�
////////////////////////////////////////////////////////////////
void Euc2Jis( BYTE *, BYTE * );						// EUC  -> JIS
//void Jis2Euc( BYTE *, BYTE * );						// JIS  -> EUC
//void Euc2Sjis( BYTE *, BYTE * );					// EUC  -> SJIS
//void Sjis2Euc( BYTE *, BYTE * );					// SJIS -> EUC
void Sjis2Jis( BYTE *, BYTE * );					// SJIS -> JIS
void Jis2Sjis( BYTE *, BYTE * );					// JIS  -> SJIS
void Convert2Jis( BYTE *, BYTE * );					// �����R�[�h�`���ϊ�(SJIS,EUC -> JIS)

#ifdef __APPLE__
const char *Sjis2UTF8( const char * );				// SJIS -> UTF8
#endif

int Sjis2P6( char *, const char * );				// SJIS -> P6


////////////////////////////////////////////////////////////////
// �摜�t�@�C������֐�
////////////////////////////////////////////////////////////////
bool SaveImgData( const char *, BYTE *, const int, const int, const int, VRect * );	// Img SAVE from Data
bool SaveImg( const char *, VSurface *, VRect * );	// Img SAVE
VSurface *LoadImg( const char * );					// Img LOAD


////////////////////////////////////////////////////////////////
// �`��֌W
////////////////////////////////////////////////////////////////
void RectAdd( VRect *, VRect *, VRect * );			// ��`�̈捇��



#endif	// COMMON_H_INCLUDED
