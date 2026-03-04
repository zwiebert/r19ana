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
    std::string text = R19Frame(xr25).getDataAsText();
    if (callback) callback(hex, text);
}