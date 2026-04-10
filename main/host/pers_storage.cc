#include "pers_storage.hh"



static int our_model = -1;
static int enable_logging = false;
static std::string log_file_name;

namespace pers_stor {

bool set_model(int model) {
  our_model = model;
  return true;
}
int get_model(int def_val) { return our_model >= 0 ? our_model : def_val; }


bool set_enable_auto_mount(bool auto_mount) {
  return true;
}
bool get_enable_auto_mount(bool default_val) {
  return true;
}

bool set_enable_logging(bool enable) {
  enable_logging = enable;
}
bool get_enable_logging(bool default_val) {
  return enable_logging >= 0 ? enable_logging : default_val;
}

bool set_log_file_name(const char* name) {
  log_file_name = name;
}

std::string get_log_file_name(const char *default_val) {
  return log_file_name.empty() ? default_val : log_file_name;
}

}  // namespace pers_stor
