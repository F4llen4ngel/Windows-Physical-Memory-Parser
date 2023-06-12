//
// Created by vanya on 6/9/2023.
//
#include <cstdint>

#ifndef DUDEDUMPER_STRUCTS_H
#define DUDEDUMPER_STRUCTS_H

#define LONG int32_t
#define UCHAR uint8_t
#define ULONG uint32_t
#define ULONGLONG uint64_t
#define USHORT uint16_t
#define LONGLONG int64_t
#define VOID void
#define CHAR int8_t
#define SHORT int16_t


struct Process {
    uint64_t KProcessAddress;
    uint64_t DirectoryTableBase;
    std::string ProcessName;
};

#define IS_LARGE_PAGE(x)    ((bool)((x >> 7) & 1) )
#define IS_PAGE_PRESENT(x)  ((bool)(x & 1))

#define PAGE_1GB_SHIFT      30
#define PAGE_1GB_OFFSET(x)  ( x & (~(UINT64_MAX << PAGE_1GB_SHIFT)) )

#define PAGE_2MB_SHIFT      21
#define PAGE_2MB_OFFSET(x)  ( x & (~(UINT64_MAX << PAGE_2MB_SHIFT)) )

#define PAGE_4KB_SHIFT      12
#define PAGE_4KB_OFFSET(x)  ( x & (~(UINT64_MAX << PAGE_4KB_SHIFT)) )

#pragma warning(push)
#pragma warning(disable:4214) // warning C4214: nonstandard extension used: bit field types other than int

/*
 * This is the format of a virtual address which would map a 4KB underlying
 *  chunk of physical memory
 */
typedef union _VIRTUAL_MEMORY_ADDRESS
{
    struct
    {
        uint64_t PageIndex : 12;  /* 0:11  */
        uint64_t PtIndex   : 9;   /* 12:20 */
        uint64_t PdIndex   : 9;   /* 21:29 */
        uint64_t PdptIndex : 9;   /* 30:38 */
        uint64_t Pml4Index : 9;   /* 39:47 */
        uint64_t Unused    : 16;  /* 48:63 */
    } Bits;
    uint64_t All;
} VIRTUAL_ADDRESS, *PVIRTUAL_ADDRESS;

/*
 * [Intel Software Development Manual, Volume 3A: Table 4-12]
 *  "Use of CR3 with 4-Level Paging and 5-level Paging and CR4.PCIDE = 0"
 */
typedef union _DIRECTORY_TABLE_BASE
{
    struct
    {
        uint64_t Ignored0         : 3;    /* 2:0   */
        uint64_t PageWriteThrough : 1;    /* 3     */
        uint64_t PageCacheDisable : 1;    /* 4     */
        uint64_t _Ignored1        : 7;    /* 11:5  */
        uint64_t PhysicalAddress  : 36;   /* 47:12 */
        uint64_t _Reserved0       : 16;   /* 63:48 */
    } Bits;
    uint64_t All;
} CR3, DIR_TABLE_BASE;

/*
 * [Intel Software Development Manual, Volume 3A: Table 4-15]
 *  "Format of a PML4 Entry (PML4E) that References a Page-Directory-Pointer Table"
 */
typedef union _PML4_ENTRY
{
    struct
    {
        uint64_t Present          : 1;    /* 0     */
        uint64_t ReadWrite        : 1;    /* 1     */
        uint64_t UserSupervisor   : 1;    /* 2     */
        uint64_t PageWriteThrough : 1;    /* 3     */
        uint64_t PageCacheDisable : 1;    /* 4     */
        uint64_t Accessed         : 1;    /* 5     */
        uint64_t _Ignored0        : 1;    /* 6     */
        uint64_t _Reserved0       : 1;    /* 7     */
        uint64_t _Ignored1        : 4;    /* 11:8  */
        uint64_t PhysicalAddress  : 40;   /* 51:12 */
        uint64_t _Ignored2        : 11;   /* 62:52 */
        uint64_t ExecuteDisable   : 1;    /* 63    */
    } Bits;
    uint64_t All;
} PML4E;

/*
 * [Intel Software Development Manual, Volume 3A: Table 4-16]
 *  "Table 4-16. Format of a Page-Directory-Pointer-Table Entry (PDPTE) that Maps a 1-GByte Page"
 */
typedef union _PDPT_ENTRY_LARGE
{
    struct
    {
        uint64_t Present            : 1;    /* 0     */
        uint64_t ReadWrite          : 1;    /* 1     */
        uint64_t UserSupervisor     : 1;    /* 2     */
        uint64_t PageWriteThrough   : 1;    /* 3     */
        uint64_t PageCacheDisable   : 1;    /* 4     */
        uint64_t Accessed           : 1;    /* 5     */
        uint64_t Dirty              : 1;    /* 6     */
        uint64_t PageSize           : 1;    /* 7     */
        uint64_t Global             : 1;    /* 8     */
        uint64_t _Ignored0          : 3;    /* 11:9  */
        uint64_t PageAttributeTable : 1;    /* 12    */
        uint64_t _Reserved0         : 17;   /* 29:13 */
        uint64_t PhysicalAddress    : 22;   /* 51:30 */
        uint64_t _Ignored1          : 7;    /* 58:52 */
        uint64_t ProtectionKey      : 4;    /* 62:59 */
        uint64_t ExecuteDisable     : 1;    /* 63    */
    } Bits;
    uint64_t All;
} PDPTE_LARGE;

/*
 * [Intel Software Development Manual, Volume 3A: Table 4-17]
 *  "Format of a Page-Directory-Pointer-Table Entry (PDPTE) that References a Page Directory"
 */
typedef union _PDPT_ENTRY
{
    struct
    {
        uint64_t Present          : 1;    /* 0     */
        uint64_t ReadWrite        : 1;    /* 1     */
        uint64_t UserSupervisor   : 1;    /* 2     */
        uint64_t PageWriteThrough : 1;    /* 3     */
        uint64_t PageCacheDisable : 1;    /* 4     */
        uint64_t Accessed         : 1;    /* 5     */
        uint64_t _Ignored0        : 1;    /* 6     */
        uint64_t PageSize         : 1;    /* 7     */
        uint64_t _Ignored1        : 4;    /* 11:8  */
        uint64_t PhysicalAddress  : 40;   /* 51:12 */
        uint64_t _Ignored2        : 11;   /* 62:52 */
        uint64_t ExecuteDisable   : 1;    /* 63    */
    } Bits;
    uint64_t All;
} PDPTE;

/*
 * [Intel Software Development Manual, Volume 3A: Table 4-18]
 *  "Table 4-18. Format of a Page-Directory Entry that Maps a 2-MByte Page"
 */
