#include "cli.hh"

#include <iterator>

#include "pers_storage.hh"

constexpr const char* reply_msg_all_hidden =
    "all entries are now hidden!\n"
    "Use <hide 0> to unhide all.\n"
    "Or use <show 1,2,3> to show some.\n";

static CliCmd cmds[] = {
    {.name = "show ",
     .help_txt =
         "\"show\" shows lines by number\n"
         "show 1,2,3  - make hidden lines 1,2 and 3 visible\n"
         "show 0 - hides all lines. It shows nothing.\n"
         "show 0,1,2,3 - shows only lines 1,2 and 3 and hide all others\n",
     .handler = [](CliCmd& cmd) -> bool {
       // command line was like: "filter 1,2,3,8,12".
       // cmd.args is now "1,2,3,8,12"
       // we need to convert this into a bitset<32> with only bits 0,1,2,7,11
       // are set to true.
       PrintCarDiag::line_view_mask_t mask = Mask;
       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", \r\n", &save_ptr)); str = nullptr) {
         if (strcasecmp("help", tok) == 0) {
           cmd.reply(cmd.help_txt);
           return true;
         }

         auto n = strtoul(tok, nullptr, 10);
         if (n == 0) {
           if (strstr(tok, "0")) mask.reset();
         } else
           mask.set(n - 1);
       }
       Mask = mask;
       if (!mask.any()) cmd.reply(reply_msg_all_hidden);
       return true;
     }},

    {.name = "hide ",
     .help_txt =
         "\"hide\" hides lines by number\n"
         "hide 1,2,3  - remove lines 1,2 and 3 from output\n"
         "hide 0 - makes all lines appear again. It hides nothing.\n"
         "hide 0,1,2,3 - hide only lines 1,2 and 3 and show all others\n",
     .handler = [](CliCmd& cmd) -> bool {
       // command line was like: "filter 1,2,3,8,12".
       // cmd.args is now "1,2,3,8,12"
       // we need to convert this into a bitset<32> with only bits 0,1,2,7,11
       // are set to true.
       PrintCarDiag::line_view_mask_t mask = Mask;
       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", \r\n", &save_ptr)); str = nullptr) {
         if (strcasecmp("help", tok) == 0) {
           cmd.reply(cmd.help_txt);
           return true;
         }

         auto n = strtoul(tok, nullptr, 10);
         if (n == 0) {
           if (strstr(tok, "0")) mask.set();
         } else
           mask.reset(n - 1);
       }
       Mask = mask;
       if (!mask.any()) cmd.reply(reply_msg_all_hidden);
       return true;
     }},

#ifdef ESP_PLATFORM
    {.name = "sd ",
     .help_txt = "SD-MMC card functions.\n"
                 "Connect UART1-TX with UART2-RX to process the data\n",

     .handler = [](CliCmd& cmd) -> bool {
       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", \r\n", &save_ptr)); str = nullptr) {
         if (strcasecmp("help", tok) == 0) {
           cmd.reply(cmd.help_txt);
           return true;
         }
         if (strcmp(tok, "mount") == 0) {
           data_storage->mount_fs();
           // data_storage->request_mount();
           return true;
         } else if (strcmp(tok, "umount") == 0) {
           data_storage->umount_fs();
           // data_storage->request_umount();
           return true;
         } else if (strcmp(tok, "auto-mount") == 0) {
           cmd.reply("sd-card will be mounted at MCU restart\n");
           return pers_stor::set_enable_auto_mount(true);
         } else if (strcmp(tok, "no-auto-mount") == 0) {
           cmd.reply("sd-card will NOT be mounted at MCU restart\n");
           return pers_stor::set_enable_auto_mount(false);
         } else if (strcmp(tok, "status") == 0) {
           bool is_mounted = data_storage->is_mounted();
           bool is_ready = data_logfile->is_ready();
           char buf[128];
           if (sizeof buf > snprintf(buf, sizeof buf,
                                     "status: mount=%d,auto-mount=%d\n",
                                     data_storage->is_mounted(),
                                     pers_stor::get_enable_auto_mount())) {
             cmd.reply(buf);
           }
           return is_mounted;
         } else {
           return false;  // unknown argument
         }
       }
       return false;
     }},
    {.name = "mock-loop ",
     .help_txt = "Sends pre-recorded data on UART1.\n"
                 "mount\n"
                 "umount\n",
     .handler = [](CliCmd& cmd) -> bool {
       static std::thread mock_uart_thread;
       static bool keep_running;

       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", \r\n", &save_ptr)); str = nullptr) {
         if (strcasecmp("help", tok) == 0) {
           cmd.reply(cmd.help_txt);
           return true;
         }
         if (strcmp(tok, "on") == 0) {
           void mock_uart_fun(bool& keep_running);
           if (keep_running) return false;
           keep_running = true;
           mock_uart_thread =
               std::thread(mock_uart_fun, std::ref(keep_running));
           return true;
         } else if (strcmp(tok, "off") == 0) {
           if (!keep_running) return false;
           keep_running = false;
           mock_uart_thread.join();
           return true;
         } else {
           return false;  // unknown argument
         }
       }
       return false;
     }},
#endif
    {.name = "model ",
     .help_txt =
         "Select car-model. Currently available are:\n"
         "x53b-740 - Renault 19 54kW, TBI, manual transmission (aliases: "
         "73PS)\n"
         "raw - Just prints the frame data (hex-string) and frame counter\n"
         "exp - experimental frame to figure out meaning of data and formulas "
         "on my x53b-740\n",
     .handler = [](CliCmd& cmd) -> bool {
       for (char *str = cmd.args, *save_ptr = nullptr, *tok;
            (tok = strtok_r(str, ", \r\n", &save_ptr)); str = nullptr) {
         if (strcasecmp("help", tok) == 0) {
           cmd.reply(cmd.help_txt);
           return true;
         }

         if (auto s = select_model(tok)) {
           cmd.reply(s);
           return true;
         }

         return false;
       }
       return false;
     }},
};

CliCmd* cli_cmds_begin() { return std::begin(cmds); }
CliCmd* cli_cmds_end() { return std::end(cmds); }
