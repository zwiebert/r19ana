#pragma once
#include "PrintCarDiag.hh"
extern class Transport&& term_transport;
extern class Transport&& xr25_transport;
extern class Transport&& mock_loop_transport;
extern class X53b740Frame R19_frame;
extern PrintCarDiag::line_view_mask_t Mask;

int terminal_puts(const char* s, bool block = false);
bool cli_parse_and_execute_cmdline(char* src);
int r19_alloc_and_print(char*& dst, const PrintCarDiag& print_diag,
                        const PrintCarDiag::line_view_mask_t& mask);

/**
 * @brief          print hex string from binary array
 *
 * @param src      source data array
 * @param src_len  byte length of source data array
 * @param dst      array to write the output
 * @param dst_size size of destination
 * @param separator a string which is placed as separator between hex-bytes.
 * @return int     the size of the written output excluding the terminating
 * null. if greater or equal than dst_size, than its the size your dst buffer
 * needs to have, excluding the terminating null. Call again with this return
 * value + 1
 */
int bin2hex(const uint8_t* src, size_t src_len, char* dst, size_t dst_size,
            const char* separator = "");