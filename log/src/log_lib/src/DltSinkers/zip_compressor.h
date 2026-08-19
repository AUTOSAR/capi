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
/// @file       zip_compressor.h
/// @brief      ZIP compression class, supporting both command line and libz API compression methods
/// @details
/// @date       2024-12-19
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/LOG/DltSinkers
/// @interface_level=unit
/// @unit_name = zip_compressor
/// @unit_description=File compression utility class, supporting tar.gz and zip formats
/// @endcode
///
/// ================================================================

#ifndef ZIP_COMPRESSOR_H_
#define ZIP_COMPRESSOR_H_

#include <atomic>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <thread>

namespace ara {
namespace log {
namespace internal {

/// @brief ZIP compression class
/// Provides two compression methods: command line and libz API
class ZipCompressor
{
    enum class CompressionFormat : uint8_t
    {
        TAR_GZ,  /// tar + gzip format
        ZIP      /// ZIP format
    };

public:
    /// @brief Compression result callback function type
    using CompressionCallback = std::function< void(const std::string&, bool, const std::string&) >;

    /// @brief Constructor
    ZipCompressor() = default;

    /// @brief Destructor
    ~ZipCompressor()
    {
        // If the thread can still be joined, wait for it to complete
        if (compressionThread_ && compressionThread_->joinable()) {
            compressionThread_->join();  // Block until the thread ends
        }
    }

    /// @brief Disable copy construction and assignment
    ZipCompressor(const ZipCompressor&) = delete;
    ZipCompressor& operator=(const ZipCompressor&) = delete;

    /// @brief Compress a file using the command line method (stateless function)
    /// @param[in] sourceFile Path to the source file
    /// @param[in] format Compression format
    /// @return Path to the compressed file, empty string on failure
    static std::string CompressWithCommand(const std::string& sourceFile) noexcept;

    /// @brief Compress a file using the libz API method (stateless function)
    /// @param[in] sourceFile Path to the source file
    /// @param[in] format Compression format
    /// @return Path to the compressed file, empty string on failure
    static std::string CompressWithLibz(const std::string& sourceFile) noexcept;

    /// @brief Asynchronously compress a file, starting a new thread for execution
    /// @param[in] sourceFile Path to the source file
    /// @param[in] format Compression format
    /// @param[in] useLibz Whether to use the libz API (false uses the command line)
    /// @param[in] callback Compression completion callback function
    /// @return Whether the thread was successfully started
    bool CompressAsync(const std::string& sourceFile, bool useLibz, CompressionCallback callback) noexcept;
    bool IsCompleted() const noexcept { return isDone_.load(std::memory_order_acquire); }

private:
    /// @brief Internal compression work function
    /// @param[in] sourceFile Path to the source file
    /// @param[in] format Compression format
    /// @param[in] useLibz Whether to use the libz API
    /// @param[in] callback Callback function
    void _compressionWorker(const std::string& sourceFile, bool useLibz, CompressionCallback callback) noexcept;

    /// @brief Delete the source file
    /// @param[in] filePath Path to the file
    /// @return Whether the deletion succeeded
    static bool _removeFile(const std::string& filePath) noexcept;

    /// @brief Check if a file exists
    /// @param[in] filePath Path to the file
    /// @return Whether the file exists
    static bool _fileExists(const std::string& filePath) noexcept;

private:
    std::unique_ptr< std::thread > compressionThread_{nullptr};  /// Compression thread
    std::atomic< bool > isDone_{false};
};
using ZipCompressorPtr = std::shared_ptr< ZipCompressor >;
using CompressorList   = std::list< ZipCompressorPtr >;
// using CompressMap =std::map<
}  // namespace internal
}  // namespace log
}  // namespace ara

#endif  // ZIP_COMPRESSOR_H_