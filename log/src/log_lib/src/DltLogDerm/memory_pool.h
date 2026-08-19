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
/// @file       memory_pool.h
/// @brief      Lib interface side of Dlt, internal support function
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_MEMORY_POOL__
#define __LOG_INTERNAL_MEMORY_POOL__

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <memory>
#include <mutex>

#include "Utils/src/private_log.h"
#include "concurrentqueue/moodycamel/concurrentqueue.h"
#include "determ_log_meta_data.h"
namespace ara {
namespace log {
namespace internal {

/// @brief Byte alignment
constexpr std::size_t kAlignBytes{64U};

/// @brief Encode length
constexpr std::size_t kEncodeLength{1272U};

/// @brief Task length GetNodeSize()=48
constexpr std::size_t kTaskLength{192U};

/// @brief Number of fixed memory blocks per logger
constexpr std::size_t kLoggerBlockCount{16U};

/// @brief Default memory expansion step
constexpr std::size_t kLoggerCount{300U};

/// @brief Default memory expansion step
constexpr std::size_t kMaxtryCount{10U};

/// @brief Default memory expansion step
constexpr std::size_t kMemScaled5{5U};

/// @brief Default memory expansion step
constexpr std::size_t kMemScaled2{2U};
/// @brief Default memory expansion step
constexpr std::size_t kMemScaled3{3U};
//// @brief Fixed-size Array (16 blocks)
struct BlockArray
{
    moodycamel::ConcurrentQueue< uint8_t * > blocks;

    /// @brief Default constructor
    BlockArray() = default;
    /// @brief Copy constructor
    BlockArray(BlockArray &other) = delete;
    /// @brief Constructor
    /// @param blockSize Memory block size
    explicit BlockArray(size_t blockSize)
    {
        for (std::size_t index = 0; index < kLoggerBlockCount; index++) {
            uint8_t *ptr{new uint8_t[blockSize]};
            memset(ptr, 0, blockSize);
            blocks.enqueue(ptr);
        }
    }
    /// @brief Copy assignment
    /// @param other Reference
    BlockArray &operator=(BlockArray const &other) = delete;
    /// @brief Move assignment
    /// @param other Reference
    BlockArray &operator=(BlockArray &&other) noexcept = default;
    /// @brief Move constructor
    /// @param other Reference
    BlockArray(BlockArray &&other) noexcept = default;
    /// @brief Destructor
    ~BlockArray()
    {
        while (true) {
            uint8_t *ptr = nullptr;
            if ((blocks.size_approx() > 0) && blocks.try_dequeue(ptr)) {
                if (nullptr != ptr) {
                    delete[] ptr;
                }
            } else {
                break;
            }
        }
    }
};

//// @brief Lock-free stack implementing a global memory pool
class GlobalMemoryPool
{
public:
    /// @brief Constructor
    /// @param blockSize Memory block size
    /// @param freeCount Number of free memory blocks
    GlobalMemoryPool(size_t blockSize, size_t freeCount)
        : freeCount_{freeCount}
        , totalMemCount_{freeCount}
        , freeBlockList_(moodycamel::ConcurrentQueue< uint8_t * >(freeCount * kScaled))
    {
        Initialize(blockSize, freeCount);
    }

    /// @brief Destructor
    ~GlobalMemoryPool()
    {
        uint8_t *blockPtr{nullptr};
        bool ret{false};
        do {
            blockPtr = nullptr;
            ret      = freeBlockList_.try_dequeue(blockPtr);
            if ((nullptr != blockPtr) && (true == ret)) {
                delete[] blockPtr;
            }

            /// In the deleter, release memory
            /// if ((ret == true) && (nullptr != blockPtr)) {
            ///   uint8_t *pRaw = (uint8_t *)(blockPtr);
            ///    delete [] pRaw;
            /// }
        } while ((true == ret) && (nullptr != blockPtr));
    }
    /// @brief Default constructor
    GlobalMemoryPool() = default;
    /// @brief Copy constructor
    GlobalMemoryPool(GlobalMemoryPool &other) = delete;
    /// @brief Copy assignment
    /// @param other Reference
    GlobalMemoryPool &operator=(GlobalMemoryPool const &other) = delete;
    /// @brief Move assignment
    /// @param other Reference
    GlobalMemoryPool &operator=(GlobalMemoryPool &&other) noexcept = delete;
    /// @brief Move constructor
    /// @param other Reference
    GlobalMemoryPool(GlobalMemoryPool &&other) noexcept = delete;
    /// @brief Initialize the memory pool
    /// @param blockSize Memory block size
    /// @param loggerCount Number of loggers
    /// @param freeCount Number of free memory blocks
    void Initialize(size_t blockSize, size_t freeCount)
    {
        alignedBlockSize_ = (blockSize + (kAlignBytes - 1)) / kAlignBytes * kAlignBytes;
        alignedNodeSize_  = ((kAlignBytes - 1)) / kAlignBytes * kAlignBytes;

        // User needs + management needs memory
        blockSize_ = alignedBlockSize_ + alignedNodeSize_;
        for (size_t i = 0; i < freeCount; i++) {
            uint8_t *ptr = new uint8_t[blockSize_];
            memset(ptr, 0, blockSize_);
            freeBlockList_.enqueue(ptr);
        }
        freeCanAllocate_ = freeCount * kScaled - freeCount;
    }
    /// @brief Allocate a BlockArray from the pool
    BlockArray *AllocateArray() const { return new BlockArray(blockSize_); }

