#pragma once
#error "header obsolete"
#include <stdint.h>
typedef uint8_t u8;
typedef int8_t i8;
typedef uint16_t u16;
typedef int16_t i16;
typedef uint32_t u32;
typedef int32_t i32;
typedef uint64_t u64;
typedef int64_t i64;

#if defined TEST_HOST
#include "app_config/host/proj_app_cfg.h"

#elif defined MCU_ESP8266
#include "app_config/esp8266/proj_app_cfg.h"

#elif defined ESP_PLATFORM
#include "app_config/esp32/proj_app_cfg.h"

#else
#error "no supported MCU"

#endif

#include "proj_kconfig.h"
