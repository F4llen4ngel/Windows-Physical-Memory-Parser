//
// Created by vanya on 6/9/2023.
//

#ifndef DUDEDUMPER_STRUCTS_H
#define DUDEDUMPER_STRUCTS_H

#define LONG long
#define UCHAR unsigned char
#define ULONG unsigned long
#define ULONGLONG unsigned long long
#define USHORT unsigned short
#define LONGLONG long long
#define VOID void
#define CHAR char


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


#endif //DUDEDUMPER_STRUCTS_H
