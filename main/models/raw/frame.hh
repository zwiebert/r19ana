#pragma once

#include <array>
#include <string>

#include "XR25Frame.hh"
#include "models/settings.hh"

/// @brief
class RawFrame : CarModelBase {
 public:
  static constexpr int FRAME_SIZE = XR25Frame::FRAME_MAX_SIZE;
  using frame_data_t = std::array<uint8_t, FRAME_SIZE>;

 public:
  int get_frame_count() const { return FrameNumber; }
  const frame_data_t& get_frame() const { return data; }
  unsigned get_frame_length() const { return m_frame_len; }

 private:
  frame_data_t data = {};  ///<  XR25 frame without header ff,00
  int FrameNumber = 0;
  unsigned m_frame_len = 0;

 public:
  RawFrame(const XR25Frame::voc_t& frame)
      : FrameNumber(frame.counter), m_frame_len(frame.frame_len) {
    for (int i = 0; i < m_frame_len; ++i) {
      data[i] = frame.frame[i];
    }
  };
  RawFrame() = default;

  uint8_t operator[](int idx) const { return data[idx - 2]; }

  std::string getDataAsText() const;
};