// UartTransport.hh
#pragma once

#include "Transport.hh"
#include "driver/uart.h"
#include <thread>

// platform‑specific implementation: open USB serial or CDC
class UartTransport : public Transport {
 public:
  ~UartTransport();
  UartTransport();
  UartTransport(unsigned uart_num);
  void start(ReadCallback cb) override;
  void stop() override;
 private:
  uart_port_t m_uart_port;
  uart_config_t m_uart_config;
  std::thread m_worker_thread;
  uint8_t *m_buf = 0;
 private:
  void worker_thread(ReadCallback cb);
  bool m_worker_thread_terminate_flag = false;

};