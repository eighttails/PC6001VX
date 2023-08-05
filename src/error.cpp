/////////////////////////////////////////////////////////////////////////////
//  P C 6 0 0 1 V
//  Copyright 1999 Yumitaro
/////////////////////////////////////////////////////////////////////////////
// --------------------------------------------------------------------------
//	M88 - PC-8801 series emulator
//	Copyright (C) cisc 1999.
// --------------------------------------------------------------------------
//	$Id: error.cpp,v 1.6 2002/04/07 05:40:08 cisc Exp $

#include <stdexcept>

#include "error.h"
#include "common.h"


const std::vector<TextID> ErrorText =
{
	TERR_NoError,			// NoError
	TERR_Unknown,			// Unknown
	TERR_MemAllocFailed,	// MemAllocFailed
	TERR_RomChange,			// RomChange
	TERR_NoRom,				// NoRom
	TERR_NoRomChange,		// NoRomChange
	TERR_RomSizeNG,			// RomSizeNG
	TERR_RomCrcNG,			// RomCrcNG
	TERR_LibInitFailed,		// LibInitFailed
	TERR_InitFailed,		// InitFailed
	TERR_FontLoadFailed,	// FontLoadFailed
	TERR_FontCreateFailed,	// FontCreateFailed
	TERR_IniDefault,		// IniDefault
	TERR_IniReadFailed,		// IniReadFailed
	TERR_IniWriteFailed,	// IniWriteFailed
	TERR_TapeMountFailed,	// TapeMountFailed
	TERR_DiskMountFailed,	// DiskMountFailed
	TERR_ExtRomMountFailed,	// ExtRomMountFailed
	TERR_DokoReadFailed,	// DokoReadFailed
	TERR_DokoWriteFailed,	// DokoWriteFailed
	TERR_DokoDiffVersion,	// DokoDiffVersion
	TERR_ReplayPlayError,	// ReplayPlayError
	TERR_ReplayRecError,	// ReplayRecError
	TERR_NoReplayData,		// NoReplayData
	TERR_CaptureFailed		// CaptureFailed

};

Error::Errno Error::err = Error::NoError;
std::string Error::arg = "";

void Error::SetError( Errno e, std::string ar )
{
	err = e;
	
	try{
		arg = GetText( ErrorText.at( e ) );
		if( ar.size() ){
			arg += "\n\n" + ar;
		}
	}
	catch( std::out_of_range& ){
		arg = GetText( TERR_Unknown );
	}
}

Error::Errno Error::GetError( void )
{
	return err;
}

const std::string& Error::GetErrorText()
{
	return arg;
}

void Error::Clear( void )
{
	err = NoError;
	arg.clear();
}

