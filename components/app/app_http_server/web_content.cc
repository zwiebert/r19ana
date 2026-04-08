#include "web_content.hh"
#include "app_cli/cli_app.hh"


#include <iterator>
#include <cstring>

static FileContentReader file_content_reader;



 extern const uint8_t r19data_bin_start[] asm("_binary_r19data_bin_start");
 extern const uint8_t r19data_bin_end[] asm("_binary_r19data_bin_end");
 

static const struct file_map uri_file_map[] = { //
        { .uri = "/f/mnt/sdcard/xr25.bin", .type = "application/octet-stream", .wc = {.content = "/f/mnt/sdcard/xr25.bin", .content_encoding = nullptr, .content_length = 0}, .content_reader = &file_content_reader }, //
        { .uri = "/f/sdcard/xr25.bin", .type = "application/octet-stream", .wc = {.content = (const char *)r19data_bin_start, .content_encoding = nullptr, .content_length = unsigned(r19data_bin_end - r19data_bin_start)} }, //
    { .uri = "/", .type = "text/html", .wc = wapp_html_gz_fm  }, //
        { .uri = "/f/js/wapp.js", .type = "text/javascript", .wc = wapp_js_gz_fm  }, //
#ifdef CONFIG_APP_USE_HTTP_SERVE_JS_MAP
        { .uri = "/f/js/wapp.js.map", .type = "application/json", .wc = wapp_js_map_gz_fm }, //
#endif
        { .uri = "/f/css/wapp.css", .type = "text/css", .wc = wapp_css_gz_fm  }, //
#ifdef CONFIG_APP_USE_HTTP_SERVE_CSS_MAP
        { .uri = "/f/css/wapp.css.map", .type = "application/json", .wc =  wapp_css_map_gz_fm, .content_reader = nullptr   }, //
#endif
        { .uri = "/f/cli/help/config", .type = "text/plain;charset=\"UTF-8\"", .wc = { .content = app::cli::cli_help_parmConfig }, .content_reader = nullptr  }, //
        { .uri = "/f/cli/help/mcu", .type = "text/plain;charset=\"UTF-8\"", .wc = { .content = app::cli::cli_help_parmMcu }, .content_reader = nullptr  }, //
        { .uri = "/f/cli/help/help", .type = "text/plain;charset=\"UTF-8\"", .wc = { .content = app::cli::cli_help_parmHelp }, .content_reader = nullptr  }, //
        { .uri = "/f/cli/help/kvs", .type = "text/plain;charset=\"UTF-8\"", .wc = { .content = app::cli::cli_help_parmKvs }, .content_reader = nullptr  }, //
        { .uri = "/f/cli/help/cmd", .type = "text/plain;charset=\"UTF-8\"", .wc = { .content = app::cli::cli_help_parmCmd }, .content_reader = nullptr  }, //
};


const struct file_map* wc_getContent(const char *uri) {
  for (auto it = std::begin(uri_file_map); it != std::end(uri_file_map); ++it) {
    const char  *q = strchr(uri, '?');
    if ((q && 0 == strncmp(uri, it->uri, q - uri)) || 0 == strcmp(uri, it->uri))
      return it;
  }
  return nullptr;
}

