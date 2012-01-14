// ----------------------------------------------------------------------------
//	M88 - PC-8801 series emulator
//	Copyright (C) cisc 1999.
//  をベースにゆみたろが細工したものです
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
		FontFailed,
		IniDefault,
		IniReadFailed,
		IniWriteFailed,
		TapeMountFailed,
		DiskMountFailed,
		ExtRomMountFailed,
		DokoReadFailed,
		DokoWriteFailed,
		DokoDiffModel,
		DokoDiffVersion,
		ReplayPlayError,
		ReplayDiffModel,
		ReplayRecError,
		NoReplayData,
		
		EndofErrors
	};
	
public:
	static void SetError( Errno e );
	static Errno GetError();
	static const char *GetErrorText();
	
private:
	Error();
	
	static Errno err;
	static const char *ErrorText[EndofErrors];
};

#endif // ERROR_H_INCLUDED
