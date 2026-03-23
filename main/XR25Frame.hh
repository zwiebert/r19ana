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

  XR25Frame();

  bool add(uint8_t b);
  bool append(const std::vector<uint8_t>& data);
  bool append(const std::vector<uint8_t>& data, int nmb);
  bool append(const std::uint8_t* data, size_t data_len);

  static std::vector<uint8_t> hexStringToByteArray(const std::string& s);
  std::string toString() const;

  int getFrameCounter() const { return frameCounter; }

  bool test();

  uint8_t operator[](int idx) const { return (get_frame_data())[idx - 2]; }

  const frame_data_t& get_frame_data() const { return m_frames[(frameCounter & 1) ? 0 : 1]; }
  int get_frame_counter() const { return m_complete_frame_counter; }
  int get_frame_length() const { return m_complete_frame_length; }

 private:
  frame_data_t& get_frame_tmp() { return m_frames[(frameCounter & 1) ? 1 : 0]; }
  frame_data_t& get_frame_cmpl() {
    return m_frames[(frameCounter & 1) ? 0 : 1];
  }

 private:
  int m_complete_frame_counter = 0;
  int m_complete_frame_length = 0;  ///< actual data frame length
 private:
  int frameCounter = 0;
  frame_data_t m_frames[2] = {};
  int m_idx = 0;
  bool m_last_byte_was_ff = false;
};

#endif  // XR25FRAME_H