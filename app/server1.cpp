#include <iostream>

#include <fmt/format.h>

#include "socket_win.h"


int main() {
    auto& socket_lib = sungmin::SocketLibrary::inst();

    sungmin::SockAddress address;
    address.set_inet_any_ip(8888);

    sungmin::Socket socket;
    socket.init();
    if (!socket.bind_to(address)) {
        fmt::print("Bind failed with error code : {}\n", WSAGetLastError());
        return -1;
    }

    const auto server_addr = socket.get_address_info();
    socket.listen_to_client();
    fmt::print("Server started: {}\n", server_addr->make_str());

    while (auto result = socket.accept_connection()) {
        const auto client_addr = result->second.make_str();
        fmt::print("Connection from {}\n", client_addr);

        const auto msg = fmt::format("Hello mate. Your address is {}\n", client_addr);
        result->first.send_data(msg.c_str(), msg.size());
    }

	return 0;
}
