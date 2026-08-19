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
/// @file       zip_compressor.cpp
/// @brief      Implementation of ZIP compression class, supporting both command line and libz API compression methods
/// @details
/// @date       2024-12-19
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "zip_compressor.h"

#include <sys/stat.h>
#include <unistd.h>
#include <zlib.h>

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#include "Utils/src/private_log.h"

namespace ara {
namespace log {
namespace internal {

std::string ZipCompressor::CompressWithCommand(const std::string& sourceFile) noexcept
{
    LOGVERBOSE(__func__) << "Starting command compression for: " << sourceFile;

    if (!_fileExists(sourceFile)) {
        LOGERROR(__func__) << "Source file does not exist: " << sourceFile;
        return "";
    }

    std::string compressedFile{sourceFile + ".tar.gz"};

    // Build the command line
    /// TODO Check if /usr/bin/tar or /usr/bin/zip exists
    std::string command{"/usr/bin/tar -czf \"" + compressedFile + "\" \"" + sourceFile + "\""};

    LOGVERBOSE(__func__) << "Executing command: " << command;

    // Execute the command
    int result = system(command.c_str());
    if (result == 0) {
        LOGVERBOSE(__func__) << "Command compression successful: " << compressedFile;

        // Delete the source file
        if (_removeFile(sourceFile)) {
            LOGVERBOSE(__func__) << "Source file removed: " << sourceFile;
        } else {
            LOGVERBOSE(__func__) << "Failed to remove source file: " << sourceFile;
        }

        return compressedFile;
    } else {
        LOGERROR(__func__) << "Command compression failed with code: " << result;
        return "";
    }
}

std::string ZipCompressor::CompressWithLibz(const std::string& sourceFile) noexcept
{
    LOGVERBOSE(__func__) << "Starting libz compression for: " << sourceFile;

    if (!_fileExists(sourceFile)) {
        LOGERROR(__func__) << "Source file does not exist: " << sourceFile;
        return "";
    }

    std::string compressedFile{sourceFile + ".gz"};

    // Open the source file
    std::ifstream sourceStream(sourceFile, std::ios::binary);
    if (!sourceStream.is_open()) {
        LOGERROR(__func__) << "Failed to open source file: " << sourceFile;
        return "";
    }

    // Create the compressed file
    gzFile gzFile = gzopen(compressedFile.c_str(), "wb9");
    if (gzFile == nullptr) {
        LOGERROR(__func__) << "Failed to create compressed file: " << compressedFile;
        sourceStream.close();
        return "";
    }

    const std::size_t bufferSize = 8192;
    char buffer[bufferSize];
    bool success = true;

    // Read the source file and write to the compressed file
    while (sourceStream.read(buffer, bufferSize) || sourceStream.gcount() > 0) {
        std::size_t bytesRead = sourceStream.gcount();
        int bytesWritten      = gzwrite(gzFile, buffer, bytesRead);
        if (bytesWritten <= 0) {
            LOGERROR(__func__) << "Failed to write to compressed file. Error: " << gzerror(gzFile, nullptr);
            success = false;
            break;
        }
    }

    // Close files
    sourceStream.close();
    int closeResult = gzclose(gzFile);

    if (closeResult != Z_OK) {
        LOGERROR(__func__) << "Failed to close compressed file. Error code: " << closeResult;
        success = false;
    }

    if (!success) {
        // Delete the failed compressed file
        _removeFile(compressedFile);
        return "";
    }

    LOGVERBOSE(__func__) << "Libz compression successful: " << compressedFile;

    // Delete the source file
    if (_removeFile(sourceFile)) {
        LOGVERBOSE(__func__) << "Source file removed: " << sourceFile;
    } else {
        LOGVERBOSE(__func__) << "Failed to remove source file: " << sourceFile;
    }

    return compressedFile;
}

bool ZipCompressor::CompressAsync(const std::string& sourceFile, bool useLibz, CompressionCallback callback) noexcept
{
    LOGVERBOSE(__func__) << "Starting async compression for: " << sourceFile;

    // Check if there is a thread already running
    if (compressionThread_ && compressionThread_->joinable()) {
        LOGVERBOSE(__func__) << "Compression thread is already running";
        return false;
    }

    // Start a new compression thread
    compressionThread_
        = std::make_unique< std::thread >(&ZipCompressor::_compressionWorker, this, sourceFile, useLibz, callback);

    if (compressionThread_ != nullptr) {
        return true;
    }
    return false;
}

void ZipCompressor::_compressionWorker(const std::string& sourceFile,
                                       bool useLibz,
                                       CompressionCallback callback) noexcept
{
    LOGVERBOSE(__func__) << "Compression worker started for: " << sourceFile;

    std::string result;
    bool success = false;
    std::string errorMessage;

    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();

    try {
        if (useLibz) {
            result = CompressWithLibz(sourceFile);
        } else {
            result = CompressWithCommand(sourceFile);
        }

        success = !result.empty();
        if (!success) {
            errorMessage = "Compression failed";
        }
    } catch (const std::exception& e) {
        errorMessage = std::string("Exception during compression: ") + e.what();
        LOGERROR(__func__) << errorMessage;
    } catch (...) {
        errorMessage = "Unknown exception during compression";
        LOGERROR(__func__) << errorMessage;
    }

    std::chrono::steady_clock::time_point endTime = std::chrono::steady_clock::now();
    std::chrono::milliseconds duration = std::chrono::duration_cast< std::chrono::milliseconds >(endTime - startTime);

    LOGVERBOSE(__func__) << "Compression completed in " << duration.count() << "ms. Success: " << success;

    // Call the callback function
    if (callback) {
        callback(result, success, errorMessage);
    }
    isDone_.store(true, std::memory_order_release);
}

bool ZipCompressor::_removeFile(const std::string& filePath) noexcept
{
    LOGVERBOSE(__func__) << "Removing source file: " << filePath;
#ifdef __qnx__
    std::int32_t const ret = remove(filePath.c_str());
#else
    std::int32_t const ret = unlink(filePath.c_str());
#endif
    return (ret == 0);
}

bool ZipCompressor::_fileExists(const std::string& filePath) noexcept { return access(filePath.c_str(), F_OK) == 0; }

}  // namespace internal
}  // namespace log
}  // namespace ara