typedef union _PD_ENTRY_LARGE
{
    struct
    {
        uint64_t Present            : 1;    /* 0     */
        uint64_t ReadWrite          : 1;    /* 1     */
        uint64_t UserSupervisor     : 1;    /* 2     */
        uint64_t PageWriteThrough   : 1;    /* 3     */
        uint64_t PageCacheDisable   : 1;    /* 4     */
        uint64_t Accessed           : 1;    /* 5     */
        uint64_t Dirty              : 1;    /* 6     */
        uint64_t PageSize           : 1;    /* 7     */
        uint64_t Global             : 1;    /* 8     */
        uint64_t _Ignored0          : 3;    /* 11:9  */
        uint64_t PageAttributeTalbe : 1;    /* 12    */
        uint64_t _Reserved0         : 8;    /* 20:13 */
        uint64_t PhysicalAddress    : 29;   /* 49:21 */
        uint64_t _Reserved1         : 2;    /* 51:50 */
        uint64_t _Ignored1          : 7;    /* 58:52 */
        uint64_t ProtectionKey      : 4;    /* 62:59 */
        uint64_t ExecuteDisable     : 1;    /* 63    */
    } Bits;
    uint64_t All;
} PDE_LARGE;

/*
 * [Intel Software Development Manual, Volume 3A: Table 4-19]
 *  "Format of a Page-Directory Entry that References a Page Table"
 */
typedef union _PD_ENTRY
{
    struct
    {
        uint64_t Present            : 1;    /* 0     */
        uint64_t ReadWrite          : 1;    /* 1     */
        uint64_t UserSupervisor     : 1;    /* 2     */
        uint64_t PageWriteThrough   : 1;    /* 3     */
        uint64_t PageCacheDisable   : 1;    /* 4     */
        uint64_t Accessed           : 1;    /* 5     */
        uint64_t _Ignored0          : 1;    /* 6     */
        uint64_t PageSize           : 1;    /* 7     */
        uint64_t _Ignored1          : 4;    /* 11:8  */
        uint64_t PhysicalAddress    : 38;   /* 49:12 */
        uint64_t _Reserved0         : 2;    /* 51:50 */
        uint64_t _Ignored2          : 11;   /* 62:52 */
        uint64_t ExecuteDisable     : 1;    /* 63    */
    } Bits;
    uint64_t All;
} PDE;

/*
 * [Intel Software Development Manual, Volume 3A: Table 4-20]
 *  "Format of a Page-Table Entry that Maps a 4-KByte Page"
 */
typedef union _PT_ENTRY
{
    struct
    {
        uint64_t Present            : 1;    /* 0     */
        uint64_t ReadWrite          : 1;    /* 1     */
        uint64_t UserSupervisor     : 1;    /* 2     */
        uint64_t PageWriteThrough   : 1;    /* 3     */
        uint64_t PageCacheDisable   : 1;    /* 4     */
        uint64_t Accessed           : 1;    /* 5     */
        uint64_t Dirty              : 1;    /* 6     */
        uint64_t PageAttributeTable : 1;    /* 7     */
        uint64_t Global             : 1;    /* 8     */
        uint64_t _Ignored0          : 3;    /* 11:9  */
        uint64_t PhysicalAddress    : 38;   /* 49:12 */
        uint64_t _Reserved0         : 2;    /* 51:50 */
        uint64_t _Ignored1          : 7;    /* 58:52 */
        uint64_t ProtectionKey      : 4;    /* 62:59 */
        uint64_t ExecuteDisable     : 1;    /* 63    */
    } Bits;
    uint64_t All;
} PTE;

/*
 * Above I'm making use of some paging structures I
 *  created while parsing out definitions within the SDM.
 *  The address bits in the above structures should be
 *  right. You can also use the previously-mentioned
 *  Windows-specific general page table structure definition,
 *  which I have taken out of KD and added a definition
 *  for below.
 *
 * 1: kd> dt ntkrnlmp!_MMPTE_HARDWARE
 *    +0x000 Valid            : Pos 0, 1 Bit
 *    +0x000 Dirty1           : Pos 1, 1 Bit
 *    +0x000 Owner            : Pos 2, 1 Bit
 *    +0x000 WriteThrough     : Pos 3, 1 Bit
 *    +0x000 CacheDisable     : Pos 4, 1 Bit
 *    +0x000 Accessed         : Pos 5, 1 Bit
 *    +0x000 Dirty            : Pos 6, 1 Bit
 *    +0x000 LargePage        : Pos 7, 1 Bit
 *    +0x000 Global           : Pos 8, 1 Bit
 *    +0x000 CopyOnWrite      : Pos 9, 1 Bit
 *    +0x000 Unused           : Pos 10, 1 Bit
 *    +0x000 Write            : Pos 11, 1 Bit
 *    +0x000 PageFrameNumber  : Pos 12, 36 Bits
 *    +0x000 ReservedForHardware : Pos 48, 4 Bits
 *    +0x000 ReservedForSoftware : Pos 52, 4 Bits
 *    +0x000 WsleAge          : Pos 56, 4 Bits
 *    +0x000 WsleProtection   : Pos 60, 3 Bits
 *    +0x000 NoExecute        : Pos 63, 1 Bit
 */
typedef union _MMPTE_HARDWARE
{
    struct
    {
        uint64_t Valid               : 1;    /* 0     */
        uint64_t Dirty1              : 1;    /* 1     */
        uint64_t Owner               : 1;    /* 2     */
        uint64_t WriteThrough        : 1;    /* 3     */
        uint64_t CacheDisable        : 1;    /* 4     */
        uint64_t Accessed            : 1;    /* 5     */
        uint64_t Dirty               : 1;    /* 6     */
        uint64_t LargePage           : 1;    /* 7     */
        uint64_t Global              : 1;    /* 8     */
        uint64_t CopyOnWrite         : 1;    /* 9     */
        uint64_t Unused              : 1;    /* 10    */
        uint64_t Write               : 1;    /* 11    */
        uint64_t PageFrameNumber     : 36;   /* 47:12 */
        uint64_t ReservedForHardware : 4;    /* 51:48 */
        uint64_t ReservedForSoftware : 4;    /* 55:52 */
        uint64_t WsleAge             : 4;    /* 59:56 */
        uint64_t WsleProtection      : 3;    /* 62:60 */
        uint64_t NoExecute           : 1;    /* 63 */
    } Bits;
    uint64_t All;
} MMPTE_HARDWARE;


//0x10 bytes (sizeof)
struct _LIST_ENTRY
{
    struct _LIST_ENTRY* Flink;                                              //0x0
    struct _LIST_ENTRY* Blink;                                              //0x8
};

