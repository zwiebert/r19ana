import { expect, test } from "vitest";
import { byte_unstuffing } from "./byte_unstuffing";
import { RenixDestuffer } from "./renix_destuffer";

const FRAME_CT = 503;
const PACKET_LEN = 30;
const size = PACKET_LEN * FRAME_CT;
const random_bytes = getPseudoRandomBytes(size);
const stuffed_bytes = new Uint8Array(size * 2);
let sb_len = 0;
let stuffed_frames_ct = 0;
const destuffed_bytes = new Uint8Array(size * 2);
let dsb_len = 0;
let destuffed_frames_ct = 0;

function getPseudoRandomBytes(size: number) {
  const arr = Uint8Array.from({ length: size }, () => Math.floor(Math.random() * 256));
  for (let i = 0; i < FRAME_CT; ++i) {
    arr[i * PACKET_LEN] = 0;
  }
  return arr;
}

function stuff(dst: Uint8Array, dst_size: number, src: Uint8Array, src_len: number, frame_len: number = PACKET_LEN) {
  let sb = 0;
  let frame_ct = 0;
  for (let df = 0; df + frame_len <= src_len; df += frame_len) {
    dst[sb++] = 0xff;
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
  console.log("source:", random_bytes);
  console.log("stuffed:", stuffed_bytes);
}

function init() {}

test("test byte-destuffing code", () => {
  init();
  stuff(stuffed_bytes, stuffed_bytes.length, random_bytes, random_bytes.length);
  //const unstuffing = new byte_unstuffing((arr: Uint8Array, ct: number) => {

  const unstuffing = new RenixDestuffer((arr: Uint8Array, ct: number) => {
    for (let i = 0; i < arr.length; ++i) {
      destuffed_bytes[dsb_len++] = arr[i];
    }
    destuffed_frames_ct++;
  });

  const chunk_len = 20;
  for (let i=0; i < sb_len; i += chunk_len){
    const len = i + chunk_len < sb_len ? chunk_len : sb_len - i;
     unstuffing.process_chunk(stuffed_bytes.subarray(i, i + len));
  }
  console.log("dsb_len", dsb_len);
  expect(random_bytes.subarray(0, dsb_len)).toEqual(destuffed_bytes.subarray(0, dsb_len));
  expect(stuffed_frames_ct).toBe(FRAME_CT);
  expect(destuffed_frames_ct).toBe(FRAME_CT);
});
