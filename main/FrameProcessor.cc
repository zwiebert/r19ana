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

    m_update_thread_cv2.notify_one();
    if (callback) callback(frame);
  }
}

unsigned FrameProcessor::feedBytes(const uint8_t* data, const size_t data_len,
                                   bool block) {
  unsigned len = 0;

  do {
    {
      std::unique_lock<std::mutex> lock(m_update_thread_mutex);
      if (len && m_nmb_frames_waiting == XR25Frame::RINGBUFFER_LENGTH) {
        m_update_thread_cv2.wait_for(lock, std::chrono::seconds(2), [this]() {
          return m_nmb_frames_waiting < XR25Frame::RINGBUFFER_LENGTH;
        });
      }
      len += xr25.append(data + len, data_len - len);
      m_nmb_frames_waiting = xr25.get_buffered_frame_count();
    }
    m_update_thread_cv.notify_one();
  } while (block && len < data_len);
  return len;
}
