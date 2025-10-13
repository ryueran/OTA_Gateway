#pragma once

#ifndef INC_WRITEHANDLER_HPP
#define INC_WRITEHANDLER_HPP

#include "Handler.hpp"

template<std::uint32_t BUFFER_SIZE>
class WriteHandler : public Handler<BUFFER_SIZE> {
public:
    explicit WriteHandler(int handler_fd)
        : Handler<BUFFER_SIZE>(handler_fd) {
        this->setEventType(EventType::Write);
        std::cout << "WriteHandler created with fd: " << handler_fd << std::endl;
    }

    ~WriteHandler()
    {
        std::cout << "WriteHandler destroyed!" << std::endl;
    }
};

#endif // INC_WRITEHANDLER_HPP