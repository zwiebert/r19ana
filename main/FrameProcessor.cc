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
  XR25Frame::voc_t frame;
  for (; m_update_thread_keep_running;) {
    {
      std::unique_lock<std::mutex> lock(m_update_thread_mutex);
      if (!(m_nmb_frames_waiting = xr25.get_buffered_frame_count()))
        m_update_thread_cv.wait_for(
            lock, std::chrono::seconds(2), [frame, this]() {
              return m_nmb_frames_waiting || !m_update_thread_keep_running;
            });
      // make copies of frame data and counter while feeder thread is locked by
      // mutex
      if (!m_nmb_frames_waiting) continue;
      if (!xr25.pull_voc(frame)) continue;
    }
    if (callback) callback(frame);
  }
}

void FrameProcessor::feedBytes(const uint8_t* data, const size_t data_len) {
  {
    std::lock_guard<std::mutex> lk(m_update_thread_mutex);
    xr25.append(data, data_len);
    m_nmb_frames_waiting = xr25.get_buffered_frame_count();
  }
  m_update_thread_cv.notify_one();
}

void FrameProcessor::feedBytes(const std::vector<uint8_t>& data) {
  return feedBytes(&data[0], data.size());
}
