// main.cc

#include "main.hh"

#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "FrameProcessor.hh"
#include "SppTransport.hh"
#include "UartTransport.hh"
#include "cli.hh"
#include "select_model.hh"

#define D(x)

Transport&& xr25_transport = UartTransport(UartTransportArgs{
    .bps = 65000, .uart_port_num = 2, .rx_gpio = 16, .tx_gpio = 17});
Transport&& mock_loop_transport = UartTransport(UartTransportArgs{
    .bps = 65000, .uart_port_num = 1, .rx_gpio = 18, .tx_gpio = 19});
Transport&& term_transport = SppTransport();

static void main_init() {
  // Initialize NVS
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
      err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    // NVS partition was truncated and needs to be erased
    // Retry nvs_flash_init
    ESP_ERROR_CHECK(nvs_flash_erase());
    err = nvs_flash_init();
  }
  ESP_ERROR_CHECK(err);

  select_model_init();
}

extern "C" int app_main() {
  main_init();
  assert(print_car_diag);

  // processor calls back when it has completed a frame from the chunks of bytes
  // it got from x25_transport. processor has a dedicated thread for doing the
  // callback. its ok to block it.
  FrameProcessor processor([](const XR25Frame::voc_t& frame) {
    print_car_diag->push_frame(frame);
    if (!spp_is_connected()) return;
    char* dst = 0;
    if (auto dst_len = r19_alloc_and_print(dst, *print_car_diag, Mask);
        dst_len > 0) {
      if (term_transport.write((const uint8_t*)dst, dst_len, true)) {
        free(dst);
        return;
      }
      free(dst);
    }
  });

  // xr25_transport calls back when it has a received a chunk of bytes from
  // the car diagnose port
  xr25_transport.start([&processor](auto data, auto data_len) {
    processor.feedBytes(data, data_len);
  });

  // term_transport calls back when it has received a line of text from
  // terminal. It should be a user CLI command.
  term_transport.start([](auto data, auto data_len) {
    cli_parse_and_execute_cmdline((char*)data);
  });

  // it seems this thread has nothing left to do. Wait here to keep local object
  // <processor> alive
  for (;; std::this_thread::sleep_for(std::chrono::days(1))) {
  }

  xr25_transport.stop();
  term_transport.stop();
  return 0;
}

void mock_uart_fun(bool& keep_running) {
  extern const uint8_t r19data_bin_start[] asm("_binary_r19data_bin_start");
  extern const uint8_t r19data_bin_end[] asm("_binary_r19data_bin_end");
  const uint8_t* data = r19data_bin_start;
  const size_t data_size = r19data_bin_end - r19data_bin_start;
  const int chunk = 40;

  mock_loop_transport.start([](uint8_t* data, size_t data_len) {});

  while (keep_running) {
    for (int i = 0; i < data_size - chunk && keep_running; i += chunk) {
      auto res = mock_loop_transport.write(data + i, chunk);
      std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
  }

  mock_loop_transport.stop();
}