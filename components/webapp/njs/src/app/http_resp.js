"use strict";

import { McuConfig } from "../store/mcu_config.js";
import {
  McuBootCount,
  McuGitTagNames,
  McuFirmwareBuildDate,
  McuChipId,
  McuFirmwareVersion,
  Stm32McuFirmwareVersion,
  McuFirmwareUpdProgress,
  McuFirmwareUpdState,
  McuTime,
} from "../store/mcu_firmware.js";
import { McuDocs } from "../store/mcu_docs.js";
import {
  ZonesAuto,
  PastWeatherData,
  WeatherAdapters,
  ZoneLPHs,
  ZoneDataMsg,
  ZoneCountMax,
  ZoneNames,
  ZoneRemainingSeconds,
  ZoneDurations,
  ZoneTimers,
  PressControlStatus,
  WaterPumpStatus,
  RainSensorStatus,
  Stm32Time,
} from "../store/zones.js";

export function http_handleResponses(obj) {
  if (!("mcu" in obj && "time" in obj.mcu))
    console.log("http/ws-in: " + JSON.stringify(obj));

  if ("config" in obj) {
    let config = obj.config;
    McuConfig.update(config);
  }

  if ("position" in obj) {
    document.getElementById("spi").value = obj.position.p;
  }

  if ("data" in obj || "update" in obj) {
    let update = "update" in obj;
    let data = obj.data || obj.update;
    if ("version" in data) {
      Stm32McuFirmwareVersion.set(data.version);
    }

    if ("timer" in data) {
      const timer = data.timer;
      const update_timer = function (t) {
        const key = "timer" + (t.vn || 0) + "." + (t.tn || 0);
        ZoneTimers.update((o) => {
          o[key] = data.timer;
          return o;
        });
      };
      if (!update) ZoneTimers.set({});

      if (Array.isArray(timer)) {
        timer.forEach(update_timer);
      } else {
        update_timer(timer);
      }
    }

    if ("dur" in data) {
      for (let i = 0; i < ZoneCountMax; ++i) {
        let key = i.toString() + ".0";
        ZoneDurations.update(i, key in data.dur ? data.dur[key] : 0);
      }
    }

    if ("rem" in data) {
      for (let i = 0; i < ZoneCountMax; ++i) {
        let key = i.toString() + ".0";
        ZoneRemainingSeconds.update(i, key in data.rem ? data.rem[key] : 0);
      }
    }

    if ("pc" in data) PressControlStatus.set(data.pc);
    if ("pump" in data) WaterPumpStatus.set(data.pump);
    if ("rain" in data) RainSensorStatus.set(data.rain);
    if ("time" in data) Stm32Time.set(data.time);
  }

  if ("rve" in obj) {
    let rve = obj.rve;
    if ("pump" in rve) WaterPumpStatus.set(rve.pump);
    if ("rain" in rve) RainSensorStatus.set(rve.rain);
    if ("pc" in rve) PressControlStatus.set(rve.pc);
  }

  if ("kvs" in obj) {
    let kvs = obj.kvs;

    for (let i = 0; i < ZoneCountMax; ++i) {
      let key = "zn" + i.toString();
      if (key in kvs) {
        ZoneNames.update(i, kvs[key]);
      }
      key = "lph" + i.toString();
      if (key in kvs) {
        ZoneLPHs.update(i, kvs[key]);
      }
    }
  }

  if ("auto" in obj) {
    let auto = obj.auto;
    if ("zones" in obj.auto) {
      let zones = obj.auto.zones;
      ZonesAuto.set(zones);
    }
    if ("adapters" in obj.auto) {
      let adapters = obj.auto.adapters;
      WeatherAdapters.set(adapters);
    }
    if ("past_wd" in obj.auto) {
      let past_wd = obj.auto.past_wd;
      PastWeatherData.set(past_wd);
    }
    for (let i = 0; i < ZoneCountMax; ++i) {
      let key = "zone." + i.toString();
      if (key in auto) {
        ZonesAuto.update(i, auto[key]);
      }
      key = "adapter." + i.toString();
      if (key in auto) {
        WeatherAdapters.update(i, auto[key]);
      }
    }
  }

  if ("mcu" in obj) {
    let mcu = obj.mcu;
    if ("time" in mcu) {
      McuTime.set(mcu.time);
    }
    if ("chip" in mcu) {
      McuChipId.set(mcu.chip);
    }
    if ("firmware" in mcu) {
      McuFirmwareVersion.set(mcu.firmware);
    }
    if ("build-time" in mcu) {
      McuFirmwareBuildDate.set(mcu["build-time"]);
    }
    if ("boot-count" in mcu) {
      McuBootCount.set(mcu["boot-count"]);
    }
    if ("ota-state" in mcu) {
      let ota_state = mcu["ota-state"];

      McuFirmwareUpdState.set(Number.parseInt(ota_state));
      handleOtaState(ota_state);
    }
    if ("stm32ota-state" in mcu) {
      let ota_state = mcu["stm32ota-state"];

      McuFirmwareUpdState.set(Number.parseInt(ota_state));
      handleOtaState(ota_state);
    }
  }
}

export function http_handleDocResponses(name, text) {
  let obj = {};
  obj[name] = { text: text };
  McuDocs.update(obj);
}

export function gitTags_handleResponse(json) {
  let names = [];
  json.forEach((item) => {
    names.push(item.name);
  });
  McuGitTagNames.set(names);
}

function handleOtaState(state) {
  if (state === 1) McuFirmwareUpdProgress.update((old) => old + 1);
  else McuFirmwareUpdProgress.set(0);
}
