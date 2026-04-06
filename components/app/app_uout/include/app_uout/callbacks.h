#pragma once
/**
 * \file app_uout/callbacks.h
 * \brief pub/sub interface to publish asynchronously to MQTT servers, HTTP-client web-sockets, USB-console, TCP-sockets/consoles
 */

#include <stdint.h>
#include <stddef.h>
#include <app_uout/status_output.h>
#include <uout/uo_callbacks.h>
#include <type_traits>

#define uo_evt_flag_valveChange uo_evt_flag_F
#define uo_evt_flag_rvStatusChange uo_evt_flag_F

struct so_arg_valve_state_t {
  uint8_t valve_number;
  bool is_open;
};


inline const so_arg_valve_state_t *uoCb_valveState_FromMsg(const uoCb_msgT msg) {
  if (msg.flags.evt.uo_evt_flag_valveChange && msg.flags.fmt.raw)
    return static_cast<const so_arg_valve_state_t *>(msg.cptr);
  return nullptr;
}

void uoAPp_publish_valve_change(so_arg_valve_state_t vs);
void uoAPp_publish_on_off_state_change(const char *key, bool state);
