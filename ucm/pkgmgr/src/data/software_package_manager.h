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
/// @file       software_package_manager.h
/// @brief      Software package manager
/// @details
/// @date       2024-08-15
/// @author     cuiyinli
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @export_level=/UCM/SoftwarePackageManager
/// @module_path=/UCM/SoftwarePackageManager
/// @interface_level=module
/// @trace_id_sr=SR_UCM_00001
/// @unit_name=SoftwarePackageManager
/// @unit_description=Manager of Software Package, to transfer software package data, extract software package, parse software package
/// @endcode
///
/// ================================================================

#ifndef ARA_UCM_PKGMGR_DATA_SOFTWARE_PACKAGE_MANAGER_H_
#define ARA_UCM_PKGMGR_DATA_SOFTWARE_PACKAGE_MANAGER_H_

#include <memory>

#include "ara/ucm/internal/extraction/software_package_extractor.h"
#include "ara/ucm/internal/transfer/streamable_software_package.h"
#include "common/alias.h"
#include "parsing/software_package_parser.h"
#include "util/synchronized_storage.h"

namespace ara {
namespace ucm {
namespace pkgmgr {

/// @brief class of Software Package Parser
class SoftwarePackageParser;
/// @brief class of Software Package
struct SoftwarePackage;
/// @brief class of FileSystemSWCLManager
class FileSystemSWCLManager;

/// @brief Manager of Software Package, to transfer software package data,
/// extracte software package, parse software package.
/// @code{.isoft}
/// @interface_level=unit
/// @trace_id_ad=AD_UCM_10325
/// @trace_id_dd=DD_UCM_10819
/// @needwork = ad
/// @endcode
class SoftwarePackageManager
{
public:
    /// @brief constructor
    /// @param softwarePackageExtractor
    /// @param softwarePackageParser
    /// @param packagesData
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10326
    /// @trace_id_dd=DD_UCM_10820
    /// @needwork = ad
    /// @endcode
    SoftwarePackageManager(
        std::unique_ptr< SoftwarePackageExtractor > softwarePackageExtractor,
        std::unique_ptr< SoftwarePackageParser > softwarePackageParser,
        std::unique_ptr< SynchronizedStorage< TransferIdType, StreamableSoftwarePackage > > packagesData) noexcept;
    /// @brief destructor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10327
    /// @trace_id_dd=DD_UCM_10821
    /// @needwork = ad
    /// @endcode
    virtual ~SoftwarePackageManager() = default;

    /// @brief delete copy construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10328
    /// @trace_id_dd=DD_UCM_10822
    /// @needwork = ad
    /// @endcode
    SoftwarePackageManager(SoftwarePackageManager const& other) = delete;
    /// @brief delete copy asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10329
    /// @trace_id_dd=DD_UCM_10823
    /// @needwork = ad
    /// @endcode
    SoftwarePackageManager& operator=(SoftwarePackageManager const& other) = delete;
    /// @brief delete move construct
    /// @param other other class object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10330
    /// @trace_id_dd=DD_UCM_10824
    /// @needwork = ad
    /// @endcode
    SoftwarePackageManager(SoftwarePackageManager&& other) = delete;
    /// @brief delete move asign
    /// @param other other class object
    /// @return ref
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10331
    /// @trace_id_dd=DD_UCM_10825
    /// @needwork = ad
    /// @endcode
    SoftwarePackageManager& operator=(SoftwarePackageManager&& other) = delete;

    /// @brief Delete a transferred Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The result of operation, which specifies if the
    /// request was successful.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10332
    /// @trace_id_dd=DD_UCM_10826
    /// @needwork = ad
    /// @endcode
    virtual FutureVoid DeleteTransfer(TransferIdType const& id) noexcept;

    /// @brief Block-wise transfer of a Software Package.
    ///
    /// @param id Transfer ID of the currently running request.
    /// @param data Data block of the Software Package.
    /// @param blockCounter Block counter of the current block.
    ///
    /// @return The result of transferring current data block, which specifies if the
    /// sw package has been successfully transferred.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10333
    /// @trace_id_dd=DD_UCM_10827
    /// @trace_id_sr=SR_UCM_00014
    /// @needwork = ad
    /// @endcode
    virtual FutureVoid TransferData(TransferIdType const& id,
                                    ByteVectorType const& data,
                                    std::uint64_t const& blockCounter) noexcept;

    /// @brief Finish the transfer of a Software Package.
    ///
    /// @param id Transfer ID of the currently running request.
    ///
    /// @return The result of finishing this sw package transfer, which specifies if the
    /// sw package has been successfully transferred.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10334
    /// @trace_id_dd=DD_UCM_10828
    /// @trace_id_sr=SR_UCM_00014
    /// @needwork = ad
    /// @endcode
    virtual FutureVoid TransferExit(TransferIdType const& id, FileSystemSWCLManager* swclManager) noexcept;

    /// @brief Start the transfer of a Software Package.
    /// Transfer Id for subsequent calls to TransferData will be generated
    /// and returned as a part of TransferStartOutput.
    ///
    /// @param size Size (in bytes) of the Software Package to be transferred.
    ///
    /// @return The struct which contains the result of the transfer start operation
    /// and transfer id generated for this operation.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10335
    /// @trace_id_dd=DD_UCM_10829
    /// @trace_id_sr=SR_UCM_00014
    /// @needwork = ad
    /// @endcode
    virtual FutureTransferStart TransferStart(std::uint64_t size) noexcept;

    /// @brief Get the progress of the currently processed Software Package.
    ///
    /// @param id Transfer ID of Software Package.
    ///
    /// @return The progress of the current package processing (0x00 - 0x64).
    ///
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10336
    /// @trace_id_dd=DD_UCM_10830
    /// @trace_id_sr=SR_UCM_00023
    /// @needwork = ad
    /// @endcode
    virtual FutureGetSwProcessProgress GetSwProcessProgress(TransferIdType const& id) const noexcept;

    /// @brief Retrieve a Software Package list.
    ///
    /// @return List of all Software Packages that have been successfully
    /// transferred and are ready to be installed.
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10337
    /// @trace_id_dd=DD_UCM_10831
    /// @needwork = ad
    /// @endcode
    virtual FutureGetSwPackages GetSwPackages() const noexcept;

    /// @brief Recover Packages Data
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10338
    /// @trace_id_dd=DD_UCM_10832
    /// @needwork = ad
    /// @endcode
    void RecoverPackagesData() const noexcept;

    /// @brief Recover Packages Data
    /// @param id Transfer ID of Software Package
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10339
    /// @trace_id_dd=DD_UCM_10833
    /// @needwork = ad
    /// @endcode
    void CleanupAfterExtraction(TransferIdType const& id) const noexcept;

    /// @brief Recover Packages Data
    /// @param streaming Software Package streaming
    /// @param streamingId Software Package streaming id
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10340
    /// @trace_id_dd=DD_UCM_10834
    /// @needwork = ad
    /// @endcode
    virtual void SetStreaming(std::unique_ptr< AraPromiseVoid > streaming, TransferIdType streamingId) noexcept;
    /// @brief GetStreamingLock
    /// @return streamingLock_
    /// @throws no
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10341
    /// @trace_id_dd=DD_UCM_10835
    /// @needwork = ad
    /// @endcode
    std::unique_lock< std::mutex > GetStreamingLock() noexcept
    {
        return std::unique_lock< std::mutex >(streamingLock_);
    }

    // getter for testing/mocking. extract the member completely?
    /// @brief Get Extractor
    /// @throws no
    /// @return SoftwarePackageExtractor
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10342
    /// @trace_id_dd=DD_UCM_10836
    /// @needwork = ad
    /// @endcode
    SoftwarePackageExtractor& GetExtractor() const { return *extractor_; }
    /// @brief Get Parser
    /// @throws no
    /// @return SoftwarePackageParser
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10343
    /// @trace_id_dd=DD_UCM_10837
    /// @needwork = ad
    /// @endcode
    SoftwarePackageParser& GetParser() const { return *parser_; }
    /// @brief Get Packages Data
    /// @throws no
    /// @return packages data
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_10344
    /// @trace_id_dd=DD_UCM_10838
    /// @needwork = ad
    /// @endcode
    SynchronizedStorage< TransferIdType, StreamableSoftwarePackage >& GetPackagesData() const { return *packagesData_; }

private:
    /// @brief delete transfer
    /// @param id the id of the transfer
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10839
    /// @needwork = dda
    /// @endcode
    AraResultVoid _deleteTransfer(TransferIdType const& id) const noexcept;

    /// @brief Task which will be executed on another thread if the transfer exit
    /// is successful
    /// @param id the id of the transfer
    /// the TransferExit function
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10840
    /// @needwork = dda
    /// @endcode
    AraResultVoid _transferExitSuccessTask(TransferIdType const& id,
                                           FileSystemSWCLManager* const swclManager) const noexcept;

    /// @brief Function which decompresses a software cluster package archive
    /// @param packageFilename
    /// @param extractionDir
    /// @param id the id of the transfer
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10841
    /// @needwork = dda
    /// @endcode
    AraResultVoid _extractSwPackage(AraString const& packageFilename,
                                    AraString const& extractionDir,
                                    TransferIdType const& id) const noexcept;

    /// @brief Function which parses a software cluster package located at
    /// extractionDirectory
    /// @param extractionDir Where the decompressed software package is
    /// located
    /// @param id the id of the transfer
    /// @return result
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10842
    /// @needwork = dda
    /// @endcode
    AraResult< std::unique_ptr< SoftwarePackage > > _parseSwPackage(AraString const& extractionDir,
                                                                    TransferIdType const& id) const noexcept;

private:
    /// @brief Implements extraction of the a software package
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10843
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SoftwarePackageExtractor > extractor_{nullptr};

    /// @brief Encapsulates the parsing of a received archive into a software
    /// package object
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10844
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SoftwarePackageParser > parser_{nullptr};

    /// @brief Container with information about software packages at work
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10845
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< SynchronizedStorage< TransferIdType, StreamableSoftwarePackage > > packagesData_{nullptr};

    /// @brief process the streaming software package
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10846
    /// @needwork = dda
    /// @endcode
    std::unique_ptr< AraPromiseVoid > streaming_{nullptr};
    /// @brief the streaming id
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10847
    /// @needwork = dda
    /// @endcode
    TransferIdType streamingId_{0U};
    /// @brief the streaming lock
    /// @code{.isoft}
    /// @interface_level=unit
    /// @trace_id_ad=AD_UCM_00000
    /// @trace_id_dd=DD_UCM_10848
    /// @needwork = dda
    /// @endcode
    std::mutex streamingLock_{};
};

}  // namespace pkgmgr
}  // namespace ucm
}  // namespace ara

#endif  // ARA_UCM_PKGMGR_DATA_SOFTWARE_PACKAGE_MANAGER_H_
