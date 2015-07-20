#ifndef PC6001V_H_INCLUDED
#define PC6001V_H_INCLUDED


/////////////////////////////////////////////////////////////////////////////
// �r���h�I�v�V���� (�s�v�ȍ��ڂ̓R�����g�A�E�g����)
/////////////////////////////////////////////////////////////////////////////
//#define	NOMONITOR	// ���j�^���[�h�Ȃ�
//#define	USEFMGEN	// PSG��fmgen���g��


/////////////////////////////////////////////////////////////////////////////
// �o�[�W�������,�@�햼�Ȃ�
/////////////////////////////////////////////////////////////////////////////
#define	VERSION		"1.23"
#define	APPNAME		"PC6001V"
#define	P60NAME		"PC-6001"
#define	P61NAME		"PC-6001A"
#define	P62NAME		"PC-6001mk2"
#define	P66NAME		"PC-6601"
#define	P64NAME		"PC-6001mk2SR"
#define	P68NAME		"PC-6601SR"


/////////////////////////////////////////////////////////////////////////////
// �I�v�V���� �����l
/////////////////////////////////////////////////////////////////////////////
#define	DEFAULT_MODEL			(60)				/* �@�� 60:PC-6001 62:PC-6001mk2 66:PC-6601 64:PC-6001mk2SR 68:PC-6601SR */
#define	DEFAULT_EXTRAM			(1)					/* �g��RAM 0:�Ȃ� 1:���� */
#define	DEFAULT_REPEAT			(70)				/* �L�[���s�[�g�̊Ԋu(�P��:ms 0�Ŗ���) */
#define	DEFAULT_SAMPLE_RATE		(44100)				/* �T���v�����O���[�g */
#define	SOUND_BUFFER_SIZE		(1)					/* �T�E���h�o�b�t�@���{��(��{����VSYNC) */
#define	DEFAULT_MASTERVOL		(70)				/* �}�X�^�[���� */
#define	DEFAULT_PSGVOL			(60)				/* PSG���� */
#define	DEFAULT_PSGLPF			(0)					/* PSG LPF�J�b�g�I�t���g��(0�Ŗ���) */
#define	DEFAULT_VOICEVOL		(100)				/* ������������ */
#define	DEFAULT_TAPEVOL			(10)				/* TAPE���j�^���� */
#define	DEFAULT_TAPELPF			(1540)				/* TAPE LPF�J�b�g�I�t���g��(0�Ŗ���) */
#define	DEFAULT_TURBO			(true)				/* Turbo TAPE Yes:�L�� No:���� */
#define	DEFAULT_BOOST			(false)				/* BoostUp Yes:�L�� No:���� */
#define DEFAULT_MAXBOOST60		(8)					/* BoostUp�ő�{��(N60���[�h) */
#define DEFAULT_MAXBOOST62		(5)					/* BoostUp�ő�{��(N60m/N66���[�h) */
#define	DEFAULT_FDD				(0)					/* FDD�ڑ��䐔 */
#define	DEFAULT_MODE4_COLOR		(1)					/* ���[�h�S�J���[���[�h 0:���m 1:��/�� 2:��/�� 3:�s���N/�� 4:��/�s���N */
#define	DEFAULT_SCANLINE		(true) 				/* �X�L�������C�� true:���� false:�Ȃ� */
#define	DEFAULT_SCANLINEBR		(75)				/* �X�L�������C���P�x (0-100)% */
#define	DEFAULT_DISPNTSC		(true) 				/* 4:3�\�� true:�L�� false:���� */
#define	DEFAULT_FRAMESKIP		(0)					/* �t���[���X�L�b�v */
#define	DEFAULT_OVERCLOCK		(100)				/* �I�[�o�[�N���b�N�� */
#define	DEFAULT_CHECKCRC		(true) 				/* CRC�`�F�b�N */
#define	DEFAULT_ROMPATCH		(true) 				/* ROM�p�b�` */
#define	DEFAULT_FULLSCREEN		(false) 			/* �t���X�N���[�� */
#define	DEFAULT_DISPSTATUS		(true) 				/* �X�e�[�^�X�o�[�\����� */
#define	DEFAULT_AVIBPP			(24) 				/* �r�f�I�L���v�`���F�[�x */
#define	DEFAULT_CKQUIT			(false) 			/* �I�����m�F */
#define	DEFAULT_SAVEQUIT		(true) 				/* �I����INI�ۑ� */
#define	DEFAULT_SOLDIER			(false) 			/* ��m�̃J�[�g���b�W�g���t���O */


