#ifndef DRVCOMM_H
  #define DRVCOMM_H

#include "ntddk.h"
#include "kbdmou.h"
#include <ntstrsafe.h>
#include <ntddkbd.h>

//Settings
#define DRV_ENABLED  (ULONG)0x00000001
#define CTL_ALT_DEL  (ULONG)0x00000002
#define ALT_TAB      (ULONG)0x00000004
#define CTL_ESC      (ULONG)0x00000008
#define WINDOWS_LOGO (ULONG)0x00000010
#define WINDOWS_MENU (ULONG)0x00000020
#define SHIFT_F10    (ULONG)0x00000040
#define CTL_ALT_F12  (ULONG)0x00000080
#define CAN_ESCAPE   (ULONG)0x00000100
#define ALT_ESC      (ULONG)0x00000200
#define F8           (ULONG)0x00000400
#define CTRL_P       (ULONG)0x00000800
#define ALT_F4       (ULONG)0x00001000
#define CTRL_N       (ULONG)0x00002000

//Scan Codes for US 104 keyboard
#define ESC_KEY      (USHORT)0x0001
#define TAB_KEY      (USHORT)0x000F
#define P_KEY        (USHORT)0x0019
#define LCTRL        (USHORT)0x001D
#define RCTRL        (USHORT)0xE01D
#define LALT         (USHORT)0x0038
#define RALT         (USHORT)0xE038
#define KDEL         (USHORT)0x0053
#define DEL          (USHORT)0xE053
#define LSHIFT       (USHORT)0x002A
#define RSHIFT       (USHORT)0x0036
#define F4_KEY       (USHORT)0x003E
#define F8_KEY       (USHORT)0x0042
#define F10_KEY      (USHORT)0x0044
#define F12_KEY      (USHORT)0x0058
#define LWIN_KEY     (USHORT)0x005B
#define RWIN_KEY     (USHORT)0x005C
#define MENU_KEY     (USHORT)0x005D
#define N_KEY        (USHORT)0x0031

#if DBG

#define TRAP()                      DbgBreakPoint()

#define DebugPrint(_x_) DbgPrint _x_

#else   // DBG

#define TRAP()

#define DebugPrint(_x_) 

#endif

typedef struct _NS_FILTER_SETTINGS
{
    ULONG  DriverEnabled;
    ULONG  Ctl_Alt_Del;
    ULONG  Alt_Tab;
    ULONG  Ctl_Esc;
    ULONG  Windows_Logo;
    ULONG  Windows_Menu;
    ULONG  Shift_F10;
    ULONG  Ctl_Alt_F12;
    ULONG  Alt_Esc;
    ULONG  DisableF8;
    ULONG  Ctl_P;
    ULONG  Alt_F4;
    ULONG  EscapeCodeEnabled;
    USHORT EscCode1;
    USHORT EscCode2;
    USHORT EscCode3;
    USHORT EscCode4;
	ULONG  Ctl_N;
} NS_FILTER_SETTINGS, *PNS_FILTER_SETTINGS;


//Globals
CHAR err[250];
NS_FILTER_SETTINGS MySettings;


NTSTATUS
GetDriverSettings(IN PNS_FILTER_SETTINGS FilterSettings);

/*
VOID CheckKeys(IN PKEYBOARD_INPUT_DATA InputDataStart,
               IN PKEYBOARD_INPUT_DATA InputDataEnd);
*/

VOID CheckKeys2(IN PKEYBOARD_INPUT_DATA InputDataStart,
               IN PKEYBOARD_INPUT_DATA InputDataEnd);

#endif