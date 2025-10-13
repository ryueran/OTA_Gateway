#pragma once

#ifndef INC_ACCEPTOR_HPP
#define INC_ACCEPTOR_HPP

#include <netinet/in.h>
#include <fcntl.h>
#include <cstdint>
#include <unistd.h>
#include <iostream>


template<std::uint32_t PORT>
class Acceptor
{
public:
    explicit Acceptor() 
    {
        server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (server_fd_ == -1) {
            std::cerr << "Socket creation failed" << std::endl;
            exit(EXIT_FAILURE);
        }
        int opt = 1;
        if (setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
            std::cerr << "Set socket options failed" << std::endl;
            close(server_fd_);
            exit(EXIT_FAILURE);
        }
        address_.sin_family = AF_INET;
        address_.sin_addr.s_addr = INADDR_ANY;
        address_.sin_port = htons(PORT);
    }

    ~Acceptor() {
        close(server_fd_);
        if (socket_fd_ != -1) {
            close(socket_fd_);
        }    
    }

    void server_bind()
    {
        if (bind(server_fd_, (struct sockaddr *)&address_, sizeof(address_)) < 0) {
            std::cerr << "Bind failed" << std::endl;
            close(server_fd_);
            exit(EXIT_FAILURE);
        }
    }

    void server_listen()
    {
        if (listen(server_fd_, 3) < 0) {
            std::cerr << "Listen failed" << std::endl;
            close(server_fd_);
            exit(EXIT_FAILURE);
        }

        int flags = fcntl(server_fd_, F_GETFL, 0);
        if (flags == -1) {
            std::cerr << "Get socket flags failed" << std::endl;
            close(server_fd_);
            exit(EXIT_FAILURE);
        }
    
        if (fcntl(server_fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
            std::cerr << "Set non-blocking mode failed" << std::endl;
            close(server_fd_);
            exit(EXIT_FAILURE);
        }
    }
    void server_accept(int server_fd)
    {
        socklen_t addrlen = sizeof(address_);
        socket_fd_ = accept(server_fd, (struct sockaddr *)&address_, &addrlen);
        if (socket_fd_ < 0) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "Accept failed" << std::endl;
                close(server_fd_);
                exit(EXIT_FAILURE);
            }
            socket_fd_ = -1; // No incoming connection
        }

        int flags = fcntl(socket_fd_, F_GETFL, 0);
        if (flags == -1) {
            std::cerr << "Get socket flags failed" << std::endl;
            close(socket_fd_);
            exit(EXIT_FAILURE);
        }

        if (fcntl(socket_fd_, F_SETFL, flags | O_NONBLOCK) == -1) {
            std::cerr << "Set non-blocking mode failed" << std::endl;
            close(socket_fd_);
            exit(EXIT_FAILURE);
        }
    }

    int get_socket_fd() const
    {
        return socket_fd_;
    }

    int get_server_fd() const
    {
        return server_fd_;
    }

private:
    int server_fd_;
    int socket_fd_;
    struct sockaddr_in address_;
};

#endif // INC_ACCEPTOR_HPP