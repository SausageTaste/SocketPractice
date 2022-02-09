#include <iostream>

#include "socket_win.h"


int main() {
    auto& socket_lib = sungmin::SocketLibrary::inst();

    sungmin::SockAddress address;
    address.set_inet_any_ip(8888);

    sungmin::Socket socket;

    if (!socket.bind_to(address)) {
        std::cout << "Bind failed with error code : " << WSAGetLastError() << '\n';
        return -1;
    }

    const auto server_addr = socket.get_address_info();
    std::cout << "Server started: " << server_addr->make_str() << '\n';

    socket.listen_to_client();

    while (auto result = socket.accept_connection()) {
        const auto client_addr = result->second.make_str();
        std::cout << "Connection from " << client_addr << '\n';

        std::string msg;
        msg += "Hello mate. Your address is ";
        msg += client_addr;

        result->first.send_data(msg.c_str(), msg.size());
    }

	return 0;
}
