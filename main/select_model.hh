#pragma once

#include "PrintCarDiag.hh"
#include "XR25Frame.hh"

extern PrintCarDiag const* print_car_diag;

const char *select_model(const char *id);

void push_frame(const XR25Frame::frame_data_t& xr25_frame, int frame_count);