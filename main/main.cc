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

int spp_write_r19_frame(char* dst, size_t dst_siz, const R19Frame& d,
                        r19frame_mask_t mask = ~0UL, bool force = false) {
  ssize_t dst_size = ssize_t(dst_siz);
  static R19Frame c;        // copy of last written frame
  static time_t last_time;  // last time when data was successfully written
  int ct = 0;

  const bool hide_unchanged = !force && true;
  const r19frame_mask_t view_mask =
      hide_unchanged ? (r19_frame_members_cmp(c, d) & mask)
      : (last_time + FORCE_IVAL_S < time(0)) ? ~0UL
                                             : mask;

  if (view_mask.any()) {
    ct = r19_frame_print(dst, dst_size, d, view_mask);

    if (ct >= dst_size || ct < 0)
      return ct;  // ERROR: data was not fully written (e.g. buffer too small)

    // all done. update our local state variables
    c = R19_frame;
    last_time = time(0);
  }

  return ct;
}

r19frame_mask_t Mask = ~0LU;
bool Force;

int spp_write_cb(char* dst, size_t dst_size, spp::status_t& status) {
  int ret = 0;

  ret = spp_write_r19_frame(dst, dst_size, R19_frame, Mask,
                            status.test(spp::JUST_CONNECTED) || Force);
  Force = false;
  if (0 < ret && ret < dst_size) return ret;

  return ret;
}

struct CliCmd {
  const char* name = 0;
  bool (*handler)(struct CliCmd& cli_cmd) = 0;
  char* args = 0;
  char* dst = 0;
  size_t dst_size = 0;

  bool execute(char* cmd_line_buf, size_t buf_size) {
    if (strstr(cmd_line_buf, name) != cmd_line_buf) return false;
    args = cmd_line_buf + strlen(name);
    dst = cmd_line_buf;
    dst_size = buf_size;  ///< in: size of buf, out: length of reply data/string
    if (handler) return handler(*this);

    dst_size = 0;
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

       cmd.dst_size =
           snprintf(cmd.dst, cmd.dst_size,
                    "We made it to the filter handler... yippie\r\n");
       return true;
     }},
};

std::size_t spp_read_cb(char* src_dst, std::size_t src_len,
                        std::size_t dst_size, spp::status_t& status) {
  printf("spp-in: %.*s", src_len, src_dst);

  for (auto cmd : cmds) {
    if (cmd.execute(src_dst, dst_size)) return cmd.dst_size;
  }

  // we are here, because no command handler matched
  return snprintf(src_dst, dst_size, "Thanks for your input, friend :)\r\n");
}

extern "C" int app_main() {
  FrameProcessor processor([](const std::string& hex, R19Frame&& r19_frame) {
    R19_frame = r19_frame;  // store latest frame for later use
#ifndef ESP_PLATFORM
    std::cout << "HEX: " << hex << "\n";

    constexpr size_t buf_size = 1024;
    auto buf = new char[buf_size];
    auto len = spp_write_r19_frame(buf, buf_size, r19_frame);
    if  (len < buf_size)
       std::cout.write(buf, len);
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

  spp_main(spp_read_cb, spp_write_cb);

  UartTransport uart;
  uart.start([&processor](auto data, auto data_len) {
    processor.feedBytes(data, data_len);
  });

  for (;; std::this_thread::sleep_for(std::chrono::seconds(5))) {
  }

  return 0;
}

int main() { return app_main(); }