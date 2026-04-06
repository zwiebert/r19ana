/*
 * cli_status.c
 *
 *  Created on: 15.06.2019
 *      Author: bertw
 */

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <cli/cli_out.hh>

#include "app_cli/cli_app.hh"
#include "app_config/proj_app_cfg.h"
#include "app_mqtt/mqtt.h"
#include "app_uout/status_output.h"
#include "cli/cli.h"
#include "cli_imp.h"
#include "debug/dbg.h"
#include "debug/log.h"
#include "stdint.h"
#include "txtio/inout.h"
#include "uout/uout_builder_json.hh"
#include "utils_misc/int_macros.h"

namespace app::cli {

#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "appcli"

#define BUF_SIZE 128

int process_parmStatus(clpar p[], int len, class UoutWriter& td) {
  D(db_logi(logtag, "%s(len=%d)", __func__, len));
  int arg_idx;

  // soMsg_RVE_begin(td);

  for (arg_idx = 1; arg_idx < len; ++arg_idx) {
    const char *key = p[arg_idx].key, *val = p[arg_idx].val;
    D(db_printf("key=%s, val=%s\n", key, val));

    if (key == NULL) {
      return -1;
    } else {
      cli_warning_optionUnknown(td, key);
    }
  }

  // soMsg_RVE_end(td);

  return 0;
}

}  // namespace app::cli
