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
/// @file       ab_partition.h
/// @brief      a/b partition
/// @details
/// @date       2024-01-01
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/FsmManager
/// @module_path=/UCM/FsmManager
/// @interface_level=unit
/// @trace_id_sr=SR_UCM_00001,SR_UCM_00027
/// @unit_name=ABPartition
/// @unit_description=Handles the A/B partition logic of the package manager.
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_FSM_AB_PARTITION_H_
#define ARA_UCM_PKGMGR_FSM_AB_PARTITION_H_

#include "common/alias.h"
#include "parsing/software_cluster.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief a/b partition
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10056
/// @trace_id_dd=DD_UCM_10089
/// @needwork = ad
/// @endcode
class ABPartition
{
public:
    /// @brief constructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10057
    /// @trace_id_dd=DD_UCM_10090
    /// @needwork = ad
    /// @endcode
    ABPartition() = default;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10058
    /// @trace_id_dd=DD_UCM_10091
    /// @needwork = ad
    /// @endcode
    ~ABPartition() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10059
    /// @trace_id_dd=DD_UCM_10092
    /// @needwork = ad
    /// @endcode
    ABPartition(ABPartition const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10060
    /// @trace_id_dd=DD_UCM_10093
    /// @needwork = ad
    /// @endcode
    ABPartition& operator=(ABPartition const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10061
    /// @trace_id_dd=DD_UCM_10094
    /// @needwork = ad
    /// @endcode
    ABPartition(ABPartition&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10062
    /// @trace_id_dd=DD_UCM_10095
    /// @needwork = ad
    /// @endcode
    ABPartition& operator=(ABPartition&& other) = delete;

    /// @brief recover last used id
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10063
    /// @trace_id_dd=DD_UCM_10096
    /// @needwork = ad
    /// @endcode
    void RecoverLastUsedId();
    /// @brief RecoverLastUsedBootOptionFileId
    /// @throws no
    /// @return result of recover
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10064
    /// @trace_id_dd=DD_UCM_10097
    /// @needwork = ad
    /// @endcode
    static std::uint32_t RecoverLastUsedBootOptionFileId();
    /// @brief RecoverLastUsedSWCLListId
    /// @throws no
    /// @return result of recover
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10065
    /// @trace_id_dd=DD_UCM_10098
    /// @needwork = ad
    /// @endcode
    static std::uint32_t RecoverLastUsedSWCLListId();

    /// @brief used in verify executor, to link to b partition
    /// @param activationSwcls
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10066
    /// @trace_id_dd=DD_UCM_10099
    /// @needwork = ad
    /// @endcode
    void BootOptionFileLinkToB(AraList< SoftwareCluster > const& activationSwcls) const;
    /// @brief used in verify executor, to link to b partition
    /// @param activationSwcls
    /// @param nonPlatformCoreUpdated
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10067
    /// @trace_id_dd=DD_UCM_10100
    /// @needwork = ad
    /// @endcode
    void SwclListFileLinkToB(AraList< SoftwareCluster > const& activationSwcls,
                             bool const nonPlatformCoreUpdated) const;

    /// @brief used in cleanup executor, to cleanup a partition
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10068
    /// @trace_id_dd=DD_UCM_10101
    /// @needwork = ad
    /// @endcode
    void BootOptionFileCleanupA();
    /// @brief used in cleanup executor, to cleanup a partition
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10069
    /// @trace_id_dd=DD_UCM_10102
    /// @needwork = ad
    /// @endcode
    void SwclListFileCleanupA();

    /// @brief used in rollback executor, to rollback to A partition and cleanup B partition
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10070
    /// @trace_id_dd=DD_UCM_10103
    /// @needwork = ad
    /// @endcode
    void BootOptionFileRollbackToA() const;
    /// @brief used in rollback executor, to rollback to A partition and cleanup B partition
    /// @param nonPlatformCoreUpdated
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10071
    /// @trace_id_dd=DD_UCM_10104
    /// @needwork = ad
    /// @endcode
    void SwclListFileRollbackToA(bool const nonPlatformCoreUpdated) const;
    /// @brief used in exception rollback executor, to exception rollback to A partition and cleanup B partition
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10072
    /// @trace_id_dd=DD_UCM_10105
    /// @needwork = ad
    /// @endcode
    void BootOptionFileExceptionRollbackToA() const;
    /// @brief used in exception rollback executor, to exception rollback to A partition and cleanup B partition
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10073
    /// @trace_id_dd=DD_UCM_10106
    /// @needwork = ad
    /// @endcode
    void SwclListFileExceptionRollbackToA() const;
    /// @brief BootOptionFileExceptionCleanupB
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10074
    /// @trace_id_dd=DD_UCM_10107
    /// @needwork = ad
    /// @endcode
    bool BootOptionFileExceptionCleanupB() const;
    /// @brief SwclListFileExceptionCleanupB
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10075
    /// @trace_id_dd=DD_UCM_10108
    /// @needwork = ad
    /// @endcode
    bool SwclListFileExceptionCleanupB() const;

    /// @brief CheckCoreVersionAfterReboot
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10076
    /// @trace_id_dd=DD_UCM_10109
    /// @needwork = ad
    /// @endcode
    static bool CheckCoreVersionAfterReboot();
    /// @brief CheckSwclListVersionAfterReboot
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10077
    /// @trace_id_dd=DD_UCM_10110
    /// @needwork = ad
    /// @endcode
    bool CheckSwclListVersionAfterReboot() const;
    /// @brief CheckSwclListVersionWithoutUpdate
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10078
    /// @trace_id_dd=DD_UCM_10111
    /// @needwork = ad
    /// @endcode
    bool CheckSwclListVersionWithoutUpdate() const;

private:
    /// @brief GetId
    /// @param fileName
    /// @param prefix
    /// @param postfix
    /// @throws no
    /// @return file name id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10112
    /// @needwork = dda
    /// @endcode
    static std::int32_t GetId(AraString const& fileName, AraString const& prefix, AraString const& postfix);

    /// @brief GetSortedFileNameIds
    /// @param dir
    /// @param prefix
    /// @param postfix
    /// @throws no
    /// @return the vector of sorted file name ids
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10113
    /// @needwork = dda
    /// @endcode
    static AraVector< std::uint32_t > GetSortedFileNameIds(AraString const& dir,
                                                           AraString const& prefix,
                                                           AraString const& postfix);

    /// @brief GetMinMaxFileNameId
    /// @param dir
    /// @param prefix
    /// @param postfix
    /// @param minId get min file name id
    /// @param maxId get max file name id
    /// @throws no
    /// @return min max file name id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10114
    /// @needwork = dda
    /// @endcode
    static void GetMinMaxFileNameId(AraString const& dir,
                                    AraString const& prefix,
                                    AraString const& postfix,
                                    std::uint32_t& minId,
                                    std::uint32_t& maxId);

    /// @brief GetMinFileNameId
    /// @param dir
    /// @param prefix
    /// @param postfix
    /// @throws no
    /// @return min file name id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10115
    /// @needwork = dda
    /// @endcode
    static std::uint32_t GetMinFileNameId(AraString const& dir, AraString const& prefix, AraString const& postfix);

    /// @brief GetVersionFromSwclStatus
    /// @param swclName
    /// @param onlyGetCurVersion
    /// @throws no
    /// @return swcl version
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10116
    /// @needwork = dda
    /// @endcode
    static AraString GetVersionFromSwclStatus(AraString const& swclName, bool const onlyGetCurVersion = false);

    /// @brief GetVersionFromBootConfig
    /// @param bootOptionFileId
    /// @param frameworkVersion
    /// @param coreVersion
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10117
    /// @needwork = dda
    /// @endcode
    static bool GetVersionFromBootConfig(std::uint32_t const bootOptionFileId,
                                         AraString& frameworkVersion,
                                         AraString& coreVersion);
    /// @brief _getFormerVersionFromBootConfig
    /// @param frameworkVer
    /// @param coreVer
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10118
    /// @needwork = dda
    /// @endcode
    bool _getFormerVersionFromBootConfig(AraString& frameworkVer, AraString& coreVer) const;

    /// @brief GetCoreVersionFromRuntime
    /// @throws no
    /// @return core version
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10119
    /// @needwork = dda
    /// @endcode
    static AraString GetCoreVersionFromRuntime();
    /// @brief GetSwclListVersionFromRuntime
    /// @throws no
    /// @return version
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10120
    /// @needwork = dda
    /// @endcode
    static AraString GetSwclListVersionFromRuntime() noexcept;

    /// @brief GetBootOptionFileName
    /// @param bootOptionFileId
    /// @throws no
    /// @return file name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10121
    /// @needwork = dda
    /// @endcode
    static AraString GetBootOptionFileName(std::uint32_t const bootOptionFileId);
    /// @brief GetSwclListFileName
    /// @param swclListFileId
    /// @throws no
    /// @return file name
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10122
    /// @needwork = dda
    /// @endcode
    static AraString GetSwclListFileName(std::uint32_t const swclListFileId);
    /// @brief GetBootOptionFilePath
    /// @param bootOptionFileId
    /// @throws no
    /// @return file path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10123
    /// @needwork = dda
    /// @endcode
    static AraString GetBootOptionFilePath(std::uint32_t const bootOptionFileId);
    /// @brief GetBootOptionFilePath
    /// @param bootOptionFileName
    /// @throws no
    /// @return file path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10124
    /// @needwork = dda
    /// @endcode
    static AraString GetBootOptionFilePath(AraString const& bootOptionFileName);
    /// @brief GetBootOptionMD5FilePath
    /// @param bootOptionFilePath
    /// @throws no
    /// @return file path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10125
    /// @needwork = dda
    /// @endcode
    static AraString GetBootOptionMD5FilePath(AraString const& bootOptionFilePath);
    /// @brief GetSwclListFilePath
    /// @param swclListFileId
    /// @throws no
    /// @return file path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10126
    /// @needwork = dda
    /// @endcode
    static AraString GetSwclListFilePath(std::uint32_t const swclListFileId);
    /// @brief GetSwclListFilePath
    /// @param swclListFileName
    /// @throws no
    /// @return file path
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10127
    /// @needwork = dda
    /// @endcode
    static AraString GetSwclListFilePath(AraString const& swclListFileName);

    /// @brief LinkSwclList
    /// @param swclListFileId
    /// @param coreVersion
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10128
    /// @needwork = dda
    /// @endcode
    static bool LinkSwclList(std::uint32_t const swclListFileId, AraString const& coreVersion);
    /// @brief _genBootConfig
    /// @param bootOptionFileId
    /// @param frameworkVersion
    /// @param coreVersion
    /// @throws no
    /// @return bool
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10129
    /// @needwork = dda
    /// @endcode
    bool _genBootConfig(std::uint32_t const bootOptionFileId,
                        AraString const& frameworkVersion,
                        AraString const& coreVersion) const;

    /// @brief DeleteBootOptionFile
    /// @param bootOptionFileId
    /// @param mayNotExist
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10130
    /// @needwork = dda
    /// @endcode
    static void DeleteBootOptionFile(std::uint32_t const bootOptionFileId, bool const mayNotExist = false);
    /// @brief DeleteSWCLListFile
    /// @param swclListId
    /// @param mayNotExist
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10131
    /// @needwork = dda
    /// @endcode
    static void DeleteSWCLListFile(std::uint32_t const swclListId, bool const mayNotExist = false);

private:
    /// @brief lastUsedBootOptionFileID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10132
    /// @needwork = dda
    /// @endcode
    std::uint32_t lastUsedBootOptionFileID_;
    /// @brief lastUsedSWCLListID_
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10133
    /// @needwork = dda
    /// @endcode
    std::uint32_t lastUsedSWCLListID_;
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_FSM_AB_PARTITION_H_
