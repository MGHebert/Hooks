
#pragma once

#include <windows.h>

#pragma pack(push, r1, 1)

#define VERSION_NUMBER TEXT("5.0")
#define NETSTOP_BASE_KEY TEXT("Software\\Moonrise Systems Inc\\NetStopPro")
#define NETSTOP_REG_KEY TEXT("Software\\Moonrise Systems Inc\\NetStopPro\\5.0")
#define SERVICE_PRINTER_REG_KEY NETSTOP_REG_KEY;
#define SERVICE_PRINTER_IPC_REG_KEY TEXT("Software\\Moonrise Systems Inc\\NetStopPro\\5.0\\PrinterIPC")

enum PrintAPI {paCreateDC, paStartDoc, paEndDoc, paStartPage, paEndPage, paNoActivity};

struct PrintNotification
{
    PrintAPI printAPI;
    char printerName[MAX_PATH];
    unsigned int numberOfPagesPrinted;
    unsigned int numberOfPagesRejected;
};

struct PrintControl
{
    int pagesAllowed;
    int printingReceipt;
};

struct HookRec
{
    PrintControl printControl;
    PrintNotification printNotification;
};

#pragma pack(pop, r1)
