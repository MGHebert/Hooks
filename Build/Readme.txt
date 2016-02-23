Updateign ShellPro to work with new PrintMonitor.
================================================
================================================


1) Add Drivers to InnoSetup Install Script
==========================================

Add the driver files to the install script:
   KlInjDriver32.sys
   KlInjDriver64.sys

They should be copied byt the InnoSetup install script to the same 
directory (%ProgramFiles%\netStopPro) as the Service exe's.


2) Add Service exes to install scrript
======================================

In the install script replace ShellProServices.exe with the two 
versions of the new service:

  ShellProServices32.exe
  ShellProServices64.exe

These should be copied to the same dir as before.


3) Add new DLLs to install script
=================================

In the install script replace PrintMonitor.dll with the two 
versions of the new dll:

  PrintMonitor32.dll
  PrintMonitor64.dll

These should be copied to the same dir as before. The Service will take care of 
deciding which ones to inject.


4) Update Shell Pro Code
========================

Remove the following functions From ShellPro's main.pas unit:

  function InjectDLL: Boolean;
  function UninjectDLL: Boolean;

They are no longer needed as the service injects the Dll's on startup.


5)  Change Install Config Utiltiy
=================================

Need to alter the SpConfig install utility to pick the correct service exe to install based on OS:

In Common\ShellproServices.pas change the "InstallPrinterService" function to the 
following (NOTE: I am assuming the Is64Bit utiltiy has been added to the Utilities.pas unit from the 
keyboard driver update):


procedure InstallPrinterService;
var
  LService : String;
  sh  : THandle;
  rh  : THandle;
  LError: Integer;
begin
  if Is64Bit then
    LService := IncludeTrailingPathDelimiter(EnvStr('%PROGRAMFILES%')) + 'NetStopPro\ShellProServices64.exe'
  else
    LService := IncludeTrailingPathDelimiter(EnvStr('%PROGRAMFILES%')) + 'NetStopPro\ShellProServices32.exe'

  if not FileExists(LService) then
  begin
    if Is64Bit then
      LService := IncludeTrailingPathDelimiter(ExtractFileDir(ParamStr(0))) + 'ShellProServices64.exe'
    else
      LService := IncludeTrailingPathDelimiter(ExtractFileDir(ParamStr(0))) + 'ShellProServices32.exe'

    if not FileExists(LService) then
      raise Exception.Create('File ' + LService + ' not found');
  end;

  sh := 0;
  rh := 0;

  try
    sh := OpenSCManager(nil, nil, SC_MANAGER_ALL_ACCESS);
    if (0 = sh) then
      raise Exception.Create('Error Opening Service manager in InstallPrinterService:' + SysErrorMessage(GetLastError));

    if (0 <> sh) then
    begin
      rh := CreateService(sh,
                          PChar('NetstopPrintingService'),
                          PChar('Netstop Print Service'),
                          SERVICE_ALL_ACCESS,
                          SERVICE_WIN32_SHARE_PROCESS	or SERVICE_INTERACTIVE_PROCESS,
                          SERVICE_AUTO_START,
                          SERVICE_ERROR_NORMAL,
                          PChar(LService),
                          nil,
                          nil,
                          nil,
                          nil,
                          nil);
      if (0 = rh) then
      begin
        LError := GetLastError;
        if ERROR_SERVICE_EXISTS <> LError then
          raise Exception.Create('Error Creating Service:' + SysErrorMessage(GetLastError));
      end;
    end;
  finally
    if 0 <> sh then
      CloseServiceHandle(sh);
    if 0 <> rh then
      CloseServiceHandle(rh);
  end;
end;

6) Registry Control Settings
============================
HKEY_LOCAL_MACHINE\Software\\Moonrise Systems Inc\\NetStopPro\\5.0\\PrinterIPC

i) PipeControl 
    DWORD 0 = False 
    Non Zero = True  

    Used to determine if Pages allowed and printing receipt come from Pipe or registry map.  Default is false (0)

ii) UseIPC 
    DWORD 0 = False 
    Non Zero = True  

    Instructs Injected DLL to use Madshi IPC Queue to send messages back to shell pro.  

    PipeControl MUST BE 0 FOR THIS TO WORK!!!!!!

iii) Lockable
    DWORD 0 = False 
    Non Zero = True  

    Controls wether critical sections are used to block threads from acessing dll routines simultaneously.

iv) IPCDelay
    DWORD 

    If Non Zero causes a delay of <IPCDelay> milliseconds usign Sleep() API call after IPC Message is sent
    to give receiving app enough time to process it.