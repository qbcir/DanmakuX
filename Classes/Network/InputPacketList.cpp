#include "InputPacketList.h"

bool InputPacketList::add(const InputPacket& p) {
    if(p.timestamp > m_lastInputTimestamp) {
        float dt = m_lastInputTimestamp >= 0.f ? p.timestamp - m_lastInputTimestamp : 0.f;
        m_lastInputTimestamp = p.timestamp;
        m_inputs.emplace_back(InputPacket(
            p.keyboardState, p.mouseState, p.timestamp, dt));
        return true;
    }
    return false;
}

void InputPacketList::removeProcessedInputs(float serverTimestamp) {
    while(!m_inputs.empty()
          && m_inputs.front().timestamp <= serverTimestamp) {
        m_inputs.pop_front();
    }
}
