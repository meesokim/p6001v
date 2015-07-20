#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include "ini.h"
#include "keydef.h"
#include "vsurface.h"


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
class CFG6 {
protected:
	cIni *Ini;								// INI�I�u�W�F�N�g�|�C���^
	
	// INI�t�@�C���ɕۑ����Ȃ������o
	char IniPath[PATH_MAX];					// INI�t�@�C���p�X
	char Caption[256];						// �E�B���h�E�L���v�V����
	char DokoFile[PATH_MAX];				// �ǂ��ł�SAVE�t�@�C����
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	bool MonDisp;							// ���j�^�E�B���h�E�\����� true:�\�� false:��\��
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	
	// INI�t�@�C���ɕۑ����郁���o
	char RomPath[PATH_MAX];					// ROM�p�X
	char ExtRomPath[PATH_MAX];				// �g��ROM�p�X
	char ExtRomFile[PATH_MAX];				// �g��ROM�t�@�C����
	char WavePath[PATH_MAX];				// WAVE�p�X
	char TapePath[PATH_MAX];				// TAPE�p�X
	char TapeFile[PATH_MAX];				// TAPE�t�@�C����
	char SaveFile[PATH_MAX];				// TAPE(SAVE)�t�@�C����
	char DiskPath[PATH_MAX];				// DISK�p�X
	char DiskFile1[PATH_MAX];				// DISK1�t�@�C����
	char DiskFile2[PATH_MAX];				// DISK2�t�@�C����
	char ImgPath[PATH_MAX];					// �X�N���[���V���b�g�i�[�p�X
	char PrinterFile[PATH_MAX];				// �v�����^�t�@�C����
	char DokoSavePath[PATH_MAX];			// �ǂ��ł�SAVE�p�X
	
	// �萔
	static const P6KeyName P6KeyNameDef[];	// P6�L�[�R�[�h ���̒�`
	static const PCKeyName PCKeyNameDef[];	// ���z�L�[�R�[�h ���̒�`
	static const VKeyConv KeyIni[];			// ���z�L�[�R�[�h -> P6�L�[�R�[�h��`�����l
	static const COLOR24 STDColor[];		// �W���J���[�f�[�^
	
	
	void InitIni( cIni *, bool );			// INI�I�u�W�F�N�g�����l�ݒ�
	const char *GetPCKeyName( PCKEYsym );	// ���z�L�[�R�[�h���疼�̎擾
	const char *GetP6KeyName( P6KEYsym );	// P6�L�[�R�[�h���疼�̎擾
	PCKEYsym GetPCKeyCode( char * );		// �L�[���̂��牼�z�L�[�R�[�h���擾
	P6KEYsym GetP6KeyCode( char * );		// �L�[���̂���P6�L�[�R�[�h���擾

public:
	CFG6();								// �R���X�g���N�^
	virtual ~CFG6();						// �f�X�g���N�^
	
	bool Init();							// ������(INI�t�@�C���Ǎ���)
	bool Write();							// INI�t�@�C��������
	
	// �����o�A�N�Z�X�֐�
	// ����
	int GetModel();							// �@��擾
	void SetModel( int );					//     �ݒ�
	char *GetCaption();						// �E�B���h�E�L���v�V�����擾
	char *GetDokoFile();					// �ǂ��ł�SAVE�t�@�C�����擾
	void SetDokoFile( const char * );		//                       �ݒ�
	int GetOverClock();						// �I�[�o�[�N���b�N���擾
	void SetOverClock( int );				//                   �ݒ�
	
	// �������֌W
	char *GetRomPath();						// ROM�p�X�擾
	void SetRomPath( const char * );		//        �ݒ�
	bool GetUseExtRam();					// �g��RAM���g���擾
	void SetUseExtRam( bool );				//              �ݒ�
	char *GetExtRomPath();					// �g��ROM�p�X�擾
	void SetExtRomPath( const char * );		//            �ݒ�
	char *GetExtRomFile();					// �g��ROM�t�@�C�����擾
	void SetExtRomFile( const char * );		//                  �ݒ�
	bool GetCheckCRC();						// CRC�`�F�b�N�擾
	void SetCheckCRC( bool );				//            �ݒ�
	bool GetRomPatch();						// ROM�p�b�`�擾
	void SetRomPatch( bool );				//          �ݒ�
	
	// �L�[�{�[�h�֌W
	int GetKeyRepeat();						// �L�[���s�[�g�擾
	void SetKeyRepeat( int );				//             �ݒ�
	
	// �T�E���h�֌W
	int GetSampleRate();					// �T���v�����O���[�g�擾
	void SetSampleRate( int );				//                   �ݒ�
	int GetSoundBuffer();					// �T�E���h�o�b�t�@���{���擾
	void SetSoundBuffer( int );				//                       �ݒ�
	int GetMasterVol();						// �}�X�^�[���ʎ擾
	void SetMasterVol( int );				//             �ݒ�
	
	// PSG�֌W
	int GetPsgVol();						// PSG���ʎ擾
	void SetPsgVol( int );					//        �ݒ�
	int GetPsgLPF();						// PSG LPF�J�b�g�I�t���g���擾
	void SetPsgLPF( int );					//                        �ݒ�
	
