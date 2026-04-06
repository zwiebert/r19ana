/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "app_config/options.hh"
#include "so_out.h"
#include "so_print.h"
//#include "app_uout/callbacks.h"
#include "app_misc/common.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "uout/uout_builder_json.hh"
#include "app_uout/status_output.h"
#include <app_uout/so_config.h>
#include <utils_time/run_time.h>

//#include "utils_misc/int_macros.h"

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.uout"

void soCfg_RTC(class UoutWriter &td) {
  char buf[64];
  if (rtc_get_by_string(buf)) {
    td.so().print(otok_strings[(int)otok::k_rtc], buf);
  }
}

void soCfg_NETWORK(class UoutWriter &td) {
#ifdef CONFIG_APP_USE_NETWORK
  td.so().print(comp_sett.get_optKeyStr(CB_NETWORK_CONNECTION),cfg_args_network[config_read_network_connection()]);
#endif
}

void soCfg_TZ(class UoutWriter &td) {
#ifdef USE_POSIX_TIME
  char buf[64];
  td.so().print(comp_sett.get_optKeyStr(CB_TZ),config_read_tz(buf, sizeof buf));
#endif
}

void soCfg_LONGITUDE(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_LONGITUDE), config_read_longitude(), 2);
}

void soCfg_LATITUDE(class UoutWriter &td) {
  td.so().print(comp_sett.get_optKeyStr(CB_LATITUDE), config_read_latitude(), 2);
}

void soCfg_GPIO_PIN(class UoutWriter &td, const int gpio_number) {
#ifdef ACCESS_GPIO
  {
    char buf[64];
    char key[10] = "gpio";
    STRCPY(key + 4, itoa(gpio_number, buf, 10));
    char ps[2] = "x";
    if (is_gpio_number_usable(gpio_number, true)) {
      enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
      ps[0] = pin_state_args[mps];
    }
    td.so().print(key, ps);
  }
#endif
}

void soCfg_GPIO_MODES(class UoutWriter &td) {
#ifdef ACCESS_GPIO
  {
    char buf[64];
    int gpio_number;
    char key[10] = "gpio";
    char pin_level_args[] = "mhl";
    for (gpio_number = 0; gpio_number < CONFIG_GPIO_SIZE; ++gpio_number) {
      STRCPY(key + 4, itoa(gpio_number, buf, 10));
      char ps[3] = "x";
      if (is_gpio_number_usable(gpio_number, true)) {
        enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
        if (mps != PIN_DEFAULT) {
          enum mcu_pin_level mpl = pin_getPinLevel(gpio_number);
          ps[0] = pin_mode_args[mps];
          ps[1] = pin_level_args[3 - mpl];
          td.so().print(key, ps);
        }
      }

    }
  }
#endif
}

void soCfg_GPIO_MODES_AS_STRING(class UoutWriter &td) {
#ifdef ACCESS_GPIO
  {
    int gpio_number;
    char val[CONFIG_GPIO_SIZE + 1];
    val[CONFIG_GPIO_SIZE] = '\0';
    for (gpio_number = 0; gpio_number < CONFIG_GPIO_SIZE; ++gpio_number) {
      if (is_gpio_number_usable(gpio_number, true)) {
        enum mcu_pin_mode mps = pin_getPinMode(gpio_number);
        val[gpio_number] = pin_mode_args[mps];
      } else {
        val[gpio_number] = 'x';
      }
    }
    td.so().print("gpio", val);
  }
#endif
}





/*
 * status_output.c
 *
 *  Created on: 13.03.2019
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "app_config/options.hh"
#include <app_uout/so_config.h>
#include "so_out.h"
#include "so_print.h"
#include "stdint.h"
#include "app_misc/firmware.h"
#include "app_misc/rtc.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "net/ipnet.h"
#include "txtio/inout.h"
#include "txtio/comp_glue.hh"
#include "uout/uout_builder_json.hh"
#include "app_uout/status_output.h"

#include <app_uout/so_config.h>

#ifdef CONFIG_NET_HTTP_SERVER_HAVE_COMPONENT
#include <net_http_server/comp_glue.hh>
#endif
#ifdef CONFIG_NET_HAVE_COMPONENT
#include <net/comp_glue.hh>
#endif
#ifdef CONFIG_NET_MQTT_CLIENT_HAVE_COMPONENT
#include <net_mqtt_client/comp_glue.hh>
#endif
#ifdef CONFIG_STM32_HAVE_COMPONENT
#include <stm32/comp_glue.hh>
#endif

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.uout"

void soCfg_begin(class UoutWriter &td) {
  td.so().x_open("config");
}

void soCfg_end(class UoutWriter &td) {
  td.so().x_close();
}

void soCfg_all_net(class UoutWriter &td) {
  soCfg_NETWORK(td);
#ifdef CONFIG_APP_USE_WLAN
  soCfg_WLAN_SSID(td);
  soCfg_WLAN_PASSWORD(td);
#endif
#ifdef CONFIG_APP_USE_NTP
  soCfg_NTP_SERVER(td);
#endif
#ifdef CONFIG_APP_USE_LAN
  soCfg_LAN_PHY(td);
  soCfg_LAN_PWR_GPIO(td);
#endif
#ifdef CONFIG_APP_USE_MQTT
  soCfg_MQTT_ENABLE(td);
  soCfg_MQTT_URL(td);
  soCfg_MQTT_USER(td);
  soCfg_MQTT_PASSWORD(td);
  soCfg_MQTT_CLIENT_ID(td);
  soCfg_MQTT_ROOT_TOPIC(td);
#endif
#ifdef CONFIG_APP_USE_HTTP
  soCfg_HTTP_ENABLE(td);
  soCfg_HTTP_USER(td);
  soCfg_HTTP_PASSWORD(td);
#endif
}


void soCfg_all_time(class UoutWriter &td) {
  soCfg_RTC(td);
#ifdef USE_POSIX_TIME
  soCfg_TZ(td);
#endif
}

void soCfg_all_rv(class UoutWriter &td) {
#ifdef CONFIG_STM32_HAVE_COMPONENT
  soCfg_STM32_BOOTGPIO_INV(td);
#endif
}


void soCfg_all(class UoutWriter &td) {
  soCfg_VERBOSE(td);

  soCfg_all_time(td);
  soCfg_all_net(td);
  soCfg_all_rv(td);
}

