// main.cc

#include "main.hh"

#include "FrameProcessor.hh"
#include "UartTransport.hh"
#include "cli.hh"
#include "select_model.hh"

#include "ConsoleTransport.hh"
#include "Xr25Transport.hh"

#include <stdio.h>
#include <string.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#define D(x)

Transport&& xr25_transport = Xr25Transport();
Transport&& term_transport = ConsoleTransport();

int terminal_puts(const char* s, bool block) {
  return term_transport.write((const uint8_t*)s, strlen(s), block);
}

PrintCarDiag::line_view_mask_t Mask = PrintCarDiag::line_view_mask_t().set();

bool cli_parse_and_execute_cmdline(char* src) {
  for (auto& cmd : cmds) {
    std::cerr << "for cmd loop line:(" << src << ")\n";
    if (cmd.execute(src)) {
      return true;
    }
  }
  terminal_puts("unknown command\n");
  return false;
}

int r19_alloc_and_print(char*& dst, const PrintCarDiag& print_diag,
                        const PrintCarDiag::line_view_mask_t& mask) {
  char dummy;
  const char prepend_txt[] = "\n";  // "\x1B[2J";
  const char append_txt[] = "";       // "\x1B[2J";
  const size_t prepend_txt_len = sizeof prepend_txt - 1;
  const size_t append_txt_len = sizeof append_txt - 1;

  if (auto buf_len = print_diag.snprint_diag(&dummy, 0, mask); buf_len > 0) {
    if (auto ptr =
            (char*)malloc(buf_len  + prepend_txt_len + append_txt_len + 1);
        ptr) {
          *ptr = '\0';
          strcat(ptr, prepend_txt);
      if (auto len = print_diag.snprint_diag(ptr + prepend_txt_len,
                                             buf_len + 1, mask)) {
        strcat(ptr + prepend_txt_len + len, append_txt);
        dst = ptr;
        return len + prepend_txt_len + append_txt_len;
      }
      free(ptr);
    }
  }
  return -1;
}

int main() {
  select_model("exp");

  FrameProcessor processor(
      [](const XR25Frame::voc_t& frame) {
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