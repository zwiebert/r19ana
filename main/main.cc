// main.cc
#include <stdio.h>
#include <string.h>

#include <chrono>
#include <iostream>
#include <thread>

#include "FrameProcessor.hh"
#include "R19Frame_utils.hh"
#include "UartTransport.hh"
#include "bt_classic/spp_acceptor.hh"

#define SPP2

R19Frame R19_frame;
constexpr unsigned FORCE_IVAL_S = 60;

std::vector<uint8_t> hexStringToByteArray(const std::string& hex) {
  std::vector<uint8_t> bytes;

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    uint8_t byte = (uint8_t)strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }

  return bytes;
}

r19frame_mask_t Mask = ~0LU;
bool Force;

int r19_alloc_and_print(char*& dst, r19frame_mask_t mask = ~0UL) {
  char dummy;
  if (auto buf_len = r19_frame_print(&dummy, 0, R19_frame, mask); buf_len > 0) {
    if (auto ptr = (char*)malloc(buf_len + 1); ptr) {
      bool succ = true;
      if (auto len = r19_frame_print(ptr, buf_len + 1, R19_frame, mask)) {
        dst = ptr;
        return len;
      }
      free(ptr);
    }
  }
  return -1;
}

int spp_write_cb(char* dst, size_t dst_size, spp::status_t& status) {
  int ret = 0;

  ret = write_r19_frame(dst, dst_size, R19_frame, Mask,
                        status.test(spp::JUST_CONNECTED) || Force);
  Force = false;
  if (0 < ret && ret < dst_size) return ret;

  return ret;
}

struct CliCmd {
  const char* name = 0;
  bool (*handler)(struct CliCmd& cli_cmd) = 0;
  char* args = 0;
  using reply_fun_t = bool (*)(const char*);
  reply_fun_t reply = [](const char* msg) -> bool {
    return spp_tx_enqueue(msg);
  };

  bool execute(char* cmd_line_buf) {
    if (strstr(cmd_line_buf, name) != cmd_line_buf) return false;
    args = cmd_line_buf + strlen(name);
    if (handler) return handler(*this);
    return true;
  }
};

const std::array<CliCmd, 1> cmds = {
    {"filter ",
     [](CliCmd& cmd) -> bool {
       // command line was like: "filter 1,2,3,8,12".
       // cmd.args is now "1,2,3,8,12"
       // we need to convert this into a bitset<32> with only bits 0,1,2,7,11
       // are set to true.
       r19frame_mask_t mask;
       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", ", &save_ptr)); str = nullptr) {
         mask.set(strtoul(tok, nullptr, 10) - 1);
       }
       Mask = mask;
       Force = true;

       cmd.reply("We made it to the filter handler... yippie\r\n");

       return true;
     }},
};

bool spp_read_cb(char* src) {
  for (auto cmd : cmds) {
    if (cmd.execute(src)) return true;
  }
  // we are here, because no command handler matched
  spp_tx_enqueue("Thanks for your input, friend :)\r\n");
  return false;
}

extern "C" int app_main() {
  FrameProcessor processor([](const std::string& hex, R19Frame&& r19_frame) {
    R19_frame = r19_frame;  // store latest frame for later use
#ifndef ESP_PLATFORM
    std::cout << "HEX: " << hex << "\n";

    constexpr size_t buf_size = 1024;
    auto buf = new char[buf_size];
    auto len = write_r19_frame(buf, buf_size, r19_frame, ~0UL, true);
    if (len < buf_size) std::cout.write(buf, len);
#endif
  });

#if 1
  // feed some data to the frame processor for testing
  processor.feedBytes(hexStringToByteArray("ff00107710447d79bf1aa45fc608080c"));
  processor.feedBytes(hexStringToByteArray("0073ffff0100006b3004048079811288"));
  processor.feedBytes(hexStringToByteArray("ff00107710437d79ba19705fc608040c"));
  processor.feedBytes(hexStringToByteArray("0073ffff0100006b3004048079811288"));
  processor.feedBytes(hexStringToByteArray("ff00107710437d79b8198c5fc408050c"));
#endif

  spp2_main();
  char* dst = 0;
  for (;; std::this_thread::sleep_for(std::chrono::milliseconds(50))) {
    if (!spp_is_connected()) continue;

    uint8_t* data_in;
    size_t data_in_len;
    if (spp_rx_dequeue(data_in, data_in_len)) {
      spp_read_cb((char*)data_in);
    }

    if (auto dst_len = r19_alloc_and_print(dst, Mask); dst_len > 0) {
      if (spp_tx_enqueue((uint8_t*)dst, dst_len, true)) {
        continue;
      }
    }
  }
  UartTransport uart;
  uart.start([&processor](auto data, auto data_len) {
    processor.feedBytes(data, data_len);
  });

  for (;; std::this_thread::sleep_for(std::chrono::seconds(5))) {
  }

  return 0;
}

int main() { return app_main(); }