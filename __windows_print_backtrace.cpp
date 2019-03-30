#include "__windows_print_backtrace.h"

#pragma comment(lib, "Dbghelp.lib") 

#if 1
HANDLE g_hHandle;
HANDLE g_hThread;
CONTEXT g_context;

void InitTrack(){    g_hHandle = GetCurrentProcess();
    SymInitialize(g_hHandle, NULL, TRUE);}
void StackTrack(){    g_hThread = GetCurrentThread();    STACKFRAME sf = { 0 };
    sf.AddrPC.Offset = g_context.Eip;    sf.AddrPC.Mode = AddrModeFlat;
    sf.AddrFrame.Offset = g_context.Ebp;    sf.AddrFrame.Mode = AddrModeFlat;
    sf.AddrStack.Offset = g_context.Esp;    sf.AddrStack.Mode = AddrModeFlat;
    typedef struct tag_SYMBOL_INFO    {        IMAGEHLP_SYMBOL symInfo;        TCHAR szBuffer[MAX_PATH];    } SYMBOL_INFO, *LPSYMBOL_INFO;
    DWORD dwDisplament = 0;    SYMBOL_INFO stack_info = { 0 };    PIMAGEHLP_SYMBOL pSym = (PIMAGEHLP_SYMBOL)&stack_info;    pSym->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL);    pSym->MaxNameLength = sizeof(SYMBOL_INFO) - offsetof(SYMBOL_INFO, symInfo.Name);    IMAGEHLP_LINE ImageLine = { 0 };    ImageLine.SizeOfStruct = sizeof(IMAGEHLP_LINE);

    int frame = 0;
    while (StackWalk(IMAGE_FILE_MACHINE_I386, g_hHandle, g_hThread, &sf, &g_context, NULL, SymFunctionTableAccess, SymGetModuleBase, NULL))    {        SymGetSymFromAddr(g_hHandle, sf.AddrPC.Offset, &dwDisplament, pSym);        SymGetLineFromAddr(g_hHandle, sf.AddrPC.Offset, &dwDisplament, &ImageLine);        printf("frame:%d : %08x + %s (FILE[%s] LINE[%d])\n", frame++, pSym->Address, pSym->Name, ImageLine.FileName, ImageLine.LineNumber);    }
}
void UninitTrack(){    SymCleanup(g_hHandle);    CloseHandle(g_hHandle);    CloseHandle(g_hThread);}

#else

#ifdef UNICODE
#define wprintf_t wprintf
#else
#define wprintf_t printf
#endif

void PrintError(const TCHAR *pstr, DWORD dwErrno)
{
    LPVOID buf;
    //static const int BUFFER_SIZE = 256;
    //TCHAR * pszBuffer = (TCHAR *)malloc(sizeof(TCHAR) * BUFFER_SIZE);
    //memset(pszBuffer, 0, sizeof(TCHAR) * BUFFER_SIZE);

    if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS
        | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_MAX_WIDTH_MASK,
        NULL,
        dwErrno,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR)&buf,
        0,
        NULL))
    {
        //wsprintf(pszBuffer, _T("%s(%u): %s.\r\n"), pstr, dwErrno, buf);
        wprintf_t(_T("%s(%u): %s.\r\n"), pstr, dwErrno, buf);
        LocalFree(buf);
    }
    else
    {
        //wsprintf(pszBuffer, _T("%s: unknown Windows error(%u).\r\n"), pstr, dwErrno);
        wprintf_t(_T("%s: unknown Windows error(%u).\r\n"), pstr, dwErrno);
    }

    //wprintf_t(pszBuffer);
    //free(pszBuffer);
    //pszBuffer = NULL;
}

void TraceStack()
{
    static const int MAX_STACK_FRAMES = 10;
    void *pStack[MAX_STACK_FRAMES];

    HANDLE process = GetCurrentProcess();
    SymInitialize(process, NULL, TRUE);
    WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pStack, NULL);

    //std::ostringstream oss;
    std::cout << "stack traceback: " << std::endl;
    for (WORD i = 0; i < frames; ++i) {
        DWORD64 address = (DWORD64)(pStack[i]);

        DWORD64 displacementSym = 0;
        char buffer[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
        PSYMBOL_INFO pSymbol = (PSYMBOL_INFO)buffer;
        pSymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
        pSymbol->MaxNameLen = MAX_SYM_NAME;

        DWORD displacementLine = 0;
        IMAGEHLP_LINE64 line;
        //SymSetOptions(SYMOPT_LOAD_LINES);
        line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);

        if (SymFromAddr(process, address, &displacementSym, pSymbol)
            && SymGetLineFromAddr64(process, address, &displacementLine, &line)) {
            std::cout << "\t" << pSymbol->Name << " at " << line.FileName << " : " 
                << line.LineNumber << " (0x" << std::hex << pSymbol->Address << std::dec << ")" << std::endl;
        }
        else {
            PrintError(_T("\terror: "), GetLastError());
        }
    }

    SymCleanup(process);
}

#endif
