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