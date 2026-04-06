#ifndef RTC_H_
#define RTC_H_

#include "app_config/proj_app_cfg.h"
#include <time.h>

#include "utils_time/ut_constants.hh"

extern volatile time_t run_time_secs;
#define run_time(x) (run_time_secs + 0)

void rtc_secTick(void);

//YYYY-MM-DDThh-mm-ss
bool rtc_set_by_string(const char *dateTimeString);
bool rtc_get_by_string(char *dst);
time_t time_iso2time(const char *dateTimeString);

time_t rtc_timezone_in_secs();

int8_t get_weekDay(void);
int16_t get_yearDay(void);
int get_dst(void);
int16_t rtc_get_next_minute(void); // get next minute_of_day or -1 until next minute begins

void rtc_setup(void);


typedef time_t rtc_time_t;
typedef enum { RTC_SRC_NONE, RTC_SRC_NTP, RTC_SRC_CLI } rtc_time_source_t;

bool ntp_set_system_time(void);

#ifdef USE_POSIX_TIME
#define rtc_tick() do { ++run_time_secs; } while (0)
#else
extern volatile time_t __system_time;
#define rtc_tick() do { ++__system_time; } while (0)
#endif

void rtc_set_system_time(rtc_time_t stamp, rtc_time_source_t source);
rtc_time_t rtc_time(void);

extern rtc_time_source_t rtc_last_time_source;


#endif
