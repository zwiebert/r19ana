#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unity.h>

#include "XR25Frame.hh"

const unsigned FRAME_CT = 80;
const unsigned FRAME_LEN = 29;
const size_t size = FRAME_LEN * FRAME_CT + 14;
uint8_t random_bytes[size];
uint8_t stuffed_bytes[size * 2];
unsigned sb_len = 0;
unsigned stuffed_frames_ct = 0;
uint8_t destuffed_bytes[size * 2];
unsigned dsb_len = 0;
unsigned destuffed_frames_ct = 0;

void stuff(uint8_t* dst, size_t dst_size, uint8_t* src, size_t src_len,
           unsigned frame_len = 29) {
  unsigned sb = 0;
  unsigned frame_ct = 0;
  for (unsigned df = 0; df + frame_len < src_len; df += frame_len) {
    dst[sb++] = 0xff;
    dst[sb++] = 0x00;
    for (unsigned i = 0; i < frame_len; ++i) {
      uint8_t b = src[df + i];
      dst[sb++] = b;
      if (b == 0xff) dst[sb++] = 0xff;
    }
    ++frame_ct;
  }
  dst[sb++] = 0xff;
  dst[sb++] = 0x00;
  sb_len = sb;
  stuffed_frames_ct = frame_ct;
}

XR25Frame xr25;

void destuff(uint8_t* dst, size_t dst_size, uint8_t* src, size_t src_len,
             unsigned frame_len = 29) {
  unsigned db = 0;
  unsigned frame_ct = 0;
  for (int sb = 0; sb < sb_len; ++sb) {
    if (xr25.add(src[sb])) continue;

    while (xr25.get_buffered_frame_count()) {
      XR25Frame::voc_t voc;
      if (xr25.pull_voc(voc)) {
        memcpy(dst + db, &voc.frame[0], voc.frame_len);
        db += voc.frame_len;
        ++frame_ct;
      }
    }
    xr25.add(src[sb]);
  }
  while (xr25.get_buffered_frame_count()) {
    XR25Frame::voc_t voc;
    if (xr25.pull_voc(voc)) {
      memcpy(dst + db, &voc.frame[0], voc.frame_len);
      db += voc.frame_len;
      ++frame_ct;
    }
  }
  dsb_len = db;
  destuffed_frames_ct = frame_ct;
}

int init() {
  // 1. Seed the random number generator once
  // srand(0);
  srand((unsigned int)time(NULL));

  // 2. Fill the array
  for (size_t i = 0; i < size; i++) {
    // rand() % 256 ensures the value fits in a uint8_t (0-255)
    random_bytes[i] = (uint8_t)(rand() % 256);
  }
  return 0;
}

void my_test() {
  init();
  stuff(stuffed_bytes, sizeof stuffed_bytes, random_bytes, sizeof random_bytes);
  destuff(destuffed_bytes, sizeof destuffed_bytes, stuffed_bytes, sb_len);
  TEST_ASSERT_EQUAL_INT16(FRAME_CT, stuffed_frames_ct);
  TEST_ASSERT_EQUAL_INT16(FRAME_CT, destuffed_frames_ct);
  TEST_ASSERT_LESS_OR_EQUAL(0, xr25.m_invalid_frame_ct);
  TEST_ASSERT_EQUAL_UINT8_ARRAY(random_bytes, destuffed_bytes, dsb_len);
}

void setUp() {}
void tearDown() {}

int main(void) {
  UNITY_BEGIN();

  // RUN_TEST sets up the setjmp point that longjmp targets
  RUN_TEST(my_test);

  return UNITY_END();
}
