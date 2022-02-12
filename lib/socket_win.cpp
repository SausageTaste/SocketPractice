#include "socket_win.h"

#include <numeric>

#include <fmt/format.h>
#include <ws2tcpip.h>
#include <tchar.h>


namespace {

    constexpr auto INT_MAX_VALUE = static_cast<size_t>((std::numeric_limits<int>::max)());


    constexpr
    sungmin::AddressFamily convert_address_family(const ADDRESS_FAMILY addr_fam) {
        using namespace sungmin;

        switch (addr_fam) {
            case AF_INET:
                return AddressFamily::ipv4;
            case AF_INET6:
                return AddressFamily::ipv6;
            default:
                return AddressFamily::unknown;
        }
    }

    // Returns 0 on error
    constexpr
    ADDRESS_FAMILY convert_address_family(const sungmin::AddressFamily addr_fam) {
        using namespace sungmin;

        switch (addr_fam) {
            case AddressFamily::ipv4:
                return AF_INET;
            case AddressFamily::ipv6:
                return AF_INET6;
            default:
                return AF_UNSPEC;
        }
    }

    constexpr
    sungmin::SocketType convert_sock_type(const int socket_type) {
        using namespace sungmin;

        switch (socket_type) {
            case SOCK_STREAM:
                return SocketType::tcp;
            case SOCK_DGRAM:
                return SocketType::udp;
            default:
                return SocketType::unknown;
        }
    }

    // Returns 0 on error
    constexpr
    int convert_sock_type(const sungmin::SocketType socket_type) {
        using namespace sungmin;

        switch (socket_type) {
            case SocketType::tcp:
                return SOCK_STREAM;
            case SocketType::udp:
                return SOCK_DGRAM;
            default:
                return 0;
        }
    }

}


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

    std::string SockAddress::address() const {
        char ipAddress[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(this->m_data.sin_addr), ipAddress, INET_ADDRSTRLEN);

        return std::string{ ipAddress };
    }

    u_short SockAddress::port_num() const {
        return htons(this->m_data.sin_port);
    }

    constexpr
    AddressFamily SockAddress::family() const {
        return ::convert_address_family(this->m_data.sin_family);
    }

    std::string SockAddress::make_str() const {
        return this->address() + ':' + std::to_string(this->port_num());
    }

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

    sockaddr* SockAddress::get_raw_ptr() {
        return reinterpret_cast<sockaddr*>(&this->m_data);
    }

    const sockaddr* SockAddress::get_raw_ptr() const {
        return reinterpret_cast<const sockaddr*>(&this->m_data);
    }

    constexpr
    size_t SockAddress::get_raw_size() const {
        return sizeof(struct sockaddr_in);
    }

}


namespace sungmin {

    Socket::Socket()
        : m_socket(INVALID_SOCKET)
    {

    }

    Socket::Socket(const SOCKET raw_handle)
        : m_socket(raw_handle)
    {

    }

    Socket::Socket(const AddressFamily addr_fam, const SocketType type) {
        this->destory();
        this->m_socket = socket(::convert_address_family(addr_fam), ::convert_sock_type(type), 0);

        if (!this->is_ready())
            throw std::runtime_error{ "Failed to create a socket" };
    }

    Socket::~Socket() {
        this->destory();
    }

    Socket::Socket(Socket&& other) {
        const auto tmp = other.m_socket;
        other.m_socket = this->m_socket;
        this->m_socket = tmp;
    }

    Socket& Socket::operator=(Socket&& other) {
        const auto tmp = other.m_socket;
        other.m_socket = this->m_socket;
        this->m_socket = tmp;

        return *this;
    }

    void Socket::destory() {
        if (!this->is_ready()) {
            closesocket(this->m_socket);
            this->m_socket = INVALID_SOCKET;
        }
    }

    bool Socket::is_ready() const {
        return INVALID_SOCKET != this->m_socket;
    }

    void Socket::connect_to(const SockAddress& address) {
        const auto result = connect(this->m_socket, address.get_raw_ptr(), static_cast<int>(address.get_raw_size()));

        if (SOCKET_ERROR == result)
            throw std::runtime_error{fmt::format("Failed to connect with error code {}", WSAGetLastError())};
    }

    void Socket::send_data(const char* const msg, const size_t msg_len) {
        if (msg_len >= ::INT_MAX_VALUE)
            throw std::runtime_error{"Message length is too long"};

        const auto msg_len_int = static_cast<int>(msg_len);
        const auto sent_bytes = send(this->m_socket, msg, msg_len_int, 0);

        if (SOCKET_ERROR == sent_bytes)
            throw std::runtime_error{"Failed to send data"};
        if (msg_len_int != sent_bytes)
            throw std::runtime_error{"Some part of the message was not sent"};
    }

    std::pair<Socket::RecvResult, size_t> Socket::recieve_data(char* const output_buf, const size_t buf_size) {
        const auto recv_size = recv(this->m_socket, output_buf, static_cast<int>(buf_size), 0);

        if (SOCKET_ERROR == recv_size)
            throw std::runtime_error{fmt::format("Failed to recieve data with error code {}", WSAGetLastError())};
        else if (0 == recv_size)
            return std::make_pair(RecvResult::closed, 0);
        else
            return std::make_pair(RecvResult::ok, recv_size);
    }

    void Socket::bind_to(const SockAddress& address) {
        const auto bind_result = bind(this->m_socket, address.get_raw_ptr(), address.get_raw_size());
        if (bind_result == SOCKET_ERROR)
            throw std::runtime_error{fmt::format(
                "Failed to bind to {} with error code {}",
                address.make_str(),
                WSAGetLastError()
            )};
    }

    void Socket::listen_to_client() {
        if (SOCKET_ERROR == listen(this->m_socket, SOMAXCONN)) {
            throw std::runtime_error{fmt::format("Listen failed with error code {}", WSAGetLastError())};
        }
    }

    void Socket::shutdown_sending() {
        if (SOCKET_ERROR == shutdown(this->m_socket, SD_SEND)) {
            throw std::runtime_error{"Failed to shutdown for sending"};
        }
    }

    SockAddress Socket::get_address_info() {
        SockAddress output;
        auto output_size = static_cast<int>(output.get_raw_size());

        const auto result = getsockname(this->m_socket, output.get_raw_ptr(), &output_size);

        if (result != 0 || output.family() != AddressFamily::ipv4 || output_size != output.get_raw_size())
            throw std::runtime_error{"Failed to get socket address info"};

        return output;
    }

    std::optional<std::pair<Socket, SockAddress>> Socket::accept_connection() {
        SockAddress client_info;
        int client_info_size = client_info.get_raw_size();

        const auto new_socket = accept(this->m_socket, client_info.get_raw_ptr(), &client_info_size);

        if (INVALID_SOCKET == new_socket) {
            throw std::runtime_error{fmt::format("Failed to accept a client with error code {}", WSAGetLastError())};
        }

        return std::make_pair(sungmin::Socket{ new_socket }, client_info);
    }

}
