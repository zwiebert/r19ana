#pragma once

#include "XR25Frame.hh"

inline int hex_nibble(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return -1;
}
inline bool hex2voc(XR25Frame::voc_t& voc, const char* src) {
  voc.packet_len = 0;
  if (!src) return false;

  unsigned dst_idx = 0;
  int high_nibble = -1;

  for (const char* p = src; *p != '\0'; ++p) {
    int val = hex_nibble(*p);
    if (val < 0) {
      // invalid char, abort and reset
      voc.packet_len = 0;
      return false;
    }
    if (high_nibble < 0) {
      high_nibble = val;
      continue;
    }
    // complete byte
    if (dst_idx >= XR25Frame::PACKET_MAX_SIZE) {
      break;
    }
    voc.packet[dst_idx++] = static_cast<uint8_t>((high_nibble << 4) | val);
    high_nibble = -1;
  }

  // ignore final nibble if odd length
  voc.packet_len = dst_idx >= XR25Frame::PACKET_MIN_SIZE ? dst_idx : 0;
  return voc.packet_len > 0;
}