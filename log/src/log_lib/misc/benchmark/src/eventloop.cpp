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
/// @file       eventloop.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "eventloop.h"

HighPrecisionEventLoop::HighPrecisionEventLoop() : epollFd_(epoll_create1(EPOLL_CLOEXEC))
{
    if (epollFd_ < 0) {
        perror("epoll_create1 failed");
        throw std::runtime_error("Failed to create epoll file descriptor");
    }
}

HighPrecisionEventLoop::~HighPrecisionEventLoop()
{
    close(epollFd_);
    // Close all timer file descriptors
    for (const auto &pair : timerFds_) {
        close(pair.first);
    }
}

void HighPrecisionEventLoop::run()
{
    running_ = true;
    std::vector< epoll_event > events(16);  // Initial event list size

    while (running_) {
        // Block and wait for events, does not consume CPU
        int numEvents = epoll_wait(epollFd_, events.data(), events.size(), -1);

        if (numEvents < 0) {
            if (errno == EINTR)
                continue;  // Interrupted by signal, continue waiting
            perror("epoll_wait failed");
            break;
        }

        // Handle all events that occurred
        for (int i = 0; i < numEvents; ++i) {
            int fd = events[i].data.fd;

            // Check if it is a timer event
            if (timerFds_.count(fd)) {
                handleTimerEvent(fd);
            }
            // Other types of event handling can be added here (e.g., network I/O)
        }

        // If the number of events reaches the current capacity, expand
        if (numEvents == events.size()) {
            events.resize(events.size() * 2);
        }
    }
}

void HighPrecisionEventLoop::stop() { running_ = false; }

int HighPrecisionEventLoop::addTimerUs(uint64_t delayUs, TimerCallback callback)
{
    return addTimerImpl(delayUs, 0, std::move(callback));
}

int HighPrecisionEventLoop::addPeriodicTimerUs(uint64_t intervalUs, TimerCallback callback)
{
    return addTimerImpl(intervalUs, intervalUs, std::move(callback));
}

bool HighPrecisionEventLoop::removeTimer(int timerId)
{
    for (auto it = timerFds_.begin(); it != timerFds_.end(); ++it) {
        if (it->second.id == timerId) {
            close(it->first);
            epoll_ctl(epollFd_, EPOLL_CTL_DEL, it->first, nullptr);
            timerFds_.erase(it);
            return true;
        }
    }
    return false;
}

std::string HighPrecisionEventLoop::getCurrentTimeString()
{
    auto now    = std::chrono::system_clock::now();
    auto now_us = std::chrono::time_point_cast< std::chrono::microseconds >(now);
    auto epoch  = now_us.time_since_epoch();
    uint64_t us = std::chrono::duration_cast< std::chrono::microseconds >(epoch).count() % 1000000;

    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    std::tm *tm          = std::localtime(&now_time);
    if (!tm) {
        return "Invalid time";
    }

    std::stringstream ss;
    ss << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << "." << std::setw(6) << std::setfill('0') << us;
    return ss.str();
}

int HighPrecisionEventLoop::addTimerImpl(uint64_t initialUs, uint64_t intervalUs, TimerCallback callback)
{
    // Create a timer file descriptor, using MONOTONIC clock to ensure monotonic time
    int tfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if (tfd < 0) {
        perror("timerfd_create failed");
        return -1;
    }

    // Set timer, accurate to nanoseconds
    struct itimerspec ts
    {
    };

    // Convert initial delay time to seconds and nanoseconds
    ts.it_value.tv_sec  = initialUs / 1000000;
    ts.it_value.tv_nsec = (initialUs % 1000000) * 1000;  // microseconds to nanoseconds

    // If it is a periodic timer, set the interval
    if (intervalUs > 0) {
        ts.it_interval.tv_sec  = intervalUs / 1000000;
        ts.it_interval.tv_nsec = (intervalUs % 1000000) * 1000;  // microseconds to nanoseconds
    }

    // Set the timer
    if (timerfd_settime(tfd, 0, &ts, nullptr) < 0) {
        perror("timerfd_settime failed");
        close(tfd);
        return -1;
    }

    // Add the timer to epoll monitoring
    struct epoll_event event
    {
    };
    event.data.fd = tfd;
    event.events  = EPOLLIN;  // Read event, timer timeout triggers a read event

    if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, tfd, &event) < 0) {
        perror("epoll_ctl add failed");
        close(tfd);
        return -1;
    }

    // Save timer information
    int timerId    = nextTimerId_++;
    timerFds_[tfd] = {timerId, std::move(callback), intervalUs > 0};

    return timerId;
}

void HighPrecisionEventLoop::handleTimerEvent(int fd)
{
    // Read the timer count (must read, otherwise the event will keep triggering)
    uint64_t expirations;
    ssize_t n = read(fd, &expirations, sizeof(expirations));
    if (n != sizeof(expirations)) {
        perror("read timerfd failed");
        return;
    }

    // Find timer information and execute callback
    auto it = timerFds_.find(fd);
    if (it != timerFds_.end()) {
        // Execute callback
        it->second.callback();

        // If it is a one-shot timer, remove it after execution
        if (!it->second.periodic) {
            close(fd);
            epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, nullptr);
            timerFds_.erase(it);
        }
    }
}
