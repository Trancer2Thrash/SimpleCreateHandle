#include "createThread.h"

#include <iostream>

DWORD WINAPI WorkerThreadProc(LPVOID lpParam)
{
    // 1) 将通用指针还原为我们定义的参数结构体。
    ThreadParams* params = static_cast<ThreadParams*>(lpParam);
    if (params == nullptr) {
        return 1;
    }

    // 2) 执行循环任务，Sleep 用来模拟子线程中的耗时工作。
    for (int i = 1; i <= params->iterations; ++i) {
        // 3) 对共享计数器做原子自增，避免并发场景下的数据竞争。
        LONG currentValue = InterlockedIncrement(params->sharedCounter);

        std::cout
            << "[Child Thread] id=" << GetCurrentThreadId()
            << ", threadIndex=" << params->threadIndex
            << ", round=" << i
            << ", sharedCounter=" << currentValue
            << '\n';

        Sleep(params->delayMs);
    }

    // 4) 返回线程退出码（主线程可通过 GetExitCodeThread 获取）。
    return 0;
}

int wmain()
{
    std::cout << "[Main Thread] id=" << GetCurrentThreadId() << '\n';

    LONG sharedCounter = 0;

    // 5) 在主线程中准备传递给子线程的参数对象。
    ThreadParams params{};
    params.threadIndex = 1;
    params.iterations = 5;
    params.delayMs = 400;
    params.sharedCounter = &sharedCounter;

    DWORD threadId = 0;

    // 6) 在本进程中创建子线程。
    //    - 第1个参数: 默认安全属性
    //    - 第2个参数: 使用默认栈大小
    //    - 第3个参数: 线程入口函数
    //    - 第4个参数: 传入线程入口的参数
    //    - 第5个参数: 0 表示创建后立即运行
    //    - 第6个参数: 返回线程 ID
    HANDLE hThread = CreateThread(
        nullptr,
        0,
        WorkerThreadProc,
        &params,
        0,
        &threadId
    );

    if (hThread == nullptr) {
        std::cerr << "CreateThread failed, error=" << GetLastError() << '\n';
        return 1;
    }

    std::cout << "[Main Thread] child thread created, id=" << threadId << '\n';

    // 7) 线程同步：主线程阻塞等待子线程执行结束。
    DWORD waitResult = WaitForSingleObject(hThread, INFINITE);
    if (waitResult != WAIT_OBJECT_0) {
        std::cerr << "WaitForSingleObject failed, error=" << GetLastError() << '\n';
        CloseHandle(hThread);
        return 1;
    }

    DWORD exitCode = 0;
    if (!GetExitCodeThread(hThread, &exitCode)) {
        std::cerr << "GetExitCodeThread failed, error=" << GetLastError() << '\n';
        CloseHandle(hThread);
        return 1;
    }

    std::cout << "[Main Thread] child thread exited, code=" << exitCode << '\n';
    std::cout << "[Main Thread] final sharedCounter=" << sharedCounter << '\n';

    // 8) 清理线程句柄，避免资源泄漏。
    CloseHandle(hThread);
    return 0;
}
/*
[Main Thread] id=38236
[Main Thread] child thread created, id=33808
[Child Thread] id=33808, threadIndex=1, round=1, sharedCounter=1
[Child Thread] id=33808, threadIndex=1, round=2, sharedCounter=2
[Child Thread] id=33808, threadIndex=1, round=3, sharedCounter=3
[Child Thread] id=33808, threadIndex=1, round=4, sharedCounter=4
[Child Thread] id=33808, threadIndex=1, round=5, sharedCounter=5
[Main Thread] child thread exited, code=0
[Main Thread] final sharedCounter=5
*/