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
/// @file       determined_executor.h
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_DETERMINED_EXECUTOR_H__
#define __LOG_INTERNAL_DETERMINED_EXECUTOR_H__

#include <cstddef>
#include <cstdint>
#include <memory>

#include "ara/log/common.h"

namespace ara {
namespace log {
namespace internal {

struct LogMetaData;
class IlogSinker;
class LocalPool;
class ThreadPool;

class DeterminedExecutor final
{
public:
    explicit DeterminedExecutor(std::shared_ptr< LocalPool > localPool,
                                ThreadPool *threadPool,
                                bool toFile,
                                bool toRemote,
                                std::shared_ptr< IlogSinker > const *fileSinker) noexcept;

    bool TryAllocateExternalBuffer(std::size_t defaultExternalSize,
                                   std::uint8_t *&externalBuffer,
                                   std::size_t &externalCapacity) noexcept;

    bool SubmitPlain(LogLevel level,
                     std::uint32_t msgCount,
                     std::uint8_t const *bodyPtr,
                     std::uint32_t bodySize) noexcept;

    bool SubmitEncoded(LogLevel level,
                       std::uint32_t msgCount,
                       std::uint8_t argsNum,
                       std::uint32_t dltIntTime,
                       std::uint8_t const *headerPtr,
                       std::uint32_t headerSize,
                       std::uint8_t const *bodyPtr,
                       std::uint32_t bodySize) noexcept;

    bool SubmitModeled(std::uint32_t dltIntTime,
                       std::uint8_t const *headerPtr,
                       std::uint32_t headerSize,
                       std::uint8_t const *bodyPtr,
                       std::uint32_t bodySize,
                       bool toFile,
                       bool toRemote) noexcept;

private:
    std::shared_ptr< LocalPool > localPool_{nullptr};
    ThreadPool *threadPool_{nullptr};
    bool toFile_{false};
    bool toRemote_{false};
    std::shared_ptr< IlogSinker > const *fileSinker_{nullptr};
};

}  // namespace internal
}  // namespace log
}  // namespace ara

#endif