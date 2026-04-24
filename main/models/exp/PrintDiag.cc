#include "PrintDiag.hh"

#include <bitset>

#include "packet_parser.hh"
#include "i18n.hh"
#include "main.hh"

using OurPacket = class ExpFrame;

#define diag_printf(fmt, ...)                              \
  do {                                                     \
    if (view_mask.test(bit++) && ct >= 0) {                \
      auto p = std::min(dst_max, dst + ct);                \
      auto l = std::max(ssize_t(0), dst_size - ct);        \
      ct += snprintf(p, l, "%02u " fmt, bit, __VA_ARGS__); \
    }                                                      \
  } while (0)

static char packet_as_hex[OurPacket::PACKET_SIZE * 3];
static void frame_hex_fill(const OurPacket::packet_data_t& packet,
                           const char* sep = ",") {
  bin2hex(&packet[0], packet.size(), packet_as_hex, sizeof packet_as_hex, sep);
}

inline const char* btoa(bool v) { return v ? "X" : " "; }

int PrintDiagExp::snprint_diag(char* dst, size_t dst_siz,
                               const line_view_mask_t& view_mask) const {
  const OurPacket& d = m_frame;
  using idx_t = OurPacket::idx_t;

  ssize_t dst_size = ssize_t(dst_siz);
  int ct = 0;

  if (view_mask.any()) {
    unsigned bit = 0;
    auto dst_max = dst + dst_size - 1;

    // line 1 always is the complete hex-string of the data-packet
    frame_hex_fill(d.get_frame(), "");
    diag_printf("%s\n", packet_as_hex);

    // print every byte, line numbers should match byte-indexes to avoid
    // confusion
    for (int i = 2; i < 30; ++i) diag_printf("%6d R%d\n", d.R(i), i);

    // print every bit from the status bits on a separate line
    for (uint8_t i = 0; i < 8; ++i)
      diag_printf("%6d bool flags=%u,bit=%u\n", getbit(d.R(idx_t::status0), i),
                  0, i);

    for (idx_t flags = idx_t::status1; flags <= idx_t::status7;
         flags = idx_t(flags + 1)) {
      uint8_t flag_mnb = 1 + flags - idx_t::status1;
      for (uint8_t i = 0; i < 8; ++i)
        diag_printf("%6d bool flags=%u,bit=%u\n", getbit(d.R(flags), i),
                    flag_mnb, i);
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
