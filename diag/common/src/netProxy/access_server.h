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
/// @file       access_server.h
/// @brief
/// @details
/// @date       2024-11-28
/// @author     xuhua
/// @version    1.2.0
///
/// ================================================================

#ifndef __ACCESS_SERVER_H__
#define __ACCESS_SERVER_H__

#include <condition_variable>
#include <deque>
#include <functional>
#include <future>
#include <mutex>
#include <string>
#include <unordered_map>

#include "netProxy/rpc_define.h"
#include "net_access.h"
#include "nsomeip/core/nsi_app.h"
#include "nsomeip/core/nsi_conf.h"
#include "nsomeip/core/nsi_routing.h"
#include "serialization/serialization/common_data_type.h"

namespace isoft {
namespace dm {
namespace dis {

/* servId           ChannelService
 * instId           ChannelInstance
 * method           ChannelId
 */
constexpr nsi_serv_t kChannelServiceId{1U};
constexpr nsi_eid_t kEventNotifyId{1U};

class AccessServer final : public NetAccess
{
public:
    /// @brief Register service callback
    /// @return void
    /// @throw unknown
    static AccessServer &GetComProxy();

    // NSI env init
    bool Init(AccessConfig const &cfg);
    void Offer();
    void StopOffer();
    bool Start(bool bStart);
    void Subscribe();

    AccessServer()  = default;  // NOLINT
    ~AccessServer() = default;  // NOLINT

    AccessServer(const AccessServer &other)  = delete;
    AccessServer(const AccessServer &&other) = delete;
    AccessServer &operator=(const AccessServer &other) = delete;
    AccessServer &operator=(AccessServer const &&other) = delete;

private:
    void _SetNsiCallBack();
    bool _Init();

private:
    static int On_Available(nsi_app_t *a,
                            void *ud,
                            int avail,
                            nsi_serv_t serv,
                            nsi_inst_t inst,
                            nsi_version_t major,
                            nsi_versmin_t minor,
                            const nsi_cred_t *cred);
    static int On_Subscribe(nsi_app_t *a,
                            void *ud,
                            nsi_cid_t cid,
                            nsi_serv_t serv,
                            nsi_inst_t inst,
                            nsi_gid_t gid,
                            nsi_eid_t eid,
                            const nsi_cred_t *cred);
    static int On_Subscribe_Ack(
        nsi_app_t *a, void *ud, int rcode, nsi_serv_t serv, nsi_inst_t inst, nsi_gid_t gid, nsi_eid_t eid);

    // Load configuration file, service ID loading
    bool _LoadConfig(std::string const &json);

    void _OfferEvent(uint16_t eventId);

private:
    nai_evloop_t *evloop_{nullptr};
    nsi_conf_t conf_{};
    nsi_routing_t routing_{};
    // nsi_app_t app{};
    nsi_appcb_t appcb_{};
    std::thread *ioThread_{nullptr};
};

}  // namespace dis
}  // namespace dm
}  // namespace isoft

#endif  // __ACCESS_SERVER_H__