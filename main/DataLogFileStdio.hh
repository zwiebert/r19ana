#pragma once

#include <stdio.h>
#include <string.h>

#include "DataLogFile.hh"
#include "main.hh"

class DataLogFileStdio : public DataLogFile {
 public:
  bool open_file() override {
    if (m_file) return false;

    m_file = fopen(get_full_path(), "wb");
    return m_file;
  }
  bool close_file() override {
    if (!m_file) return false;
    bool res = fclose(m_file) == 0;
    m_file = nullptr;
    return res;
  }

  int feed_bytes(const uint8_t* src, unsigned src_len) override {
    if (!m_file) return -1;
    return fwrite(src, sizeof *src, src_len, m_file);
  }

  bool write(const XR25Frame::voc_t& frame) override {
    if (!m_file) return -1;

    if (putc('\xff', m_file) == EOF) return false;
    if (putc('\x00', m_file) == EOF) return false;

    for (unsigned i = 0; i < frame.frame_len; ++i) {
      auto db = frame.frame[i];
      if (putc(db, m_file) == EOF) return false;
      if (db == 0xff)
        if (putc(db, m_file) == EOF) return false;
    }
    return true;
  }

  virtual bool set_full_path(const char* file_name) override {
    if (!file_name) return false;
    return sizeof m_full_path <
           snprintf(m_full_path, sizeof m_full_path, "/tmp/%s", file_name);
  }
  virtual const char* get_full_path() override { return m_full_path; }

 private:
  FILE* m_file = 0;

 protected:
  char m_full_path[32] = {};
};
