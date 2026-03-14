#ifndef XR25FRAME_H
#define XR25FRAME_H

#include <array>
#include <vector>
#include <string>
#include <cstdint>

class XR25Frame {
public:
    static constexpr int FRAME_SIZE = 30;

    XR25Frame();

    bool add(uint8_t b);
    bool append(const std::vector<uint8_t>& data);
    bool append(const std::vector<uint8_t>& data, int nmb);
    bool append(const std::uint8_t *data, size_t data_len);

    static std::vector<uint8_t> hexStringToByteArray(const std::string& s);
    std::string toString() const;

    int getFrameCounter() const { return frameCounter; }

    bool test();

    uint8_t operator[] (int idx) const {
        return m_frame[idx];
    }

private:
    int frameCounter = 0;
private:
    std::array<uint8_t, FRAME_SIZE> m_frame = {};
    int m_idx = 0;
    bool m_last_byte_was_ff = false;
};

#endif // XR25FRAME_H