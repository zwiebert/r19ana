// Transport.hh
#pragma once

#include <functional>
#include <vector>
#include <cstdint>

class Transport {
public:
    using ReadCallback = std::function<void(const std::vector<uint8_t>&)>;

    virtual ~Transport() = default;
    virtual void start(ReadCallback cb) = 0;
    virtual void stop() = 0;
};