#include <iostream>

#include "socket_win.h"


int main() {
    sungmin::Socket socket;
    socket.init();

    std::cout << "Hello world!" << std::endl;
    return 0;
}
