
#include <utils_misc/int_macros.h>
#include <utils_misc/cstring_utils.hh>
#include <txtio/txtio_mutex.hh>
#include <app_uout/so_msg.h>
#include <app_uout/so_types.h>
#include <uout/uout_builder_json.hh>
#include <stdio.h>
#include <string.h>
#include <debug/dbg.h>

#define UOUT_PROTECTED
#include <uout/uo_callbacks.h>
#include <app_uout/callbacks.h>

void uoAPp_publish_on_off_state_change(const char *key, bool state) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_rvStatusChange = true;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    snprintf(buf, sizeof buf, R"({"data":{"%s":%d}})", key, state);
    uoCb_publish(idxs, buf, flags);
  }
}

void uoAPp_publish_valve_change(so_arg_valve_state_t vs) {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_valveChange = true;

  flags.fmt.raw = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    uoCb_publish(idxs, &vs, flags);
  }

  flags.fmt.raw = false;
  flags.fmt.json = true;
  if (auto idxs = uoCb_filter(flags); idxs.size) {
    char buf[64];
    snprintf(buf, sizeof buf, "{\"valve\":{\"v%d\":%d}}", vs.valve_number, vs.is_open);
    uoCb_publish(idxs, buf, flags);
  }
}

