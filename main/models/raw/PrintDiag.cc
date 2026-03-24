#include "PrintDiag.hh"

#include <bitset>

#include "frame.hh"
#include "i18n.hh"

using OurFrame = RawFrame;

static int tohex(const uint8_t* in, size_t insz, char* out, size_t outsz, const char *sep = "") {
  const auto sep_len = strlen(sep);
  const unsigned char* pin = in;
  const char* hex = "0123456789ABCDEF";
  const int req_size = insz * (2 + sep_len) - sep_len; 

  char* pout = out;
  for (; pin < in + insz; pout += 2 + sep_len, pin++) {
    pout[0] = hex[(*pin >> 4) & 0xF];
    pout[1] = hex[*pin & 0xF];
    memcpy (&pout[2], sep, sep_len);
    if (pout + 2 + sep_len - out > outsz) {
      /* Better to truncate output string than overflow buffer */
      /* it would be still better to either return a status */
      /* or ensure the target buffer is large enough and it never happen */
      break;
    }
  }
  pout[-sep_len] = '\0';
  return req_size;
}

static char frame_hex[OurFrame::FRAME_SIZE * 3];
static void frame_hex_fill(const OurFrame &frame, const char *sep = ",") {
  tohex(&frame.get_frame()[0], frame.get_frame_length(), frame_hex, sizeof frame_hex, sep);
}


/// @brief Conditional print members of X53b740Frame object
/// @param dst       write buffer
/// @param dst_siz   write buffer size
/// @param d         object
/// @param view_mask bitmask representing members. Only members with 1 bits are
/// printed.
/// @return  bytes written or if greater than dst_siz, the required buffer size
/// (man 3 snprintf)
static int frame_print(char* dst, size_t dst_siz, const OurFrame& d,
                       const PrintCarDiag::line_view_mask_t& view_mask) {
  ssize_t dst_size = ssize_t(dst_siz);
  int ct = 0;

  if (view_mask.any()) {
    unsigned bit = 0;
    auto dst_max = dst + dst_size - 1;

    ///////////////////////////////////////////////
    frame_hex_fill(d, "");
    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct +=
          snprintf(p, l, "%02u: %s\n", bit,  frame_hex);
    }
    ///////////////////////////////////////////////

    if (view_mask.test(bit++) && ct >= 0) {
      auto p = std::min(dst_max, dst + ct);
      auto l = std::max(ssize_t(0), dst_size - ct);
      ct += snprintf(p, l, "%02u: %6d Frame-Count\n", bit,
                     d.get_frame_count());
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
                                   line_view_mask_t show_lines) const {
  return frame_print(dst, dst_size, m_frame, show_lines);
}