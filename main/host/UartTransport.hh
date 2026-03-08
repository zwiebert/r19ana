// UartTransport.hh
#pragma once

#include "Transport.hh"

// platform‑specific implementation: open USB serial or CDC
class UartTransport : public Transport {
 public:
  void start(ReadCallback cb) override {};
  void stop() override {};
};