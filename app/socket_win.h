#include <WinSock2.h>


namespace sungmin {

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

    };

}
