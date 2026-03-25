#include "XR25Frame.hh"

#include <cctype>
#include <iomanip>
#include <sstream>


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

