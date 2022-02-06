#include <iostream>

#include "socket_win.h"


int main() {
    sungmin::Socket socket;

    if (!socket.init()) {
        std::cout << "Failed to initiate socket\n";
        return -1;
    }

    sungmin::SockAddress address;
    address.set_inet_addr("223.130.200.104", 80);

    if (!socket.connect_to(address)) {
        std::cout << "Failed to connect\n";
        return -1;
    }

    std::cout << "All done successfully\n";
    return 0;
}
