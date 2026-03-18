#pragma once

#include <string.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "R19Frame.hh"
#include "R19Frame_utils.hh"
#include "Transport.hh"
#include "main.hh"

struct CliCmd {
  const char* name = "";
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

constexpr const char* reply_msg_all_hidden =
    "all entries are now hidden!\r\n"
    "Use <hide 0> to unhide all.\r\n"
    "Or use <show 1,2,3> to show some.\r\n";

CliCmd cmds[] = {
    {.name = "show ",
     .handler = [](CliCmd& cmd) -> bool {
       // command line was like: "filter 1,2,3,8,12".
       // cmd.args is now "1,2,3,8,12"
       // we need to convert this into a bitset<32> with only bits 0,1,2,7,11
       // are set to true.
       r19frame_mask_t mask = Mask;
       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", \r\n", &save_ptr)); str = nullptr) {
         auto n = strtoul(tok, nullptr, 10);
         if (n == 0) {
           if (strstr(tok, "0")) mask.reset();
         } else
           mask.set(n - 1);
       }
       Mask = mask;
       if (!mask.any()) cmd.reply(reply_msg_all_hidden);
       return true;
     }},

    {.name = "hide ",
     .handler = [](CliCmd& cmd) -> bool {
       // command line was like: "filter 1,2,3,8,12".
       // cmd.args is now "1,2,3,8,12"
       // we need to convert this into a bitset<32> with only bits 0,1,2,7,11
       // are set to true.
       r19frame_mask_t mask = Mask;
       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", \r\n", &save_ptr)); str = nullptr) {
         auto n = strtoul(tok, nullptr, 10);
         if (n == 0) {
           if (strstr(tok, "0")) mask.set();
         } else
           mask.reset(n - 1);
       }
       Mask = mask;
       if (!mask.any()) cmd.reply(reply_msg_all_hidden);
       return true;
     }},

#ifdef ESP_PLATFORM
    {.name = "mock-loop",
     .handler = [](CliCmd& cmd) -> bool {
       static std::thread mock_uart_thread;
       static bool keep_running;

       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", \r\n", &save_ptr)); str = nullptr) {
         if (strcmp(tok, "on") == 0) {
           void mock_uart_fun(bool& keep_running);
           if (keep_running) return false;
           keep_running = true;
           mock_uart_thread =
               std::thread(mock_uart_fun, std::ref(keep_running));
           return true;
         } else if (strcmp(tok, "off") == 0) {
           if (!keep_running) return false;
           keep_running = false;
           mock_uart_thread.join();
           return true;
         } else {
           return false;  // unknown argument
         }
       }
       return false;
     }},
#endif
};
