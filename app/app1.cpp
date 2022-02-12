#include <string>
#include <vector>
#include <iostream>

#include "socket_win.h"


int main() try {
    auto& socket_lib = sungmin::SocketLibrary::inst();

    sungmin::SockAddress address;
    address.set_inet_addr("223.130.200.104", 80);

    sungmin::Socket socket{ sungmin::AddressFamily::ipv4, sungmin::SocketType::tcp };
    socket.connect_to(address);

    const std::string msg = "GET / HTTP/1.1\r\n\r\n";
    socket.send_data( msg.c_str(), static_cast<int>(msg.size()) );
    socket.shutdown_sending();

    std::vector<char> server_reply(32);

    while (true) {
        const auto [recv_result, recv_size] = socket.recieve_data( server_reply.data(), static_cast<int>(server_reply.size() - 1) );
        server_reply[recv_size] = '\0';

        switch (recv_result) {
            case sungmin::Socket::RecvResult::ok:
                std::cout << "Server reply is following...\n<<<\n" << server_reply.data() << "\n>>>\n";
                break;
            case sungmin::Socket::RecvResult::closed:
                std::cout << "Connection closed successfully\n";
                return 0;
            case sungmin::Socket::RecvResult::failed:
                std::cout << "Failed to recieve data\n";
                return -1;
            default:
                std::cout << "Unknown recieve result\n";
        }
    }
}
catch (const std::exception& e) {
    std::cout << "An exception caught: " << e.what() << '\n';
    throw e;
}
