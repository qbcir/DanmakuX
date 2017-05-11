#ifndef __dx_PACKET_H__
#define __dx_PACKET_H__

#include "NetTypes.h"
#include "asio.hpp"
#include <vector>

enum class PacketType : packet_t {
    HELLO,
    WELCOME,
    CREATE,
    INPUT,
    STATE,
    INVALID
};

class Packet {
public:
    Packet() {}
    Packet(uint8_t *ptr, size_t len,
           const asio::ip::udp::endpoint& endpoint);
    Packet(const bytes_t& data,
           const asio::ip::udp::endpoint& endpoint);
    uint8_t* getDataPtr();
    size_t getDataLen();
    const bytes_t& getData();
    const asio::ip::udp::endpoint& getEndpoint() const;
protected:
    bytes_t m_data;
    asio::ip::udp::endpoint m_endpoint;
};

#endif /* __dx_PACKET_H__ */
