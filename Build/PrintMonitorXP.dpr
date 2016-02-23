//
//              (C) Moonrise Systems Inc 1999
//              ALL RIGHTS RESERVED.
//__________________________________________________________________________
// Unit File  : PrintMonitor.DPR
// Author     : Yves Mailhot
//
// Description: This DLL is the implementation of a systems wide hook
//              to count the number of pages printed.
//__________________________________________________________________________
//

library PrintMonitorXP;

{$IMAGEBASE $5a000000}

uses
  Windows,
  madCodeHook,
  SysUtils,
  madStrings,
  PrinterIPC,
  CommonPrintMonitor;

// ACME PRINTER ISSUE CHANGE, USED TO READ NUMBER OF COPIES FROM REGISTRY

  // There are 4 pieces of information we keep track of:
  //   How many pages the main application allows us to print
  //   The number of copies to be printed
  //   The number of pages printed
  //   The number of pages rejected so that the application can tell the user.

{$R *.RES}

var
  CreateDCANext : function (driver, device, output: pchar; dm: PDeviceModeA) : dword; stdcall;
  CreateDCWNext : function (driver, device, output: pwidechar; dm: PDeviceModeW) : dword; stdcall;
  StartDocANext : function (dc: dword; const di: TDocInfoA) : integer; stdcall;
  StartDocWNext : function (dc: dword; const di: TDocInfoW) : integer; stdcall;
  EndDocNext    : function (dc: dword) : integer; stdcall;
  StartPageNext : function (dc: dword) : integer; stdcall;
  EndPageNext   : function (dc: dword) : integer; stdcall;
  //FMap: THookRecordFile;
  FMap: TPrinterIPC;
  FPagesPrinted, FCopies, FPagesRejected: DWORD;
  FStartPage, FRejectingPages: Boolean;

procedure SetNumberOfCopies(const AValue: Integer);
begin
  FCopies := AValue;
  //SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'NumberOfCopies', AValue);
end;

function GetNumberOfCopies: Integer;
begin
  Result := FCopies;
  //Result := GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'NumberOfCopies');
end;

procedure SetNumberOfPagesPrinted(const AValue: DWORD);
begin
  FPagesPrinted := AValue;
  //SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY { + '\' + IntToStr(GetCurrentProcess)}, 'NumberOfPagesPrinted', AValue);
end;

function GetNumberOfPagesPrinted: DWORD;
begin
  Result := FPagesPrinted;
  //Result := GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'NumberOfPagesPrinted');
end;

procedure SetNumberOfPagesRejected(const AValue: DWORD);
begin
  FPagesRejected := AValue;
  //SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'NumberOfPagesRejected', AValue);
end;

function GetNumberOfPagesRejected: DWORD;
begin
  Result := FPagesRejected;
  //Result := GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'NumberOfPagesRejected');
end;

procedure SetRejectingPages(const AValue: Boolean);
begin
  FRejectingPages := AValue;
//  if AValue then
//    SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'RejectingPages', 1)
//  else
//    SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'RejectingPages', 0);
end;

function GetRejectingPages: Boolean;
begin
  Result := FRejectingPages;
//  Result := (0 <> GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'RejectingPages'));
end;

procedure SetStartPage(const AValue: Boolean);
begin
  FStartPage := AValue;
//  if AValue then
//    SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'StartPage', 1)
//  else
//    SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'StartPage', 0);
end;

function GetStartPage: Boolean;
begin
  Result := FStartPage;
//  Result := (0 <> GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY{ + '\' + IntToStr(GetCurrentProcess)}, 'StartPage'));
end;

function GetLogFileDir: String;
begin
  Result := ExtractFileDir(GetRegistryString(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_REG_KEY, 'Log File - Log File Location'));
end;

function DebuggingEnabled: Boolean;
begin
  Result := (0 <> GetRegistryInt(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_IPC_REG_KEY, 'PrintMonitorLog', 0));
end;

