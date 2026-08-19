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
/// @file       thread_monitor.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "thread_monitor.h"

// Windows platform implementation
#ifdef _WIN32
void WindowsThreadMonitor::setThreadName(const ThreadInfo& threadInfo)
{
    const DWORD MS_VC_EXCEPTION = 0x406D1388;
    #pragma pack(push, 8)
    struct THREADNAME_INFO
    {
        DWORD dwType;      // Must be 0x1000
        LPCSTR szName;     // Thread name
        DWORD dwThreadID;  // Thread ID, 0 means current thread
        DWORD dwFlags;     // Reserved, must be 0
    } threadNameInfo;
    #pragma pack(pop)

    threadNameInfo.dwType     = 0x1000;
    threadNameInfo.szName     = threadInfo.name.c_str();
    threadNameInfo.dwThreadID = threadInfo.systemId;
    threadNameInfo.dwFlags    = 0;

    __try {
        RaiseException(MS_VC_EXCEPTION, 0, sizeof(threadNameInfo) / sizeof(ULONG_PTR), (ULONG_PTR*)&threadNameInfo);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
    }
}

unsigned long long WindowsThreadMonitor::getThreadCpuTime(const ThreadInfo& threadInfo)
{
    FILETIME creationTime, exitTime, kernelTime, userTime;
    HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, threadInfo.systemId);
    if (hThread == NULL) {
        return 0;
    }

    if (!GetThreadTimes(hThread, &creationTime, &exitTime, &kernelTime, &userTime)) {
        CloseHandle(hThread);
        return 0;
    }

    CloseHandle(hThread);

    // Convert to microseconds
    ULARGE_INTEGER kernel, user;
    kernel.LowPart  = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart    = userTime.dwLowDateTime;
    user.HighPart   = userTime.dwHighDateTime;

    return kernel.QuadPart / 10 + user.QuadPart / 10;
}

unsigned long long WindowsThreadMonitor::getSystemCpuTime()
{
    FILETIME idleTime, kernelTime, userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return 0;
    }

    ULARGE_INTEGER kernel, user;
    kernel.LowPart  = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;
    user.LowPart    = userTime.dwLowDateTime;
    user.HighPart   = userTime.dwHighDateTime;

    return kernel.QuadPart / 10 + user.QuadPart / 10;
}

MemoryInfo WindowsThreadMonitor::getProcessMemoryInfo()
{
    MemoryInfo info = {0, 0};

    PROCESS_MEMORY_COUNTERS_EX pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc))) {
        info.virtualMemory  = pmc.PrivateUsage;    // Private virtual memory
        info.physicalMemory = pmc.WorkingSetSize;  // Working set size (physical memory)
    }

    return info;
}

void WindowsThreadMonitor::getCurrentThreadId(ThreadInfo& threadInfo) { threadInfo.systemId = GetCurrentThreadId(); }
#endif

// Linux platform implementation
#ifndef _WIN32
void LinuxThreadMonitor::setThreadName(const ThreadInfo& threadInfo)
{
    // Thread name limited to 15 characters
    std::string truncatedName = threadInfo.name.substr(0, 15);
    pthread_setname_np(threadInfo.pthreadId, truncatedName.c_str());
}

unsigned long long LinuxThreadMonitor::getThreadCpuTime(const ThreadInfo& threadInfo)
{
    std::stringstream ss;
    ss << "/proc/" << getpid() << "/task/" << threadInfo.pthreadId << "/stat";
    std::ifstream file(ss.str());

    if (!file.is_open()) {
        return 0;
    }

    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);

    // Fields 14 and 15 in the stat file are user and kernel CPU times (clock ticks)
    unsigned long long utime, stime;
    std::string token;
    int count = 0;

    while (iss >> token) {
        count++;
        if (count == 14) {
            utime = std::stoull(token);
        } else if (count == 15) {
            stime = std::stoull(token);
            break;
        }
    }

    // Convert to microseconds (assuming clock tick is 10ms)
    return (utime + stime) * 10000;
}

