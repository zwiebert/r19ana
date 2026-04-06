/**
 * \file   app_main/esp32/main_loop_periodic.h
 * \brief  Run events in main task by setting event bits from periodic 100ms timer
 */
#include "app_config/proj_app_cfg.h"

#include "main.h"
#include <cli/cli.h>
#ifdef CONFIG_APP_USE_WEATHER_AUTO
#include "full_auto/weather.hh"
#endif
#include <app_settings/config.h>
#include "utils_misc/int_macros.h"
#include "stdint.h"
#include "utils_time/run_time.h"
#include "utils_time/ut_constants.hh"
#include "main_loop/main_queue.hh"
#include "net/ipnet.h"

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <freertos/projdefs.h>
#include <freertos/timers.h>
#include <esp_log.h>

#ifdef CONFIG_RV_NETMCU_DEBUG
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#define logtag "rv.main.periodic";

static void publish_time() {
  char json[64] = {};

  if (sizeof json <= snprintf(json, sizeof json, R"({"from":"netmcu", "mcu":{"time":%lld}})", time(0)))
    return;

  uoCb_publish_wsJson(json);
}

/**
 * \brief Create 100ms timer for periodic actions
 * Actions should be called with mainLoop_callFun() to execute them in mainLoop context instead of "Tmr Svc" Task.
 *
 */
void tmr_loopPeriodic_start() {
  const int interval = pdMS_TO_TICKS(LOOP_PERIODIC_INTERVAL_MS);
  TimerHandle_t tmr = xTimerCreate("PerLoop100ms", interval, pdTRUE, nullptr, [](TimerHandle_t xTimer) {

    // Note: This function body runs in FREERTOS_TIMER_TASK (Tmr Svc) Task.
    // Note: use mainLoop_callFun() to execute actions in main task


    static uint32_t count; // counts up every 0.1 seconds.


    // Every 100ms: run cli_loop, watch_dog, ...
    mainLoop_callFun(lfPer100ms_mainFun);

    // Ever 1.6 seconds publish current time to webapp
    if ((count & (BIT(4) - 1)) == 0) {
      mainLoop_callFun(publish_time);
    }

    // Every 51.2 seconds check if actions based on local time are due
    if ((count & (BIT(9) - 1)) == 0) {
      const time_t tnow = time(0);
      struct tm tms;
      if (localtime_r(&tnow, &tms)) {


#ifdef CONFIG_NETMCU_FORCED_REBOOT
    // forced daily reboot
    // XXX: restart every >=24 hours at 23:33
    if (run_time_s() > SECS_PER_DAY) { //
      if (tms.tm_hour == 23 && tms.tm_min >= 33)
        mainLoop_mcuRestart(0);
    }
#endif
  }
}
  ++count;
} );
  if (!tmr || xTimerStart(tmr, 10 ) != pdPASS) {
    printf("PerLoopTimer start error");
  }

}
