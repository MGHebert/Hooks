unit CommonPrintMonitor;

interface

uses
  SysUtils, Windows, Constants;

const
  SERVICE_PRINTER_REG_KEY = NETSTOP_REG_KEY;
  SERVICE_PRINTER_IPC_REG_KEY = NETSTOP_REG_KEY + '\PrinterIPC';

  //Communication Type
  SERVICE_REQUEST   = 0;
  SERVICE_RESPONSE  = 1;

  //Printer Service Commands
  INJECT_DLL         = 1;
  UNINJECT_DLL       = 2;
  REPORT_STATUS      = 3;
  REGISTER_WINDOW    = 4;
  UNREGISTER_WINDOW  = 5;
  CHECK_REGISTRATION = 6;

  //Message Name
  PRINT_MESSAGE = 'CB4D66F8-5B71-422A-9670-5E4B709D5B34';
type

  TPrintAPI = (paCreateDC, paStartDoc, paEndDoc, paStartPage, paEndPage, paNoActivity);

  TPrinterName = array[0..254] of AnsiChar;
  // This is what the printer hook sends
  //Note the Use of CHAR and AnsiChar.
  PPrintNotification = ^TPrintNotification;
  TPrintNotification = packed record
    PrintAPI: TPrintAPI;
    {$IFDEF VER210}
    AppName: array[0..254] of AnsiChar;
    PrinterName: array[0..254] of AnsiChar;
    {$ELSE}
    AppName: array[0..254] of Char;
    PrinterName: array[0..254] of Char;
    {$ENDIF}
    ThreadID: Int64;
    NumberOfPagesPrinted: DWORD;
    NumberOfPagesRejected: DWORD;
    Copies: DWORD;
  end;

  // This is what the printer hook sends
  PPrintReply = ^TPrintReply;
  TPrintReply = packed record
    ErrorCode: DWORD;
  end;

  TPrintControl = packed record
    PagesAllowed: Integer;
    PrintingReceipt: Boolean;
  end;

  PHookRec = ^THookRec;
  THookRec = packed record
    PrintControl: TPrintControl;
    PrintNotification: TPrintNotification;
  end;

  TPrtSvcRequest = packed record       // the Communication Block used in both parts of Printer Service (Server & Client)
  Sender: String[255];   // the sender of the message
  case Command: Integer of
    REGISTER_WINDOW, UNREGISTER_WINDOW: (FormHandle: THandle);
    REPORT_STATUS:();
  end;

  TPrtSvcResponse = packed record
    Sender: String[255];
    Msg: String[255];
    IPCQueueExists: Boolean;
    DLLInjected: Boolean;
    WindowRegistered: Boolean;
  end;

  TPrtSvcMessage = packed record
    case MessageType: Integer of
      SERVICE_REQUEST:(MyRequest: TPrtSvcRequest);
      SERVICE_RESPONSE:(MyResponse: TPrtSvcResponse);
  end;

function MyFileSize(AFileName: String): DWORD;
procedure CreateRegistryKey(const ARootKey: HKEY; const AKeyName: String);
procedure DeleteRegistryKey(const ARootKey: HKEY; const AKeyName: String);
function GetRegistryString(const ARootKey: HKEY; const AKeyName, AValueName: String; ADefault: String = ''): String;
procedure SetRegistryString(const ARootKey: HKEY; const AKeyName, AValueName, AStringValue: String);
function GetRegistryInt(const ARootKey: HKEY; const AKeyName, AValueName: String; ADefault: Integer = 0): Integer;
procedure SetRegistryInt(const ARootKey: HKEY; const AKeyName, AValueName: String; const AIntegerValue: Integer);

procedure CreateIPCKey;
function GetPagesAllowed: Integer;
procedure SetPagesAllowed(const AValue: Integer);
function GetPrintingReceipt: Boolean;
procedure SetPrintingReceipt(const AValue: Boolean);
function GetPrintAPI: TPrintAPI;
procedure SetPrintAPI(const AValue: TPrintAPI);
function GetPrinterName: String;
procedure SetPrinterName(const AValue: String);
function GetPagesPrinted: Integer;
procedure SetPagesPrinted(const AValue: Integer);
function GetPagesPrintedForJob: Integer;
procedure SetPagesPrintedForJob(const AValue: Integer);
function GetPagesRejected: Integer;
procedure SetPagesRejected(const AValue: Integer);
function GetPagesRejectedForJob: Integer;
procedure SetPagesRejectedForJob(const AValue: Integer);
procedure ClearIPC;

