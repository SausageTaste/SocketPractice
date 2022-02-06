#pragma once

#include <stdexcept>

#include <WinSock2.h>


namespace sungmin {

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
        void set_inet_addr(const char* const ip_addr, const u_short port_num);

        const sockaddr* get_raw_ptr() const;

        constexpr size_t get_raw_size() const;

    };


    class Socket {

    private:
        SOCKET m_socket;

    public:
        Socket(const Socket&) = delete;
        Socket(Socket&&) = delete;
        Socket& operator=(const Socket&) = delete;
        Socket& operator=(Socket&&) = delete;

    public:
        Socket();

        bool connect_to(const SockAddress& address);

        bool send_data(const char* const msg, const int msg_len);

        bool recieve_data(char* const output_buf, const int buf_size);

    };

}
