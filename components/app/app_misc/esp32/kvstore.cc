/*
 * store.c
 *
 *  Created on: 12.06.2019
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_err.h"

#include "app_misc/kvstore.h"

static const char *nvs_namespace = "kvs";

bool kvs_store_string(const char *key, const char *val) {
  bool result = false;
  nvs_handle handle;

  if (auto err = nvs_open(nvs_namespace, NVS_READWRITE, &handle); err == ESP_OK) {
    result = nvs_set_str(handle, key, val) == ESP_OK && nvs_commit(handle) == ESP_OK;
    nvs_close(handle);
  }
  return result;;
}

bool kvs_get_string(const char *key, char *buf, size_t buf_size) {
  bool result = false;
  nvs_handle handle;

  if (auto err = nvs_open(nvs_namespace, NVS_READONLY, &handle); err == ESP_OK) {
    size_t len = buf_size;
    result = nvs_get_str(handle, key, buf, &len) == ESP_OK;
    nvs_close(handle);
  }
  return result;
}

bool kvs_store_int32(const char *key, i32 val) {
  bool result = false;
  nvs_handle handle;

  if (auto err = nvs_open(nvs_namespace, NVS_READWRITE, &handle); err == ESP_OK) {
    result = nvs_set_i32(handle, key, val) == ESP_OK && nvs_commit(handle) == ESP_OK;
    nvs_close(handle);
  }
  return result;
}

bool kvs_get_int32(const char *key, i32 *dest) {
  bool result = false;
  nvs_handle handle;

  if (auto err = nvs_open(nvs_namespace, NVS_READONLY, &handle); err == ESP_OK) {
    result = nvs_get_i32(handle, key, dest) == ESP_OK;
    nvs_close(handle);
  }
  return result;
}