    /// @brief User-available block size (aligned)
    size_t UserBlockSize() const noexcept { return alignedBlockSize_; }

    /// @brief Allocate a memory block from the pool
    /// @return Memory block pointer
    uint8_t *AllocateBlock()
    {
        uint8_t *ptr{nullptr};
        bool ret{freeBlockList_.try_dequeue(ptr)};
        if (ret && (nullptr != ptr)) {
            freeCount_.fetch_sub(1, std::memory_order_release);
        } else {
            LOGERROR(__func__) << " freeCount_=" << freeCount_.load(std::memory_order_acquire)
                               << ", freeBlockList_.size=" << freeBlockList_.size_approx()
                               << ", freeCalledCount_=" << freeCalledCount_.load(std::memory_order_acquire);
        }
        return ptr;
    }

    /// @brief Free a memory block
    /// @param block Memory block pointer
    void FreeBlock(uint8_t *block)
    {
        if (nullptr != block) {
            memset(block, 0, blockSize_);
            freeBlockList_.enqueue(block);
            freeCount_.fetch_add(1, std::memory_order_release);
            freeCalledCount_.fetch_add(1, std::memory_order_release);
        }
    }
    /// @brief Expansion strategy
    void CheckAndExpand()
    {
        if (freeCount_.load(std::memory_order_acquire) > (kLoggerBlockCount * kLoggerCount / kMemScaled2)) {
            return;
        }
        size_t expandSize{kLoggerCount * kMemScaled2 * kLoggerBlockCount};
        if (freeCount_.load(std::memory_order_acquire) < (kLoggerBlockCount * kLoggerCount / kMemScaled5)) {
            expandSize = kLoggerCount * kMemScaled5 * kLoggerBlockCount;
        }
        if (freeCount_.load(std::memory_order_acquire) < (kLoggerBlockCount * kLoggerCount / kMemScaled3)) {
            expandSize = kLoggerCount * kMemScaled3 * kLoggerBlockCount;
        }
        if (expandSize < freeCanAllocate_) {
            for (size_t i = 0; i < expandSize; i++) {
                uint8_t *ptr = new uint8_t[blockSize_];
                memset(ptr, 0, blockSize_);
                freeBlockList_.enqueue(ptr);
            }
            freeCount_.fetch_add(expandSize, std::memory_order_release);
            totalMemCount_.fetch_add(expandSize, std::memory_order_release);
            freeCanAllocate_ = freeCanAllocate_ - expandSize;
        } else if (0 < freeCanAllocate_) {
            for (size_t i = 0; i < freeCanAllocate_; i++) {
                uint8_t *ptr = new uint8_t[blockSize_];
                memset(ptr, 0, blockSize_);
                freeBlockList_.enqueue(ptr);
            }
            freeCount_.fetch_add(freeCanAllocate_, std::memory_order_release);
            totalMemCount_.fetch_add(freeCanAllocate_, std::memory_order_release);
            freeCanAllocate_ = 0;
        }
        LOGERROR(__func__) << " freeCount_=" << freeCount_.load(std::memory_order_acquire)
                           << ", totalMemCount_=" << totalMemCount_.load(std::memory_order_acquire);
    }

private:
    size_t alignedBlockSize_{0};
    size_t alignedNodeSize_{0};
    size_t blockSize_{0};

