#pragma once

#include <stdint.h>
#include "XR25Frame.hh"

class DataLogFile {
 public:
  virtual ~DataLogFile() = default;
  virtual bool mount_fs() { return true; }
  virtual bool umount_fs() { return true; } 
  virtual bool open_file() = 0;
  virtual bool close_file() = 0;
  virtual int feed_bytes(const uint8_t* src, unsigned src_len) = 0;
  virtual bool write(const XR25Frame::voc_t& frame) = 0;

  virtual bool set_full_path(const char *file_name) = 0;
  virtual const char *get_full_path() = 0;
protected:
};