//0x18 bytes (sizeof)
struct _DISPATCHER_HEADER
{
    union
    {
        volatile LONG Lock;                                                 //0x0
        LONG LockNV;                                                        //0x0
        struct
        {
            UCHAR Type;                                                     //0x0
            UCHAR Signalling;                                               //0x1
            UCHAR Size;                                                     //0x2
            UCHAR Reserved1;                                                //0x3
        };
        struct
        {
            UCHAR TimerType;                                                //0x0
            union
            {
                UCHAR TimerControlFlags;                                    //0x1
                struct
                {
                    UCHAR Absolute:1;                                       //0x1
                    UCHAR Wake:1;                                           //0x1
                    UCHAR EncodedTolerableDelay:6;                          //0x1
                };
            };
            UCHAR Hand;                                                     //0x2
            union
            {
                UCHAR TimerMiscFlags;                                       //0x3
                struct
                {
                    UCHAR Index:6;                                          //0x3
                    UCHAR Inserted:1;                                       //0x3
                    volatile UCHAR Expired:1;                               //0x3
                };
            };
        };
        struct
        {
            UCHAR Timer2Type;                                               //0x0
            union
            {
                UCHAR Timer2Flags;                                          //0x1
                struct
                {
                    UCHAR Timer2Inserted:1;                                 //0x1
                    UCHAR Timer2Expiring:1;                                 //0x1
                    UCHAR Timer2CancelPending:1;                            //0x1
                    UCHAR Timer2SetPending:1;                               //0x1
                    UCHAR Timer2Running:1;                                  //0x1
                    UCHAR Timer2Disabled:1;                                 //0x1
                    UCHAR Timer2ReservedFlags:2;                            //0x1
                };
            };
            UCHAR Timer2ComponentId;                                        //0x2
            UCHAR Timer2RelativeId;                                         //0x3
        };
        struct
        {
            UCHAR QueueType;                                                //0x0
            union
            {
                UCHAR QueueControlFlags;                                    //0x1
                struct
                {
                    UCHAR Abandoned:1;                                      //0x1
                    UCHAR DisableIncrement:1;                               //0x1
                    UCHAR QueueReservedControlFlags:6;                      //0x1
                };
            };
            UCHAR QueueSize;                                                //0x2
            UCHAR QueueReserved;                                            //0x3
        };
        struct
        {
            UCHAR ThreadType;                                               //0x0
            UCHAR ThreadReserved;                                           //0x1
            union
            {
                UCHAR ThreadControlFlags;                                   //0x2
                struct
                {
                    UCHAR CycleProfiling:1;                                 //0x2
                    UCHAR CounterProfiling:1;                               //0x2
                    UCHAR GroupScheduling:1;                                //0x2
                    UCHAR AffinitySet:1;                                    //0x2
                    UCHAR Tagged:1;                                         //0x2
                    UCHAR EnergyProfiling:1;                                //0x2
                    UCHAR SchedulerAssist:1;                                //0x2
                    UCHAR ThreadReservedControlFlags:1;                     //0x2
                };
            };
            union
            {
                UCHAR DebugActive;                                          //0x3
                struct
                {
                    UCHAR ActiveDR7:1;                                      //0x3
                    UCHAR Instrumented:1;                                   //0x3
                    UCHAR Minimal:1;                                        //0x3
                    UCHAR Reserved4:2;                                      //0x3
                    UCHAR AltSyscall:1;                                     //0x3
                    UCHAR Emulation:1;                                      //0x3
                    UCHAR Reserved5:1;                                      //0x3
                };
            };
        };
        struct
        {
            UCHAR MutantType;                                               //0x0
            UCHAR MutantSize;                                               //0x1
            UCHAR DpcActive;                                                //0x2
            UCHAR MutantReserved;                                           //0x3
        };
    };
    LONG SignalState;                                                       //0x4
    struct _LIST_ENTRY WaitListHead;                                        //0x8
};

//0x108 bytes (sizeof)
struct _KAFFINITY_EX
{
    USHORT Count;                                                           //0x0
    USHORT Size;                                                            //0x2
    ULONG Reserved;                                                         //0x4
    union
    {
        ULONGLONG Bitmap[1];                                                //0x8
        ULONGLONG StaticBitmap[32];                                         //0x8
    };
};

//0x8 bytes (sizeof)
struct _SINGLE_LIST_ENTRY
{
    struct _SINGLE_LIST_ENTRY* Next;                                        //0x0
};

//0x1 bytes (sizeof)
union _KEXECUTE_OPTIONS
{
    UCHAR ExecuteDisable:1;                                                 //0x0
    UCHAR ExecuteEnable:1;                                                  //0x0
    UCHAR DisableThunkEmulation:1;                                          //0x0
    UCHAR Permanent:1;                                                      //0x0
    UCHAR ExecuteDispatchEnable:1;                                          //0x0
    UCHAR ImageDispatchEnable:1;                                            //0x0
    UCHAR DisableExceptionChainValidation:1;                                //0x0
    UCHAR Spare:1;                                                          //0x0
    volatile UCHAR ExecuteOptions;                                          //0x0
    UCHAR ExecuteOptionsNV;                                                 //0x0
};

//0x4 bytes (sizeof)
union _KSTACK_COUNT
{
    LONG Value;                                                             //0x0
    ULONG State:3;                                                          //0x0
    ULONG StackCount:29;                                                    //0x0
};

//0x8 bytes (sizeof)
struct _KSCHEDULING_GROUP_POLICY
{
    union
    {
        ULONG Value;                                                        //0x0
        USHORT Weight;                                                      //0x0
        struct
        {
            USHORT MinRate;                                                 //0x0
            USHORT MaxRate;                                                 //0x2
        };
    };
    union
    {
        ULONG AllFlags;                                                     //0x4
        struct
        {
            ULONG Type:1;                                                   //0x4
            ULONG Disabled:1;                                               //0x4
            ULONG RankBias:1;                                               //0x4
            ULONG Spare1:29;                                                //0x4
        };
    };
};

//0x18 bytes (sizeof)
struct _RTL_BALANCED_NODE
{
    union
    {
        struct _RTL_BALANCED_NODE* Children[2];                             //0x0
        struct
        {
            struct _RTL_BALANCED_NODE* Left;                                //0x0
            struct _RTL_BALANCED_NODE* Right;                               //0x8
        };
    };
    union
    {
        struct
        {
            UCHAR Red:1;                                                    //0x10
            UCHAR Balance:2;                                                //0x10
        };
        ULONGLONG ParentValue;                                              //0x10
    };
};

//0x10 bytes (sizeof)
struct _RTL_RB_TREE
{
    struct _RTL_BALANCED_NODE* Root;                                        //0x0
    union
    {
        UCHAR Encoded:1;                                                    //0x8
        struct _RTL_BALANCED_NODE* Min;                                     //0x8
    };
};

//0x1a8 bytes (sizeof)
struct _KSCB
{
    ULONGLONG GenerationCycles;                                             //0x0
    ULONGLONG MinQuotaCycleTarget;                                          //0x8
    ULONGLONG MaxQuotaCycleTarget;                                          //0x10
    ULONGLONG RankCycleTarget;                                              //0x18
    ULONGLONG LongTermCycles;                                               //0x20
    ULONGLONG LastReportedCycles;                                           //0x28
    volatile ULONGLONG OverQuotaHistory;                                    //0x30
    ULONGLONG ReadyTime;                                                    //0x38
    ULONGLONG InsertTime;                                                   //0x40
    struct _LIST_ENTRY PerProcessorList;                                    //0x48
    struct _RTL_BALANCED_NODE QueueNode;                                    //0x58
    UCHAR Inserted:1;                                                       //0x70
    UCHAR MaxOverQuota:1;                                                   //0x70
    UCHAR MinOverQuota:1;                                                   //0x70
    UCHAR RankBias:1;                                                       //0x70
    UCHAR UnconstrainedMaxQuota:1;                                          //0x70
    UCHAR UnconstrainedMinQuota:1;                                          //0x70
    UCHAR ShareRankOwner:1;                                                 //0x70
    UCHAR Spare1:1;                                                         //0x70
    UCHAR Depth;                                                            //0x71
    USHORT ReadySummary;                                                    //0x72
    ULONG Rank;                                                             //0x74
    volatile ULONG* ShareRank;                                              //0x78
    volatile ULONG OwnerShareRank;                                          //0x80
    struct _LIST_ENTRY ReadyListHead[16];                                   //0x88
    struct _RTL_RB_TREE ChildScbQueue;                                      //0x188
    struct _KSCB* Parent;                                                   //0x198
    struct _KSCB* Root;                                                     //0x1a0
};

