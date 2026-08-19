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
/// @file       process_flock.h
/// @brief      No description provided.
/// @details
/// @date       2024-06-28
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef __LOG_INTERNAL_PROCESS_FLOCK__
#define __LOG_INTERNAL_PROCESS_FLOCK__
#include <fcntl.h>
#include <fcntl.h>  // open, O_WRONLY, etc.
#include <sys/file.h>
#include <sys/stat.h>  // mkdir, stat
#include <sys/types.h>
#include <unistd.h>
#include <unistd.h>  // close, unlink, access

#include <cassert>  // assertion
#include <cerrno>   // errno
#include <cstdlib>  // getenv
#include <cstring>  // strerror
#include <iostream>
#include <mutex>
#include <string>

namespace ara {
namespace log {
namespace internal {

// 1. Lock entity: responsible for maintaining the FD and Mutex (recommended to be global or a singleton)
class ProcessThreadLock
{
public:
    explicit ProcessThreadLock(const std::string& name)
    {
        std::string path     = "/tmp/" + name + ".lock";
        std::string lockPath = _getLockFilePath(name);
        if (lockPath.empty()) {
            fd_ = -1;
            return;
        }
        path = lockPath;
        fd_  = open(path.c_str(), O_RDWR | O_CREAT, 0666);
    }

    ~ProcessThreadLock()
    {
        if (fd_ != -1)
            close(fd_);
    }

    void Lock()
    {
        threadmtx_.lock();  // First layer: thread
        if (fd_ != -1)
            flock(fd_, LOCK_EX);  // Second layer: process
    }

    void Unlock()
    {
        if (fd_ != -1)
            flock(fd_, LOCK_UN);
        threadmtx_.unlock();
    }

private:
    bool _isDirectoryWritable(const std::string& dir)
    {
        // 1. First check if the directory exists; if not, try to create it (permissions 0700, only readable/writable/executable by the current user)
        struct stat st
        {
        };
        if (stat(dir.c_str(), &st) != 0) {
            if (errno == ENOENT) {
                // Directory does not exist, try to create it (recursive creation? Here only single layer, conforming to regular usage)
                if (mkdir(dir.c_str(), 0700) != 0) {
                    std::cerr << "Failed to create directory: " << dir << ", error: " << strerror(errno) << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Failed to check directory status: " << dir << ", error: " << strerror(errno) << std::endl;
                return false;
            }
        } else if (!S_ISDIR(st.st_mode)) {
            // Path exists but is not a directory
            std::cerr << "Path is not a directory: " << dir << std::endl;
            return false;
        }

        // 2. Construct a temporary file name (to avoid conflicts, using process ID + random suffix)
        std::string tempFile
            = dir + "/.tmp_writable_check_" + std::to_string(getpid()) + "_" + std::to_string(rand() % 10000);

        // 3. Try to create the temporary file (O_EXCL ensures the file does not exist, avoiding overwriting)
        int fd = open(tempFile.c_str(), O_WRONLY | O_CREAT | O_EXCL, 0600);
        if (fd < 0) {
            std::cerr << "Failed to create test file (directory not writable): " << dir << ", error: " << strerror(errno) << std::endl;
            return false;
        }

        // 4. Try to write 1 byte to verify actual writability
        char test = 'a';
        if (write(fd, &test, 1) != 1) {
            std::cerr << "Failed to write to test file: " << tempFile << ", error: " << strerror(errno) << std::endl;
            close(fd);
            unlink(tempFile.c_str());  // Clean up the temporary file
            return false;
        }

        // 5. Clean up the temporary file
        close(fd);
        if (unlink(tempFile.c_str()) != 0) {
            std::cerr << "Failed to clean up test file: " << tempFile << ", error: " << strerror(errno) << std::endl;
            // Cleanup failure does not affect the writability judgment, only a warning
        }

        return true;
    }

    /**
 * @brief Get the final usable lock file path (no exception thrown)
 * @param name The externally provided lock file name prefix (non-empty)
 * @return The complete writable lock file path; return an empty string if none of the paths are writable or name is empty
 */
    std::string _getLockFilePath(const std::string& name)
    {
        // Validate input parameter legality; if empty, return empty string directly
        if (name.empty()) {
            std::cerr << "Error: name parameter cannot be empty" << std::endl;
            return "";
        }

        // Step 1: Check the path corresponding to the environment variable ISOFT_ARA_RUNTIME_DIR
        const char* runtimeDirEnv = getenv("ISOFT_ARA_RUNTIME_DIR");
        if (runtimeDirEnv != nullptr && !std::string(runtimeDirEnv).empty()) {
            std::string runtimeDir = runtimeDirEnv;
            if (_isDirectoryWritable(runtimeDir)) {
                return runtimeDir + "/" + name + ".lock";
            }
            std::cerr << "Environment variable path is not writable: " << runtimeDir << ", attempting /tmp directory" << std::endl;
        } else {
            std::cerr << "Environment variable ISOFT_ARA_RUNTIME_DIR is not set, attempting /tmp directory" << std::endl;
        }

        // Step 2: Check if the /tmp directory is writable
        const std::string tmpDir = "/tmp";
        if (_isDirectoryWritable(tmpDir)) {
            return tmpDir + "/" + name + ".lock";
        }
        std::cerr << "/tmp directory is not writable, attempting /run directory" << std::endl;

        // Step 3: Check if the /run directory is writable
        const std::string runDir = "/run";
        if (_isDirectoryWritable(runDir)) {
            return runDir + "/" + name + ".lock";
        }

        // All paths are not writable, return an empty string
        std::cerr << "Error: All candidate directories (ISOFT_ARA_RUNTIME_DIR, /tmp, /run) are not writable, cannot create lock file" << std::endl;
        return "";
    }

private:
    int fd_ = -1;
    std::mutex threadmtx_;
};

class ScopedFLock
{
public:
    explicit ScopedFLock(ProcessThreadLock& lock) : lock_(lock) { lock_.Lock(); }
    ~ScopedFLock() { lock_.Unlock(); }

    // Disable copying (lock cannot be copied)
    ScopedFLock(const ScopedFLock&) = delete;
    ScopedFLock& operator=(const ScopedFLock&) = delete;

private:
    ProcessThreadLock& lock_;
};

}  // namespace internal
}  // namespace log
}  // namespace ara
#endif