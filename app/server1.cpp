#include <vector>
#include <thread>
#include <memory>
#include <iostream>

#include <fmt/format.h>

#include "socket_win.h"


namespace {

    class Session {

    private:
        sungmin::SockAddress m_addr;
        sungmin::Socket m_socket;

    public:
        explicit
        Session(sungmin::Socket&& socket, const sungmin::SockAddress& address)
            : m_addr(address)
            , m_socket(std::move(socket))
        {

        }

        void operator()() {
            std::vector<char> buffer(256);

            while (true) {
                const auto [recv_result, recv_size] = this->m_socket.recieve_data(buffer.data(), buffer.size() - 1);

                switch (recv_result) {
                    case sungmin::Socket::RecvResult::ok:
                        buffer[recv_size] = '\0';
                        fmt::print("[{}] {}\n", this->m_addr.make_str(), buffer.data());
                        break;
                    case sungmin::Socket::RecvResult::closed:
                        fmt::print("[{}] Connection closed\n", this->m_addr.make_str());
                        return;
                    case sungmin::Socket::RecvResult::failed:
                        fmt::print("[{}] Connection lost\n", this->m_addr.make_str());
                    default:
                        return;
                }
            }
        }

    };


    class SessionThread {

    private:
        ::Session m_session;
        std::thread m_thread;

    public:
        SessionThread(sungmin::Socket&& socket, const sungmin::SockAddress& address)
            : m_session(std::move(socket), address)
            , m_thread(std::ref(this->m_session))
        {

        }

        ~SessionThread() {
            this->m_thread.join();
        }

    };

}


int main() try {
    auto& socket_lib = sungmin::SocketLibrary::inst();

    std::vector<std::unique_ptr<::SessionThread>> sessions;
    sungmin::SockAddress address;
    address.set_inet_any_ip(8888);

    sungmin::Socket socket{ sungmin::AddressFamily::ipv4, sungmin::SocketType::tcp };
    socket.bind_to(address);

    const auto server_addr = socket.get_address_info();
    socket.listen_to_client();
    fmt::print("Server started: {}\n", server_addr.make_str());

    while (auto result = socket.accept_connection()) {
        const auto client_addr = result->second.make_str();
        fmt::print("Connection from {}\n", client_addr);

        sessions.push_back(
            std::make_unique<::SessionThread>(std::move(result->first), result->second)
        );
    }

	return 0;
}
catch (const std::exception& e) {
    std::cout << "An exception caught: " << e.what() << '\n';
    throw e;
}
