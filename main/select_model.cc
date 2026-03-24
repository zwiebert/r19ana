#include "select_model.hh"
#include "model_table.hh"

#include <string.h>

#include "PrintCarDiag.hh"
#include "pers_storage.hh"

PrintCarDiag* print_car_diag;

const char* select_model(int model_idx, bool permanent) {
  if (0 <= model_idx && model_idx < model_table_size) {
    auto& model = ::model_table[model_idx];
    if (auto pcd = model.new_pcd_fun()) {
      delete print_car_diag;
      print_car_diag = pcd;
      if (permanent) pers_stor::set_model(model_idx);
      return model.description;
    }
  }
  return nullptr;
}

const char* select_model(const char* id, bool permanent) {
  for (int i = 0; i < model_table_size; ++i) {
    if (model_table[i].match_name(id)) {
      return select_model(i, permanent);
    }
  }
  return nullptr;
}

void select_model_init() {
  select_model(pers_stor::get_model(), false);
}
