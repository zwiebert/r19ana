#include "PrintDiag.hh"

#include <sys/time.h>

#include <bitset>

#include "packet_parser.hh"
#include "i18n.hh"
#include "main.hh"

using OurPacket = RawPacket;

#define diag_printf(fmt, ...)                              \
  do {                                                     \
    if (view_mask.test(bit++) && ct >= 0) {                \
      auto p = std::min(dst_max, dst + ct);                \
      auto l = std::max(ssize_t(0), dst_size - ct);        \
      ct += snprintf(p, l, "%02u " fmt, bit, __VA_ARGS__); \
    }                                                      \
  } while (0)

static char packet_as_hex[OurPacket::PACKET_SIZE * 3];
static void frame_hex_fill(const OurPacket& packet, const char* sep = ",") {
  bin2hex(&packet.get_frame()[0], packet.get_frame_length(), packet_as_hex,
          sizeof packet_as_hex, sep);
}

/// @brief Conditional print members of X53b740Frame object
/// @param dst       write buffer
/// @param dst_siz   write buffer size
/// @param d         object
/// @param view_mask bitmask representing members. Only members with 1 bits are
/// printed.
/// @return  bytes written or if greater than dst_siz, the required buffer size
/// (man 3 snprintf)
static int frame_print(char* dst, size_t dst_siz, const OurPacket& d,
                       const PrintCarDiag::line_view_mask_t& view_mask) {
  ssize_t dst_size = ssize_t(dst_siz);
  int ct = 0;

  if (view_mask.any()) {
    unsigned bit = 0;
    auto dst_max = dst + dst_size - 1;

    ///////////////////////////////////////////////
    frame_hex_fill(d, "");
    diag_printf("%s\n", packet_as_hex);
    ///////////////////////////////////////////////

    diag_printf("%6d %s\n", d.get_frame_count(), _("Packet-Count"));
    diag_printf("%6u bytes %s\n", d.get_frame_length(), _("Packet-Length"));

    {
      static struct timeval tv0;
      struct timeval tv1;
      if (0 == gettimeofday(&tv1, 0)) {
        int diff_sec = tv1.tv_sec - tv0.tv_sec;
        int diff_usec = tv1.tv_usec - tv0.tv_usec;
        while (diff_usec < 0) {
          diff_usec += 1000;
        }

        diag_printf("%6.01f ms %s\n", diff_usec / 1000.0f, _("Frame-Period"));
        tv0 = tv1;
      }
    }

    if (ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "\n");
    }

    if (ct >= dst_size || ct < 0)
      return ct;  // ERROR: data was not fully written (e.g. buffer too small)
  }

  return ct;
}

int PrintDiagRaw::snprint_diag(char* dst, size_t dst_size,
                               const line_view_mask_t& show_lines) const {
  return frame_print(dst, dst_size, m_frame, show_lines);
}