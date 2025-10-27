#include <functional>
#include <cstdint>
#include <unistd.h>
#include <limits.h>
#include <cstring>
#include <thread>
#include <future>

#include "ReadHandler.hpp"
#include "WriteHandler.hpp"
#include "Server.hpp"

void Server::start_server()
{
    acceptor_.server_bind();
    acceptor_.server_listen();
    server_fd_ = acceptor_.get_server_fd();

    auto server_handler = std::make_shared<ReadHandler<BUFFER_SIZE>>(server_fd_);
    server_handler->setCallback(std::bind(&Server::accept, this, std::placeholders::_1));
    reactor_.registerHandler(server_handler);

    // tech debt
    std::thread([this]() {
        while (true) {
            checkTimeouts();
        }
    }).detach();

    reactor_.eventLoop();
}

void Server::accept(int server_fd)
{
    acceptor_.server_accept(server_fd);
    int client_fd = acceptor_.get_socket_fd();
    if (client_fd != -1) {
        std::cout << "Accepted new connection, fd: " << client_fd << std::endl;
        auto client_handler = std::make_shared<WriteHandler<BUFFER_SIZE>>(client_fd);
        auto session = std::make_shared<Session>(client_fd, client_handler);

        client_handler->setCallback(std::bind(&Server::read_client, this, std::placeholders::_1));
        reactor_.registerHandler(client_handler);
        sessions_[client_fd] = session;
    }
}

void Server::read_client(int client_fd)
{
    auto it = sessions_.find(client_fd);
    if (it == sessions_.end()) return;
    auto session = it->second;
    auto handler = session->handler_;

    std::cout << "Session updated by read" << std::endl;

    ssize_t bytes_read = read(client_fd, handler->message_buffer, BUFFER_SIZE);
    if(bytes_read > 0)
    {
        session->refresh();
        handler->message_buffer[bytes_read] = '\0';
        std::cout << "Read " << bytes_read << " bytes from fd " << client_fd << std::endl;
        handler->setCallback(std::bind(&Server::write_client, this, std::placeholders::_1));
        reactor_.registerHandler(handler);
    } else if (bytes_read == 0) {
        close_session(client_fd);
    }
}

void Server::write_client(int client_fd)
{
    auto it = sessions_.find(client_fd);
    if(it == sessions_.end()) return;
    auto session = it->second;
    auto handler = session->handler_;

    std::cout << "Session updated by write" << std::endl;

    ssize_t bytes_written = write(client_fd, handler->message_buffer, strlen(handler->message_buffer));
    if (bytes_written > 0) {
        session->refresh();
        handler->setCallback(std::bind(&Server::read_client, this, std::placeholders::_1));
        reactor_.registerHandler(handler);
    } else {
        close_session(client_fd);
    }
}

void Server::checkTimeouts()
{
    for (auto it = sessions_.begin(); it != sessions_.end(); ) {
        auto session = it->second;
        if (std::difftime(std::time(nullptr), session->last_activate_time_) > 10) {
            std::cout << "Session timeout, closing fd " << session->fd_<< std::endl;
            close_session(session->fd_);
            it = sessions_.erase(it);
        } else {
            ++it;
        }
    }
}

void Server::close_session(int fd)
{
    auto it = sessions_.find(fd);
    if (it != sessions_.end()) {
        reactor_.unregisterHandler(it->second->handler_);
        std::cout << "Close session of: " << fd << std::endl;
    }
}