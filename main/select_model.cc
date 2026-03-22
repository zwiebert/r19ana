#include "select_model.hh"

#include "PrintDiagX53b740.hh"

X53b740Frame R19_frame;
PrintDiagX53b740 print_diag_X53b740 = {R19_frame};
PrintCarDiag const* print_car_diag = &print_diag_X53b740;


const char *select_model(const char *id) {
  if (strcasecmp(id, "x53b-740") == 0) {
    print_car_diag = &print_diag_X53b740;
    return "X53B-740 (R19, 54kW, TBI, Manual)";
  }
  return nullptr;
}