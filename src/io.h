#ifndef IO_H_INCLUDED
#define IO_H_INCLUDED

#include "typedef.h"
#include "device.h"

// �f�t�H���g�|�[�g��
#define	BANKSIZE	256


////////////////////////////////////////////////////////////////
// �N���X��`
////////////////////////////////////////////////////////////////
// ---------------------------------------------------------------------------
//	IOBus : I/O��Ԃ�񋟂���N���X
//	  Original     : cisc
//	  Modification : Yumitaro
// ---------------------------------------------------------------------------
class IOBus {
// �^��`
public:
	typedef Device::InFuncPtr InFuncPtr;
	typedef Device::OutFuncPtr OutFuncPtr;
	
	struct InBank{
		IDevice* device;
		InFuncPtr func;
		InBank* next;
	};
	struct OutBank{
		IDevice* device;
		OutFuncPtr func;
		OutBank* next;
	};
	
	enum ConnectRule{
		end = 0, portin = 1, portout = 2
	};
	struct Connector{
		DWORD bank;
		BYTE rule;
		BYTE id;
	};
	
private:
	class DummyIO : public Device{
	public:
		DummyIO() : Device(NULL,0) {}
		~DummyIO() {}
		
		BYTE dummyin( int );
		void dummyout( int, BYTE );
	};

// ��������{��
public:
	IOBus();
	~IOBus();
	
	bool Init( DeviceList* devlist = 0, int bs = BANKSIZE );
	
	bool ConnectIn( int bank, IDevice* device, InFuncPtr func );
	bool ConnectOut( int bank, IDevice* device, OutFuncPtr func );
	
	InBank* GetIns() { return ins; }
	OutBank* GetOuts() { return outs; }
	BYTE* GetFlags() { return flags; }
	
	bool Connect( IDevice* device, const Connector* connector );
	bool Disconnect( IDevice* device );
	BYTE In( int port );
	void Out( int port, BYTE data );
	
private:
	InBank* ins;
	OutBank* outs;
	BYTE* flags;
	DeviceList* devlist;
	
	int banksize;
	
	static DummyIO dummyio;
};



class IO6 {
private:
	// �I�u�W�F�N�g�|�C���^
	IOBus *io;								// I/O�o�X
	DeviceList *dl;							// �f�o�C�X���X�g
	
	int Iwait[BANKSIZE];					// IN �E�F�C�g�e�[�u��
	int Owait[BANKSIZE];					// OUT�E�F�C�g�e�[�u��
	
public:
	IO6();									// �R���X�g���N�^
	~IO6();									// �f�X�g���N�^
	
	bool Init( int );						// ������
	
	bool Connect( IDevice*, const IOBus::Connector* );	// �f�o�C�X�ڑ�
	bool Disconnect( IDevice* );						// �f�o�C�X�ؒf
	
	BYTE In( int, int * = NULL );			// IN�֐�
	void Out( int, BYTE, int * = NULL );	// OUT�֐�
	
	void SetInWait( int, int );				// IN �E�F�C�g�ݒ�
	void SetOutWait( int, int );			// OUT�E�F�C�g�ݒ�
	int GetInWait( int );					// IN �E�F�C�g�擾
	int GetOutWait( int );					// OUT�E�F�C�g�擾
};


#endif		// IO_H_INCLUDED
