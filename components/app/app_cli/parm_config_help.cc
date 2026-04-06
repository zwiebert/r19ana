#include "app_config/proj_app_cfg.h"
#include "app_cli/cli_app.hh"


namespace app::cli {

const char cli_help_parmConfig[] = "'config' sets or gets options. Use: config option=value ...; to set. Use: config option=? ...; to get, if supported\n\n"
    "rtc=(ISO_TIME|?)   set local time it NTP is not working.;\n"
#ifdef CONFIG_APP_USE_NETWORK
    "network=(wlan|ap|lan) ap opens wlan access-point at 192.168.4.1 (restart required)\n"
#endif
#ifdef CONFIG_APP_USE_WLAN
    "wlan-ssid=(SSID|?)\n"
    "wlan-password=PW\n"
#endif
#ifdef CONFIG_APP_USE_NTP
    "ntp-server=(dhcp|IP4ADDR|NAME) default: ntp.pool.org (TODO: implement list of servers)\n"
#endif
#ifdef CONFIG_APP_USE_MQTT
    "mqtt-enable=(0|1) enable MQTT\n"
    "mqtt-url=URL      broker/server URL (e.g. mqtt://192.168.1.42:7777)\n"
    "mqtt-user=NAME    user name on server\n"
    "mqtt-password=PW  user password on server\n"
    "mqtt-client-id=CID  default: rv\n"
#endif
#ifdef CONFIG_APP_USE_HTTP
    "http-enable=(0|1) enable HTTP\n"
    "http-user=NAME    user name on server\n"
    "http-password=PW  user password on server\n"
#endif
#ifdef USE_POSIX_TIME
    "tz=(POSIX_TZ|?)    time zone for RTC/NTP\n"
#endif
    "verbose=(0..5|?)   diagnose output verbosity level\n"
    "set-pw=password    set a config password. if set every config commands needs the pw option\n"
    "pw=PW              example: config pw=my_passw dst=eu;\n"
#if ENABLE_RESTART
    "restart            restart MCU\n"
#endif
#ifdef ACCESS_GPIO
    "gpioN=(i|p|o|0|1|d|?) Set gpio pin as input (i,p) or output (o,0,1) or use default\n"
#endif
    "stm32-bootgpio-inv   Invert Boot-Pin for STM32 OTA\n"
//  "set-expert-password=\n"
;

} // namespace
