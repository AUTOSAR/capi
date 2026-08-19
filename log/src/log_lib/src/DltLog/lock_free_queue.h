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
/// @file       lock_free_queue.h
/// @brief      Lock-free queue
/// @details
/// @date       2025-09-15
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltProtocol
/// @interface_level = module
/// @trace_id_sr=LOG_SR_00005,LOG_SR_00008,LOG_SR_00010
/// @unit_name = lock_free_queue
/// @unit_description=Lock-free queue
/// @endcode
///
/// ================================================================

#ifndef LOCK_FREE_QUEUE_H
#define LOCK_FREE_QUEUE_H
#include <atomic>
#include <cstdint>
#include <iostream>
#include <memory>
#include <vector>
// Assume these types are already defined elsewhere
using BUFFERTYPE = std::vector< std::uint8_t >;

struct LogMetaData;
// Message node and queue definitions
struct MessageNode
{
    std::int32_t value;
    std::shared_ptr< LogMetaData > logMetaData_;
    // std::shared_ptr<BUFFERTYPE> stringBuffer_{nullptr};
    // std::shared_ptr<BUFFERTYPE> dlTbuffer_{nullptr};
    std::atomic< MessageNode * > next{nullptr};

    // Add default constructor
    MessageNode() : value(0), next(nullptr) {}
    MessageNode(const std::int32_t &val) : value(val), next(nullptr) {}
};

class LockFreeQueue
{
private:
    std::atomic< MessageNode * > head;
    std::atomic< MessageNode * > tail;
    std::atomic< MessageNode * > dummyNode;  // Used to store the dummy node

    std::atomic< std::int64_t > nodeCount;  // Atomic counter, records the number of nodes

public:
    LockFreeQueue();
    ~LockFreeQueue();

    // Enqueue a single node
    bool enqueue(MessageNode *node);

    // Batch enqueue nodes (enqueue a list of nodes)
    bool enqueueBatch(MessageNode *batchHead);

    // Dequeue a single node
    MessageNode *dequeue();

    // Batch dequeue nodes
    MessageNode *dequeueBatch(std::int32_t batchSize);

    // Check if the queue is empty
    bool empty() const;

    // Get the current number of nodes in the queue
    std::int32_t size() const;

    // Clear the queue (for destruction or reset)
    bool clear();
};

#endif  // LOCK_FREE_QUEUE_H