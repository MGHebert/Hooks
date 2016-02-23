//
//              (C) Moonrise Systems Inc 1999
//              ALL RIGHTS RESERVED.
//__________________________________________________________________________
// Unit File  : Constants.PAS
// Authors    : Yves Mailhot
//__________________________________________________________________________
//
// Description:
//
//     This unit contains constants used in other units.
//__________________________________________________________________________

unit Constants;
{$WRITEABLECONST ON}
interface

uses
  Graphics, WinSpool, Windows;

var
  vFullVersionInfo : String = '5.0'; // '4.2.2.0' [1174: Princess 3.97->5.0 Upgrade];

const
  VERSION_NUMBER = '5.0';
  CrLF = ^M^J;

  // Monitoring Server
  cMonitoringServer = 'https://www.moonrisesystems.com/cgi-bin/ddb35/';
  TIFPath = 'C:\Progra~1\Netstoppro\FaxDocuments\';
  // Release number and version number for Netstop.
  {$IFDEF PRINCESSCUST}   // BEGIN: [1174: Princess 3.97->5.0 Upgrade];
  cMajorVersion: Integer   = 5;
  cMinorVersion: Integer   = 0; 
  cRelease: Integer        = 0;
  cReleaseComment: String  = ' (Princess)'; // Official release build number
  cBuildNo: integer        = 0;
  {$ELSE}                // END: [1174: Princess 3.97->5.0 Upgrade];
  cMajorVersion: Integer   = 5;
  cMinorVersion: Integer   = 0;
  cRelease: Integer        = 0;
  cReleaseComment: String  = ' (Retail)'; // Official release build number
  cBuildNo: integer        = 0;
  {$ENDIF}

 (* [1174: Princess 3.97->5.0 Upgrade];
  I don't think this section of code in "Shellpro" is used any more
  // Release number and version number for MMP.
  cMMPMajorVersion: Integer = 2;
  cMMPMinorVersion: Integer = 2;
  cMMPRelease: Integer = 0;
  cMMPReleaseComment: String = ''; *)

  // Constants used for encryption and security.
  cBfInitString: String = 'wkqehqwoiefbnoicuh2egorgf23or8fiygbkwcnboerigrfoigfoew';
   cBfMACInitString: String = 'lkmvlkdfgoij35t890u54oijbsglkmbfdvrhereljkdsfg098e5t908';
  // Heddier
  //cBfMACInitString: String = '7wd78he88bcbxcw37gml999j031062';

  // surftimer
//  cBfMACInitString: String = 'surfdb';

  cBfIVString1: String = 'jfwi59dlgp[uj-760439dlfvph-uy-6pfdfoflkhiy3h0wrtherther54dsdflkg';
  cBfIVString2: String = 'kskritivmcis845799489t969g90k9c90l0x0oskrp[pgkfvje493';
  cEcKeyPublic: String = 'VaTgWoiSqf5MV6iaslRhPkNx9ytF8Ttc3GpwXabNUY++';
  cSignature: String = 'qwerf34grf3p;itfj305tejrogeorg34ewrgewrgwergewrdu6';
  cBfMACRegistration: String = 'iuwqeou324ljkmq0hdityjhgfo89645jkjrrrjou4r897kjlfer         ';

 //  IP SOLUTION
  SingleFaxFileName = 'SendNetStopSingleFax.XML';  // XML file to submit the fax to the "protus" web site
  GetPageCountName = 'SingleFaxGetPageCount.XML';  // XML file to send the pages fax and receiv the page count.
  EncodeType='base64';                             // protus is case sensitive so it has to be lower case
  DocContentType='txt';
  EncodedTxtFileName='EncodeTmpFile.Txt';
  ProtusAcountLoginID='218728';         // test account '178894';               prodcution a/c: 218728
  ProtusAccountPassword='logix1';       // test account password 'kl123456';                  : logix1 
  SchemaVersion='1.1';
  TmpCvrPgName='TmpCvrpg.txt';     // the contents of the cover page needs to written to file and from that file it is appended to the main fax
  BillCodeForPGCount='PageCountCheck';                               // flag to inform "protus" we are going to check the page count
  FaxNumUsedWhenPollingForPageCount = '15554445555';       // When we poll "protus" web site to get the page count, we need to build this "fixed" number in the "XML document"
  ProtusURLToConnect_SendSingleFax = 'https://www.protusfax.com/protus/xmlwebservices/xmlsubmitter/messaging.asmx/SendSingleFax';         // the new location of the web-service for SendSingleFax
  ProtusURLToConnect_GetFaxDetails = 'https://www.protusfax.com/protus/xmlwebservices/xmlreports/statusreports.asmx/GetDetailFaxStatus';   // to get the details about the fax
//END IP SOLUTION


  // Constants used to access ISAPI DLLs.
  cISAPILocationPrimary: String = 'http://www.moonrisesystems.com/cgi-bin/ddb35/';
  cISAPILocationSecondary: String = 'http://www.moonrisesystems.com/cgi-bin/ddb35/';
//heddier
//  cISAPILocationPrimary: String = 'http://wfpro-access1.de/cgi-bin/version35/';
//  cISAPILocationSecondary: String = 'http://wfpro-access2.de/cgi-bin/version35/';

// surftimer
//  cISAPILocationPrimary: String = 'http://db.surf-timer.com/cgi-bin/';
//  cISAPILocationSecondary: String = 'http://db.surf-timer.com/cgi-bin/';

  cKeyboardShowHideButtons: array[0..1] of TColor = ($000DD6F2, $000DD6F2);
  cKeyboardShowHideButtonsMouseOut: array[0..1] of TColor = (clBlack, clBlack);
  cKeyboardShowHideButtonsMouseIn: array[0..1] of TColor = (clRed, clRed);

  // Volume
  cVolumeLevels: array[0..8] of {integer} Int64 = ( $00000000
                                            ,$22222222
                                            ,$44444444
                                            ,$66666666
                                            ,$88888888
                                            ,$AAAAAAAA
                                            ,$CCCCCCCC
                                            ,$EEEEEEEE
                                            ,$FFFFFFFF);

  cApplicationButtonVerticalSeparation: Integer = 63;

  cNumberOfLanguages: Integer = 8;

  winetdll = 'wininet.dll';

  MAX_PATH = 255;

  MyRootKey = HKEY_LOCAL_MACHINE;

//  MyRootKey = HKEY_CURRENT_USER;

  NETSTOP_BASE_KEY  = 'Software\Moonrise Systems Inc\NetStopPro';
  NETSTOP_REG_KEY = NETSTOP_BASE_KEY + '\' + VERSION_NUMBER;
  NETSTOP_MACHINE_KEY = NETSTOP_BASE_KEY + '\Machine\' +  VERSION_NUMBER;
  ACCPRO_REG_KEY = 'Software\Moonrise Systems Inc\AccountPro\' + VERSION_NUMBER;

implementation

{$WRITEABLECONST OFF}
end.








