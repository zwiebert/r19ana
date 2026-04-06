import { writable } from 'svelte/store';
import { ArrayStore, PersistentIntStore } from './custom_stores';

export const ZoneCountMax = 14;
export const ZoneCount = writable(ZoneCountMax);

export const ZoneNames = ArrayStore([]);
export const ZoneDisabled= PersistentIntStore("zdis");
export const ZoneRemainingSeconds = ArrayStore([0,0,0,0,0,0,0,0,0,0,0,0,0,0]);
export const ZoneDescriptions = ArrayStore(["<enter names...>",0, 0,0,0,0,0,0,0,0,0,0,0,0]);
export const ZoneDurations = ArrayStore([0,0,0,0,0,0,0,0,0,0,0,0,0,0]);
export const ZoneLPHs = ArrayStore([0,0,0,0,0,0,0,0,0,0,0,0,0,0]);
export const ZoneDataMsg = writable(0);
export const ZoneTimers = writable({});

export const PressControlStatus = writable(false);
export const WaterPumpStatus = writable(false);
export const RainSensorStatus = writable(false);

export const Stm32Time = writable("");

export const ZonesAuto = ArrayStore([]);
export const WeatherAdapters = ArrayStore([]);
export const PastWeatherData = ArrayStore([]);


