/**
 * \file app_settings/app_settings.hh
 * \brief Settings for application components
 */
#include "app_settings/app_settings.hh"
#include <app_config/options.hh>
#include <app_uout/so_config.h>
#include <utils_misc/int_macros.h>
#include "config_kvs/config.h"
#include "config_kvs/comp_settings.hh"
#include <config_kvs/settings.hh>
#include <assert.h>

template<typename settings_type>
constexpr void register_app_settings(settings_type &settings) {
    settings.initField(CB_CFG_PASSWD, "C_CFG_PW", otok::NONE, CBT_str, 0, STF_direct);
    settings.initField(CB_LONGITUDE, "C_LONGITUDE", otok::k_longitude, CBT_f, soCfg_LONGITUDE, STF_direct);
    settings.initField(CB_LATITUDE, "C_LATITUDE", otok::k_latitude, CBT_f, soCfg_LATITUDE, STF_direct);
    settings.initField(CB_TZ, "C_TZ", otok::k_tz, CBT_str, soCfg_TZ, STF_direct);
#ifdef CONFIG_APP_USE_NETWORK
    settings.initField(CB_NETWORK_CONNECTION, "C_NW_CONN", otok::k_network, CBT_i8, soCfg_NETWORK);
#endif
#ifdef CONFIG_APP_USE_LPH
    settings.initField(CB_LPH, "C_LPH", otok::NONE, CBT_blob, nullptr);
#endif
}


#include <config_kvs/register_settings.hh>
#include <config_kvs/comp_settings.hh>
class CompSettings final : public Settings<configItem, CBA_size> {
public:
  using Base = Settings<configItem, CB_size>;
public:
  constexpr CompSettings() {
    register_settings(*this);
    register_app_settings(*this);
 }
};
static constexpr CompSettings comp_settings_obj;
const SettingsBase<configItem> &comp_sett = comp_settings_obj;

