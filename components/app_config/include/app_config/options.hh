/**
 * \file      app_config/options.hh
 * \brief     defines strings used in user interface
 */

#pragma once
#include "proj_app_cfg.h"
#include <stdint.h>

typedef const char *const_cstringT;

#ifdef ACCESS_GPIO
constexpr const_cstringT pin_mode_args = "diqQoO";
constexpr const_cstringT pin_level_args = "lh";
constexpr const_cstringT pin_state_args = "?01t";
#endif

#ifdef CONFIG_APP_USE_NETWORK
using cfg_args_networkT = const char *[4];
constexpr cfg_args_networkT cfg_args_network = { "none", "wlan", "ap", "lan" };
#endif

#ifdef CONFIG_APP_USE_LAN
using cfg_args_lanPhyT = const char *[4];
constexpr cfg_args_lanPhyT cfg_args_lanPhy = { "none", "lan8270", "rtl8201", "ip101", };
#endif


typedef int8_t otokBaseT;
/// \brief options as enum (matching the string options in @ref otok_strings  )
enum class otok : otokBaseT  {
  NONE = -1,  // not existing
      /////////// begin of config keys //////////////////
      k_rtc,//
      k_network, k_wlan_ssid, k_wlan_password, k_lan_phy, k_lan_pwr_gpio, //
      k_ntp_server, k_longitude, k_latitude,  k_tz, k_verbose, //
      k_mqtt_enable, k_mqtt_url, k_mqtt_user, k_mqtt_password, k_mqtt_root_topic, k_mqtt_client_id, //
      k_http_enable, k_http_user, k_http_password, //
      k_stm32_bootgpio_inv, //
      k_restart, k_all, //
      ///////////// end of config keys /////////////////

  SIZE // key array size
};
#if 0
using cfg_keysT = const char *[SO_CFG_size];
constexpr cfg_keysT cfg_keys = { "rtc", "network", "wlan-ssid", "wlan-password", "lan-phy", "lan-pwr-gpio", "ntp-server", "tz", "verbose", "mqtt-enable",
    "mqtt-url", "mqtt-user", "mqtt-password", "mqtt-root-topic", "mqtt-client-id", "http-enable", "http-user", "http-password", "stm32-bootgpio-inv" };
#endif

/// \brief options as null terminated strings (matching the enum options in @ref otok  )
constexpr const_cstringT otok_strings[] = {
     /////////// begin of config keys //////////////////
    "rtc", //
    "network", "wlan-ssid", "wlan-password", "lan-phy", "lan-pwr-gpio", //
    "ntp-server", "longitude", "latitude", "tz", "verbose", //
    "mqtt-enable", "mqtt-url", "mqtt-user", "mqtt-password", "mqtt-root-topic", "mqtt-client-id",//
    "http-enable", "http-user", "http-password",//
    "stm32-bootgpio-inv",
    "restart", "all"
    ///////////// end of config keys /////////////////

    };
static_assert(!((sizeof otok_strings / sizeof otok_strings[0]) > (size_t)otok::SIZE));
static_assert(!((sizeof otok_strings / sizeof otok_strings[0]) < (size_t)otok::SIZE));
