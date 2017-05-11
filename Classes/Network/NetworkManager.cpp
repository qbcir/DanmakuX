#include "NetworkManager.h"
#include "../GameManager.h"
#include "UDPSocket.h"
#include "../InputManager.h"
#include "InputPacket.h"
#include "../Layers/LevelLayer.h"
#include "../Objects/PlayerObject.h"
#include "../Input/InputPacketComponent.h"

NetworkManager* NetworkManager::getInstance() {
    return GameManager::getInstance()->getNetworkManager();
}

NetworkManager::NetworkManager()
    : m_avgRoundTripTime(1.f)
{}

void NetworkManager::start() {
    auto im = InputManager::getInstance();
    auto sched = cocos2d::Director::getInstance()->getScheduler();
    m_avgRoundTripTime.init();
    auto sock = new UDPSocket(m_io, m_remoteIP, m_remotePort, m_stopNetThread);
    m_socket = std::unique_ptr<UDPSocket>(sock);
    m_netThread = std::thread(&UDPSocket::run, sock);
    m_netThread.detach();
}

void NetworkManager::stop() {
    m_io.stop();
}

void NetworkManager::step() {
    Packet packet;
    for (size_t i = 0; i < m_maxPacketsPerFrame; ++i) {
        if (!popPacket(packet)) {
            break;
        }
        auto dataptr = packet.getDataPtr();
        processPacket(dataptr);
    }
}

void NetworkManager::sendPacket(const bytes_t& buf) {
    m_socket->send(buf);
}

void NetworkManager::pushPacket(const Packet& packet) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_packets.push(packet);
}

bool NetworkManager::popPacket(Packet& packet) {
    std::unique_lock<std::mutex> lock(m_mutex);
    if (m_packets.empty()) {
        return false;
    }
    packet = m_packets.front();
    m_packets.pop();
    return true;
}

void NetworkManager::sendHelloPacket() {
    bytes_t buf;
    dx_write_bytes<PacketType>(buf, PacketType::HELLO);
    sendPacket(buf);
    m_clientState = ClientState::JOINING;
}

void NetworkManager::processPacket(uint8_t* &p) {
    auto packetType = dx_read_bytes<PacketType>(p);
    cocos2d::log("received packet: type=%d", int(packetType));
    switch (packetType) {
        case PacketType::WELCOME:
            onWelcomePacket(p);
            break;
        case PacketType::STATE:
            if (m_deliveryNotificationMgr.readAndProcessState(p)) {
                onStatePacket(p);
            }
            break;
        default:;
    }
}

GameObject* NetworkManager::getGameObject(net_id_t netId) {
    auto it = m_netId2GameObjectMap.find(netId);
    if (it == std::end(m_netId2GameObjectMap)) {
        return nullptr;
    }
    return it->second;
}

void NetworkManager::addGameObject(GameObject* obj) {
    m_netId2GameObjectMap.insert(obj->getNetId(), obj);
}

void NetworkManager::removeGameObject(GameObject* obj) {
    m_netId2GameObjectMap.erase(obj->getNetId());
}

void NetworkManager::updateBytesSentLastFrame() {

}

void NetworkManager::onWelcomePacket(uint8_t* &p) {
    if (m_clientState != ClientState::JOINING) {
        return;
    }
    m_playerId = dx_read_bytes<player_id_t>(p);
    m_clientState = ClientState::JOINED;
    auto sched = cocos2d::Director::getInstance()->getScheduler();
    cocos2d::log("joined as %d", m_playerId);
}

void NetworkManager::onStatePacket(uint8_t* &p) {
    if (m_clientState != ClientState::JOINED) {
        return;
    }
    readLastInputProcessedOnServerTimestamp(p);
    readRepBytes(p);
}

