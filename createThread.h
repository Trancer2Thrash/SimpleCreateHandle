#pragma once

#include <Windows.h>

// 传递给子线程的参数结构体。
// 把业务参数和共享状态打包后，一次性通过 LPVOID 传入线程入口函数。
struct ThreadParams {
    int threadIndex;
    int iterations;
    DWORD delayMs;
    LONG* sharedCounter;
};

// CreateThread 线程入口函数的标准签名。
DWORD WINAPI WorkerThreadProc(LPVOID lpParam);
