#include "pers_storage.hh"

#include <nvs.h>

static const char* nvs_namespace = "pers_storage";

template <typename T, esp_err_t (*nvs_set_T)(nvs_handle_t handle,
                                             const char* key, T in_value)>
static bool store(const char* key, T val) {
  nvs_handle_t h;
  bool ret = false;
  if (ESP_OK == nvs_open(nvs_namespace, NVS_READWRITE, &h)) {
    if (ESP_OK == nvs_set_T(h, key, val)) {
      if (ESP_OK == nvs_commit(h)) {
        ret = true;
      }
    }
    nvs_close(h);
  }
  return ret;
}

template <typename T, esp_err_t (*nvs_get_T)(nvs_handle_t c_handle,
                                             const char* key, T* out_value)>
static bool restore(const char* key, T& val) {
  nvs_handle_t h;
  bool ret = false;
  if (ESP_OK == nvs_open(nvs_namespace, NVS_READONLY, &h)) {
    if (ESP_OK == nvs_get_T(h, key, &val)) {
      ret = true;
    }
    nvs_close(h);
  }
  return ret;
}

static int restore_str(char* dst, unsigned dst_len, const char* key) {
  nvs_handle_t h;
  size_t len = dst_len;
  bool success = false;

  if (ESP_OK == nvs_open(nvs_namespace, NVS_READONLY, &h)) {
    if (ESP_OK == nvs_get_str(h, key, dst, &len)) {
      success = true;
    }
    nvs_close(h);
  }
  if (!success) return -1;
  return int(len);
}

namespace pers_stor {

bool set_model(int model) {
  return store<int32_t, nvs_set_i32>("model", model);
}
int get_model(int default_val) {
  int32_t model;
  return restore<int32_t, nvs_get_i32>("model", model) && 0 < model
             ? int(model)
             : default_val;
}

bool set_enable_auto_mount(bool auto_mount) {
  return store<uint8_t, nvs_set_u8>("auto_mount", auto_mount);
}
bool get_enable_auto_mount(bool default_val) {
  uint8_t res = false;
  return restore<uint8_t, nvs_get_u8>("auto_mount", res) ? res : default_val;
}

bool set_enable_logging(bool enable) {
  return store<uint8_t, nvs_set_u8>("log_enabled", enable);
}
bool get_enable_logging(bool default_val) {
  uint8_t res = false;
  return restore<uint8_t, nvs_get_u8>("log_enabled", res) ? res : default_val;
}

bool set_log_file_name(const char* name) {
  return store<const char*, nvs_set_str>("log_name", name);
}

std::string get_log_file_name(const char* default_val) {
  char buf[64];
  return (0 < restore_str(buf, sizeof buf, "log_name")) ? buf : default_val;
}

}  // namespace pers_stor