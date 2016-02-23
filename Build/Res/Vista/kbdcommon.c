#include "kbdcommon.h"

/*==============================================================================
  Globals
==============================================================================*/
USHORT ctrlState   = KEY_BREAK;
USHORT altState    = KEY_BREAK;
USHORT delState    = KEY_BREAK;
USHORT f4State     = KEY_BREAK;
USHORT f10State    = KEY_BREAK;
USHORT f12State    = KEY_BREAK;
USHORT winState    = KEY_BREAK;
USHORT tabState    = KEY_BREAK;
USHORT escState    = KEY_BREAK;
USHORT shiftState  = KEY_BREAK;
USHORT pState      = KEY_BREAK;
USHORT nState      = KEY_BREAK;
USHORT esc1State   = KEY_BREAK;
USHORT esc2State   = KEY_BREAK;
USHORT esc3State   = KEY_BREAK;
USHORT esc4State   = KEY_BREAK;

NTSTATUS
GetDriverSettings(IN PNS_FILTER_SETTINGS FilterSettings)
{
    //This routine can only be called at IRQL = PASSIVE_LEVEL
    NTSTATUS ntStatus;
    OBJECT_ATTRIBUTES  ObjectAttrib;
    UNICODE_STRING subKey;
    UNICODE_STRING tagName; //Tag for memory allocated from the pool
    ULONG tagNamePointer;  //Pointer to the tag
    UNICODE_STRING DrvSet1;
    HANDLE hCapabilityKeyHandle = 0;

    //strcpy(&err[0], TEXT("No Error"));

    //Initilise FilterSettings
    FilterSettings->DriverEnabled = 0;
    FilterSettings->Ctl_Alt_Del = 0;
    FilterSettings->Alt_Tab = 0;
    FilterSettings->Ctl_Esc = 0;
    FilterSettings->Alt_Esc = 0;
    FilterSettings->Windows_Menu = 0;
    FilterSettings->Windows_Logo = 0;
    FilterSettings->Shift_F10 = 0;
    FilterSettings->Ctl_Alt_F12 = 0;
    FilterSettings->DisableF8 = 0;
    FilterSettings->Ctl_P = 0;
    FilterSettings->Alt_F4 = 0;
    FilterSettings->EscapeCodeEnabled = 0;
    FilterSettings->EscCode1 = 0;
    FilterSettings->EscCode2 = 0;
    FilterSettings->EscCode3 = 0;
    FilterSettings->EscCode4 = 0;
	FilterSettings->Ctl_N = 0;

    RtlInitUnicodeString(&subKey, L"\\Registry\\Machine\\System\\CurrentControlSet\\Services\\NetStopKBFiltr");

	//Create a tag name from when we attempt to allocate memory from the pool
	//Then a assign the address of this tag name to a ULON for use in the
	//ExAllocatePoolWithTag call
    RtlInitUnicodeString(&tagName, L"PoolTag1");
    tagNamePointer = PtrToUlong(&tagName);

    InitializeObjectAttributes(&ObjectAttrib, &subKey, OBJ_KERNEL_HANDLE | OBJ_CASE_INSENSITIVE, NULL, NULL);

    DebugPrint(("    Opening Key"));
    ntStatus = ZwOpenKey(&hCapabilityKeyHandle,
                    KEY_READ | KEY_QUERY_VALUE,
                    &ObjectAttrib);
    /*
    //In Free Build Warnings Are Errors so this needs to be commented out
    if ( STATUS_INVALID_HANDLE == ntStatus ) {
        strcpy(&err[0], TEXT("Opening Key Failed: Invlaid Handle"));
    }

    if ( STATUS_ACCESS_DENIED == ntStatus ) {
        strcpy(&err[0], TEXT("Opening Key Failed: Acess Denied"));
    }

    if ( !NT_SUCCESS(ntStatus) ) {
        sprintf(err, "Opening Key Failed: Error Code 0x%x", ntStatus);
    }
    */
    if ( NT_SUCCESS(ntStatus) )
    {
        OBJECT_ATTRIBUTES objAttribDwordKeyVal;
        UNICODE_STRING subValDword;
        ULONG MyData;
        ULONG MyValueLength = sizeof(KEY_VALUE_PARTIAL_INFORMATION) - 1 + sizeof(ULONG);

        KEY_VALUE_PARTIAL_INFORMATION* MyValue = ExAllocatePoolWithTag( NonPagedPool, MyValueLength, tagNamePointer );

        RtlInitUnicodeString(&DrvSet1,L"DrvSet1");
        ntStatus = ZwQueryValueKey(hCapabilityKeyHandle, &DrvSet1, KeyValuePartialInformation, MyValue, MyValueLength, &MyValueLength);
        MyData = *(PULONG)(MyValue->Data);


       /* if ( (!NT_SUCCESS(ntStatus)) && (STATUS_MORE_ENTRIES != ntStatus) ) {
           //In Free Build Warnings Are Errors so this needs to be commented out
           sprintf(err, "Querying Value Failed: Error Code 0x%x", ntStatus);
        }
        else
        {   */
            if (DRV_ENABLED == (MyData & DRV_ENABLED)) {
                FilterSettings->DriverEnabled = 1;
            }
            if (CTL_ALT_DEL == (MyData & CTL_ALT_DEL)) {
                FilterSettings->Ctl_Alt_Del = 1;
            }
            if (ALT_TAB == (MyData & ALT_TAB)) {
                FilterSettings->Alt_Tab = 1;
            }
            if (CTL_ESC == (MyData & CTL_ESC)) {
                FilterSettings->Ctl_Esc = 1;
            }
            if (WINDOWS_LOGO == (MyData & WINDOWS_LOGO)) {
                FilterSettings->Windows_Logo = 1;
            }
            if (SHIFT_F10 == (MyData & SHIFT_F10)) {
                FilterSettings->Shift_F10 = 1;
            }
            if (MENU_KEY == (MyData & MENU_KEY)) {
                FilterSettings->Windows_Menu = 1;
            }
            if (CTL_ALT_F12 == (MyData & CTL_ALT_F12)) {
                FilterSettings->Ctl_Alt_F12 = 1;
            }
            if (CAN_ESCAPE == (MyData & CAN_ESCAPE)) {
                FilterSettings->EscapeCodeEnabled = 1;
            }
            if (ALT_ESC == (MyData & ALT_ESC)) {
                FilterSettings->Alt_Esc = 1;
            }
            if (F8 == (MyData & F8)) {
                FilterSettings->DisableF8 = 1;
            }
            if (CTRL_P == (MyData & CTRL_P)) {
                FilterSettings->Ctl_P = 1;
            }
			if (CTRL_N == (MyData & CTRL_N)) {
                FilterSettings->Ctl_N = 1;
            }
            if (ALT_F4 == (MyData & ALT_F4)) {
                FilterSettings->Alt_F4 = 1;
            }

        //}

        if (1 == FilterSettings->EscapeCodeEnabled)  //Only pull out values if enabled
        {                                            //otherwise we are wasting time
            RtlInitUnicodeString(&DrvSet1,L"EscCode1");
            ntStatus = ZwQueryValueKey(hCapabilityKeyHandle, &DrvSet1, KeyValuePartialInformation, MyValue, MyValueLength, &MyValueLength);
            MyData = *(PULONG)(MyValue->Data);

            if (NT_SUCCESS(ntStatus)) {
                FilterSettings->EscCode1 = (USHORT)MyData;
            }

            RtlInitUnicodeString(&DrvSet1,L"EscCode2");
            ntStatus = ZwQueryValueKey(hCapabilityKeyHandle, &DrvSet1, KeyValuePartialInformation, MyValue, MyValueLength, &MyValueLength);
            MyData = *(PULONG)(MyValue->Data);

            if (NT_SUCCESS(ntStatus)) {
                FilterSettings->EscCode2 = (USHORT)MyData;
            }

            RtlInitUnicodeString(&DrvSet1,L"EscCode3");
            ntStatus = ZwQueryValueKey(hCapabilityKeyHandle, &DrvSet1, KeyValuePartialInformation, MyValue, MyValueLength, &MyValueLength);
            MyData = *(PULONG)(MyValue->Data);

            if (NT_SUCCESS(ntStatus)) {
                FilterSettings->EscCode3 = (USHORT)MyData;
            }

            RtlInitUnicodeString(&DrvSet1,L"EscCode4");
            ntStatus = ZwQueryValueKey(hCapabilityKeyHandle, &DrvSet1, KeyValuePartialInformation, MyValue, MyValueLength, &MyValueLength);
            MyData = *(PULONG)(MyValue->Data);

            if (NT_SUCCESS(ntStatus)) {
                FilterSettings->EscCode4 = (USHORT)MyData;
            }
        }
        ExFreePool( MyValue );
    }

   return STATUS_SUCCESS;
}

