import { expect, test } from "vitest";
import { byte_unstuffing } from "./byte_unstuffing.js";

function getPseudoRandomBytes(size: number) {
  return Uint8Array.from({ length: size }, () => Math.floor(Math.random() * 256));
}
const unstuffing = new byte_unstuffing();
const FRAME_CT = 200;
const FRAME_LEN = 29;
const size = FRAME_LEN * FRAME_CT;
const random_bytes = getPseudoRandomBytes(size);
const stuffed_bytes = new Uint8Array(size * 2);
let sb_len = 0;
let stuffed_frames_ct = 0;
const destuffed_bytes = new Uint8Array(size * 2);
let dsb_len = 0;
let destuffed_frames_ct = 0;

function stuff(dst: Uint8Array, dst_size: number, src: Uint8Array, src_len: number, frame_len: number = 29) {
  let sb = 0;
  let frame_ct = 0;
  for (let df = 0; df + frame_len <= src_len; df += frame_len) {
    dst[sb++] = 0xff;
    dst[sb++] = 0x00;
    for (let i = 0; i < frame_len; ++i) {
      const b = src[df + i];
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
  unstuffing.set_callback((arr:Uint8Array, ct:number) => {
    for (let i = 0; i < arr.length; ++i) {
      destuffed_bytes[dsb_len++] = arr[i];
    }
    destuffed_frames_ct++;
  });
  unstuffing.set_data(stuffed_bytes.subarray(0, sb_len));
  unstuffing.process_data();
  expect(stuffed_frames_ct).toBe(FRAME_CT);
  expect(destuffed_frames_ct).toBe(FRAME_CT);
  expect(random_bytes.subarray(0, dsb_len)).toEqual(destuffed_bytes.subarray(0, dsb_len));
});
