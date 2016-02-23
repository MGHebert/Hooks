// ***************************************************************
//  HookProcessCreation.dll   version: 1.1.0  ·  date: 2010-01-10
//  -------------------------------------------------------------
//  hook all process creation calls and ask for confirmation
//  -------------------------------------------------------------
//  Copyright (C) 1999 - 2010 www.madshi.net, All Rights Reserved
// ***************************************************************

// 2010-01-10 1.1.0 adjustments for madCodeHook 3
// 2003-06-15 1.0   initial release

// ***************************************************************
#include <atlstr.h>
#include <windows.h>
#include <atlbase.h>
#include ".\\CodeHook\\madCHook.h"
#include "Utilities.h"

// ***************************************************************


BOOL IsAllowed(LPCTSTR appNameA, LPTSTR cmdLineA, LPCWSTR appNameW, LPWSTR cmdLineW)
// ask the user whether the current process may execute the specified command line
{
  CHAR  arrChA   [MAX_PATH + 1];
  WCHAR arrChW   [MAX_PATH + 1];
  CHAR  question [MAX_PATH + 1];
  PCHAR pc;
  INT   i1, i2;
  if (!AmSystemProcess()) {
    // ask the name of the current process
    if (!(GetVersion() & 0x80000000)) {
      GetModuleFileNameW(NULL, arrChW, MAX_PATH);
      WideToAnsi(arrChW, arrChA);
    } else
      GetModuleFileNameA(NULL, arrChA, MAX_PATH);
    // we only want the file name
    i2 = 0;
    for (i1 = lstrlenA(arrChA); i1 > 0; i1--)
      if (arrChA[i1] == '\\') {
        i2 = i1 + 1;
        break;
      }
    lstrcpyA(question, "May the process ");
    lstrcatA(question, &arrChA[i2]);
    lstrcatA(question, " execute the following line?\n\n");
    // let's get a command line string which we can show to the user
    _try {
      if (cmdLineA) {
        pc = (PCHAR) LocalAlloc(LPTR, lstrlenA(cmdLineA) + 1);
        lstrcpyA(pc, cmdLineA);
      } else if (cmdLineW) {
        pc = (PCHAR) LocalAlloc(LPTR, lstrlenW(cmdLineW) + 1);
        WideToAnsi(cmdLineW, pc);
      } else if (appNameA) {
        pc = (PCHAR) LocalAlloc(LPTR, lstrlenA(appNameA) + 1);
        lstrcpyA(pc, appNameA);
      } else {
        pc = (PCHAR) LocalAlloc(LPTR, lstrlenW(appNameW) + 1);
        WideToAnsi(appNameW, pc);
      }
      if (lstrlenA(pc) > MAX_PATH)
        pc[MAX_PATH] = 0;
      lstrcatA(question, pc);
      LocalFree((HLOCAL) pc);
    } _except(true) {
      lstrcatA(question, "??? (invalid command line!)");
    }
    // finally let's ask the user
    if (!(GetVersion() & 0x80000000)) {
      AnsiToWide(question, arrChW);
      return (MessageBoxW(0, arrChW, L"Question", MB_ICONQUESTION | MB_YESNO | MB_TASKMODAL | MB_TOPMOST) == IDYES);
    } else
      return (MessageBoxA(0, question, "Question", MB_ICONQUESTION | MB_YESNO | MB_TASKMODAL | MB_TOPMOST) == IDYES);
  } else
    // let's allow system processes to execute whatever they want
    return true;
}



// ***************************************************************

BOOL (WINAPI *CreateProcessANext)  (LPCTSTR               lpApplicationName,
                                    LPTSTR                lpCommandLine,
                                    LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                    LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                    BOOL                  bInheritHandles,
                                    DWORD                 dwCreationFlags,
                                    LPVOID                lpEnvironment,
                                    LPCTSTR               lpCurrentDirectory,
                                    LPSTARTUPINFO         lpStartupInfo,
                                    LPPROCESS_INFORMATION lpProcessInformation);