function OneLogPerProcess:Boolean;
begin
  Result := (0 <> GetRegistryInt(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_IPC_REG_KEY, 'PerProcess', 0));
end;

function MonitoredProcess: String;
begin
  Result := GetRegistryString(HKEY_LOCAL_MACHINE, SERVICE_PRINTER_IPC_REG_KEY, 'MonitoredProcess');
end;

function ClearLog(ALogString: String):Boolean;
var
  LFileName : String;
  LProcess  : String;
  LOneLogPerProcess: Boolean;
begin
  try
    if not DebuggingEnabled then
      EXIT;

    LProcess := MonitoredProcess;
    LOneLogPerProcess := OneLogPerProcess;

    if LOneLogPerProcess and ('' <> Trim(LProcess)) and (Pos(UpperCase(LProcess), UpperCase(ParamStr(0)) ) = 0) then
      EXIT;

    if LOneLogPerProcess then
      LFileName := IncludeTrailingPathDelimiter(GetLogFileDir) + 'printmon' + ChangeFileExt(ExtractFileName(ParamStr(0)), '.log')
    else
      LFileName := IncludeTrailingPathDelimiter(GetLogFileDir) + 'printmon.log';

    if FileExists(LFileName) then
      DeleteFile(LFileName);
  except
    //Debug only. App should still work if this does not
  end;
end;

function Dlog(ALogString: String):Boolean;
var
  LFileName : String;
  f         : TextFile;
  LText     : String;
  LProcess  : String;
  LOneLogPerProcess: Boolean;
begin
  if not DebuggingEnabled then
    EXIT;

  LProcess := MonitoredProcess;
  LOneLogPerProcess := OneLogPerProcess;

  if LOneLogPerProcess and ('' <> Trim(LProcess)) and (Pos(UpperCase(LProcess), UpperCase(ParamStr(0)) ) = 0) then
    EXIT;

  LText := FormatDateTime('YYYY-MM-DD hh:nn:ss.zzz', Now) + ' ' + Format('0x%.4x/0x%.4x ', [GetCurrentProcessID, GetCurrentThreadID]) + ' ' + ALogString;

  if LOneLogPerProcess then
    LFileName := IncludeTrailingPathDelimiter(GetLogFileDir) + 'printmon' + ChangeFileExt(ExtractFileName(ParamStr(0)), '.log')
  else
    LFileName := IncludeTrailingPathDelimiter(GetLogFileDir) + 'printmon.log';

  try
    AssignFile(f, LFileName);
    if FileExists(LFileName) then
      Append(f)
    else
      Rewrite(f);
    try
      WriteLn(f, LText);
    finally
      CloseFile(f);
    end;
  except
    //Debug only. App should still work if this does not
  end;
end;

function AllowPage(dc: dword) : Integer;
begin
  Dlog('Allowing Page');
  Result := StartPageNext(dc);
  Dlog('StartPageNext Result = ' + IntToStr(Result));
  if Result <= 0 then
    Dlog('Error Retunred:' + SysErrorMessage(GetLastError));
  SetRejectingPages(FALSE);
end;

function RejectPage(dc: dword) : Integer;
begin
  Dlog('Rejecting Page');
  Result := EndDocNext(dc);
  if Result <= 0 then
    Dlog(SysErrorMessage(GetLastError));
  SetRejectingPages(TRUE);
end;

function ProcessPage(dc: dword) : Integer;
var
  LPageLimit, LAllowed: Integer;
