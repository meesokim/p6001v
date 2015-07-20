// ----------------------------------------------------------------------------
//	M88 - PC-8801 series emulator
//	Copyright (C) cisc 1999.
//  ���x�[�X�ɂ�݂��낪�׍H�������̂ł�
// ----------------------------------------------------------------------------
//	$Id: error.h,v 1.2 1999/12/07 00:14:14 cisc Exp $

#ifndef ERROR_H_INCLUDED
#define ERROR_H_INCLUDED

class Error {
public:
	enum Errno
	{
		NoError = 0,
		Unknown,
		MemAllocFailed,
		RomChange,
		NoRom,
		RomSizeNG,
		RomCrcNG,
		LibInitFailed,
		InitFailed,
		FontLoadFailed,
		FontCreateFailed,
		IniDefault,
		IniReadFailed,
		IniWriteFailed,
		TapeMountFailed,
		DiskMountFailed,
		ExtRomMountFailed,
		DokoReadFailed,
		DokoWriteFailed,
		DokoDiffVersion,
		ReplayPlayError,
		ReplayRecError,
		NoReplayData,
		
		EndofErrors
	};
	
public:
	static void SetError( Errno e );
	static Errno GetError();
	static const char *GetErrorText();
	static void Reset();
	
private:
	Error();
	
	static Errno err;
	static const char *ErrorText[EndofErrors];
};

#endif // ERROR_H_INCLUDED
