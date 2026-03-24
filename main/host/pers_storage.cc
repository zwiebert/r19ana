#include "pers_storage.hh"

static int our_model;

namespace pers_stor {

bool set_model(int model) {
  our_model = model;
  return true;
}
int get_model() { return our_model; }

}  // namespace pers_stor