BOOL (WINAPI *CreateProcessWNext)  (LPCWSTR               lpApplicationName,
                                    LPWSTR                lpCommandLine,
                                    LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                    LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                    BOOL                  bInheritHandles,
                                    DWORD                 dwCreationFlags,
                                    LPVOID                lpEnvironment,
                                    LPCWSTR               lpCurrentDirectory,
                                    LPSTARTUPINFO         lpStartupInfo,
                                    LPPROCESS_INFORMATION lpProcessInformation);
UINT (WINAPI *WinExecNext)         (LPCSTR                lpCmdLine,
                                    UINT                  uCmdShow);

// ***************************************************************

BOOL WINAPI CreateProcessACallback(LPCTSTR               lpApplicationName,
                                   LPTSTR                lpCommandLine,
                                   LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                   LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                   BOOL                  bInheritHandles,
                                   DWORD                 dwCreationFlags,
                                   LPVOID                lpEnvironment,
                                   LPCTSTR               lpCurrentDirectory,
                                   LPSTARTUPINFO         lpStartupInfo,
                                   LPPROCESS_INFORMATION lpProcessInformation)
{
  if (!IsAllowed(lpApplicationName, lpCommandLine, NULL, NULL)) {
    SetLastError(ERROR_ACCESS_DENIED);
    return false;
  } else {
    BOOL result = CreateProcessANext(lpApplicationName, lpCommandLine,
                                     lpProcessAttributes, lpThreadAttributes,
                                     bInheritHandles, dwCreationFlags,
                                     lpEnvironment, lpCurrentDirectory,
                                     lpStartupInfo, lpProcessInformation);
    // CreateProcess hooks are used very often, so to be sure we renew the hook
    RenewHook((PVOID*) &CreateProcessANext);
    return result;
  }
}

BOOL WINAPI CreateProcessWCallback(LPCWSTR               lpApplicationName,
                                   LPWSTR                lpCommandLine,
                                   LPSECURITY_ATTRIBUTES lpProcessAttributes,
                                   LPSECURITY_ATTRIBUTES lpThreadAttributes,
                                   BOOL                  bInheritHandles,
                                   DWORD                 dwCreationFlags,
                                   LPVOID                lpEnvironment,
                                   LPCWSTR               lpCurrentDirectory,
                                   LPSTARTUPINFO         lpStartupInfo,
                                   LPPROCESS_INFORMATION lpProcessInformation)
{
  if (!IsAllowed(NULL, NULL, lpApplicationName, lpCommandLine)) {
    SetLastError(ERROR_ACCESS_DENIED);
    return false;
  } else {
    BOOL result = CreateProcessWNext(lpApplicationName, lpCommandLine,
                                     lpProcessAttributes, lpThreadAttributes,
                                     bInheritHandles, dwCreationFlags,
                                     lpEnvironment, lpCurrentDirectory,
                                     lpStartupInfo, lpProcessInformation);
    // CreateProcess hooks are used very often, so to be sure we renew the hook
    RenewHook((PVOID*) &CreateProcessWNext);
    return result;
  }
}

UINT WINAPI WinExecCallback(LPCSTR lpCmdLine, UINT uCmdShow)
{
  if (!IsAllowed(NULL, (LPTSTR) lpCmdLine, NULL, NULL))
    return ERROR_ACCESS_DENIED;
  else {
    DWORD result = WinExecNext(lpCmdLine, uCmdShow);
    RenewHook((PVOID*) &WinExecNext);
    return result;
  }
}

// ***************************************************************

BOOL WINAPI DllMain(HANDLE hModule, DWORD fdwReason, LPVOID lpReserved)
{
  if (fdwReason == DLL_PROCESS_ATTACH) {
    // InitializeMadCHook is needed only if you're using the static madCHook.lib
    InitializeMadCHook();

    HookAPI("kernel32.dll", "CreateProcessA", CreateProcessACallback, (PVOID*) &CreateProcessANext);
    HookAPI("kernel32.dll", "CreateProcessW", CreateProcessWCallback, (PVOID*) &CreateProcessWNext);
    HookAPI("kernel32.dll",        "WinExec",        WinExecCallback, (PVOID*)        &WinExecNext);

  } else if (fdwReason == DLL_PROCESS_DETACH)
    // FinalizeMadCHook is needed only if you're using the static madCHook.lib
    FinalizeMadCHook();

  return true;
}