//0x240 bytes (sizeof)
struct _KSCHEDULING_GROUP
{
    struct _KSCHEDULING_GROUP_POLICY Policy;                                //0x0
    ULONG RelativeWeight;                                                   //0x8
    ULONG ChildMinRate;                                                     //0xc
    ULONG ChildMinWeight;                                                   //0x10
    ULONG ChildTotalWeight;                                                 //0x14
    ULONGLONG QueryHistoryTimeStamp;                                        //0x18
    LONGLONG NotificationCycles;                                            //0x20
    LONGLONG MaxQuotaLimitCycles;                                           //0x28
    volatile LONGLONG MaxQuotaCyclesRemaining;                              //0x30
    union
    {
        struct _LIST_ENTRY SchedulingGroupList;                             //0x38
        struct _LIST_ENTRY Sibling;                                         //0x38
    };
    struct _KDPC* NotificationDpc;                                          //0x48
    struct _LIST_ENTRY ChildList;                                           //0x50
    struct _KSCHEDULING_GROUP* Parent;                                      //0x60
    struct _KSCB PerProcessor[1];                                           //0x80
};

//0x40 bytes (sizeof)
struct _KDPC
{
    union
    {
        ULONG TargetInfoAsUlong;                                            //0x0
        struct
        {
            UCHAR Type;                                                     //0x0
            UCHAR Importance;                                               //0x1
            volatile USHORT Number;                                         //0x2
        };
    };
    struct _SINGLE_LIST_ENTRY DpcListEntry;                                 //0x8
    ULONGLONG ProcessorHistory;                                             //0x10
    VOID (*DeferredRoutine)(struct _KDPC* arg1, VOID* arg2, VOID* arg3, VOID* arg4); //0x18
    VOID* DeferredContext;                                                  //0x20
    VOID* SystemArgument1;                                                  //0x28
    VOID* SystemArgument2;                                                  //0x30
    VOID* DpcData;                                                          //0x38
};

//0x438 bytes (sizeof)
struct _KPROCESS
{
    struct _DISPATCHER_HEADER Header;                                       //0x0
    struct _LIST_ENTRY ProfileListHead;                                     //0x18
    ULONGLONG DirectoryTableBase;                                           //0x28
    struct _LIST_ENTRY ThreadListHead;                                      //0x30
    ULONG ProcessLock;                                                      //0x40
    ULONG ProcessTimerDelay;                                                //0x44
    ULONGLONG DeepFreezeStartTime;                                          //0x48
    struct _KAFFINITY_EX Affinity;                                          //0x50
    struct _LIST_ENTRY ReadyListHead;                                       //0x158
    struct _SINGLE_LIST_ENTRY SwapListEntry;                                //0x168
    volatile struct _KAFFINITY_EX ActiveProcessors;                         //0x170
    union
    {
        struct
        {
            ULONG AutoAlignment:1;                                          //0x278
            ULONG DisableBoost:1;                                           //0x278
            ULONG DisableQuantum:1;                                         //0x278
            ULONG DeepFreeze:1;                                             //0x278
            ULONG TimerVirtualization:1;                                    //0x278
            ULONG CheckStackExtents:1;                                      //0x278
            ULONG CacheIsolationEnabled:1;                                  //0x278
            ULONG PpmPolicy:4;                                              //0x278
            ULONG VaSpaceDeleted:1;                                         //0x278
            ULONG MultiGroup:1;                                             //0x278
            ULONG ReservedFlags:19;                                         //0x278
        };
        volatile LONG ProcessFlags;                                         //0x278
    };
    ULONG ActiveGroupsMask;                                                 //0x27c
    CHAR BasePriority;                                                      //0x280
    CHAR QuantumReset;                                                      //0x281
    CHAR Visited;                                                           //0x282
    union _KEXECUTE_OPTIONS Flags;                                          //0x283
    USHORT ThreadSeed[32];                                                  //0x284
    USHORT IdealProcessor[32];                                              //0x2c4
    USHORT IdealNode[32];                                                   //0x304
    USHORT IdealGlobalNode;                                                 //0x344
    USHORT Spare1;                                                          //0x346
    volatile _KSTACK_COUNT StackCount;                                 //0x348
    struct _LIST_ENTRY ProcessListEntry;                                    //0x350
    ULONGLONG CycleTime;                                                    //0x360
    ULONGLONG ContextSwitches;                                              //0x368
    struct _KSCHEDULING_GROUP* SchedulingGroup;                             //0x370
    ULONG FreezeCount;                                                      //0x378
    ULONG KernelTime;                                                       //0x37c
    ULONG UserTime;                                                         //0x380
    ULONG ReadyTime;                                                        //0x384
    ULONGLONG UserDirectoryTableBase;                                       //0x388
    UCHAR AddressPolicy;                                                    //0x390
    UCHAR Spare2[71];                                                       //0x391
    VOID* InstrumentationCallback;                                          //0x3d8
    union
    {
        ULONGLONG SecureHandle;                                             //0x3e0
        struct
        {
            ULONGLONG SecureProcess:1;                                      //0x3e0
            ULONGLONG Unused:1;                                             //0x3e0
        } Flags;                                                            //0x3e0
    } SecureState;                                                          //0x3e0
    ULONGLONG KernelWaitTime;                                               //0x3e8
    ULONGLONG UserWaitTime;                                                 //0x3f0
    ULONGLONG LastRebalanceQpc;                                             //0x3f8
    VOID* PerProcessorCycleTimes;                                           //0x400
    ULONGLONG ExtendedFeatureDisableMask;                                   //0x408
    USHORT PrimaryGroup;                                                    //0x410
    USHORT Spare3[3];                                                       //0x412
    VOID* UserCetLogging;                                                   //0x418
    struct _LIST_ENTRY CpuPartitionList;                                    //0x420
    ULONGLONG EndPadding[1];                                                //0x430
};

//0x8 bytes (sizeof)
struct _EX_PUSH_LOCK
{
    union
    {
        struct
        {
            ULONGLONG Locked:1;                                             //0x0
            ULONGLONG Waiting:1;                                            //0x0
            ULONGLONG Waking:1;                                             //0x0
            ULONGLONG MultipleShared:1;                                     //0x0
            ULONGLONG Shared:60;                                            //0x0
        };
        ULONGLONG Value;                                                    //0x0
        VOID* Ptr;                                                          //0x0
    };
};

//0x8 bytes (sizeof)
struct _EX_RUNDOWN_REF
{
    union
    {
        ULONGLONG Count;                                                    //0x0
        VOID* Ptr;                                                          //0x0
    };
};

//0x8 bytes (sizeof)
union _LARGE_INTEGER
{
    struct
    {
        ULONG LowPart;                                                      //0x0
        LONG HighPart;                                                      //0x4
    };
    struct
    {
        ULONG LowPart;                                                      //0x0
        LONG HighPart;                                                      //0x4
    } u;                                                                    //0x0
    LONGLONG QuadPart;                                                      //0x0
};

