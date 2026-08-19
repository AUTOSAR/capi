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
/// @file       eventloop.h
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef HIGH_PRECISION_EVENT_LOOP_H
#define HIGH_PRECISION_EVENT_LOOP_H

#include <sys/epoll.h>
#include <sys/timerfd.h>
#include <unistd.h>

#include <chrono>
#include <cstring>
#include <ctime>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>
// High-precision event loop class, supports microsecond-level timers
class HighPrecisionEventLoop
{
public:
    using TimerCallback = std::function< void() >;
    using TimePoint     = std::chrono::system_clock::time_point;

    HighPrecisionEventLoop();
    ~HighPrecisionEventLoop();

    // Disable copy construction and assignment
    HighPrecisionEventLoop(const HighPrecisionEventLoop &) = delete;
    HighPrecisionEventLoop &operator=(const HighPrecisionEventLoop &) = delete;

    // Start the event loop
    void run();

    // Stop the event loop
    void stop();

    // Add a one-shot timer (microsecond precision)
    // delayUs: Delay time (microseconds)
    // callback: Callback function when the timer triggers
    int addTimerUs(uint64_t delayUs, TimerCallback callback);

    // Add a periodic timer (microsecond precision)
    // intervalUs: Interval time (microseconds)
    // callback: Callback function when the timer triggers
    int addPeriodicTimerUs(uint64_t intervalUs, TimerCallback callback);

    // Remove a timer
    bool removeTimer(int timerId);

    // Get the current time as a string (including microseconds)
    static std::string getCurrentTimeString();

private:
    // Timer information structure
    struct TimerInfo
    {
        int id;
        TimerCallback callback;
        bool periodic;
    };

    // Timer implementation (microsecond level)
    int addTimerImpl(uint64_t initialUs, uint64_t intervalUs, TimerCallback callback);

    // Handle timer events
    void handleTimerEvent(int fd);

    int epollFd_;                                    // epoll file descriptor
    bool running_    = false;                        // Whether the event loop is running
    int nextTimerId_ = 1;                            // Next timer ID
    std::unordered_map< int, TimerInfo > timerFds_;  // Timer file descriptor mapping
};

#endif  // HIGH_PRECISION_EVENT_LOOP_H
