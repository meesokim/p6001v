#include <new>

#include "error.h"
#include "io.h"
#include "log.h"


// ---------------------------------------------------------------------------
//	IOBus : I/O��Ԃ�񋟂���N���X
//	  Original     : cisc
//	  Modification : Yumitaro
// ---------------------------------------------------------------------------
IOBus::DummyIO IOBus::dummyio;

IOBus::IOBus() : ins(NULL), outs(NULL), flags(NULL), devlist(NULL), banksize(0) {}

IOBus::~IOBus()
{
	if( ins ) delete [] ins;
	if( outs ) delete [] outs;
	if( flags ) delete [] flags;
}

////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool IOBus::Init( DeviceList* dl, int bs )
{
	devlist  = dl;
	banksize = bs;
	
	// ���j�Z�Ŋ肢�܂��Ă�
	if( ins ){   delete [] ins;   ins   = NULL; }
	if( outs ){  delete [] outs;  outs  = NULL; }
	if( flags ){ delete [] flags; flags = NULL; }
	
	// �������m��
	try{
		ins   = new InBank[banksize];
		outs  = new OutBank[banksize];
		flags = new BYTE[banksize];
	}
	catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
		Error::SetError( Error::MemAllocFailed );
		if( ins ){   delete [] ins;   ins   = NULL; }
		if( outs ){  delete [] outs;  outs  = NULL; }
		return false;
	}
	
	ZeroMemory( flags, banksize );
	
	// �Ƃ肠�����S�Ẵ|�[�g�Ƀ_�~�[�f�o�C�X�����蓖�Ă�
	for( int i=0; i<banksize; i++ ){
		ins[i].device  = &dummyio;
		ins[i].func    = STATIC_CAST( InFuncPtr, &DummyIO::dummyin );
		ins[i].next    = 0;
		outs[i].device = &dummyio;
		outs[i].func   = STATIC_CAST( OutFuncPtr, &DummyIO::dummyout );
		outs[i].next   = 0;
	}
	
	return true;
}


////////////////////////////////////////////////////////////////
// �f�o�C�X�ڑ�
////////////////////////////////////////////////////////////////
// IN/OUT -----------
bool IOBus::Connect( IDevice* device, const Connector* connector )
{
	if( !device || !connector ) return false;
	
	if( devlist ) devlist->Add(device);
	
	const IDevice::Descriptor* desc = device->GetDesc();
	
	for( ; connector->rule; connector++ ){
		switch( connector->rule & 3 ){
		case portin:
			if( !ConnectIn(connector->bank, device, desc->indef[connector->id]) )
				return false;
			break;
			
		case portout:
			if( !ConnectOut(connector->bank, device, desc->outdef[connector->id]) )
				return false;
			break;
		}
	}
	return true;
}


// IN -----------
bool IOBus::ConnectIn( int bank, IDevice* device, InFuncPtr func )
{
	PRINTD( IO_LOG, "[IO][ConnectIn] %02XH -> ", bank&0xff );
	
	InBank* i = &ins[bank];
	if( i->func == STATIC_CAST( InFuncPtr, &DummyIO::dummyin ) ){
		// �ŏ��̐ڑ�
		i->device = device;
		i->func   = func;
	}else{
		// 2��ڈȍ~�̐ڑ�
		InBank *j;
		try{
			j = new InBank;
		}
		catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
			Error::SetError( Error::MemAllocFailed );
			PRINTD( IO_LOG, "Failed\n" );
			return false;
		}
		j->device = device;
		j->func   = func;
		j->next   = i->next;
		i->next   = j;
	}
	PRINTD( IO_LOG, "OK\n" );
	return true;
}


// OUT -----------
bool IOBus::ConnectOut( int bank, IDevice* device, OutFuncPtr func )
{
	PRINTD( IO_LOG, "[IO][ConnectOut] %02XH -> ", bank&0xff );
	
	OutBank* i = &outs[bank];
	if( i->func == STATIC_CAST( OutFuncPtr, &DummyIO::dummyout ) ){
		// �ŏ��̐ڑ�
		i->device = device;
		i->func   = func;
	}else{
		// 2��ڈȍ~�̐ڑ�
		OutBank *j;
		try{
			j = new OutBank;
		}
		catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
			Error::SetError( Error::MemAllocFailed );
			PRINTD( IO_LOG, "Failed\n" );
			return false;
		}
		j->device = device;
		j->func   = func;
		j->next   = i->next;
		i->next   = j;
	}
	PRINTD( IO_LOG, "OK\n" );
	return true;
}


////////////////////////////////////////////////////////////////
// �f�o�C�X�ؒf
////////////////////////////////////////////////////////////////
bool IOBus::Disconnect( IDevice* device )
{
	if( devlist ) devlist->Del(device);
	
	// IN
  	for( int i=0; i<banksize; i++ ){
		InBank* current = &ins[i];
		InBank* referer = 0;
		while( current ){
			InBank* next = current->next;
			if( current->device == device ){
				if( referer ){
					referer->next = next;
					delete current;
				}else{
					// �폜����ׂ��A�C�e�����ŏ��ɂ������ꍇ
					if( next ){
						// ���̃A�C�e���̓��e�𕡎ʂ��č폜
						*current = *next;
						referer = 0;
						delete next;
						continue;
					}else{
						// ���̃A�C�e�����B��̃A�C�e���������ꍇ
						current->func = STATIC_CAST( InFuncPtr, &DummyIO::dummyin );
					}
				}
			}
			current = next;
		}
	}
	
	// OUT
	for( int i=0; i<banksize; i++ ){
		OutBank* current = &outs[i];
		OutBank* referer = 0;
		while( current ){
			OutBank* next = current->next;
			if( current->device == device ){
				if( referer ){
					referer->next = next;
					delete current;
				}else{
					// �폜����ׂ��A�C�e�����ŏ��ɂ������ꍇ
					if( next ){
						// ���̃A�C�e���̓��e�𕡎ʂ��č폜
						*current = *next;
						referer = 0;
						delete next;
						continue;
					}else{
						// ���̃A�C�e�����B��̃A�C�e���������ꍇ
						current->func = STATIC_CAST( OutFuncPtr, &DummyIO::dummyout );
					}
				}
			}
			current = next;
		}
	}
	return true;
}


