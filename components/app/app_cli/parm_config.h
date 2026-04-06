#include <app_config/options.hh>
#include <config_kvs/settings.hh>
#include <uout/uout_writer.hh>
#include <app_uout/so_types.h>
#include <cli/cli_types.h>
#include <stdint.h>

#define ENABLE_RESTART 1 // allow software reset

#define isValid_optStr(cfg, new) true
#define set_optStr(v, cb) config_save_item_s(comp_sett.get_kvsKey(cb), v)
#define set_optBlob(v, cb) config_save_item_b(comp_sett.get_kvsKey(cb), &v, sizeof v)
#define set_optStr_ifValid set_optStr
#define set_opt(t, v, cb) (config_save_item_##t(comp_sett.get_kvsKey(cb), v) && config_item_modified((cb)))
#define set_optN(t, v, cb) (config_save_item_n_##t(comp_sett.get_kvsKey(cb), v) && config_item_modified((cb)))

#define has_changed() SET_BIT(*changed_mask, so_key)

#define is_kt(k) (kt == otok::k_##k)
#define is_key(k) (strcmp(key, k) == 0)
#define is_val(k) (strcmp(val, k) == 0)


namespace app::cli {

struct SettData {
  const char *kvsKey;
  KvsType kvsType;
  StoreFun storeFun;
};

template<class Sett, typename CfgItem>
SettData settings_getData(const Sett &settings, CfgItem item) {
  SettData res { };
  res.kvsKey = settings.get_kvsKey(item);
  res.kvsType = settings.get_kvsType(item);
  res.storeFun = settings.get_storeFun(item);
  return res;
}

SettData get_settingsData(otok kt, u32 &changed_mask);

bool process_parmConfig_get_app(otok kt, const char *val, class UoutWriter &td);
bool process_parmConfig_get_comp(otok kt, const char *val, class UoutWriter &td);

int process_parmConfig_assign(KvsType kvsType, const char *kvsKey, StoreFun storeFun, const char *val);

bool process_parmConfig_comp(otok kt, const char *key, const char *val, class UoutWriter &td, int &errors, u32 &changed_mask);
bool process_parmConfig_app(otok kt, const char *key, const char *val, class UoutWriter &td, int &errors, u32 &changed_mask);

void parmConfig_reconfig_comp(uint32_t changed_mask);
void parmConfig_reconfig_app(uint32_t changed_mask);

} // namespace
