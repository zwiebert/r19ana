#pragma once

#include <fstream>
#include <thread>

#include "Transport.hh"

class Xr25Transport : public Transport {
 public:
  using thread_fun_t = void (*)(ReadCallback cb, bool& keep_running);

 public:
  Xr25Transport(thread_fun_t thread_fun = read_thread_fun)
      : m_thread_fun(read_thread_fun) {}

  ~Xr25Transport() { stop(); }

  static void read_thread_fun(ReadCallback cb, bool& keep_running) {
    if (std::ifstream is("data/r19data.bin", std::ifstream::binary); is) {
      char buf[16];
      for (; is && keep_running;
           std::this_thread::sleep_for(std::chrono::milliseconds(1))) {
        is.read(buf, sizeof buf);
        if (is) {
          if (cb) cb((uint8_t*)buf, sizeof buf);
        }
      }
    }
  }
  void start(ReadCallback cb) override {
    if (keep_running) return;
    keep_running = true;
    m_thread = std::thread(m_thread_fun, cb, std::ref(keep_running));
  }

  void stop() override {
    if (!keep_running) return;
    keep_running = false;
    if (m_thread.joinable()) m_thread.join();
  }

 private:
  thread_fun_t m_thread_fun;
  std::thread m_thread;
  bool keep_running = false;
};
