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
/// @file       campaign_mediator.h
/// @brief
/// @details
/// @date       2023-11-15
/// @author     zhaoyunfei
/// @version    1.2.0
///
/// ================================================================

namespace ara {
namespace ucm {
namespace vpkgmgr {

class CampaignMediator
{
public:
    CampaignMediator(void)                     = default;
    virtual ~CampaignMediator(void)            = default;
    CampaignMediator(CampaignMediator const &) = delete;
    CampaignMediator(CampaignMediator &&)      = delete;
    CampaignMediator &operator=(CampaignMediator const &) = delete;
    CampaignMediator &operator=(CampaignMediator &&) = delete;

private:
    virtual void OnCancel()  = 0;
    virtual void DoProcess() = 0;
};

}  // namespace vpkgmgr
}  // namespace ucm
}  // namespace ara