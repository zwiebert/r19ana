// FrameProcessor.cc
#include "FrameProcessor.hh"

FrameProcessor::FrameProcessor(UpdateCallback cb)
    : callback(std::move(cb))
{
}

void FrameProcessor::feedBytes(const std::vector<uint8_t>& data)
{
    if (xr25.append(data)) {
        // optionally notify about new frame
    }
    std::string hex  = xr25.toString();
    if (callback) callback(hex, R19Frame(xr25));
}

void FrameProcessor::feedBytes(const uint8_t * data, std::size_t data_len)
{
    if (xr25.append(data, data_len)) {
        // optionally notify about new frame
    }
    std::string hex  = xr25.toString();
    if (callback) callback(hex, R19Frame(xr25));
}
