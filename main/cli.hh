#pragma once

#include <string.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "Transport.hh"
#include "main.hh"
#include "select_model.hh"

struct CliCmd {
  const char* name = "";
  const char* help_txt = "";
  bool (*handler)(struct CliCmd& cli_cmd) = 0;
  char* args = 0;
  using reply_fun_t = bool (*)(const char*);
  reply_fun_t reply = [](const char* msg) -> bool {
#ifdef ESP_PLATFORM
    return term_transport.write((const uint8_t*)msg, strlen(msg));
#else
    std::cout << msg << "\n";
    return true;
#endif
  };

  bool execute(char* cmd_line_buf) {
    if (*cmd_line_buf && strstr(cmd_line_buf, name) != cmd_line_buf)
      return false;
    args = cmd_line_buf + strlen(name);
    if (handler) return handler(*this);
    return true;
  }
};

CliCmd *cli_cmds_begin();
CliCmd *cli_cmds_end();
