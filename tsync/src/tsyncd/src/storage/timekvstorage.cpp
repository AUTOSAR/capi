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
/// @file       timekvstorage.cpp
/// @brief      time KV persistence management class
/// @details
/// @date       2023-01-12
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/storage/timekvstorage.h"

#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace storage {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief constructor
///
/// @param modelIdentifier instance descriptor
TimeKVStorage::TimeKVStorage(ara::core::StringView const &modelIdentifier) noexcept
    : kvsInstanceSpecifier_{modelIdentifier}, kvs_{}, splitBit_{";"}
{
}

/// @brief open persistence library
/// @return true, success; false, failure;
bool TimeKVStorage::Open() noexcept
{
    ara::core::Result< ara::per::SharedHandle< ara::per::KeyValueStorage > > const res{
        ara::per::OpenKeyValueStorage(kvsInstanceSpecifier_)};
    if (res) {
        kvs_ = res.Value();
        return true;
    }
    LOG().Error() << "OpenKeyValueStorage failed, instance:" << kvsInstanceSpecifier_.ToString()
                  << ", error:" << res.Error().Message();
    return false;
}

/// @brief persist time base content
/// @param key - persistent key value
/// @param userData - user data
/// @param rateDeviation - rate deviation
/// @param lastGlobalNano - last set global time value
/// @return true, success; false, failure;
bool TimeKVStorage::StoreTime(ara::core::StringView const &key,
                              ara::core::String const &userData,
                              const double rateDeviation,
                              std::uint64_t const &lastGlobalNano) noexcept
{
    ara::core::String const content{ara::core::to_string(rateDeviation) + ara::core::String(splitBit_)
                                    + ara::core::to_string(lastGlobalNano) + ara::core::String(splitBit_) + userData};

    LOG().Debug() << "TimeKVStorage::StoreTime, key=" << key << ", content=" << content;
    ara::core::Result< void > res{kvs_->SetValue< TimeKVStorage::ValueType >(key, content)};
    if (res.HasValue()) {
        res = kvs_->SyncToStorage();
    }
    if (!res.HasValue()) {
        LOG().Error() << "TimeKVStorage::StoreTime ,error:" << res.Error().Message()
                      << ", instance=" << kvsInstanceSpecifier_.ToString();
    }
    return res.HasValue();
}

/// @brief retrieve time base content from persistent storage
/// @param key - persistent key value
/// @param userData - user data
/// @param rateDeviation - rate deviation
/// @param lastGlobalNano - last set global time value
/// @return true, success; false, failure;
bool TimeKVStorage::GetTime(ara::core::StringView const &key,
                            ara::core::String &userData,
                            double &rateDeviation,
                            std::uint64_t &lastGlobalNano) noexcept
{
    std::ignore = rateDeviation;
    std::ignore = lastGlobalNano;
    ara::core::Result< ara::core::String > const res{kvs_->GetValue< TimeKVStorage::ValueType >(key)};
    if (res) {
        LOG().Debug() << "TimeKVStorage::GetTime(), get a value for key:" << key << ", value=" << res.Value();

        std::string content{res.Value().c_str()};

        std::stringstream timeStream{content};
        std::string line;
        ara::core::Vector< std::string > strVec;

        while (std::getline(timeStream, line, splitBit_.front())) {
            strVec.push_back(line);
        }
        // userdata not set, size is 2
        if (internal::kTS_NUM_2 > strVec.size()) {
            LOG().Error() << "TimeKVStorage::GetTime(), value is illegal, size=" << strVec.size();
            return false;
        }
        std::size_t strIndex{0U};
        try {
            rateDeviation = std::stod(strVec[strIndex]);
        } catch (std::exception const &e) {
            LOG().Error() << "TimeKVStorage::GetTime(), get rateDeviation error:" << e.what()
                          << ", value:" << strVec[strIndex];
            return false;
        }

        strIndex++;
        try {
            lastGlobalNano = std::stoul(strVec[strIndex]);
        } catch (std::exception const &e) {
            LOG().Error() << "TimeKVStorage::GetTime(), get lastGlobalNano error:" << e.what()
                          << ", value:" << strVec[strIndex];
            return false;
        }
        strIndex++;
        bool bFirst{true};
        for (; strIndex < strVec.size(); strIndex++) {
            if (!bFirst) {
                static_cast< void >(userData.append(splitBit_));
            }
            static_cast< void >(userData.append(strVec[strIndex].c_str()));
            bFirst = false;
        }
        bool const matchSplit{splitBit_.front() == content.at(content.length() - 1U)};
        if ((strVec.size() > internal::kTS_NUM_2) && (matchSplit)) {
            static_cast< void >(userData.append(splitBit_));
        }
        LOG().Debug() << "TimeKVStorage::GetTime(), userData:" << userData << ", rateDeviation:" << rateDeviation
                      << ", lastGlobalNano:" << lastGlobalNano;
        return true;
    }

    LOG().Info() << "TimeKVStorage::GetTime(), can't get a value for key:" << key
                 << ", error:" << res.Error().Message();
    return false;
}

}  // namespace storage
}  // namespace internal
}  // namespace tsync
}  // namespace ara
