#pragma once

#include "PrintCarDiag.hh"
#include "X53B_740_frame.hh"

/**
 * @brief Implementation for Model X53B-740 (54kw TBI with manual transmission)
 *
 */
class PrintDiagX53b740 : public PrintCarDiag {
  using line_view_mask_t = std::bitset<64U>;

 public:
  int snprint_diag(char* dst, size_t dst_size,
                   line_view_mask_t show_lines) const override;

  void push_frame(const XR25Frame::frame_data_t& xr25_frame,
                  int frame_count) override {
    m_frame = {xr25_frame, frame_count};
  }

 private:
  X53b740Frame m_frame;
};