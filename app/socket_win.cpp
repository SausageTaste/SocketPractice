#include "socket_win.h"

#include <ws2tcpip.h>
#include <tchar.h>


namespace sungmin {

    SocketLibrary& SocketLibrary::inst() {
        static SocketLibrary inst;
        return inst;
    }

    SocketLibrary::SocketLibrary() {
        if (0 != WSAStartup(MAKEWORD(2, 2), &this->m_wsa)) {
            throw std::runtime_error{"Failed to initiate socket library"};
        }
    }

    SocketLibrary::~SocketLibrary() {
        WSACleanup();
    }

}


namespace sungmin {

    void SockAddress::set_inet_addr(const char* const ip_addr, const u_short port_num) {
        InetPton(AF_INET, _T(ip_addr), &this->m_data.sin_addr.s_addr);
        this->m_data.sin_family = AF_INET;
        this->m_data.sin_port = htons(port_num);
    }

    void SockAddress::set_inet_any_ip(const u_short port_num) {
        this->m_data.sin_addr.s_addr = INADDR_ANY;
        this->m_data.sin_family = AF_INET;
        this->m_data.sin_port = htons(port_num);
    }

    const sockaddr* SockAddress::get_raw_ptr() const {
        return reinterpret_cast<const sockaddr*>(&this->m_data);
    }

    constexpr size_t SockAddress::get_raw_size() const {
        return sizeof(struct sockaddr_in);
    }

}


// ClientInfo
namespace sungmin {

    std::string ClientInfo::address() const {
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(this->m_data.sin_addr), ipAddress, INET_ADDRSTRLEN);

        return std::string{ ipAddress };
    }

    u_short ClientInfo::port_num() const {
        return htons(this->m_data.sin_port);
    }

    std::string ClientInfo::make_str() const {
        return this->address() + ':' + std::to_string(this->port_num());
    }

    sockaddr* ClientInfo::get_raw_ptr() {
        return reinterpret_cast<sockaddr*>(&this->m_data);
    }

    const sockaddr* ClientInfo::get_raw_ptr() const {
        return reinterpret_cast<const sockaddr*>(&this->m_data);
    }

    constexpr size_t ClientInfo::get_raw_size() const {
        return sizeof(struct sockaddr_in);
    }

}


namespace sungmin {

    Socket::Socket() {
        this->m_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (INVALID_SOCKET == this->m_socket) {
            throw std::runtime_error{"Failed to initiate socket"};
        }
    }

    Socket::Socket(const SOCKET raw_handle)
        : m_socket(raw_handle)
    {
        if (INVALID_SOCKET == this->m_socket) {
            throw std::runtime_error{"Failed to initiate socket"};
        }
    }

    bool Socket::connect_to(const SockAddress& address) {
        if (connect(this->m_socket , address.get_raw_ptr(), static_cast<int>(address.get_raw_size())) < 0) {
            return false;
        }

        return true;
    }

    bool Socket::send_data(const char* const msg, const int msg_len) {
        const auto result = send(this->m_socket , msg , msg_len, 0);
        if (result < 0) {
            return false;
        }

        return true;
    }

    bool Socket::recieve_data(char* const output_buf, const int buf_size) {
        const auto recv_size = recv(this->m_socket, output_buf, buf_size, 0);
        if (SOCKET_ERROR == recv_size) {
            return false;
        }

        output_buf[recv_size] = '\0';
        return true;
    }

    bool Socket::bind_to(const SockAddress& address) {
        const auto bind_result = bind(this->m_socket, address.get_raw_ptr(), address.get_raw_size());
        if (bind_result == SOCKET_ERROR)
            return false;

        return true;
    }

    void Socket::listen_to_client() {
        listen(this->m_socket, 3);
    }

    std::optional<std::pair<Socket, ClientInfo>> Socket::accept_connection() {
        ClientInfo client_info;
        int client_info_size = client_info.get_raw_size();

        const auto new_socket = accept(this->m_socket, client_info.get_raw_ptr(), &client_info_size);

        if (INVALID_SOCKET == new_socket) {
            return std::nullopt;
        }

        return std::make_pair(sungmin::Socket{ new_socket }, client_info);
    }

}
