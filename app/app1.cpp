#include <string>
#include <vector>
#include <iostream>

#include "socket_win.h"


int main() try {
    auto& socket_lib = sungmin::SocketLibrary::inst();

    sungmin::SockAddress address;
    address.set_inet_addr("127.0.0.1", 8888);

    sungmin::Socket socket{ sungmin::AddressFamily::ipv4, sungmin::SocketType::tcp };
    socket.connect_to(address);

    while (true) {
        std::cout << ">>>";
        std::string user_input;
        std::getline(std::cin, user_input);

        socket.send_data(user_input.data(), user_input.size());
    }
}
catch (const std::exception& e) {
    std::cout << "An exception caught: " << e.what() << '\n';
    throw e;
}
