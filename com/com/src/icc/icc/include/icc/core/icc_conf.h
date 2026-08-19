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
/// @file       icc_conf.h
/// @brief
/// @details
/// @date       2025-11-27
/// @author     Zan Shigang
/// @version    1.2.0
///
/// ================================================================

#ifndef _CONF_H_ICC
#define _CONF_H_ICC

#pragma once

#include "icc_types.h"
#include "nai/os/nai_socket.h"
#include "nai/runtime/nai_array.h"
#include "nai/runtime/nai_pool.h"
#include "nai/runtime/nai_rbtree.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    /**
 * json example:
 *  {
 *      application: [{
 *          name: 'app1',
 *          client: 1
 *      }], 
 *      service: [{
 *          service: 1, 
 *          instace: 1, 
 *      }] 
 *  };
 *
 * call example:
 *
 *  icc_conf_init(c);
 *
 *  # add service
 *  icc_conf_setopts(c, "begin", "service");
 *
 *      # set service property
 *      icc_conf_setopti(c, "service", 1);
 *      icc_conf_setopti(c, "instance", 1);
 *      icc_conf_setopts(c, "reliablity", "both");
 *      ...
 *  icc_conf_setopts(c, "end", "service");
 *
 *  # finish config
 *  icc_conf_finish(c);
 */

#ifndef _ICC_TYPEDEF_CONF_T
    #define _ICC_TYPEDEF_CONF_T
    typedef struct icc_conf_s icc_conf_t;
#endif

#ifndef _ICC_TYPEDEF_SERVICE_INFO_T
    #define _ICC_TYPEDEF_SERVICE_INFO_T
    typedef struct icc_service_info_s icc_service_info_t;
#endif

#ifndef _ICC_TYPEDEF_EVENT_INFO_T
    #define _ICC_TYPEDEF_EVENT_INFO_T
    typedef struct icc_event_info_s icc_event_info_t;
#endif

    /**
 * the structure of the configuration
 */
    struct icc_conf_s
    {
        nai_pool_t pool;    /**< the pool */
        nai_rbtree_t apps;  /**< the applications map */
        nai_rbtree_t servs; /**< the services map */
        void* ctx;          /**< pointer to internal context */
    };

    /**
 * the structure of the service information
 */
    struct icc_service_info_s
    {
        icc_serv_t serv;          /**< the service id */
        icc_inst_t inst;          /**< the instance id */
        uint16_t icc_type;        /**< endpoint type */
        nai_str_t icc_identifier; /**< endpoint info */
        icc_version_t major;
        icc_versmin_t minor;
    };

    /**
 * the structure of the event information
 */
    struct icc_event_info_s
    {
        int type;          /**< the event type */
        int reliability;   /**< the reliable type, see @ref NSI_RELIABLE */
        int ngroup;        /**< the number of group ids */
        icc_gid_t* groups; /**< pointer to the array of group ids */
    };

    /**
 * initial the configuration
 * @param   p       pointer to the configuration
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 * @note    no error should occur during initialization, and return value is 
 *          allowed not to be processed.
 */
    ICC_EXTERN
    int icc_conf_init(icc_conf_t* p);

    /**
 * set option with an integer
 * @param   p       pointer to the configuration
 * @param   name    pointer to the option name
 * @param   v       the value of the option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_conf_setopti(icc_conf_t* p, const char* name, intptr_t v);

    /**
 * set option with a float
 * @param   p       pointer to the configuration
 * @param   name    pointer to the option name
 * @param   v       the value of the option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_conf_setoptf(icc_conf_t* p, const char* name, float v);

    /**
 * set option with a double
 * @param   p       pointer to the configuration
 * @param   name    pointer to the option name
 * @param   v       the value of the option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_conf_setoptd(icc_conf_t* p, const char* name, double v);

    /**
 * set option with a string
 * @param   p       pointer to the configuration
 * @param   name    pointer to the option name
 * @param   v       the value of the option
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_conf_setopts(icc_conf_t* p, const char* name, const char* v);

    /**
 * finish config
 * @param   p       pointer to the configuration
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_conf_finish(icc_conf_t* p);

    /**
 * close the configuration
 * @param   p       pointer to the configuration
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_conf_close(icc_conf_t* p);

    /**
 * lookup the client id by a name
 * @param   p       pointer to the configuration
 * @param   name    pointer to the string view of the name
 * @param   cid     pointer to the client id
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_conf_get_client_id(icc_conf_t* p, const nai_str_t* name, icc_cid_t* cid);

    /**
 * get the service information
 * @param   p       pointer to the configuration
 * @param   serv    the service id
 * @param   inst    the instance id
 * @param   info    pointer to the service information
 * @retval  >=0     on success
 * @retval  -1      an error occurred, see #nai_errno
 */
    ICC_EXTERN
    int icc_conf_get_service(icc_conf_t* p, icc_serv_t serv, icc_inst_t inst, icc_service_info_t* info);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif
