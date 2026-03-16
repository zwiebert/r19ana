// FrameProcessor.hh
#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <condition_variable>

#include "R19Frame.hh"
#include "XR25Frame.hh"

class FrameProcessor {
 public:
  using UpdateCallback = std::function<void(const XR25Frame::frame_data_t& frame, int frame_count)>;

  explicit FrameProcessor(UpdateCallback);
  ~FrameProcessor();

  void feedBytes(const std::vector<uint8_t>& data);
  void feedBytes(const uint8_t* data, std::size_t data_len);

  void test() { xr25.test(); }

 private:
  void update_thread_fun();

 private:
  int old_frame_number = 0;
  XR25Frame xr25;
  UpdateCallback callback;
  std::thread m_update_thread;
  std::mutex m_update_thread_mutex;
  std::condition_variable m_update_thread_cv;
  bool m_update_thread_keep_running = false;
  int m_last_xr25_frame_number = 0;
};