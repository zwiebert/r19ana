#include "main.h"

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include <net/tcp_cli_server_setup.hh>
#include <net_http_server/http_server_setup.h>
#include "../app_private.h"
#include <esp_log.h>

#define logtag "main"

i32 boot_counter;

#ifdef CONFIG_APP_USE_WLAN
#define WIFI_AP_SSID "rv"
#define WIFI_AP_PASSWD "12345678"

bool wifi_ap_active;
void lfa_createWifiAp() {
  if (!wifi_ap_active) {
    wifi_ap_active = true;
    wifiAp_setup(WIFI_AP_SSID, WIFI_AP_PASSWD);

#ifdef USE_TCPS_TASK
    struct cfg_tcps cfg_tcps = { .enable = true }; // XXX: user-flags not set
    tcpCli_setup_task(&cfg_tcps);
#endif

    struct cfg_http cfg_http = { .enable = true };
    hts_setup(&cfg_http);
  }
}
#endif
void  mcu_delayedRestart(unsigned delay_ms) {
  printf("mcu_restart()\n");
  vTaskDelay(pdMS_TO_TICKS(delay_ms));
  esp_restart();
  for (;;) {
  }
}
extern "C" void bloat_app_main(void) {

  mcu_init();

  tmr_loopPeriodic_start();
  while (1) {
    loop();
#ifdef CONFIG_APP_USE_EG
    vTaskDelay(pdMS_TO_TICKS(LOOP_INTERVAL_MS));
#endif
  }
}


