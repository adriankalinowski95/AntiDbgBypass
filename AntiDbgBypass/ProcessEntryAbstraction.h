#pragma once
#include <cstdint>

class ProcessEntryAbstraction {
    virtual std::uint64_t getDwSize() = 0;
    virtual std::uint64_t getProcessId() = 0;
    virtual std::uint64_t getDefaultHeapId() = 0;
    virtual std::uint64_t getModuleId() = 0;
    virtual std::uint64_t getCntThreads() = 0;
    virtual std::uint64_t getDwFlags();

    DWORD   dwSize;
    DWORD   cntUsage;
    DWORD   th32ProcessID;          // this process
    ULONG_PTR th32DefaultHeapID;
    DWORD   th32ModuleID;           // associated exe
    DWORD   cntThreads;
    DWORD   th32ParentProcessID;    // this process's parent process
    LONG    pcPriClassBase;         // Base priority of process's threads
    DWORD   dwFlags;
    WCHAR   szExeFile[MAX_PATH];    // Path
};