begin
  DLog('ProcessPage - Enter');
  try
    // We check if there is enough balance to allow this page to print,
    // taking into consideration the number of copies requested.
    LPageLimit := GetNumberOfPagesPrinted + GetNumberOfCopies;
    //LAllowed := GetPagesAllowed;
    LAllowed := FMap.PagesAllowed;
    DLog('Pages Allowed = ' + IntToStr(LAllowed));
    DLog('NumberOfPagesPrinted = ' + IntToStr(GetNumberOfPagesPrinted));
    DLog('NumberOfCopies = ' + IntToStr(GetNumberOfCopies));
    if (LAllowed >= 0) and (LAllowed < LPageLimit) then
    begin
      Dlog('Page Limit Exceeded');
      Result := RejectPage(dc);
    end else
    begin
      // Changed the following line on 9/13/2005 to fix problems with host based
      // printers crashing NetStop when the result returned was a raw integer. Now, when
      // pages are rejected, the End Document hook is called.
      // At least 8908790980`5one more page can be printed
      Dlog('Pages Allowed Less Than Limit');
      Result := AllowPage(dc);
    end;
    DLog('ProcessPage - Exit');
  except
    on E:Exception do
    begin
      Dlog('Exception raised in ProcessPage: ' + E.Message);
      raise;
    end;
  end;
end;

// This function returns true if the OS is Windows XP
// this is a workaround for printing in XP
function IsWindowsXP: Boolean; //lenis
var
  vVersionInfo: TOSVersionInfo;
begin
  vVersionInfo.dwOSVersionInfoSize := SizeOf(vVersionInfo);
  GetVersionEx(vVersionInfo);
  Result := (vVersionInfo.dwMinorVersion = 1);
end;

// This is the procedure that will notify the application with the following info:
//     Print API
//     Printer Name
//     Number of Pages Printed
//     Number of Pages Rejected
procedure NotifyApplication(pPrintAPI: TPrintAPI; pPrinterName: PChar);
var
  vPrintNotification: TPrintNotification;  // Record with the info sent to the application
  LReply: TPrintReply;
  LSize: DWORD;
begin
  LSize := 0;
  Dlog('NotifyApplication - Enter');

  vPrintNotification.PrintAPI := pPrintAPI;
  lstrcpyA(vPrintNotification.AppName, PChar(ParamStr(0)));
  lstrcpyA(vPrintNotification.PrinterName, pPrinterName);
  vPrintNotification.NumberOfPagesPrinted := GetNumberOfPagesPrinted;
  vPrintNotification.NumberOfPagesRejected := GetNumberOfPagesRejected;
  vPrintNotification.Copies := GetNumberOfCopies;
  Dlog('    Pages Printed = ' + IntToStr(vPrintNotification.NumberOfPagesPrinted));
  Dlog('    Pages Rejected = ' + IntToStr(vPrintNotification.NumberOfPagesRejected));
  Dlog('    Copies = ' + IntToStr(vPrintNotification.Copies));

  FMap.PrintAPI := pPrintAPI;
  //SetPrintAPI(pPrintAPI);
  FMap.PrinterName := pPrinterName;
  //SetPrinterName(pPrinterName);
  FMap.PagesPrinted := vPrintNotification.NumberOfPagesPrinted;
  //SetPagesPrintedForJob(vPrintNotification.NumberOfPagesPrinted);
  FMap.PagesRejected := vPrintNotification.NumberOfPagesRejected;
  //SetPagesRejectedForJob(vPrintNotification.NumberOfPagesRejected);

  // Now send the composed strings to our log window
  // hopefully there's an instance running in the specified session
  Dlog('    Sending IPC Message to PrintMonitorIPC');
  //SendIpcMessage(pchar('PrintMonitorIPC'), @vPrintNotification, sizeOf(vPrintNotification));
  CallNamedPipe( PChar('\\.\pipe\PrintMonitorPipe'), @vPrintnotification, Sizeof(TPrintNotification), @LReply, Sizeof(TPrintReply), LSize, NMPWAIT_WAIT_FOREVER );
  Dlog('NotifyApplication - Exit');
end;

// This API provides us with the number of copies and the printer name. We
// store and use the number of copies locally and send the printer name to the application
// since different printers have different prices.
function CreateDCACallback(Driver, Device, Output: PChar; DM: PDeviceModeA): dword; stdcall;
var
  vPrinterName: array [0..MAX_PATH] of Char;