implementation

function MyFileSize(AFileName: String): DWORD;
var
  f : THandle;
  d : DWORD;
begin
  f := CreateFile(PChar(AFileName), 0, FILE_SHARE_READ, nil, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
  if f = INVALID_HANDLE_VALUE then
  begin
    Result := 0;
    EXIT;
  end;

  Result := Windows.GetFileSize(f, @d);
  CloseHandle(f);
end;

procedure CreateRegistryKey(const ARootKey: HKEY; const AKeyName: String);
var
  TempKey: HKey;
  Disposition: DWORD;
begin
  if RegCreateKeyEx(ARootKey, PChar(AKeyName), 0, nil, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nil, TempKey, @Disposition) <> ERROR_SUCCESS then
    raise Exception.Create('Could not Create Key ' + AKeyName + ': Windows Error ' + IntToStr(GetLastError));
  RegClosekey(TempKey);
end;

procedure DeleteRegistryKey(const ARootKey: HKEY; const AKeyName: String);
var
  LErr : DWORD;
begin
  LErr := RegDeleteKey(ARootKey, PChar(AKeyName));
  if LErr <> ERROR_SUCCESS then
    raise Exception.Create('Could not Delete Key ' + AKeyName + ': Windows Error ' + IntToStr(GetLastError));
end;

function GetRegistryString(const ARootKey: HKEY; const AKeyName, AValueName: String; ADefault: String = ''): String;
var
  TempKey: HKey;
  buff   : PChar;
  buffsize: DWORD;
  DataType: DWORD;
begin
  DataType := REG_SZ;
  if RegOpenKeyEx(ARootKey, PChar(AKeyName), 0, KEY_READ, TempKey) <> ERROR_SUCCESS then
  begin
    Result := ADefault;
    EXIT;
  end;

  try
    if RegQueryValueEx(TempKey, PChar(AValueName), nil, @DataType, nil, @buffsize) <> ERROR_SUCCESS then
      Result := ADefault
    else
    begin
      GetMem(buff, buffsize);
      try
        if RegQueryValueEx(TempKey, PChar(AValueName), nil, @DataType, PByte(buff), @buffsize) <> ERROR_SUCCESS then
          raise Exception.Create('Could not query value ' + AValueName + ': Windows Error ' + IntToStr(GetLastError));
        Result := String(buff);
      finally
        FreeMem(buff, buffsize);
      end;
    end;
  finally
    RegCloseKey(TempKey);
  end;
end;

procedure SetRegistryString(const ARootKey: HKEY; const AKeyName, AValueName, AStringValue: String);
var
  TempKey: HKey;
  buff   : PChar;
  buffsize: DWORD;
  Disposition: DWORD;
begin
  if RegCreateKeyEx(ARootKey, PChar(AKeyName), 0, nil, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS	, nil, TempKey, @Disposition) <> ERROR_SUCCESS then
    raise Exception.Create('Could not open Key ' + AKeyName + ': Windows Error ' + IntToStr(GetLastError));

  try
    buffsize := Length(AStringValue) + 1;
    GetMem(buff, buffsize);
    try
      StrPCopy(buff, AStringValue);
      if RegSetValueEx(TempKey, PChar(AValueName), 0, REG_SZ, PByte(buff), buffsize) <> ERROR_SUCCESS then
        raise Exception.Create('Could not query value ' + AValueName + ': Windows Error ' + IntToStr(GetLastError));
    finally
      FreeMem(buff, buffsize);
    end;
  finally
    RegCloseKey(TempKey);
  end;
end;

function GetRegistryInt(const ARootKey: HKEY; const AKeyName, AValueName: String; ADefault: Integer = 0): Integer;
var
  TempKey: HKey;
  buff   : Integer;
  buffsize: DWORD;
  DataType: DWORD;
begin
  DataType := REG_DWORD;
  buffsize := 4;
  if RegOpenKeyEx(ARootKey, PChar(AKeyName), 0, KEY_READ, TempKey) <> ERROR_SUCCESS then
  begin
    Result := ADefault;
    EXIT;
  end;

  try
    if RegQueryValueEx(TempKey, PCHar(AValueName), nil, @DataType, PByte(@buff), @buffsize) <> ERROR_SUCCESS then
      Result := ADefault
    else
      Result := buff;
  finally
    RegCloseKey(TempKey);
  end;
end;

procedure SetRegistryInt(const ARootKey: HKEY; const AKeyName, AValueName: String; const AIntegerValue: Integer);
var
  TempKey: HKey;
  buff   : Integer;
  buffsize: DWORD;
  Disposition: DWORD;
begin
  buff := AIntegerValue;
  buffsize := 4;
  if RegCreateKeyEx(ARootKey, PChar(AKeyName), 0, nil, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS	, nil, TempKey, @Disposition) <> ERROR_SUCCESS then
    raise Exception.Create('Could not open Key ' + AKeyName + ': Windows Error ' + IntToStr(GetLastError));

  try
    if RegSetValueEx(TempKey, PCHar(AValueName), 0, REG_DWORD, PByte(@buff), buffsize) <> ERROR_SUCCESS then
      raise Exception.Create('Could not query value ' + AValueName + ': Windows Error ' + IntToStr(GetLastError));
  finally
    RegCloseKey(TempKey);
  end;
end;


procedure CreateIPCKey;
begin
  CreateRegistryKey(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY);
end;

function GetPagesAllowed: Integer;
const
  VALUE_NAME = 'PagesAllowed';
begin
  Result := GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME);
