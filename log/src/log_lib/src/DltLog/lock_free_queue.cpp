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
/// @file       lock_free_queue.cpp
/// @brief      No description provided.
/// @details
/// @date       2024-06-28
/// @author     Copilot
/// @version    1.2.0
///
/// ================================================================

#include "lock_free_queue.h"

#include <atomic>
#include <memory>
#include <thread>

// Michael-Scott lock-free queue implementation (with dummy node, supports multiple producers and multiple consumers)
LockFreeQueue::LockFreeQueue() : nodeCount(0)
{
    // Initialize dummy node, using the correct constructor
    MessageNode *dummy = new struct MessageNode();
    dummy->next.store(nullptr, std::memory_order_seq_cst);
    head.store(dummy, std::memory_order_seq_cst);
    tail.store(dummy, std::memory_order_seq_cst);
}

LockFreeQueue::~LockFreeQueue()
{
    clear();
    // Delete the last dummy node
    MessageNode *dummy = head.load(std::memory_order_seq_cst);
    delete dummy;
}

// Enqueue a single node
bool LockFreeQueue::enqueue(MessageNode *node)
{
    node->next.store(nullptr, std::memory_order_seq_cst);
    while (true) {
        MessageNode *last = tail.load(std::memory_order_acquire);
        MessageNode *next = last->next.load(std::memory_order_acquire);
        if (next == nullptr) {
            // The next of the tail node is NULL, try to insert
            if (last->next.compare_exchange_weak(next, node, std::memory_order_release, std::memory_order_seq_cst)) {
                // Insertion succeeded, try to advance the tail
                tail.compare_exchange_weak(last, node, std::memory_order_release, std::memory_order_seq_cst);
                std::int32_t local_count = nodeCount.fetch_add(1, std::memory_order_seq_cst);
                std::string logsrt
                    = "internal enqueue : " + std::to_string((std::uint64_t)node) + " _ " + std::to_string(local_count);

                return true;
            }
        } else {
            // Tail is lagging, advance the tail
            tail.compare_exchange_weak(last, next, std::memory_order_release, std::memory_order_seq_cst);
        }
    }
    return false;
}

// Dequeue a single node
MessageNode *LockFreeQueue::dequeue()
{
    while (true) {
        MessageNode *first = head.load(std::memory_order_acquire);
        MessageNode *next  = first->next.load(std::memory_order_acquire);

        if (next == nullptr) {
            std::this_thread::yield();
            return nullptr;  // Queue is empty
        }

        // Try to move the head pointer
        if (head.compare_exchange_weak(first, next, std::memory_order_release, std::memory_order_seq_cst)) {
            // first is the old dummy, next is the first data node (now becomes the new dummy)
            // Transfer the data of next to first, delete next, return first
            std::swap(first->value, next->value);
            std::swap(first->logMetaData_, next->logMetaData_);

            // next is now the new dummy node, stays in the queue
            // first now holds the data, return it to the caller
            std::int32_t local_count = nodeCount.fetch_sub(1, std::memory_order_seq_cst);
            std::string logsrt
                = "internal dequeue : " + std::to_string((std::uint64_t)first) + " _ " + std::to_string(local_count);
            std::cout << logsrt << std::endl;

            return first;
        }
    }
}

// Batch enqueue nodes (tail insertion of a linked list)
bool LockFreeQueue::enqueueBatch(MessageNode *batchHead)
{
    if (!batchHead)
        return false;

    // Find the tail of the batch node and its count
    MessageNode *batchTail = batchHead;
    std::int32_t count     = 1;
    while (batchTail->next.load()) {
        batchTail = batchTail->next.load();
        count++;
    }
    batchTail->next.store(nullptr, std::memory_order_seq_cst);

    while (true) {
        MessageNode *last = tail.load(std::memory_order_acquire);
        MessageNode *next = last->next.load(std::memory_order_acquire);
        if (next == nullptr) {
            if (last->next.compare_exchange_weak(next, batchHead, std::memory_order_release,
                                                 std::memory_order_seq_cst)) {
                tail.compare_exchange_weak(last, batchTail, std::memory_order_release, std::memory_order_seq_cst);
                nodeCount.fetch_add(count, std::memory_order_seq_cst);
                return true;
            }
        } else {
            tail.compare_exchange_weak(last, next, std::memory_order_release, std::memory_order_seq_cst);
        }
    }
}

// Batch dequeue nodes
MessageNode *LockFreeQueue::dequeueBatch(std::int32_t batchSize)
{
    if (batchSize <= 0)
        return nullptr;

    MessageNode *batchHead = nullptr;
    MessageNode *batchTail = nullptr;
    int count              = 0;

    while (count < batchSize) {
        MessageNode *node = dequeue();
        if (!node)
            break;

        // Reset the next pointer of this node to avoid pointing to other nodes in the queue
        node->next.store(nullptr, std::memory_order_seq_cst);

        if (!batchHead)
            batchHead = node;

        if (batchTail)
            batchTail->next.store(node, std::memory_order_seq_cst);

        batchTail = node;
        count++;
    }

    return batchHead;
}

// Check if the queue is empty
bool LockFreeQueue::empty() const
{
    MessageNode *first = head.load(std::memory_order_acquire);
    MessageNode *next  = first->next.load(std::memory_order_acquire);
    return next == nullptr;
}

// Get the current number of nodes in the queue
std::int32_t LockFreeQueue::size() const { return nodeCount.load(std::memory_order_seq_cst); }

// Clear the queue (for destruction or reset)
bool LockFreeQueue::clear()
{
    // First dequeue all data nodes and delete them
    while (true) {
        MessageNode *node = dequeue();
        if (node == nullptr)
            break;
        delete node;
    }

    // Now only one dummy node remains in the queue
    // Reset the counter
    nodeCount.store(0, std::memory_order_seq_cst);
    return true;
}