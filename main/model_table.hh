

#include <string.h>

struct model_t {
  using new_pcd_fun_t = class PrintCarDiag* (*)();
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

model_t* model_table_begin();
model_t* model_table_end();