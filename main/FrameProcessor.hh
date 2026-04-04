// FrameProcessor.hh
#pragma once

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <condition_variable>

#include "XR25Frame.hh"

class FrameProcessor {
 public:
  using UpdateCallback = std::function<void(const XR25Frame::voc_t& frame)>;

  explicit FrameProcessor(UpdateCallback cb, unsigned upd_pulse = 1000);
  ~FrameProcessor();

  unsigned feedBytes(const uint8_t* data, std::size_t data_len, bool block = false);

  void test() { xr25.test(); }

 private:
  void update_thread_fun();

 private:
  unsigned m_nmb_frames_waiting = 0;
  XR25Frame xr25;
  UpdateCallback callback = nullptr, log_callback = nullptr;
  unsigned m_upd_pulse = 0;
  std::thread m_update_thread;
  std::mutex m_update_thread_mutex;
  std::condition_variable m_update_thread_cv;
  std::condition_variable m_update_thread_cv2;
  bool m_update_thread_keep_running = false;
  int m_last_xr25_frame_number = 0;
};