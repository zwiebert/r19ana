#pragma once

#include <array>
#include <string>

#include "XR25Frame.hh"
#include "models/settings.hh"

/// @brief
class RawPacket : public CarModelBase {
 public:
  static constexpr int PACKET_SIZE = XR25Frame::PACKET_MAX_SIZE;
  using packet_data_t = std::array<uint8_t, PACKET_SIZE>;

 public:
  int get_frame_count() const { return FrameNumber; }
  const packet_data_t& get_frame() const { return m_packet; }
  unsigned get_frame_length() const { return m_packet_len; }

 private:
  packet_data_t m_packet = {};  ///<  XR25 destuffed packet
  int FrameNumber = 0;
  unsigned m_packet_len = 0;

 public:
  RawPacket(const XR25Frame::voc_t& voc)
      : FrameNumber(voc.counter), m_packet_len(voc.packet_len) {
    for (int i = 0; i < m_packet_len; ++i) {
      m_packet[i] = voc.packet[i];
    }
  };
  RawPacket() = default;

  uint8_t operator[](int idx) const { return m_packet[idx - 2]; }
};