////////////////////////////////////////////////////////////////
// IN�֐�
////////////////////////////////////////////////////////////////
BYTE IOBus::In( int port )
{
	InBank* list = &ins[port&0xff];
	
	BYTE data = 0xff;
	do{
		data &= (list->device->*list->func)( port );
		list = list->next;
	} while ( list );
	return data;
}


////////////////////////////////////////////////////////////////
// OUT�֐�
////////////////////////////////////////////////////////////////
void IOBus::Out( int port, BYTE data )
{
	OutBank* list = &outs[port&0xff];
	do{
		(list->device->*list->func)( port, data );
		list = list->next;
	} while ( list );
}


////////////////////////////////////////////////////////////////
// �_�~�[�f�o�C�X(IN)
////////////////////////////////////////////////////////////////
BYTE IOBus::DummyIO::dummyin( int )
{
	return 0xff;
}


////////////////////////////////////////////////////////////////
// �_�~�[�f�o�C�X(OUT)
////////////////////////////////////////////////////////////////
void IOBus::DummyIO::dummyout( int, BYTE )
{
	return;
}





















////////////////////////////////////////////////////////////////
// �R���X�g���N�^
////////////////////////////////////////////////////////////////
IO6::IO6( void ) : io(NULL), dl(NULL)
{
	INITARRAY( Iwait, 0 );
	INITARRAY( Owait, 0 );
}


////////////////////////////////////////////////////////////////
// �f�X�g���N�^
////////////////////////////////////////////////////////////////
IO6::~IO6( void )
{
	if( dl ) delete dl;
	if( io ) delete io;
}


////////////////////////////////////////////////////////////////
// ������
////////////////////////////////////////////////////////////////
bool IO6::Init( int banksize )
{
	PRINTD( IO_LOG, "[IO][Init]\n" );
	
	// �I�u�W�F�N�g�m��
	try{
		dl = new DeviceList;	// �f�o�C�X���X�g
		io = new IOBus;			// I/O
		
		if( !io->Init( dl, banksize ) ) throw Error::InitFailed;
	}
	catch( std::bad_alloc ){	// new �Ɏ��s�����ꍇ
		Error::SetError( Error::MemAllocFailed );
		if( dl ){ delete dl; dl = NULL; }
		return false;
	}
	// ��O����
	catch( Error::Errno i ){
		Error::SetError( i );
		return false;
	}
	
	for( int i=0; i<BANKSIZE; i++ ) Iwait[i] = Owait[i] = 0;
	
	return true;
}


////////////////////////////////////////////////////////////////
// �f�o�C�X�ڑ�
////////////////////////////////////////////////////////////////
bool IO6::Connect( IDevice* device, const IOBus::Connector* connector )
{
	return io->Connect( device, connector );
}


////////////////////////////////////////////////////////////////
// �f�o�C�X�ؒf
////////////////////////////////////////////////////////////////
bool IO6::Disconnect( IDevice* device )
{
	return io->Disconnect( device );
}


////////////////////////////////////////////////////////////////
// IN�֐�
////////////////////////////////////////////////////////////////
BYTE IO6::In( int port, int *wcnt )
{
	PRINTD( IO_LOG, "[IO][In] port : %02X\n", port&0xff );
	
	if( wcnt ) (*wcnt) += Iwait[port&0xff];
	return io->In( port );
}


////////////////////////////////////////////////////////////////
// OUT�֐�
////////////////////////////////////////////////////////////////
void IO6::Out( int port, BYTE data, int *wcnt )
{
	PRINTD( IO_LOG, "[IO][Out] port : %02X  data : %02X\n", port&0xff, data );
	
	if( wcnt ) (*wcnt) += Owait[port&0xff];
	io->Out( port, data );
}


////////////////////////////////////////////////////////////////
// IN �E�F�C�g�ݒ�
////////////////////////////////////////////////////////////////
void IO6::SetInWait( int port, int wait )
{
	Iwait[port&0xff] = wait;
}


////////////////////////////////////////////////////////////////
// OUT�E�F�C�g�ݒ�
////////////////////////////////////////////////////////////////
void IO6::SetOutWait( int port, int wait )
{
	Owait[port&0xff] = wait;
}


////////////////////////////////////////////////////////////////
// IN �E�F�C�g�擾
////////////////////////////////////////////////////////////////
int IO6::GetInWait( int port )
{
	return Iwait[port&0xff];
}


////////////////////////////////////////////////////////////////
// IN �E�F�C�g�擾
////////////////////////////////////////////////////////////////
int IO6::GetOutWait( int port )
{
	return Owait[port&0xff];
}
