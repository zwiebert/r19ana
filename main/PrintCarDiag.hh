#pragma once

#include <bitset>

#include "XR25Frame.hh"

/**
 * @brief Interface class. Should be implemented with model specific classes.
 *
 */
class PrintCarDiag {
 public:
  using line_view_mask_t = std::bitset<64U>;

 public:
  /**
   * @brief Print formatted data into buffer
   *
   * @param dst  output buffer. If dst_size is zero, than pass at least a
   * pointer to a single byte.
   * @param dst_size size of output buffer for output and terminating null. If
   * null the function returns the buffers size it needs.
   * @param show_lines a bitset where each set-bit is a line which should be
   * printed
   * @return  like snprintf() its returns the bytes it needs for output exluding
   * the terminating null.
   */
  virtual int snprint_diag(char* dst, size_t dst_size,
                           line_view_mask_t show_lines) const = 0;
  virtual void push_frame(const XR25Frame::frame_data_t& xr25_frame,
                          int frame_count) = 0;
};