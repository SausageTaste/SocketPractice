#include <iostream>

#include "socket_win.h"


int main(int argc , char *argv[]) {
    auto& socket_lib = sungmin::SocketLibrary::inst();

    sungmin::SockAddress address;
    address.set_inet_any_ip(8888);

    sungmin::Socket socket;

    if (!socket.bind_to(address)) {
        printf("Bind failed with error code : %d" , WSAGetLastError());
        std::cout << "Bind failed with error code : " << WSAGetLastError() << '\n';
        return -1;
    }

    socket.listen_to_client();

    while (auto result = socket.accept_connection()) {
        std::cout << "Connection accepted\n";

        //Reply to the client
        std::cout << "Got connection from " << result->second.address() << ':' << result->second.port_num() << '\n';

        std::string msg;
        msg += "Hello mate. Your address is ";
        msg += result->second.make_str();

        result->first.send_data(msg.c_str(), msg.size());
    }

	return 0;
}
