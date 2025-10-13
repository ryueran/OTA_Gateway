#pragma once

#ifndef INC_GENERALHANDLER_HPP
#define INC_GENERALHANDLER_HPP

#include "Handler.hpp"
#include "Constant.hpp"
#include "Delegate.hpp"
#include <memory>

template<std::uint32_t BUFFER_SIZE, typename R, typename... Args>
class GeneralHandler: public Handler<BUFFER_SIZE>
{
public:
    explicit GeneralHandler(int handler_fd, R f(Args...)): Handler<BUFFER_SIZE>(handler_fd)
    {
        this->setEventType(EventType::Default);
        delegate_ = std::make_unique<Delegate<R, Args...>>(f);
    }
private:
    std::unique_ptr<Delegate<R, Args...>> delegate_;
};

#endif // INC_GENERALHANDLER_HPP