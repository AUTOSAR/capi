// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       thread_monitor.h
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef THREAD_MONITOR_H
#define THREAD_MONITOR_H

#include <atomic>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

// Memory information structure
struct MemoryInfo
{
    size_t virtualMemory;   // Virtual memory, unit: bytes
    size_t physicalMemory;  // Physical memory, unit: bytes
};

// Thread information structure
struct ThreadInfo
{
    std::thread::id threadId;
    std::string name;
    std::chrono::high_resolution_clock::time_point lastCpuCheck;
    unsigned long long lastCpuTime;
    double cpuUsage;

    // Platform-specific thread ID
#ifdef _WIN32
    DWORD systemId;  // Windows thread ID
#else
    pthread_t pthreadId;  // Linux thread ID
#endif
};

// Abstract base class (interface)
class ThreadMonitorInterface
{
public:
    virtual ~ThreadMonitorInterface() = default;

    // Set thread name
    virtual void setThreadName(const ThreadInfo& threadInfo) = 0;

    // Get thread CPU time (microseconds)
    virtual unsigned long long getThreadCpuTime(const ThreadInfo& threadInfo) = 0;

    // Get total system CPU time (microseconds)
    virtual unsigned long long getSystemCpuTime() = 0;

    // Get process memory usage
    virtual MemoryInfo getProcessMemoryInfo() = 0;

    // Get the current thread's platform-specific ID
    virtual void getCurrentThreadId(ThreadInfo& threadInfo) = 0;
};

// Windows platform implementation
#ifdef _WIN32
    #include <psapi.h>
    #include <windows.h>
    #pragma comment(lib, "psapi.lib")

class WindowsThreadMonitor : public ThreadMonitorInterface
{
public:
    void setThreadName(const ThreadInfo& threadInfo) override;
    unsigned long long getThreadCpuTime(const ThreadInfo& threadInfo) override;
    unsigned long long getSystemCpuTime() override;
    MemoryInfo getProcessMemoryInfo() override;
    void getCurrentThreadId(ThreadInfo& threadInfo) override;
};
#endif

// Linux platform implementation
#ifndef _WIN32
    #include <pthread.h>
    #include <sys/sysinfo.h>
    #include <unistd.h>

    #include <fstream>
    #include <sstream>

class LinuxThreadMonitor : public ThreadMonitorInterface
{
public:
    void setThreadName(const ThreadInfo& threadInfo) override;
    unsigned long long getThreadCpuTime(const ThreadInfo& threadInfo) override;
    unsigned long long getSystemCpuTime() override;
    MemoryInfo getProcessMemoryInfo() override;
    void getCurrentThreadId(ThreadInfo& threadInfo) override;
};
#endif

// Thread monitoring manager
class ThreadMonitorManager
{
private:
    std::unique_ptr< ThreadMonitorInterface > monitor;
    std::vector< ThreadInfo > threadsInfo;
    std::vector< std::thread > workerThreads;
    std::thread monitorThread;
    std::mutex threadsInfoMutex;
    std::atomic< bool > running;
    std::string outputFileName;
    int numThreads;

    // Worker thread function
    void workerThreadFunc(const std::string& name, int threadNum);

    // Monitoring thread function
    void monitorThreadFunc();

    // Write statistics to a Markdown file
    void writeStatsToFile(const MemoryInfo& memInfo, const std::chrono::system_clock::time_point& timestamp);

public:
    // Constructor
    ThreadMonitorManager(int numThreads, const std::string& outputFileName = "thread_stats.md");

    // Destructor
    ~ThreadMonitorManager();

    // Start monitoring
    void start();

    // Stop monitoring
    void stop();
};

#endif  // THREAD_MONITOR_H
