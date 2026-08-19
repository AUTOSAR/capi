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
/// @file       determined_executor.cpp
/// @brief
/// @details
/// @date       2025-09-03
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "determined_executor.h"

#include <algorithm>
#include <chrono>
#include <cstring>

#include "DltLogDerm/memory_pool.h"
#include "DltLogDerm/thread_pool.h"
#include "determ_log_meta_data.h"

namespace ara {
namespace log {
namespace internal {

DeterminedExecutor::DeterminedExecutor(std::shared_ptr< LocalPool > localPool,
                                       ThreadPool *threadPool,
                                       bool toFile,
                                       bool toRemote,
                                       std::shared_ptr< IlogSinker > const *fileSinker) noexcept
    : localPool_(std::move(localPool))
    , threadPool_(threadPool)
    , toFile_(toFile)
    , toRemote_(toRemote)
    , fileSinker_(fileSinker)
{
}

bool DeterminedExecutor::TryAllocateExternalBuffer(std::size_t defaultExternalSize,
                                                   std::uint8_t *&externalBuffer,
                                                   std::size_t &externalCapacity) noexcept
{
    externalBuffer   = nullptr;
    externalCapacity = 0U;

    if ((localPool_ == nullptr) || (threadPool_ == nullptr) || (defaultExternalSize == 0U)) {
        return false;
    }

    externalCapacity = (localPool_->BlockSize() < defaultExternalSize) ? localPool_->BlockSize() : defaultExternalSize;
    if (externalCapacity == 0U) {
        return false;
    }

    externalBuffer = localPool_->Allocate(externalCapacity);
    return (externalBuffer != nullptr);
}

bool DeterminedExecutor::SubmitPlain(LogLevel level,
                                     std::uint32_t msgCount,
                                     std::uint8_t const *bodyPtr,
                                     std::uint32_t bodySize) noexcept
{
    if ((localPool_ == nullptr) || (threadPool_ == nullptr)) {
        return false;
    }

    std::uint8_t *memPtr{localPool_->Allocate()};
    if (memPtr == nullptr) {
        threadPool_->PrintTaskCount();
        return false;
    }

    LogMetaData *logData{new (memPtr) LogMetaData()};
    logData->toConsle = true;
    logData->logLevel = level;
    logData->nowTime  = std::chrono::system_clock::now().time_since_epoch().count();
    logData->msgCount = static_cast< std::uint8_t >(msgCount & 0xFFU);

    if ((bodyPtr != nullptr) && (bodySize > 0U)) {
        std::size_t const maxLen{internal::kLogBufferSize - 1U};
        std::size_t const copyLen{(std::min)(static_cast< std::size_t >(bodySize), maxLen)};
        std::memcpy(logData->buffer.data(), bodyPtr, copyLen);
        logData->buffer[copyLen] = '\0';
        logData->size            = copyLen;
    }

    if (true == threadPool_->IsNetworkThreadWorking()) {
        threadPool_->AddNetworkTask(logData);
    } else {
        threadPool_->AddTask(logData);
    }
    localPool_->RefillMemory(1);
    return true;
}

bool DeterminedExecutor::SubmitEncoded(LogLevel level,
                                       std::uint32_t msgCount,
                                       std::uint8_t argsNum,
                                       std::uint32_t dltIntTime,
                                       std::uint8_t const *headerPtr,
                                       std::uint32_t headerSize,
                                       std::uint8_t const *bodyPtr,
                                       std::uint32_t bodySize) noexcept
{
    if ((localPool_ == nullptr) || (threadPool_ == nullptr) || (fileSinker_ == nullptr)) {
        return false;
    }

    std::uint8_t *memPtr{localPool_->Allocate()};
    if (memPtr == nullptr) {
        threadPool_->PrintTaskCount();
        return false;
    }

    LogMetaData *logData{new (memPtr) LogMetaData()};

    logData->toFile        = toFile_ && ((*fileSinker_) != nullptr);
    logData->toRemote      = toRemote_;
    logData->rawFileSinker = ((*fileSinker_) != nullptr) ? (*fileSinker_).get() : nullptr;

    logData->logLevel  = level;
    logData->nowTime   = std::chrono::system_clock::now().time_since_epoch().count();
    logData->msgCount  = static_cast< std::uint8_t >(msgCount & 0xFFU);
    logData->timeStamp = dltIntTime;
    logData->argsNum   = argsNum;

    if ((headerPtr != nullptr) && (headerSize > 0U) && (headerSize <= sizeof(logData->message.headerbuffer))) {
        std::memcpy(logData->message.headerbuffer, headerPtr, headerSize);
        logData->message.standardheader
            = reinterpret_cast< internal::DltStandardHeader * >(logData->message.headerbuffer);
        logData->message.headersize = headerSize;
    }

    if ((bodyPtr != nullptr) && (bodySize > 0U)) {
        std::size_t const copySize{(bodySize > internal::kLogBufferSize) ? internal::kLogBufferSize : bodySize};
        std::memcpy(logData->buffer.data(), bodyPtr, copySize);
        logData->size = copySize;
    }

    if ((!logData->toFile && !logData->toRemote) || (logData->argsNum == 0U) || (logData->message.headersize == 0U)) {
        localPool_->FreeBlock(reinterpret_cast< std::uint8_t * >(logData));
        return false;
    }
    // The deterministic encode buffer is allocated from the memory pool and needs to be released
    localPool_->FreeBlock(const_cast< uint8_t * >(headerPtr));
#ifdef ARA_LOG_HAS_REMOTE
    if ((logData->toRemote) && (true == threadPool_->IsNetworkThreadWorking())) {
        threadPool_->AddNetworkTask(logData);
        localPool_->RefillMemory(1);
    } else
#endif
    {
        threadPool_->AddTask(logData);
        localPool_->RefillMemory(1);
    }

    return true;
}

bool DeterminedExecutor::SubmitModeled(std::uint32_t dltIntTime,
                                       std::uint8_t const *headerPtr,
                                       std::uint32_t headerSize,
                                       std::uint8_t const *bodyPtr,
                                       std::uint32_t bodySize,
                                       bool toFile,
                                       bool toRemote) noexcept
{
    if ((localPool_ == nullptr) || (threadPool_ == nullptr) || (fileSinker_ == nullptr)) {
        return false;
    }

    std::uint8_t *memPtr{localPool_->Allocate()};
    if (nullptr == memPtr) {
        threadPool_->PrintTaskCount();
        return false;
    }

    LogMetaData *dataPtr{new (memPtr) LogMetaData()};
    dataPtr->toFile        = toFile;
    dataPtr->toRemote      = toRemote;
    dataPtr->rawFileSinker = ((*fileSinker_) != nullptr) ? (*fileSinker_).get() : nullptr;

    dataPtr->timeStamp = dltIntTime;
    dataPtr->nowTime   = std::chrono::system_clock::now().time_since_epoch().count();

    if ((headerPtr != nullptr) && (headerSize > 0U) && (headerSize <= sizeof(dataPtr->message.headerbuffer))) {
        std::memcpy(dataPtr->message.headerbuffer, headerPtr, headerSize);
        dataPtr->message.standardheader
            = reinterpret_cast< internal::DltStandardHeader * >(dataPtr->message.headerbuffer);
        dataPtr->message.headersize = headerSize;
    }

    if ((bodyPtr != nullptr) && (bodySize > 0U)) {
        std::size_t const copySize{(bodySize > internal::kLogBufferSize) ? internal::kLogBufferSize : bodySize};
        std::memcpy(dataPtr->buffer.data(), bodyPtr, copySize);
        dataPtr->size = copySize;
    }
    // The deterministic encode buffer is allocated from the memory pool and needs to be released
    localPool_->FreeBlock(const_cast< uint8_t * >(headerPtr));
    if (dataPtr->message.headersize == 0U) {
        localPool_->FreeBlock(memPtr);
        return false;
    }

    if (dataPtr->toRemote && threadPool_->IsNetworkThreadWorking()) {
        threadPool_->AddNetworkTask(dataPtr);
    } else {
        threadPool_->AddTask(dataPtr);
    }
    localPool_->RefillMemory(1);
    return true;
}

}  // namespace internal
}  // namespace log
}  // namespace ara
