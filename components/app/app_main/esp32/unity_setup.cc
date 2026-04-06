/*
 * unity_setup.c
 *
 *  Created on: 27.02.2020
 *      Author: bertw
 */

#include "kvs/kvs_wrapper.h"
#include "storage/storage.h"
#include "txtio/inout.h"
#include "app_settings/config.h"
#include "cli/mutex.hh"

int unit_testing;

void mcu_init(void);

void unity_suiteSetUp() {

  unit_testing = 1;

#if 0
 mcu_init();
#else
  //config_setup();

  kvs_setup();
  config_ext_setup_txtio();
  stor_setup();
#endif

}
