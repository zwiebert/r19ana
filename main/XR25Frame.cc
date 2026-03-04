#include "XR25Frame.hh"
#include <sstream>
#include <iomanip>
#include <cctype>

const std::array<uint8_t,4> XR25Frame::FRAME_START_ID = {0xff, 0x00, 0x10, 0x77};

XR25Frame::XR25Frame()
{
    buffer.fill(0);
    dampbuf.fill(0);
}

void XR25Frame::dampbufFill()
{
    for (int i = 0; i < FRAME_SIZE; ++i) {
        int b = getIntByIndex(i);
        int d = dampbuf[i] & 0xff;
        if (b == d) continue;
        d = (b > d) ? d + 1 : d - 1;
        if (d >= 0x00 && d <= 0xff)
            dampbuf[i] = static_cast<uint8_t>(d);
    }
}

std::string XR25Frame::dampbufAsString() const
{
    std::ostringstream sb;
    for (int i = 0; i < FRAME_SIZE; ++i) {
        uint8_t c = dampbuf[i];
        sb << std::hex << std::nouppercase << std::setw(1) << (int)(c >> 4)
           << std::hex << std::nouppercase << std::setw(1) << (int)(c & 0xF)
           << " ";
    }
    return sb.str();
}

int XR25Frame::distance() const
{
    return (tail < head)
        ? (tail + BUF_SIZE) - head
        : tail - head;
}

int XR25Frame::find(const std::vector<uint8_t>& data) const
{
    for (int i = 0, n = (distance() - (int)data.size()); n > 0; --n, ++i) {
        bool match = true;
        for (size_t k = 0; match && k < data.size(); ++k) {
            if (getByteByIndex(k + i) != data[k])
                match = false;
        }
        if (match)
            return i;
    }
    return -1;
}

bool XR25Frame::adjustHead()
{
    int last_good_head = -1;
    int frames_found = 0;

    while (distance() > FRAME_SIZE) {
        int pos = find(std::vector<uint8_t>(FRAME_START_ID.begin(),
                                            FRAME_START_ID.end()));
        if ((distance() - pos) < FRAME_SIZE) {
            break;
        }
        if (pos == 0) {
            last_good_head = head;
            head = (head + 1 + BUF_SIZE) % BUF_SIZE;
            continue;
        } else if (pos < 0) {
            head = (last_good_head < 0)
                   ? ((tail + BUF_SIZE - 4) % BUF_SIZE)
                   : last_good_head;
            break;
        } else {
            head = last_good_head = (head + pos) % BUF_SIZE;
            ++frames_found;
            dampbufFill();
        }
    }
    if (last_good_head >= 0)
        head = last_good_head;

    frameCounter += frames_found;
    return frames_found > 0;
}

uint8_t XR25Frame::getByteByIndex(int idx) const
{
    idx = (head + idx + BUF_SIZE) % BUF_SIZE;
    return buffer[idx];
}

int XR25Frame::getIntByIndex(int idx) const
{
    return 0xff & getByteByIndex(idx);
}

int XR25Frame::getDampedIntByIndex(int idx) const
{
    return 0xff & dampbuf[idx];
}

void XR25Frame::add(uint8_t b)
{
    buffer[tail] = b;
    tail = (tail + 1) % BUF_SIZE;
}

bool XR25Frame::append(const std::vector<uint8_t>& data)
{
    for (auto b : data) add(b);
    return adjustHead();
}

bool XR25Frame::append(const std::vector<uint8_t>& data, int nmb)
{
    for (int i = 0; i < nmb; ++i) add(data[i]);
    return adjustHead();
}

std::vector<uint8_t> XR25Frame::hexStringToByteArray(const std::string& s)
{
    std::string str;
    for (char c : s) if (!std::isspace((unsigned char)c)) str.push_back(c);

    std::vector<uint8_t> data;
    data.reserve(str.size()/2);
    for (size_t i = 0; i < str.size(); i += 2) {
        uint8_t hi = std::isdigit(str[i]) ? str[i]-'0' : std::toupper(str[i])-'A'+10;
        uint8_t lo = std::isdigit(str[i+1]) ? str[i+1]-'0' : std::toupper(str[i+1])-'A'+10;
        data.push_back((hi << 4) | lo);
    }
    return data;
}

std::string XR25Frame::toString() const
{
    std::ostringstream sb;
    for (int i = 0; i < FRAME_SIZE; ++i) {
        uint8_t c = getByteByIndex(i);
        sb << std::hex << std::nouppercase << (int)((c >> 4) & 0xF)
           << std::hex << std::nouppercase << (int)(c & 0xF) << " ";
    }
    return sb.str();
}

bool XR25Frame::test()
{
    std::vector<uint8_t> test_data;
    bool result;

    test_data = hexStringToByteArray("ff00107710447d79bf1aa45fc608080c");
    result = append(test_data);
    test_data = hexStringToByteArray("0073ffff0100006b3004048079811288");
    result = append(test_data);
    test_data = hexStringToByteArray("ff00107710437d79ba19705fc608040c");
    result = append(test_data);

    test_data = hexStringToByteArray("0073ffff0100006b3004048079811288");
    result = append(test_data);
    test_data = hexStringToByteArray("ff00107710437d79b8198c5fc408050c");
    result = append(test_data);

    return result;
}