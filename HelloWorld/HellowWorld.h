#pragma once

/*
Create definitions to auto-export the functions in this DLL.
This works well for Windows, the Mac and HP/UX. For Linux
and Solaris, export everything, and for AIX, create an export
definition file. You will need to add the constants for Unix
operating systems to your makefile.
このDLL内の関数を自動的にエクスポートするための定義を作成します。
これはWindows、Mac、HP/UXではうまく動作します。
LinuxとSolarisではすべてをエクスポートし、AIXではエクスポート定義ファイルを作成します。
Unixオペレーティングシステムの定数をmakefileに追加する必要があります。
*/


#if defined (_WINDOWS) || defined (HPUX_ACC)
#define EXPORT __declspec(dllexport)
#elif defined(__APPLE__)
#define EXPORT __attribute__((visibility("default")))
#elif defined (LINUX) || defined (SOLARIS) || defined (AIX_VACPP6)
#define DLL_EXPORT
/* fine, just create the exports file for AIX
   you will have to link your app with dl.so
   so you can resolve dlopen, dlclose and dlsym.
*/
#else
#error Unsupported compiler
#endif