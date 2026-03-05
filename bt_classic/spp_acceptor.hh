#pragma once

#include <cstdint>

typedef int (*spp_write_cb_t)(char *dst, std::size_t dst_size);
typedef void (*spp_read_cb_t)(const char *src, std::size_t src_len);

void spp_main(spp_read_cb_t rcb, spp_write_cb_t wcb);
