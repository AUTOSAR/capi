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
/// @file       base64.hpp
/// @brief      Base64 encoding and decoding functions.
/// @details
/// @date       2023-11-02
/// @author     hanzhibo
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/UCMLib
/// @module_path=/UCM/UCMLib
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00006
/// @unit_name=Base64
/// @unit_description=Base64 encoding and decoding functions provided for UCM
/// @endcode
///
/// ================================================================

#ifndef _BASE_64_HPP_
#define _BASE_64_HPP_

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @needwork = dd
class Base64
{
public:
    // Decodes a Base64 string into a byte array
    /// @brief Decodes a Base64 string into a byte array
    /// @param encodedString The Base64 encoded string
    /// @return The Decoded byte array as a string
    /// @throws std::invalid_argument if the input is not a valid Base64 encoded string
    /// @needwork = dda
    static std::string Decode(const std::string& encodedString)
    {
        const std::string base64Chars
            = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
              "abcdefghijklmnopqrstuvwxyz"
              "0123456789+/";

        const auto isBase64 = [](unsigned char const c) -> bool {
            bool const isAlnum{0 != isalnum(static_cast< int >(c))};
            return (isAlnum || (c == static_cast< unsigned char >('+')) || (c == static_cast< unsigned char >('/')));
        };

        size_t i = 0U;

        std::vector< unsigned char > charArray4(4U);
        std::vector< unsigned char > charArray3(3U);
        std::string decodedString;

        for (const auto c : encodedString) {
            if ((c == '=') || (!isBase64(static_cast< unsigned char >(c)))) {
                break;
            }
            charArray4[i] = static_cast< unsigned char >(c);
            i++;
            if (i == 4U) {
                std::ignore
                    = std::transform(charArray4.begin(), charArray4.end(), charArray4.begin(),
                                     [&](unsigned char const ch) { return base64Chars.find(static_cast< char >(ch)); });

                constexpr uint16_t kNUM_30{0x30U};
                charArray3[0U] = static_cast< unsigned char >(static_cast< unsigned char >(charArray4[0U] << 2)
                                                              + ((charArray4[1U] & kNUM_30) >> 4));
                constexpr uint16_t kNUM_15{0xfU};
                constexpr uint16_t kNUM_60{0x3cU};
                charArray3[1U]
                    = static_cast< unsigned char >((static_cast< unsigned char >((charArray4[1U] & kNUM_15) << 4))
                                                   + ((charArray4[2U] & kNUM_60) >> 2));

                constexpr int16_t kNUM_6{6};
                charArray3[2U] = static_cast< unsigned char >(
                    static_cast< unsigned char >((charArray4[2U] & 0x3U) << kNUM_6) + charArray4[3U]);

                std::ignore = decodedString.append(charArray3.begin(), charArray3.end());
                i           = 0U;
            }
        }

        if (i > 0U) {
            std::fill(charArray4.begin() + static_cast< std::vector< unsigned char >::iterator::difference_type >(i),
                      charArray4.end(), 0);

            std::ignore
                = std::transform(charArray4.begin(), charArray4.end(), charArray4.begin(),
                                 [&](unsigned char const c) { return base64Chars.find(static_cast< char >(c)); });

            constexpr uint16_t kNUM_30{0x30U};
            charArray3[0U] = static_cast< unsigned char >(static_cast< unsigned char >(charArray4[0U] << 2)
                                                          + ((charArray4[1U] & kNUM_30) >> 4));

            constexpr uint16_t kNUM_15{0xfU};
            constexpr uint16_t kNUM_60{0x3cU};
            charArray3[1U] = static_cast< unsigned char >(
                (static_cast< unsigned char >((charArray4[1U] & kNUM_15) << 4)) + ((charArray4[2U] & kNUM_60) >> 2));

            constexpr int16_t kNUM_6{6};
            charArray3[2U] = static_cast< unsigned char >(
                static_cast< unsigned char >((charArray4[2U] & 0x3U) << kNUM_6) + charArray4[3U]);

            for (size_t j = 0U; j < i - 1U; ++j) {
                decodedString += static_cast< char >(charArray3[j]);
            }
        }

        return decodedString;
    }
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // _BASE_64_HPP_