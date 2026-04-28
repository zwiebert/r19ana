#pragma once

#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include <mutex>

#include "IFileLogger.hh"
#include "main.hh"

class DataLogFileStdio : public IFileLogger {
 public:
  virtual bool open_file() override {
    if (!m_full_path_valid) return false;
    if (m_file) return false;

    m_file = fopen(get_full_path(), "ab");
    return m_file;
  }
  virtual bool close_file() override {
    if (!m_file) return false;
    bool res = fclose(m_file) == 0;
    m_file = nullptr;
    return res;
  }
  virtual bool flush() override {
    if (!m_file) return false;
    return 0 == fflush(m_file);
  }

  virtual bool ls_files(puts_cb_t puts_cb, bool json, const char* dirname,
                        const char* suffix) override {
    if (DIR* dir = opendir(dirname)) {
      for (errno = 0; dirent* de = readdir(dir); errno = 0) {
        char buf[256];
        if (suffix &&
            strcasecmp(de->d_name + strlen(de->d_name) - strlen(suffix),
                       suffix) != 0)
          continue;
        if (sizeof buf > snprintf(buf, sizeof buf, "%s\n", de->d_name)) {
          puts_cb(buf);
        }
      }
      if (errno) {
        // readdir error
      }
      closedir(dir);
    }

    return false;
  }
  virtual int feed_bytes(const uint8_t* src, unsigned src_len) override {
    if (!m_file) return -1;
    return fwrite(src, sizeof *src, src_len, m_file);
  }

  virtual bool write(const XR25Frame::voc_t& voc) override {
    if (!m_file) return -1;

    if (putc('\xff', m_file) == EOF) return false;
    if (putc('\x00', m_file) == EOF) return false;

    for (unsigned i = 0; i < voc.packet_len; ++i) {
      auto db = voc.packet[i];
      if (putc(db, m_file) == EOF) return false;
      if (db == 0xff)
        if (putc(db, m_file) == EOF) return false;
    }
    return true;
  }
  virtual bool is_open() const override { return m_file; }
  virtual bool set_full_path(const char* file_name) override {
    if (!file_name) return false;
    if (sizeof m_full_path <=
        snprintf(m_full_path, sizeof m_full_path, "/tmp/%s", file_name))
      return false;
    m_full_path_valid = true;
    return true;
  }
  virtual const char* get_full_path() const override { return m_full_path; }

 private:
  FILE* m_file = 0;

 protected:
  char m_full_path[32] = {};
  bool m_full_path_valid = false;
  std::mutex m_mtx;
};
