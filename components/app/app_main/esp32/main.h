#pragma once

#include "app_config/proj_app_cfg.h"

#include "app_misc/common.h"
#include "app_misc/rtc.h"
#include "app_misc/timer.h"
#include "app_config/proj_app_cfg.h"
#include "app_net/gw_ping.h"
#include "app_cli/cli_app.hh"
#include "app_settings/config.h"
#include "driver/gpio.h"
#include "kvs/kvs_wrapper.h"
#include "app_misc/kvstore.h"
#include "utils_misc/int_macros.h"
#include "stdint.h"
#include "net/ethernet_setup.hh"
#include "net/ipnet.h"
#include "app_mqtt/mqtt.h"
#include "net/ntp_client_setup.hh"
#include "net/tcp_cli_server.h"
#include "net/wifi_ap_setup.h"
#include "net/wifi_station_setup.hh"
#include "nvs_flash.h"
#include "storage/storage.h"
#include "txtio/inout.h"
#include "app_misc/watch_dog.h"

#include "freertos/FreeRTOS.h"
#include "freertos/timers.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include <esp_event.h>
#include <esp_system.h>
#include <esp_sntp.h>
#include <string.h>



#define KEY_BOOT_COUNTER "BOOT_CT"

extern i32 boot_counter;
extern bool wifi_ap_active;

void lfa_createWifiAp(void);
void lfa_gotIpAddr(void);
void lfa_lostIpAddr(void);
void lfa_mcuRestart(void);
void lfPer100ms_mainFun(void);
void loop(void);
void tmr_checkNetwork_start();
void tmr_loopPeriodic_start();
void tmr_pingLoop_start();
void loop_eventBits_setup();
u32 loop_eventBits_wait();

void  mcu_delayedRestart(unsigned delay_ms);

extern "C" void main_setup_ip_dependent();
void mcu_init();



