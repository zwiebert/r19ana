#pragma once
#include "X53B_740_frame_utils.hh"

extern class Transport&& term_transport;
extern class Transport&& xr25_transport;
extern class Transport&& mock_loop_transport;
extern class X53b740Frame R19_frame;
extern r19frame_mask_t Mask;

int terminal_puts(const char* s, bool block = false);