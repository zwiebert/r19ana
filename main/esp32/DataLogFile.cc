#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include <atomic>

#include "DataLogFileStdio.hh"
#include "driver/sdmmc_host.h"
#include "esp_vfs_fat.h"
#include "main.hh"
#include "sd_test_io.h"
#include "sdmmc_cmd.h"
#if SOC_SDMMC_IO_POWER_EXTERNAL
#include "sd_pwr_ctrl_by_on_chip_ldo.h"
#endif

static const char* TAG = "example";

#define MOUNT_POINT "/sdcard"
#define EXAMPLE_IS_UHS1                      \
  (CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_SDR50 || \
   CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_DDR50)

#ifdef CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS
const char* names[] = {"CLK", "CMD", "D0", "D1", "D2", "D3"};
const int pins[] = {CONFIG_EXAMPLE_PIN_CLK,
                    CONFIG_EXAMPLE_PIN_CMD,
                    CONFIG_EXAMPLE_PIN_D0
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
                    ,
                    CONFIG_EXAMPLE_PIN_D1,
                    CONFIG_EXAMPLE_PIN_D2,
                    CONFIG_EXAMPLE_PIN_D3
#endif
};

const int pin_count = sizeof(pins) / sizeof(pins[0]);

#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
const int adc_channels[] = {CONFIG_EXAMPLE_ADC_PIN_CLK,
                            CONFIG_EXAMPLE_ADC_PIN_CMD,
                            CONFIG_EXAMPLE_ADC_PIN_D0
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
                            ,
                            CONFIG_EXAMPLE_ADC_PIN_D1,
                            CONFIG_EXAMPLE_ADC_PIN_D2,
                            CONFIG_EXAMPLE_ADC_PIN_D3
#endif
};
#endif  // CONFIG_EXAMPLE_ENABLE_ADC_FEATURE

pin_configuration_t config = {
    .names = names,
    .pins = pins,
#if CONFIG_EXAMPLE_ENABLE_ADC_FEATURE
    .adc_channels = adc_channels,
#endif
};
#endif  // CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS

class DataLogfileEsp32 final : public DataLogFileStdio, public IMountable {
 public:
  bool mount_fs() override {
    constexpr unsigned try_mount_interval_ms = 5000;

    if (card) return false;  // already mounted

    if (m_last_mount_try_ms) {
      auto now = esp_log_timestamp();
      if (m_last_mount_try_ms + try_mount_interval_ms > now) {
        return false;
      }
      m_last_mount_try_ms = 0;
    }

    std::lock_guard lock(m_mtx);
    esp_err_t ret;

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {};
#ifdef CONFIG_EXAMPLE_FORMAT_IF_MOUNT_FAILED
    mount_config.format_if_mount_failed = true;
#else
    mount_config.format_if_mount_failed = false;
#endif  // EXAMPLE_FORMAT_IF_MOUNT_FAILED
    mount_config.max_files = 5;
    mount_config.allocation_unit_size = 16 * 1024;
    ESP_LOGI(TAG, "Initializing SD card");

    // Use settings defined above to initialize SD card and mount FAT
    // filesystem. Note: esp_vfs_fat_sdmmc/sdspi_mount is all-in-one convenience
    // functions. Please check its source code and implement error recovery when
    // developing production applications.

    ESP_LOGI(TAG, "Using SDMMC peripheral");

    // By default, SD card frequency is initialized to SDMMC_FREQ_DEFAULT
    // (20MHz) For setting a specific frequency, use host.max_freq_khz (range
    // 400kHz - 40MHz for SDMMC) Example: for fixed frequency of 10MHz, use
    // host.max_freq_khz = 10000;
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
#if CONFIG_EXAMPLE_SDMMC_SPEED_HS
    host.max_freq_khz = SDMMC_FREQ_HIGHSPEED;
#elif CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_SDR50
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_SDR50;
    host.flags &= ~SDMMC_HOST_FLAG_DDR;
#elif CONFIG_EXAMPLE_SDMMC_SPEED_UHS_I_DDR50
    host.slot = SDMMC_HOST_SLOT_0;
    host.max_freq_khz = SDMMC_FREQ_DDR50;
#endif

    // For SoCs where the SD power can be supplied both via an internal or
    // external (e.g. on-board LDO) power supply. When using specific IO pins
    // (which can be used for ultra high-speed SDMMC) to connect to the SD card
    // and the internal LDO power supply, we need to initialize the power supply
    // first.
#if CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
    sd_pwr_ctrl_ldo_config_t ldo_config = {
        .ldo_chan_id = CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_IO_ID,
    };
    sd_pwr_ctrl_handle_t pwr_ctrl_handle = NULL;

    ret = sd_pwr_ctrl_new_on_chip_ldo(&ldo_config, &pwr_ctrl_handle);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to create a new on-chip LDO power control driver");
      goto fail;
    }
    host.pwr_ctrl_handle = pwr_ctrl_handle;
#endif

    // This initializes the slot without card detect (CD) and write protect (WP)
    // signals. Modify slot_config.gpio_cd and slot_config.gpio_wp if your board
    // has these signals.
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
#if EXAMPLE_IS_UHS1
    slot_config.flags |= SDMMC_SLOT_FLAG_UHS1;
#endif

