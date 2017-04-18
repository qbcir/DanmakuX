#include "Packet.h"

Packet::Packet(uint8_t *ptr,
        size_t len,
        const asio::ip::udp::endpoint& endpoint) :
    m_endpoint(endpoint)
{
    m_data.assign(ptr, ptr + len);
}

Packet::Packet(const bytes_t& data,
       const asio::ip::udp::endpoint& endpoint) :
    m_data(data),
    m_endpoint(endpoint)
{}

uint8_t* Packet::getDataPtr() {
    return m_data.data();
}

size_t Packet::getDataLen() {
    return m_data.size();
}

const bytes_t& Packet::getData() {
    return m_data;
}

const asio::ip::udp::endpoint& Packet::getEndpoint() const {
    return m_endpoint;
}
