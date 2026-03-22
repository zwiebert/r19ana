#pragma once

#include <array>
#include <cstring>

enum i18n_txt : unsigned {
  test,
  Crankshaft,
  Battery,
  Atmosphere,
  Manifold,
  Intake_Air,
  Coolant,
  Injection_Duration,
  O2_Sensor,
  Engine_Knock,
  Throttle_Full_Power,
  Throttle_Idle,
  EGR_enabled,
  O2_sensor_loop,
  Fuel_Pump,
  Advance,
  Idle_Regulation,
  Idle_Period,
  Knock_Delay,
  Throttle,
  Fault_Flags,
  Fault_Fugitive,

  //
  i18n_txt_SIZE
};

enum i18_lang : unsigned {
  en,
  de,
  //
  i18n_lang_SIZE
};

class I18n {
 public:
  I18n() {
    m_txt[i18n_txt::Crankshaft] = {
        "Crankshaft",
        "Kurbelwelle",
    };
    m_txt[i18n_txt::Battery] = {
        "Battery",
        "Batterie",
    };
    m_txt[i18n_txt::Atmosphere] = {
        "Atmosphere",
        "Atmosphäre",
    };
    m_txt[i18n_txt::Manifold] = {
        "Manifold",
        "Ansaugkrümmer",
    };
    m_txt[i18n_txt::Intake_Air] = {
        "Intake Air",
        "Ansaugluft",
    };
    m_txt[i18n_txt::Coolant] = {
        "Coolant",
        "Kühlwasser",
    };
    m_txt[i18n_txt::Injection_Duration] = {
        "Injection Duration",
        "Einspritzdauer",
    };
    m_txt[i18n_txt::O2_Sensor] = {
        "O2 Sensor",
        "Lambda-Sonde",
    };
    m_txt[i18n_txt::Engine_Knock] = {
        "Engine-Knock",
        "Klopfen",
    };
    m_txt[i18n_txt::Throttle_Full_Power] = {
        "Throttle Full-Power",
        "DK-Vollgas",
    };
    m_txt[i18n_txt::Throttle_Idle] = {
        "Throttle Idle",
        "DK-Leerlauf",
    };
    m_txt[i18n_txt::EGR_enabled] = {
        "EGR+EVap enabled",
        "AGR aktiviert",
    };
    m_txt[i18n_txt::O2_sensor_loop] = {
        "O2 sensor loop",
        "Lambda Regelschleife",
    };
    m_txt[i18n_txt::Fuel_Pump] = {
        "Fuel-Pump",
        "Kraftstoff-Pumpe",
    };
    m_txt[i18n_txt::Advance] = {
        "Advance",
        "Zündwinkel",
    };
    m_txt[i18n_txt::Idle_Regulation] = {
        "Idle-Regulation",
        "Leerlaufregulierung",
    };
    m_txt[i18n_txt::Idle_Period] = {
        "Idle-Period",
        "Leerlauf-Periode",
    };
    m_txt[i18n_txt::Knock_Delay] = {
        "Knock-Delay",
        "Klopf-Verstellung",
    };
    m_txt[i18n_txt::Throttle] = {
        "Throttle",
        "Drosselklappe",
    };

    m_txt[i18n_txt::Fault_Flags] = {
        "Fault-Flags",
        "Fehler-Flags",
    };
    m_txt[i18n_txt::Fault_Fugitive] = {
        "Fault-Fugitive",
        "Flüchtiger Fehler",
    };
  }
  bool set_lang(i18_lang lang) {
    m_lang = lang;
    return true;
  }
  bool set_lang(const char* lang) {
    if (strcmp(lang, "en") == 0) {
      m_lang = i18_lang::en;
      return true;
    } else if (strcmp(lang, "de") == 0) {
      m_lang = i18_lang::de;
      return true;
    }
    return false;
  }

  i18_lang get_lang() const { return m_lang; }

  const char* translate(const char* str, i18_lang from_lang, i18_lang to_lang) {
    for (const auto& sa : m_txt) {
      if (!(sa[from_lang])) continue;
      if (std::strcmp(str, sa[from_lang]) == 0) return sa[to_lang];
    }
    return nullptr;
  }

 private:
  std::array<std::array<const char*, i18n_lang_SIZE>, i18n_txt_SIZE> m_txt;
  i18_lang m_lang = i18_lang::de;
};

extern I18n i18n;

inline const char* _(const char* txt) {
  if (i18n.get_lang() == i18_lang::en) return txt;
  auto txt_translated = i18n.translate(txt, i18_lang::en, i18n.get_lang());
  return txt_translated ? txt_translated : txt;
}
const char* _(i18n_txt txt);