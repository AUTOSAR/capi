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
/// @file       errno_exception.h
/// @brief      Error exception
/// @details
/// @date       2022-04-20
/// @author     niuliming
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/idsm/Common
/// @interface_level=module
/// @trace_id_sr=SRS_IDSM_0011
/// @unit_description=System call exception
/// @endcode
///
/// ================================================================

#ifndef ADAPTIVEAUTOSAREXECUTIONMANAGER_SRC_INCLUDE_ERRNOEXCEPTION_H_
#define ADAPTIVEAUTOSAREXECUTIONMANAGER_SRC_INCLUDE_ERRNOEXCEPTION_H_

#include <cstring>
#include <exception>
#include <string>

namespace ara {
namespace idsm {
namespace internal {
namespace libadaptiveplatform {

/**
 * \brief Abstract base class for all exceptions related to calls that set the errno variable.
 */
class ErrnoException : public std::exception
{
private:
    int errorNumber_;
    std::string errorMessage_;

public:
    /**
     * \brief Gets the error number (errno) recorded in this exception.
     *
     * \returns the errno set in this exception.
     */
    int GetErrorNumber() const { return errorNumber_; }

    /**
     * \brief Gets the clear-text error message
     *
     * The message is obtained using strerror(errorNumber)
     *
     * \returns A reference to a string object stored in this exception containing the error message.
     */
    std::string const& GetErrorMessage() const { return errorMessage_; }

    /**
     * \brief Construct a new ErrnoExcpetion and record the error message for the given errno.
     *
     * \param errorNumber The C errno that has occured.
     *
     * \returns A reference to a string object stored in this exception containing the error message.
     */
    explicit ErrnoException(int errorNumber) : errorNumber_(errorNumber)
    {
        errorMessage_ = strerror(errorNumber);  // NOLINT
    }
    ErrnoException(ErrnoException const&) = default;
    ErrnoException(ErrnoException&&)      = default;
    ErrnoException& operator=(ErrnoException const&) = default;
    ErrnoException& operator=(ErrnoException&&) = default;
    ~ErrnoException() override                  = default;

    char const* what() const noexcept override { return errorMessage_.c_str(); }
};

} /* namespace libadaptiveplatform */
} /* namespace internal */
}  // namespace idsm
} /* namespace ara */

#endif /* ADAPTIVEAUTOSAREXECUTIONMANAGER_SRC_INCLUDE_ERRNOEXCEPTION_H_ */
