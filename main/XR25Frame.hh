#ifndef XR25FRAME_H
#define XR25FRAME_H

#include <array>
#include <cstdint>
#include <string>
#include <vector>

#include "Ringbuffer.hh"
#include "renix_destuffer.hh"

class XR25Frame {
 public:
  static constexpr int PACKET_MAX_SIZE = 64;  // cannot store larger frames
  static constexpr int PACKET_MIN_SIZE = 29;  // reject smaller frames
  static constexpr int RINGBUFFER_LENGTH =
      32;  // can buffer this number of frames
  using packet_data_t = std::array<uint8_t, PACKET_MAX_SIZE>;

  struct voc_t {
    packet_data_t packet;
    unsigned packet_len;
    unsigned counter;
  };

  bool pull_voc(voc_t& dst) { return 1 == m_frames_rb.read(dst); }

  XR25Frame() : m_frames_rb(RINGBUFFER_LENGTH) {}

  bool add(uint8_t b);
  unsigned append(const std::uint8_t* data, size_t data_len);
  unsigned get_buffered_frame_count() const {
    return m_frames_rb.getOccupied();
  }
  std::string toString() const;

  int getFrameCounter() const { return m_frame_counter; }

  bool test();
  int get_frame_counter() const { return m_frame_counter - 1; }

 private:
  void destuffer_callback(const uint8_t* packet, size_t packet_len) {
    if (packet_len < PACKET_MIN_SIZE || PACKET_MAX_SIZE < packet_len) return;
    voc_t voc;
    memcpy(&voc.packet[0], packet, packet_len);
    voc.packet_len = packet_len;
    voc.counter = m_frame_counter;
    m_frames_rb.write(voc);
    ++m_frame_counter;
  }

 private:
  unsigned m_frame_counter = 0;
  RenixDestuffer m_destuffer =
      RenixDestuffer([this](const uint8_t* packet, size_t packet_len) {
        destuffer_callback(packet, packet_len);
      });

  ringbuffer<voc_t> m_frames_rb;
  packet_data_t m_frame;
  unsigned m_invalid_frame_ct = 0;
};

#endif  // XR25FRAME_H