// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'madRipeMD.pas' rev: 22.00

#ifndef MadripemdHPP
#define MadripemdHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Madripemd
{
//-- type declarations -------------------------------------------------------
typedef System::StaticArray<System::Byte, 20> TDigest;

//-- var, const, procedure ---------------------------------------------------
extern PACKAGE unsigned __fastcall InitHash(void);
extern PACKAGE void __fastcall UpdateHash(unsigned hash, void * buf, int len);
extern PACKAGE TDigest __fastcall CloseHash(unsigned &hash);
extern PACKAGE TDigest __fastcall Hash(void * buf, int len)/* overload */;
extern PACKAGE TDigest __fastcall Hash(System::AnsiString buf)/* overload */;

}	/* namespace Madripemd */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Madripemd;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// MadripemdHPP
