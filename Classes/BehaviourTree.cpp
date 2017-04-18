#include "BehaviourTree.h"

BehaviorStatus Behavior::getStatus() const {
    return m_status;
}

bool Behavior::isTerminated() const {
    return m_status == BehaviorStatus::SUCCESS
        || m_status == BehaviorStatus::FAILURE;
}

bool Behavior::isRunning() const {
    return m_status == BehaviorStatus::RUNNING;
}

void Behavior::reset() {

}

void Behavior::abort() {

}

void Behavior::update(float dt) {
}

void RepeatBehavior::update(float dt) {
    m_inner->update(dt);
    if (m_inner->getStatus() == BehaviorStatus::RUNNING) {
        return;
    }
    if (m_inner->getStatus() == BehaviorStatus::FAILURE) {
        m_status = BehaviorStatus::FAILURE;
    }
    if (++m_counter == m_limit) {
        m_status = BehaviorStatus::SUCCESS;
        return;
    }
    m_inner->reset();
}

void CompositeBehavior::addChild(Behavior* child) {
    m_children.pushBack(child);
}

void CompositeBehavior::removeChild(Behavior* child) {
    m_children.eraseObject(child);
}

void CompositeBehavior::clearChildren() {
    m_children.clear();
}

void SequenceBehavior::update(float dt) {
    (*m_currChild)->update(dt);
    if (getStatus() != BehaviorStatus::SUCCESS) {
        m_status = getStatus();
        return;
    }
    if (++m_currChild == m_children.end()) {
        m_status = BehaviorStatus::SUCCESS;
    }
}

void SelectorBehavior::update(float dt) {
    (*m_currChild)->update(dt);
    if (getStatus() != BehaviorStatus::FAILURE) {
        m_status = getStatus();
        return;
    }
    if (++m_currChild == m_children.end()) {
        m_status = BehaviorStatus::FAILURE;
    }
}

void ParallelBehavior::update(float dt) {
    size_t successCnt = 0;
    size_t failureCnt = 0;
    auto end_it = m_children.end();
    for (auto it = m_children.begin(); it != end_it; ++it) {
        Behavior* b = *it;
        if (!b->isTerminated()) {
            b->update(dt);
        }
        if (b->getStatus() == BehaviorStatus::SUCCESS) {
            ++successCnt;
            if (m_successPolicy == Policy::RequireOne) {
                m_status = BehaviorStatus::SUCCESS;
                return;
            }
        }
        if (b->getStatus() == BehaviorStatus::FAILURE) {
            ++failureCnt;
            if (m_failurePolicy == Policy::RequireOne) {
                m_status = BehaviorStatus::FAILURE;
                return;
            }
        }
    }
    if (m_failurePolicy == Policy::RequireAll
            && failureCnt == m_children.size()) {
        m_status = BehaviorStatus::FAILURE;
        return;
    }
    if (m_successPolicy == Policy::RequireAll
            && successCnt == m_children.size()) {
        m_status = BehaviorStatus::SUCCESS;
        return;
    }
    m_status = BehaviorStatus::RUNNING;
}

void MonitorBehavior::addCondition(Behavior* condition) {
    m_children.insert(0, condition);
}

void MonitorBehavior::addAction(Behavior* action) {
    m_children.pushBack(action);
}
