#include <cstddef>
#pragma once

namespace pers_stor {
int get_model();
bool set_model(int model);
bool set_enable_auto_mount(bool auto_mount);
bool get_enable_auto_mount();
};