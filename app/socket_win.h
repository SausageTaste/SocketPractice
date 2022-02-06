#include <WinSock2.h>


namespace sungmin {

    class SockAddress {

    private:
        struct sockaddr_in m_data;

    public:
        void set_inet_addr(const char* const ip_addr, const u_short port_num) {
            this->m_data.sin_addr.s_addr = inet_addr(ip_addr);
            this->m_data.sin_family = AF_INET;
            this->m_data.sin_port = htons(port_num);
        }

        auto get_raw_ptr() const {
            return reinterpret_cast<const struct sockaddr*>(&this->m_data);
        }

        constexpr auto get_raw_size() const {
            return sizeof(struct sockaddr_in);
        }

    };


    class Socket {

    private:
        WSADATA m_wsa;
        SOCKET m_socket;

    public:
        bool init() {
            if (0 != WSAStartup(MAKEWORD(2, 2), &this->m_wsa)) {
                return false;
            }

            this->m_socket = socket(AF_INET, SOCK_STREAM, 0);
            if (INVALID_SOCKET == this->m_socket) {
                return false;
            }

            return true;
        }

        bool connect_to(const SockAddress& address) {
            if (connect(this->m_socket , address.get_raw_ptr(), address.get_raw_size()) < 0) {
                return false;
            }

            return true;
        }

    };

}