begin
  Dlog('CreateDCACallback - Enter');
  Result := CreateDCANext(Driver, Device, Output, DM);

  //if (Device <> nil) and (not rHookRec^.PrintingReceipt) then
  if (Device <> nil) and (not GetPrintingReceipt) then
  begin
    lstrcpyA(vPrinterName, Device);
    vPrinterName[11] := #0;

    // We don't want to display dcs
    if lstrcmpA('\\.\DISPLAY', vPrinterName) <> 0 then
    begin
      // We notify the application only if it is a printer DC creation
      if (Device <> nil) and (not IsBadReadPtr(Device, 1)) and (Device^ <> #0) then
      begin
        // If the number of copies was set, use the value.
        if (DM <> nil) and (PDeviceModeA(DM)^.dmFields and DM_COPIES > 0) then
        begin
          SetNumberOfCopies(PDeviceModeA(DM)^.dmCopies);
          Dlog('    Set Number of Copies = ' + IntToStr(PDeviceModeA(DM)^.dmCopies));
        end else
          SetNumberOfCopies(0);

        lstrcpyA(vPrinterName, Device);
        Dlog('    Sending paCreateDC notification to application, Printer = ' + String(vPrinterName));
        NotifyApplication(paCreateDC, vPrinterName);

        //Clear counters -  Lenis
        //  cNumberOfPagesPrinted  := 0;
        //  cNumberOfPagesRejected := 0;

        //if IsWindowsXP then
        //  cNumberOfCopies := 0;
      end;
    end;
  end;
  Dlog('CreateDCACallback - Exit');
end;

function CreateDCWCallback(Driver, Device, Output: PWideChar; DM: PDeviceModeW) : dword; stdcall;
var
  vTempArrayCharW: array [0..MAX_PATH] of WideChar;
  vPrinterName: array [0..MAX_PATH] of Char;
begin
  Dlog('CreateDCWCallback - Enter');
  Result := CreateDCWNext(Driver, Device, Output, DM);

  //if (Device <> nil) and (not rHookRec^.PrintingReceipt) then
  if (Device <> nil) and (not GetPrintingReceipt) then
  begin
    lstrcpyW(vTempArrayCharW, Device);
    vTempArrayCharW[11] := #0;

    // We don't want to display dcs
    if lstrcmpW('\\.\DISPLAY', vTempArrayCharW) <> 0 then
    begin
      // We notify the application only if it is a printer DC creation
      if (Device <> nil) and (not IsBadReadPtr(Device, 2)) and (Device^ <> #0) then
      begin
        // If the number of copies was set, use the value.
        if (DM <> nil) and (PDeviceModeW(DM)^.dmFields and DM_COPIES > 0) then
        begin
          SetNumberOfCopies(PDeviceModeW(DM)^.dmCopies);
          Dlog('    Set Number of Copies = ' + IntToStr(PDeviceModeA(DM)^.dmCopies));
        end else
          SetNumberOfCopies(0);

        WideToAnsi(Device, vPrinterName);
        Dlog('    Sending paCreateDC notification to application, Printer = ' + String(vPrinterName));
        NotifyApplication(paCreateDC, vPrinterName);

        //Clear counters -  Lenis
        //cNumberOfPagesPrinted  := 0;
        //cNumberOfPagesRejected := 0;
      end;
    end;
  end;
  Dlog('CreateDCWCallback - Exit');
end;

// This API tells us when the document starts printing - we notify the application
// because this is the event that triggers the display of the print dialog.
// No info is passe from it though.
function StartDocACallback(DC: dword; const DI: TDocInfoA): Integer; stdcall;
begin
  Dlog('StartDocACallback - Enter');
  //Clear counters -  Lenis
  SetNumberOfPagesPrinted(0);
  SetNumberOfPagesRejected(0);

  { ACME issue 369 - Commented this code out because we do not want to reset the
  copy counter here!
    cNumberOfCopies := 0;}

  // We notify the application because that's when the Print dialog is displayed
  //if (not rHookRec^.PrintingReceipt) then
  if (not GetPrintingReceipt) then
  begin
    Dlog('    Sending paStartDoc notification');
    NotifyApplication(paStartDoc, nil);
  end;

  Result := StartDocANext(DC, DI);
  Dlog('StartDocACallback - Exit');
end;

function StartDocWCallback(DC: dword; const DI: TDocInfoW): Integer; stdcall;
begin
  Dlog('StartDocWCallback - Enter');
  //Clear counters -  Lenis
  SetNumberOfPagesPrinted(0);
  SetNumberOfPagesRejected(0);

  //if (not rHookRec^.PrintingReceipt) then
  if (not GetPrintingReceipt) then
  begin
    // We notify the application because that's when the Print dialog is displayed
    Dlog('    Sending paStartDoc notification');
    NotifyApplication(paStartDoc, nil);
  end;
    
  Result := StartDocWNext(DC, DI);
  Dlog('StartDocWCallback - Exit');
end;

// This API tells us when a page is to be printed. This is where we reject pages.
function StartPageCallback(dc: dword) : integer; stdcall;
begin
  Dlog('StartPageCallback - Enter');
  try
    //Result := StartPageNext(dc);
    //EXIT;
   // BEGIN ACME PRINTER ISSUE JOSH
   if 0 <> GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_REG_KEY, 'Printing - Use Registry Copy Counter') then
     SetNumberOfCopies(GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_REG_KEY, 'Printing - Number of Copies'));
    // END ACME PRINTER ISSUE
    SetStartPage(TRUE);

    //if rHookRec^.PrintingReceipt then
    if GetPrintingReceipt then
      Result := StartPageNext(dc)
    else
    begin
      Result := ProcessPage(dc);
    end;
    Dlog('StartPageCallback - Exit');
  except
    on E:Exception do
    begin
      DLog('Exception in StartPageCallback: ' + E.Message);
      raise;
    end;
  end;
