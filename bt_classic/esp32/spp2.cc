/*
 * SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */

#include <esp_pthread.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <mutex>
#include <thread>

#include "esp_bt.h"
#include "esp_bt_device.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_log.h"
#include "esp_spp_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "sys/time.h"
#include "time.h"

#define D(x)

#define SPP_TAG "SPP_ACCEPTOR_DEMO"
#define SPP_SERVER_NAME "SPP_SERVER"
#define SPP_SHOW_DATA 0
#define SPP_SHOW_SPEED 1
#define SPP_SHOW_MODE SPP_SHOW_DATA /*Choose show mode: show data or speed*/
constexpr size_t SPP_TX_QUEUE_LEN = 10;
constexpr size_t SPP_RX_QUEUE_LEN = 10;

struct spp_msg_t {
  uint8_t* data;
  uint16_t len;
};
static QueueHandle_t spp_tx_queue = NULL;
bool tx_thread_kill;

class {
 public:
  bool store_new_msg(spp_msg_t& msg, bool free_msg_data = false) {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_bytes_left) return false;
    free_data();
    m_free_data = free_msg_data;
    m_msg = msg;
    m_bytes_left = msg.len;
    return true;
  }

  spp_msg_t get_remaining_msg() {
    std::lock_guard<std::mutex> guard(m_mutex);
    if (m_write_pending) return spp_msg_t{};
    m_write_pending = true;
    return spp_msg_t{.data = m_msg.data + (m_msg.len - m_bytes_left),
                     .len = m_bytes_left};
  }

  void write_has_failed() {
    std::lock_guard<std::mutex> guard(m_mutex);
    assert(m_write_pending);
    m_write_pending = false;
  }

  uint16_t number_of_bytes_written(uint16_t n) {
    std::lock_guard<std::mutex> guard(m_mutex);
    assert(m_bytes_left >= n);
    m_write_pending = false;
    return (m_bytes_left -= n);
  }

  void clear() {
    std::lock_guard<std::mutex> guard(m_mutex);
    free_data();
    m_msg = spp_msg_t{};
    m_bytes_left = 0;
  }

  bool is_empty() {
    std::lock_guard<std::mutex> guard(m_mutex);
    return m_bytes_left == 0;
  }

 private:
  void free_data() {
    if (m_free_data) free(m_msg.data);
    m_msg.data = 0;
  }

 private:
  std::mutex m_mutex;
  spp_msg_t m_msg = {};
  uint16_t m_bytes_left = 0;
  bool m_free_data = true;
  bool m_write_pending = false;
} our_msg;

static QueueHandle_t spp_rx_queue = NULL;

static const char local_device_name[] = CONFIG_EXAMPLE_LOCAL_DEVICE_NAME;
static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
static const bool esp_spp_enable_l2cap_ertm = true;

static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;

constexpr auto SPP_READY_TO_WRITE_BIT = BIT0;
constexpr auto SPP_TX_QUEUE_NOT_EMPTY = BIT1;
constexpr auto SPP_TX_QUEUE_NOT_FULL = BIT2;
constexpr auto SPP_RX_QUEUE_NOT_EMPTY = BIT3;
constexpr auto SPP_RX_QUEUE_NOT_FULL = BIT4;
constexpr auto SPP_IS_OPEN = BIT5;
static EventGroupHandle_t spp_event_group;
static uint32_t spp_handle = 0;

static void clean_up() {
  our_msg.clear();
  xEventGroupClearBits(spp_event_group,
                       SPP_READY_TO_WRITE_BIT | SPP_TX_QUEUE_NOT_EMPTY |
                           SPP_RX_QUEUE_NOT_EMPTY | SPP_IS_OPEN);
  xEventGroupSetBits(spp_event_group,
                     SPP_TX_QUEUE_NOT_FULL | SPP_RX_QUEUE_NOT_FULL);
  xQueueReset(spp_tx_queue);
  xQueueReset(spp_rx_queue);
  spp_handle = 0;
}

static char* bda2str(uint8_t* bda, char* str, size_t size) {
  if (bda == NULL || str == NULL || size < 18) {
    return NULL;
  }

  uint8_t* p = bda;
  sprintf(str, "%02x:%02x:%02x:%02x:%02x:%02x", p[0], p[1], p[2], p[3], p[4],
          p[5]);
  return str;
}

std::thread spp_tx_thread;
void spp_tx_thread_fun();

