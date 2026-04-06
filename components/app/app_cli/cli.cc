/*
 * cli.c
 *
 * Created: 24.08.2017 21:45:48
 *  Author: bertw
 */

#include "app_config/proj_app_cfg.h"
#include <string.h>
#include "cli_imp.h"
#include "app_cli/cli_app.hh"
#include "cli/mutex.hh"
#include "cli/cli.h"
#include "uout/uout_builder_json.hh"
#include "app_uout/status_output.h"
#include "app_settings/config.h"
#include <app_uout/so_config.h>
#include <app_uout/so_msg.h>
#include <algorithm>
#include <iterator>

namespace app::cli {

bool cli_isJson;

int process_parmHelp(clpar p[], int len, class UoutWriter &td);

const char cli_help_parmHelp[]  =
"type 'help command;'  or 'help all;'\ncommands are: ";

const char cli_help_None[]  = "none";

static struct parm_handler handlers[] = {
    { "config", app::cli::process_parmConfig, app::cli::cli_help_parmConfig },
    { "help", app::cli::process_parmHelp, app::cli::cli_help_parmHelp },
    { "cmd", app::cli::process_parmCmd, app::cli::cli_help_parmCmd },
    { "mcu", app::cli::process_parmMcu, app::cli::cli_help_parmMcu },
    { "kvs", app::cli::process_parmKvs, app::cli::cli_help_parmKvs },
    { "status", app::cli::process_parmStatus, cli_help_None},
#ifdef CONFIG_APP_USE_PROTOBUF
    { "pbuf", app::cli::process_parmProtoBuf, cli_help_None},
#endif
  };

static const struct parm_handlers our_parm_handlers = { .handlers = handlers, .count = sizeof(handlers) / sizeof(handlers[0]), };

const parm_handler* cli_parmHandler_find(const char *key) {
  auto handler = std::find_if(std::begin(handlers), std::end(handlers), [&key](auto el) {
    return strcmp(key, el.parm) == 0;
  });
  if (std::end(handlers) == handler) {
    return nullptr;
  }

  return handler;
}


/////////////// setup //////////////////


static bool cliApp_checkPassword(clpar p[], int len, class UoutWriter &td) {
  if (len < 2)
    return true;

  if (strcmp(p[0].key, "config") != 0)
    return true;

  if (!C.app_configPassword[0])
    return true;
  if (strcmp(p[1].key, "pw") == 0) {
    if (strcmp(p[1].val, C.app_configPassword) == 0) {
      soMsg_cfgpasswd_ok(td);
      return true;
    } else {
      soMsg_cfgpasswd_wrong(td);
    }
  } else {
    soMsg_cfgpasswd_missing(td);
  }

  return false;
}




void cliApp_setup() {
 // cli_hook_process_json = cliApp_redirect_to_rv;
  cli_hook_process_json_obj = app::cli::process_objJson;
  cli_hook_checkPassword = cliApp_checkPassword;

  cli_parmHandler_find_cb = cli_parmHandler_find;
  cli_parm_handlers = &our_parm_handlers;
}


} // namespace
