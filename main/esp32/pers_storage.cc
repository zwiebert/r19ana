#include "pers_storage.hh"

#include <nvs.h>

static const char* nvs_namespace = "pers_storage";

static bool store(const char* key, int32_t val) {
  nvs_handle_t h;
  bool ret = false;
  if (ESP_OK == nvs_open(nvs_namespace, NVS_READWRITE, &h)) {
    if (ESP_OK == nvs_set_i32(h, key, val)) {
      if (ESP_OK == nvs_commit(h)) {
        ret = true;
      }
    }
    nvs_close(h);
  }
  return ret;
}
static bool restore(const char* key, int32_t& val) {
  nvs_handle_t h;
  bool ret = false;
  if (ESP_OK == nvs_open(nvs_namespace, NVS_READONLY, &h)) {
    if (ESP_OK == nvs_get_i32(h, key, &val)) {
      ret = true;
    }
    nvs_close(h);
  }
  return ret;
}

namespace pers_stor {

bool set_model(int model) { return store("model", model); }
int get_model() {
  int32_t model;
  if (restore("model", model)) {
    return int(model);
  }
  return 0;
}

}  // namespace pers_stor