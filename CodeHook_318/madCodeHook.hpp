// CodeGear C++Builder
// Copyright (c) 1995, 2010 by Embarcadero Technologies, Inc.
// All rights reserved

// (DO NOT EDIT: machine generated header) 'madCodeHook.pas' rev: 22.00

#ifndef MadcodehookHPP
#define MadcodehookHPP

#pragma delphiheader begin
#pragma option push
#pragma option -w-      // All warnings off
#pragma option -Vx      // Zero-length empty class member functions
#pragma pack(push,8)
#include <System.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <madTypes.hpp>	// Pascal unit
#include <madDisAsm.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Madcodehook
{
//-- type declarations -------------------------------------------------------
typedef void __stdcall (*TIpcCallback)(char * name, void * messageBuf, unsigned messageLen, void * answerBuf, unsigned answerLen);

//-- var, const, procedure ---------------------------------------------------
static const System::ShortInt SECURE_MEMORY_MAPS = 0x2;
static const System::ShortInt DISABLE_CHANGED_CODE_CHECK = 0x3;
static const System::ShortInt USE_NEW_IPC_LOGIC = 0x4;
static const System::ShortInt SET_INTERNAL_IPC_TIMEOUT = 0x5;
static const System::ShortInt VMWARE_INJECTION_MODE = 0x6;
static const System::ShortInt DONT_TOUCH_RUNNING_PROCESSES = 0x7;
static const System::ShortInt RENEW_OVERWRITTEN_HOOKS = 0x9;
static const System::ShortInt INJECT_INTO_RUNNING_PROCESSES = 0xa;
static const System::ShortInt UNINJECT_FROM_RUNNING_PROCESSES = 0xb;
static const System::ShortInt X86_ALLOCATION_ADDRESS = 0xc;
static const System::ShortInt DONT_COUNT = 0x1;
static const System::ShortInt NO_SAFE_UNHOOKING = 0x1;
static const System::ShortInt NO_IMPROVED_SAFE_UNHOOKING = 0x40;
static const System::ShortInt SAFE_HOOKING = 0x20;
static const System::Word NO_AUTO_UNHOOKING = 0x100;
static const System::ShortInt MIXTURE_MODE = 0x2;
static const System::ShortInt NO_MIXTURE_MODE = 0x10;
static const System::Byte ALLOW_WINSOCK2_MIXTURE_MODE = 0x80;
static const System::Word FOLLOW_JMP = 0x200;
static const System::ShortInt SYSTEM_WIDE_9X = 0x4;
static const System::ShortInt ACCEPT_UNKNOWN_TARGETS_9X = 0x8;
extern PACKAGE unsigned ALL_SESSIONS;
extern PACKAGE unsigned CURRENT_SESSION;
extern PACKAGE bool amMchDll;
extern PACKAGE void __fastcall InitSpecialProcs(void);
extern PACKAGE void __stdcall AnsiToWide(char * ansi, System::WideChar * wide);
extern PACKAGE void __stdcall WideToAnsi(System::WideChar * wide, char * ansi);
extern PACKAGE unsigned __stdcall CreateGlobalMutex(char * name);
extern PACKAGE unsigned __stdcall OpenGlobalMutex(char * name);
extern PACKAGE unsigned __stdcall CreateGlobalFileMapping(char * name, unsigned size);
extern PACKAGE unsigned __stdcall OpenGlobalFileMapping(char * name, BOOL write);
extern PACKAGE unsigned __stdcall CreateGlobalEvent(char * name, BOOL manual, BOOL initialState);
extern PACKAGE unsigned __stdcall OpenGlobalEvent(char * name);
extern PACKAGE void * __fastcall FindRealCode(void * code);
extern PACKAGE BOOL __stdcall HookCode(void * code, void * callbackFunc, /* out */ void * &nextHook, unsigned flags = (unsigned)(0x0));
extern PACKAGE BOOL __stdcall HookAPI(char * module, char * api, void * callbackFunc, /* out */ void * &nextHook, unsigned flags = (unsigned)(0x0));
extern PACKAGE BOOL __stdcall RenewHook(void * &nextHook);
extern PACKAGE int __stdcall IsHookInUse(void * &nextHook);
extern PACKAGE BOOL __stdcall UnhookCode(void * &nextHook);
extern PACKAGE BOOL __stdcall UnhookAPI(void * &nextHook);
extern PACKAGE void __fastcall CollectHooks(void);
extern PACKAGE void __fastcall FlushHooks(void);
extern PACKAGE BOOL __stdcall RestoreCode(void * code);
extern PACKAGE BOOL __stdcall AmUsingInputDesktop(void);
extern PACKAGE unsigned __stdcall GetCallingModule(void * returnAddress = (void *)(0x0));
extern PACKAGE BOOL __stdcall AmSystemProcess(void);
extern PACKAGE unsigned __stdcall GetCurrentSessionId(void);
extern PACKAGE unsigned __stdcall GetInputSessionId(void);
extern PACKAGE BOOL __stdcall ProcessIdToFileNameA(unsigned processId, char * fileName, System::Word bufLenInChars);
extern PACKAGE BOOL __stdcall ProcessIdToFileNameW(unsigned processId, System::WideChar * fileName, System::Word bufLenInChars);
extern PACKAGE bool __fastcall ProcessIdToFileName(unsigned processId, System::AnsiString &fileName);
extern PACKAGE BOOL __stdcall InstallInjectionDriver(System::WideChar * driverName, System::WideChar * fileName32bit, System::WideChar * fileName64bit, System::WideChar * description);
extern PACKAGE BOOL __stdcall UninstallInjectionDriver(System::WideChar * driverName);
extern PACKAGE BOOL __stdcall StopInjectionDriver(System::WideChar * driverName);
extern PACKAGE BOOL __stdcall StartInjectionDriver(System::WideChar * driverName);
extern PACKAGE BOOL __stdcall IsInjectionDriverInstalled(System::WideChar * driverName);
extern PACKAGE BOOL __stdcall IsInjectionDriverRunning(System::WideChar * driverName);
extern PACKAGE BOOL __stdcall LoadInjectionDriver(System::WideChar * driverName, System::WideChar * fileName32bit, System::WideChar * fileName64bit);
extern PACKAGE BOOL __stdcall SetInjectionMethod(System::WideChar * driverName, BOOL newInjectionMethod);
extern PACKAGE BOOL __stdcall SetMadCHookOption(unsigned option, System::WideChar * param);
extern PACKAGE void __fastcall AutoUnhook(unsigned moduleHandle);
extern PACKAGE bool __fastcall CheckLibFilePath(char * &libA, System::WideChar * &libW, System::AnsiString &strA, System::AnsiString &strW);
extern PACKAGE BOOL __stdcall UninjectAllLibrariesW(System::WideChar * driverName, Madtypes::TPCardinal excludePIDs = (Madtypes::TPCardinal)(0x0), unsigned timeOutPerUninject = (unsigned)(0x1b58));
extern PACKAGE BOOL __stdcall UninjectAllLibrariesA(char * driverName, Madtypes::TPCardinal excludePIDs = (Madtypes::TPCardinal)(0x0), unsigned timeOutPerUninject = (unsigned)(0x1b58));
extern PACKAGE BOOL __stdcall UninjectAllLibraries(char * driverName, Madtypes::TPCardinal excludePIDs = (Madtypes::TPCardinal)(0x0), unsigned timeOutPerUninject = (unsigned)(0x1b58));
extern PACKAGE BOOL __stdcall CreateProcessExA(char * applicationName, char * commandLine, Windows::PSecurityAttributes processAttr, Windows::PSecurityAttributes threadAttr, BOOL inheritHandles, unsigned creationFlags, void * environment, char * currentDirectory, const Windows::_STARTUPINFOA &startupInfo, _PROCESS_INFORMATION &processInfo, char * loadLibrary);
extern PACKAGE BOOL __stdcall CreateProcessExW(System::WideChar * applicationName, System::WideChar * commandLine, Windows::PSecurityAttributes processAttr, Windows::PSecurityAttributes threadAttr, BOOL inheritHandles, unsigned creationFlags, void * environment, System::WideChar * currentDirectory, const Windows::_STARTUPINFOW &startupInfo, _PROCESS_INFORMATION &processInfo, System::WideChar * loadLibrary);
extern PACKAGE bool __fastcall CreateProcessEx(char * applicationName, char * commandLine, Windows::PSecurityAttributes processAttr, Windows::PSecurityAttributes threadAttr, BOOL inheritHandles, unsigned creationFlags, void * environment, char * currentDirectory, const Windows::_STARTUPINFOA &startupInfo, _PROCESS_INFORMATION &processInfo, System::AnsiString loadLibrary);
extern PACKAGE BOOL __stdcall InjectLibraryA(char * libFileName, unsigned processHandle, unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall InjectLibraryW(System::WideChar * libFileName, unsigned processHandle, unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall InjectLibrary(char * libFileName, unsigned processHandle, unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall UninjectLibraryA(char * libFileName, unsigned processHandle, unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall UninjectLibraryW(System::WideChar * libFileName, unsigned processHandle, unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall UninjectLibrary(char * libFileName, unsigned processHandle, unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall InjectLibraryA(char * driverName, char * libFileName, unsigned session, BOOL systemProcesses, char * includeMask = (char *)(0x0), char * excludeMask = (char *)(0x0), Madtypes::TPCardinal excludePIDs = (Madtypes::TPCardinal)(0x0), unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall InjectLibraryW(System::WideChar * driverName, System::WideChar * libFileName, unsigned session, BOOL systemProcesses, System::WideChar * includeMask = (System::WideChar *)(0x0), System::WideChar * excludeMask = (System::WideChar *)(0x0), Madtypes::TPCardinal excludePIDs = (Madtypes::TPCardinal)(0x0), unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall InjectLibrary(char * driverName, char * libFileName, unsigned session, BOOL systemProcesses, char * includeMask = (char *)(0x0), char * excludeMask = (char *)(0x0), Madtypes::TPCardinal excludePIDs = (Madtypes::TPCardinal)(0x0), unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall UninjectLibraryA(char * driverName, char * libFileName, unsigned session, BOOL systemProcesses, char * includeMask = (char *)(0x0), char * excludeMask = (char *)(0x0), Madtypes::TPCardinal excludePIDs = (Madtypes::TPCardinal)(0x0), unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall UninjectLibraryW(System::WideChar * driverName, System::WideChar * libFileName, unsigned session, BOOL systemProcesses, System::WideChar * includeMask = (System::WideChar *)(0x0), System::WideChar * excludeMask = (System::WideChar *)(0x0), Madtypes::TPCardinal excludePIDs = (Madtypes::TPCardinal)(0x0), unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall UninjectLibrary(char * driverName, char * libFileName, unsigned session, BOOL systemProcesses, char * includeMask = (char *)(0x0), char * excludeMask = (char *)(0x0), Madtypes::TPCardinal excludePIDs = (Madtypes::TPCardinal)(0x0), unsigned timeOut = (unsigned)(0x1b58))/* overload */;
extern PACKAGE BOOL __stdcall CreateIpcQueueEx(char * ipc, TIpcCallback callback, unsigned maxThreadCount = (unsigned)(0x10), unsigned maxQueueLen = (unsigned)(0x1000));
extern PACKAGE BOOL __stdcall CreateIpcQueue(char * ipc, TIpcCallback callback);
extern PACKAGE BOOL __stdcall SendIpcMessage(char * ipc, void * messageBuf, unsigned messageLen, void * answerBuf = (void *)(0x0), unsigned answerLen = (unsigned)(0x0), unsigned answerTimeOut = (unsigned)(0xffffffff), BOOL handleMessages = true);
extern PACKAGE BOOL __stdcall DestroyIpcQueue(char * ipc);
extern PACKAGE BOOL __stdcall AddAccessForEveryone(unsigned processOrService, unsigned access);
extern PACKAGE void __fastcall EnableAllPrivileges(void);
extern PACKAGE void __fastcall InitializeMadCHook(void);
extern PACKAGE void __fastcall FinalizeMadCHook(void);

}	/* namespace Madcodehook */
#if !defined(DELPHIHEADER_NO_IMPLICIT_NAMESPACE_USE)
using namespace Madcodehook;
#endif
#pragma pack(pop)
#pragma option pop

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// MadcodehookHPP
