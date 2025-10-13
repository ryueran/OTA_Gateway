#pragma once

#ifndef INC_REACTOR_HPP
#define INC_REACTOR_HPP

#include <iostream>
#include <memory>
#include <vector>
#include "Epoller.hpp"
#include "Handler.hpp"
#include "Constant.hpp"

class Reactor {
public:
    Reactor()
    {
        std::cout << "Reactor created." << std::endl;
    }

    ~Reactor()
    {
        std::cout << "Reactor destroyed." << std::endl;
    }

    void registerHandler(const std::shared_ptr<Handler<BUFFER_SIZE>>& handler)
    {
        epoller_.update(handler);
    }

    void unregisterHandler(const std::shared_ptr<Handler<BUFFER_SIZE>>& handler)
    {
        epoller_.remove(handler);
    }

    void eventLoop()
    {
        while (true) {
            auto active_handlers = epoller_.poll();
            for (auto& handler : active_handlers) {
                handler->handleEvent();
            }
        }
    }

private:
    Epoller<POLLER_SIZE, BUFFER_SIZE> epoller_;
};

#endif // INC_REACTOR_HPP