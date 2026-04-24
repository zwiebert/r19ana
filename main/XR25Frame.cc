#include "XR25Frame.hh"

#include <cctype>
#include <iomanip>
#include <sstream>

unsigned XR25Frame::append(const std::uint8_t* data, size_t data_len) {
  if (m_frames_rb.getFree() == 0) return 0;
  const unsigned chunk_size = FRAME_MIN_SIZE;
  for (unsigned i = 0; i < data_len; i += chunk_size) {
    if (m_frames_rb.getFree() == 0) return i;
    unsigned len = i + chunk_size >= data_len ? data_len - i : chunk_size;
    m_destuffer.processChunk(data + i, len);
  }
  return data_len;
}

bool XR25Frame::add(uint8_t b) { return append(&b, 1) == 1; }