/*
VOID CheckKeys(IN PKEYBOARD_INPUT_DATA InputDataStart,
               IN PKEYBOARD_INPUT_DATA InputDataEnd)
{
    ULONG i;

    DebugPrint(("KbFilter_ServiceCallback routine running\n"));
    DebugPrint(("    Number Of Packets: 0x%x\n", (InputDataEnd - InputDataStart)));
    DebugPrint(("    MySettings.DriverEnabled: 0x%x\n", MySettings.DriverEnabled));
    DebugPrint(("    MySettings.Ctl_Alt_Del: 0x%x\n", MySettings.Ctl_Alt_Del));
    DebugPrint(("    MySettings.Alt_Tab: 0x%x\n", MySettings.Alt_Tab));
    DebugPrint(("    MySettings.Ctl_Esc: 0x%x\n", MySettings.Ctl_Esc));
    DebugPrint(("    MySettings.Alt_Esc: 0x%x\n", MySettings.Alt_Esc));
    DebugPrint(("    MySettings.Windows_Logo: 0x%x\n", MySettings.Windows_Logo));
    DebugPrint(("    MySettings.Shift_F10: 0x%x\n", MySettings.Shift_F10));
    DebugPrint(("    MySettings.Ctl_Alt_F12: 0x%x\n", MySettings.Ctl_Alt_F12));
    DebugPrint(("    MySettings.EscapeCodeEnabled: 0x%x\n", MySettings.EscapeCodeEnabled));
    DebugPrint(("    MySettings.EscCode1: 0x%x\n", MySettings.EscCode1));
    DebugPrint(("    MySettings.EscCode2: 0x%x\n", MySettings.EscCode2));
    DebugPrint(("    MySettings.EscCode3: 0x%x\n", MySettings.EscCode3));
    DebugPrint(("    MySettings.EscCode4: 0x%x\n", MySettings.EscCode4));
    DebugPrint(("    Error: %s\n", err));


    if (1 == MySettings.DriverEnabled)
    {
        for (i = 0; i < (ULONG)(InputDataEnd - InputDataStart); i++)
        {
            if ( 1 == MySettings.Ctl_Alt_Del )
            {
                if ( (LCTRL == (InputDataStart + i)->MakeCode) || (RCTRL == (InputDataStart + i)->MakeCode) )
                {
                    ctrlState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == altState) && (KEY_MAKE == delState))
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    ctrl Key 0x%x", ctrlState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

                if ( (LALT == (InputDataStart + i)->MakeCode) || (RALT == (InputDataStart + i)->MakeCode) )
                {
                    altState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == ctrlState) && (KEY_MAKE == delState))
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    alt Key 0x%x", altState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

                if ( (KDEL == (InputDataStart + i)->MakeCode) || (DEL == (InputDataStart + i)->MakeCode) )
                {
                    delState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == ctrlState) && (KEY_MAKE == altState))
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    del Key 0x%x", delState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }
            } //if ctl_alt_del

            if ( 1 == MySettings.Ctl_Alt_F12 )
            {
                if ( (LCTRL == (InputDataStart + i)->MakeCode) || (RCTRL == (InputDataStart + i)->MakeCode) )
                {
                    ctrlState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == altState) && (KEY_MAKE == f12State))
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    ctrl Key 0x%x", ctrlState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

                if ( (LALT == (InputDataStart + i)->MakeCode) || (RALT == (InputDataStart + i)->MakeCode) )
                {
                    altState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == ctrlState) && (KEY_MAKE == f12State))
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    alt Key 0x%x", altState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

                if (F12_KEY == (InputDataStart + i)->MakeCode)
                {
                    f12State = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == ctrlState) && (KEY_MAKE == altState))
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    f12 Key 0x%x", f12State));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }
            } //if ctl_alt_F12

            if ( 1 == MySettings.Alt_Tab )
            {
                if ( (LALT == (InputDataStart + i)->MakeCode) || (RALT == (InputDataStart + i)->MakeCode) )
                {
                    altState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if (KEY_MAKE == tabState) //check if tab key is already down
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    alt Key 0x%x", altState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

                if (TAB_KEY == (InputDataStart + i)->MakeCode)
                {
                    tabState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if (KEY_MAKE == altState) //check if alt key is already down
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    tab Key 0x%x", tabState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

            }//if alt_tab

            if ( 1 == MySettings.Ctl_Esc )
            {
                if ( (LCTRL == (InputDataStart + i)->MakeCode) || (RCTRL == (InputDataStart + i)->MakeCode) )
                {
                    ctrlState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if (KEY_MAKE == escState) //check if ESC key is already down
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    alt Key 0x%x", altState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

                if (ESC_KEY == (InputDataStart + i)->MakeCode)
                {
                    escState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if (KEY_MAKE == ctrlState) //check if ctrl key is already down
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    esc Key 0x%x", escState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

            }//if ctl_esc

            if ( 1 == MySettings.Alt_Esc )
            {
                if ( (LALT == (InputDataStart + i)->MakeCode) || (RALT == (InputDataStart + i)->MakeCode) )
                {
                    altState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if (KEY_MAKE == escState) //check if esc key is already down
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    alt Key 0x%x", altState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

                if (ESC_KEY == (InputDataStart + i)->MakeCode)
                {
                    escState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if (KEY_MAKE == altState) //check if alt key is already down
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    tab Key 0x%x", tabState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

            }//if alt_esc

            if ( 1 == MySettings.Shift_F10 )
            {
                if ( (LSHIFT == (InputDataStart + i)->MakeCode) || (RSHIFT == (InputDataStart + i)->MakeCode) )
                {
                    shiftState = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if (KEY_MAKE == f10State) //check if F10 key is already down
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    shift Key 0x%x", shiftState));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

                if (F10_KEY == (InputDataStart + i)->MakeCode)
                {
                    f10State = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if (KEY_MAKE == shiftState) //check if ctrl key is already down
                    {
                        (InputDataStart + i)->MakeCode = 0x0000;
                        DebugPrint(("    scan code changed"));
                        continue;
                    }
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    DebugPrint(("    f10 Key 0x%x", f10State));
                    DebugPrint(("    Scan Code 0x%x", (InputDataStart + i)->MakeCode));
                }

            }//if shift_F10

            if ( 1 == MySettings.Windows_Menu )
            {
                if (MENU_KEY == (InputDataStart + i)->MakeCode)
                {
                    (InputDataStart + i)->MakeCode = 0x0000;
                    DebugPrint(("    scan code changed"));
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    continue;
                }

            }//if Windows_Menu

            if ( 1 == MySettings.Windows_Logo )
            {
                if ( (LWIN_KEY == (InputDataStart + i)->MakeCode) || (RWIN_KEY == (InputDataStart + i)->MakeCode) )
                {
                    (InputDataStart + i)->MakeCode = 0x0000;
                    DebugPrint(("    scan code changed"));
                    DebugPrint(("    Flags 0x%x", (InputDataStart + i)->Flags));
                    continue;
                }

            }//if Windows_Menu


            //Check to see if secret key combo is enabled and pressed
            if (1 == MySettings.EscapeCodeEnabled)
            {
                if ((InputDataStart + i)->MakeCode == MySettings.EscCode1)
                {
                    esc1State = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == esc1State) && (KEY_MAKE == esc2State) && (KEY_MAKE == esc3State) && (KEY_MAKE == esc4State))
                    {
                        MySettings.DriverEnabled = 0;
                        DebugPrint(("    Netstop Keyboard Filtering Disabled"));
                        continue;
                    }
                }

                if ((InputDataStart + i)->MakeCode == MySettings.EscCode2)
                {
                    esc2State = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == esc1State) && (KEY_MAKE == esc2State) && (KEY_MAKE == esc3State) && (KEY_MAKE == esc4State))
                    {
                        MySettings.DriverEnabled = 0;
                        DebugPrint(("    Netstop Keyboard Filtering Disabled"));
                        continue;
                    }
                }

                if ((InputDataStart + i)->MakeCode == MySettings.EscCode3)
                {
                    esc3State = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == esc1State) && (KEY_MAKE == esc2State) && (KEY_MAKE == esc3State) && (KEY_MAKE == esc4State))
                    {
                        MySettings.DriverEnabled = 0;
                        DebugPrint(("    Netstop Keyboard Filtering Disabled"));
                        continue;
                    }
                }

                if ((InputDataStart + i)->MakeCode == MySettings.EscCode4)
                {
                    esc4State = (KEY_BREAK & (InputDataStart + i)->Flags);
                    if ((KEY_MAKE == esc1State) && (KEY_MAKE == esc2State) && (KEY_MAKE == esc3State) && (KEY_MAKE == esc4State))
                    {
                        MySettings.DriverEnabled = 0;
                        DebugPrint(("    Netstop Keyboard Filtering Disabled"));
                        continue;
                    }
                }
            } //if Escape Code Enabled
        }// for loop
    } //if enabled

}
*/

