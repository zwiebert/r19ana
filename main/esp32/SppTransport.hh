// UartTransport.hh
#pragma once

#include <thread>
#include "Transport.hh"
#include "bt_classic/spp_acceptor.hh"

class SppTransport : public Transport {
 public:
  ~SppTransport() { stop(); }
  void read_cb_thread(ReadCallback cb) {
    char* dst = 0;
    for (; keep_running;
         std::this_thread::sleep_for(std::chrono::milliseconds(10))) {
      if (!spp_is_connected()) continue;
      uint8_t* data_in = 0;
      size_t data_in_len = 0;
      if (read(data_in, data_in_len, true) > 0) {
        cb(data_in, data_in_len);
        free(data_in);
      }
    }
  }
  int write(const uint8_t* data, size_t data_len, bool block = false) override {
    return spp_tx_enqueue(data, data_len, block);
  }
  int read(uint8_t*& dst, size_t& dst_len, bool block = false) override {
    return spp_rx_dequeue(dst, dst_len, block);
  }

  void start(ReadCallback cb) override {
    if (keep_running) return;
    spp2_main();
    keep_running = true;
    m_thread = std::thread(&SppTransport::read_cb_thread, this, cb);
  }

  void stop() override {
    if (!keep_running) return;
    keep_running = false;
    m_thread.join();
  }

 private:
  std::thread m_thread;
  bool keep_running = false;
};
