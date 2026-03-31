#pragma once

#include <array>
#include <string>

#include "XR25Frame.hh"
#include "models/settings.hh"
#include "models/x53b_740/frame.hh"

/// @brief
class ExpFrame : public X53b740Frame {


 public:
  uint8_t get_flags_0() const { return X(idx_t::flags0); }
  uint8_t get_flags_1() const { return X(idx_t::flags1); }
  uint8_t get_flags_2() const { return X(idx_t::flags2); }
  uint8_t get_flags_3() const { return X(idx_t::flags3); }
  uint8_t get_flags_4() const { return X(idx_t::flags4); }
  uint8_t get_flags_5() const { return X(idx_t::flags5); }
  uint8_t get_flags_6() const { return X(idx_t::flags6); }
  uint8_t get_flags_7() const { return X(idx_t::flags7); }

  bool get_flag_0(uint8_t bit) const { return X(idx_t::flags0) & (1 << bit); }
  bool get_flag_1(uint8_t bit) const { return X(idx_t::flags1) & (1 << bit); }
  bool get_flag_2(uint8_t bit) const { return X(idx_t::flags2) & (1 << bit); }
  bool get_flag_3(uint8_t bit) const { return X(idx_t::flags3) & (1 << bit); }
  bool get_flag_4(uint8_t bit) const { return X(idx_t::flags4) & (1 << bit); }
  bool get_flag_5(uint8_t bit) const { return X(idx_t::flags5) & (1 << bit); }
  bool get_flag_6(uint8_t bit) const { return X(idx_t::flags6) & (1 << bit); }
  bool get_flag_7(uint8_t bit) const { return X(idx_t::flags7) & (1 << bit); }
  ///////////////////////// Flags ////////////////////


 public:
  ExpFrame(const XR25Frame::voc_t& frame) : X53b740Frame(frame) {
  };
  ExpFrame() = default;
};
