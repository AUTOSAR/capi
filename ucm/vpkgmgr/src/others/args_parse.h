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
/// @file       args_parse.h
/// @brief
/// @details
/// @date       2023-12-22
/// @author     zhaoyunfei
/// @version    1.2.0
///
/// ================================================================

#ifndef VPKGMGR_SRC_COMMON_ARGS_PARSE_H__
    #define VPKGMGR_SRC_COMMON_ARGS_PARSE_H__

    #include <cstdint>
    #include <ios>
    #include <memory>
    #include <sstream>
    #include <stdexcept>
    #include <string>
    #include <type_traits>
    #include <unordered_map>
    #include <utility>
    #include <vector>
// NOLINTBEGIN
namespace ara {
namespace ucm {
namespace arg_parser {

enum class ArgType : uint8_t
{
    kNone,
    kBool,
    kInt8,
    kUint8,
    kInt16,
    kUint16,
    kInt32,
    kUint32,
    kInt64,
    kUint64,
    kFloat,
    kDouble,
    kString,
};

template < typename T >
struct is_option_args_type
    : std::integral_constant<
          bool,
          std::is_arithmetic< T >::value || std::is_same< T, char* >::value || std::is_same< T, const char* >::value
              || std::is_same< T, const std::string& >::value || std::is_same< T, const std::string >::value
              || std::is_same< T, std::string& >::value || std::is_same< T, std::string >::value >
{
};

class ValueType  // NOLINT
{
public:
    virtual bool IsArry() const = 0;
    virtual ArgType Type() const { return ArgType::kNone; }

    virtual std::string default_value_str() const  // NOLINT
    {
        return "";
    }

    bool has_default() const  // NOLINT
    {
        return hasDefault_;
    }

    virtual void FromString(const std::vector< std::string >&) {}
    virtual ~ValueType() = default;

protected:
    bool hasDefault_{false};  // NOLINT
};

template < typename T >
inline ArgType ToArgType()
{
    if (std::is_same< T, bool >::value || std::is_same< T, const bool >::value) {
        return ArgType::kBool;
    }
    if (std::is_same< T, int8_t >::value || std::is_same< T, const int8_t >::value) {
        return ArgType::kInt8;
    }
    if (std::is_same< T, uint8_t >::value || std::is_same< T, const uint8_t >::value) {
        return ArgType::kUint8;
    }
    if (std::is_same< T, int16_t >::value || std::is_same< T, const int16_t >::value) {
        return ArgType::kInt16;
    }
    if (std::is_same< T, uint16_t >::value || std::is_same< T, const uint16_t >::value) {
        return ArgType::kUint16;
    }
    if (std::is_same< T, int32_t >::value || std::is_same< T, const int32_t >::value) {
        return ArgType::kInt32;
    }
    if (std::is_same< T, uint32_t >::value || std::is_same< T, const uint32_t >::value) {
        return ArgType::kUint32;
    }
    if (std::is_same< T, int64_t >::value || std::is_same< T, const int64_t >::value) {
        return ArgType::kInt64;
    }
    if (std::is_same< T, uint64_t >::value || std::is_same< T, const uint64_t >::value) {
        return ArgType::kUint64;
    }
    if (std::is_same< T, float >::value || std::is_same< T, const float >::value) {
        return ArgType::kFloat;
    }
    if (std::is_same< T, double >::value || std::is_same< T, const double >::value) {
        return ArgType::kDouble;
    }
    if (std::is_same< T, char* >::value || std::is_same< T, const char* >::value
        || std::is_same< T, const std::string& >::value || std::is_same< T, const std::string >::value
        || std::is_same< T, std::string& >::value || std::is_same< T, std::string >::value) {
        return ArgType::kString;
    }
    return ArgType::kNone;
}

template < typename T >
inline T cast_string(const std::string& s)
{
    throw std::logic_error("cant not parse unkown type");
}

template <>
inline int8_t cast_string(const std::string& s)
{
    return static_cast< int8_t >(std::stoi(s));
}

template <>
inline uint8_t cast_string(const std::string& s)
{
    return static_cast< uint8_t >(std::stoi(s));
}
template <>
inline int16_t cast_string(const std::string& s)
{
    return static_cast< int16_t >(std::stoi(s));
}

template <>
inline uint16_t cast_string(const std::string& s)
{
    return static_cast< uint16_t >(std::stoi(s));
}

template <>
inline int32_t cast_string(const std::string& s)
{
    return std::stoi(s);
}

template <>
inline uint32_t cast_string(const std::string& s)
{
    return static_cast< uint32_t >(std::stoi(s));
}

template <>
inline int64_t cast_string(const std::string& s)
{
    return std::stoll(s);
}

template <>
inline uint64_t cast_string(const std::string& s)
{
    return std::stoull(s);
}

template <>
inline float cast_string(const std::string& s)
{
    return std::stof(s);
}

template <>
inline double cast_string(const std::string& s)
{
    return std::stod(s);
}

template <>
inline bool cast_string(const std::string& s)
{
    return true;
}

template <>
inline std::string cast_string(const std::string& s)
{
    return s;
}

template < typename T >
class single_type : public ValueType
{
public:
    bool IsArry() const override { return false; }
    ValueType* DefaultValue(T value)
    {
        value_      = value;
        hasDefault_ = true;
        return this;
    }

    T Value() const { return value_; }

    std::string default_value_str() const override
    {
        std::ostringstream oss;
        oss << std::boolalpha << value_;
        return oss.str();
    }

    ArgType Type() const override { return ToArgType< T >(); }

    void FromString(const std::vector< std::string >& args) override
    {
        if (args.empty() && (std::is_same< T, bool >::value || std::is_same< T, const bool >::value)) {
            value_ = true;
        } else {
            value_ = cast_string< T >(args[0]);
        }
    }

private:
    T value_;
};

template < typename T >
class array_type : public ValueType
{
public:
    bool IsArry() const override { return true; }
    ValueType* DefaultValue(const std::vector< T >& value)
    {
        value_      = value;
        hasDefault_ = true;
        return this;
    }

    std::string default_value_str() const override
    {
        std::ostringstream oss;
        oss << "{";
        for (auto& ele : value_) {
            oss << ele << ",";
        }
        oss << "}";
        return oss.str();
    }

    ArgType Type() const override { return ToArgType< T >(); }

    std::vector< T > Value() const { return value_; }

    void FromString(const std::vector< std::string >& args) override
    {
        value_.clear();
        for (auto& arg : args) {
            value_.push_back(cast_string< T >(arg));
        }
    }

private:
    std::vector< T > value_;
};

template < typename C >
struct is_vector : std::false_type
{
    typedef C ArgType;
};
template < typename T, typename A >
struct is_vector< std::vector< T, A > > : std::true_type
{
    static_assert(is_option_args_type< T >::value, "element type of vector must be option args type");
    typedef T ArgType;
};

template < typename T >
inline typename std::enable_if< is_option_args_type< T >::value, single_type< T >* >::type Value()
{
    return new single_type< T >();
}

template < typename T >
inline typename std::enable_if< is_vector< T >::value, array_type< typename is_vector< T >::ArgType >* >::type Value()
{
    return new array_type< typename is_vector< T >::ArgType >();
}

template < typename T >
inline typename std::enable_if< is_option_args_type< T >::value, single_type< T >* >::type ValueCast(ValueType* value)
{
    if (ToArgType< T >() != value->Type()) {
        throw std::runtime_error("single cast type error");
    }
    return dynamic_cast< single_type< T >* >(value);
}

template < typename T >
inline typename std::enable_if< is_vector< T >::value, array_type< typename is_vector< T >::ArgType >* >::type
ValueCast(ValueType* value)
{
    if (ToArgType< typename is_vector< T >::ArgType >() != value->Type()) {
        throw std::runtime_error("array cast type error");
    }
    return dynamic_cast< array_type< typename is_vector< T >::ArgType >* >(value);
}

static std::string& Trim(std::string& s)
{
    if (s.empty()) {
        return s;
    }
    s.erase(0, s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

static std::vector< std::string > split(const std::string& str, const std::string& pattern, bool trim_empty)
{
    std::vector< std::string > ret;
    if (pattern.empty()) {
        return ret;
    }
    size_t start = 0;
    size_t index = str.find_first_of(pattern, 0);
    while (index != str.npos) {
        if (start != index) {
            auto tmpStr = str.substr(start, index - start);
            if (trim_empty) {
                ret.push_back(Trim(tmpStr));
            } else {
                ret.push_back(tmpStr);
            }
        }
        start = index + pattern.size();
        index = str.find_first_of(pattern, start);
    }
    if (!str.substr(start).empty()) {
        auto tmpStr = str.substr(start);
        if (trim_empty) {
            ret.push_back(Trim(tmpStr));
        } else {
            ret.push_back(tmpStr);
        }
    }
    return ret;
}

static std::string set_width(const std::string& s, std::size_t width, bool left)
{
    if (s.size() >= width) {
        return s + std::string("\n") + std::string(width, ' ');
    }
    if (left) {
        std::string result(s);
        for (std::size_t i = s.size(); i < width; ++i) {
            result.append(" ");
        }
        return result;
    } else {
        std::string result;
        for (std::size_t i = s.size(); i < width; ++i) {
            result.append(" ");
        }
        return result + s;
    }
}

struct option_arg
{
    std::string short_name;
    std::string long_name;
    std::string raw_name;
    std::string description = "";
    ValueType* value        = nullptr;

    option_arg(const option_arg& other)
        : short_name(other.short_name)
        , long_name(other.long_name)
        , raw_name(other.raw_name)
        , description(other.description)
        , value(other.value)
    {
    }

    option_arg(const std::string& stShortNameArg,
               const std::string& long_name_arg,
               const std::string& raw_name_arg,
               const std::string& description_arg = "",
               ValueType* value_arg               = nullptr)
        : short_name(stShortNameArg)
        , long_name(long_name_arg)
        , raw_name(raw_name_arg)
        , description(description_arg)
        , value(value_arg)
    {
    }

    typedef std::shared_ptr< option_arg > s_ptr;

    inline std::string key() const { return short_name + long_name; }

    inline std::string help_info() const
    {
        std::string result;
        if (!short_name.empty()) {
            result += short_name;
            result += " ";
        }
        if (!long_name.empty()) {
            if (!result.empty()) {  // has short name
                result += "[ ";
                result += long_name;
                result += " ]";
            } else {
                result += long_name;
            }
        }
        if (value->Type() != ArgType::kBool && short_name != "-h" && long_name != "--help") {
            result += " arg";
        }

        if (value->has_default()) {
            result += " (=";
            result += value->default_value_str();
            result += ")";
        }

        return set_width(result, 50, true) + description;
    }
};

class Command_Line_Parser
{
public:
    Command_Line_Parser() { add_option("h,help", value< bool >(), "print help info"); }
    ~Command_Line_Parser()
    {
        for (auto& argKey : optionArgKeyValues_) {
            delete argKey.second->value;
        }
    }
    void parse(int argc, const char* argv[])
    {
        parse_help();
        auto isKey = [](const std::string& stOptionName) -> bool {
            if (stOptionName.size() == 2) {
                if (stOptionName[0] == '-' && stOptionName[1] != '-') {
                    return true;
                }
                return false;
            }
            if (stOptionName.size() == 3) {
                if (stOptionName[0] == '-' && stOptionName[1] == '-') {
                    throw std::logic_error("option name invalid");
                }
                return false;
            }
            if (stOptionName.size() >= 4) {
                if (stOptionName[0] == '-' && stOptionName[1] == '-' && stOptionName[2] != '-') {
                    return true;
                }
                return false;
            }

            return false;
        };

        std::unordered_map< std::string, std::vector< std::string > > keyParams;
        int i = 1;
        while (i < argc) {
            if (isKey(argv[i])) {
                break;
            }
            ++i;
        }
        std::string stLastKey;
        while (i < argc) {
            const std::string arg = argv[i];
            if (isKey(arg)) {
                stLastKey            = arg;
                keyParams[stLastKey] = {};
            } else {
                keyParams[stLastKey].push_back(arg);
            }
            ++i;
        }

        parse_key_params(keyParams);
    }

    void add_option(const std::string& name, ValueType* ValueType, const std::string& description)
    {
        auto optionArg = parse_option_arg(name);
        auto iter      = optionArgKeyValues_.find(optionArg->key());
        if (iter != optionArgKeyValues_.end()) {
            throw std::logic_error("repeated option name");
        }
        optionArg->description                = description;
        optionArg->value                      = ValueType;
        optionArgKeyValues_[optionArg->key()] = optionArg;
        if (!optionArg->short_name.empty()) {
            optionArgShortValues_[optionArg->short_name] = optionArg;
        }
        if (!optionArg->long_name.empty()) {
            optionArgLongValues_[optionArg->long_name] = optionArg;
        }
    }
    std::string help_string() const { return stHelpString_; }

    bool exist(const std::string& name)
    {
        const auto stOptionName = get_option_name(name);
        auto iter_parsed_short  = optionArgParsedShortValues_.find(stOptionName);
        if (iter_parsed_short != optionArgParsedShortValues_.end()) {
            return true;
        }
        auto iter_parsed_long = optionArgParsedLongValues_.find(stOptionName);
        if (iter_parsed_long != optionArgParsedLongValues_.end()) {
            return true;
        }

        auto iter_short = optionArgShortValues_.find(stOptionName);
        if (iter_short != optionArgShortValues_.end() && iter_short->second->value->has_default()) {
            return true;
        }

        auto iter_long = optionArgLongValues_.find(stOptionName);
        if (iter_long != optionArgLongValues_.end() && iter_long->second->value->has_default()) {
            return true;
        }
        return false;
    }

    template < typename T >
    T get_option_value(const std::string& name)
    {
        const auto stOptionName = get_option_name(name);
        if (!exist(name)) {
            throw std::runtime_error("can not find option " + stOptionName);
        }
        auto iter_short             = optionArgShortValues_.find(stOptionName);
        auto iter_long              = optionArgLongValues_.find(stOptionName);
        option_arg::s_ptr optionArg = nullptr;
        if (iter_short != optionArgShortValues_.end()) {
            optionArg = iter_short->second;
        }
        if (iter_long != optionArgLongValues_.end()) {
            optionArg = iter_long->second;
        }
        if (optionArg == nullptr) {
            throw std::runtime_error("can not find option " + stOptionName);
        }
        return ValueCast< T >(optionArg->value)->Value();
    }

private:
    option_arg::s_ptr parse_option_arg(const std::string& name)
    {
        auto stOptionNames = split(name, ",", true);
        if (stOptionNames.size() > 2 || stOptionNames.empty()) {
            throw std::logic_error("option name invalid");
        }
        if (stOptionNames.size() == 1) {
            const auto stOptionName = get_option_name(stOptionNames[0]);
            if (stOptionName.size() == 2) {
                return std::make_shared< option_arg >(stOptionName, "", name);
            }
            (stOptionName.size() > 2) { return std::make_shared< option_arg >("", stOptionName, name); }

            throw std::logic_error("option name invalid");
        }

        const auto& stOptionName1 = stOptionNames[0];
        const auto& stOptionName2 = stOptionNames[1];
        if (stOptionName1.size() > 1 && stOptionName2.size() > 1) {
            throw std::logic_error("option name invalid");
        }

        if (stOptionName1.size() == 1 && stOptionName2.size() == 1) {
            throw std::logic_error("option name invalid");
        }

        if (stOptionName1.size() == 1) {
            return std::make_shared< option_arg >(get_option_name(stOptionName1), get_option_name(stOptionName2), name);
        }

        if (stOptionName2.size() == 1) {
            return std::make_shared< option_arg >(get_option_name(stOptionName2), get_option_name(stOptionName1), name);
        }

        throw std::logic_error("option name invalid");
        return std::make_shared< option_arg >("", "", name);
    }
    void parse_key_params(const std::unordered_map< std::string, std::vector< std::string > >& key_params)
    {
        for (auto& key_param : key_params) {
            const auto& stOptionName    = key_param.first;
            auto iter_short             = optionArgShortValues_.find(stOptionName);
            auto iter_long              = optionArgLongValues_.find(stOptionName);
            option_arg::s_ptr optionArg = nullptr;
            if (iter_short != optionArgShortValues_.end()) {
                optionArg = iter_short->second;
            }
            if (iter_long != optionArgLongValues_.end()) {
                optionArg = iter_long->second;
            }
            if (optionArg == nullptr) {
                throw std::logic_error("unkown option " + stOptionName);
            }

            const auto& params = key_param.second;
            if (!optionArg->value->IsArry()) {
                if (optionArg->value->Type() == ArgType::kBool) {
                    if (!params.empty()) {
                        throw std::logic_error("option " + stOptionName
                                               + " params size error, bool switch must not has params");
                    }
                } else {
                    if (params.empty() || params.size() != 1) {
                        throw std::logic_error("option " + stOptionName + " params size error, shouble be single");
                    }
                }
            } else {
                if (params.empty()) {
                    throw std::logic_error("option " + stOptionName + " params size error");
                }
            }
            if (!optionArg->short_name.empty()) {
                optionArgParsedShortValues_[optionArg->short_name] = optionArg;
            }
            if (!optionArg->long_name.empty()) {
                optionArgParsedLongValues_[optionArg->long_name] = optionArg;
            }
            optionArg->value->FromString(params);
        }
    }
    std::string get_option_name(const std::string& stOptionName)
    {
        if (stOptionName.size() == 1) {
            return "-" + stOptionName;
        }
        if (stOptionName.size() > 1) {
            return "--" + stOptionName;
        }
        throw std::logic_error("option name invalid");
    }
    void parse_help()
    {
        std::ostringstream oss;
        oss << "Options:" << std::endl;
        for (auto& optionArg : optionArgKeyValues_) {
            oss << "  " << optionArg.second->help_info() << std::endl;
        }
        stHelpString_ = oss.str();
    }

private:
    std::unordered_map< std::string, option_arg::s_ptr > optionArgKeyValues_;
    std::unordered_map< std::string, option_arg::s_ptr > optionArgShortValues_;
    std::unordered_map< std::string, option_arg::s_ptr > optionArgLongValues_;
    std::unordered_map< std::string, option_arg::s_ptr > optionArgParsedShortValues_;
    std::unordered_map< std::string, option_arg::s_ptr > optionArgParsedLongValues_;
    std::string stHelpString_;
};

}  // namespace arg_parser
}  // namespace ucm
}  // namespace ara

#endif
// NOLINTEND