    // Set bus width to use:
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.width = 4;
#else
    slot_config.width = 1;
#endif

    // On chips where the GPIOs used for SD card can be configured, set them in
    // the slot_config structure:
#ifdef CONFIG_SOC_SDMMC_USE_GPIO_MATRIX
    slot_config.clk = CONFIG_EXAMPLE_PIN_CLK;
    slot_config.cmd = CONFIG_EXAMPLE_PIN_CMD;
    slot_config.d0 = CONFIG_EXAMPLE_PIN_D0;
#ifdef CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
    slot_config.d1 = CONFIG_EXAMPLE_PIN_D1;
    slot_config.d2 = CONFIG_EXAMPLE_PIN_D2;
    slot_config.d3 = CONFIG_EXAMPLE_PIN_D3;
#endif  // CONFIG_EXAMPLE_SDMMC_BUS_WIDTH_4
#endif  // CONFIG_SOC_SDMMC_USE_GPIO_MATRIX

    // Enable internal pullups on enabled pins. The internal pullups
    // are insufficient however, please make sure 10k external pullups are
    // connected on the bus. This is for debug / example purpose only.
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdmmc_mount(mount_point, &host, &slot_config,
                                  &mount_config, &card);

    if (ret != ESP_OK) {
      if (ret == ESP_FAIL) {
        ESP_LOGE(TAG,
                 "Failed to mount filesystem. "
                 "If you want the card to be formatted, set the "
                 "EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
      } else {
        ESP_LOGE(TAG,
                 "Failed to initialize the card (%s). "
                 "Make sure SD card lines have pull-up resistors in place.",
                 esp_err_to_name(ret));
#ifdef CONFIG_EXAMPLE_DEBUG_PIN_CONNECTIONS
        check_sd_card_pins(&config, pin_count);
#endif
      }
      goto fail;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    // Card has been initialized, print its properties
    sdmmc_card_print_info(stdout, card);

    m_last_mount_try_ms = 0;
    return true;
  fail:
    if (m_last_mount_try_ms == 0) {
      m_last_mount_try_ms = esp_log_timestamp();
    }

    return false;
  }
  bool umount_fs() override {
    if (!card) return false;  // not mounted
    std::lock_guard lock(m_mtx);
    if (is_open()) {
      if (!close_file()) {
        m_flag_request_umount = true;
        return false;
      }
    }
    // All done, unmount partition and disable SDMMC peripheral
    esp_vfs_fat_sdcard_unmount(mount_point, card);
    ESP_LOGI(TAG, "Card unmounted");
    card = 0;

    // Deinitialize the power control driver if it was used
#if CONFIG_EXAMPLE_SD_PWR_CTRL_LDO_INTERNAL_IO
    ret = sd_pwr_ctrl_del_on_chip_ldo(pwr_ctrl_handle);
    if (ret != ESP_OK) {
      ESP_LOGE(TAG, "Failed to delete the on-chip LDO power control driver");
      return;
    }
#endif

    return true;
  }

  bool set_full_path(const char* file_name) override {
    if (!file_name) return false;
    std::lock_guard lock(m_mtx);
    return sizeof m_full_path > snprintf(m_full_path, sizeof m_full_path,
                                         "%s/%s", mount_point, file_name);
  }

  bool is_mounted() const override { return card; }

 public:
  int feed_bytes(const uint8_t* src, unsigned src_len) override {
    if (m_file_owner_task != xTaskGetCurrentTaskHandle()) {
      return false;
    }
    return DataLogFileStdio::feed_bytes(src, src_len);
  }
  bool write(const XR25Frame::voc_t& frame) override {
    if (m_file_owner_task != xTaskGetCurrentTaskHandle()) {
      return false;
    }
    return DataLogFileStdio::write(frame);
  }
  bool open_file() override {
    if (!DataLogFileStdio::open_file()) return false;
    m_file_owner_task = xTaskGetCurrentTaskHandle();
    return true;
  }
  bool close_file() override {
    if (m_file_owner_task != xTaskGetCurrentTaskHandle()) {
      m_flag_request_close = is_open();
      return false;
    }
    return DataLogFileStdio::close_file();
  }

 public:
  void service_logging() override {
    if (m_flag_request_close) {
      close_file();
      m_flag_request_close = false;
    }
  }

  void service_mounting() override {
    if (m_flag_request_umount) {
      umount_fs();
      m_flag_request_umount = is_mounted();
    }
    if (m_flag_request_mount) {
      mount_fs();
      m_flag_request_mount = !is_mounted();
    }
  }

 private:
  const char mount_point[16] = MOUNT_POINT;
  sdmmc_card_t* card = 0;
  uint32_t m_last_mount_try_ms = 0;
  TaskHandle_t m_file_owner_task = nullptr;

 public:
  void request_mount() { m_flag_request_mount = true; }
  void request_umount() { m_flag_request_umount = true; }

 private:
  std::atomic<bool> m_flag_request_close = false;
  std::atomic<bool> m_flag_request_umount = false;
  std::atomic<bool> m_flag_request_mount = true;

 public:
  bool is_ready() const override { return is_mounted(); }
};

static DataLogfileEsp32 logfile;

IFileLogger* data_logfile = &logfile;
IMountable* data_storage = &logfile;