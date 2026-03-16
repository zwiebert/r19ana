#pragma once
#include "R19Frame_utils.hh"

extern class Transport&& term_transport;
extern class Transport&& xr25_transport;
extern class Transport&& mock_loop_transport;
extern class R19Frame R19_frame;
extern r19frame_mask_t Mask;

int terminal_puts(const char* s, bool block = false);