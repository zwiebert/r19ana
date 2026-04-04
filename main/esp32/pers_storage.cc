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

namespace pers_stor {

bool set_model(int model) {
  return store<int32_t, nvs_set_i32>("model", model);
}
int get_model() {
  int32_t model;
  if (restore<int32_t, nvs_get_i32>("model", model)) {
    return model > 0 ? int(model) : 0;
  }
  return 0;
}

bool set_enable_auto_mount(bool auto_mount) {
  return store<uint8_t, nvs_set_u8>("auto_mount", auto_mount);
}
bool get_enable_auto_mount() {
  uint8_t res = false;
  return restore<uint8_t, nvs_get_u8>("auto_mount", res) ? res : false;
}
}  // namespace pers_stor