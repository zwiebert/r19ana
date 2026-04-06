#include "app_config/proj_app_cfg.h"
#include "so_out.h"

#include "app_settings/config.h"
#include "txtio/inout.h"



void
so_print_startup_info(void) {
  static const char msg_starting[] = "\n\n" "tf: info: start: tronferno-mcu\n" "tf: info: build-date: " __DATE__ " " __TIME__ "\n";
  static const char msg_hint[] = "tf: hint: type 'help;' to get a command list\n";

  if (TXTIO_IS_VERBOSE(vrbNone)) {
    io_puts(msg_starting);
    io_puts(msg_hint);
  }
}
