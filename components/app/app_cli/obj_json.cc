#include <app_cli/cli_app.hh>
#include <jsmn/jsmn_iterate.hh>
#include <debug/log.h>
#include <uout/uout_writer.hh>
#ifdef CONFIG_APP_USE_WEATHER_AUTO
#include <full_auto/adapter.hh>
#include <full_auto/ui_handler_auto.hh>
#include <full_auto/setup.hh>
#endif
#define logtag  "rv.cli"

namespace app::cli {

bool process_objJson(UoutWriter &td, jsoneat::Jsmn_String::Iterator &it) {
  db_loge("test.json_obj", "all_json: <%s>", it.get_json());

  assert(it->type == JSMN_OBJECT);

  using token_handler_fun_type = bool (*)(class UoutWriter &td, jsoneat::Jsmn_String::Iterator &it, int &err);
  static const token_handler_fun_type tok_processRootChilds_funs[] = { //

#ifdef CONFIG_APP_USE_WEATHER_AUTO
      [](class UoutWriter &td, jsoneat::Jsmn_String::Iterator &it, int &err) -> bool {
        if (it.keyIsEqual("auto", JSMN_OBJECT)) {
          return app::fa::ui::handler__auto(app::fa::full_auto->auto_timer(), td, ++it);
        }
        return false;

      },
#endif
      [](class UoutWriter &td, jsoneat::Jsmn_String::Iterator &it, int &err) -> bool {
        return it.skip_key_and_value();
;
      } };

  int err = 0;
  auto count = it->size;
  for (++it; count > 0 && it; --count) {
    for (auto fun : tok_processRootChilds_funs) {
      if (fun(td, it, err))
        break;
    }

  }
  return !err;
  return false;
}

} // namespace

