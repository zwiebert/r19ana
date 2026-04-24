#pragma once

#include <stdint.h>

#include "XR25Frame.hh"

class IFileLogger {
 public:
  virtual ~IFileLogger() = default;
  virtual bool open_file() = 0;
  virtual bool close_file() = 0;
  virtual bool flush() = 0;
  virtual int feed_bytes(const uint8_t* src, unsigned src_len) = 0;
  virtual bool write(const XR25Frame::voc_t& voc) = 0;

  virtual bool is_open() const = 0;
  virtual bool set_full_path(const char* file_name) = 0;
  virtual const char* get_full_path() const = 0;
  virtual bool is_ready() const { return true; }

 public:
  virtual void service_logging() {}
};
