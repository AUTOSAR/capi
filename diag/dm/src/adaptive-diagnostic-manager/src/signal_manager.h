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
/// @file       signal_manager.h
/// @brief      This file provides the Signal Management class
/// @details
/// @date       2022-07-21
/// @author     gaohuiming
/// @version    1.2.0
///
/// ================================================================

#ifndef ARA_DIAG_COMMON_SIGNAL_MANAGER_H_
#define ARA_DIAG_COMMON_SIGNAL_MANAGER_H_
#include <ara/core/map.h>
#include <nai/io/nai_signal.h>

#include <cstdint>
#include <functional>
#include <memory>

namespace ara {
namespace diag {
namespace dmd {

/// @brief SignalManager
class SignalManager
{
    /// @brief SignalManager
    struct SignalHandler final : public nai_signal_t
    {
        /// @brief Set number
        /// @param[in] number
        /// @param[in] number
        void SetNumber(std::int32_t const number) noexcept { number_ = number; }
        /// @brief Set processor
        /// @param[in] processor
        /// @param[in] processor
        void SetProcessor(std::function< void(std::int32_t) > const& processor) noexcept { processor_ = processor; }

        /// @brief Get number
        /// @return number
        std::int32_t GetNumber() const noexcept { return number_; }
        /// @brief Execute processor
        /// @return processor
        void ExecProcessor() const noexcept { processor_(number_); }

    private:
        /// @brief number
        std::int32_t number_{};
        /// @brief processor
        std::function< void(std::int32_t) > processor_;
    };

    /// @brief handlerTable_
    ara::core::Map< std::int32_t, std::shared_ptr< SignalHandler > > handlerTable_;

    /// @brief Signal processing
    /// @param[in] node Signal object
    /// @param[in] events Event value
    /// @return Processing result
    static std::int32_t Process(nai_signal_t* const node, std::int32_t const events) noexcept;

    /// @brief Success code for registering signal processing callback
    std::int32_t const kRegisterCodeSuccess{0};
    /// @brief Failure code for registering signal processing callback
    std::int32_t const kRegisterCodeFail_1{-1};
    /// @brief Failure code for registering signal processing callback
    std::int32_t const kRegisterCodeFail_2{-2};

public:
    /// @brief Copy constructor is prohibited
    /// @param[in]  other
    SignalManager(SignalManager const& other) = delete;

    /// @brief Copy assignment operator is prohibited
    /// @param[in]  right
    /// @return Reference
    SignalManager& operator=(SignalManager const& right) = delete;

    /// @brief Move constructor is prohibited
    /// @param[in] right
    SignalManager(SignalManager&& right) = delete;

    /// @brief Move assignment operator is prohibited
    /// @param[in] right
    /// @return Reference
    SignalManager& operator=(SignalManager&& right) = delete;

    /// @brief Constructor
    SignalManager() noexcept = default;

    /// @brief Destructor
    virtual ~SignalManager() noexcept;

    /// @brief Register signal
    /// @param[in] number Signal value
    /// @param[in] processor Callback function
    /// @return Registration result code
    std::int32_t Register(std::int32_t number, std::function< void(std::int32_t) >&& processor) noexcept;

    void Close();
};

}  // namespace dmd
}  // namespace diag
}  // namespace ara

#endif  // ARA_DIAG_COMMON_SIGNAL_MANAGER_H_