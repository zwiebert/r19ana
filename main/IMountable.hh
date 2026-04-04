#pragma once

#include <stdint.h>

class IMountable {
 public:
  virtual ~IMountable() = default;

 public:
  virtual bool mount_fs() { return true; }
  virtual bool umount_fs() { return true; }
  virtual bool is_mounted() const { return true; }

  virtual void request_mount() {}
  virtual void request_umount() {}

 public:
  virtual void service_mounting() {}
};