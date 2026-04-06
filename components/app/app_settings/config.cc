/*
 * config.c
 *
 *  Created on: 10.09.2018
 *      Author: bertw
 */


/*
 * config_defaults.c
 *
 *  Created on: 16.09.2017
 *      Author: bertw
 */
#include "app_config/proj_app_cfg.h"
#include "app_settings/config.h"
#include "app_settings/app_settings.hh"
#include "app_settings/config_defaults.h"
#include "utils_misc/base64.h"
#include <config_kvs/config.h>

#ifdef CONFIG_RV_NETMCU_DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.settings.config"
config C = {

};

#ifdef CONFIG_APP_USE_NETWORK
enum nwConnection config_read_network_connection() {
  return static_cast<enum nwConnection>(config_read_item((CB_NETWORK_CONNECTION), nwWlanAp));
}
#endif

#ifdef USE_POSIX_TIME
const char* config_read_tz(char *d, unsigned d_size) {
  return config_read_item((CB_TZ), d, d_size, CONFIG_APP_GEO_TZ);
}
#endif



#ifdef CONFIG_APP_USE_LPH
bool config_read_lph(uint16_t lph[14]) {
  return config_read_item_b(comp_sett.get_kvsKey(CB_LPH), (void*)lph, sizeof(uint16_t) * 14, (void*)0) != 0;
}
bool config_save_lph(uint16_t lph[14]) {
  return config_save_item_b(comp_sett.get_kvsKey(CB_LPH), (void*)lph, sizeof(uint16_t) * 14);
}
#endif

#ifdef CONFIG_APP_USE_PROTOBUF
bool config_save_pb64(enum configItem item, const char *pb64) {
  return config_save_item_s(comp_sett.get_kvsKey(item), pb64);
}

bool config_read_pb64(enum configItem item, char *pb64_buf, size_t buf_size) {
  return config_read_item_s(comp_sett.get_kvsKey(item), pb64_buf, buf_size, "");
}

bool config_save_pb(enum configItem item, const uint8_t *pb, size_t pb_len) {
  unsigned char pb64[pb_len/2 + pb_len];

  int err = mbedtls_base64_encode(pb64, sizeof pb64, &pb_len, pb, pb_len);
  if (err)
    return false;
  return config_save_pb64(item, (const char *)pb64);
}

bool config_read_pb(enum configItem item, uint8_t *pb_buf, size_t *pb_size) {

  if (!config_read_pb64(item, (char *)pb_buf, *pb_size))
    return false;

  int err = mbedtls_base64_decode(pb_buf, *pb_size, pb_size, pb_buf, strlen((char*)pb_buf));

  if (err)
    return false;

  return true;
}
#endif
