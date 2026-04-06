#ifndef _common_h
#define _common_h

////////////////////////

#include <stdint.h>
#include <stdbool.h>
#include "stdint.h"

#ifdef DISTRIBUTION
#define NO_PRECOND
#define NO_POSTCOND
#endif

#include "debug/dbg.h"

void mcu_delay_us(u16 us);
bool watchDog_checkCommandLine(const char *cmdLine);

#endif // _common_h
