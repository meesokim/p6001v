#ifndef GRAPH_H_INCLUDED
#define GRAPH_H_INCLUDED

#include "typedef.h"
#include "p6vm.h"
#include "vsurface.h"

////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class DSP6 {
protected:
	VM6* vm;
	
	HWINDOW Wh;				// �E�B���h�E�n���h��
//	VSurface *SBuf;			// �T�u�o�b�t�@�|�C���^
	
	bool SetScreenSurface();				// �X�N���[���T�[�t�F�X�쐬
//	bool UpdateSubBuf();					// �T�u�o�b�t�@�X�V
//	bool RefreshSubBuf();					// �T�u�o�b�t�@���t���b�V��
	
public:
	DSP6( VM6 * );							// �R���X�g���N�^
	~DSP6();								// �f�X�g���N�^
	
	bool Init();							// ������
	void SetIcon( const int );				// �A�C�R���ݒ�
	
	bool ResizeScreen();					// �X�N���[���T�C�Y�ύX
	
	void DrawScreen();						// ��ʍX�V
	void SnapShot( const char * );			// �X�i�b�v�V���b�g
	
	int ScreenX() const;					// �L���X�N���[�����擾
	int ScreenY() const;					// �L���X�N���[�������擾
	
	VSurface *GetSubBuffer();				// �T�u�o�b�t�@�擾
	HWINDOW GetWindowHandle();				// �E�B���h�E�n���h���擾
};


#endif	// GRAPH_H_INCLUDED
