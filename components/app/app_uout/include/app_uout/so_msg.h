#pragma once
#include <app_uout/so_types.h>

void soMsg_fw_start_msg_print(class UoutWriter &td);
void soMsg_status_ok(class UoutWriter &td);
void soMsg_status_error(class UoutWriter &td);
void soMsg_mcu_begin(class UoutWriter &td);
void soMsg_mcu_run_time(class UoutWriter &td);
void soMsg_mcu_version(class UoutWriter &td);
void soMsg_mcu_ota(class UoutWriter &td, const char *url);
void soMsg_mcu_ota_state(class UoutWriter &td);
void soMsg_mcu_boot_count(class UoutWriter &td);
void soMsg_mcu_end(class UoutWriter &td);
void soMsg_sep_enable(class UoutWriter &td);
void soMsg_sep_disable(class UoutWriter &td);
void soMsg_sep_button_pressed_error(class UoutWriter &td);
void soMsg_cfgpasswd_ok(class UoutWriter &td);
void soMsg_cfgpasswd_wrong(class UoutWriter &td);
void soMsg_cfgpasswd_missing(class UoutWriter &td);
void soMsg_cuas_start(class UoutWriter &td, uint16_t id);
void soMsg_cuas_timeout(class UoutWriter &td);
void soMsg_cuas_done(class UoutWriter &td);
void soMsg_cuas_state(class UoutWriter &td, int state);
void soMsg_pras_start_listening(class UoutWriter &td, uint16_t id);
void soMsg_pras_stop_listening(class UoutWriter &td);
void soMsg_pras_timeout(class UoutWriter &td);
void soMsg_pras_done(class UoutWriter &td, bool success, bool unpair);
int soMsg_timer_print_begin(class UoutWriter &td);
void soMsg_timer_print_end(class UoutWriter &td);
void soMsg_timer_end(class UoutWriter &td);



void soMsg_pos_begin(class UoutWriter &td);
void soMsg_pos_end(class UoutWriter &td);
void soMsg_pair_begin(class UoutWriter &td);
void soMsg_pair_end(class UoutWriter &td);
void soMsg_pair_all_begin(class UoutWriter &td);
void soMsg_pair_all_end(class UoutWriter &td);

void soMsg_inet_print_address(class UoutWriter &td);
void soMsg_gpio_pin(class UoutWriter &td, const so_arg_pch_t a, bool broadcast = false);


void soMsg_print_kvd(class UoutWriter &td, const so_arg_kvd_t a);
void soMsg_print_kvs(class UoutWriter &td, so_arg_kvs_t a);

void soMsg_kv(class UoutWriter &td, const char *key, const char *val);
void soMsg_kv(class UoutWriter &td, const char *key, int val);
void soMsg_kv(class UoutWriter &td, const char *key, bool val);




void soMsg_FW_START_MSG_PRINT(class UoutWriter &td);
void soMsg_STATUS_OK(class UoutWriter &td);
void soMsg_STATUS_ERROR(class UoutWriter &td);
void soMsg_MCU_begin(class UoutWriter &td);
void soMsg_MCU_RUN_TIME(class UoutWriter &td);
void soMsg_MCU_VERSION(class UoutWriter &td);
void soMsg_MCU_OTA(class UoutWriter &td, const char *url);
void soMsg_MCU_OTA_STATE(class UoutWriter &td);
void soMsg_MCU_STM32OTA(class UoutWriter &td, const char *url);
void soMsg_MCU_STM32OTA_STATE(class UoutWriter &td);
void soMsg_MCU_BOOT_COUNT(class UoutWriter &td);
void soMsg_MCU_end(class UoutWriter &td);
void soMsg_KVS_begin(class UoutWriter &td);
void soMsg_KVS_ZN_SINGLE(class UoutWriter &td, const char *key);
void soMsg_KVS_ZN_ALL(class UoutWriter &td, const char *keyBase);
void soMsg_KVS_end(class UoutWriter &td);
void soMsg_RVE_begin(class UoutWriter &td);
void soMsg_RVE_PUMP(class UoutWriter &td, const so_arg_on_t *state);
void soMsg_RVE_RAIN(class UoutWriter &td, const so_arg_on_t *state);
void soMsg_RVE_PRESS_CTL(class UoutWriter &td, const so_arg_on_t *state);
void soMsg_RVE_VALVES(class UoutWriter &td, const so_arg_valves_t *valves);
void soMsg_RVE_end(class UoutWriter &td);
void soMsg_CFGPASSWD_OK(class UoutWriter &td);
void soMsg_CFGPASSWD_WRONG(class UoutWriter &td);
void soMsg_CFGPASSWD_MISSING(class UoutWriter &td);
void soMsg_cfgpasswd_ok(class UoutWriter &td);
void soMsg_cfgpasswd_wrong(class UoutWriter &td);
void soMsg_cfgpasswd_missing(class UoutWriter &td);
void soMsg_INET_PRINT_ADDRESS(class UoutWriter &td);
void soMsg_PBUF_begin(class UoutWriter &td);
void soMsg_PBUF_KV64(class UoutWriter &td, const so_arg_pbuf_t *pba);
void soMsg_PBUF_end(class UoutWriter &td);
