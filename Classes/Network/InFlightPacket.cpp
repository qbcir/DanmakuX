#include "InFlightPacket.h"
#include <chrono>

InFlightPacket::InFlightPacket(packet_seq_num_t seqNum) :
    m_seqNum(seqNum)
{
    m_timeDispatched = 0; // FIXME
}

packet_seq_num_t InFlightPacket::getSeqNum() const {
    return m_seqNum;
}

float InFlightPacket::getTimeDispatched() const {
    return m_timeDispatched;
}

const TransmissionData* InFlightPacket::getTransmissionData(int key) {
    auto it = m_transmissionDataMap.find(key);
    return it != std::end(m_transmissionDataMap) ? it->second : nullptr;
}

void InFlightPacket::setTransmissionData(
        int key,
        TransmissionData* transmissionData) {
    m_transmissionDataMap.insert(key, transmissionData);
}

void InFlightPacket::onDeliveryFail(
        DeliveryNotificationManager* dnMgr) const {
    for (const auto& p : m_transmissionDataMap) {
        p.second->onDeliveryFail(dnMgr);
    }
}
void InFlightPacket::onDeliverySucc(
        DeliveryNotificationManager* dnMgr) const {
    for (const auto& p : m_transmissionDataMap) {
        p.second->onDeliverySucc(dnMgr);
    }
}
