#pragma once

struct settings_model_specific_t {
  unsigned serial_bps = 65000;
};

class CarModelBase {
 public:
  virtual ~CarModelBase() = default;

 public:
  virtual void get_defaults(settings_model_specific_t& dst) const { dst = {}; }
};