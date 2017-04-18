#ifndef __dx_INFLIGHTPACKET_H__
#define __dx_INFLIGHTPACKET_H__

#include "TransmissionData.h"
#include "NetTypes.h"

class DeliveryNotificationManager;
class InFlightPacket {
public:
    InFlightPacket(packet_seq_num_t seqNum);
    packet_seq_num_t getSeqNum() const;
    float getTimeDispatched() const;
    const TransmissionData* getTransmissionData(int key);
    void setTransmissionData(int key, TransmissionData* transmissionData);
    void onDeliveryFail(DeliveryNotificationManager* dnMgr) const;
    void onDeliverySucc(DeliveryNotificationManager* dnMgr) const;
private:
    packet_seq_num_t m_seqNum;
    float m_timeDispatched;
    cocos2d::Map<int, TransmissionData*> m_transmissionDataMap;
};

#endif /* __dx_INFLIGHTPACKET_H__ */