//0x8 bytes (sizeof)
struct _EX_FAST_REF
{
    union
    {
        VOID* Object;                                                       //0x0
        ULONGLONG RefCnt:4;                                                 //0x0
        ULONGLONG Value;                                                    //0x0
    };
};

//0x10 bytes (sizeof)
struct _M128A
{
    ULONGLONG Low;                                                          //0x0
    LONGLONG High;                                                          //0x8
};

//0x200 bytes (sizeof)
struct _XSAVE_FORMAT
{
    USHORT ControlWord;                                                     //0x0
    USHORT StatusWord;                                                      //0x2
    UCHAR TagWord;                                                          //0x4
    UCHAR Reserved1;                                                        //0x5
    USHORT ErrorOpcode;                                                     //0x6
    ULONG ErrorOffset;                                                      //0x8
    USHORT ErrorSelector;                                                   //0xc
    USHORT Reserved2;                                                       //0xe
    ULONG DataOffset;                                                       //0x10
    USHORT DataSelector;                                                    //0x14
    USHORT Reserved3;                                                       //0x16
    ULONG MxCsr;                                                            //0x18
    ULONG MxCsr_Mask;                                                       //0x1c
    struct _M128A FloatRegisters[8];                                        //0x20
    struct _M128A XmmRegisters[16];                                         //0xa0
    UCHAR Reserved4[96];                                                    //0x1a0
};

//0x1 bytes (sizeof)
union _KWAIT_STATUS_REGISTER
{
    UCHAR Flags;                                                            //0x0
    UCHAR State:3;                                                          //0x0
    UCHAR Affinity:1;                                                       //0x0
    UCHAR Priority:1;                                                       //0x0
    UCHAR Apc:1;                                                            //0x0
    UCHAR UserApc:1;                                                        //0x0
    UCHAR Alert:1;                                                          //0x0
};

//0x190 bytes (sizeof)
struct _KTRAP_FRAME
{
    ULONGLONG P1Home;                                                       //0x0
    ULONGLONG P2Home;                                                       //0x8
    ULONGLONG P3Home;                                                       //0x10
    ULONGLONG P4Home;                                                       //0x18
    ULONGLONG P5;                                                           //0x20
    union
    {
        CHAR PreviousMode;                                                  //0x28
        UCHAR InterruptRetpolineState;                                      //0x28
    };
    UCHAR PreviousIrql;                                                     //0x29
    union
    {
        UCHAR FaultIndicator;                                               //0x2a
        UCHAR NmiMsrIbrs;                                                   //0x2a
    };
    UCHAR ExceptionActive;                                                  //0x2b
    ULONG MxCsr;                                                            //0x2c
    ULONGLONG Rax;                                                          //0x30
    ULONGLONG Rcx;                                                          //0x38
    ULONGLONG Rdx;                                                          //0x40
    ULONGLONG R8;                                                           //0x48
    ULONGLONG R9;                                                           //0x50
    ULONGLONG R10;                                                          //0x58
    ULONGLONG R11;                                                          //0x60
    union
    {
        ULONGLONG GsBase;                                                   //0x68
        ULONGLONG GsSwap;                                                   //0x68
    };
    struct _M128A Xmm0;                                                     //0x70
    struct _M128A Xmm1;                                                     //0x80
    struct _M128A Xmm2;                                                     //0x90
    struct _M128A Xmm3;                                                     //0xa0
    struct _M128A Xmm4;                                                     //0xb0
    struct _M128A Xmm5;                                                     //0xc0
    union
    {
        ULONGLONG FaultAddress;                                             //0xd0
        ULONGLONG ContextRecord;                                            //0xd0
    };
    union
    {
        struct
        {
            ULONGLONG Dr0;                                                  //0xd8
            ULONGLONG Dr1;                                                  //0xe0
            ULONGLONG Dr2;                                                  //0xe8
            ULONGLONG Dr3;                                                  //0xf0
            ULONGLONG Dr6;                                                  //0xf8
            ULONGLONG Dr7;                                                  //0x100
        };
        struct
        {
            ULONGLONG ShadowStackFrame;                                     //0xd8
            ULONGLONG Spare[5];                                             //0xe0
        };
    };
    ULONGLONG DebugControl;                                                 //0x108
    ULONGLONG LastBranchToRip;                                              //0x110
    ULONGLONG LastBranchFromRip;                                            //0x118
    ULONGLONG LastExceptionToRip;                                           //0x120
    ULONGLONG LastExceptionFromRip;                                         //0x128
    USHORT SegDs;                                                           //0x130
    USHORT SegEs;                                                           //0x132
    USHORT SegFs;                                                           //0x134
    USHORT SegGs;                                                           //0x136
    ULONGLONG TrapFrame;                                                    //0x138
    ULONGLONG Rbx;                                                          //0x140
    ULONGLONG Rdi;                                                          //0x148
    ULONGLONG Rsi;                                                          //0x150
    ULONGLONG Rbp;                                                          //0x158
    union
    {
        ULONGLONG ErrorCode;                                                //0x160
        ULONGLONG ExceptionFrame;                                           //0x160
    };
    ULONGLONG Rip;                                                          //0x168
    USHORT SegCs;                                                           //0x170
    UCHAR Fill0;                                                            //0x172
    UCHAR Logging;                                                          //0x173
    USHORT Fill1[2];                                                        //0x174
    ULONG EFlags;                                                           //0x178
    ULONG Fill2;                                                            //0x17c
    ULONGLONG Rsp;                                                          //0x180
    USHORT SegSs;                                                           //0x188
    USHORT Fill3;                                                           //0x18a
    ULONG Fill4;                                                            //0x18c
};

//0x30 bytes (sizeof)
struct _KAPC_STATE
{
    struct _LIST_ENTRY ApcListHead[2];                                      //0x0
    struct _KPROCESS* Process;                                              //0x20
    union
    {
        UCHAR InProgressFlags;                                              //0x28
        struct
        {
            UCHAR KernelApcInProgress:1;                                    //0x28
            UCHAR SpecialApcInProgress:1;                                   //0x28
        };
    };
    UCHAR KernelApcPending;                                                 //0x29
    union
    {
        UCHAR UserApcPendingAll;                                            //0x2a
        struct
        {
            UCHAR SpecialUserApcPending:1;                                  //0x2a
            UCHAR UserApcPending:1;                                         //0x2a
        };
    };
};

//0x40 bytes (sizeof)
struct _KQUEUE
{
    struct _DISPATCHER_HEADER Header;                                       //0x0
    struct _LIST_ENTRY EntryListHead;                                       //0x18
    volatile ULONG CurrentCount;                                            //0x28
    ULONG MaximumCount;                                                     //0x2c
    struct _LIST_ENTRY ThreadListHead;                                      //0x30
};

