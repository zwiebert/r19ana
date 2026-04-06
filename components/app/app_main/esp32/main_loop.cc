#include "main.h"
#include <cli/cli.h>
#include <config_kvs/config.h>
#include "main_loop/main_queue.hh"

#ifdef CONFIG_APP_USE_WLAN_AP
void tmr_checkNetwork_start() {
  const int interval = pdMS_TO_TICKS(1000 * CHECK_NETWORK_INTERVAL);
  TimerHandle_t tmr = xTimerCreate("CheckNetworkTimer", interval, pdFALSE, nullptr, [](TimerHandle_t xTimer) {
    if (!ipnet_isConnected()) {
      mainLoop_callFun(lfa_createWifiAp);
    }
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("CheckNetworkTimer start error");
  }
}
#endif

void tmr_pingLoop_start() {
  const int interval = pdMS_TO_TICKS(1000 * PING_INTERVAL);
  TimerHandle_t tmr = xTimerCreate("PingLoopTimer", interval, pdTRUE, nullptr, [](TimerHandle_t xTimer) {
    if (!wifi_ap_active) {
      mainLoop_callFun(ping_loop);
    }
  });
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("PingLoopTimer start error");
  }
}

void loop(void) {
  mainLoop_processMessages(0, 5000);
}