end;

function EndPageCallback(dc: dword) : integer; stdcall;
begin
  Dlog('EndPageCallback - Enter');
  if GetPrintingReceipt then
    Result := EndPageNext(dc)
  else
  begin
    if GetRejectingPages then
    begin
      Dlog('Rejecting Pages, Ending Document');
      // Changed the following line on 9/27/05 to fix an occasional problem
      // in the "Prepaid Minutes & Pages" usage mode where NetStop would
      // not charge for the pages printed.
      Result := EndDocNext(dc); // old code before 4.8.2 Result := 1 (issue no 765)


      // Only one End page allowed per StartPage.
      if GetStartPage then
      begin
        {ACME issue 369 - Commented this code out because it leads to inaccurate
        page counts.
        if IsWindowsXP then
            Inc(cNumberOfPagesRejected); //lenis }
        // If a valid copies value is detected, we use it. Else increment the page count by 1
        if (GetNumberOfCopies > 0) then
          SetNumberOfPagesRejected(GetNumberOfPagesRejected + GetNumberOfCopies)
        else
          SetNumberOfPagesRejected(GetNumberOfPagesRejected+1);
        Dlog('Starting Page, Pages Rejected = ' + IntToStr(GetNumberOfPagesRejected));
      end;
    end
    else
    begin
      Dlog('Not Rejecting Pages, Ending Page');
      Result := EndPageNext(dc);

      // Only one End page allowed per StartPage.
      if GetStartPage then
      begin
        // If a valid copies value is detected, we use it. Else increment the page count by 1
        if (GetNumberOfCopies > 0) then
          SetNumberOfPagesPrinted(GetNumberOfPagesPrinted + GetNumberOfCopies)
        else
          SetNumberOfPagesPrinted(GetNumberOfPagesPrinted + 1);
        Dlog('Ending Page, Pages Rejected = ' + IntToStr(GetNumberOfPagesRejected));
        Dlog('Ending Page, Pages Printed = ' + IntToStr(GetNumberOfPagesPrinted));
      end else
        Dlog('GetStartPage returned FALSE');
    end;
    SetStartPage(FALSE);
  end;
  Dlog('EndPageCallback - Exit');
end;

function EndDocCallback(DC: dword): Integer; stdcall;
begin
  Dlog('EndDocCallback - Enter');
  //if (not rHookRec^.PrintingReceipt) then
  if (not GetPrintingReceipt) then
  begin
    // We notify the application that the document is finished - the application then
    // it knows it is time to charge for the printing (deduct from balance)
    NotifyApplication(paEndDoc, nil);

    // The application has been notified of these pages so we can reset
    SetNumberOfPagesPrinted(0);
    SetNumberOfPagesRejected(0);
    SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_REG_KEY, 'Printing - Number of Copies', GetNumberOfCopies);
    SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_REG_KEY, 'Printing - Use Registry Copy Counter', 0);
    //SetCopies(GetNumberOfCopies,False); // ACME PRINT ISSUE Set Registry Copy Count Flag to False
  end;

  Result := EndDocNext(DC);

  Dlog('EndDocCallback - Exit');