//0x30 bytes (sizeof)
struct _KWAIT_BLOCK
{
    struct _LIST_ENTRY WaitListEntry;                                       //0x0
    UCHAR WaitType;                                                         //0x10
    volatile UCHAR BlockState;                                              //0x11
    USHORT WaitKey;                                                         //0x12
    LONG SpareLong;                                                         //0x14
    union
    {
        struct _KTHREAD* Thread;                                            //0x18
        struct _KQUEUE* NotificationQueue;                                  //0x18
        struct _KDPC* Dpc;                                                  //0x18
    };
    VOID* Object;                                                           //0x20
    VOID* SparePtr;                                                         //0x28
};

//0x8 bytes (sizeof)
union _ULARGE_INTEGER
{
    struct
    {
        ULONG LowPart;                                                      //0x0
        ULONG HighPart;                                                     //0x4
    };
    struct
    {
        ULONG LowPart;                                                      //0x0
        ULONG HighPart;                                                     //0x4
    } u;                                                                    //0x0
    ULONGLONG QuadPart;                                                     //0x0
};

//0x40 bytes (sizeof)
struct _KTIMER
{
    struct _DISPATCHER_HEADER Header;                                       //0x0
    union _ULARGE_INTEGER DueTime;                                          //0x18
    struct _LIST_ENTRY TimerListEntry;                                      //0x20
    struct _KDPC* Dpc;                                                      //0x30
    USHORT Processor;                                                       //0x38
    USHORT TimerType;                                                       //0x3a
    ULONG Period;                                                           //0x3c
};

//0x4 bytes (sizeof)
struct _PROCESSOR_NUMBER
{
    USHORT Group;                                                           //0x0
    UCHAR Number;                                                           //0x2
    UCHAR Reserved;                                                         //0x3
};

//0x4 bytes (sizeof)
enum _HARDWARE_COUNTER_TYPE
{
    PMCCounter = 0,
    MaxHardwareCounterType = 1
};

//0x18 bytes (sizeof)
struct _COUNTER_READING
{
    enum _HARDWARE_COUNTER_TYPE Type;                                       //0x0
    ULONG Index;                                                            //0x4
    ULONGLONG Start;                                                        //0x8
    ULONGLONG Total;                                                        //0x10
};

//0x1c0 bytes (sizeof)
struct _THREAD_PERFORMANCE_DATA
{
    USHORT Size;                                                            //0x0
    USHORT Version;                                                         //0x2
    struct _PROCESSOR_NUMBER ProcessorNumber;                               //0x4
    ULONG ContextSwitches;                                                  //0x8
    ULONG HwCountersCount;                                                  //0xc
    volatile ULONGLONG UpdateCount;                                         //0x10
    ULONGLONG WaitReasonBitMap;                                             //0x18
    ULONGLONG HardwareCounters;                                             //0x20
    struct _COUNTER_READING CycleTime;                                      //0x28
    struct _COUNTER_READING HwCounters[16];                                 //0x40
};

//0x1a8 bytes (sizeof)
struct _KTHREAD_COUNTERS
{
    ULONGLONG WaitReasonBitMap;                                             //0x0
    struct _THREAD_PERFORMANCE_DATA* UserData;                              //0x8
    ULONG Flags;                                                            //0x10
    ULONG ContextSwitches;                                                  //0x14
    ULONGLONG CycleTimeBias;                                                //0x18
    ULONGLONG HardwareCounters;                                             //0x20
    struct _COUNTER_READING HwCounter[16];                                  //0x28
};

//0x40 bytes (sizeof)
struct _XSAVE_AREA_HEADER
{
    ULONGLONG Mask;                                                         //0x0
    ULONGLONG CompactionMask;                                               //0x8
    ULONGLONG Reserved2[6];                                                 //0x10
};

//0x240 bytes (sizeof)
struct _XSAVE_AREA
{
    struct _XSAVE_FORMAT LegacyState;                                       //0x0
    struct _XSAVE_AREA_HEADER Header;                                       //0x200
};

//0x20 bytes (sizeof)
struct _XSTATE_CONTEXT
{
    ULONGLONG Mask;                                                         //0x0
    ULONG Length;                                                           //0x8
    ULONG Reserved1;                                                        //0xc
    struct _XSAVE_AREA* Area;                                               //0x10
    VOID* Buffer;                                                           //0x18
};

//0x38 bytes (sizeof)
struct _XSTATE_SAVE
{
    struct _XSTATE_SAVE* Prev;                                              //0x0
    struct _KTHREAD* Thread;                                                //0x8
    UCHAR Level;                                                            //0x10
    struct _XSTATE_CONTEXT XStateContext;                                   //0x18
};

//0x58 bytes (sizeof)
struct _KAPC
{
    UCHAR Type;                                                             //0x0
    UCHAR AllFlags;                                                         //0x1
    UCHAR Size;                                                             //0x2
    UCHAR SpareByte1;                                                       //0x3
    ULONG SpareLong0;                                                       //0x4
    struct _KTHREAD* Thread;                                                //0x8
    struct _LIST_ENTRY ApcListEntry;                                        //0x10
    VOID* Reserved[3];                                                      //0x20
    VOID* NormalContext;                                                    //0x38
    VOID* SystemArgument1;                                                  //0x40
    VOID* SystemArgument2;                                                  //0x48
    CHAR ApcStateIndex;                                                     //0x50
    CHAR ApcMode;                                                           //0x51
    UCHAR Inserted;                                                         //0x52
};

//0x1 bytes (sizeof)
union _KPRIORITY_STATE
{
    UCHAR Priority:7;                                                       //0x0
    UCHAR IsolationWidth:1;                                                 //0x0
    UCHAR AllFields;                                                        //0x0
};

//0x20 bytes (sizeof)
struct _KPRCB_TRACEPOINT_LOG_ENTRY
{
    ULONGLONG OldPc;                                                        //0x0
    ULONGLONG OldSp;                                                        //0x8
    ULONGLONG NewPc;                                                        //0x10
    ULONGLONG NewSp;                                                        //0x18
};

//0x2008 bytes (sizeof)
struct _KPRCB_TRACEPOINT_LOG
{
    struct _KPRCB_TRACEPOINT_LOG_ENTRY Entries[256];                        //0x0
    ULONG LogIndex;                                                         //0x2000
};

//0x10 bytes (sizeof)
struct _GROUP_AFFINITY
{
    ULONGLONG Mask;                                                         //0x0
    USHORT Group;                                                           //0x8
    USHORT Reserved[3];                                                     //0xa
};

//0x1 bytes (sizeof)
struct _flags
{
    UCHAR SmtSetsPresent:1;                                                 //0x0
    UCHAR Fill:7;                                                           //0x0
};

//0x18 bytes (sizeof)
struct _KHETERO_PROCESSOR_SET
{
    ULONGLONG IdealMask;                                                    //0x0
    ULONGLONG PreferredMask;                                                //0x8
    ULONGLONG AvailableMask;                                                //0x10
};

//0x10 bytes (sizeof)
union _KQOS_GROUPING_SETS
{
    struct
    {
        ULONGLONG SingleCoreSet;                                            //0x0
    };
    ULONGLONG SmtSet;                                                       //0x8
};

