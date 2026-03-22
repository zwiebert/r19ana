#pragma once

#include "PrintCarDiag.hh"
#include "X53B_740_frame_utils.hh"

/**
 * @brief Implementation for Model X53B-740 (54kw TBI with manual transmission)
 *
 */
class PrintDiagX53b740 : public PrintCarDiag {
  using line_view_mask_t = std::bitset<64U>;

 public:
  PrintDiagX53b740(const X53b740Frame& frame) : m_frame(frame) {}

  int snprint_diag(char* dst, size_t dst_size,
                   line_view_mask_t show_lines) const override {
    return r19_frame_print(dst, dst_size, m_frame, show_lines);
  }

 private:
  const X53b740Frame& m_frame;
};