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

#define ECHO_UART_PORT_NUM (m_uart_port)
#define ECHO_UART_BAUD_RATE (CONFIG_EXAMPLE_UART_BAUD_RATE)
#define ECHO_TASK_STACK_SIZE (CONFIG_EXAMPLE_TASK_STACK_SIZE)

static const char* TAG = "UART TEST";

#define BUF_SIZE (1024)
UartTransport::~UartTransport() {
  stop();
}

UartTransport::UartTransport()
    : m_uart_port(uart_port_t(CONFIG_EXAMPLE_UART_PORT_NUM)),
      m_uart_config({
          .baud_rate = ECHO_UART_BAUD_RATE,
          .data_bits = UART_DATA_8_BITS,
          .parity = UART_PARITY_DISABLE,
          .stop_bits = UART_STOP_BITS_1,
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
    int len = uart_read_bytes(ECHO_UART_PORT_NUM, data, (BUF_SIZE - 1),
                              10000 / portTICK_PERIOD_MS);
    if (len) {
      data[len] = '\0';
      cb(data, len);
      ESP_LOGI(TAG, "Recv str: %s", (char*)data);
    }
  }
}
void UartTransport::stop() {
  m_worker_thread_terminate_flag = true;
  m_worker_thread.join();
  free(m_buf);
  m_buf = 0;
}

void UartTransport::start(ReadCallback cb) {
  int intr_alloc_flags = 0;
#if CONFIG_UART_ISR_IN_IRAM
  intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

  ESP_ERROR_CHECK(uart_driver_install(ECHO_UART_PORT_NUM, BUF_SIZE * 2, 0, 0,
                                      NULL, intr_alloc_flags));
  ESP_ERROR_CHECK(uart_param_config(ECHO_UART_PORT_NUM, &m_uart_config));
  ESP_ERROR_CHECK(uart_set_pin(ECHO_UART_PORT_NUM, ECHO_TEST_TXD, ECHO_TEST_RXD,
                               ECHO_TEST_RTS, ECHO_TEST_CTS));

  // Configure a temporary buffer for the incoming data
  m_buf = (uint8_t*)malloc(BUF_SIZE);

  // Create a thread using default values that can run on any core
  auto cfg = esp_pthread_get_default_config();
  esp_pthread_set_cfg(&cfg);
  m_worker_thread = std::thread(&UartTransport::worker_thread, this, cb);
}

/* pthread/std::thread example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <esp_log.h>
#include <esp_pthread.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <thread>

using namespace std::chrono;

const auto sleep_time = seconds{5};

void print_thread_info(const char* extra = nullptr) {
  std::stringstream ss;
  if (extra) {
    ss << extra;
  }
  ss << "Core id: " << xPortGetCoreID()
     << ", prio: " << uxTaskPriorityGet(nullptr)
     << ", minimum free stack: " << uxTaskGetStackHighWaterMark(nullptr)
     << " bytes.";
  ESP_LOGI(pcTaskGetName(nullptr), "%s", ss.str().c_str());
}

void thread_func_inherited() {
  while (true) {
    print_thread_info(
        "This is the INHERITING thread with the same parameters as our parent, "
        "including name. ");
    std::this_thread::sleep_for(sleep_time);
  }
}

void spawn_another_thread() {
  // Create a new thread, it will inherit our configuration
  std::thread inherits(thread_func_inherited);

  while (true) {
    print_thread_info();
    std::this_thread::sleep_for(sleep_time);
  }
}

void thread_func_any_core() {
  while (true) {
    print_thread_info(
        "This thread (with the default name) may run on any core.");
    std::this_thread::sleep_for(sleep_time);
  }
}

void thread_func() {
  while (true) {
    print_thread_info();
    std::this_thread::sleep_for(sleep_time);
  }
}

esp_pthread_cfg_t create_config(const char* name, int core_id, int stack,
                                int prio) {
  auto cfg = esp_pthread_get_default_config();
  cfg.thread_name = name;
  cfg.pin_to_core = core_id;
  cfg.stack_size = stack;
  cfg.prio = prio;
  return cfg;
}

extern "C" void pthread_main(void) {
  // Create a thread using default values that can run on any core
  auto cfg = esp_pthread_get_default_config();
  esp_pthread_set_cfg(&cfg);
  std::thread any_core(thread_func_any_core);

  // Create a thread on core 0 that spawns another thread, they will both have
  // the same name etc.
  cfg = create_config("Thread 1", 0, 3 * 1024, 5);
  cfg.inherit_cfg = true;
  esp_pthread_set_cfg(&cfg);
  std::thread thread_1(spawn_another_thread);

  // Create a thread on core 1.
  cfg = create_config("Thread 2", 1, 3 * 1024, 5);
  esp_pthread_set_cfg(&cfg);
  std::thread thread_2(thread_func);

  // Let the main task do something too
  while (true) {
    std::stringstream ss;
    ss << "core id: " << xPortGetCoreID()
       << ", prio: " << uxTaskPriorityGet(nullptr)
       << ", minimum free stack: " << uxTaskGetStackHighWaterMark(nullptr)
       << " bytes.";
    ESP_LOGI(pcTaskGetName(nullptr), "%s", ss.str().c_str());
    std::this_thread::sleep_for(sleep_time);
  }
}
