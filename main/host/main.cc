// main.cc

#include "main.hh"

#include <stdio.h>
#include <string.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "ConsoleTransport.hh"
#include "FrameProcessor.hh"
#include "UartTransport.hh"
#include "Xr25Transport.hh"
#include "cli.hh"
#include "select_model.hh"

#define D(x)

Transport&& xr25_transport = Xr25Transport();
Transport&& term_transport = ConsoleTransport();

int main() {
  select_model("raw");

  FrameProcessor processor([](const XR25Frame::voc_t& frame) {
    print_car_diag->push_frame(frame);
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

  xr25_transport.start([&processor](auto data, auto data_len) {
    processor.feedBytes(data, data_len);
  });
  term_transport.start();
#if 1
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