#include "UartTransport.hh"

/* UART Echo Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <esp_log.h>
#include <esp_pthread.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

#include "driver/gpio.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

using namespace std::chrono;

#define D(x)

/**
 * This is an example which echos any data it receives on configured UART back
 * to the sender, with hardware flow control turned off. It does not use UART
 * driver event queue.
 *
 * - Port: configured UART
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: off
 * - Pin assignment: see defines below (See Kconfig)
 */

#define ECHO_TEST_TXD (CONFIG_EXAMPLE_UART_TXD)
#define ECHO_TEST_RXD (CONFIG_EXAMPLE_UART_RXD)
#define ECHO_TEST_RTS (UART_PIN_NO_CHANGE)
#define ECHO_TEST_CTS (UART_PIN_NO_CHANGE)

#define ECHO_UART_PORT_NUM (CONFIG_EXAMPLE_UART_PORT_NUM)
#define ECHO_UART_BAUD_RATE (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE (CONFIG_EXAMPLE_TASK_STACK_SIZE)

static const char* TAG = "UART TEST";

#define BUF_SIZE (512)
UartTransport::~UartTransport() { stop(); }

UartTransport::UartTransport(const UartTransportArgs&& a)
    : m_uart_port(uart_port_t(a.uart_port_num)),
      m_rx_gpio(a.rx_gpio),
      m_tx_gpio(a.tx_gpio),
      m_uart_config(uart_config_t{
          .baud_rate = a.bps,
          .data_bits = a.data_bits,
          .parity = a.parity,
          .stop_bits = a.stop_bits,
          .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
          .rx_flow_ctrl_thresh = 0,
          .source_clk = UART_SCLK_DEFAULT,
          .flags = {},
      }) {}

void UartTransport::worker_thread(ReadCallback cb) {
  ESP_LOGI(TAG, "%s: read uart from thread", __func__);
  while (!m_worker_thread_terminate_flag) {
    auto data = m_buf;
    // Read data from the UART
    int len = uart_read_bytes(m_uart_port, data, (BUF_SIZE - 1),
                              100 / portTICK_PERIOD_MS);
    if (len) {
    D(ESP_LOGI("uart2", "read len: %d", len));
      data[len] = '\0';
      cb(data, len);
    }
  }
}
void UartTransport::stop() {
  m_worker_thread_terminate_flag = true;
  m_worker_thread.join();
  free(m_buf);
  m_buf = 0;
  ESP_ERROR_CHECK(uart_driver_delete(m_uart_port));
}

void UartTransport::start(ReadCallback cb) {
  int intr_alloc_flags = 0;
#if CONFIG_UART_ISR_IN_IRAM
  intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

  ESP_ERROR_CHECK(uart_driver_install(m_uart_port, BUF_SIZE * 2, 0, 0, NULL,
                                      intr_alloc_flags));
  ESP_ERROR_CHECK(uart_param_config(m_uart_port, &m_uart_config));
  ESP_ERROR_CHECK(
      uart_set_pin(m_uart_port, m_tx_gpio, m_rx_gpio, m_rts_gpio, m_cts_gpio));

  // Configure a temporary buffer for the incoming data
  m_buf = (uint8_t*)malloc(BUF_SIZE);

  // Create a thread using default values that can run on any core
  auto cfg = esp_pthread_get_default_config();
  esp_pthread_set_cfg(&cfg);
  cfg.pin_to_core = 1;
  m_worker_thread = std::thread(&UartTransport::worker_thread, this, cb);
}

int UartTransport::write(const u_int8_t* data, size_t data_len, bool block) {
  return uart_write_bytes(m_uart_port, (const char*)data, data_len);
}