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
/// @file       finite_state_machine.h
/// @brief      Finite state machine class definition
/// @details
/// @date       2023-04-01
/// @author     james.feng
/// @version    1.2.0
///
/// ================================================================
///
/// @code{.isoft}
/// @module_path=/ExecutionManagement/Emd
/// @interface_level=none
/// @endcode
///
/// ================================================================

#ifndef _ARA_EXEC_INTERNAL_FINITE_STATE_MACHINE_H_
#define _ARA_EXEC_INTERNAL_FINITE_STATE_MACHINE_H_

#include <ara/core/map.h>
#include <ara/core/string.h>
#include <ara/core/string_view.h>
#include <isoft/core/set.h>

#include <cstdint>
#include <functional>
#include <type_traits>

namespace ara {
namespace exec {
namespace internal {
namespace emd {

/// @brief State machine abstract class, elements of a finite state machine:
///         1. States: A set of state values, the core of state machine operations is to conditionally switch between defined states
///
///         2. Events: The only driving force for state machine activity, only events can trigger state transitions. The state machine will not spontaneously transition states
///                     Events can be indirectly triggered by state machine actions or come from external sources
///                     When using a state machine, users must ensure the deterministic nature of event triggers; otherwise, if no events occur, the state machine will stall
///
///         3. Actions: Transactions that need to be performed, generally executed after an event is triggered or a state is switched
///                     A state machine without actions is meaningless. The purpose of state machine switching is to perform transactions (actions) at certain times (states)
///
///         4. Transition table: The path for state switching; a set of transition tables forms the state routing, specifying the clear sequence of state switching
///
///         5. Transition conditions: Only when the specified conditions are met can the target state be switched to
/// @note This example is a synchronous state machine, event triggers will execute actions and state transitions synchronously, so it is strictly forbidden to call event trigger functions within action callbacks
/// @code{.isoft}
/// @interface_level=none
/// @needwork = no
/// @endcode
class FiniteStateMachine
{
public:
    /// @brief State transition process finite state machine state enumeration
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    enum class State : int16_t
    {
        kInvalidState = -1,  ///< Invalid state
        kTerminating = 1,  ///< Terminating processes, terminating processes in this function group that do not need to be started or need to be restarted for the next function group state
        kTerminated = 2,  ///< Termination completed, all processes not needed in the next function group state have been terminated
        kStarting   = 3,  ///< Starting processes, processes in this function group that need to be started for the next function group state
        kStarted    = 4,  ///< Process startup completed, all processes that need to be started have been started
    };

    /// @brief State transition process finite state machine event enumeration
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    enum class Event : int16_t
    {
        kInvalidEvent      = -1,  ///< Invalid event
        kProcessTerminated = 1,   ///< Process termination event
        kProcessStarted    = 2    ///< Process start event
    };

    /// @brief Action callback function, called when an event is triggered, or when entering or exiting a state
    /// @param s The state of the state machine when the action is triggered
    /// @param e The event that triggered the action
    /// @return 0 Action execution successful, continue to next operation; !0 Action execution failed, state machine stops running and immediately enters the "invalid" state
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using Action = std::function< int32_t(State const &s, Event const &e) >;

    /// @brief Transition condition judgment callback function, checks if the condition is met before state switching; if true, executes the transition
    /// @param srcState The current state when this judgment function is executed
    /// @param destState The next state when this judgment function is executed
    /// @return true Transition condition satisfied; false Not satisfied
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    using TransitionCondition = std::function< bool(State const &srcState, State const &destState) >;

    /// @brief Disable use of default constructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FiniteStateMachine() noexcept = delete;

    /// @brief Finite state machine constructor
    /// @param name User-defined state machine name
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    explicit FiniteStateMachine(ara::core::String name) noexcept
        : name_{std::move(name)}
        , events_{}
        , states_{}
        , currentState_{State::kInvalidState}
        , lastEvent_{Event::kInvalidEvent}
        , errorAction_{nullptr}
        , finalAction_{nullptr}
    {
        /// Users need to inherit and override the constructor, adding events, states, transition tables, etc.
    }

    /// @brief Finite state machine destructor
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    virtual ~FiniteStateMachine() noexcept = default;

    /// @brief Disable use of move constructor
    /// @param other Other state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FiniteStateMachine(FiniteStateMachine &&other) noexcept = delete;

    /// @brief Disable use of copy constructor
    /// @param other Other state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FiniteStateMachine(FiniteStateMachine const &other) noexcept = delete;

    /// @brief Disable move assignment
    /// @param other the other FiniteStateMachine
    /// @return FiniteStateMachine
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FiniteStateMachine &operator=(FiniteStateMachine &&other) noexcept = delete;