unsigned long long LinuxThreadMonitor::getSystemCpuTime()
{
    std::ifstream file("/proc/stat");
    if (!file.is_open()) {
        return 0;
    }

    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);

    std::string token;
    unsigned long long user, nice, system, idle, iowait, irq, softirq;
    iss >> token;  // Skip "cpu"
    iss >> user >> nice >> system >> idle >> iowait >> irq >> softirq;

    // Convert to microseconds (assuming clock tick is 10ms)
    return (user + nice + system + irq + softirq) * 10000;
}

MemoryInfo LinuxThreadMonitor::getProcessMemoryInfo()
{
    MemoryInfo info = {0, 0};

    std::ifstream file("/proc/self/statm");
    if (file.is_open()) {
        unsigned long totalPages, residentPages;
        file >> totalPages >> residentPages;

        long pageSize = sysconf(_SC_PAGESIZE);
        if (pageSize == -1) {
            pageSize = 4096;  // Default page size
        }

        info.virtualMemory  = totalPages * pageSize;
        info.physicalMemory = residentPages * pageSize;
    }

    return info;
}

void LinuxThreadMonitor::getCurrentThreadId(ThreadInfo& threadInfo) { threadInfo.pthreadId = pthread_self(); }
#endif

// ThreadMonitorManager implementation
ThreadMonitorManager::ThreadMonitorManager(int numThreads, const std::string& outputFileName)
    : numThreads(numThreads), outputFileName(outputFileName), running(false)
{
    // Create the corresponding monitor instance based on the platform
#ifdef _WIN32
    monitor = std::make_unique< WindowsThreadMonitor >();
#else
    monitor = std::make_unique< LinuxThreadMonitor >();
#endif

    // Initialize the Markdown file
    std::ofstream file(outputFileName, std::ios::trunc);
    if (file.is_open()) {
        file << "# Thread Monitoring Statistics Report\n\n";
        file << "This report records the CPU usage of each thread and the memory usage of the process.\n\n";
        file.close();
    }
}

ThreadMonitorManager::~ThreadMonitorManager() { stop(); }

void ThreadMonitorManager::start()
{
    if (running)
        return;

    running = true;

    // Create worker threads
    for (int i = 0; i < numThreads; ++i) {
        std::string threadName = "Worker-" + std::to_string(i + 1);
        workerThreads.emplace_back(&ThreadMonitorManager::workerThreadFunc, this, threadName, i);
    }

    // Create monitoring thread
    monitorThread = std::thread(&ThreadMonitorManager::monitorThreadFunc, this);
}

void ThreadMonitorManager::stop()
{
    if (!running)
        return;

    running = false;

    // Wait for all worker threads to end
    for (auto& t : workerThreads) {
        if (t.joinable()) {
            t.join();
        }
    }
    workerThreads.clear();

    // Wait for the monitoring thread to end
    if (monitorThread.joinable()) {
        monitorThread.join();
    }
}

void ThreadMonitorManager::workerThreadFunc(const std::string& name, int threadNum)
{
    // Register thread information
    ThreadInfo info;
    info.threadId = std::this_thread::get_id();
    info.name     = name;
    monitor->getCurrentThreadId(info);

    // Set thread name
    monitor->setThreadName(info);

    // Initialize CPU time tracking
    info.lastCpuCheck = std::chrono::high_resolution_clock::now();
    info.lastCpuTime  = monitor->getThreadCpuTime(info);
    info.cpuUsage     = 0.0;

    {
        std::lock_guard< std::mutex > lock(threadsInfoMutex);
        threadsInfo.push_back(info);
    }

    // Simulate workload
    while (running) {
        // Randomly sleep for a while to simulate different workloads
        int sleepTime = (threadNum % 3 + 1) * 100;  // 100ms, 200ms, 300ms
        std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));

        // Perform some computation tasks
        volatile long long sum = 0;
        for (int i = 0; i < 1000000 * (threadNum + 1); ++i) {
            sum += i;
        }
    }
}

void ThreadMonitorManager::monitorThreadFunc()
{
    unsigned long long lastSystemCpuTime = monitor->getSystemCpuTime();
    auto lastCheckTime                   = std::chrono::high_resolution_clock::now();

    while (running) {
        // Update statistics every second
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // Get current system CPU time and check time
        unsigned long long currentSystemCpuTime = monitor->getSystemCpuTime();
        auto currentCheckTime                   = std::chrono::high_resolution_clock::now();

        // Calculate time interval (microseconds)
        auto duration
            = std::chrono::duration_cast< std::chrono::microseconds >(currentCheckTime - lastCheckTime).count();

        unsigned long long systemCpuDiff = currentSystemCpuTime - lastSystemCpuTime;

        // Update thread CPU usage
        {
            std::lock_guard< std::mutex > lock(threadsInfoMutex);

            for (auto& threadInfo : threadsInfo) {
                unsigned long long currentCpuTime = monitor->getThreadCpuTime(threadInfo);
                unsigned long long cpuDiff        = currentCpuTime - threadInfo.lastCpuTime;

                // Calculate CPU usage (percentage)
                if (duration > 0 && systemCpuDiff > 0) {
                    threadInfo.cpuUsage = static_cast< double >(cpuDiff) / duration * 100.0;
                    // Clamp within 0-100 range
                    threadInfo.cpuUsage = std::max(0.0, std::min(100.0, threadInfo.cpuUsage));
                }

                threadInfo.lastCpuTime  = currentCpuTime;
                threadInfo.lastCpuCheck = currentCheckTime;
            }
        }

        // Get memory usage
        MemoryInfo memInfo = monitor->getProcessMemoryInfo();

        // Print to console
        std::cout << "\n===== Thread Monitoring Statistics =====" << std::endl;
        std::cout << "Time: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
        std::cout << "Process memory usage:" << std::endl;
        std::cout << "  Virtual memory: " << memInfo.virtualMemory / (1024 * 1024) << " MB" << std::endl;
        std::cout << "  Physical memory: " << memInfo.physicalMemory / (1024 * 1024) << " MB" << std::endl;
        std::cout << "Thread CPU usage:" << std::endl;

        {
            std::lock_guard< std::mutex > lock(threadsInfoMutex);
            for (const auto& threadInfo : threadsInfo) {
                std::cout << "  " << threadInfo.name << ": " << std::fixed << std::setprecision(2)
                          << threadInfo.cpuUsage << "%" << std::endl;
            }
        }

        // Write to Markdown file
        writeStatsToFile(memInfo, std::chrono::system_clock::now());

        // Update last check time
        lastSystemCpuTime = currentSystemCpuTime;
        lastCheckTime     = currentCheckTime;
    }
}

void ThreadMonitorManager::writeStatsToFile(const MemoryInfo& memInfo,
                                            const std::chrono::system_clock::time_point& timestamp)
{
    std::ofstream file(outputFileName, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Unable to open output file: " << outputFileName << std::endl;
        return;
    }

    // Write timestamp
    time_t time = std::chrono::system_clock::to_time_t(timestamp);
    file << "## " << ctime(&time) << "\n";

    // Write memory information table
    file << "### Memory Usage\n";
    file << "| Memory Type | Usage (MB) |\n";
    file << "|----------|-------------|\n";
    file << "| Virtual Memory | " << memInfo.virtualMemory / (1024 * 1024) << " |\n";
    file << "| Physical Memory | " << memInfo.physicalMemory / (1024 * 1024) << " |\n\n";

    // Write thread CPU usage table
    file << "### Thread CPU Usage\n";
    file << "| Thread Name | CPU Usage (%) |\n";
    file << "|----------|---------------|\n";

    {
        std::lock_guard< std::mutex > lock(threadsInfoMutex);
        for (const auto& threadInfo : threadsInfo) {
            file << "| " << threadInfo.name << " | " << std::fixed << std::setprecision(2) << threadInfo.cpuUsage
                 << " |\n";
        }
    }

    file << "\n---\n\n";
    file.close();
}
