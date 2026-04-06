/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */

#include "app_config/proj_app_cfg.h"


#include "app_settings/config.h"
#include "utils_misc/int_macros.h"
#include "kvs/kvs_wrapper.h"
#include "stdint.h"

#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.settings.config.kvs"


#define CFG_KEY "global.C"


bool config_item_modified(enum configItem item) {
  return config_item_modified((configAppItem)item);
}
bool config_item_modified(enum configAppItem item) {
  kvshT h;
  if ((h = kvs_open(CFG_NAMESPACE, kvs_READ))) {
    switch ((int)item) {
    case CB_VERBOSE:
      config_ext_setup_txtio();
      break;
    default:
      break;
    }
    kvs_close(h);
  }
  return true;
}

