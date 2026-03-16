// Transport.hh
#pragma once

#include <functional>
#include <vector>
#include <cstdint>

class Transport {
public:
    using ReadCallback = std::function<void(std::uint8_t *data, std::size_t data_len)>;

    virtual ~Transport() = default;
    virtual void start(ReadCallback cb = 0) = 0;
    virtual void stop() = 0;
    virtual int write(const uint8_t* data, size_t data_len, bool block = false) { return -1; };
    virtual int read(uint8_t* &dst, size_t &dst_len, bool block = false) { return -1; };
};