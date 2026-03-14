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
  const char prepend_txt[] = "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\r----Live Data----\r\n";// "\x1B[2J";
  const char append_txt[] = "-----------\r\n";// "\x1B[2J";

  if (auto buf_len = r19_frame_print(&dummy, 0, R19_frame, mask); buf_len > 0) {
    if (auto ptr = (char*)malloc(buf_len + 1 + sizeof prepend_txt + sizeof append_txt); ptr) {
      memcpy(ptr, prepend_txt, sizeof prepend_txt);
      if (auto len = r19_frame_print(ptr + sizeof prepend_txt, buf_len + 1, R19_frame, mask)) {
        memcpy(ptr + sizeof prepend_txt + len, append_txt, sizeof append_txt);
        dst = ptr;
        return len + sizeof prepend_txt + sizeof append_txt ;
      }
      free(ptr);
    }
  }
  return -1;
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
         auto n = strtoul(tok, nullptr, 10);
         if (n == 0) {
           if (strstr(tok, "0")) mask = ~0UL;
         } else
           mask.set(n - 1);
       }
       Mask = mask;
       Force = true;

       return true;
     }},
};

bool cli_parse_and_execute_cmdline(char* src) {
  for (auto cmd : cmds) {
    if (cmd.execute(src)) {
      return true;
    }
  }
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

#if 0
  // feed some data to the frame processor for testing
  processor.feedBytes(hexStringToByteArray("ff00107710447d79bf1aa45fc608080c"));
  processor.feedBytes(hexStringToByteArray("0073ffff0100006b3004048079811288"));
  processor.feedBytes(hexStringToByteArray("ff00107710437d79ba19705fc608040c"));
  processor.feedBytes(hexStringToByteArray("0073ffff0100006b3004048079811288"));
  processor.feedBytes(hexStringToByteArray("ff00107710437d79b8198c5fc408050c"));
#endif

  UartTransport uart2(UartTransportArgs{
      .bps = 65000, .uart_port_num = 2, .rx_gpio = 16, .tx_gpio = 17});
  uart2.start([&processor](auto data, auto data_len) {
    ESP_LOGI("uart2", "read len: %u", data_len);
    processor.feedBytes(data, data_len);
  });

  void mock_uart_fun();
  auto mock_uart_thread = std::thread(mock_uart_fun);

  spp2_main();
  char* dst = 0;
  for (;; std::this_thread::sleep_for(std::chrono::milliseconds(50))) {
    if (!spp_is_connected()) continue;

    uint8_t* data_in;
    size_t data_in_len;
    if (spp_rx_dequeue(data_in, data_in_len)) {
      if (cli_parse_and_execute_cmdline((char*)data_in)) {
        free(data_in);
      } else {
        spp_tx_enqueue("error: unknown commandline\r\n");
        std::this_thread::sleep_for(std::chrono::seconds(2));
      }
    }

    if (auto dst_len = r19_alloc_and_print(dst, Mask); dst_len > 0) {
      if (spp_tx_enqueue((uint8_t*)dst, dst_len, true)) {
        continue;
      }
    }
  }

  return 0;
}

int main() { return app_main(); }

void mock_uart_fun() {
  UartTransport uart1(UartTransportArgs{
      .bps = 65000, .uart_port_num = 1, .rx_gpio = 18, .tx_gpio = 19});
  uart1.start([](uint8_t* data, size_t data_len) {});
  auto v0 = hexStringToByteArray("ff00107710447d79bf1aa45fc608080c");
  auto v1 = hexStringToByteArray("0073ffff0100006b3004048079811288");
  auto v2 = hexStringToByteArray("ff00107710437d79ba19705fc608040c");
  auto v3 = hexStringToByteArray("0073ffff0100006b3004048079811288");
  auto v4 = hexStringToByteArray("ff00107710437d79b8198c5fc408050c");

  for (;; std::this_thread::sleep_for(std::chrono::seconds(1))) {
    constexpr int pms = 100;
    auto res = uart1.write(&v0[0], v0.size());
    ESP_LOGI("uart1", "write len: %u", res);
    std::this_thread::sleep_for(std::chrono::milliseconds(pms));
    res = uart1.write(&v1[0], v1.size());
    ESP_LOGI("uart1", "write len: %u", res);
    std::this_thread::sleep_for(std::chrono::milliseconds(pms));
    res = uart1.write(&v2[0], v2.size());
    ESP_LOGI("uart1", "write len: %u", res);
    std::this_thread::sleep_for(std::chrono::milliseconds(pms));
    res = uart1.write(&v3[0], v3.size());
    ESP_LOGI("uart1", "write len: %u", res);
    std::this_thread::sleep_for(std::chrono::milliseconds(pms));
    res = uart1.write(&v4[0], v4.size());
    ESP_LOGI("uart1", "write len: %u", res);
    std::this_thread::sleep_for(std::chrono::milliseconds(pms));
  }
}