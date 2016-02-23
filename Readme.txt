
http://www.tavendale.com/PrinterHooks.zip

In order to build this you will need Visual Studio 2008 with Service Pack 1.

Important: VS 2008 does not install the 64 Bit compiler by default. So If you have previously installed it with the default C++ options you will need to uninstall it first.

When installing VS 2008 make sure you select the Custom Install Option.  Go into the Microsoft Visual C++ options and make sure x64 compilers and tools is checked.  Once the install is complete download and apply Service Pack 1 for VS 2008.

Projects In The Process Creation Demo Solution

KioskManagementService
This is the service application.  There are two possible build configurations: Release|Win32 and Release|x64

Print Monitor
This is the injected Dll. There are two possible build configurations for this: Release|Win32 and Release|x64

Both of these projects and all their configurations use statically linked libraries so there is no need to deploy the MSVC runtime with them.

TestApp
This was used to test various concepts. It does not need to be built and deployed for this to work.

There are two other folders important to this solution

CodeHook: Contains pre-built Madshi .lib files which are statically linked into the Service & dll. (these are the same as what is installed into MadCodeHook Components directory when the Madshi Components are installed).

Common: Contains ports of the common files and registry handling code of the print monitor project from Delphi.  Also has logging routines in a static class.  These are C++ functions so you need to use formatting specifiers (%d, %s etc) as routines like IntToStr are not available.

Build: Contains the latest builds of the executables, signed driver files and Dlls.  Also there is a batch file called idcxsign.bat. With some changes (w.r.t. the directories) this can be made to recreate and resign the drivers, DLLs etc.


Delphi:

Pipe Client
This is a test client app written with D2010 (though it should be easily downgradable to Delphi 7). It can act as both a named pipe client and an IPC Client.





