#include "InputPacketList.h"
#include <iostream>

bool InputPacketList::add(const InputPacket& p) {
    if(p.timestamp > m_lastInputTimestamp) {
        m_inputs.emplace_back(p);
        m_inputs.back().delta = 0;
        m_lastInputTimestamp = p.timestamp;
        return true;
    }
    return false;
}

void InputPacketList::removeProcessedInputs(float serverTimestamp) {
    while(!m_inputs.empty()) {
        auto& front = m_inputs.front();
        if (front.timestamp > serverTimestamp) {
            break;
        }
        m_inputs.pop_front();
    }
}

std::deque<InputPacket>& InputPacketList::getInputPackets() {
    return m_inputs;
}

float InputPacketList::getLastInputTimestamp() const {
    return m_lastInputTimestamp;
}

void InputPacketList::clear() {
    std::deque<InputPacket>().swap(m_inputs);
}
