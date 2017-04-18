#include "NetworkManager.h"
#include "../GameManager.h"
#include "UDPSocket.h"
#include "../Input/KeyboardManager.h"
#include "../Input/MouseManager.h"
#include "InputPacket.h"
#include "../Layers/LevelLayer.h"

NetworkManager* NetworkManager::getInstance() {
    return GameManager::getInstance()->getNetworkManager();
}

NetworkManager::NetworkManager()
{}

void NetworkManager::start() {
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
    sched->schedule(
        CC_CALLBACK_0(NetworkManager::sendInputPacket, this),
        this, 0.33f, false, "sendInputPacket");
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
        m_lastProcessedInputPacketTimestamp = dx_read_bytes<float>(p);
    }
    // FIXME TODO
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
            CCASSERT("Invalid replication action: %d", int(action));
    }
}

void NetworkManager::readRepBytesAndDoCreateAction(uint8_t* &p, uint64_t netId) {
    // read class
    auto go = getGameObject(netId);
    if (!go) {
        go->setNetId(netId);
        addGameObject(go);
    }
    go->readStateBytes(p);
}

void NetworkManager::readRepBytesAndDoUpdateAction(uint8_t* &p, uint64_t netId) {
    auto go = getGameObject(netId);
    go->readStateBytes(p);
}

void NetworkManager::readRepBytesAndDoDestroyAction(uint8_t* &p, uint64_t netId) {

}

void NetworkManager::sendInputPacket() {
    auto gameMgr = GameManager::getInstance();
    auto kbdMgr = KeyboardManager::getInstance();
    auto mouseMgr = MouseManager::getInstance();
    InputPacket packet(kbdMgr->getKeysPressed(),
        mouseMgr->getButtonsPressed(), gameMgr->getTime(), 0);
    bytes_t buf;
    dx_write_bytes<PacketType>(buf, PacketType::INPUT);
    m_deliveryNotificationMgr.writeState(buf);
    dx_write_bytes<size_t>(buf, 1);// movesCount FIXME TODO
    dx_write_bytes<InputPacket>(buf, packet);
    sendPacket(buf);
}
