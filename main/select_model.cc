#include "select_model.hh"

#include <string.h>

#include "PrintCarDiag.hh"
#include "model_table.hh"
#include "pers_storage.hh"

PrintCarDiag* print_car_diag;

const char* select_model(int model_idx, bool permanent) {
  if (model_idx < 0) return nullptr;

  for (auto it = model_table_begin(), end = model_table_end(); it != end;
       ++it) {
    if (model_idx-- != 0) continue;

    if (auto pcd = it->new_pcd_fun()) {
      delete print_car_diag;
      print_car_diag = pcd;
      if (permanent) pers_stor::set_model(model_idx);
      return it->description;
    }
  }
  return nullptr;
}

const char* select_model(const char* id, bool permanent) {
  int idx = 0;
  for (auto it = model_table_begin(), end = model_table_end(); it != end;
       ++it, ++idx) {
    if (it->match_name(id)) {
      return select_model(idx, permanent);
    }
  }
  return nullptr;
}

void select_model_init() { select_model(pers_stor::get_model(), false); }
