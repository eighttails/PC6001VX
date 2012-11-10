// ----------------------------------------------------------------------------
//	M88 - PC-8801 series emulator
//	Copyright (C) cisc 1999.
//  をベースにゆみたろが細工したものです
// ----------------------------------------------------------------------------
//	$Id: error.cpp,v 1.6 2002/04/07 05:40:08 cisc Exp $

#include "error.h"
#include "osd.h"

Error::Errno Error::err = Error::NoError;

const char *Error::ErrorText[Error::EndofErrors] =
{
	MSERR_NoError,				// NoError
	MSERR_Unknown,				// Unknown
	MSERR_MemAllocFailed,		// MemAllocFailed
	MSERR_RomChange,			// RomChange
	MSERR_NoRom,				// NoRom
	MSERR_RomSizeNG,			// RomSizeNG
	MSERR_RomCrcNG,				// RomCrcNG
	MSERR_LibInitFailed,		// LibInitFailed
	MSERR_InitFailed,			// InitFailed
	MSERR_FontLoadFailed,		// FontLoadFailed
	MSERR_FontCreateFailed,		// FontCreateFailed
	MSERR_IniDefault,			// IniDefault
	MSERR_IniReadFailed,		// IniReadFailed
	MSERR_IniWriteFailed,		// IniWriteFailed
	MSERR_TapeMountFailed,		// TapeMountFailed
	MSERR_DiskMountFailed,		// DiskMountFailed
	MSERR_ExtRomMountFailed,	// ExtRomMountFailed
	MSERR_DokoReadFailed,		// DokoReadFailed
	MSERR_DokoWriteFailed,		// DokoWriteFailed
	MSERR_DokoDiffVersion,		// DokoDiffVersion
	MSERR_ReplayPlayError,		// ReplayPlayError
	MSERR_ReplayRecError,		// ReplayRecError
	MSERR_NoReplayData			// NoReplayData
};

const char *Error::GetErrorText()
{
	return ErrorText[err];
}

void Error::SetError( Errno e )
{
	err = e;
}

Error::Errno Error::GetError( void )
{
	return err;
}
