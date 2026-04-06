#pragma once
#include "cli/cli.h"
#include "app_cli/cli_app.hh"
#include <cli/cli_out.hh>

extern const char pin_state_args[];


namespace app::cli {

#define NODEFAULT() if (val==0) return cli_replyFailure(td)

void cli_out_config_reply_entry(const char *key, const char *val, int len);
void cli_out_mcu_reply_entry(const char *key, const char *val, int len);


bool config_receiver(const char *val);
bool config_transmitter(const char *val);

bool asc2u8(const char *s, uint8_t *n, uint8_t limit);

} // namespace
