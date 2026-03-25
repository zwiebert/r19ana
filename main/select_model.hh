#pragma once

#include "PrintCarDiag.hh"
#include "XR25Frame.hh"

extern PrintCarDiag * print_car_diag;

void select_model_init();

const char *select_model(const char *id, bool persistent = true);
