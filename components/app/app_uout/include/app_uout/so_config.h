#pragma once

class UoutWriter;

void soCfg_all(class UoutWriter &td);
void soCfg_all_net(class UoutWriter &td);
void soCfg_all_rv(class UoutWriter &td);
void soCfg_all_time(class UoutWriter &td);


void soCfg_BAUD(class UoutWriter &td);
void soCfg_RTC(class UoutWriter &td);
void soCfg_CU(class UoutWriter &td);
void soCfg_NETWORK(class UoutWriter &td);
void soCfg_TZ(class UoutWriter &td);
void soCfg_LONGITUDE(UoutWriter &td);
void soCfg_LATITUDE(UoutWriter &td);
void soCfg_GPIO_PIN(class UoutWriter &td, int gpio_number);
void soCfg_GPIO_MODES(class UoutWriter &td);
void soCfg_GPIO_MODES_AS_STRING(class UoutWriter &td);

void soCfg_begin(class UoutWriter &td);
void soCfg_end(class UoutWriter &td);