    /// @brief Disable copy assignment
    /// @param other the other FiniteStateMachine
    /// @return new FiniteStateMachine
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    FiniteStateMachine &operator=(FiniteStateMachine const &other) noexcept = delete;

#if ARA_EXEC_DEBUG
    /// @brief Get the state machine name
    /// @return State machine name
    ara::core::StringView const GetName() const noexcept { return name_; }
#endif

    /// @brief Set the initial state, the state machine will start transitioning from the initial state
    /// @param s State value
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void SetInitialState(State const &s) noexcept { currentState_ = s; }

    /// @brief Get the current state
    /// @return State value
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    State GetCurrentState() const noexcept { return currentState_; }

    /// @brief Start the state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Start() noexcept
    {
        // First entry into state, execute state action
        ara::core::Map< State const, StateInfo >::iterator const it{states_.find(currentState_)};
        // State not found, return error
        if (states_.end() == it) {
#if ARA_EXEC_DEBUG
            std::cout << "ERROR on FSM::Start(), can`t find state: " << static_cast< int32_t >(currentState_)
                      << std::endl;
#endif
            if (errorAction_) {
                std::ignore = errorAction_(currentState_, lastEvent_);
            }
            // Set the current state to an invalid value to mark the error
            currentState_ = State::kInvalidState;
            return;
        }
        Action &action{it->second.actions.enterAction};
        if (action) {
            // Execute the enterAction of the target state
            if (0 != action(currentState_, lastEvent_)) {
                // Action execution failed, error handling
#if ARA_EXEC_DEBUG
                std::cout << "ERROR on execute FSM::enterAction(), current state: "
                          << static_cast< int32_t >(currentState_) << std::endl;
#endif
                if (errorAction_) {
                    std::ignore = errorAction_(currentState_, lastEvent_);
                }
                // Set the current state to an invalid value to mark the error
                currentState_ = State::kInvalidState;
                return;
            }
        }
        // Attempt state transition
        _Transfer();
    }

    /// @brief Stop the state machine
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void Stop() noexcept
    {
        currentState_ = State::kInvalidState;
#if ARA_EXEC_DEBUG
        std::cout << "FSM::Stoped" << std::endl;
#endif
    }

    /// @brief Trigger an event
    /// @param e Event
    /// @exception std::runtime_error If processing fails
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    template < typename T >
    void TriggerEvent(T const &e)
    {
        Event const event{static_cast< Event >(e)};
        ara::core::Map< Event const, Action >::iterator const it{events_.find(event)};
        if (events_.end() == it) {
            return;
        }

        // Record the last triggered event
        lastEvent_ = event;
        Action &action{it->second};
        if (action) {
            if (0 != action(currentState_, event)) {
#if ARA_EXEC_DEBUG
                std::cout << "ERROR on FSM::TriggerEvent action, event: " << static_cast< int32_t >(lastEvent_)
                          << std::endl;
#endif
                if (errorAction_) {
                    std::ignore = errorAction_(currentState_, lastEvent_);
                }
                return;
            }
        }
        _Transfer();
    }

    /// @brief Register the state machine transition completion callback function
    /// @param action The action to be executed when the transition ends
    /// @note  This action is triggered when the state transition path reaches the end (no next state)
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void OnFinal(Action const &action) noexcept { finalAction_ = action; }

    /// @brief Register the state machine error callback function
    /// @param action The action to be executed when a transition error occurs
    /// @note  This action is triggered when a state transition fails, usually because the user's action return value is non-zero
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void OnError(Action const &action) noexcept { errorAction_ = action; }

protected:
    /// @brief Add an event and specify an action for it. Whenever the event is triggered, the state machine automatically executes the action
    /// @note  Users need to ensure that event names are unique. An event can only be bound to one action. If the same event is added multiple times, the action will be overwritten
    ///        The execution time of the event action is before Transfer()
    /// @param event Event
    /// @param eventAction Event action
    /// @exception std::runtime_error
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    template < typename T >
    void _AddEvent(T const &event, Action const &eventAction)
    {
        std::ignore = events_.emplace(std::make_pair(static_cast< Event >(event), eventAction));
    }

    /// @brief Delete an event
    /// @param event Event
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    template < typename T >
    void _DelEvent(T const &event) noexcept
    {
        auto it = events_.find(static_cast< Event >(event));
        if (events_.end() != it) {
            events_.erase(it);
        }
    }

    /// @brief Add a state and specify two actions for it, which will be automatically executed after entering the state and before exiting the state respectively
    /// @param state State name
    /// @param enterAction Action executed after entering the state
    /// @param exitAction Action executed before exiting the state
    /// @exception std::runtime_error
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    template < typename T >
    void _AddState(T const &state, Action const &enterAction, Action const &exitAction)
    {
        ActionPair ac;
        StateInfo si;
        ac.enterAction = enterAction;
        ac.exitAction  = exitAction;
        si.state       = static_cast< State >(state);
        si.actions     = ac;
        std::ignore    = states_.emplace(std::make_pair(static_cast< State >(state), si));
    }

