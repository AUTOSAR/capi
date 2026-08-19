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
/// @file       timefilestorage.cpp
/// @brief      time file persistence management class
/// @details
/// @date       2023-01-12
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================

#include "ara/tsync/internal/storage/timefilestorage.h"

#include <ara/core/vector.h>

#include <iostream>
#include <sstream>
#include <string>

#include "ara/tsync/internal/log/logger.h"

namespace ara {
namespace tsync {
namespace internal {
namespace storage {

/// @brief log output
/// @return Logger object reference
static inline ara::tsync::internal::tslog::Logger &LOG() noexcept { return ara::tsync::internal::tslog::Log(); }

/// @brief constructor
/// @param modelIdentifier instance descriptor
/// @param fileName persistent file name
TimeFileStorage::TimeFileStorage(ara::core::StringView const &modelIdentifier,
                                 ara::core::StringView const &fileName) noexcept
    : fileInstanceSpecifier_{modelIdentifier}, fileName_{fileName}, fileStorage_{}, fileAccessor_{}, splitBit_{";"}
{
}

/// @brief open persistence library
/// @return true, success; false, failure;
bool TimeFileStorage::Open() noexcept
{
    ara::core::Result< ara::per::SharedHandle< ara::per::FileStorage > > res{
        ara::per::OpenFileStorage(fileInstanceSpecifier_)};
    if (res.HasValue()) {
        fileStorage_ = std::move(res).Value();
        ara::core::Result< ara::per::UniqueHandle< ara::per::ReadWriteAccessor > > resultOpenFile{
            fileStorage_->OpenFileReadWrite(fileName_)};
        if (resultOpenFile.HasValue()) {
            fileAccessor_ = std::move(resultOpenFile).Value();
        } else {
            LOG().Error() << "OpenFileReadWrite failed, instance:" << fileName_ << ", error:" << res.Error().Message();
            return false;
        }
        return true;
    }
    LOG().Error() << "OpenFileStorage failed, instance:" << fileInstanceSpecifier_.ToString()
                  << ",error:" << res.Error().Message();
    return false;
}

/// @brief persist time base content
/// @param userData - user data
/// @param rateDeviation - rate deviation
/// @param lastGlobalNano - last set global time value
/// @return true, success; false, failure;
bool TimeFileStorage::StoreTime(ara::core::String const &userData,
                                double const rateDeviation,
                                std::uint64_t const &lastGlobalNano) noexcept
{
    ara::core::String const content{ara::core::to_string(rateDeviation) + ara::core::String(splitBit_)
                                    + ara::core::to_string(lastGlobalNano) + ara::core::String(splitBit_) + userData};

    LOG().Debug() << "TimeFileStorage::StoreTime:" << content;
    /// Overwrite existing content each time
    std::ignore = fileAccessor_->SetPosition(0U);
    ara::core::StringView contentView{content.c_str()};
    ara::core::Result< void > res{fileAccessor_->WriteText(std::move(contentView))};
    bool const bDoneOk{res.HasValue()};
    if (bDoneOk) {
        res = fileAccessor_->SyncToFile();
    } else {
        LOG().Error() << "TimeFileStorage::StoreTime ,error:" << res.Error().Message()
                      << ", instance=" << fileInstanceSpecifier_.ToString();
    }
    return bDoneOk;
}

/// @brief retrieve time base content from persistent storage
/// @param userData - user data
/// @param rateDeviation - rate deviation
/// @param lastGlobalNano - last set global time value
/// @return true, success; false, failure;
bool TimeFileStorage::GetTime(ara::core::String &userData,
                              double &rateDeviation,
                              std::uint64_t &lastGlobalNano) noexcept
{
    std::ignore = rateDeviation;
    std::ignore = lastGlobalNano;
    ara::core::Result< ara::core::String > const res{fileAccessor_->ReadText()};
    if (res.HasValue()) {
        LOG().Debug() << "TimeFileStorage::GetTime(), get a value:" << res.Value()
                      << ", instance =" << fileInstanceSpecifier_.ToString();
        std::string content{res.Value().c_str()};
        std::stringstream timeStream{content};
        std::string line;
        ara::core::Vector< std::string > strVec;

        while (std::getline(timeStream, line, *(splitBit_.data()))) {
            strVec.push_back(line);
        }

        // userdata not set, size is 2
        if (internal::kTS_NUM_2 > strVec.size()) {
            LOG().Error() << "TimeFileStorage::GetTime(), value is illegal";
            return false;
        }
        std::size_t strIndex{0U};
        try {
            rateDeviation = std::stod(strVec[strIndex]);
        } catch (std::exception const &e) {
            LOG().Error() << "TimeFileStorage::GetTime(), get rateDeviation error:" << e.what()
                          << ", value:" << strVec[strIndex];
            return false;
        }

        strIndex++;
        try {
            lastGlobalNano = std::stoul(strVec[strIndex]);
        } catch (std::exception const &e) {
            LOG().Error() << "TimeFileStorage::GetTime(), get lastGlobalNano error:" << e.what()
                          << ", value:" << strVec[strIndex];
            return false;
        }
        strIndex++;
        bool bFirst{true};
        for (; strIndex < strVec.size(); strIndex++) {
            if (!bFirst) {
                std::ignore = userData.append(splitBit_);
            }
            std::ignore = userData.append(strVec[strIndex].c_str());
            bFirst      = false;
        }
        bool const matchSplit{splitBit_.front() == content.at(content.length() - 1U)};
        if ((strVec.size() > internal::kTS_NUM_2) && (matchSplit)) {
            std::ignore = userData.append(splitBit_);
        }

        LOG().Debug() << "TimeFileStorage::GetTime(), userData:" << userData << ", rateDeviation:" << rateDeviation
                      << ", lastGlobalNano:" << lastGlobalNano;
        return true;
    }
    LOG().Info() << "TimeFileStorage::GetTime(), can't get a value for instance:" << fileInstanceSpecifier_.ToString()
                 << ", error:" << res.Error().Message();
    return {};
}

}  // namespace storage
}  // namespace internal
}  // namespace tsync
}  // namespace ara