//0x180 bytes (sizeof)
struct _KSCHEDULER_SUBNODE
{
    ULONGLONG SubNodeLock;                                                  //0x0
    ULONGLONG IdleNonParkedCpuSet;                                          //0x8
    union
    {
        struct
        {
            ULONGLONG IdleCpuSet;                                           //0x10
            ULONGLONG IdleSmtSet;                                           //0x18
            ULONGLONG IdleModuleSet;                                        //0x20
        };
        ULONGLONG IdleIsolationUnitSet[2];                                  //0x10
    };
    ULONGLONG NonPairedSmtSet;                                              //0x28
    ULONGLONG DeepIdleSet;                                                  //0x40
    ULONGLONG IdleConstrainedSet;                                           //0x48
    ULONGLONG NonParkedSet;                                                 //0x50
    ULONGLONG ParkRequestSet;                                               //0x58
    ULONGLONG SoftParkRequestSet;                                           //0x60
    ULONGLONG NonIsrTargetedSet;                                            //0x68
    LONG ParkLock;                                                          //0x70
    UCHAR ProcessSeed;                                                      //0x74
    UCHAR Spare5[3];                                                        //0x75
    union
    {
        struct _GROUP_AFFINITY Affinity;                                    //0x80
        struct
        {
            UCHAR AffinityFill[10];                                         //0x80
            USHORT ParentNodeNumber;                                        //0x8a
            USHORT SubNodeNumber;                                           //0x8c
            USHORT Spare;                                                   //0x8e
        };
    };
    ULONGLONG SiblingMask;                                                  //0x90
    ULONGLONG SharedReadyQueueMask;                                         //0x98
    ULONGLONG StrideMask;                                                   //0xa0
    ULONGLONG LLCLeaders;                                                   //0xa8
    ULONG Lowest;                                                           //0xb0
    ULONG Highest;                                                          //0xb4
    struct _flags Flags;                                                    //0xb8
    UCHAR WorkloadClasses;                                                  //0xb9
    struct _KHETERO_PROCESSOR_SET* HeteroSets;                              //0xc0
    ULONGLONG PpmConfiguredQosSets[7];                                      //0xc8
    union _KQOS_GROUPING_SETS QosGroupingSets;                              //0x100
    UCHAR SoftParkRanks[64];                                                //0x140
};

//0x4 bytes (sizeof)
union _KPRCBFLAG
{
    volatile LONG PrcbFlags;                                                //0x0
    ULONG BamQosLevel:8;                                                    //0x0
    ULONG PendingQosUpdate:2;                                               //0x0
    ULONG CacheIsolationEnabled:1;                                          //0x0
    ULONG TracepointActive:1;                                               //0x0
    ULONG LongDpcRunning:1;                                                 //0x0
    ULONG PrcbFlagsReserved:19;                                             //0x0
};

//0x10 bytes (sizeof)
struct _KDESCRIPTOR
{
    USHORT Pad[3];                                                          //0x0
    USHORT Limit;                                                           //0x6
    VOID* Base;                                                             //0x8
};

//0xf0 bytes (sizeof)
struct _KSPECIAL_REGISTERS
{
    ULONGLONG Cr0;                                                          //0x0
    ULONGLONG Cr2;                                                          //0x8
    ULONGLONG Cr3;                                                          //0x10
    ULONGLONG Cr4;                                                          //0x18
    ULONGLONG KernelDr0;                                                    //0x20
    ULONGLONG KernelDr1;                                                    //0x28
    ULONGLONG KernelDr2;                                                    //0x30
    ULONGLONG KernelDr3;                                                    //0x38
    ULONGLONG KernelDr6;                                                    //0x40
    ULONGLONG KernelDr7;                                                    //0x48
    struct _KDESCRIPTOR Gdtr;                                               //0x50
    struct _KDESCRIPTOR Idtr;                                               //0x60
    USHORT Tr;                                                              //0x70
    USHORT Ldtr;                                                            //0x72
    ULONG MxCsr;                                                            //0x74
    ULONGLONG DebugControl;                                                 //0x78
    ULONGLONG LastBranchToRip;                                              //0x80
    ULONGLONG LastBranchFromRip;                                            //0x88
    ULONGLONG LastExceptionToRip;                                           //0x90
    ULONGLONG LastExceptionFromRip;                                         //0x98
    ULONGLONG Cr8;                                                          //0xa0
    ULONGLONG MsrGsBase;                                                    //0xa8
    ULONGLONG MsrGsSwap;                                                    //0xb0
    ULONGLONG MsrStar;                                                      //0xb8
    ULONGLONG MsrLStar;                                                     //0xc0
    ULONGLONG MsrCStar;                                                     //0xc8
    ULONGLONG MsrSyscallMask;                                               //0xd0
    ULONGLONG Xcr0;                                                         //0xd8
    ULONGLONG MsrFsBase;                                                    //0xe0
    ULONGLONG SpecialPadding0;                                              //0xe8
};

//0x4d0 bytes (sizeof)
struct _CONTEXT
{
    ULONGLONG P1Home;                                                       //0x0
    ULONGLONG P2Home;                                                       //0x8
    ULONGLONG P3Home;                                                       //0x10
    ULONGLONG P4Home;                                                       //0x18
    ULONGLONG P5Home;                                                       //0x20
    ULONGLONG P6Home;                                                       //0x28
    ULONG ContextFlags;                                                     //0x30
    ULONG MxCsr;                                                            //0x34
    USHORT SegCs;                                                           //0x38
    USHORT SegDs;                                                           //0x3a
    USHORT SegEs;                                                           //0x3c
    USHORT SegFs;                                                           //0x3e
    USHORT SegGs;                                                           //0x40
    USHORT SegSs;                                                           //0x42
    ULONG EFlags;                                                           //0x44
    ULONGLONG Dr0;                                                          //0x48
    ULONGLONG Dr1;                                                          //0x50
    ULONGLONG Dr2;                                                          //0x58
    ULONGLONG Dr3;                                                          //0x60
    ULONGLONG Dr6;                                                          //0x68
    ULONGLONG Dr7;                                                          //0x70
    ULONGLONG Rax;                                                          //0x78
    ULONGLONG Rcx;                                                          //0x80
    ULONGLONG Rdx;                                                          //0x88
    ULONGLONG Rbx;                                                          //0x90
    ULONGLONG Rsp;                                                          //0x98
    ULONGLONG Rbp;                                                          //0xa0
    ULONGLONG Rsi;                                                          //0xa8
    ULONGLONG Rdi;                                                          //0xb0
    ULONGLONG R8;                                                           //0xb8
    ULONGLONG R9;                                                           //0xc0
    ULONGLONG R10;                                                          //0xc8
    ULONGLONG R11;                                                          //0xd0
    ULONGLONG R12;                                                          //0xd8
    ULONGLONG R13;                                                          //0xe0
    ULONGLONG R14;                                                          //0xe8
    ULONGLONG R15;                                                          //0xf0
    ULONGLONG Rip;                                                          //0xf8
    union
    {
        struct _XSAVE_FORMAT FltSave;                                       //0x100
        struct
        {
            struct _M128A Header[2];                                        //0x100
            struct _M128A Legacy[8];                                        //0x120
            struct _M128A Xmm0;                                             //0x1a0
            struct _M128A Xmm1;                                             //0x1b0
            struct _M128A Xmm2;                                             //0x1c0
            struct _M128A Xmm3;                                             //0x1d0
            struct _M128A Xmm4;                                             //0x1e0
            struct _M128A Xmm5;                                             //0x1f0
            struct _M128A Xmm6;                                             //0x200
            struct _M128A Xmm7;                                             //0x210
            struct _M128A Xmm8;                                             //0x220
            struct _M128A Xmm9;                                             //0x230
            struct _M128A Xmm10;                                            //0x240
            struct _M128A Xmm11;                                            //0x250
            struct _M128A Xmm12;                                            //0x260
            struct _M128A Xmm13;                                            //0x270
            struct _M128A Xmm14;                                            //0x280
            struct _M128A Xmm15;                                            //0x290
        };
    };
    struct _M128A VectorRegister[26];                                       //0x300
    ULONGLONG VectorControl;                                                //0x4a0
    ULONGLONG DebugControl;                                                 //0x4a8
    ULONGLONG LastBranchToRip;                                              //0x4b0
    ULONGLONG LastBranchFromRip;                                            //0x4b8
    ULONGLONG LastExceptionToRip;                                           //0x4c0
    ULONGLONG LastExceptionFromRip;                                         //0x4c8
};