    /// @brief Delete a state
    /// @param state State name
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    template < typename T >
    void _DelState(T const &state) noexcept
    {
        auto it = states_.find(static_cast< State >(state));
        if (states_.end() != it) {
            states_.erase(it);
        }
    }

    /// @brief Add a state transition rule
    /// @param srcState Source state
    /// @param destState Target state
    /// @param condition Transition condition judgment callback function, nullptr means default true, i.e., unconditionally enter the next state
    /// @note Only one rule can exist for the same path, and only one judgment function. If added multiple times, it will be overwritten later
    ///       The target and source states can be the same, but users should handle carefully to avoid infinite loops, unless intentionally done
    /// @exception std::runtime_error
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    template < typename T >
    void _AddTransitionRule(T const &srcState, T const &destState, TransitionCondition const &condition)
    {
        ara::core::Map< State const, StateInfo >::iterator const it{states_.find(static_cast< State >(srcState))};
        if (states_.end() == it) {
            return;
        }
        StateTransitionRule stateTransRule;
        stateTransRule.nextState = static_cast< State >(destState);
        stateTransRule.condition = condition;
        StateInfo &stateInfo{it->second};
        isoft::core::Set< StateTransitionRule > &transRules{stateInfo.transitionRules};
        std::ignore = transRules.emplace(stateTransRule);
    }

    /// @brief Delete a state transition rule
    /// @param srcState Source state
    /// @param destState Target state
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    template < typename T >
    void _DelTransitionRule(T const &srcState, T const &destState) noexcept
    {
        auto it = states_.find(static_cast< State >(srcState));
        if (states_.end() == it) {
            return;
        }
        auto &stateInfo    = it->second;
        auto &transRules   = stateInfo.transitionRules;
        auto transRuleIter = transRules.begin();
        while (transRuleIter != transRules.end()) {
            if (transRuleIter->nextState == static_cast< State >(destState)) {
                break;
            }
            transRuleIter++;
        }
        transRules.erase(transRuleIter);
    }

