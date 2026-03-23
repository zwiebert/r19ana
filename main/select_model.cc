#include "select_model.hh"

#include <string.h>

#include "models/x53b_740/PrintDiagX53b740.hh"
static PrintDiagX53b740 print_diag_X53b740;
#include "models/raw/PrintDiagRaw.hh"
static PrintDiagRaw print_diag_raw;

PrintCarDiag* print_car_diag = &print_diag_X53b740;  // default

const char* select_model(const char* id) {
  if (strcasecmp(id, "x53b-740") == 0) {
    print_car_diag = &print_diag_X53b740;
    return "X53B-740 (R19, 54kW, TBI, Manual)";
  }
  if (strcasecmp(id, "raw") == 0) {
    print_car_diag = &print_diag_raw;
    return "universal. raw data print only)";
  }
  return nullptr;
}
