/**
 * \file app_settings/app_settings.hh
 * \brief Settings for application components
 */
#pragma once
#include "app_config/proj_app_cfg.h"
#include <app_config/options.hh>
#include <app_uout/so_config.h>
#include <utils_misc/int_macros.h>
#include "config.h"
#include <config_kvs/comp_settings.hh>
#include <config_kvs/settings.hh>
#include <config_kvs/settings_template.hh>
#include <assert.h>
#include <config_kvs/register_settings.hh>

enum configAppItem : i8 {
  CBA_NONE = -1, CBA_start = CB_size - 1, //
  //----------------
  CB_CFG_PASSWD,
  CB_LONGITUDE,
  CB_LATITUDE,
  CB_TZ,
#ifdef CONFIG_APP_USE_NETWORK
  CB_NETWORK_CONNECTION,
#endif
#ifdef CONFIG_APP_USE_LPH
  CB_LPH,
#endif

//-----------
  CBA_size
};

extern const SettingsBase<configAppItem> &app_sett;

class AppSettingsEnums {
public:
  enum {
    CBA_NONE = -1, CBA_start = CB_size - 1, //
    //----------------
    CB_CFG_PASSWD,
    CB_TZ,
#ifdef CONFIG_APP_USE_NETWORK
    CB_NETWORK_CONNECTION,
#endif
#ifdef CONFIG_APP_USE_LPH
    CB_LPH,
#endif
//-----------
    CBA_size
  };

};
#if 0
class AppSettings final: public Settings<configAppItem, CBA_size > {
public:
  using Base = Settings<configAppItem, CBA_size>;
  using Item = configAppItem;
  using storeFunT = void (*)(configAppItem item, const char *val);
public:
  constexpr AppSettings() {
    initField(CB_CFG_PASSWD, "C_CFG_PW", otok::NONE, CBT_str, 0, STF_direct);
    initField(CB_TZ, "C_TZ", otok::k_tz, CBT_str, soCfg_TZ, STF_direct);
#ifdef CONFIG_APP_USE_NETWORK
    initField(CB_NETWORK_CONNECTION, "C_NW_CONN", otok::k_network, CBT_i8, soCfg_NETWORK);
#endif
#ifdef CONFIG_APP_USE_LPH
    initField(CB_LPH, "C_LPH", otok::NONE, CBT_blob, nullptr);
#endif
  }
public:

};
#endif

bool config_item_modified(enum configAppItem item);

