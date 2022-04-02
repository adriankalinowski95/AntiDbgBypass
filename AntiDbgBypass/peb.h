#pragma once
#include <basetsd.h>
#include <minwindef.h>
#pragma pack(push)
#pragma pack(1)

template <class T>
struct LIST_ENTRY_T {
    T Flink;
    T Blink;
};

template <class T>
struct UNICODE_STRING_T {
    union {
        struct {
            WORD Length;
            WORD MaximumLength;
        };
        T dummy;
    };
    T _Buffer;
};

template <class T, class NGF, int A>
struct _PEB_T {
    union {
        struct {
            BYTE InheritedAddressSpace;
            BYTE ReadImageFileExecOptions;
            BYTE BeingDebugged;
            BYTE BitField;
        };
        T dummy01;
    };
    T Mutant;
    T ImageBaseAddress;
    T Ldr;
    T ProcessParameters;
    T SubSystemData;
    T ProcessHeap;
    T FastPebLock;
    T AtlThunkSListPtr;
    T IFEOKey;
    T CrossProcessFlags;
    T UserSharedInfoPtr;
    DWORD SystemReserved;
    DWORD AtlThunkSListPtr32;
    T ApiSetMap;
    T TlsExpansionCounter;
    T TlsBitmap;
    DWORD TlsBitmapBits[2];
    T ReadOnlySharedMemoryBase;
    T HotpatchInformation;
    T ReadOnlyStaticServerData;
    T AnsiCodePageData;
    T OemCodePageData;
    T UnicodeCaseTableData;
    DWORD NumberOfProcessors;
    union {
        DWORD NtGlobalFlag;
        NGF dummy02;
    };
    LARGE_INTEGER CriticalSectionTimeout;
    T HeapSegmentReserve;
    T HeapSegmentCommit;
    T HeapDeCommitTotalFreeThreshold;
    T HeapDeCommitFreeBlockThreshold;
    DWORD NumberOfHeaps;
    DWORD MaximumNumberOfHeaps;
    T ProcessHeaps;
    T GdiSharedHandleTable;
    T ProcessStarterHelper;
    T GdiDCAttributeList;
    T LoaderLock;
    DWORD OSMajorVersion;
    DWORD OSMinorVersion;
    WORD OSBuildNumber;
    WORD OSCSDVersion;
    DWORD OSPlatformId;
    DWORD ImageSubsystem;
    DWORD ImageSubsystemMajorVersion;
    T ImageSubsystemMinorVersion;
    T ActiveProcessAffinityMask;
    T GdiHandleBuffer[A];
    T PostProcessInitRoutine;
    T TlsExpansionBitmap;
    DWORD TlsExpansionBitmapBits[32];
    T SessionId;
    ULARGE_INTEGER AppCompatFlags;
    ULARGE_INTEGER AppCompatFlagsUser;
    T pShimData;
    T AppCompatInfo;
    UNICODE_STRING_T<T> CSDVersion;
    T ActivationContextData;
    T ProcessAssemblyStorageMap;
    T SystemDefaultActivationContextData;
    T SystemAssemblyStorageMap;
    T MinimumStackCommit;
    T FlsCallback;
    LIST_ENTRY_T<T> FlsListHead;
    T FlsBitmap;
    DWORD FlsBitmapBits[4];
    T FlsHighIndex;
    T WerRegistrationData;
    T WerShipAssertPtr;
    T pContextData;
    T pImageHeaderHash;
    T TracingFlags;
};

typedef _PEB_T<DWORD, DWORD64, 34> PEB32;
typedef _PEB_T<DWORD64, DWORD, 30> PEB64;
#pragma pack(pop)

template <class T>
struct _HEAP_ENTRY_T     {
    union {
        struct {
            WORD Size;
            UCHAR Flags;
            UCHAR SmallTagIndex;
        };
        struct {
            T SubSegmentCode;
            WORD PreviousSize;
            union {
                UCHAR SegmentOffset;
                struct {
                    UCHAR LFHFlags;
                    UCHAR UnusedBytes;
                };
                struct {
                    WORD FunctionIndex;
                    WORD ContextValue;
                };
                struct {
                    ULONG InterceptorValue;
                    WORD UnusedBytesLength;
                };
                struct {
                    UCHAR EntryOffset;
                    UCHAR ExtendedBlockSignature;
                };
                struct {
                    ULONG Code1;
                    WORD Code2;
                };
                UCHAR Code3;
            };
            UCHAR Code4;
        };
        UINT64 AgregateCode;
    };
};

