#include "ReplicationCommand.h"

ReplicaCommand::ReplicaCommand()
{}

void ReplicaCommand::addDirtyState(uint64_t dirtyState) {
    m_dirtyState |= dirtyState;
}

bool ReplicaCommand::hasDirtyState() const {
    return m_action == ReplicaActionType::DESTROY || m_dirtyState != 0;
}

ReplicaActionType ReplicaCommand::getActionType() const {
    return m_action;
}

uint64_t ReplicaCommand::getDirtyState() const {
    return m_dirtyState;
}
