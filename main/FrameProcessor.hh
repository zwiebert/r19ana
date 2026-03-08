// FrameProcessor.hh
#pragma once

#include "XR25Frame.hh"
#include "R19Frame.hh"
#include <functional>
#include <string>
#include <vector>

class FrameProcessor {
public:
    using UpdateCallback = std::function<void(const std::string& hex, R19Frame &&r19_frame)>;

    explicit FrameProcessor(UpdateCallback cb);
    void feedBytes(const std::vector<uint8_t>& data);
    void feedBytes(const uint8_t * data, std::size_t data_len);

private:
    XR25Frame xr25;
    UpdateCallback callback;
};