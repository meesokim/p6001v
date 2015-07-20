#ifndef LOG_H_INCLUDED
#define LOG_H_INCLUDED


/////////////////////////////////////////////////////////////////////////////
// �f�o�b�O���O�֌W
/////////////////////////////////////////////////////////////////////////////
// ���O�����ꍇ1�ɂ���
#define KEY_LOG		0	// �L�[���͊֌W
#define MEM_LOG		0	// �������֌W
#define TAPE_LOG	0	// TAPE�֌W
#define P6T2_LOG	0	// P6T2�֌W
#define DISK_LOG	0	// DISK�֌W
#define FDC_LOG		0	// FDC�֌W
#define D88_LOG		0	// D88�֌W
#define PSG_LOG		0	// PSG/OPN�֌W
#define INTR_LOG	0	// �����݊֌W
#define CPU_LOG		0	// CPU�֌W
#define SUB_LOG		0	// �T�uCPU�֌W
#define PPI_LOG		0	// 8255�֌W
#define SND_LOG		0	// Sound�֌W
#define GRP_LOG		0	// �`��֌W
#define VDG_LOG		0	// VDG�֌W
#define OSD_LOG		0	// OS�ˑ��֌W
#define TIC_LOG		0	// �X�P�W���[���֌W
#define INI_LOG		0	// INI�֌W
#define CON_LOG		0	// �R���X�g���N�^�֌W
#define IO_LOG		0	// I/O�֌W
#define WIN_LOG		0	// �E�B���h�E�֌W
#define CONST_LOG	0	// �R���X�g���N�^�E�f�X�g���N�^�֌W
#define VOI_LOG		0	// ���������֌W
#define VM_LOG		0	// ���z�}�V���R�A�֌W

#if KEY_LOG || MEM_LOG || TAPE_LOG || P6T2_LOG || DISK_LOG || D88_LOG || FDC_LOG || PSG_LOG || INTR_LOG || CPU_LOG || SUB_LOG || PPI_LOG || SND_LOG || GRP_LOG || VDG_LOG || OSD_LOG || TIC_LOG || INI_LOG || CON_LOG || IO_LOG || WIN_LOG || CONST_LOG || VOI_LOG || VM_LOG

#include <stdio.h>
#define PRINTD(m,...)	{ if( m ){ fprintf( stdout, __VA_ARGS__ );	fflush( stdout ); } }
#else
#define PRINTD(m,...)
#endif


#endif	// LOG_H_INCLUDED
