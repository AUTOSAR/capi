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
/// @file       ini_parser.h
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#ifndef INI_PARSER_H
#define INI_PARSER_H

#include <string>
#include <unordered_map>
#include <vector>

class IniParser
{
public:
    // Load an INI file
    bool load(const std::string& filename);
    // Save an INI file
    bool save(const std::string& filename);

    // Get a string value
    std::string getString(const std::string& key, const std::string& default_value = "");
    // Get an integer value
    int getInt(const std::string& key, int default_value = 0);
    // Get a double-precision floating-point value
    double getDouble(const std::string& key, double default_value = 0.0);
    // Get a boolean value
    bool getBool(const std::string& key, bool default_value = false);

    // Set a key-value pair
    void setValue(const std::string& key, const std::string& value);

    // Check if a key exists
    bool hasKey(const std::string& key);

    // Get all keys
    std::vector< std::string > getKeys();

    // Check if a line is a comment
    bool isComment(const std::string& line);

private:
    // Container for storing key-value pairs, no section needed
    std::unordered_map< std::string, std::string > data_;

    // Trim leading and trailing whitespace characters from a string
    static void trim(std::string& str);
    // Convert a string to lowercase
    static std::string toLower(const std::string& str);
};

#endif  // INI_PARSER_H
