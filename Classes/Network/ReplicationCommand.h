#ifndef REPLICATIONCOMMAND_H
#define REPLICATIONCOMMAND_H

#include <cstdint>

enum class ReplicaActionType {
    CREATE,
    UPDATE,
    DESTROY,
    RPC,
    INVALID
};

class ReplicaCommand {
public:
    ReplicaCommand();
    ReplicaCommand(uint64_t dirtyState);
    void addDirtyState(uint64_t dirtyState);
    bool hasDirtyState() const;
    ReplicaActionType getActionType() const;
    uint64_t getDirtyState() const;
private:
    ReplicaActionType m_action;
    uint64_t m_dirtyState;
};

#endif // REPLICATIONCOMMAND_H
