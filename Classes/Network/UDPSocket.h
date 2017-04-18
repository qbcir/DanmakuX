#ifndef CLIENT_H
#define CLIENT_H

#include "asio/include/asio.hpp"
#include "Packet.h"
#include "WeightedTimedMovingAverage.h"

class NetworkManager;
class UDPSocket {
public:
    UDPSocket(asio::io_service& io,
              const std::string& host,
              ushort port,
              bool& stopFlag);
    void run();
    void send(const std::vector<uint8_t> &buf);
protected:
    void do_resolve();
    void do_receive();
    bool& m_stopFlag;
    asio::io_service& m_io;
    std::string m_remoteHost;
    ushort m_remotePort;
    asio::ip::udp::endpoint m_endpoint;
    asio::ip::udp::endpoint m_remoteEndpoint;
    asio::ip::udp::resolver m_resolver;
    asio::ip::udp::socket m_socket;
    std::vector<uint8_t> m_buf;
    NetworkManager* m_networkManager = nullptr;
    //
    WeightedTimedMovingAverage m_nbytesReceivedPerSec;
    WeightedTimedMovingAverage m_nbytesSentPerSec;
};

#endif // CLIENT_H
