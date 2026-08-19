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
/// @file       time.cpp
/// @brief
/// @details
/// @date       2025-12-09
/// @author     liyalong
/// @version    1.2.0
///
/// ================================================================

#include "ara/com/internal/time.h"

#include <chrono>
#include <list>
#include <map>

#include "ara/com/internal/log/log.h"
#include "ara/com/internal/runtime.h"
#include "ara/com/internal/threadpool.h"

namespace ara {
namespace com {
namespace internal {
class TimerImpl : public Timer
{
private:
    using ClockType    = std::chrono::high_resolution_clock;
    using PointType    = typename ClockType::time_point;
    using DurationType = typename ClockType::duration;
    class Step
    {
    public:
        int32_t count{};
        DurationType elapsed{};
        DurationType min{};
        DurationType max{};
        PointType now{};
        inline void Record(DurationType duration) noexcept
        {
            ++count;
            elapsed += duration;
            if (count == 1) {
                min = duration;
                max = duration;
            } else if (min > duration) {
                min = duration;
            } else if (max < duration) {
                max = duration;
            }
        }
    };
    class RecorderImpl : public Recorder
    {
    public:
        explicit RecorderImpl(Step& stepIn) noexcept : step{stepIn}, now{ClockType::now()} {}
        ~RecorderImpl() noexcept override { step.Record(ClockType::now() - now); }
        /// @brief Constructor
        RecorderImpl() noexcept = delete;
        /// @brief Copy constructor
        /// @param other
        RecorderImpl(RecorderImpl const& other) noexcept = delete;
        /// @brief Move constructor
        /// @param other
        RecorderImpl(RecorderImpl&& other) noexcept = delete;
        /// @brief Copy assignment operator
        /// @param other
        /// @return RecorderImpl
        RecorderImpl& operator=(RecorderImpl const& other) noexcept = delete;
        /// @brief Move assignment operator
        /// @param other
        /// @return RecorderImpl
        RecorderImpl& operator=(RecorderImpl&& other) noexcept = delete;
        Step& step;
        PointType now;
    };
    std::map< Key, Step > key2step_;
    std::unique_ptr< ThreadPool > const& threadPool_;

public:
    TimerImpl() noexcept : threadPool_{ara::com::internal::GetInstance().CreateThreadPool("timer", 1)} {}
    ~TimerImpl() noexcept override = default;
    /// @brief Copy constructor
    /// @param other
    TimerImpl(TimerImpl const& other) noexcept = delete;
    /// @brief Move constructor
    /// @param other
    TimerImpl(TimerImpl&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return TimerImpl
    TimerImpl& operator=(TimerImpl const& other) noexcept = delete;
    /// @brief Move assignment operator
    /// @param other
    /// @return TimerImpl
    TimerImpl& operator=(TimerImpl&& other) noexcept = default;  // NOLINT
    std::unique_ptr< Recorder > Record(Key const& key) noexcept override
    {
        auto& step{key2step_[key]};
        return std::make_unique< RecorderImpl >(step);
    }
    void Start(Key const& key) noexcept override
    {
        auto& step{key2step_[key]};
        step.now = ClockType::now();
    }
    void Stop(Key const& key) noexcept override
    {
        auto it{key2step_.find(key)};
        if (it == key2step_.end()) {
            return;
        }
        auto& step{it->second};
        step.Record(ClockType::now() - step.now);
    }
    void Report() const noexcept override
    {
        for (auto const& it : key2step_) {
            auto const& key{it.first};
            auto const& step{it.second};
            auto count{step.count};
            auto elapsed{std::chrono::duration< double, std::micro >(step.elapsed).count()};
            auto min{std::chrono::duration< double, std::micro >(step.min).count()};
            auto max{std::chrono::duration< double, std::micro >(step.max).count()};
            auto task{[key, count, elapsed, min, max]() {
                auto avg{elapsed / count};
                constexpr auto kRatio{static_cast< double >(std::micro::den) / std::micro::num};
                auto calls{count * kRatio / elapsed};
                ComLogPlain(key, GenArg(count), GenK2V("elapsed(us)", elapsed), GenK2V("avg(us)", avg),
                            GenK2V("min(us)", min), GenK2V("max(us)", max), GenK2V("calls(Hz/s)", calls));
            }};
            if (threadPool_ == nullptr) {
                task();
                break;
            }
            threadPool_->Dispatch(std::move(task));
        }
    }
    void Reset() noexcept override { key2step_.clear(); }
    size_t Count(Key const& key) const noexcept override
    {
        auto it{key2step_.find(key)};
        if (it == key2step_.end()) {
            return {};
        }
        auto const& step{it->second};
        auto count{step.count};
        return count;
    }
    double Elapsed(Key const& key) const noexcept override
    {
        auto it{key2step_.find(key)};
        if (it == key2step_.end()) {
            return {};
        }
        auto const& step{it->second};
        auto elapsed{std::chrono::duration< double, std::micro >(step.elapsed).count()};
        return elapsed;
    }
    double Calls(Key const& key) const noexcept override
    {
        auto it{key2step_.find(key)};
        if (it == key2step_.end()) {
            return {};
        }
        auto const& step{it->second};
        auto count{step.count};
        auto elapsed{std::chrono::duration< double, std::micro >(step.elapsed).count()};
        constexpr auto kRatio{static_cast< double >(std::micro::den) / std::micro::num};
        auto calls{count * kRatio / elapsed};
        return calls;
    };
};
std::unique_ptr< Timer > Timer::Create() noexcept { return std::make_unique< TimerImpl >(); }
}  // namespace internal
}  // namespace com
}  // namespace ara