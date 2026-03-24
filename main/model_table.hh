

#include <string.h>

struct model_table_t {
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

extern size_t model_table_size;
extern model_table_t* model_table;