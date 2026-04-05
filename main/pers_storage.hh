#include <cstddef>
#include <string>
#pragma once

namespace pers_stor {
int get_model(int default_val = 0);
bool set_model(int model);

bool set_enable_auto_mount(bool auto_mount);
bool get_enable_auto_mount(bool default_val = true);

bool set_log_file_name(const char *name);
std::string get_log_file_name(const char *default_val = "xr25.bin");

bool set_enable_logging(bool enable);
bool get_enable_logging(bool default_val = true);
};