	// ���������֌W
	char *GetWavePath();					// WAVE�p�X�擾
	void SetWavePath( const char * );		//         �ݒ�
	int GetVoiceVol();						// �����������ʎ擾
	void SetVoiceVol( int );				//             �ݒ�
	
	// CMT�֌W
	char *GetTapePath();					// TAPE�p�X�擾
	void SetTapePath( const char * );		//         �ݒ�
	char *GetTapeFile();					// TAPE�t�@�C�����擾
	void SetTapeFile( const char * );		//               �ݒ�
	int GetCmtVol();						// TAPE���j�^���ʎ擾
	void SetCmtVol( int );					//               �ݒ�
	bool GetTurboTAPE();					// Turbo TAPE �L���t���O�擾
	void SetTurboTAPE( bool );				//                      �ݒ�
	bool GetBoostUp();						// BoostUp �L���t���O�擾
	void SetBoostUp( bool );				//                   �ݒ�
	int GetMaxBoost1();						// BoostUp �ő�{��(N60���[�h)�擾
	void SetMaxBoost1( int );				//                            �ݒ�
	int GetMaxBoost2();						// BoostUp �ő�{��(N60m/N66���[�h)�擾
	void SetMaxBoost2( int );				//                                 �ݒ�
	char *GetSaveFile();					// TAPE(SAVE)�t�@�C�����擾
	void SetSaveFile( const char * );		//                     �ݒ�
	
	// DISK�֌W
	char *GetDiskPath();					// DISK�p�X�擾
	void SetDiskPath( const char * );		//         �ݒ�
	char *GetDiskFile( int );				// DISK�t�@�C�����擾
	void SetDiskFile( int, const char * );	//               �ݒ�
	int GetFddNum();						// FDD�ڑ��䐔�擾
	void SetFddNum( int );					//            �ݒ�
	
	// ��ʊ֌W
	const char *GetImgPath();				// �X�N���[���V���b�g�i�[�p�X�擾
	void SetImgPath( const char * );		//                           �ݒ�
	int GetMode4Color() const;				// ���[�h4�J���[���[�h�擾
	void SetMode4Color( int );				//                    �ݒ�
	bool GetScanLine() const;				// �X�L�������C���擾
	void SetScanLine( bool );				//               �ݒ�
	int GetScanLineBr() const;				// �X�L�������C���P�x�擾
	void SetScanLineBr( int );				//                   �ݒ�
	bool GetDispNTSC() const;				// 4:3�\���擾
	void SetDispNTSC( bool );				//        �ݒ�
	bool GetFullScreen() const;				// �t���X�N���[���擾
	void SetFullScreen( bool );				//               �ݒ�
	bool GetDispStat() const;				// �X�e�[�^�X�o�[�\����Ԏ擾
	void SetDispStat( bool );				//                       �ݒ�
	int GetFrameSkip() const;				// �t���[���X�L�b�v�擾
	void SetFrameSkip( int );				//                 �ݒ�
	
	// �r�f�I�L���v�`���֌W
	int GetAviBpp();						// �F�[�x�擾
	void SetAviBpp( int );					//       �ݒ�
	
	// �v�����^�֌W
	char *GetPrinterFile();					// �v�����^�t�@�C�����擾
	void SetPrinterFile( const char * );	//                   �ݒ�
	
	// �m�F�֌W
	bool GetCkQuit();						// �I�����m�F�擾
	void SetCkQuit( bool );					//           �ݒ�
	bool GetSaveQuit();						// �I����INI�ۑ��擾
	void SetSaveQuit( bool );				//              �ݒ�
	
	// ����@�\�E�g���@�\�֌W
	bool GetUseSoldier();					// ��m�̃J�[�g���b�W�g���t���O�擾
	void SetUseSoldier( bool );				//                             �ݒ�
	
	// �p���b�g�֌W
	COLOR24 *GetColor( int, COLOR24 * );	// �J���[�f�[�^�擾
	void SetColor( int, const COLOR24 * );	//             �ݒ�
	
	// �L�[��`�֌W
	P6KEYsym GetVKey( PCKEYsym );			// �L�[��`�擾
	void SetVKey( PCKEYsym, P6KEYsym );		//         �ݒ�
	int GetVKeyDef( VKeyConv ** );			// �L�[��`�z��擾
	
	#ifndef NOMONITOR	// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	bool GetMonDisp() const;				// ���j�^�E�B���h�E�\����Ԏ擾
	void SetMonDisp( bool );				// ���j�^�E�B���h�E�\����Ԑݒ�
	#endif				// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	
	// �ǂ��ł�SAVE�֌W
	char *GetDokoSavePath();				// �ǂ��ł�SAVE�p�X�擾
	void SetDokoSavePath( const char * );	//                 �ݒ�
	// ------------------------------------------
	bool DokoSave( cIni * );	// �ǂ��ł�SAVE
	bool DokoLoad( cIni * );	// �ǂ��ł�LOAD
	// ------------------------------------------
};

#endif	// CONFIG_H_INCLUDED
