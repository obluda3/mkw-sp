#pragma once

#include "revolution/gx/GXStruct.h"

#ifdef RVL_OS_NEEDS_IMPORT
#undef RVL_OS_NEEDS_IMPORT
#define RVL_OS_NEEDS_IMPORT
#endif

void *OSAllocFromMEM1ArenaLo(u32 size, u32 align);

#define OSRoundUp32B(x) (((u32)(x) + 32 - 1) & ~(32 - 1))
#define OSRoundDown32B(x) (((u32)(x)) & ~(32 - 1))

void OSInit(void);

typedef s64 OSTime;

#define OS_BUS_CLOCK (*(u32 *)0x800000f8)
#define OS_TIMER_CLOCK (OS_BUS_CLOCK / 4)

#define OSMillisecondsToTicks(msec) ((msec) * (OS_TIMER_CLOCK / 1000))
#define OSTicksToSeconds(ticks) ((ticks) / OS_TIMER_CLOCK)
#define OSTicksToNanoseconds(ticks) ((ticks) * (1000000000 / OS_TIMER_CLOCK))
#define OSTicksToMilliseconds(ticks) ((ticks) / (OS_TIMER_CLOCK / 1000))

u32 OSGetTick(void);

void OSReport(const char *msg, ...);
void OSFatal(GXColor fg, GXColor bg, const char *msg);

#include "revolution/os/OSCache.h"
#include "revolution/os/OSMemory.h"
#include "revolution/os/OSModule.h"
#include "revolution/os/OSMutex.h"
#include "revolution/os/OSThread.h"
