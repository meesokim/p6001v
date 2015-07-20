#ifndef PIO_H_INCLUDED
#define PIO_H_INCLUDED

#include "typedef.h"
#include "device.h"
#include "device/pd8255.h"

////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class cPRT {
private:
	char FilePath[PATH_MAX];		// PRINTER�t�@�C���t���p�X
	
	FILE *fp;						// FILE �|�C���^
	BYTE pdata;						// �p�������|�[�g����󂯎�����f�[�^
	
	bool strb;						// �X�g���[�u
	
public:
	cPRT();							// �R���X�g���N�^
	~cPRT();						// �f�X�g���N�^
	
	void Init( const char * );		// ������
	void SetData( BYTE );			// �������f�[�^����t
	void Strobe( bool );			// �X�g���[�u��t
};




class PIO6 : public Device, public cD8255, public cPRT, public IDoko {
private:
	// ���o�͏����֐�
	void JobWriteA( BYTE );
	void JobWriteB( BYTE );
	void JobWriteC1( BYTE );
	void JobWriteD( BYTE );
	
	// �f�o�C�X��`
	static const Descriptor descriptor;
	static const InFuncPtr  indef[];
	static const OutFuncPtr outdef[];
	const Descriptor* GetDesc() const { return &descriptor; }
	
	// I/O�A�N�Z�X�֐�
	void Out90H( int, BYTE );
	void Out91H( int, BYTE );
	void Out92H( int, BYTE );
	void Out93H( int, BYTE );
	BYTE In90H( int );
	BYTE In92H( int );
	BYTE In93H( int );
	
	void OutPBH( int, BYTE );
	BYTE InPBH( int );
	BYTE InIBF( int );
	BYTE InOBF( int );
	
public:
	PIO6( VM6 *, const ID& );		// �R���X�g���N�^
	~PIO6();						// �f�X�g���N�^
	
	// �f�o�C�XID
	enum IDOut{ out90H=0, out91H, out92H, out93H, outPBH                 };
	enum IDIn {  in90H=0,          in92H,  in93H,  inPBH,  inIBF,  inOBF };
	
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};

#endif	// PIO_H_INCLUDED
