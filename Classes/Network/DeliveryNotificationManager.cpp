#include "DeliveryNotificationManager.h"
#include "../Data/ByteParser.h"

DeliveryNotificationManager::DeliveryNotificationManager(
        bool shouldSendAcks,
        bool shouldProcessAcks) :
    m_shouldSendAcks(shouldSendAcks),
    m_shouldProcessAcks(shouldProcessAcks)
{}

InFlightPacket* DeliveryNotificationManager::writeSeqNum(BytesT& buf) {
    //write the sequence number, but also create an inflight packet for this...
    packet_seq_num_t seqNum = m_nextOutgoingSeqNum++;
    dx_write_bytes<packet_seq_num_t>(buf, seqNum);
    ++m_dispatchedPacketCnt;
    if (m_shouldProcessAcks) {
        m_inFlightPackets.emplace_back(InFlightPacket(seqNum));
        return &m_inFlightPackets.back();
    } else {
        return nullptr;
    }
}


void DeliveryNotificationManager::writeAckData(BytesT& buf) {
    //we usually will only have one packet to ack
    //so we'll follow that with a 0 bit if that's the case
    //however, if we have more than 1, we'll make that 1 bit a 1 and then write 8 bits of how many packets
    //we could do some statistical analysis to determine if this is the best strategy but we'll use it for now

    //do we have any pending acks?
    //if so, write a 1 bit and write the first range
    //otherwise, write 0 bit
    bool hasAcks = m_pendingAcks.size() > 0;
    dx_write_bytes<bool>(buf, hasAcks);
    if(hasAcks) {
        //note, we could write all the acks
        dx_write_bytes<AckRange>(buf, m_pendingAcks.front());
        m_pendingAcks.pop_front();
    }
}

bool DeliveryNotificationManager::processSeqNumber(uint8_t* &p) {
    auto seqNum = dx_read_bytes<packet_seq_num_t>(p);
    if (seqNum >= m_nextExpectedSeqNum) {
        m_nextExpectedSeqNum = seqNum + 1;
        if (m_shouldSendAcks) {
            addPendingAck(seqNum);
        }
        return true;
    }
    return false;
}

void DeliveryNotificationManager::processAcks(uint8_t* &p) {
    auto hasAcks = dx_read_bytes<bool>(p);
    if (hasAcks) {
        auto ackRange = dx_read_bytes<AckRange>(p);
        //for each InfilghtPacket with a sequence number less than the start, handle delivery failure...
        packet_seq_num_t nextAckdSeqNum = ackRange.start;
        uint32_t onePastAckdSeqNum = nextAckdSeqNum + ackRange.count;
        while(nextAckdSeqNum < onePastAckdSeqNum
              && !m_inFlightPackets.empty()) {
            const auto& nextInFlightPacket = m_inFlightPackets.front();
            //if the packet has a lower sequence number, we didn't get an ack for it, so it probably wasn't delivered
            packet_seq_num_t nextInFlightPacketSeqNum = nextInFlightPacket.getSeqNum();
            if (nextInFlightPacketSeqNum < nextAckdSeqNum) {
                //copy this so we can remove it before handling the failure- we don't want to find it when checking for state
                auto copyOfInFlightPacket = nextInFlightPacket;
                m_inFlightPackets.pop_front();
                onDeliveryFail(copyOfInFlightPacket);
            } else if (nextInFlightPacketSeqNum == nextAckdSeqNum ) {
                onDeliverySucc(nextInFlightPacket);
                //received!
                m_inFlightPackets.pop_front();
                //decrement count, advance nextAckdSequenceNumber
                ++nextAckdSeqNum;
            } else {
                ++nextAckdSeqNum;
            }
        }
    }
}

void DeliveryNotificationManager::addPendingAck(packet_seq_num_t seqNum)
{
    //if you don't have a range yet, or you can't correctly extend the final range with the sequence number,
    //start a new range
    if(m_pendingAcks.size() == 0
            || !m_pendingAcks.back().extend(seqNum))
    {
        m_pendingAcks.emplace_back(AckRange(seqNum));
    }
}

void DeliveryNotificationManager::onDeliveryFail(const InFlightPacket& inFlightPacket) {
    ++m_droppedPacketCnt;
    inFlightPacket.onDeliveryFail(this);
}

void DeliveryNotificationManager::onDeliverySucc(const InFlightPacket& inFlightPacket) {
    ++m_deliveredPacketCnt;
    inFlightPacket.onDeliverySucc(this);
}

InFlightPacket*	DeliveryNotificationManager::writeState(BytesT& buf) {
    auto res = writeSeqNum(buf);
    if (m_shouldSendAcks) {
        writeAckData(buf);
    }
    return res;
}

bool DeliveryNotificationManager::readAndProcessState(uint8_t* &p) {
    bool res = processSeqNumber(p);
    if(m_shouldSendAcks) {
        processAcks(p);
    }
    return res;
}

void DeliveryNotificationManager::processTimedOutPackets() {
    float timeoutTime = 0;//Timing::sInstance.GetTimef() - kDelayBeforeAckTimeout;
    while(!m_inFlightPackets.empty()) {
        const auto& nextInFlightPacket = m_inFlightPackets.front();
        //was this packet dispatched before the current time minus the timeout duration?
        if(nextInFlightPacket.getTimeDispatched() < timeoutTime) {
            //it failed! let us know about that
            onDeliveryFail(nextInFlightPacket);
            m_inFlightPackets.pop_front();
        } else {
            //it wasn't, and packets are all in order by time here, so we know we don't have to check farther
            break;
        }
    }
}

