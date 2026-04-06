/*
 * config.h
 *
 *  Created on: 15.05.2019
 *      Author: bertw
 */

#ifndef MAIN_CONFIG_CONFIG_H_
#define MAIN_CONFIG_CONFIG_H_

#include "app_config/proj_app_cfg.h"
#include "config_kvs/config.h"
#include "config_kvs/comp_settings.hh"

#include "app_settings.hh"


#ifdef CONFIG_APP_USE_LAN
#include "net/ethernet_setup.hh"
#endif
#ifdef CONFIG_APP_USE_MQTT
#include "net_mqtt_client/mqtt.hh"
#endif
#ifdef CONFIG_APP_USE_WLAN
#include "net/wifi_station_setup.hh"
#endif
#ifdef CONFIG_APP_USE_HTTP
#include "net_http_server/http_server_setup.h"
#endif
#ifdef CONFIG_APP_USE_NTP
#include "net/ntp_client_setup.hh"
#endif
#include <stdint.h>

#ifdef CONFIG_APP_USE_NETWORK
enum nwConnection {
  nwNone, nwWlanSta, nwWlanAp, nwLan, nwLEN,
};
#endif

typedef struct {
	char app_configPassword[16];
#ifdef CONFIG_APP_USE_NETWORK
  enum nwConnection network;
#endif
} config;

extern config C;



// save C to persistent storage
void save_config_all();
void save_config_item(enum configItem item);
void save_config(uint32_t mask);

// restore C from persistent storage
void read_config_all();
void read_config_item(enum configItem item);
void read_config(uint32_t mask);

void config_setup_mqttAppClient();
bool config_item_modified(enum configItem item);

float config_read_longitude();
float config_read_latitude();
const char *config_read_tz(char *d, unsigned d_size);
#ifdef CONFIG_APP_USE_NETWORK
enum nwConnection  config_read_network_connection();
#endif

bool config_read_lph(uint16_t lph[14]);
bool config_save_lph(uint16_t lph[14]);

bool config_save_pb64(enum configItem item, const char *pb64);
bool config_read_pb64(enum configItem item, char *pb64_buf, size_t buf_size);
bool config_save_pb(enum configItem item, const uint8_t *pb, size_t pb_len);
bool config_read_pb(enum configItem item, uint8_t *pb_buf, size_t *pb_size);


void config_ext_setup_txtio();
void config_ext_setup_cliTcpServer();
#endif /* MAIN_CONFIG_CONFIG_H_ */