void NetworkManager::readLastInputProcessedOnServerTimestamp(uint8_t* &p) {
    bool isTimestampDirty = dx_read_bytes<bool>(p);
    if (isTimestampDirty) {
        auto gm = GameManager::getInstance();
        m_lastProcessedInputPacketTimestamp = dx_read_bytes<float>(p);
        cocos2d::log("last processed: %f", m_lastProcessedInputPacketTimestamp);
        float rtt = gm->getFrameStartTime() - m_lastProcessedInputPacketTimestamp;
        m_lastRoundTripTime = rtt;
        cocos2d::log("rtt=%f", rtt);
        m_avgRoundTripTime.update(rtt);
        auto im = InputManager::getInstance();
        im->removeInputPacketsByTimestamp(m_lastProcessedInputPacketTimestamp);
    }
}

void NetworkManager::readRepBytes(uint8_t* &p) {
    auto netId = dx_read_bytes<net_id_t>(p);
    auto action = dx_read_bytes<ReplicaActionType>(p);
    switch (action) {
        case ReplicaActionType::CREATE:
            readRepBytesAndDoCreateAction(p, netId);
            break;
        case ReplicaActionType::UPDATE:
            readRepBytesAndDoUpdateAction(p, netId);
            break;
        case ReplicaActionType::DESTROY:
            readRepBytesAndDoDestroyAction(p, netId);
            break;
        default:
            CC_ASSERT("Invalid replication action");
    }
}

void NetworkManager::readRepBytesAndDoCreateAction(uint8_t* &p, uint64_t netId) {
    // read class

    auto go = getGameObject(netId);
    if (!go) {
        auto gm = GameManager::getInstance();
        auto objMgr = ObjectManager::getInstance();
        auto objType = dx_read_bytes<GameObjectType>(p);
        auto label = dx_read_bytes<std::string>(p);
        go = objMgr->createObject(label, objType);
        cocos2d::log("created object: %p", go);
        go->setNetId(netId);
        addGameObject(go);
        auto ll = gm->getLevelLayer();
        //ll->addObject(go);
        auto ic = InputPacketComponent::create();
        ic->setName("input");
        go->addComponent(ic);
        ll->setPlayerObject(dynamic_cast<PlayerObject*>(go));
    }
    go->readStateBytes(p);
    auto sched = cocos2d::Director::getInstance()->getScheduler();
    auto im = InputManager::getInstance();
    sched->schedule(CC_CALLBACK_0(InputManager::sample, im),
        im, 0.1f, false, "sampleInput");
    sched->schedule(CC_CALLBACK_0(NetworkManager::sendInputPacket, this),
        this, 1.f/4.f, false, "sendInputPacket");
}

void NetworkManager::readRepBytesAndDoUpdateAction(uint8_t* &p, uint64_t netId) {
    auto go = getGameObject(netId);
    auto old = go->readStateBytes(p);
    go->clientSideInterpolate(old);
}

void NetworkManager::readRepBytesAndDoDestroyAction(uint8_t* &p, uint64_t netId) {

}

void NetworkManager::sendInputPacket() {
    bytes_t buf;
    dx_write_bytes<PacketType>(buf, PacketType::INPUT);
    m_deliveryNotificationMgr.writeState(buf);
    auto gm = GameManager::getInstance();
    auto im = InputManager::getInstance();
    auto& ipl = im->getInputPacketList();
    auto& uipl = ipl.getInputPackets();
    size_t np = 0;
    for (auto& p: uipl) {
        if (p.timestamp > m_lastSentTimestamp && p.processed) {
            np++;
        }
    }
    dx_write_bytes<size_t>(buf, np);
    for (auto& p: uipl) {
        if (p.timestamp > m_lastSentTimestamp && p.processed) {
            cocos2d::log("send ts=%f dt=%f", p.timestamp, p.delta);
            dx_write_bytes<InputPacket>(buf, p);
            m_lastSentTimestamp = p.timestamp;
        }
    }
    sendPacket(buf);
}

float NetworkManager::getRoundTripTime() const {
    return m_lastRoundTripTime;
}
