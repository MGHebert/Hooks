// ***************************************************************
//  DllInjector               version: 1.1.0  ·  date: 2010-01-10
//  -------------------------------------------------------------
//  tool to inject/uninject dlls system wide or user wide
//  -------------------------------------------------------------
//  Copyright (C) 1999 - 2010 www.madshi.net, All Rights Reserved
// ***************************************************************

// 2010-01-10 1.1.0 adjustments for madCodeHook 3
// 2003-06-15 1.0   initial release

// ***************************************************************

#include <windows.h>
#include ".\\CodeHook\\madCHook.h"
#include "Utilities.h"

// ***************************************************************

WCHAR dllFileW [MAX_PATH + 1];

void InitDllFile()
// dllFile is set to "ourExePath\*.dll"
{
  GetModuleFileNameW(NULL, dllFileW, MAX_PATH);
  for (int i1 = lstrlenW(dllFileW) - 1; i1 > 0; i1--)
    if (dllFileW[i1] == L'\\') {
      dllFileW[i1 + 1] = 0;
      break;
    }
  lstrcatW(dllFileW, L"*.dll");
}

BOOL GetDllFile(HWND hParentWindow)
// let the user choose a dll file, the result is stored in "dllFile"
{
  OPENFILENAMEW ofnW;

  ZeroMemory(&ofnW, sizeof(ofnW));
  ofnW.lStructSize  = sizeof(ofnW);
  ofnW.hwndOwner    = hParentWindow;
  ofnW.lpstrFilter  = L"system wide hook dll\0*.dll";
  ofnW.nFilterIndex = 1;
  ofnW.lpstrFile    = dllFileW;
  ofnW.nMaxFile     = MAX_PATH;
  ofnW.lpstrTitle   = L"Choose system wide hook dll";
  ofnW.Flags        = OFN_NOREADONLYRETURN | OFN_HIDEREADONLY;
  return GetOpenFileNameW(&ofnW) != false;
}

// ***************************************************************

// here the window handles for our 4 controls are stored
HWND hSystemWide, hUserWide, hInject, hUninject;

LRESULT WINAPI MainBoxWndProc(HWND window, DWORD msg, INT wParam, INT lParam)
// this is our main box' window proc, quite easy actually
{
  BOOL b1;

  if (msg == WM_COMMAND) {
    if ((HWND) lParam == hInject) {
      // the user pressed on the inject button
      if (GetDllFile(window)) {
        if (SendMessage(hSystemWide, BM_GETCHECK, 0, 0) == BST_CHECKED)
          b1 = InjectLibraryW(L"HookProcessCreationDemoDriver", dllFileW, ALL_SESSIONS, true);
        else
          b1 = InjectLibraryW(L"HookProcessCreationDemoDriver", dllFileW, CURRENT_SESSION, false);
        if (!b1)
          MessageBox(0, "injection failed", "error", 0);
      }
      SetFocus(hInject);
    } else if ((HWND) lParam == hUninject) {
      // the user pressed on the uninject button
      if (GetDllFile(window)) {
        if (SendMessage(hSystemWide, BM_GETCHECK, 0, 0) == BST_CHECKED)
          b1 = UninjectLibraryW(L"HookProcessCreationDemoDriver", dllFileW, ALL_SESSIONS, true);
        else
          b1 = UninjectLibraryW(L"HookProcessCreationDemoDriver", dllFileW, CURRENT_SESSION, false);
        if (!b1)
          MessageBox(0, "uninjection failed", "error", 0);
      }
      SetFocus(hUninject);
    } else if (((HWND) lParam != hSystemWide) && ((HWND) lParam != hUserWide))
      DestroyWindow(window);  // escape was pressed
    return 0;
  } else 
    return DefWindowProc(window, msg, wParam, lParam);
}

