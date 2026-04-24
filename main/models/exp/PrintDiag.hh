#pragma once

#include "PrintCarDiag.hh"
#include "packet_parser.hh"

/**
 * @brief Implementation for Model X53B-740 (54kw TBI with manual transmission)
 *
 */
class PrintDiagExp : public PrintCarDiag {
 public:
  int snprint_diag(char* dst, size_t dst_size,
                   const line_view_mask_t &show_lines) const override;

  void push_packet(const XR25Frame::voc_t& xr25_frame) override {
    m_frame = {xr25_frame};
  }

 private:
  ExpFrame m_frame;
};
