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
/// @file       ini_parser.cpp
/// @brief
/// @details
/// @date       2025-09-17
/// @author     yangjinbiao
/// @version    1.2.0
///
/// ================================================================

#include "ini_parser.h"

#include <algorithm>
#include <cctype>
#include <fstream>

// Trim leading and trailing whitespace characters from a string
void IniParser::trim(std::string& str)
{
    // Find the first non-whitespace character
    size_t start = str.find_first_not_of(" \t\n\r");
    // Find the last non-whitespace character
    size_t end = str.find_last_not_of(" \t\n\r");

    if (start == std::string::npos) {
        // The string is all whitespace
        str = "";
    } else {
        str = str.substr(start, end - start + 1);
    }
}

// Convert a string to lowercase
std::string IniParser::toLower(const std::string& str)
{
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), [](unsigned char c) { return std::tolower(c); });
    return lowerStr;
}

// Check if a line is a comment
bool IniParser::isComment(const std::string& line)
{
    std::string trimmed = line;
    trim(trimmed);
    return !trimmed.empty() && (trimmed[0] == ';' || trimmed[0] == '#');
}

// Load INI configuration from a file (no sections)
bool IniParser::load(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    data_.clear();
    std::string line;

    while (std::getline(file, line)) {
        // Trim whitespace from the beginning and end of the line
        trim(line);

        // Skip empty lines and comments
        if (line.empty() || isComment(line)) {
            continue;
        }

        // Ignore any lines resembling a section ([xxx])
        if (line.front() == '[' && line.back() == ']') {
            continue;
        }

        // Parse key-value pair (key=value)
        size_t eqPos = line.find('=');
        if (eqPos != std::string::npos && eqPos > 0)  // Ensure the key is not empty
        {
            std::string key   = line.substr(0, eqPos);
            std::string value = line.substr(eqPos + 1);

            trim(key);
            trim(value);

            if (!key.empty())  // Only store non-empty keys
            {
                data_[key] = value;
            }
        }
    }

    file.close();
    return true;
}

// Save the configuration to a file
bool IniParser::save(const std::string& filename)
{
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }

    // Write all key-value pairs
    for (const auto& keyValue : data_) {
        file << keyValue.first << " = " << keyValue.second << "\n";
    }

    file.close();
    return true;
}

// Get a string value
std::string IniParser::getString(const std::string& key, const std::string& default_value)
{
    auto it = data_.find(key);
    if (it != data_.end()) {
        return it->second;
    }
    return default_value;
}

// Get an integer value
int IniParser::getInt(const std::string& key, int default_value)
{
    auto it = data_.find(key);
    if (it == data_.end()) {
        return default_value;
    }

    try {
        return std::stoi(it->second);
    } catch (...) {
        return default_value;
    }
}

// Get a double-precision floating-point value
double IniParser::getDouble(const std::string& key, double default_value)
{
    auto it = data_.find(key);
    if (it == data_.end()) {
        return default_value;
    }

    try {
        return std::stod(it->second);
    } catch (...) {
        return default_value;
    }
}

// Get a boolean value
bool IniParser::getBool(const std::string& key, bool default_value)
{
    auto it = data_.find(key);
    if (it == data_.end()) {
        return default_value;
    }

    std::string lowerValue = toLower(it->second);
    if (lowerValue == "true" || lowerValue == "1" || lowerValue == "yes") {
        return true;
    } else if (lowerValue == "false" || lowerValue == "0" || lowerValue == "no") {
        return false;
    }

    return default_value;
}

// Set a value
void IniParser::setValue(const std::string& key, const std::string& value) { data_[key] = value; }

// Check if a key exists
bool IniParser::hasKey(const std::string& key) { return data_.find(key) != data_.end(); }

// Get all keys
std::vector< std::string > IniParser::getKeys()
{
    std::vector< std::string > keys;
    for (const auto& keyValue : data_) {
        keys.push_back(keyValue.first);
    }
    return keys;
}