end;

procedure EntryPointProc(Reason: Integer);
begin
  case Reason of
    DLL_PROCESS_ATTACH:
      begin
        {*Dlog('Running Print Monitor Entry Point Procedure');
        Dlog('Hooking APIs');
        HookAPI('gdi32.dll', 'CreateDCA', @CreateDCACallback, @CreateDCANext);
        HookAPI('gdi32.dll', 'CreateDCW', @CreateDCWCallback, @CreateDCWNext);
        HookAPI('gdi32.dll', 'StartDocA', @StartDocACallback, @StartDocANext);
        HookAPI('gdi32.dll', 'StartDocW', @StartDocWCallback, @StartDocWNext);
        HookAPI('gdi32.dll', 'EndDoc',@EndDocCallback,@EndDocNext);
        HookAPI('gdi32.dll', 'StartPage', @StartPageCallback, @StartPageNext);
        HookAPI('gdi32.dll', 'EndPage', @EndPageCallback, @EndPageNext);

        Set8087CW($1337); *}
        try
          //FMap := THookRecordFile.Create('_PrintMonitor', PAGE_READWRITE);
          //FMap.CreateFile;
          FMap := TPrinterIPC.Create;
        except
          on E:Exception do
          begin
            Dlog('Failed to create Map');
            FMap := nil;
          end;
        end;
      end;
    DLL_PROCESS_DETACH:
      begin
        try
          FMap.Free;
          (*UnhookAPI(@CreateDCANext);
          UnhookAPI(@CreateDCWNext);
          UnhookAPI(@StartDocANext);
          UnhookAPI(@StartDocWNext);
          UnhookAPI(@EndDocNext);
          UnhookAPI(@StartPageNext);
          UnhookAPI(@EndPageNext);*)
        except
        end;
      end;
  end;
end;

begin
  Dlog('******************* Print Monitor Loaded ****************');
  Dlog('  Exe = ' + ParamStr(0));
  // Collecting hooks can improve the hook installation performance in win9x
  //CollectHooks;
  Dlog('Hooking APIs');
  HookAPI('gdi32.dll', 'CreateDCA', @CreateDCACallback, @CreateDCANext);
  HookAPI('gdi32.dll', 'CreateDCW', @CreateDCWCallback, @CreateDCWNext);
  HookAPI('gdi32.dll', 'StartDocA', @StartDocACallback, @StartDocANext);
  HookAPI('gdi32.dll', 'StartDocW', @StartDocWCallback, @StartDocWNext);
  HookAPI('gdi32.dll', 'EndDoc',@EndDocCallback,@EndDocNext);
  HookAPI('gdi32.dll', 'StartPage', @StartPageCallback, @StartPageNext);
  HookAPI('gdi32.dll', 'EndPage', @EndPageCallback, @EndPageNext);
  //Set8087CW($1337);
  DllProc := @EntryPointProc;
  EntryPointProc(DLL_PROCESS_ATTACH);
end.
