
#pragma once

#include <Common.h>

typedef s32 IOSCommand;

typedef enum  {
    IPC_OK = 0,
    IPC_EBUSY = -2,
    IPC_EINVAL = -4,
    IPC_ENOENT = -6,
    IPC_EQUEUEFULL = -8,
    IPC_ENOMEM = -22,
} IPCResult;

typedef struct {
    void *data;
    u32 size;
} IOVector;

s32 IOS_Open(const char *path, u32 flags);
s32 IOS_Close(s32 handle);
s32 IOS_Ioctl(s32 handle, IOSCommand command, void *input, u32 inputSize, void *output,
        u32 outputSize);
s32 IOS_Ioctlv(
        s32 handle, IOSCommand command, u32 inputCount, u32 outputCount, IOVector *vec);