typedef _HEAP_ENTRY_T<DWORD> HEAP_ENTRY_32;
typedef _HEAP_ENTRY_T<DWORD64> HEAP_ENTRY_64;


template <class T>
struct _HEAP_UNPACKED_PACKET_T {
    union {
        struct {
            WORD Size;
            UCHAR Flags;
            UCHAR SmallTagIndex;
        };
        struct {
            T SubSegmentCode;
            WORD PreviousSize;
            union {
                UCHAR SegmentOffset;
                struct {
                    UCHAR LFHFlags;
                    UCHAR UnusedBytes;
                };
            }; 
        };
    };
};

typedef _HEAP_UNPACKED_PACKET_T<DWORD> HEAP_UNPACKED_PACKET_32;

template <class T>
struct _LIST_ENTRY_T {
    T Flink;
    T Blink;
};


struct _HEAP_COUNTERS {
    ULONG TotalMemoryReserved;
    ULONG TotalMemoryCommitted;
    ULONG TotalMemoryLargeUCR;
    ULONG TotalSizeInVirtualBlocks;
    ULONG TotalSegments;
    ULONG TotalUCRs;
    ULONG CommittOps;
    ULONG DeCommitOps;
    ULONG LockAcquires;
    ULONG LockCollisions;
    ULONG CommitRate;
    ULONG DecommittRate;
    ULONG CommitFailures;
    ULONG InBlockCommitFailures;
    ULONG CompactHeapCalls;
    ULONG CompactedUCRs;
    ULONG InBlockDeccommits;
    ULONG InBlockDeccomitSize;
};

struct _HEAP_TUNING_PARAMETERS {
    ULONG CommittThresholdShift;
    ULONG MaxPreCommittThreshold;
};

template <class T>
struct _HEAP_T {
    _HEAP_UNPACKED_PACKET_T<T> Entry;
    ULONG SegmentSignature;
    ULONG SegmentFlags;
    _LIST_ENTRY_T<T> SegmentListEntry;
    T Heap;
    T BaseAddress;
    ULONG NumberOfPages;
    T FirstEntry;
    T LastValidEntry;
    ULONG NumberOfUnCommittedPages;
    ULONG NumberOfUnCommittedRanges;
    WORD SegmentAllocatorBackTraceIndex;
    WORD Reserved;
    _LIST_ENTRY_T<T> UCRSegmentList;
    ULONG Flags;
    ULONG ForceFlags;
    ULONG CompatibilityFlags;
    ULONG EncodeFlagMask;
    _HEAP_UNPACKED_PACKET_T<T> Encoding;
    ULONG PointerKey;
    ULONG Interceptor;
    ULONG VirtualMemoryThreshold;
    ULONG Signature;
    ULONG SegmentReserve;
    ULONG SegmentCommit;
    ULONG DeCommitFreeBlockThreshold;
    ULONG DeCommitTotalFreeThreshold;
    ULONG TotalFreeSize;
    ULONG MaximumAllocationSize;
    WORD ProcessHeapsListIndex;
    WORD HeaderValidateLength;
    T HeaderValidateCopy;
    WORD NextAvailableTagIndex;
    WORD MaximumTagIndex;
    T TagEntries;
    _LIST_ENTRY_T<T> UCRList;
    ULONG AlignRound;
    ULONG AlignMask;
    _LIST_ENTRY_T<T> VirtualAllocdBlocks;
    _LIST_ENTRY_T<T> SegmentList;
    WORD AllocatorBackTraceIndex;
    ULONG NonDedicatedListLength;
    T BlocksIndex;
    T UCRIndex;
    T PseudoTagEntries;
    _LIST_ENTRY_T<T> FreeLists;
    T LockVariable;
    T CommitRoutine;
    T FrontEndHeap;
    WORD FrontHeapLockCount;
    UCHAR FrontEndHeapType;
    _HEAP_COUNTERS Counters;
    _HEAP_TUNING_PARAMETERS TuningParameters;
};

typedef _HEAP_T<DWORD> HEAP32;