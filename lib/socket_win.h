#pragma once

#include <string>
#include <optional>
#include <stdexcept>

#include <WinSock2.h>


namespace sungmin {

    enum class AddressFamily { ipv4, ipv6, unknown };

    enum class SocketType { tcp, udp, unknown };


    class SocketLibrary {

    private:
        WSADATA m_wsa;

    public:
        SocketLibrary(const SocketLibrary&) = delete;
        SocketLibrary(SocketLibrary&&) = delete;
        SocketLibrary& operator=(const SocketLibrary&) = delete;
        SocketLibrary& operator=(SocketLibrary&&) = delete;

    public:
        static SocketLibrary& inst();

        SocketLibrary();

        ~SocketLibrary();

    };


    class SockAddress {

    private:
        struct sockaddr_in m_data;

    public:
        std::string address() const;

        u_short port_num() const;

        constexpr
        AddressFamily family() const;

        std::string make_str() const;

        void set_inet_addr(const char* const ip_addr, const u_short port_num);

        void set_inet_any_ip(const u_short port_num);

        sockaddr* get_raw_ptr();

        const sockaddr* get_raw_ptr() const;

        constexpr
        size_t get_raw_size() const;

    };


    class Socket {

    public:
        enum class RecvResult { failed, ok, closed };

    private:
        SOCKET m_socket;

    public:
        Socket(const Socket&) = delete;
        Socket& operator=(const Socket&) = delete;

    public:
        Socket();

        explicit
        Socket(const SOCKET raw_handle);

        Socket(const AddressFamily addr_fam, const SocketType type);

        ~Socket();

        Socket(Socket&& other);

        Socket& operator=(Socket&& other);

        bool init(const AddressFamily addr_fam, const SocketType type);

        void destory();

        bool is_ready() const;

        void connect_to(const SockAddress& address);

        bool send_data(const char* const msg, const size_t msg_len);

        std::pair<RecvResult, size_t> recieve_data(char* const output_buf, const size_t buf_size);

        bool bind_to(const SockAddress& address);

        void listen_to_client();

        std::optional<SockAddress> get_address_info();

        std::optional<std::pair<Socket, SockAddress>> accept_connection();

    };

}
