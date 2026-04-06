#include "unity.h"
#include <debug/dbg.h>

#include "../full_auto/automatic_timer.cc"
#include "full_auto/weather.hh"
#include "full_auto/weather.hh"

#include "weather/weather_irrigation.hh"
#include "weather/weather_provider_owm.hh"
#include <full_auto/automatic_timer.hh>

#if 1 //def   CONFIG_RV_NETMCU_TEST_DEBUG
#include <iostream>
using namespace std;
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#undef logtag
#define logtag "rv.test.full_auto"


static app::fa::AutoTimer at;
char buf[1024], buf2[1024];



void test_json()
{
  int count = 0;
  for(auto it = at.adapters_begin(); it != at.adapters_end(); ++it, ++count) {
    if (it->flags.exists)
      continue;
    if (0 <= count && count <= 4) {
     sprintf(it->name, "adapt_%d", count);
     it->flags.exists = true;
     it->d_temp = rando(0.0001, 0.5);
     it->d_wind = rando(0.0001, 0.5);
     it->d_humi = rando(0.0001, 0.5);
     it->d_clouds = rando(0.0001, 0.5);
    }
  }

  at.save_settings("at");
  app::fa::AutoTimer at2;
  at2.restore_settings("at");

  app::fa::WeatherAdapter ad;
  count = 0;
  for(auto it = at2.adapters_begin(); it != at2.adapters_end(); ++it, ++count) {
    if (!it->flags.exists)
      continue;
    TEST_ASSERT_TRUE_MESSAGE(it->to_json(buf, sizeof buf), "Serialize object to json");
    TEST_ASSERT_TRUE_MESSAGE(ad.from_json(buf), "Read back the json we just generated");
    bzero(buf2, sizeof buf2);
    TEST_ASSERT_TRUE_MESSAGE(ad.to_json(buf2, sizeof buf2), "Serialize object to json");
    TEST_ASSERT_EQUAL_STRING_MESSAGE(buf, buf2, "The json generated after  to_json()->from_json()->to_json() must be equal");


    D(cout << "at2 json:" << buf << "\n");
  }
}

void test_mulitple_fetch() {
  for (int i=0; i < 4; ++i) {
    TEST_ASSERT_TRUE_MESSAGE(app::fa::fa_poll_weather_full_hour(), "Fetch, parse and save JSON data ... what could go wrong?");
    sleep(10);
  }
}


void test_at_loop() {
  static Weather_Provider_Owm weather_provider;
  static Weather_Irrigation weather_irrigation(&weather_provider);
  weather_irrigation.dev_fill_past_wd_randomly();
  static app::fa::AutoTimer at(&weather_irrigation);
  at.dev_random_fill_data();


  at.todo_loop();

}


int main() {
  UNITY_BEGIN();

  RUN_TEST(test_at_loop);
  //RUN_TEST(test_mulitple_fetch);
  RUN_TEST(test_json);

  return UNITY_END();
}

