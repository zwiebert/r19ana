#pragma once

#include "Transport.hh"
#include <iostream>
#include <thread>
#include <string.h>

class ConsoleTransport : public Transport {
 public:
  ~ConsoleTransport() { stop(); }
  int write(const uint8_t* data, size_t data_len, bool block = false) override {
    if (std::cout.write((const char*)data, data_len)) return data_len;

    return -1;
  }
  int read(uint8_t*& dst, size_t& dst_len, bool block = false) override {
    std::cerr << "in read function \n";
    std::string line;
    std::cin.clear();
    std::getline(std::cin, line);
    if (!std::cin.fail()) {
      if (auto ptr = (char*)malloc(line.length() + 1)) {
        strcpy(ptr, line.c_str());
        dst = (uint8_t*)ptr;
        dst_len = line.length();
      }
    }
    std::cerr << "failure\n";
    return -1;
  }

  static void read_data_thread(ReadCallback cb, bool& keep_running) {
    do {
      std::string line;
      std::getline(std::cin, line);
      if (cb) cb((uint8_t*)line.c_str(), line.length());
    } while (keep_running);
  }

  void start(ReadCallback cb) override {
    if (keep_running) return;
    keep_running = true;
    m_thread = std::thread(read_data_thread, cb, std::ref(keep_running));
  }

  void stop() override {
    if (!keep_running) return;
    keep_running = false;
    m_thread.join();
  }

 private:
  std::thread m_thread;
  bool keep_running = false;
};
