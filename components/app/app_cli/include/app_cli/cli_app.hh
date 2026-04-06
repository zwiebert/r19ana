/**
 * \file   cli/cli_app.hh
 * \brief  CLI main header file
 */
#include <jsmn/jsmn_iterate.hh>
#include <cli/cli_types.h>


namespace app::cli {

bool process_objJson(class UoutWriter &td, jsoneat::Jsmn_String::Iterator &it);


extern const char cli_help_parmKvs[];
extern const char cli_help_parmCmd[];
extern const char cli_help_parmConfig[];
extern const char cli_help_parmMcu[];
extern const char cli_help_parmStatus[];
extern const char cli_help_parmHelp[];

int process_parmConfig(clpar p[], int len, class UoutWriter &td);
int process_parmHelp(clpar p[], int len, class UoutWriter &td);
int process_parmCmd(clpar p[], int len, class UoutWriter &td);
int process_parmMcu(clpar p[], int len, class UoutWriter &td);
int process_parmKvs(clpar p[], int len, class UoutWriter &td);
int process_parmStatus(clpar p[], int len, class UoutWriter &td);
int process_parmProtoBuf(clpar p[], int len, class UoutWriter &td);
bool cli_checkStm32CommandLine(char *line);

void cliApp_setup();

} // namespace
