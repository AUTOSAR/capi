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
/// @file       ucm_database.cpp
/// @brief      Definition and implementation of the database for UCM execution actions
/// @details
/// @date       2022-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/UCM/SoftwareClusterManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=UcmDatabase
/// @unit_description=An interface to a database where UCM stores all actions it has performed since its first execution
/// @endcode
///
/// ================================================================

#include "ucm_database.h"

#include "ara/ucm/internal/extraction/tinyfsys.h"
#include "common/log.h"
#include "common/strtype.h"
#include "parsing/version.h"
#include "util/rjson.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief Save the actions performed by the UCM
/// during the current Update sequence
/// @param actionsOfThisUpdateSequence history actions
/// @throws no
/// @return result
AraResultVoid UcmDatabase::SaveLastSnapshot(AraList< GetHistoryType > const& actionsOfThisUpdateSequence) const
{
    LOGD << "call...";

    if (actionsOfThisUpdateSequence.empty()) {
        LOGE << "no new actions to save";
        return {};
    }

    std::int64_t const timestamp{
        std::chrono::duration_cast< std::chrono::milliseconds >(std::chrono::system_clock::now().time_since_epoch())
            .count()};
    std::uint64_t const ts{static_cast< std::uint64_t >(timestamp)};

    std::string seqStr;
    {
        /// representation of one update sequence, which is an array of objects
        rjson::Doc seqDom{rjson::MakeDomArray()};

        for (auto const& it : actionsOfThisUpdateSequence) {
            /// representation of a UCM it with all its related information (val1 to val5 below)
            rjson::Value snapDom{rjson::MakeObject()};
            rjson::AddKN(seqDom, snapDom, "Time", ts);
            rjson::AddKS(seqDom, snapDom, "Name", it.Name);
            rjson::AddKS(seqDom, snapDom, "Version", it.Version);
            rjson::AddKS(seqDom, snapDom, "Action", strtype::ActionTypeToStr(it.Action));
            rjson::AddKS(seqDom, snapDom, "Resolution", strtype::ResultTypeToStr(it.Resolution));
            rjson::PushO(seqDom, seqDom, snapDom);
        }

        seqStr = rjson::Str(seqDom);
    }

    const bool historyFileExist{tinyfsys::DoesFileExist(kHistoryFile)};
    std::fstream fs{};
    if (historyFileExist) {
        fs.open(kHistoryFile.c_str());  // default is std::ios_base::in | std::ios_base::out

        // Set the input position indicator to the end of the file
        std::ignore = fs.seekg(0, std::ios_base::end);

        // Set the output position indicator to the end of the file
        std::ignore = fs.seekp(0, std::ios_base::end);
    } else {
        fs.open(kHistoryFile.c_str(), std::ios_base::out);

        // Set the output position indicator to the end of the file
        std::ignore = fs.seekp(0, std::ios_base::end);
    }

    const bool isOpen{fs.is_open()};
    const bool isGood{fs.good()};
    if ((!isOpen) || (!isGood)) {
        fs.close();
        LOGE << "history database can't be opened properly";
        return AraResultVoid(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }

    /// check if history is empty
    // if not, don't create a new array of objects, just add a new member inside the existing array
    // otherwise the History file will have multiple root elements which is not a valid JSON schema
    if (historyFileExist) {  // UCM History file contains already some former update sequence(s)
        // So, there is a need to remove the last ']' of the file
        // + to remove the first '[' of the new update sequence
        // ==> Necessary trick to add a member in an existing array
        // without having to parse each time the History file in order to place it in a rapidJson object
        LOGD << "history database is not empty, append new actions";
        // remove the first (and only) '[' of the buffer
        std::ignore = seqStr.erase(0U, 1U);
        // set the cursor to be right after the last '}' delimiting last object
        int32_t const jsonOffset{-2};
        char8_t const jsonSep{','};
        std::ignore = fs.seekg(jsonOffset, std::fstream::cur);
        std::ignore = fs.put(jsonSep);
    }

    fs << seqStr;
    fs.close();
    LOGD << "history database is updated";

    return {};
}

/// @brief Retrieve all the actions (and related information) performed by the UCM
/// from the 1st execution of the UCM until the call to this method
/// @throws no
/// @return history actions
AraResult< GetHistoryVectorType > UcmDatabase::GetFullHistory()
{
    LOGD << "call...";

    rjson::Doc doc;
    bool const ret{rjson::OpenAndCheck(doc, kHistoryFile, std::move(LOGE))};
    if (!ret) {
        return AraResult< GetHistoryVectorType >(UcmFilesystemErrc::kNoSuchFileOrDirectory);
    }

    if (!doc.IsArray()) {
        LOGE << "History is not an array";
        return AraResult< GetHistoryVectorType >(UcmFilesystemErrc::kDetectRegularFile);
    }

    for (auto& it : doc.GetArray()) {
        if (!it.IsObject()) {
            LOGE << "it is not object";
            continue;
        }

        fullHistory_.emplace_back(GetHistoryType{it["Time"].GetUint64(), std::move(it["Name"].GetString()),
                                                 std::move(it["Version"].GetString()),
                                                 strtype::ActionTypeFromStr(it["Action"].GetString()),
                                                 strtype::ResultTypeFromStr(it["Resolution"].GetString())});
    }

    return AraResult< GetHistoryVectorType >(fullHistory_);
}

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara
