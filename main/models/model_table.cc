#include "model_table.hh"
////////////////////////// Add Models here //////////////////////////////////
#include "exp/PrintDiag.hh"
#include "raw/PrintDiag.hh"
#include "x53b_740/PrintDiag.hh"
static model_table_t our_model_table[] = {
    {
        .new_pcd_fun = []() -> PrintCarDiag* { return new PrintDiagRaw; },
        .names = {"raw"},
        .description = "universal. raw data print only)",
    },
    {
        .new_pcd_fun = []() -> PrintCarDiag* { return new PrintDiagExp; },
        .names = {"exp"},
        .description = "Experimental X53B-740 (R19, 54kW, TBI, Manual)",
    },
    {
        .new_pcd_fun = []() -> PrintCarDiag* { return new PrintDiagX53b740; },
        .names = {"x53b-740", "73PS"},
        .description = "X53B-740 (R19, 54kW, TBI, Manual)",
    },
};
/////////////////////////////////////////////////////////////////////////////
model_table_t* model_table = our_model_table;
size_t model_table_size = sizeof our_model_table / sizeof our_model_table[0];