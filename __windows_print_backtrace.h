#ifndef __WINDOWS_PRINT_BACKTRACE_HH__
#define __WINDOWS_PRINT_BACKTRACE_HH__

#ifdef _WIN32

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <DbgHelp.h>

#if 1
void InitTrack();
void StackTrack(char * buf, unsigned int maxlen);
void UninitTrack();

extern CONTEXT g_context;

#define OPEN_STACK_TRACK(buff,len) \
HANDLE hThread = GetCurrentThread();\
GetThreadContext(hThread, &g_context);\
__asm{call $ + 5}\
__asm{pop eax}\
__asm{mov g_context.Eip, eax}\
__asm{mov g_context.Ebp, ebp}\
__asm{mov g_context.Esp, esp}\
InitTrack();\
StackTrack(buff,len);\
UninitTrack();

#else
void TraceStack();
#endif

#endif //_WIN32

#endif //__WINDOWS_PRINT_BACKTRACE_HH__
