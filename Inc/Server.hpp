#pragma once

#ifndef INC_SERVER_HPP
#define INC_SERVER_HPP

#include <iostream>
#include <map>
#include "Acceptor.hpp"
#include "Constant.hpp"
#include "Reactor.hpp"
#include "session/Session.hpp"

class Server {
public:
    explicit Server()
    {
        std::cout << "Server created." << std::endl;
    }

    ~Server()
    {
        std::cout << "Server destroyed." << std::endl;
    }

    void start_server();
    void accept(int server_fd);
    void read_client(int client_fd);
    void write_client(int client_fd);
    void checkTimeouts();
    void close_session(int fd);
private:
    int server_fd_;
    Acceptor<PORT> acceptor_;
    Reactor reactor_;
    std::unordered_map<int, std::shared_ptr<Session>> sessions_;
};

#endif // INC_SERVER_HPP