    size_t freeCanAllocate_{0};
    std::atomic< std::size_t > freeCount_{0};
    std::atomic< std::size_t > totalMemCount_{0};

    std::atomic< std::size_t > freeCalledCount_{0};

    const size_t kScaled{10};
    moodycamel::ConcurrentQueue< uint8_t * > freeBlockList_;
};

/// @brief Dedicated secondary memory pool
class LocalPool
{
public:
    /// @brief Constructor
    /// @param globalPoolRef Reference to the global memory pool
    explicit LocalPool(GlobalMemoryPool &globalPoolRef) : globalPool_(globalPoolRef), blockArray_{nullptr}
    {
        blockArray_ = globalPool_.AllocateArray();
    }
    /// @brief Destructor
    ~LocalPool()
    {
        if (nullptr != blockArray_) {
            blockArray_->~BlockArray();
            uint8_t *pRaw{reinterpret_cast< uint8_t * >(blockArray_)};
            delete[] pRaw;
            blockArray_ = nullptr;
        }
    }
    /// @brief Default constructor
    LocalPool() = delete;
    /// @brief  Copy constructor
    /// @param other Reference
    LocalPool(LocalPool const &other) = delete;
    /// @brief Copy assignment
    /// @param other Reference
    /// @return Reference
    LocalPool &operator=(LocalPool const &other) = delete;
    /// @brief  Move constructor
    /// @param other Reference
    LocalPool(LocalPool &&other) noexcept = delete;
    /// @brief Move assignment
    /// @param other Reference
    /// @return Reference
    LocalPool &operator=(LocalPool &&other) noexcept = delete;

    /// @brief Allocate a memory block
    /// @return Memory block pointer
    uint8_t *Allocate()
    {
        if (nullptr == blockArray_) {
            return nullptr;
        }
        uint8_t *pData{nullptr};
        std::size_t tryCount{0};
        while (tryCount < kMaxtryCount) {
            tryCount++;
            bool ret{blockArray_->blocks.try_dequeue(pData)};
            if ((nullptr == pData) || (false == ret)) {
                RefillMemory(kLoggerBlockCount);
                ret = blockArray_->blocks.try_dequeue(pData);
            }
            if (nullptr != pData) {
                break;
            }
        }
        if (nullptr == pData) {
            LOGERROR(__func__) << "Allocate nullptr == pData tryCount=" << tryCount << ",pid=" << getpid()
                               << ", allocateCalledCount_=" << allocateCalledCount_.load(std::memory_order_acquire);
        } else {
            allocateCalledCount_.fetch_add(1, std::memory_order_release);
        }
        return pData;
    }

    /// @brief Allocate blocks on demand; returns empty when requested size exceeds user block size
    uint8_t *Allocate(std::size_t requestedSize)
    {
        if ((requestedSize == 0U) || (requestedSize > BlockSize())) {
            return nullptr;
        }
        return Allocate();
    }
    /// @brief Refill from the global pool
    /// @param blockCount Number of memory blocks to refill
    /// @return Whether the operation succeeded
    bool RefillMemory(std::uint8_t blockCount)
    {
        std::ignore = blockCount;
        std::size_t tryCount{AvailableBlock()};
        while ((kLoggerBlockCount > tryCount) && (kLoggerBlockCount > AvailableBlock())) {
            tryCount++;
            uint8_t *ptr{globalPool_.AllocateBlock()};
            if (nullptr != ptr) {
                bool bOk = blockArray_->blocks.enqueue(ptr);
                if (!bOk) {
                    globalPool_.FreeBlock(ptr);
                    break;
                }
            }
        }
        return kLoggerBlockCount == AvailableBlock();
    }

    /// @brief View the current number of available blocks
    size_t AvailableBlock() const { return blockArray_->blocks.size_approx(); }

    /// @brief Size of a single block available for business data
    size_t BlockSize() const noexcept { return globalPool_.UserBlockSize(); }

    /// @brief Free a memory block
    /// @param blockPtr Memory block pointer
    void FreeBlock(uint8_t *blockPtr) { globalPool_.FreeBlock(blockPtr); }
    /// @brief Check and expand the global memory pool
    void CheckAndExpand() { globalPool_.CheckAndExpand(); }

private:
    /// @brief Reference to the global memory pool
    GlobalMemoryPool &globalPool_;
    /// @brief Memory pointer
    BlockArray *blockArray_;
    std::atomic< std::size_t > allocateCalledCount_{0};
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif