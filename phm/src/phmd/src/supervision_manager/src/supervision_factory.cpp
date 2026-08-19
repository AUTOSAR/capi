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
/// @file       supervision_factory.cpp
/// @brief
/// @details
/// @date       2024-06-20
/// @author     wangyanlong
/// @version    1.2.0
///
/// ================================================================

#include "ara/phm/internal/svmanager/supervision_factory.h"

#include <utility>

#include "ara/phm/internal/phm_log.h"

namespace ara {
namespace phm {
namespace internal {
/// @brief Create SupervisionCheckpoint objects base on conf.
/// @param checkpointConf conf of checkpoint.
/// @return objects of SupervisionCheckpoint.
ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > SupervisionFactory::MakeCheckpoint(
    ara::core::Vector< CheckpointConf > const& checkpointConf) noexcept
{
    LOG_INFO << "make checkpoint, count " << checkpointConf.size();

    ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > ret;
    for (CheckpointConf const& conf : checkpointConf) {
        ret.push_back(std::make_shared< SupervisionCheckpoint >(conf));
    }
    return ret;
}

/// @brief Create AliveSupervision objects base on conf.
/// @param aliveSupervisionConf conf of alive supervision.
/// @param checkpoint The container of checkpoint, find checkpoint referenced by AliveSupervision in it.
/// @return objects of AliveSupervision.
ara::core::Vector< std::shared_ptr< AliveSupervision > > SupervisionFactory::MakeAliveSupervision(
    ara::core::Vector< AliveSupervisionConf > const& aliveSupervisionConf,
    ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > >& checkpoint) noexcept
{
    LOG_INFO << "make alive supervision, count " << aliveSupervisionConf.size();

    ara::core::Vector< std::shared_ptr< AliveSupervision > > ret;
    for (AliveSupervisionConf const& conf : aliveSupervisionConf) {
        if (checkpoint.count(conf.checkpointFqn) == 0U) {
            LOG_FATAL << "SupervisionFactory::MakeAliveSupervision, checkpoint " << conf.checkpointFqn.c_str()
                      << " referenced by alive supervision " << conf.fqn.c_str() << " not exist";
            std::terminate();
            return ret;
        }

        ret.push_back(std::make_shared< AliveSupervision >(conf, checkpoint[conf.checkpointFqn]));
    }
    return ret;
}

/// @brief Create DeadlineSupervision objects base on conf.
/// @param deadlineSupervisionConf conf of deadline supervision.
/// @param checkpoint The container of checkpoint, find checkpoint referenced by DeadlineSupervision in it.
/// @return objects of DeadlineSupervision.
ara::core::Vector< std::shared_ptr< DeadlineSupervision > > SupervisionFactory::MakeDeadlineSupervision(
    ara::core::Vector< DeadlineSupervisionConf > const& deadlineSupervisionConf,
    ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > >& checkpoint) noexcept
{
    LOG_INFO << "make deadline supervision, count " << deadlineSupervisionConf.size();

    ara::core::Vector< std::shared_ptr< DeadlineSupervision > > ret;
    for (DeadlineSupervisionConf const& conf : deadlineSupervisionConf) {
        size_t const temp{2U};
        if (conf.checkpointTransition.size() != temp) {
            LOG_FATAL << "make deadline supervision, transition of deadline supervision " << conf.fqn.c_str()
                      << " size is not 2, it is " << conf.checkpointTransition.size();
            std::terminate();
            return ret;
        }

        if (checkpoint.count(conf.checkpointTransition[0U]) == 0U) {
            LOG_FATAL << "make deadline supervision, checkpoint " << conf.checkpointTransition[0U].c_str()
                      << " referenced by deadline supervision " << conf.fqn.c_str() << " not exist.";
            std::terminate();
            return ret;
        }

        if (checkpoint.count(conf.checkpointTransition[1U]) == 0U) {
            LOG_FATAL << "make deadline supervision, checkpoint " << conf.checkpointTransition[1U].c_str()
                      << " referenced by deadline supervision " << conf.fqn.c_str() << " not exist.";
            std::terminate();
            return ret;
        }
        std::shared_ptr< SupervisionCheckpoint > const sCheckPoint{checkpoint[conf.checkpointTransition[0U]]};
        std::shared_ptr< SupervisionCheckpoint > const tCheckPoint{checkpoint[conf.checkpointTransition[1U]]};
        ret.push_back(std::make_shared< DeadlineSupervision >(conf, sCheckPoint, tCheckPoint));
    }
    return ret;
}

/// @brief Create LogicalSupervision objects base on LogicalSupervisionConf.
/// @param logicalSupervisionConf conf of logical supervision
/// @param checkpoint The container of checkpoint, find checkpoint referenced by LogicalSupervision in it.
/// @return objects of LogicalSupervision.
ara::core::Vector< std::shared_ptr< LogicalSupervision > > SupervisionFactory::MakeLogicalSupervision(
    ara::core::Vector< LogicalSupervisionConf > const& logicalSupervisionConf,
    ara::core::Map< ara::core::String, std::shared_ptr< SupervisionCheckpoint > >& checkpoint) noexcept
{
    LOG_INFO << "make logical supervision, count " << logicalSupervisionConf.size();

    ara::core::Vector< std::shared_ptr< LogicalSupervision > > ret;
    for (LogicalSupervisionConf const& conf : logicalSupervisionConf) {
        ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > initialCheckpoint;
        ara::core::Vector< std::shared_ptr< SupervisionCheckpoint > > finalCheckpoint;
        ///@details <target checkpoint, source checkpoint>
        ara::core::Vector<
            std::pair< std::shared_ptr< SupervisionCheckpoint >, std::shared_ptr< SupervisionCheckpoint > > >
            checkpointTransition;

        for (ara::core::String const& checkpointFqn : conf.initialCheckpoint) {
            if (checkpoint.count(checkpointFqn) == 0U) {
                LOG_FATAL << "SupervisionFactory::MakeLogicalSupervision, checkpoint " << checkpointFqn.c_str()
                          << " referenced by logical supervision " << conf.fqn.c_str() << " not exist.";
                std::terminate();
                return ret;
            }
            initialCheckpoint.push_back(checkpoint[checkpointFqn]);
        }

        for (ara::core::String const& checkpointFqn : conf.finalCheckpoint) {
            if (checkpoint.count(checkpointFqn) == 0U) {
                LOG_FATAL << "SupervisionFactory::MakeLogicalSupervision, initial checkpoint " << checkpointFqn.c_str()
                          << " referenced by " << conf.fqn.c_str() << " not exist.";
                std::terminate();
                return ret;
            }
            finalCheckpoint.push_back(checkpoint[checkpointFqn]);
        }

        for (TransitionConf const& transition : conf.transition) {
            if (checkpoint.count(transition.targetCheckpoint) == 0U) {
                LOG_FATAL << "SupervisionFactory::MakeLogicalSupervision, final checkpoint "
                          << transition.targetCheckpoint.c_str() << " referenced by logical supervision "
                          << conf.fqn.c_str() << " not exist.";
                std::terminate();
                return ret;
            }

            if (checkpoint.count(transition.sourceCheckpoint) == 0U) {
                LOG_FATAL << "SupervisionFactory::MakeLogicalSupervision, transition checkpoint "
                          << transition.sourceCheckpoint.c_str() << " referenced by logical supervision "
                          << conf.fqn.c_str() << " not exist.";
                std::terminate();
                return ret;
            }

            ///@details <targetCheckpoint, sourceCheckpoint>
            std::shared_ptr< SupervisionCheckpoint > const tCheckPoint{checkpoint[transition.targetCheckpoint]};
            std::shared_ptr< SupervisionCheckpoint > const sCheckPoint{checkpoint[transition.sourceCheckpoint]};
            checkpointTransition.push_back(std::make_pair(tCheckPoint, sCheckPoint));
        }

        ret.push_back(
            std::make_shared< LogicalSupervision >(conf, initialCheckpoint, finalCheckpoint, checkpointTransition));
    }
    return ret;
}

/// @brief  Create LocalSupervision objects base on conf.
/// @param localSupervisionConf conf of local supervision.
/// @param baseSupervision The container of base supervision, find base supervision referenced by
/// local supervision in it.
/// @param baseLocalMap key is fqn of base supervision, value is local supervision.
/// @return objects of LocalSupervision.
ara::core::Vector< std::shared_ptr< LocalSupervision > > SupervisionFactory::MakeLocalSupervision(
    ara::core::Vector< LocalSupervisionConf > const& localSupervisionConf,
    ara::core::Map< ara::core::String, std::shared_ptr< BaseSupervision > >& baseSupervision,
    ara::core::Map< ara::core::String, ara::core::Vector< std::shared_ptr< LocalSupervision > > >&
        baseLocalMap) noexcept
{
    LOG_INFO << "make local supervision, count " << localSupervisionConf.size();

    ara::core::Vector< std::shared_ptr< LocalSupervision > > ret;
    for (LocalSupervisionConf const& conf : localSupervisionConf) {
        ara::core::Vector< std::shared_ptr< BaseSupervision > > baseSupervisionReferenced;
        for (ara::core::String const& baseSupervisionFqn : conf.aliveSupervision) {
            if (baseSupervision.count(baseSupervisionFqn) == 0U) {
                LOG_FATAL << "SupervisionFactory::MakeLocalSupervision, alive supervision "
                          << baseSupervisionFqn.c_str() << " referenced by local supervision " << conf.fqn.c_str()
                          << " not exist";
                std::terminate();
                return ret;
            }
            baseSupervisionReferenced.push_back(baseSupervision[baseSupervisionFqn]);
        }

        for (ara::core::String const& baseSupervisionFqn : conf.deadlineSupervision) {
            if (baseSupervision.count(baseSupervisionFqn) == 0U) {
                LOG_FATAL << "SupervisionFactory::MakeLocalSupervision, deadline supervision "
                          << baseSupervisionFqn.c_str() << " referenced by local supervision " << conf.fqn.c_str()
                          << " not exist";
                std::terminate();
                return ret;
            }
            baseSupervisionReferenced.push_back(baseSupervision[baseSupervisionFqn]);
        }

        for (ara::core::String const& baseSupervisionFqn : conf.logicalSupervision) {
            if (baseSupervision.count(baseSupervisionFqn) == 0U) {
                LOG_FATAL << "SupervisionFactory::MakeLocalSupervision, logical supervision "
                          << baseSupervisionFqn.c_str() << " referenced by local supervision " << conf.fqn.c_str()
                          << " not exist";
                std::terminate();
                return ret;
            }
            baseSupervisionReferenced.push_back(baseSupervision[baseSupervisionFqn]);
        }

        std::shared_ptr< LocalSupervision > const tmp{
            std::make_shared< LocalSupervision >(baseSupervisionReferenced, conf)};
        ret.push_back(tmp);
        for (std::shared_ptr< BaseSupervision > const& ele : baseSupervisionReferenced) {
            if (baseLocalMap.count(ele->GetFqn()) > 0U) {
                baseLocalMap[ele->GetFqn()].push_back(tmp);
            } else {
                baseLocalMap[ele->GetFqn()] = {tmp};
            }
        }
    }
    return ret;
}

/// @brief Create GlobalSupervision objects base on GlobalSupervisionConf.
/// @param globalSupervisionConf conf of global supervision.
/// @param localSupervision The container of local supervision, find local supervision referenced by
/// global supervision in it.
/// @return objects of GlobalSupervision.
ara::core::Vector< std::shared_ptr< GlobalSupervision > > SupervisionFactory::MakeGlobalSupervision(
    ara::core::Vector< GlobalSupervisionConf > const& globalSupervisionConf,
    ara::core::Map< ara::core::String, std::shared_ptr< LocalSupervision > >& localSupervision) noexcept
{
    LOG_INFO << "make global supervision, count " << globalSupervisionConf.size();

    ara::core::Vector< std::shared_ptr< GlobalSupervision > > ret;
    for (GlobalSupervisionConf const& conf : globalSupervisionConf) {
        ara::core::Vector< std::shared_ptr< LocalSupervision > > localSupervisionReferenced;

        for (ara::core::String const& localSupervisionFqn : conf.localSupervision) {
            if (localSupervision.count(localSupervisionFqn) == 0U) {
                LOG_FATAL << "SupervisionFactory::MakeGlobalSupervision, local supervision "
                          << localSupervisionFqn.c_str() << " referenced by global supervision "
                          << conf.shortName.c_str() << " not exist.";
                std::terminate();
                return ret;
            }
            localSupervisionReferenced.push_back(localSupervision[localSupervisionFqn]);
        }
        ret.push_back(std::make_shared< GlobalSupervision >(localSupervisionReferenced, conf));
    }
    return ret;
}

/// @brief Create FgSupervisionMode objects base on FgSupervisionModeConf.
/// @param fgSupervisionModeConf conf of fg supervision mode
/// @return FgSupervisionMode objects
ara::core::Vector< std::shared_ptr< FgSupervisionMode > > SupervisionFactory::MakeSupervisionMode(
    ara::core::Vector< FgSupervisionModeConf > const& fgSupervisionModeConf) noexcept
{
    LOG_INFO << "make supervision mode, count " << fgSupervisionModeConf.size();

    ara::core::Vector< std::shared_ptr< FgSupervisionMode > > ret;
    for (FgSupervisionModeConf const& conf : fgSupervisionModeConf) {
        ara::core::Vector< std::shared_ptr< SupervisionMode > > supervisionMode;
        for (OneFgStateConf const& stateConf : conf.fgStateConf) {
            ara::core::Map< ara::core::String, int32_t > processInfoMap;
            for (ProcessInfoConf const& processInfo : stateConf.processInfo) {
                processInfoMap[processInfo.processName] = processInfo.executionError;
            }
            supervisionMode.push_back(std::make_shared< SupervisionMode >(conf.fqn, stateConf.stateName,
                                                                          stateConf.phmSupervision, processInfoMap));
        }
        ret.push_back(std::make_shared< FgSupervisionMode >(conf.fqn, supervisionMode));
    }
    return ret;
}

}  // namespace internal
}  // namespace phm
}  // namespace ara