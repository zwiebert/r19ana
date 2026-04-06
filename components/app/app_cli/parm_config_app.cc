#include "parm_config.h"

#include "app_config/proj_app_cfg.h"

#include "app_cli/cli_config.h"
#include "app_misc/opt_map.hh"
#include <string.h>


#include "app_uout/status_output.h"
#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "app_misc/rtc.h"
#include "utils_misc/bcd.h"
#include "cli_imp.h"
#include "utils_misc/stof.h"
#include "utils_misc/cstring_utils.h"
#ifdef CONFIG_APP_USE_MQTT
#include "app_mqtt/mqtt.h"
#endif
#ifdef CONFIG_APP_USE_HTTP
#include "net_http_server/http_server_setup.h"
#endif
#ifdef CONFIG_APP_USE_NTP
#include "net/ntp_client_setup.hh"
#endif


#include "stdint.h"
#include "stdint.h"
#include <utils_misc/mutex.hh>
#include <utils_misc/int_macros.h>

#include <stdlib.h>
#include <iterator>
#include <algorithm>


namespace app::cli {

bool process_parmConfig_get_app(otok kt, const char *val, class UoutWriter &td) {
  switch (kt) {


  default:
    return false;
  }
}

bool process_parmConfig_app(otok kt, const char *key, const char *val, class UoutWriter &td, int &errors, u32 &changed_mask) {
  switch (kt) {

  case otok::NONE:
    return false;
  default:
    return false;
  }
  return true;
}


void parmConfig_reconfig_app(uint32_t changed_mask) {
}

} // namespace