static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t* param) {
  char bda_str[18] = {0};

  switch (event) {
    case ESP_SPP_INIT_EVT:
      if (param->init.status == ESP_SPP_SUCCESS) {
        D(ESP_LOGI(SPP_TAG, "ESP_SPP_INIT_EVT"));
        esp_spp_start_srv(sec_mask, role_slave, 0, SPP_SERVER_NAME);
      } else {
        D(ESP_LOGE(SPP_TAG, "ESP_SPP_INIT_EVT status:%d", param->init.status));
      }
      break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
      D(ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT"));
      break;
    case ESP_SPP_OPEN_EVT:
      D(ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT"));
      break;
    case ESP_SPP_CLOSE_EVT:
      clean_up();
      ESP_LOGI(SPP_TAG,
               "ESP_SPP_CLOSE_EVT status:%d handle:%" PRIu32
               " close_by_remote:%d",
               param->close.status, param->close.handle, param->close.async);
      break;
    case ESP_SPP_START_EVT:
      if (param->start.status == ESP_SPP_SUCCESS) {
        ESP_LOGI(SPP_TAG,
                 "ESP_SPP_START_EVT handle:%" PRIu32 " sec_id:%d scn:%d",
                 param->start.handle, param->start.sec_id, param->start.scn);
        esp_bt_gap_set_device_name(local_device_name);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE,
                                 ESP_BT_GENERAL_DISCOVERABLE);
      } else {
        D(ESP_LOGE(SPP_TAG, "ESP_SPP_START_EVT status:%d",
                   param->start.status));
      }
      break;

    case ESP_SPP_CL_INIT_EVT:
      D(ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT"));
      break;

    case ESP_SPP_DATA_IND_EVT: {
      if (xEventGroupGetBits(spp_event_group) & SPP_RX_QUEUE_NOT_FULL) {
        spp_msg_t msg = {(uint8_t*)malloc(param->data_ind.len + 1),
                         param->data_ind.len};
        if (!msg.data) break;
        memcpy(msg.data, param->data_ind.data, msg.len);
        msg.data[msg.len] = 0;
        if (pdTRUE == xQueueSend(spp_rx_queue, (void*)&msg, TickType_t(10))) {
          xEventGroupSetBits(spp_event_group, SPP_RX_QUEUE_NOT_EMPTY);
        } else {
          xEventGroupSetBits(spp_event_group, SPP_RX_QUEUE_NOT_EMPTY);
          xEventGroupClearBits(spp_event_group, SPP_RX_QUEUE_NOT_FULL);
        }
      }
      /*
       * We only show the data in which the data length is less than 128 here.
       * If you want to print the data and the data rate is high, it is strongly
       * recommended to process them in other lower priority application task
       * rather than in this callback directly. Since the printing takes too
       * much time, it may stuck the Bluetooth stack and also have a effect on
       * the throughput!
       */
      D(ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT len:%d handle:%" PRIu32,
                 param->data_ind.len, param->data_ind.handle));
      if (param->data_ind.len < 128) {
        D(ESP_LOG_BUFFER_HEX("", param->data_ind.data, param->data_ind.len));
      }
      break;
    }
    case ESP_SPP_CONG_EVT:
      // Explicit congestion status change
      if (param->cong.cong) {
        xEventGroupClearBits(spp_event_group, SPP_READY_TO_WRITE_BIT);
      } else {
        xEventGroupSetBits(spp_event_group, SPP_READY_TO_WRITE_BIT);
      }
      D(ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT"));
      break;
    case ESP_SPP_WRITE_EVT:
      if (param->write.status == ESP_SPP_SUCCESS) {
        auto bytes_left = our_msg.number_of_bytes_written(param->write.len);
        D(ESP_LOGW(SPP_TAG,
                   "ESP_SPP_WRITE_EVT success. status:%d, bytes_left:%u",
                   param->write.status, bytes_left));
      } else {
        /* Means the previous data packet is not sent at all, need to send the
         * whole data packet again. */
        D(ESP_LOGE(SPP_TAG, "ESP_SPP_WRITE_EVT status:%d",
                   param->write.status));
      }
      // Check if the stack became congested after this write
      if (param->write.cong) {
        D(ESP_LOGW(SPP_TAG, "ESP_SPP_WRITE_EVT cong. clear tx-ready bit"));
        xEventGroupClearBits(spp_event_group, SPP_READY_TO_WRITE_BIT);
      } else {
        D(ESP_LOGW(SPP_TAG, "ESP_SPP_WRITE_EVT no-cong. set tx-ready bit"));
        xEventGroupSetBits(spp_event_group, SPP_READY_TO_WRITE_BIT);
      }
      break;
    case ESP_SPP_SRV_OPEN_EVT:
      clean_up();
      spp_handle = param->srv_open.handle;
      // Connection opened, we are initially ready to send
      xEventGroupSetBits(spp_event_group, SPP_IS_OPEN | SPP_READY_TO_WRITE_BIT);
      ESP_LOGI(SPP_TAG,
               "ESP_SPP_SRV_OPEN_EVT status:%d handle:%" PRIu32
               ", rem_bda:[%s]",
               param->srv_open.status, param->srv_open.handle,
               bda2str(param->srv_open.rem_bda, bda_str, sizeof(bda_str)));
      break;
    case ESP_SPP_SRV_STOP_EVT:
      D(ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_STOP_EVT"));
      break;
    case ESP_SPP_UNINIT_EVT:
      D(ESP_LOGI(SPP_TAG, "ESP_SPP_UNINIT_EVT"));
      break;
    default:
      break;
  }
}

void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t* param) {
  char bda_str[18] = {0};

  switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT: {
      if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
        ESP_LOGI(SPP_TAG, "authentication success: %s bda:[%s]",
                 param->auth_cmpl.device_name,
                 bda2str(param->auth_cmpl.bda, bda_str, sizeof(bda_str)));
      } else {
        ESP_LOGE(SPP_TAG, "authentication failed, status:%d",
                 param->auth_cmpl.stat);
      }
      break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT: {
      ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d",
               param->pin_req.min_16_digit);
      if (param->pin_req.min_16_digit) {
        D(ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000"));
        esp_bt_pin_code_t pin_code = {0};
        esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
      } else {
        D(ESP_LOGI(SPP_TAG, "Input pin code: 1234"));
        esp_bt_pin_code_t pin_code;
        pin_code[0] = '1';
        pin_code[1] = '2';
        pin_code[2] = '3';
        pin_code[3] = '4';
        esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
      }
      break;
    }

#if (CONFIG_EXAMPLE_SSP_ENABLED == true)
    case ESP_BT_GAP_CFM_REQ_EVT:
      ESP_LOGI(
          SPP_TAG,
          "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %06" PRIu32,
          param->cfm_req.num_val);
      esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
      break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
      ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%06" PRIu32,
               param->key_notif.passkey);
      break;
    case ESP_BT_GAP_KEY_REQ_EVT:
      D(ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!"));
      break;
#endif

    case ESP_BT_GAP_MODE_CHG_EVT:
      ESP_LOGI(SPP_TAG, "ESP_BT_GAP_MODE_CHG_EVT mode:%d bda:[%s]",
               param->mode_chg.mode,
               bda2str(param->mode_chg.bda, bda_str, sizeof(bda_str)));
      break;

    default: {
      D(ESP_LOGI(SPP_TAG, "event: %d", event));
      break;
    }
  }
  return;
}

void spp2_main(void) {
  char bda_str[18] = {0};
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));

  esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
  if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
    ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
    ESP_LOGE(SPP_TAG, "%s enable controller failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  esp_bluedroid_config_t bluedroid_cfg = BT_BLUEDROID_INIT_CONFIG_DEFAULT();
#if (CONFIG_EXAMPLE_SSP_ENABLED == false)
  bluedroid_cfg.ssp_en = false;
#endif
  if ((ret = esp_bluedroid_init_with_cfg(&bluedroid_cfg)) != ESP_OK) {
    ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  if ((ret = esp_bluedroid_enable()) != ESP_OK) {
    ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
    ESP_LOGE(SPP_TAG, "%s gap register failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
    ESP_LOGE(SPP_TAG, "%s spp register failed: %s", __func__,
             esp_err_to_name(ret));
    return;
  }

  esp_spp_cfg_t bt_spp_cfg = {
      .mode = esp_spp_mode,
      .enable_l2cap_ertm = esp_spp_enable_l2cap_ertm,
      .tx_buffer_size = 0, /* Only used for ESP_SPP_MODE_VFS mode */
  };
  if ((ret = esp_spp_enhanced_init(&bt_spp_cfg)) != ESP_OK) {
    D(ESP_LOGE(SPP_TAG, "%s spp init failed: %s", __func__,
               esp_err_to_name(ret)));
    return;
  }

#if (CONFIG_EXAMPLE_SSP_ENABLED == true)
  /* Set default parameters for Secure Simple Pairing */
  esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
  esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
  esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
#endif

  /*
   * Set default parameters for Legacy Pairing
   * Use variable pin, input pin code when pairing
   */
  esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
  esp_bt_pin_code_t pin_code;
  esp_bt_gap_set_pin(pin_type, 0, pin_code);

  ESP_LOGI(
      SPP_TAG, "Own address:[%s]",
      bda2str((uint8_t*)esp_bt_dev_get_address(), bda_str, sizeof(bda_str)));

  spp_tx_queue = xQueueCreate(SPP_TX_QUEUE_LEN, sizeof(spp_msg_t));
  spp_event_group = xEventGroupCreate();
  spp_rx_queue = xQueueCreate(SPP_RX_QUEUE_LEN, sizeof(spp_msg_t));

  auto cfg = esp_pthread_get_default_config();
  esp_pthread_set_cfg(&cfg);
  spp_tx_thread = std::thread(spp_tx_thread_fun);
}

bool spp_rx_dequeue(uint8_t*& data, size_t& len, bool block) {
  spp_msg_t spp_msg_in = {};
  do {
    if (xQueueReceive(spp_rx_queue, (void*)&spp_msg_in,
                      block ? portMAX_DELAY : TickType_t(10)) == pdFALSE) {
      xEventGroupClearBits(spp_event_group, SPP_RX_QUEUE_NOT_EMPTY);
      continue;
    }

    data = spp_msg_in.data;
    len = spp_msg_in.len;
    return true;
  } while (block);
  return false;
}

bool spp_tx_enqueue(uint8_t* data, size_t len, bool block) {
  if (!spp_handle) return false;  // we are not connected
  do {
    xEventGroupWaitBits(spp_event_group, (SPP_TX_QUEUE_NOT_FULL), pdFALSE,
                        pdTRUE, portMAX_DELAY);

    spp_msg_t msg = {data, uint16_t(len)};
    if (pdTRUE == xQueueSend(spp_tx_queue, (void*)&msg, TickType_t(10))) {
      xEventGroupSetBits(spp_event_group, SPP_TX_QUEUE_NOT_EMPTY);
      D(ESP_LOGI(SPP_TAG, "%s: msg appended to fifo, event_bits: %x", __func__,
                 xEventGroupGetBits(spp_event_group)));
      vTaskDelay(pdMS_TO_TICKS(10));
      return true;
    }
    xEventGroupClearBits(spp_event_group, SPP_TX_QUEUE_NOT_FULL);
    D(ESP_LOGE(SPP_TAG, "%s: fifo full, event_bits: %x", __func__,
               xEventGroupGetBits(spp_event_group)));
  } while (block);
  return false;
}

void spp_tx_thread_fun() {
  for (; !tx_thread_kill; vTaskDelay(pdMS_TO_TICKS(10))) {
    xEventGroupWaitBits(
        spp_event_group,
        (SPP_IS_OPEN | SPP_READY_TO_WRITE_BIT | SPP_TX_QUEUE_NOT_EMPTY), pdFALSE,
        pdTRUE, portMAX_DELAY);
    {

      // if needed, try to get a new message from queue
      if (our_msg.is_empty()) {
        spp_msg_t msg = {};
        if (xQueueReceive(spp_tx_queue, (void*)&msg, TickType_t(10)) ==
            pdFALSE) {
          xEventGroupClearBits(spp_event_group, SPP_TX_QUEUE_NOT_EMPTY);
          continue; // no messages in queue
        }
        xEventGroupSetBits(spp_event_group, SPP_TX_QUEUE_NOT_FULL);
        our_msg.store_new_msg(msg, true);
      }
      // send (wohle or partial) message 
      if (auto msg = our_msg.get_remaining_msg(); msg.len) {
        if (ESP_OK != esp_spp_write(spp_handle, int(msg.len), msg.data)) {
          our_msg.write_has_failed(); // clear the internal has-been-sent-flag
        }
      }
    }
  }
}

bool spp_is_connected() { return !!spp_handle; }

bool spp_tx_enqueue(const uint8_t* data, size_t data_len, bool block) {
  if (uint8_t* ptr = (uint8_t*)malloc(data_len)) {
    memcpy(ptr, data, data_len);
    return spp_tx_enqueue(ptr, data_len, block);
  }
  return false;
}

bool spp_tx_enqueue(const char* data_str, bool block) {
  return spp_tx_enqueue((const uint8_t*)data_str, strlen(data_str), block);
}