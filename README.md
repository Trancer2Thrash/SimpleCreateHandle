# SimpleCreateHandle
创建简单线程 模拟子线程

```
[Main Thread] id=38236
[Main Thread] child thread created, id=33808
[Child Thread] id=33808, threadIndex=1, round=1, sharedCounter=1
[Child Thread] id=33808, threadIndex=1, round=2, sharedCounter=2
[Child Thread] id=33808, threadIndex=1, round=3, sharedCounter=3
[Child Thread] id=33808, threadIndex=1, round=4, sharedCounter=4
[Child Thread] id=33808, threadIndex=1, round=5, sharedCounter=5
[Main Thread] child thread exited, code=0
[Main Thread] final sharedCounter=5
```
## CreateThread 进程线程示例说明

这个示例演示了如何在 Win32 控制台程序中使用 `CreateThread` 在本进程创建子线程，并理解以下关键点：

- 线程入口函数签名（`DWORD WINAPI ThreadProc(LPVOID)`）
- 参数传递（通过结构体指针传递给子线程）
- 主线程与子线程同步（`WaitForSingleObject`）
- 线程退出码获取（`GetExitCodeThread`）
- 句柄资源释放（`CloseHandle`）

## 文件结构

- `createThread.h`：线程参数结构体与线程入口声明
- `createThread.cpp`：主线程逻辑、子线程逻辑、同步与清理流程

## 代码逻辑流程

1. 主线程启动后打印自己的线程 ID。
2. 主线程初始化共享计数器 `sharedCounter`，并准备 `ThreadParams` 参数。
3. 调用 `CreateThread` 创建子线程，把 `ThreadParams` 地址作为 `LPVOID` 传入。
4. 子线程中把 `LPVOID` 转回 `ThreadParams*`，循环执行任务：
   - 使用 `InterlockedIncrement` 原子增加共享计数器
   - 输出当前轮次和共享计数
   - `Sleep` 模拟耗时工作
5. 主线程调用 `WaitForSingleObject` 阻塞等待子线程结束。
6. 子线程结束后，主线程用 `GetExitCodeThread` 读取退出码。
7. 主线程打印最终共享计数，最后 `CloseHandle` 释放线程句柄。

## 关键 API 说明

- `CreateThread`：在当前进程地址空间创建新线程。
- `WaitForSingleObject`：等待线程对象变为有信号状态（线程退出）。
- `GetExitCodeThread`：获取线程返回码。
- `CloseHandle`：关闭线程对象句柄，避免资源泄露。
- `InterlockedIncrement`：原子自增，避免多线程下共享变量竞态。
