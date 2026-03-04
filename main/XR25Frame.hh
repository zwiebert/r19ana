#ifndef XR25FRAME_H
#define XR25FRAME_H

#include <array>
#include <vector>
#include <string>
#include <cstdint>

class XR25Frame {
public:
    static constexpr int BUF_SIZE   = 500;
    static constexpr int FRAME_SIZE = 32;
    static const std::array<uint8_t,4> FRAME_START_ID;

    XR25Frame();

    void dampbufFill();
    std::string dampbufAsString() const;

    int distance() const;
    int find(const std::vector<uint8_t>& data) const;
    bool adjustHead();

    uint8_t getByteByIndex(int idx) const;
    int     getIntByIndex(int idx) const;
    int     getDampedIntByIndex(int idx) const;

    void add(uint8_t b);
    bool append(const std::vector<uint8_t>& data);
    bool append(const std::vector<uint8_t>& data, int nmb);

    static std::vector<uint8_t> hexStringToByteArray(const std::string& s);
    std::string toString() const;

    int getFrameCounter() const { return frameCounter; }

    bool test();

private:
    std::array<uint8_t, BUF_SIZE> buffer;
    std::array<uint8_t, FRAME_SIZE> dampbuf;
    int tail = 0;
    int head = BUF_SIZE - 2;
    int frameCounter = 0;
};

#endif // XR25FRAME_H