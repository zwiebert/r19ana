#include "select_model.hh"

#include <string.h>

#include "PrintCarDiag.hh"
#include "model_table.hh"
#include "pers_storage.hh"

PrintCarDiag* print_car_diag;

const char* select_model(const int model_idx, const bool persistent) {
  if (model_idx < 0) return nullptr;
  auto idx = model_idx;

  for (auto it = model_table_begin(), end = model_table_end(); it != end;
       ++it) {
    if (idx-- != 0) continue;

    if (auto pcd = it->new_pcd_fun()) {
      delete print_car_diag;
      print_car_diag = pcd;
      if (persistent) pers_stor::set_model(model_idx);
      return it->description;
    }
  }
  return nullptr;
}

const char* select_model(const char* id, bool persistent) {
  int idx = 0;
  for (auto it = model_table_begin(), end = model_table_end(); it != end;
       ++it, ++idx) {
    if (it->match_name(id)) {
      return select_model(idx, persistent);
    }
  }
  return nullptr;
}

void select_model_init() { select_model(pers_stor::get_model(), false); }
