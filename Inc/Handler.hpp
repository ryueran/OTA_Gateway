#pragma once

#ifndef INC_HANDLER_HPP
#define INC_HANDLER_HPP

#include <iostream>
#include <functional>
#include <cstdint>

enum class EventType {
    Default,
    Read,
    Write
};

using callBack = std::function<void(int)>;

template<std::uint32_t BUFFER_SIZE>
class Handler{
private:
    int handler_fd_;
    bool isInEventLoop_;
    callBack callBack_;
    EventType event_type_ = EventType::Default;

protected:
    void setEventType(EventType type) { event_type_ = type; }
public:
    explicit Handler(int handler_fd)
        : handler_fd_(handler_fd), isInEventLoop_(false) {
            std::cout << "Handler created with fd: " << handler_fd_ << std::endl;
        }

    virtual ~Handler()
    {
        if (handler_fd_ != -1) {
            ::close(handler_fd_);
        }
        std::cout << "Handler with fd " << handler_fd_ << " destroyed." << std::endl;
    }
    virtual void handleEvent() // there might be handler that doing sth else in the future, and delegate may alos be imported later
    {
        if (callBack_) {
            callBack_(handler_fd_);
        }   
    }

    EventType get_handler_event() const { return event_type_; }
    bool is_in_Epoll() const { return isInEventLoop_; } 
    void set_out_Epoll() { isInEventLoop_ = false; }

    void setCallback(callBack cb) { callBack_ = cb; }
    int getHandlerFd() const { return handler_fd_; }
    void setInEventLoop(bool inLoop) { isInEventLoop_ = inLoop; }
    char message_buffer[BUFFER_SIZE];
};

#endif // INC_HANDLER_HPP