#define	CPUM_CLOCK60			(3993600)			/* ���C��CPU�N���b�N(Hz) 60 */
#define	CPUM_CLOCK62			(4000000)			/* ���C��CPU�N���b�N(Hz) 62,66 */
#define	CPUM_CLOCK64			(3580000)			/* ���C��CPU�N���b�N(Hz) 64,68 */
#define	CPUS_CLOCK60			(CPUM_CLOCK60*2)	/* �T�uCPU�N���b�N(Hz) 60 */
#define	CPUS_CLOCK62			(CPUM_CLOCK62*2)	/* �T�uCPU�N���b�N(Hz) 62,66 */
#define	CPUS_CLOCK64			(CPUM_CLOCK64*2)	/* �T�uCPU�N���b�N(Hz) 64,68 */
#define	PSG_CLOCK60				(CPUM_CLOCK60/2)	/* PSG�N���b�N(Hz) 60 */
#define	PSG_CLOCK62				(CPUM_CLOCK62/2)	/* PSG�N���b�N(Hz) 62,66 */
#define	PSG_CLOCK64				(2000000)			/* PSG�N���b�N(Hz) 64,68 CPU�N���b�N�Ƃ͕�?? */

#define	VSYNC_HZ				(60)				/* VSYNC���g�� */
#define	DEFAULT_BAUD			(1200)				/* CMT�{�[���[�g �����l */
#define	DEFAULT_CMT_HZ			(DEFAULT_BAUD/12)	/* CMT�����ݎ��g�� �����l (1byte -> StartBit:1 + DataBit:8 + StopBit:3 = 12bits �Ƃ���) */

//#define	DEFAULT_FPS_CNT			(true)			/* FPS�J�E���^ true:�\�� false:��\�� */

/////////////////////////////////////////////////////////////////////////////
// �e��f�B���N�g���� �����l
/////////////////////////////////////////////////////////////////////////////
#define	ROM_DIR			"rom"				/* ROM�C���[�W */
#define	TAPE_DIR		"tape"				/* TAPE�C���[�W */
#define	DISK_DIR		"disk"				/* DISK�C���[�W */
#define	EXTROM_DIR		"extrom"			/* �g��ROM�C���[�W */
#define	IMAGE_DIR		"img"				/* �X�i�b�v�V���b�g */
#define	FONT_DIR		"font"				/* �t�H���g */
#define	WAVE_DIR		"wave"				/* WAVE�t�@�C�� */
#define	DOKOSAVE_DIR	"doko"				/* �ǂ��ł�SAVE�t�@�C�� */


/////////////////////////////////////////////////////////////////////////////
// �e��t�@�C���� �����l
/////////////////////////////////////////////////////////////////////////////
#define	IMG_EXT			"png"				/* �摜�t�@�C���g���q */
#define	CONF_FILE		"pc6001v.ini"		/* �ݒ�t�@�C�� */
#define	PRINTER_FILE	"printer.txt"		/* �v�����^�o�̓t�@�C�� */
#define	DOKODEMO_FILE	"dokodemo.dds"		/* �ǂ��ł�SAVE�t�@�C�� */
#define	SERIAL_FILE		"serial.txt"		/* �V���A���o�̓t�@�C�� */
#define	SAVE_FILE		"_csave.p6t"		/* TAPE(CSAVE)�t�@�C�� */
#define	FONTZ_FILE		"fontz12."IMG_EXT	/* ���p�t�H���g�t�@�C�� */
#define	FONTH_FILE		"fonth12."IMG_EXT	/* �S�p�t�H���g�t�@�C�� */

#define	SUBCPU60		"subcpu.60"			/* �T�uCPU ROM(PC-6001) */
#define	SUBCPU61		"subcpu.61"			/* �T�uCPU ROM(PC-6001A) */
#define	SUBCPU62		"subcpu.62"			/* �T�uCPU ROM(PC-6001mk2) */
#define	SUBCPU66		"subcpu.66"			/* �T�uCPU ROM(PC-6601) */
#define	SUBCPU64		"subcpu.64"			/* �T�uCPU ROM(PC-6001mk2SR) */
#define	SUBCPU68		"subcpu.68"			/* �T�uCPU ROM(PC-6601SR) */


#endif	// PC6001V_H_INCLUDED
