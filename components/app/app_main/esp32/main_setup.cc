#include "main.h"
#include <cli/cli.h>
#include "app_settings/config.h"
#include "app_cli/cli_app.hh"
#include "cli/mutex.h"
#include "net/http_client.h"
#include "app_settings/config.h"
#include "app_http_server/setup.h"
#include "../app_private.h"
#include "main_loop/main_queue.hh"
#include <esp_netif.h>
#include <esp_log.h>
#include <config_kvs/register_settings.hh>
#ifdef CONFIG_APP_USE_WEATHER_AUTO
#include <full_auto/setup.hh>
#endif

#define logtag "main"

void ntpApp_setup(void) {
  sntp_set_time_sync_notification_cb([](struct timeval *tv) {
    ESP_LOGI(logtag, "ntp synced: %lld", (long long int)time(0));

    static bool once;
    for (int i=0; !once && i < SNTP_MAX_SERVERS; ++i, once = true) {
      if (auto name = esp_sntp_getservername(i); name) {
        ESP_LOGE(logtag, "ntp server #%d is <%s>", i, name);
      }
    }
  });

  config_setup_ntpClient();
}

void main_setup_ip_dependent() {
  static int once;
  if (!once) {
    once = 1;

#ifdef CONFIG_APP_USE_NTP
    ntpApp_setup();
#endif
#ifdef CONFIG_APP_USE_MQTT
    config_setup_mqttAppClient();
#endif
#ifdef CONFIG_APP_USE_TCPS_TASK
    config_ext_setup_cliTcpServer();
#endif
#ifdef CONFIG_APP_USE_HTTP
  config_setup_httpServer();
#endif

  }
#ifdef CONFIG_APP_PING_THE_GATEWAY
  tmr_pingLoop_start();
#endif
}


void lfPer100ms_mainFun() {
#ifndef CONFIG_APP_USE_CLI_TASK
  cli_loop();
#endif
}


void mcu_init() {
  mainLoop_setup(32);

  kvs_setup();

  config_ext_setup_txtio();

  io_puts("\r\n\r\n");

  ESP_ERROR_CHECK(esp_event_loop_create_default());

  if constexpr (use_NETWORK) {

    ipnet_CONNECTED_cb = main_setup_ip_dependent;
    enum nwConnection network = config_read_network_connection();

#ifdef CONFIG_APP_USE_AP_FALLBACb
se=
bbbbbbbbbbbbbbbbbbbb();
#else
    if (network != nwNone)
      esp_netif_init();
#endif

#ifdef PING_NOT_BROKEN // pings will timeout with latest ESP-IDF (was working April 2020, broken April 2021 or earlier)
    //XXX reset tcp/ip adapter here instead of reboot?
    ping_restart_cb = [] {
      lf_setBit(lf_mcuRestart);
    };
#endif

#ifdef CONFIG_APP_USE_WLAN
      if (network == nwWlanSta)
        config_setup_wifiStation();
#endif

#ifdef CONFIG_APP_USE_WLAN_AP
      if (network == nwWlanAp)
        lfa_createWifiAp(); // XXX: Create the fall-back AP. Should we have a regular configured AP also?
#endif


#ifdef CONFIG_APP_USE_LAN
      if (network == nwLan)
        config_setup_ethernet();
#endif

#ifdef CONFIG_APP_USE_AP_FALLBACK
      if (network != nwWlanAp)
        tmr_checkNetwork_start();
#endif

#ifdef CONFIG_APP_USE_HTTP
      hts_setup_content();
#endif

  }
  ////Orig

  io_puts("\r\n\r\n");

  if constexpr (use_FS)
    stor_setup();
  rtc_setup();
  app::cli::cliApp_setup();
#ifdef CONFIG_APP_USE_CLI_TASK
  cli_setup_task(true);
#else
#error "currently unsupported" // FIXME: support CLI without its own task, or remove that APP_USE_CLI_TASK kconfig option
#endif

#ifdef CONFIG_APP_USE_WEATHER_AUTO
  app::fa::fa_setup(nullptr);
#endif
  kvs_get_int32(KEY_BOOT_COUNTER, &boot_counter), kvs_store_int32(KEY_BOOT_COUNTER, ++boot_counter);

}
