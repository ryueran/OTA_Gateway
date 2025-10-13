#pragma once

#ifndef INC_READHANDLER_CPP
#define INC_READHANDLER_CPP

#include "Handler.hpp"
#include "Constant.hpp"

template<std::uint32_t BUFFER_SIZE>
class ReadHandler : public Handler<BUFFER_SIZE> {
public:
    explicit ReadHandler(int handler_fd)
        : Handler<BUFFER_SIZE>(handler_fd) {
        this->setEventType(EventType::Read);
        std::cout << "ReadHandler created with fd: " << handler_fd << std::endl;
    }

    
};

#endif // INC_READHANDLER_CPP