    /// @brief State transition function, executed once every time an event occurs. This function executes after Action
    ///        This function switches all states that can be switched according to the state transition table, until encountering an unsatisfied condition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    void _Transfer() noexcept
    {
#if ARA_EXEC_DEBUG
        std::cout << "FSM::_Transfer(), State {" << static_cast< int32_t >(currentState_) << "}, Event {"
                  << static_cast< int32_t >(lastEvent_) << "}" << std::endl;
#endif

        // If the current state is invalid, exit directly
        if (State::kInvalidState == currentState_) {
#if ARA_EXEC_DEBUG
            std::cout << "FSM::_Transfer(): current State is Invalid." << std::endl;
#endif
            return;
        }

        // Mark the loop condition: if a state transition occurs, continue looping; otherwise, do not loop
        bool loopCondition = false;
        do {
            // Initialize the loop condition, default is not to loop, unless a state transition occurs
            loopCondition = false;

            /// Traverse the transition table of the current state, find the transition sequence that satisfies the conditions
            ara::core::Map< State const, StateInfo >::iterator const it{states_.find(currentState_)};
            // State not found, return error
            if (states_.end() == it) {
#if ARA_EXEC_DEBUG
                std::cout << "ERROR on FSM::_Transfer(), can't find state: " << static_cast< int32_t >(currentState_)
                          << std::endl;
#endif
                if (nullptr != errorAction_) {
                    std::ignore = errorAction_(currentState_, lastEvent_);
                }
                // Set the current state to an invalid value to mark the error
                currentState_ = State::kInvalidState;
                return;
            }

            StateInfo &curStateInfo{it->second};
            isoft::core::Set< StateTransitionRule > &transRules{curStateInfo.transitionRules};

            // If the current state has no next transition path, consider that the state machine has reached the end, execute the callback and end
            if (transRules.empty()) {
#if ARA_EXEC_DEBUG
                std::cout << "end of FSM::_Transfer(), current state: " << static_cast< int32_t >(currentState_)
                          << std::endl;
#endif
                if (nullptr != finalAction_) {
                    std::ignore = finalAction_(currentState_, lastEvent_);
                }
                return;
            }

            for (auto const &nextStateInfo : transRules) {
                // If conditions are not satisfied, check the next path
                if (nullptr != nextStateInfo.condition) {
                    // condition != nullptr && true != condition()
                    if (true != nextStateInfo.condition(currentState_, nextStateInfo.nextState)) {
                        continue;
                    }
                } else {
                    // If the condition function is nullptr, consider it always true, i.e., unconditionally enter the next state
                    ;  // Do Nothing
                }

                // condition == nullptr || true == condition()

                // If conditions are satisfied, execute the state switching process
                // Execute the exitAction of the current state
                if (nullptr != curStateInfo.actions.exitAction) {
                    if (0 != curStateInfo.actions.exitAction(currentState_, lastEvent_)) {
                        // Action execution failed, error handling
#if ARA_EXEC_DEBUG
                        std::cout << "ERROR on execute FSM::Transfer::exitAction(), current state: "
                                  << static_cast< int32_t >(currentState_) << std::endl;
#endif
                        if (nullptr != errorAction_) {
                            std::ignore = errorAction_(currentState_, lastEvent_);
                        }
                        // Set the current state to an invalid value to mark the error
                        currentState_ = State::kInvalidState;
                        return;
                    }
                }

#if ARA_EXEC_DEBUG
                std::cout << "FSM::_Transfer(), State { " << static_cast< int32_t >(currentState_)
                          << " } change to State { " << static_cast< int32_t >(nextStateInfo.nextState) << " }"
                          << std::endl;
#endif
                // Execute state switching
                currentState_ = nextStateInfo.nextState;
                // State switching completed, continue looping
                loopCondition = true;

                // Execute the enterAction of the target state
                ara::core::Map< State const, StateInfo >::iterator const tmpIt{states_.find(currentState_)};
                if (nullptr != tmpIt->second.actions.enterAction) {
                    if (0 != tmpIt->second.actions.enterAction(currentState_, lastEvent_)) {
                        // Action execution failed, error handling
#if ARA_EXEC_DEBUG
                        std::cout << "ERROR on execute FSM::Transfer::enterAction(), current state: "
                                  << static_cast< int32_t >(currentState_) << std::endl;
#endif
                        if (nullptr != errorAction_) {
                            std::ignore = errorAction_(currentState_, lastEvent_);
                        }
                        // Set the current state to an invalid value to mark the error
                        currentState_ = State::kInvalidState;
                        return;
                    }
                }

                // Exit after hitting the path, cannot continue traversing the state transition table because the state has already changed
                break;
            }  ///< for
        } while (loopCondition);
#if ARA_EXEC_DEBUG
        std::cout << "end of FSM::_Transfer(), current state: " << static_cast< int32_t >(currentState_) << std::endl;
#endif
    }

private:
    /// @brief State machine name (identifier)
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ara::core::String name_;

    /// @brief Defined events, event names are saved as strings and bound to an action
    /// @note  Whenever an event is triggered, the corresponding action is executed
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ara::core::Map< Event const, Action > events_;

    /// @brief State action structure
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    struct ActionPair
    {
        /// @brief Action executed after entering the state
        Action enterAction;

        /// @brief Action executed before exiting the state
        Action exitAction;
    };

    /// @brief State transition table
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    class StateTransitionRule
    {
    public:
        /// @brief Next state
        State nextState;

        /// @brief Transition condition
        TransitionCondition condition;
    };

    /// @brief State object
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    struct StateInfo
    {
        /// @brief State name
        State state;
        /// @brief Action
        ActionPair actions;
        /// @brief State transition rule table
        isoft::core::Set< StateTransitionRule > transitionRules;
    };

    /// @brief State name to object mapping table, for quick lookup
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    ara::core::Map< State const, StateInfo > states_;

    /// @brief Current state
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    State currentState_;

    /// @brief Record the last triggered event
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Event lastEvent_;

    /// @brief Action to execute when a transition error occurs
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Action errorAction_;

    /// @brief Action to execute when reaching the end of a transition
    /// @code{.isoft}
    /// @interface_level=none
    /// @needwork = no
    /// @endcode
    Action finalAction_;

    /// @brief Less than operator overload
    /// @param  l the left StateTransitionRule
    /// @param  r the right StateTransitionRule
    /// @return true less than; false not less than
    friend bool operator<(FiniteStateMachine::StateTransitionRule const &l,
                          FiniteStateMachine::StateTransitionRule const &r) noexcept;
};

/// @brief Less than operator
/// @param  l the left StateTransitionRule
/// @param  r the right StateTransitionRule
/// @return true less than; false not less than
inline bool operator<(FiniteStateMachine::StateTransitionRule const &l,
                      FiniteStateMachine::StateTransitionRule const &r) noexcept
{
    return l.nextState < r.nextState;
}
}  // namespace emd
}  // namespace internal
}  // namespace exec
}  // namespace ara

#endif  ///< _ARA_EXEC_INTERNAL_FINITE_STATE_MACHINE_H_
