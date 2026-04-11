/*
 * config_comps.c
 *
 *  Created on: 21.04.2020
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/config_defaults.h"
#include "app_uout/callbacks.h"
#include "config_kvs.h"
#include "utils_misc/int_macros.h"
#include "kvs/kvs_wrapper.h"
#include "stdint.h"

#ifdef CONFIG_APP_USE_MQTT
#include <net_mqtt_client/comp_glue.hh>
#include "app_mqtt/mqtt.h"
void config_setup_mqttAppClient() {
  struct cfg_mqtt c;
  config_read_mqttClient(&c);
  io_mqttApp_setup(&c);
}
#endif

#include <txtio/comp_glue.hh>
void config_ext_setup_txtio() {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_valveChange = true;
  config_setup_txtio(&flags);
}

#ifdef CONFIG_APP_USE_NETWORK
#include <net/comp_glue.hh>
#endif
void config_ext_setup_cliTcpServer() {
  uo_flagsT flags;
  flags.evt.uo_evt_flag_valveChange = true;
#ifdef CONFIG_APP_USE_TCPS_TASK
  config_setup_cliTcpServer(&flags);
#endif
}

float config_read_longitude() {
  return config_read_item((CB_LONGITUDE), MY_GEO_LONGITUDE);
}
float config_read_latitude() {
  return config_read_item((CB_LATITUDE), MY_GEO_LATITUDE);
}
