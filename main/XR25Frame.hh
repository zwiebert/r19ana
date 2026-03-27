#ifndef XR25FRAME_H
#define XR25FRAME_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include "Ringbuffer.hh"

class XR25Frame {
 public:
  static constexpr int FRAME_MAX_SIZE = 64;
  static constexpr int RINGBUFFER_LENGTH = 32;
  using frame_data_t = std::array<uint8_t, FRAME_MAX_SIZE>;

  struct voc_t {
    frame_data_t frame;
    unsigned frame_len;
    unsigned counter; 
  };

  bool pull_voc(voc_t &dst) {
    return 1 == m_frames_rb.read(dst);
  }

  XR25Frame() : m_frames_rb(RINGBUFFER_LENGTH) {}

  bool add(uint8_t b);
  unsigned append(const std::uint8_t* data, size_t data_len);
  unsigned get_buffered_frame_count() const { return m_frames_rb.getOccupied(); }
  std::string toString() const;

  int getFrameCounter() const { return m_frame_counter; }

  bool test();


  int get_frame_counter() const { return m_frame_counter - 1; }
  size_t get_frame_length() const { return m_complete_frame_length; }

 private:
  int get_idx_tmp() { return (m_frame_counter & 1) ? 1 : 0;}
uint8_t*rbuf_begin() { return &m_frame[0];} 
uint8_t*rbuf_end() { return &m_frame[FRAME_MAX_SIZE];} 
void rbuf_clear() {
  m_rbuf_ptr = rbuf_begin();
  m_last_byte_was_ff = false;
}
void rbuf_push() {
    voc_t voc = {.frame =  m_frame, .frame_len = (unsigned)std::distance(rbuf_begin(), m_rbuf_ptr), .counter = m_frame_counter};
    m_frames_rb.write(voc);
    ++m_frame_counter;
    rbuf_clear();
}

 private:
  size_t m_complete_frame_length = 0;  ///< actual data frame length
 private:
  unsigned m_frame_counter = 0;
  ringbuffer<voc_t> m_frames_rb;
  
  frame_data_t m_frame;
  uint8_t *m_rbuf_ptr = &m_frame[0];
  bool m_last_byte_was_ff = false;
};

#endif  // XR25FRAME_H