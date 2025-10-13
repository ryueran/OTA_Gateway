#include <functional>
#include <cstdint>
#include <unistd.h>
#include <limits.h>
#include <cstring>
#include "ReadHandler.hpp"
#include "WriteHandler.hpp"
#include "Server.hpp"

void Server::start_server()
{
    acceptor_.server_bind();
    acceptor_.server_listen();
    server_fd_ = acceptor_.get_server_fd();

    auto server_handler = std::make_shared<ReadHandler<BUFFER_SIZE>>(server_fd_);
    client_handlers_[server_fd_] = server_handler;
    server_handler->setCallback(std::bind(&Server::accept, this, std::placeholders::_1));
    reactor_.registerHandler(server_handler);
    reactor_.eventLoop();
}

void Server::accept(int server_fd)
{
    acceptor_.server_accept(server_fd);
    int client_fd = acceptor_.get_socket_fd();
    if (client_fd != -1) {
        std::cout << "Accepted new connection, fd: " << client_fd << std::endl;
        auto client_handler = std::make_shared<WriteHandler<BUFFER_SIZE>>(client_fd);
        client_handlers_[client_fd] = client_handler;
        client_handler->setCallback(std::bind(&Server::read_client, this, std::placeholders::_1));
        reactor_.registerHandler(client_handler);
    }
}

void Server::read_client(int client_fd)
{
    auto it = client_handlers_.find(client_fd);
    if (it != client_handlers_.end()) {
        auto handler = it->second;
        ssize_t bytes_read = read(client_fd, handler->message_buffer, BUFFER_SIZE);
        if (bytes_read > 0) {
            std::cout << "Read " << bytes_read << " bytes from fd " << client_fd << std::endl;
            handler->message_buffer[bytes_read] = '\0'; // Null-terminate the buffer
            std::cout << "Message: " << handler->message_buffer << std::endl;
            handler->setCallback(std::bind(&Server::write_client, this, std::placeholders::_1));
            reactor_.registerHandler(handler);
        } else if (bytes_read == 0) {
            std::cout << "Client disconnected, fd: " << client_fd << std::endl;
            reactor_.unregisterHandler(handler);
            client_handlers_.erase(it);
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "Read error on fd " << client_fd << std::endl;
                reactor_.unregisterHandler(handler);
                client_handlers_.erase(it);
            }
        }
    }
}

void Server::write_client(int client_fd)
{
    auto it = client_handlers_.find(client_fd);
    if (it != client_handlers_.end()) {
        auto handler = it->second;
        ssize_t bytes_written = write(client_fd, handler->message_buffer, strlen(handler->message_buffer));
        if (bytes_written > 0) {
            std::cout << "Wrote " << bytes_written << " bytes to fd " << client_fd << std::endl;
            handler->setCallback(std::bind(&Server::read_client, this, std::placeholders::_1));
            // handler->setEventType(EventType::Read);
            reactor_.registerHandler(handler);
        } else {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                std::cerr << "Write error on fd " << client_fd << std::endl;
                reactor_.unregisterHandler(handler);
                client_handlers_.erase(it);
            }
        }
    }
}
