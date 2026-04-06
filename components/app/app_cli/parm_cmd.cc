#include <debug/log.h>
#include <stdio.h>
#include <string.h>

#include <cli/cli_out.hh>

#include "app_cli/cli_app.hh"
#include "app_config/options.hh"
#include "app_config/proj_app_cfg.h"
#include "app_misc/opt_map.hh"
#include "app_mqtt/mqtt.h"
#include "app_uout/status_output.h"
#include "cli/cli.h"
#include "cli_imp.h"
#include "debug/dbg.h"
#include "txtio/inout.h"
#include "uout/uout_builder_json.hh"
#include "utils_misc/int_macros.h"

#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.cli.parm_cmd"

namespace app::cli {

#define ONE_MINUTE (60)
#define ONE_HOUR (ONE_MINUTE * 60)
#define ONE_DAY (ONE_HOUR * 24)

const char cli_help_parmCmd[] =
    "durN=[0-60]      activate zone N for up to 60 minutes (0=off). Example: "
    "cmd dur3=45;\n"
    "%f,%d,%f,%d,%f,%d,%f,%f, on, ignoreRainSensor, off, repeats, period, "
    "dInterval, dhBegin, dhEnd\n"
    "dur=?            request durations of all activated zones\n"
    "rem=?            request remaining times of all activated zones\n"
    "timer=?          request list of the active timers\n"
    "status=?         request status from rvmcu\n"
    "rv-version=?     request version string from rvmcu\n";

#define KEY_DURATION_PREFIX "dur"
#define KEY_DURATION_PREFIX_LEN ((sizeof KEY_DURATION_PREFIX) - 1)

#define ZONE_COUNT 14
#define MAX_DURATION 60

int process_parmCmd(clpar p[], int len, class UoutWriter& td) {
  int arg_idx;

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    D(db_logi(logtag, "key=%s, val=%s", key, val));

    if (!key) return -1;
#if 1
    otok kt = optMap_findToken(key);
    if (kt != otok::NONE) {
      switch (kt) {
        default:
          break;
      }
    }
#endif
  }

  return 0;
}

}  // namespace app::cli
