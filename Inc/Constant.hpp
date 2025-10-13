#pragma once

#ifndef INC_CONSTANT_HPP
#define INC_CONSTANT_HPP

#include <cstdint>

constexpr std::uint32_t PORT = 8443;
constexpr std::uint32_t MAX_EVENTS = 20;
constexpr std::uint32_t BUFFER_SIZE = 1024;
constexpr std::uint32_t POLLER_SIZE = 32;

#endif // INC_CONSTANT_HPP