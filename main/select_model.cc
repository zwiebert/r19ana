#include "select_model.hh"

#include <string.h>

#include "pers_storage.hh"

struct model_table_t {
  using new_pcd_fun_t = PrintCarDiag* (*)();
  new_pcd_fun_t new_pcd_fun;
  const char* names[4];
  const char* description;

  bool match_name(const char* name) const {
    for (auto nm : names) {
      if (!nm) continue;
      if (strcasecmp(nm, name) == 0) return true;
    }
    return false;
  }
};

#include "models/raw/PrintDiag.hh"
#include "models/x53b_740/PrintDiag.hh"
#include "models/exp/PrintDiag.hh"

static model_table_t our_model_table[] = {
    {
        .new_pcd_fun = []() -> PrintCarDiag* { return new PrintDiagRaw; },
        .names = {"raw"},
        .description = "universal. raw data print only)",
    },
    {
        .new_pcd_fun = []() -> PrintCarDiag* { return new PrintDiagExp; },
        .names = {"exp"},
        .description = "Experimental X53B-740 (R19, 54kW, TBI, Manual)",
    },
    {
        .new_pcd_fun = []() -> PrintCarDiag* { return new PrintDiagX53b740; },
        .names = {"x53b-740", "73PS"},
        .description = "X53B-740 (R19, 54kW, TBI, Manual)",
    },
};
constexpr size_t our_model_table_size =
    sizeof our_model_table / sizeof our_model_table[0];
PrintCarDiag* print_car_diag = new PrintDiagRaw;

const char* select_model(int model, bool permanent) {
  if (0 <= model && model < our_model_table_size) {
    auto& model_table = our_model_table[model];
    if (auto pcd = model_table.new_pcd_fun()) {
      delete print_car_diag;
      print_car_diag = pcd;
      if (permanent) pers_stor::set_model(model);
      return model_table.description;
    }
  }
  return nullptr;
}

const char* select_model(const char* id, bool permanent) {
  for (int i = 0; i < our_model_table_size; ++i) {
    auto& model_table = our_model_table[i];
    if (model_table.match_name(id)) {
      return select_model(i, permanent);
    }
  }
  return nullptr;
}

void select_model_init() {
  auto model = pers_stor::get_model();
  select_model(model, false);
}