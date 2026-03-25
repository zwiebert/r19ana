// FrameProcessor.cc
#include "FrameProcessor.hh"

FrameProcessor::FrameProcessor(UpdateCallback cb) : callback(std::move(cb)) {
  m_update_thread_keep_running = true;
  m_update_thread = std::thread(&FrameProcessor::update_thread_fun, this);
}

FrameProcessor::~FrameProcessor() {
  if (m_update_thread_keep_running) {
    m_update_thread_keep_running = false;
    if (m_update_thread.joinable()) m_update_thread.join();
  }
}

void FrameProcessor::update_thread_fun() {
  XR25Frame frame;
  for (; m_update_thread_keep_running;) {
    {
      std::unique_lock<std::mutex> lock(m_update_thread_mutex);
      m_update_thread_cv.wait_for(lock, std::chrono::seconds(2), [this]() {
        return m_last_xr25_frame_number < old_frame_number ||
               !m_update_thread_keep_running;
      });
      // make copies of frame data and counter while feeder thread is locked by
      // mutex
      if(m_last_xr25_frame_number >= old_frame_number) continue;
      frame = xr25;
      m_last_xr25_frame_number = old_frame_number;
    }
    if (callback) callback(frame);
  }
}

void FrameProcessor::feedBytes(const uint8_t* data, std::size_t data_len) {
  {
    std::lock_guard<std::mutex> lk(m_update_thread_mutex);
    xr25.append(data, data_len);
    if (old_frame_number >= xr25.get_frame_counter())
      return;  // current frame is not completed

    old_frame_number = xr25.get_frame_counter();
  }
  m_update_thread_cv.notify_one();
}

void FrameProcessor::feedBytes(const std::vector<uint8_t>& data) {
  return feedBytes(&data[0], data.size());
}
