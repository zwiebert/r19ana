// main.cc

#include "main.hh"

#include <stdio.h>
#include <string.h>

#include <chrono>
#include <fstream>
#include <iostream>
#include <thread>

#include "FrameProcessor.hh"
#include "cli.hh"
#include "select_model.hh"

#define D(x)

int terminal_puts(const char* s, bool block) {
  return term_transport.write((const uint8_t*)s, strlen(s), block);
}

PrintCarDiag::line_view_mask_t Mask = PrintCarDiag::line_view_mask_t().set();

bool cli_parse_and_execute_cmdline(char* src) {
  for (auto cmd = cli_cmds_begin(), end = cli_cmds_end(); cmd != end; ++cmd) {
    std::cerr << "for cmd loop line:(" << src << ")\n";
    if (cmd->execute(src)) {
      return true;
    }
  }
  terminal_puts("unknown command\n");
  return false;
}

int r19_alloc_and_print(char*& dst, const PrintCarDiag& print_diag,
                        const PrintCarDiag::line_view_mask_t& mask) {
  const char prepend_txt[] = "\n";  // "\x1B[2J";
  const char append_txt[] = "";     // "\x1B[2J";
  const size_t prepend_txt_len = sizeof prepend_txt - 1;
  const size_t append_txt_len = sizeof append_txt - 1;

  static size_t buf_len;  // save last calls buf_len to optimize one
                          // snprintf_diag call away
  char* ptr = nullptr;

  for (int pass = 0; pass < 2; ++pass) {
    constexpr size_t alloc_add = 10;
    const size_t alloc_size =
        buf_len + prepend_txt_len + append_txt_len + alloc_add + 1;
    auto tmp = (char*)realloc(ptr, alloc_size);
    if (!tmp) {
      free(ptr);
      return -1;
    }
    ptr = tmp;
    {
      char* dst = ptr + prepend_txt_len;
      const size_t dst_size = buf_len + 1;
      buf_len = print_diag.snprint_diag(dst, dst_size, mask);
      if (buf_len >= dst_size) continue;
    }

    strcat(ptr + prepend_txt_len + buf_len, append_txt);
    dst = ptr;
    return buf_len + prepend_txt_len + append_txt_len;
  }
  return -1;
}
