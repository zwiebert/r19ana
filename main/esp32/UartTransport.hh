// UartTransport.hh
#pragma once

#include <thread>

#include "Transport.hh"
#include "driver/uart.h"

struct UartTransportArgs {
  int bps = 9600;
  uint8_t uart_port_num = 2;
  uart_word_length_t data_bits = UART_DATA_8_BITS;
  uart_parity_t parity = UART_PARITY_DISABLE;
  uart_stop_bits_t stop_bits = UART_STOP_BITS_1;
  int8_t rx_gpio = -1;
  int8_t tx_gpio = -1;
};

// platform‑specific implementation: open USB serial or CDC
class UartTransport : public Transport {
 public:
  ~UartTransport();
  UartTransport(const UartTransportArgs&& args = {});

  void start(ReadCallback cb) override;
  void stop() override;
  int write(const u_int8_t* data, size_t data_len);

 private:
  uart_port_t m_uart_port;
  int8_t m_rx_gpio, m_tx_gpio;
  int8_t m_rts_gpio = -1,
         m_cts_gpio = -1;  // hardware flow control not supported for now
  uart_config_t m_uart_config;
  std::thread m_worker_thread;
  uint8_t* m_buf = 0;

 private:
  void worker_thread(ReadCallback cb);
  bool m_worker_thread_terminate_flag = false;
};