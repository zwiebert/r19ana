import { expect, test } from "vitest";
import byte_unstuffing from "./byte_unstuffing.js";

function getPseudoRandomBytes(size) {
  return Uint8Array.from({ length: size }, () => Math.floor(Math.random() * 256));
}

const FRAME_CT = 40;
const FRAME_LEN = 29;
const size = FRAME_LEN * FRAME_CT;
let random_bytes = getPseudoRandomBytes(size);
let stuffed_bytes = new Uint8Array(size * 2);
let sb_len = 0;
let stuffed_frames_ct = 0;
let destuffed_bytes = new Uint8Array(size * 2);
let dsb_len = 0;
let destuffed_frames_ct = 0;

function stuff(dst, dst_size, src, src_len, frame_len = 29) {
  let sb = 0;
  let frame_ct = 0;
  for (let df = 0; df + frame_len <= src_len; df += frame_len) {
    dst[sb++] = 0xff;
    dst[sb++] = 0x00;
    for (let i = 0; i < frame_len; ++i) {
      let b = src[df + i];
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

function init() {}

test("test byte-destuffing code", () => {
  init();
  stuff(stuffed_bytes, stuffed_bytes.length, random_bytes, random_bytes.length);
  byte_unstuffing.set_callback((arr, ct) => {
    for (let i = 0; i < arr.length; ++i) {
      destuffed_bytes[dsb_len++] = arr[i];
    }
    destuffed_frames_ct++;
  });
  byte_unstuffing.set_data(stuffed_bytes.subarray(0, sb_len));
  byte_unstuffing.process_data();
  expect(stuffed_frames_ct).toBe(FRAME_CT);
  expect(destuffed_frames_ct).toBe(FRAME_CT);
  //TEST_ASSERT_EQUAL_UINT8_ARRAY(random_bytes, destuffed_bytes, dsb_len);
});
