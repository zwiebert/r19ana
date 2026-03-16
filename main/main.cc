// main.cc

#include "main.hh"

#include "FrameProcessor.hh"
#include "R19Frame_utils.hh"
#include "UartTransport.hh"
#include "cli.hh"
#ifdef ESP_PLATFORM
#include "SppTransport.hh"
#else
#include "ConsoleTransport.hh"
#include "Xr25Transport.hh"
#endif

#include <stdio.h>
#include <string.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#define D(x)

#ifdef ESP_PLATFORM
Transport&& xr25_transport = UartTransport(UartTransportArgs{
    .bps = 65000, .uart_port_num = 2, .rx_gpio = 16, .tx_gpio = 17});
Transport&& mock_loop_transport = UartTransport(UartTransportArgs{
    .bps = 65000, .uart_port_num = 1, .rx_gpio = 18, .tx_gpio = 19});
Transport&& term_transport = SppTransport();
#else
Transport&& xr25_transport = Xr25Transport();
Transport&& term_transport = ConsoleTransport();
#endif

int terminal_puts(const char* s, bool block) {
  return term_transport.write((const uint8_t*)s, strlen(s), block);
}

R19Frame R19_frame;
r19frame_mask_t Mask = ~0LU;

bool cli_parse_and_execute_cmdline(char* src) {
  for (auto& cmd : cmds) {
    std::cerr << "for cmd loop\n";
    if (cmd.execute(src)) {
      return true;
    }
  }
  terminal_puts("unknown command\r\n");
  return false;
}

int r19_alloc_and_print(char*& dst, const R19Frame& R19_frame,
                        r19frame_mask_t mask = ~0UL) {
  char dummy;
  const char prepend_txt[] = "";     // "\x1B[2J";
  const char append_txt[] = "\r\n";  // "\x1B[2J";

  if (auto buf_len = r19_frame_print(&dummy, 0, R19_frame, mask); buf_len > 0) {
    if (auto ptr =
            (char*)malloc(buf_len + 1 + sizeof prepend_txt + sizeof append_txt);
        ptr) {
      memcpy(ptr, prepend_txt, sizeof prepend_txt);
      if (auto len = r19_frame_print(ptr + sizeof prepend_txt, buf_len + 1,
                                     R19_frame, mask)) {
        memcpy(ptr + sizeof prepend_txt + len, append_txt, sizeof append_txt);
        dst = ptr;
        return len + sizeof prepend_txt + sizeof append_txt;
      }
      free(ptr);
    }
  }
  return -1;
}
void test_print_frame(const XR25Frame& frame) {
  std::cout << "HEX: " << frame.toString() << "\n";
  std::cout << "bit:  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 "
               "18 19 20 21 22 23 24 25 26 27 28 29 30 31\n";
  constexpr size_t buf_size = 1024;
  auto buf = new char[buf_size];
  auto len = write_r19_frame(buf, buf_size, R19Frame(frame), ~0UL, true);
  if (len < buf_size) std::cout.write(buf, len);
}

#ifdef ESP_PLATFORM
extern "C" int app_main() {
  // processor calls back when it has completed a frame from the chunks of bytes
  // it got from x25_transport
  FrameProcessor processor([](const XR25Frame& frame) {
    R19_frame = frame;
    if (!spp_is_connected()) return;
    char* dst = 0;
    if (auto dst_len = r19_alloc_and_print(dst, R19_frame, Mask); dst_len > 0) {
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

#endif

#ifdef ESP_PLATFORM
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
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  mock_loop_transport.stop();
}
#endif

int main() {
  FrameProcessor processor(
      [](const XR25Frame& frame) { test_print_frame(frame); });

  xr25_transport.start([&processor](auto data, auto data_len) {
    processor.feedBytes(data, data_len);
  });
  term_transport.start();
#if 0
  char* dst = 0;
  for (;; std::this_thread::sleep_for(std::chrono::milliseconds(10))) {
    std::cerr << "for loop\n";
    uint8_t* data_in = 0;
    size_t data_in_len = 0;
    if (term_transport.read(data_in, data_in_len)) {
      if (cli_parse_and_execute_cmdline((char*)data_in)) {
        free(data_in);
      } else {
        std::this_thread::sleep_for(std::chrono::seconds(2));
      }
    }
  }
#endif

  std::this_thread::sleep_for(std::chrono::seconds(2));

  xr25_transport.stop();
  term_transport.stop();
  return 0;
}