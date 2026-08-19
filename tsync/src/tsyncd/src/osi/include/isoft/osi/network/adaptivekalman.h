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
/// @file       adaptivekalman.h
/// @brief      Kalman filter algorithm  stable after 2576ns  https://github.com/eigen-mirror/eigen.git
/// @details
/// @date       2023-01-09
/// @author     david.he
/// @version    1.2.0
///
/// ================================================================

#ifndef ISOFT_OSI_NETWORK_ADAPTIVECLOCKKALMAN_H
#define ISOFT_OSI_NETWORK_ADAPTIVECLOCKKALMAN_H
#include <cmath>
#include <deque>
#include <eigen3/Eigen/Dense>
#include <iostream>
#include <numeric>  // for std::accumulate
namespace isoft {
namespace osi {
namespace network {
static constexpr double kKALMAN_INIT_OFFSET{10000000000.0};
static constexpr std::size_t kKALMAN_INIT_WARMUP{50};
static constexpr double kKALMAN_INIT_COVARIANCE{1000.0};
static constexpr double kKALMAN_INIT_NOISE{0.05};  // process noise
static constexpr double kKALMAN_NOISE_SCALE{8.0};  // process noise
class AdaptiveClockKalman
{
public:
    explicit AdaptiveClockKalman(double initOffset = kKALMAN_INIT_OFFSET, std::size_t warmup = kKALMAN_INIT_WARMUP)
        : warmupSteps_(warmup), maxSize_(warmup)
    {
        x_.resize(1);
        x_ << initOffset;
        p_.resize(1, 1);
        p_ << kKALMAN_INIT_COVARIANCE;  // initial covariance given a large value, indicating uncertainty
        f_.resize(1, 1);
        f_ << 1.0;
        q_.resize(1, 1);
        q_ << kKALMAN_INIT_NOISE;
        r_.resize(1, 1);
        r_ << 1.0;  // measurement noise
        h_.resize(1, 1);
        h_ << 1.0;
    }

    bool Update(double z)
    {
        // 1. Prediction
        x_ = f_ * x_;
        p_ = f_ * p_ * f_.transpose() + q_;

        // 2. Calculate residual (Innovation)
        double innovation = std::abs(z - x_(0));

        // 3. Dynamic threshold logic
        bool isOutlier = false;
        if (currentStep_ > warmupSteps_) {
            // Stable period: perform strict check
            if (innovation > baseThreshold_) {
                isOutlier = true;
            }
        } else {
            // Warm-up period: increment count each step, and relax threshold by 10 times or do not check at all
            // if (innovation > baseThreshold_ * 10.0) {
            //     isOutlier = true;
            // }
            currentStep_++;
        }

        // 4. Update or skip
        if (!isOutlier) {
            _addThresholdSample(innovation);
            baseThreshold_        = _getAverage() * kKALMAN_NOISE_SCALE;
            Eigen::MatrixXd sTemp = h_ * p_ * h_.transpose() + r_;
            Eigen::MatrixXd kTemp = p_ * h_.transpose() * sTemp.inverse();
            x_                    = x_ + kTemp * (z - x_(0));
            p_                    = (Eigen::MatrixXd::Identity(1, 1) - kTemp * h_) * p_;
        } else {
            // If it is an outlier, keep P as the predicted value (uncertainty slightly increased), but do not update state x_
            //std::cout << "AdaptiveClockKalman [Jump out] Step " << z << std::endl;
        }

        return !isOutlier;
    }

private:
    void _addThresholdSample(double val)
    {
        if (data_.size() >= maxSize_) {
            data_.pop_front();
        }
        data_.push_back(val);
    }
    // Average noise
    double _getAverage() const
    {
        if (data_.empty()) {
            return 0.0;
        }
        double sum = std::accumulate(data_.begin(), data_.end(), 0.0);
        return sum / static_cast< double >(data_.size());
    }

private:
    //x (state vector - State Vector)
    //P (state covariance matrix - Error Covariance Matrix)
    // F (state transition matrix - State Transition Matrix)
    // H (observation matrix - Observation Matrix)
    // Q (process noise covariance - Process Noise Covariance)
    // R (measurement noise covariance - Measurement Noise Covariance)
    Eigen::VectorXd x_;
    Eigen::MatrixXd p_, f_, q_, r_, h_;

    std::size_t warmupSteps_{0U};  // warm-up period count
    std::size_t currentStep_{0U};
    double baseThreshold_{0.0};  // threshold after stabilization
    // Calculate threshold, take the value of the most recent data
    std::deque< double > data_;
    std::size_t maxSize_{0U};
};
}  // namespace network
}  // namespace osi
}  // namespace isoft
#endif  // ISOFT_OSI_NETWORK_ADAPTIVECLOCKKALMAN_H
