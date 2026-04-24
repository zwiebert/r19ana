// FrameProcessor.cc
#include "FrameProcessor.hh"
#ifdef ESP_PLATFORM
 #include <esp_pthread.h>
#endif

FrameProcessor::FrameProcessor(UpdateCallback cb, unsigned upd_pulse_ms)
    : callback(std::move(cb)), m_upd_pulse(upd_pulse_ms) {
  m_update_thread_keep_running = true;
#ifdef ESP_PLATFORM
    auto cfg = esp_pthread_get_default_config();
    cfg.stack_size = 8192;
    esp_pthread_set_cfg(&cfg);
#endif
  m_update_thread = std::thread(&FrameProcessor::update_thread_fun, this);
}

FrameProcessor::~FrameProcessor() {
  if (m_update_thread_keep_running) {
    m_update_thread_keep_running = false;
    if (m_update_thread.joinable()) m_update_thread.join();
  }
}

void FrameProcessor::update_thread_fun() {
  XR25Frame::voc_t voc;
  for (; m_update_thread_keep_running;) {
    {
      std::unique_lock<std::mutex> lock(m_update_thread_mutex);
      if (!(m_nmb_frames_waiting = xr25.get_buffered_frame_count()))
        m_update_thread_cv.wait_for(
            lock, std::chrono::milliseconds(m_upd_pulse), [voc, this]() {
              return m_nmb_frames_waiting || !m_update_thread_keep_running;
            });
      // make copies of packet data and counter while feeder thread is locked by
      // mutex
      if (!m_nmb_frames_waiting || !xr25.pull_voc(voc)) {
        // no packet is available, so mark our copy as empty.
        voc.packet_len = 0;
      }
    }
    if (voc.packet_len) {
      m_update_thread_cv2.notify_one();
    }

    if (callback) callback(voc);
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
