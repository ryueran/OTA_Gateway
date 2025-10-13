#pragma once

#ifndef INC_EPOLLER_HPP
#define INC_EPOLLER_HPP

#include <iostream>
#include <memory>
#include <vector>
#include <unordered_map>
#include <sys/epoll.h>
#include <cstdint>
#include "Handler.hpp"

template<std::uint32_t BUFFER_SIZE>
class Handler;

template<std::uint32_t POLLER_SIZE, std::uint32_t BUFFER_SIZE>
class Epoller {
public:
    Epoller(): epoll_fd_(::epoll_create1(EPOLL_CLOEXEC)), events_(POLLER_SIZE) {
        if (epoll_fd_ == -1) {
            std::cerr << "Failed to create epoll file descriptor" << std::endl;
            throw std::runtime_error("epoll_create1 failed");
        }
    }
    ~Epoller()
    {
        if (epoll_fd_ != -1) {
            ::close(epoll_fd_);
        }
    }
    void update(const std::shared_ptr<Handler<BUFFER_SIZE>>& ptr_handler)
    {
        epoll_event event;
        if(ptr_handler->get_handler_event() != EventType::Default)
        {
            event.events = (ptr_handler->get_handler_event() == EventType::Read) ? EPOLLIN | EPOLLET : EPOLLOUT;
        }

        int fd = ptr_handler->getHandlerFd();
        event.data.fd = fd;
        handler_map_[fd] = ptr_handler;

        int ret;
        if(ptr_handler->is_in_Epoll())
        {
            ret = epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, ptr_handler->getHandlerFd(), &event);
        } else {
            ret = epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, ptr_handler->getHandlerFd(), &event);
            ptr_handler->setInEventLoop(true);
        }

        if(ret < 0)
        {
            perror("epoll_ctl ADD failed!");
        } else {
            std::cout << "epoll event added! " << ptr_handler->getHandlerFd()  << " " << int(ptr_handler->get_handler_event()) << std::endl;
        }
    }

    void remove(const std::shared_ptr<Handler<BUFFER_SIZE>>& ptr_handler)
    {
        if(epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, ptr_handler->getHandlerFd(), nullptr) < 0)
        {   
            perror("epoll_ctl DEL failed!");
        } else {
            ptr_handler->set_out_Epoll();
            std::cout << "epoll event deleted!" << std::endl;
        }
    }

    std::vector<std::shared_ptr<Handler<BUFFER_SIZE>>> poll()
    {
        int event_count = ::epoll_wait(epoll_fd_, events_.data(), static_cast<int>(events_.size()), -1);
        std::vector<std::shared_ptr<Handler<BUFFER_SIZE>>> active_handlers;

        if (event_count < 0) {
            if (errno != EINTR) {
                std::cerr << "epoll_wait error" << std::endl;
            }
            return active_handlers;
        }

        for (int i = 0; i < event_count; ++i) {
            int fd = events_[i].data.fd;
            if(auto sp = handler_map_[fd].lock()) {
                active_handlers.emplace_back(sp);
            }
        }
        return active_handlers;
    }
private:
    int epoll_fd_;
    std::vector<::epoll_event> events_;
    std::unordered_map<int, std::weak_ptr<Handler<BUFFER_SIZE>>> handler_map_;
};

#endif // INC_EPOLLER_HPP