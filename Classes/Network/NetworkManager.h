#ifndef __dx_NETWORKMANAGER_H__
#define __dx_NETWORKMANAGER_H__

#include "cocos2d.h"
#include "asio/include/asio.hpp"
#include "Packet.h"
#include "WeightedTimedMovingAverage.h"
#include "DeliveryNotificationManager.h"
#include "../Objects/GameObject.h"
#include "UDPSocket.h"
#include "InputPacket.h"

enum class ReplicaActionType : uint8_t {
    CREATE,
    UPDATE,
    DESTROY,
    RPC,
    INVALID
};

class KeyboardManager;
class MouseManager;
class LevelLayer;
class NetworkManager {
public:
    enum class ClientState {
        UNINITIALIZED,
        JOINING,
        JOINED,
        INVALID = -1
    };
    static NetworkManager* getInstance();
    NetworkManager();
    void start();
    void step();
    void stop();
    float getRoundTripTime() const;
    void pushPacket(const Packet& packet);
    GameObject* getGameObject(net_id_t netId);
    void addGameObject(GameObject* obj);
    void removeGameObject(GameObject* obj);
    void sendHelloPacket();
private:
    //
    bool popPacket(Packet& packet);
    void sendPacket(const bytes_t &buf);
    void sendInputPacket();
    void onWelcomePacket(uint8_t* &p);
    void onStatePacket(uint8_t* &p);
    void readLastInputProcessedOnServerTimestamp(uint8_t* &p);
    // Replication
    void readRepBytes(uint8_t* &p);
    void readRepBytesAndDoCreateAction(uint8_t* &p, uint64_t netId);
    void readRepBytesAndDoUpdateAction(uint8_t* &p, uint64_t netId);
    void readRepBytesAndDoDestroyAction(uint8_t* &p, uint64_t netId);
    //
    void processPacket(uint8_t* &p);
    void updateBytesSentLastFrame();
    //
    float m_lastRoundTripTime;
    float m_lastSentTimestamp = -1.f;
    float m_lastProcessedInputPacketTimestamp;
    //
    LevelLayer* m_stage = nullptr;
    //
    ClientState m_clientState = ClientState::UNINITIALIZED;
    player_id_t m_playerId = 0;
    std::string m_remoteIP = "localhost";
    short m_remotePort = 33333;
    std::unique_ptr<UDPSocket> m_socket;
    bool m_stopNetThread = false;
    asio::io_service m_io;
    std::thread m_netThread;
    std::mutex m_mutex;
    std::queue<Packet> m_packets;
    size_t m_maxPacketsPerFrame = 256;
    cocos2d::Map<net_id_t, GameObject*> m_netId2GameObjectMap;
    DeliveryNotificationManager	m_deliveryNotificationMgr;
    WeightedTimedMovingAverage m_avgRoundTripTime;
};

#endif /* __dx_NETWORKMANAGER_H__ */