VOID CheckKeys2(IN PKEYBOARD_INPUT_DATA InputDataStart,
               IN PKEYBOARD_INPUT_DATA InputDataEnd)
{
    ULONG i;

    DebugPrint(("KbFilter_ServiceCallback routine running\n"));
    DebugPrint(("    Number Of Packets: 0x%x\n", (InputDataEnd - InputDataStart)));
    DebugPrint(("    MySettings.DriverEnabled: 0x%x\n", MySettings.DriverEnabled));
    DebugPrint(("    MySettings.Ctl_Alt_Del: 0x%x\n", MySettings.Ctl_Alt_Del));
    DebugPrint(("    MySettings.Alt_Tab: 0x%x\n", MySettings.Alt_Tab));
    DebugPrint(("    MySettings.Ctl_Esc: 0x%x\n", MySettings.Ctl_Esc));
    DebugPrint(("    MySettings.Alt_Esc: 0x%x\n", MySettings.Alt_Esc));
    DebugPrint(("    MySettings.Windows_Logo: 0x%x\n", MySettings.Windows_Logo));
    DebugPrint(("    MySettings.Shift_F10: 0x%x\n", MySettings.Shift_F10));
    DebugPrint(("    MySettings.Ctl_Alt_F12: 0x%x\n", MySettings.Ctl_Alt_F12));
    DebugPrint(("    MySettings.DisableF8: 0x%x\n", MySettings.DisableF8));
	DebugPrint(("    MySettings.Ctl_P: 0x%x\n", MySettings.Ctl_P));
	DebugPrint(("    MySettings.Ctl_N: 0x%x\n", MySettings.Ctl_N));
    DebugPrint(("    MySettings.EscapeCodeEnabled: 0x%x\n", MySettings.EscapeCodeEnabled));
    DebugPrint(("    MySettings.EscCode1: 0x%x\n", MySettings.EscCode1));
    DebugPrint(("    MySettings.EscCode2: 0x%x\n", MySettings.EscCode2));
    DebugPrint(("    MySettings.EscCode3: 0x%x\n", MySettings.EscCode3));
    DebugPrint(("    MySettings.EscCode4: 0x%x\n", MySettings.EscCode4));
    DebugPrint(("    Error: %s\n", err));

    /*for (i = 0; i < (ULONG)(InputDataEnd - InputDataStart); i++)
    {
        DebugPrint(("    ScanCode: 0x%x\n", (InputDataStart + i)->MakeCode));
    } */

    if (1 != MySettings.DriverEnabled) {
      DebugPrint(("    Driver Disabled: Exiting\n", err));
      return;
    }

    for (i = 0; i < (ULONG)(InputDataEnd - InputDataStart); i++)
    {
        if ( 1 == MySettings.Windows_Menu )
        {
            if (MENU_KEY == (InputDataStart + i)->MakeCode)
            {
                (InputDataStart + i)->MakeCode = 0x0000;
                DebugPrint(("    Windows Menu Scan Code Changed\n"));
                continue;
            }
        }//if Windows_Menu

        if ( 1 == MySettings.Windows_Logo )
        {
            if ( (LWIN_KEY == (InputDataStart + i)->MakeCode) || (RWIN_KEY == (InputDataStart + i)->MakeCode) )
            {
                (InputDataStart + i)->MakeCode = 0x0000;
                DebugPrint(("    Windows Logo Scan Code Changed\n"));
                continue;
            }
        }//if Windows_Logo

        if ( 1 == MySettings.DisableF8 )
        {
            if (F8_KEY == (InputDataStart + i)->MakeCode)
            {
                (InputDataStart + i)->MakeCode = 0x0000;
                DebugPrint(("    F8 Scan Code Changed\n"));
                continue;
            }
        }//if F8

        if ( (LCTRL == (InputDataStart + i)->MakeCode) || (RCTRL == (InputDataStart + i)->MakeCode) )
        {
            ctrlState = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    Ctrl Key State Changed to 0x%x\n", ctrlState));
        }

        if ( (LALT == (InputDataStart + i)->MakeCode) || (RALT == (InputDataStart + i)->MakeCode) )
        {
            altState = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    Alt Key State Changed to 0x%x\n", altState));
        }

        if ( (KDEL == (InputDataStart + i)->MakeCode) || (DEL == (InputDataStart + i)->MakeCode) )
        {
            delState = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    Del Key State Changed to 0x%x\n", delState));
        }

        if (F12_KEY == (InputDataStart + i)->MakeCode)
        {
            f12State = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    F12 Key State Changed to 0x%x\n", f12State));
        }

        if (ESC_KEY == (InputDataStart + i)->MakeCode)
        {
            escState = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    Esc Key State Changed to 0x%x\n", escState));
        }

        if ( TAB_KEY == (InputDataStart + i)->MakeCode )
        {
            tabState = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    Tab Key State Changed to 0x%x\n", tabState));
        }

        if ( (LSHIFT == (InputDataStart + i)->MakeCode) || (RSHIFT == (InputDataStart + i)->MakeCode) )
        {
            shiftState = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    Shift Key State Changed to 0x%x\n", shiftState));
        }

        if (F10_KEY == (InputDataStart + i)->MakeCode)
        {
            f10State = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    F12 Key State Changed to 0x%x\n", f10State));
        }

        if (P_KEY == (InputDataStart + i)->MakeCode)
        {
            pState = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    P Key State Changed to 0x%x\n", pState));
        }

		if (N_KEY == (InputDataStart + i)->MakeCode)
        {
            pState = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    N Key State Changed to 0x%x\n", pState));
        }

        if (F4_KEY == (InputDataStart + i)->MakeCode)
        {
            f4State = (KEY_BREAK & (InputDataStart + i)->Flags);
            DebugPrint(("    F4 Key State Changed to 0x%x\n", f4State));
        }

        if ( 1 == MySettings.Ctl_Alt_Del )
        {
            if ((KEY_MAKE == altState) && (KEY_MAKE == delState) && (KEY_MAKE == ctrlState))
            {
                if ( ( LCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( RCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( LALT == (InputDataStart + i)->MakeCode ) ||
                     ( RALT == (InputDataStart + i)->MakeCode ) ||
                     ( DEL == (InputDataStart + i)->MakeCode ) ||
                     ( KDEL == (InputDataStart + i)->MakeCode ) )
                {
                    DebugPrint(("    Changing Scan Code 0x%x to 0x0000\n", (InputDataStart + i)->MakeCode));
                    (InputDataStart + i)->MakeCode = 0x0000;
                    continue;
                }
            }
        } //If Ctl-Alt-Del

        if ( 1 == MySettings.Ctl_Alt_F12 )
        {
            if ((KEY_MAKE == altState) && (KEY_MAKE == f12State) && (KEY_MAKE == ctrlState))
            {
                if ( ( LCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( RCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( LALT == (InputDataStart + i)->MakeCode ) ||
                     ( RALT == (InputDataStart + i)->MakeCode ) ||
                     ( F12_KEY == (InputDataStart + i)->MakeCode ) )
                {
                    DebugPrint(("    Changing Scan Code 0x%x to 0x0000\n", (InputDataStart + i)->MakeCode));
                    (InputDataStart + i)->MakeCode = 0x0000;
                    continue;
                }
            }
        } //If Ctl-Alt-F12

        if ( 1 == MySettings.Alt_Tab )
        {
            if ((KEY_MAKE == altState) && (KEY_MAKE == tabState))
            {
                if ( ( LALT == (InputDataStart + i)->MakeCode ) ||
                     ( RALT == (InputDataStart + i)->MakeCode ) ||
                     ( TAB_KEY == (InputDataStart + i)->MakeCode ) )
                {
                    DebugPrint(("    Changing Scan Code 0x%x to 0x0000\n", (InputDataStart + i)->MakeCode));
                    (InputDataStart + i)->MakeCode = 0x0000;
                    continue;
                }
            }
        } //if alt_tab

        if ( 1 == MySettings.Ctl_Esc )
        {
            if ((KEY_MAKE == ctrlState) && (KEY_MAKE == escState))
            {
                if ( ( RCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( RCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( ESC_KEY == (InputDataStart + i)->MakeCode ) )
                {
                    DebugPrint(("    Changing Scan Code 0x%x to 0x0000\n", (InputDataStart + i)->MakeCode));
                    (InputDataStart + i)->MakeCode = 0x0000;
                    continue;
                }
            }
        } //if ctl_esc

        if ( 1 == MySettings.Ctl_P )
        {
            if ((KEY_MAKE == ctrlState) && (KEY_MAKE == pState))
            {
                if ( ( RCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( RCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( P_KEY == (InputDataStart + i)->MakeCode ) )
                {
                    DebugPrint(("    Changing Scan Code 0x%x to 0x0000\n", (InputDataStart + i)->MakeCode));
                    (InputDataStart + i)->MakeCode = 0x0000;
                    continue;
                }
            }
        } //if ctl_p

		if ( 1 == MySettings.Ctl_N )
        {
            if ((KEY_MAKE == ctrlState) && (KEY_MAKE == pState))
            {
                if ( ( RCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( RCTRL == (InputDataStart + i)->MakeCode ) ||
                     ( N_KEY == (InputDataStart + i)->MakeCode ) )
                {
                    DebugPrint(("    Changing Scan Code 0x%x to 0x0000\n", (InputDataStart + i)->MakeCode));
                    (InputDataStart + i)->MakeCode = 0x0000;
                    continue;
                }
            }
        } //if ctl_n

        if ( 1 == MySettings.Alt_Esc )
        {
            if ((KEY_MAKE == altState) && (KEY_MAKE == escState))
            {
                if ( ( RALT == (InputDataStart + i)->MakeCode ) ||
                     ( RALT == (InputDataStart + i)->MakeCode ) ||
                     ( ESC_KEY == (InputDataStart + i)->MakeCode ) )
                {
                    DebugPrint(("    Changing Scan Code 0x%x to 0x0000\n", (InputDataStart + i)->MakeCode));
                    (InputDataStart + i)->MakeCode = 0x0000;
                    continue;
                }
            }
        }//if alt_esc

        if ( 1 == MySettings.Shift_F10 )
        {
            if ((KEY_MAKE == f10State) && (KEY_MAKE == shiftState))
            {
                if ( ( RSHIFT == (InputDataStart + i)->MakeCode ) ||
                     ( RSHIFT == (InputDataStart + i)->MakeCode ) ||
                     ( F10_KEY == (InputDataStart + i)->MakeCode ) )
                {
                    DebugPrint(("    Changing Scan Code 0x%x to 0x0000\n", (InputDataStart + i)->MakeCode));
                    (InputDataStart + i)->MakeCode = 0x0000;
                    continue;
                }
            }
        }//if Shift-F10

        if ( 1 == MySettings.Alt_F4 )
        {
            if ((KEY_MAKE == altState) && (KEY_MAKE == f4State))
            {
                if ( ( LALT == (InputDataStart + i)->MakeCode ) ||
                     ( RALT == (InputDataStart + i)->MakeCode ) ||
                     ( F4_KEY == (InputDataStart + i)->MakeCode ) )
                {
                    DebugPrint(("    Changing Scan Code 0x%x to 0x0000\n", (InputDataStart + i)->MakeCode));
                    (InputDataStart + i)->MakeCode = 0x0000;
                    continue;
                }
            }
        } //if alt_F4

        //Check to see if secret key combo is enabled and pressed
        if (1 == MySettings.EscapeCodeEnabled)
        {
            if ((InputDataStart + i)->MakeCode == MySettings.EscCode1)
            {
                esc1State = (KEY_BREAK & (InputDataStart + i)->Flags);
                if ((KEY_MAKE == esc1State) && (KEY_MAKE == esc2State) && (KEY_MAKE == esc3State) && (KEY_MAKE == esc4State))
                {
                    MySettings.DriverEnabled = 0;
                    DebugPrint(("    Netstop Keyboard Filtering Disabled\n"));
                }
                continue;
            }

            if ((InputDataStart + i)->MakeCode == MySettings.EscCode2)
            {
                esc2State = (KEY_BREAK & (InputDataStart + i)->Flags);
                if ((KEY_MAKE == esc1State) && (KEY_MAKE == esc2State) && (KEY_MAKE == esc3State) && (KEY_MAKE == esc4State))
                {
                    MySettings.DriverEnabled = 0;
                    DebugPrint(("    Netstop Keyboard Filtering Disabled\n"));
                }
                continue;
            }

            if ((InputDataStart + i)->MakeCode == MySettings.EscCode3)
            {
                esc3State = (KEY_BREAK & (InputDataStart + i)->Flags);
                if ((KEY_MAKE == esc1State) && (KEY_MAKE == esc2State) && (KEY_MAKE == esc3State) && (KEY_MAKE == esc4State))
                {
                    MySettings.DriverEnabled = 0;
                    DebugPrint(("    Netstop Keyboard Filtering Disabled\n"));
                }
                continue;
            }

            if ((InputDataStart + i)->MakeCode == MySettings.EscCode4)
            {
                esc4State = (KEY_BREAK & (InputDataStart + i)->Flags);
                if ((KEY_MAKE == esc1State) && (KEY_MAKE == esc2State) && (KEY_MAKE == esc3State) && (KEY_MAKE == esc4State))
                {
                    MySettings.DriverEnabled = 0;
                    DebugPrint(("    Netstop Keyboard Filtering Disabled\n"));
                }
                continue;
            }
        } //if Escape Code Enabled
    }// for loop
}