end;

procedure SetPagesAllowed(const AValue: Integer);
const
  VALUE_NAME = 'PagesAllowed';
begin
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME, AValue);
end;

function GetPrintingReceipt: Boolean;
const
  VALUE_NAME = 'PrintingReceipt';
begin
  Result := (0 <> GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME));
end;

procedure SetPrintingReceipt(const AValue: Boolean);
const
  VALUE_NAME = 'PrintingReceipt';
begin
  if not AValue then
    SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME, 0)
  else
    SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME, 1);
end;

function GetPrintAPI: TPrintAPI;
const
  VALUE_NAME = 'PrintAPI';
begin
  Result := TPrintAPI(GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME));
end;

procedure SetPrintAPI(const AValue: TPrintAPI);
const
  VALUE_NAME = 'PrintAPI';
begin
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME, Ord(AValue));
end;

function GetPrinterName: String;
const
  VALUE_NAME = 'PrinterName';
begin
  Result := GetRegistryString(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME);
end;

procedure SetPrinterName(const AValue: String);
const
  VALUE_NAME = 'PrinterName';
begin
  SetRegistryString(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME, AValue);
end;

function GetPagesPrinted: Integer;
const
  VALUE_NAME = 'PagesPrinted';
begin
  Result := GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME);
end;

procedure SetPagesPrinted(const AValue: Integer);
const
  VALUE_NAME = 'PagesPrinted';
begin
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME, AValue);
end;

function GetPagesPrintedForJob: Integer;
const
  VALUE_NAME = 'PagesPrintedForJob';
begin
  Result := GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME);
end;

procedure SetPagesPrintedForJob(const AValue: Integer);
const
  VALUE_NAME = 'PagesPrintedForJob';
begin
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME, AValue);
end;

function GetPagesRejected: Integer;
const
  VALUE_NAME = 'PagesRejected';
begin
  Result := GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME);
end;

procedure SetPagesRejected(const AValue: Integer);
const
  VALUE_NAME = 'PagesRejected';
begin
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME, AValue);
end;

function GetPagesRejectedForJob: Integer;
const
  VALUE_NAME = 'PagesRejectedForJob';
begin
  Result := GetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME);
end;

procedure SetPagesRejectedForJob(const AValue: Integer);
const
  VALUE_NAME = 'PagesRejectedForJob';
begin
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, VALUE_NAME, AValue);
end;

procedure ClearIPC;
begin
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, 'PagesAllowed', 0);
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, 'PrintingReceipt', 0);
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, 'PrintAPI', Ord(paNoActivity));
  SetRegistryString(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, 'PrinterName', '');
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, 'PagesPrinted', 0);
  SetRegistryInt(HKEY_CURRENT_USER, SERVICE_PRINTER_IPC_REG_KEY, 'PagesRejected', 0);
end;

end.
