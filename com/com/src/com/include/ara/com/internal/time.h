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
/// @file       time.h
/// @brief
/// @details
/// @date       2025-12-09
/// @author     liyalong
/// @version    1.2.0
///
/// ================================================================

#pragma once

#ifdef HAS_COM_TIMER
    #include <memory>

    #include "ara/core/string_view.h"

    #define TimeNM                   ::ara::com::internal
    #define TimeInstance             TimeNM::Timer::GetInstance
    #define TimeRecord(scene, key)   TimeInstance< scene >().Record(key)
    #define TimeRecorder(scene, key) auto __recorder(TimeRecord(scene, key))
    #define TimeStart(scene, key)    TimeInstance< scene >().Start(key)
    #define TimeStop(scene, key)     TimeInstance< scene >().Stop(key)
    #define TimeReport(scene)        TimeInstance< scene >().Report()
    #define TimeReset(scene)         TimeInstance< scene >().Reset()
    #define TimeCount(scene, key)    TimeInstance< scene >().Count(key)
    #define TimeElapsed(scene, key)  TimeInstance< scene >().Elapsed(key)
    #define TimeCalls(scene, key)    TimeInstance< scene >().Calls(key)

namespace ara {
namespace com {
namespace internal {
class Timer
{
public:
    template < typename Scene = Timer >
    static Timer& GetInstance() noexcept
    {
        static auto s_PInstance{Create()};
        static auto& s_Instance{*s_PInstance};
        return s_Instance;
    }
    static std::unique_ptr< Timer > Create() noexcept;
    virtual ~Timer() noexcept = default;

public:
    /// @brief Constructor
    Timer() noexcept = default;
    /// @brief Copy constructor
    /// @param other
    Timer(Timer const& other) noexcept = default;
    /// @brief Move constructor
    /// @param other
    Timer(Timer&& other) noexcept = default;
    /// @brief Copy assignment operator
    /// @param other
    /// @return Timer
    Timer& operator=(Timer const& other) noexcept = default;
    /// @brief Move assignment operator
    /// @param other
    /// @return Timer
    Timer& operator=(Timer&& other) noexcept = default;

public:
    using Key = ara::core::StringView;
    class Recorder
    {
    public:
        virtual ~Recorder() noexcept = default;

    public:
        /// @brief Constructor
        Recorder() noexcept = default;
        /// @brief Copy constructor
        /// @param other
        Recorder(Recorder const& other) noexcept = default;
        /// @brief Move constructor
        /// @param other
        Recorder(Recorder&& other) noexcept = default;
        /// @brief Copy assignment operator
        /// @param other
        /// @return Recorder
        Recorder& operator=(Recorder const& other) noexcept = default;
        /// @brief Move assignment operator
        /// @param other
        /// @return Recorder
        Recorder& operator=(Recorder&& other) noexcept = default;
    };
    virtual std::unique_ptr< Recorder > Record(Key const& key) noexcept = 0;
    virtual void Start(Key const& key) noexcept                         = 0;
    virtual void Stop(Key const& key) noexcept                          = 0;
    virtual void Report() const noexcept                                = 0;
    virtual void Reset() noexcept                                       = 0;
    virtual size_t Count(Key const& key) const noexcept                 = 0;
    virtual double Elapsed(Key const& key) const noexcept               = 0;
    virtual double Calls(Key const& key) const noexcept                 = 0;
};
}  // namespace internal
}  // namespace com
}  // namespace ara
#else
    #define TimeNM
    #define TimeInstance
    #define TimeRecord(scene, key) 0
    #define TimeRecorder(scene, key)
    #define TimeStart(scene, key)
    #define TimeStop(scene, key)
    #define TimeReport(scene)
    #define TimeReset(scene)
    #define TimeCount(scene, key) 1
    #define TimeElapsed(scene, key)
    #define TimeCalls(scene, key)
#endif