//0x5c0 bytes (sizeof)
struct _KPROCESSOR_STATE
{
    struct _KSPECIAL_REGISTERS SpecialRegisters;                            //0x0
    struct _CONTEXT ContextFrame;                                           //0xf0
};

//0x10 bytes (sizeof)
struct _KSPIN_LOCK_QUEUE
{
    struct _KSPIN_LOCK_QUEUE* volatile Next;                                //0x0
    ULONGLONG* volatile Lock;                                               //0x8
};

//0x10 bytes (sizeof)
union _SLIST_HEADER
{
    struct
    {
        ULONGLONG Alignment;                                                //0x0
        ULONGLONG Region;                                                   //0x8
    };
    struct
    {
        ULONGLONG Depth:16;                                                 //0x0
        ULONGLONG Sequence:48;                                              //0x0
        ULONGLONG Reserved:4;                                               //0x8
        ULONGLONG NextEntry:60;                                             //0x8
    } HeaderX64;                                                            //0x0
};

//0x4 bytes (sizeof)
enum _POOL_TYPE
{
    NonPagedPool = 0,
    NonPagedPoolExecute = 0,
    PagedPool = 1,
    NonPagedPoolMustSucceed = 2,
    DontUseThisType = 3,
    NonPagedPoolCacheAligned = 4,
    PagedPoolCacheAligned = 5,
    NonPagedPoolCacheAlignedMustS = 6,
    MaxPoolType = 7,
    NonPagedPoolBase = 0,
    NonPagedPoolBaseMustSucceed = 2,
    NonPagedPoolBaseCacheAligned = 4,
    NonPagedPoolBaseCacheAlignedMustS = 6,
    NonPagedPoolSession = 32,
    PagedPoolSession = 33,
    NonPagedPoolMustSucceedSession = 34,
    DontUseThisTypeSession = 35,
    NonPagedPoolCacheAlignedSession = 36,
    PagedPoolCacheAlignedSession = 37,
    NonPagedPoolCacheAlignedMustSSession = 38,
    NonPagedPoolNx = 512,
    NonPagedPoolNxCacheAligned = 516,
    NonPagedPoolSessionNx = 544
};

//0x60 bytes (sizeof)
struct _GENERAL_LOOKASIDE_POOL
{
    union
    {
        union _SLIST_HEADER ListHead;                                       //0x0
        struct _SINGLE_LIST_ENTRY SingleListHead;                           //0x0
    };
    USHORT Depth;                                                           //0x10
    USHORT MaximumDepth;                                                    //0x12
    ULONG TotalAllocates;                                                   //0x14
    union
    {
        ULONG AllocateMisses;                                               //0x18
        ULONG AllocateHits;                                                 //0x18
    };
    ULONG TotalFrees;                                                       //0x1c
    union
    {
        ULONG FreeMisses;                                                   //0x20
        ULONG FreeHits;                                                     //0x20
    };
    enum _POOL_TYPE Type;                                                   //0x24
    ULONG Tag;                                                              //0x28
    ULONG Size;                                                             //0x2c
    union
    {
        VOID* (*AllocateEx)(enum _POOL_TYPE arg1, ULONGLONG arg2, ULONG arg3, struct _LOOKASIDE_LIST_EX* arg4); //0x30
        VOID* (*Allocate)(enum _POOL_TYPE arg1, ULONGLONG arg2, ULONG arg3); //0x30
    };
    union
    {
        VOID (*FreeEx)(VOID* arg1, struct _LOOKASIDE_LIST_EX* arg2);        //0x38
        VOID (*Free)(VOID* arg1);                                           //0x38
    };
    struct _LIST_ENTRY ListEntry;                                           //0x40
    ULONG LastTotalAllocates;                                               //0x50
    union
    {
        ULONG LastAllocateMisses;                                           //0x54
        ULONG LastAllocateHits;                                             //0x54
    };
    ULONG Future[2];                                                        //0x58
};

//0x60 bytes (sizeof)
struct _LOOKASIDE_LIST_EX
{
    struct _GENERAL_LOOKASIDE_POOL L;                                       //0x0
};

//0x80 bytes (sizeof)
struct _GENERAL_LOOKASIDE
{
    union
    {
        union _SLIST_HEADER ListHead;                                       //0x0
        struct _SINGLE_LIST_ENTRY SingleListHead;                           //0x0
    };
    USHORT Depth;                                                           //0x10
    USHORT MaximumDepth;                                                    //0x12
    ULONG TotalAllocates;                                                   //0x14
    union
    {
        ULONG AllocateMisses;                                               //0x18
        ULONG AllocateHits;                                                 //0x18
    };
    ULONG TotalFrees;                                                       //0x1c
    union
    {
        ULONG FreeMisses;                                                   //0x20
        ULONG FreeHits;                                                     //0x20
    };
    enum _POOL_TYPE Type;                                                   //0x24
    ULONG Tag;                                                              //0x28
    ULONG Size;                                                             //0x2c
    union
    {
        VOID* (*AllocateEx)(enum _POOL_TYPE arg1, ULONGLONG arg2, ULONG arg3, struct _LOOKASIDE_LIST_EX* arg4); //0x30
        VOID* (*Allocate)(enum _POOL_TYPE arg1, ULONGLONG arg2, ULONG arg3); //0x30
    };
    union
    {
        VOID (*FreeEx)(VOID* arg1, struct _LOOKASIDE_LIST_EX* arg2);        //0x38
        VOID (*Free)(VOID* arg1);                                           //0x38
    };
    struct _LIST_ENTRY ListEntry;                                           //0x40
    ULONG LastTotalAllocates;                                               //0x50
    union
    {
        ULONG LastAllocateMisses;                                           //0x54
        ULONG LastAllocateHits;                                             //0x54
    };
    ULONG Future[2];                                                        //0x58
};

//0x10 bytes (sizeof)
struct _PP_LOOKASIDE_LIST
{
    struct _GENERAL_LOOKASIDE* P;                                           //0x0
    struct _GENERAL_LOOKASIDE* L;                                           //0x8
};



#endif //DUDEDUMPER_STRUCTS_H
