#include "XR25Frame.hh"

#include <cctype>
#include <iomanip>
#include <sstream>

XR25Frame::XR25Frame() {}

bool XR25Frame::add(uint8_t b) {
  if (b == 0xff && !m_last_byte_was_ff) {
    m_last_byte_was_ff = true;
    return true;
  }

  if (b == 0x00 && m_last_byte_was_ff) {
    m_complete_frame_length = std::distance(rbuf_begin(), m_rbuf_ptr);
    rbuf_push();
    return true;
  }

  m_last_byte_was_ff = false;

  if (m_rbuf_ptr >= rbuf_end()) {
    rbuf_clear();
    return false;
  }

  *m_rbuf_ptr++ = b;

  return true;
}

bool XR25Frame::append(const std::vector<uint8_t>& data) {
  for (auto b : data)
    if (!add(b)) return false;

  return true;
}

bool XR25Frame::append(const std::vector<uint8_t>& data, int nmb) {
  for (int i = 0; i < nmb; ++i)
    if (!add(data[i])) return false;

  return true;
}

bool XR25Frame::append(const std::uint8_t* data, size_t data_len) {
  for (unsigned i = 0; i < data_len; ++i)
    if (!add(data[i])) return false;

  return true;
}

std::vector<uint8_t> XR25Frame::hexStringToByteArray(const std::string& s) {
  std::string str;
  for (char c : s)
    if (!std::isspace((unsigned char)c)) str.push_back(c);

  std::vector<uint8_t> data;
  data.reserve(str.size() / 2);
  for (size_t i = 0; i < str.size(); i += 2) {
    uint8_t hi =
        std::isdigit(str[i]) ? str[i] - '0' : std::toupper(str[i]) - 'A' + 10;
    uint8_t lo = std::isdigit(str[i + 1]) ? str[i + 1] - '0'
                                          : std::toupper(str[i + 1]) - 'A' + 10;
    data.push_back((hi << 4) | lo);
  }
  return data;
}

std::string XR25Frame::toString() const {
  std::ostringstream sb;
  for (int i = 0; i < m_complete_frame_length; ++i) {
    auto c = (*this)[i];
    sb << std::hex << std::nouppercase << (int)((c >> 4) & 0xF) << std::hex
       << std::nouppercase << (int)(c & 0xF) << " ";
  }
  return sb.str();
}

bool XR25Frame::test() {
  std::vector<uint8_t> test_data;
  bool result;

  test_data = hexStringToByteArray("ff00107710447d79bf1aa45fc608080c");
  //                                00  02  04  06  08  10  12  14
  result = append(test_data);
  test_data = hexStringToByteArray("0073ffff0100006b3004048079811288");
  //                                16    18  20  22  24  26  28  30
  result = append(test_data);
  test_data = hexStringToByteArray("ff00107710437d79ba19705fc608040c");
  //                                00  02  04  06  08  10  12  14
  result = append(test_data);

  test_data = hexStringToByteArray("0073ffff0100006b3004048079811288");
  //                                16    18  20  22  24  26  28  30
  result = append(test_data);
  test_data = hexStringToByteArray("ff00107710437d79b8198c5fc408050c");
  //                                00  02  04  06  08  10  12  14
  result = append(test_data);

  return result;
}