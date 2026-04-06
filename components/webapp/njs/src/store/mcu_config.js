'use strict';
import { derived } from 'svelte/store';
import { OptionStore } from './custom_stores.js';


export const McuConfig = OptionStore();
export const McuConfigKeys = derived(McuConfig, mcuConfig => Object.keys(mcuConfig) || []); // XXX: ordering of keys?


export const McuGpiosFree = derived(McuConfig, (cfg) => {
    const gpios = cfg.gpio || "";
    let result = [];
  
    for (let i = 0; i < gpios.length; ++i) {
      if (gpios.charAt(i) !== "d") continue;
      result.push(i);
    }
    return result;
  });
  
  