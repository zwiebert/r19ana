#ifndef XR25FRAME_H
#define XR25FRAME_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

class XR25Frame {
 public:
  static constexpr int FRAME_MAX_SIZE = 64;
  using frame_data_t = std::array<uint8_t, FRAME_MAX_SIZE>;

  struct voc_t {
    frame_data_t frame;
    size_t frame_len;
    unsigned counter; 
  };

  void get_voc(voc_t &dst) {
    dst.frame =  get_frame_data();
    dst.frame_len = get_frame_length();
    dst.counter = get_frame_counter();
  }

  XR25Frame();

  bool add(uint8_t b);
  bool append(const std::vector<uint8_t>& data);
  bool append(const std::vector<uint8_t>& data, int nmb);
  bool append(const std::uint8_t* data, size_t data_len);

  static std::vector<uint8_t> hexStringToByteArray(const std::string& s);
  std::string toString() const;

  int getFrameCounter() const { return m_frame_counter; }

  bool test();

  uint8_t operator[](int idx) const { return (get_frame_data())[idx - 2]; }

  const frame_data_t& get_frame_data() const { return m_frames[(m_frame_counter & 1) ? 0 : 1]; }
  int get_frame_counter() const { return m_frame_counter - 1; }
  size_t get_frame_length() const { return m_complete_frame_length; }

 private:
  int get_idx_tmp() { return (m_frame_counter & 1) ? 1 : 0;}
uint8_t*rbuf_begin() { return &m_frames[get_idx_tmp()][0];} 
uint8_t*rbuf_end() { return &m_frames[get_idx_tmp()][FRAME_MAX_SIZE];} 
void rbuf_clear() {
  m_rbuf_ptr = rbuf_begin();
  m_last_byte_was_ff = false;
}
void rbuf_push() {
    ++m_frame_counter;
    rbuf_clear();
}
  frame_data_t& get_frame_tmp() { return m_frames[get_idx_tmp()]; }
  frame_data_t& get_frame_cmpl() {
    return m_frames[(m_frame_counter & 1) ? 0 : 1];
  }

 private:
  size_t m_complete_frame_length = 0;  ///< actual data frame length
 private:
  int m_frame_counter = 0;
  frame_data_t m_frames[2] = {};
  uint8_t *m_rbuf_ptr = &m_frames[0][0];
  bool m_last_byte_was_ff = false;
};

#endif  // XR25FRAME_H