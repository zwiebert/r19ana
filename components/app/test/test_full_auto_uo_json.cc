#include "unity.h"

#include <debug/dbg.h>
#include "../full_auto/automatic_timer.cc"
#include "full_auto/weather.hh"
#include <full_auto/automatic_timer.hh>
#include "weather/weather_irrigation.hh"
#include "weather/weather_provider_owm.hh"

#include <unistd.h>

#if 1 //def   CONFIG_RV_NETMCU_TEST_DEBUG
#include <iostream>
#include <cstdio>
using namespace std;
#define DEBUG
#define D(x) x
#else
#define D(x)
#endif
#undef logtag
#define logtag "rv.test.full_auto"

static app::fa::AutoTimer at;
char buf[1024], buf2[1024], buf3[100000], buf4[100000];

int writecb(const char *src, ssize_t src_len, int chunk_status) {
  static char *wp = buf4;
  if (-1 <= chunk_status && chunk_status <= 0)
    wp = buf4;
  memcpy(wp, src, src_len);
  wp += src_len;
  *wp = '\0';
  return src_len;
  return write(1, src, src_len);
}

void test_uo_json() {
  {
    auto td = UoutWriterFunction(writecb, SO_TGT_HTTP | SO_TGT_FLAG_JSON);
    auto &sj = td.sj();
    TEST_ASSERT_TRUE_MESSAGE(sj.open_root_object("testroot"), "We need a root object");
    TEST_ASSERT_TRUE_MESSAGE(sj.add_object("testobj"), "We need a root object");
    TEST_ASSERT_TRUE_MESSAGE(at.write_zones_to_json(td, "zones"), "As long as there is enought buffer");
    sj.close_object();
    sj.close_root_object();
  }
  puts(buf4);
  jsoneat::Jsmn_String jsmn(buf4, 1024);
  TEST_ASSERT_TRUE_MESSAGE((bool )jsmn, "Validate JSON by trying to parse it");

}

int main() {
  UNITY_BEGIN();

  RUN_TEST(test_uo_json);

  return UNITY_END();
}
