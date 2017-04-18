#include "UDPSocket.h"
#include "NetworkManager.h"

UDPSocket::UDPSocket(asio::io_service& io,
    const std::string &host,
    ushort port,
    bool& stopFlag) :
    m_io(io),
    m_remoteHost(host),
    m_remotePort(port),
    m_remoteEndpoint(asio::ip::udp::v4(), 0),
    m_endpoint(asio::ip::udp::v4(), 0),
    m_resolver(m_io),
    m_socket(m_io),
    m_buf(4096),
    m_stopFlag(stopFlag)
{
    m_networkManager = NetworkManager::getInstance();
    do_resolve();
}

void UDPSocket::run() {
    m_io.run();
}

void UDPSocket::do_resolve() {
    asio::ip::udp::resolver::query query("localhost", "33333");
    m_resolver.async_resolve(query, [this](
        std::error_code ec, asio::ip::udp::resolver::iterator endpoint_it) {\
        if (ec) {
            cocos2d::log("resolve error: %s", ec.message().c_str());
        } else {
            m_remoteEndpoint = *endpoint_it;
            m_socket = asio::ip::udp::socket(m_io, m_endpoint);
            m_networkManager->sendHelloPacket();
            do_receive();
        }
    });
}

void UDPSocket::do_receive() {
    if (m_stopFlag) {
        return;
    }
    m_socket.async_receive_from(
        asio::buffer(m_buf.data(), m_buf.size()), m_remoteEndpoint,
        [this](std::error_code ec, std::size_t nbytes) {
            if (ec) {
                cocos2d::log("send error");
            } else {
                //cocos2d::log("receive %d", int(nbytes));
                Packet packet(m_buf.data(), nbytes, m_remoteEndpoint);
                m_networkManager->pushPacket(packet);
            }
            do_receive();
        }
    );
}

void UDPSocket::send(const std::vector<uint8_t>& buf) {
    m_socket.async_send_to(
        asio::buffer(buf.data(), buf.size()), m_remoteEndpoint,
        [this](std::error_code ec, std::size_t nbytes) {
            if (ec) {
                cocos2d::log("send error");
            } else {
                //cocos2d::log("send %d", int(nbytes));
            }
        }
    );
}