void ShowMainWindow()
// show our little info box, nothing special here
{
  WNDCLASS wndClass;
  HWND mainBox;
  HFONT font;
  MSG msg;
  RECT r1;

  // first let's register our window class
  ZeroMemory(&wndClass, sizeof(WNDCLASS));
  wndClass.lpfnWndProc   = (WNDPROC) &MainBoxWndProc;
  wndClass.hInstance     = GetModuleHandle(NULL);
  wndClass.hbrBackground = (HBRUSH) (COLOR_BTNFACE + 1);
  wndClass.lpszClassName = "DllInjectorWindow";
  wndClass.hCursor       = LoadCursor(0, IDC_ARROW);
  RegisterClass(&wndClass);
  // next we create our window
  r1.left   = 0;
  r1.top    = 0;
  r1.right  = 199;
  r1.bottom = 92;
  AdjustWindowRectEx(&r1, WS_CAPTION | WS_SYSMENU, false, WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME);
  r1.right  = r1.right  - r1.left;
  r1.bottom = r1.bottom - r1.top;
  r1.left   = (GetSystemMetrics(SM_CXSCREEN) - r1.right ) / 2;
  r1.top    = (GetSystemMetrics(SM_CYSCREEN) - r1.bottom) / 2;
  mainBox = CreateWindowEx(WS_EX_WINDOWEDGE | WS_EX_DLGMODALFRAME,
                           wndClass.lpszClassName, "Dll Injector...",
                           WS_CAPTION | WS_SYSMENU, 
                           r1.left, r1.top, r1.right, r1.bottom, 0, 0, GetModuleHandle(NULL), NULL);
  // now we create the controls
  hSystemWide = CreateWindow("Button", "system wide",
                             WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON | WS_TABSTOP,
                             16, 12, 80, 28, mainBox, 0, GetModuleHandle(NULL), NULL);
  hUserWide   = CreateWindow("Button", "user wide",
                             WS_CHILD | WS_VISIBLE | BS_AUTORADIOBUTTON,
                             110, 12, 80, 28, mainBox, 0, GetModuleHandle(NULL), NULL);
  hInject     = CreateWindow("Button", "inject dll",
                             WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON | WS_TABSTOP,
                             14, 48, 80, 28, mainBox, 0, GetModuleHandle(NULL), NULL);
  hUninject   = CreateWindow("Button", "uninject dll",
                             WS_CHILD | WS_VISIBLE | WS_TABSTOP,
                             104, 48, 80, 28, mainBox, 0, GetModuleHandle(NULL), NULL);
  // ... and initialize them
  SendMessage(hSystemWide, BM_SETCHECK, BST_CHECKED, 0);
  if (GetVersion() & 0x80000000) {
    EnableWindow(hSystemWide, false);
    EnableWindow(  hUserWide, false);
  }
  SetFocus(hInject);
  // the controls need a nice font
  font = CreateFont(-12, 0, 0, 0, 400, 0, 0, 0, DEFAULT_CHARSET,
                    OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
                    DEFAULT_PITCH | FF_DONTCARE, "MS Sans Serif");
  SendMessage(hSystemWide, WM_SETFONT, (UINT) font, 0);
  SendMessage(hUserWide,   WM_SETFONT, (UINT) font, 0);
  SendMessage(hInject,     WM_SETFONT, (UINT) font, 0);
  SendMessage(hUninject,   WM_SETFONT, (UINT) font, 0);
  // finally show our window
  ShowWindow(mainBox, SW_SHOWNORMAL);
  while ((IsWindow(mainBox)) && (GetMessage(&msg, 0, 0, 0)))
    if (!IsDialogMessage(mainBox, &msg)) {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
    }
  // let's Windows clean up the font etc for us
}

// ***************************************************************

int WINAPI WinMain(HINSTANCE,
                   HINSTANCE,
                   LPSTR,
                   int)
{
	
	
	// InitializeMadCHook is needed only if you're using the static madCHook.lib
	InitializeMadCHook();

	CUtilities::Dlog("Attempting to Intialize Code Hook");

	if (!LoadInjectionDriver(L"HookProcessCreationDemoDriver", L"DemoDriver32.sys", L"DemoDriver64.sys"))
        MessageBoxA(0, "loading driver failed", "error", 0);

    InitDllFile();
    ShowMainWindow();

    // this will only work if no injection is active, anymore
    if (!StopInjectionDriver(L"HookProcessCreationDemoDriver"))
        MessageBoxA(0, "stopping driver failed", "error", 0);

    // FinalizeMadCHook is needed only if you're using the static madCHook.lib
    FinalizeMadCHook();

  return true;
}
