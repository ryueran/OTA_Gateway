#include <vector>
#include <time.h>
#include <chrono>
#include <string>
#include <cstdint>
#include <iostream>
#include <memory>
#include "Handler.hpp"
#include "Constant.hpp"

class Session
{
public:
    explicit Session(uint8_t fd, std::shared_ptr<Handler<BUFFER_SIZE>> handler)
    : fd_(fd), handler_(std::move(handler)), last_activate_time_(std::time(nullptr)) 
    {

    }

    void refresh()
    {
        last_activate_time_ = std::time(nullptr);
    }

    int fd_;
    std::shared_ptr<Handler<BUFFER_SIZE>> handler_;
    time_t last_activate_time_;
};