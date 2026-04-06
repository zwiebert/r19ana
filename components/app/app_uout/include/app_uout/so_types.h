#pragma once

#include <uout/uo_types.h>
#include <stdint.h>

typedef struct {
  bool on;
} so_arg_on_t;

typedef struct {
  uint32_t state_bits, changed_bits;
} so_arg_valves_t;

typedef struct {
  const char *key;
  uint8_t *buf;
  int buf_len;
} so_arg_pbuf_t;

typedef struct {
  const char *key, *val;
} so_arg_kvs_t;

typedef struct {
  const char *key;
  int val;
} so_arg_kvd_t;

