#ifndef __dx_BEHAVIOURTREE_H__
#define __dx_BEHAVIOURTREE_H__

#include "cocos2d.h"

enum class BehaviorStatus {
    SUCCESS,
    FAILURE,
    RUNNING,
    ABORTED,
    INVALID
};

class Behavior : public cocos2d::Ref {
public:
    virtual ~Behavior() {}
    virtual void update(float dt);
    bool isTerminated() const;
    bool isRunning() const;
    void reset();
    void abort();
    BehaviorStatus getStatus() const;
protected:
    BehaviorStatus m_status = BehaviorStatus::INVALID;
};

class RepeatBehavior : public Behavior {
public:
    void update(float dt);
protected:
    int m_counter = 0;
    int m_limit = 0;
    Behavior* m_inner = nullptr;
};

class CompositeBehavior : public Behavior {
public:
    void addChild(Behavior* child);
    void removeChild(Behavior* child);
    void clearChildren();
protected:
    typedef cocos2d::Vector<Behavior*> Behaviors;
    Behaviors m_children;
};

class SequenceBehavior : public CompositeBehavior {
public:
    void update(float dt);
protected:
    Behaviors::iterator m_currChild;
};

class SelectorBehavior : public CompositeBehavior {
public:
    void update(float dt);
protected:
    Behaviors::iterator m_currChild;
};

class ParallelBehavior : public CompositeBehavior {
public:
    enum class Policy {
        RequireOne,
        RequireAll
    };
    ParallelBehavior(Policy forSuccess, Policy forFailure) :
        m_successPolicy(forSuccess),
        m_failurePolicy(forFailure)
    {}
    void update(float dt);
protected:
    Policy m_successPolicy;
    Policy m_failurePolicy;
};

class MonitorBehavior : public ParallelBehavior {
public:
    MonitorBehavior() : ParallelBehavior(
        Policy::RequireOne, Policy::RequireOne)
    {}
    void addCondition(Behavior* condition);
    void addAction(Behavior* action);
};

class BehaviorRunner : public cocos2d::FiniteTimeAction {
public:
};

#endif // __dx_BEHAVIOURTREE_H__
