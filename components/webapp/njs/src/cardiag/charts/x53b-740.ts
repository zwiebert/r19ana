import { x53b_740_parser, x53b_740_metrics_table, x53b_740_metrics_table_pos } from "../parser/x53b-740";
import type { CarMetrics, Icar_chart, ILabel } from "./iface";
export type { Icar_chart, ILabel };

const t = x53b_740_metrics_table;
const e = x53b_740_metrics_table_pos;
const default_order = [
  e.engine_speed,
  e.idle_switch,

  e.map_sensor,
  e.unknown_xx,

  e.ignition_advance,
  e.full_load_switch,

  e.o2_sensor,
  e.fuel_pump_relay,

  e.injection_duration,
  e.batt_voltage,

  e.ect_sensor,
  e.iat_sensor,

  e.engine_pinking,
  e.detonation_correction,

  e.richness_regulation,
  e.unknown_x,

  e.richness_adaption_idle2low,
  e.richness_adaption_avg2high,

  e.idle_regulation,
  e.idle_adaption,

  e.status0,
  e.status1,

  e.status2,
  e.status3,

  e.status4,
  e.status5,

  e.status6,
  e.status7,
];

const default_labels = new Array<ILabel>(default_order.length);
for (let i = 0; i < default_order.length; ++i) {
  const table_entry = t[default_order[i]];
  default_labels[i] = { series_label: table_entry.short_name, axis_label: table_entry.unit, range: table_entry.range };
}

export class x53b_740_chart implements Icar_chart {
  private yn_arr: (number | boolean)[][] = Array.from({ length: default_order.length }, () => []);
  public nmbGraphs: number = default_order.length;
  public order = [...default_order];
  private labels = [...default_labels];

  get_info() {
    return { name: "X53B_740", description: "this is the R19-F3N740 (54kW, TBI, manual)" };
  }

  clear_chart_data() {
    this.yn_arr = Array.from({ length: this.order.length }, () => []);
    //this.yn_arr.forEach(subArray => subArray.length = 0);
  }

  get_chart_data() {
    return this.yn_arr;
  }
  get_labels() {
    return this.labels;
  }
  get_label(n: number) {
    return this.labels[n];
  }
  get_nmb_of_graphs() {
    return this.order.length;
  }

  get_car_metrics(): Array<CarMetrics> {
    return [...x53b_740_metrics_table];
  }
  get_order(): number[] {
    return this.order;
  }
  set_order(order: number[]): void {
    this.order.length = 0; // Clear contents
    this.order.push(...order); // Append new contents
  }

  process_data_packet(arr: Uint8Array, ct: number) {
    console.assert(this.yn_arr.length === this.order.length);
    const m = new x53b_740_parser(arr);
    let idx = 0;
    for (const i of this.order) {
      console.assert(this.yn_arr[idx] !== undefined);
      this.yn_arr[idx++].push(t[i].parse.call(m));
    }
    //this.yn_arr = this.yn_arr;
  }
}

export function x53b_740_chart_factory() {
  return new x53b_740_chart() as Icar_chart;
}
