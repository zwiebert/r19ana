#include "parm_config.h"
#include <app_config/proj_app_cfg.h>

#include <app_settings/config.h>

#include <app_misc/rtc.h>
#include "cli_imp.h"
#include <app_uout/so_config.h>
#include <config_kvs/comp_settings.hh>
#include <config_kvs/config.h>
#include <cli/cli_out.hh>

#ifdef CONFIG_APP_USE_MQTT
#include <app_mqtt/mqtt.h>
#endif
#ifdef CONFIG_APP_USE_HTTP
#include <net_http_server/http_server_setup.h>
#endif
#ifdef CONFIG_NET_HTTP_SERVER_HAVE_COMPONENT
#include <net_http_server/comp_glue.hh>
#endif
#ifdef CONFIG_APP_USE_NTP
#include <net/ntp_client_setup.hh>
#endif

#ifdef CONFIG_STM32_HAVE_COMPONENT
#include <stm32/comp_glue.hh>
#endif

#include "main_loop/main_queue.hh"

#include <stdint.h>
#include <stdint.h>
#include <utils_misc/mutex.hh>
#include <utils_misc/int_macros.h>

#include <stdlib.h>
#include <iterator>
#include <algorithm>


namespace app::cli {

bool process_parmConfig_get_comp(otok kt, const char *val, class UoutWriter &td) {
  switch (kt) {

  case otok::k_all: {
    if (is_val("?")) {
      soCfg_all(td);
      return true;
    } else if (is_val("net?")) {
      soCfg_all_net(td);
      return true;
    }
  }
    return false;

  case otok::k_rtc:
    soCfg_RTC(td);
    return true;

#ifdef CONFIG_STM32_HAVE_COMPONENT
  case otok::k_stm32_bootgpio_inv:
    soCfg_STM32_BOOTGPIO_INV(td);
    return true;
#endif

  default: {
    if (auto sd = comp_sett.get_SettingsData(kt); sd && sd->so_cfg_fun) {
      sd->so_cfg_fun(td);
      return true;
    }
    return false;
  }
  }
}

int process_parmConfig_assign(KvsType kvsType, const char *kvsKey, StoreFun storeFun, const char *val) {
  if (storeFun == STF_direct) {
    switch (kvsType) {
    case CBT_str:
      if (!config_save_item_s(kvsKey, val))
        return -1;
      return 1;

    case CBT_i8:
      if (!config_save_item_i8(kvsKey, val))
        return -1;
      return 1;

    case CBT_u32:
      if (!config_save_item_u32(kvsKey, val))
        return -1;
      return 1;

    case CBT_f:
      if (!config_save_item_f(kvsKey, val))
        return -1;
      return 1;

    default:
      return false;
    }
  } else if (storeFun == STF_direct_hex) {
    switch (kvsType) {
    case CBT_u32:
      if (!config_save_item_u32(kvsKey, val, 16))
        return -1;
      return 1;
    default:
      break;
    }
  }
  return 0;
}

bool process_parmConfig_comp(otok kt, const char *key, const char *val, class UoutWriter &td, int &errors, u32 &changed_mask) {
  switch (kt) {
#if ENABLE_RESTART
  case otok::k_restart:
    mainLoop_callFun([]() { mainLoop_mcuRestart(1500); });
    break;
#endif

    ////////////////////////////////////////////////////////////
#ifdef CONFIG_APP_USE_LAN
  case otok::k_lan_phy: {
    NODEFAULT();
    if (auto it = std::find(std::begin(cfg_args_lanPhy), std::end(cfg_args_lanPhy), val); it != std::end(cfg_args_lanPhy)) {
      int idx = std::distance(std::begin(cfg_args_lanPhy), it);
      set_optN(i8, idx, CB_LAN_PHY);
    }
  }
    break;
  case otok::k_lan_pwr_gpio: {
    NODEFAULT();
    set_opt(i8, val, CB_LAN_PWR_GPIO);
  }
    break;
#endif // CONFIG_APP_USE_LAN

#ifdef CONFIG_APP_USE_MQTT
  case otok::k_mqtt_enable:
    set_optN(i8, (*val == '1'), CB_MQTT_ENABLE);
    break;
#endif //CONFIG_APP_USE_MQTT
#ifdef CONFIG_APP_USE_HTTP
  case otok::k_http_enable:
    set_optN(i8, (*val == '1'), CB_HTTP_ENABLE);
    break;
#endif
    ////////////////////////////////////////////////////////////////

  case otok::k_rtc: {
    cli_replyResult(td, val ? rtc_set_by_string(val) : false);
  }
    break;

#ifdef CONFIG_APP_USE_NETWORK
  case otok::k_network: {
    int i;
    NODEFAULT();
    bool success = false;
    for (i = 0; i < nwLEN; ++i) {
#ifndef CONFIG_APP_USE_LAN
    if (i == nwLan)
    {
      continue;
    }
#endif
#ifndef CONFIG_APP_USE_WLAN
    if (i == nwWlanSta || i == nwWlanAp)
    {
      continue;
    }
#endif
      if (is_val(cfg_args_network[i])) {
        (void) (set_optN(i8, i, CB_NETWORK_CONNECTION));
        success = true;
        break;
      }
    }
    if (!success)
      cli_replyFailure(td);
  }
    break;
#endif

  case otok::NONE:
  default:
    return false;
  }
  return true;
}

void parmConfig_reconfig_comp(uint32_t changed_mask) {
  if (changed_mask & BIT(CB_TZ)) {
    rtc_setup();
  }

#ifdef CONFIG_STM32_HAVE_COMPONENT
  if (changed_mask & CB_STM32_INV_BOOTPIN) {
    mainLoop_callFun(config_setup_stm32);
 }
#endif

#ifdef CONFIG_APP_USE_LAN
  if (changed_mask & CMB_lan) {
    mainLoop_callFun(config_setup_ethernet);
  }
#endif
#ifdef CONFIG_APP_USE_MQTT
  if (changed_mask & CBM_mqttClient) {
    mainLoop_callFun(config_setup_mqttAppClient);
  }
#endif
#ifdef CONFIG_APP_USE_HTTP
  if (changed_mask & CBM_httpServer) {
    mainLoop_callFun(config_setup_httpServer);
  }
#endif
  if (changed_mask & CBM_txtio) {
    mainLoop_callFun(config_ext_setup_txtio);
  }
}

} // namespace
