#include "XR25Frame.hh"

#include <cctype>
#include <iomanip>
#include <sstream>

bool XR25Frame::add(uint8_t b) {
  if (m_frames_rb.getFree() == 0) return false;

  if (b == 0xff && !m_last_byte_was_ff) {
    m_last_byte_was_ff = true;
    return true;
  }

  if (b == 0x00 && m_last_byte_was_ff) {

   if (!m_header_found) { // this is the first frame.
      rbuf_clear();
      m_header_found = true;
      return true;
   }

    auto frame_length = std::distance(rbuf_begin(), m_rbuf_ptr);

    if (frame_length < FRAME_MIN_SIZE) { // discard too short frames
      if (frame_length > 0) ++m_invalid_frame_ct;
      rbuf_clear();
      return false;
    }

    // we have a valid frame, and a new frame header
    m_complete_frame_length = frame_length;
    m_header_found = true;
    rbuf_push();
    return true;
  }

  // now all special bytes are handled.
  m_last_byte_was_ff = false;


  if (m_rbuf_ptr >= rbuf_end()) { // drop all data, frame is too long
    rbuf_clear();
    return false;
  }

  // store data byte
  *m_rbuf_ptr++ = b;
  return true;
}

unsigned XR25Frame::append(const std::uint8_t* data, size_t data_len) {
  for (unsigned i = 0; i < data_len; ++i)
    if (!add(data[i])) return i;

  return data_len;
}
