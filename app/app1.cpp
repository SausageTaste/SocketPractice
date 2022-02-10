#include <string>
#include <vector>
#include <iostream>

#include "socket_win.h"


int main() {
    auto& socket_lib = sungmin::SocketLibrary::inst();

    sungmin::SockAddress address;
    address.set_inet_addr("223.130.200.104", 80);

    sungmin::Socket socket;
    socket.init();
    if (!socket.connect_to(address)) {
        std::cout << "Failed to connect\n";
        return -1;
    }

    std::string msg = "GET / HTTP/1.1\r\n\r\n";

    if (!socket.send_data( msg.c_str(), static_cast<int>(msg.size()) )) {
        std::cout << "Failed to send data\n";
        return -1;
    }

    std::vector<char> server_reply(1024 * 8);

    if (!socket.recieve_data( server_reply.data(), static_cast<int>(server_reply.size()) )) {
        std::cout << "Failed to recieve data\n";
        return -1;
    }

    std::cout << "Server reply is following...\n'''\n" << server_reply.data() << "\n'''\n";

    std::cout << "All done successfully\n";
    return 0;
}
