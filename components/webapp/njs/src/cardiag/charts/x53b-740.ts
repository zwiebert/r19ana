import { x53b_740_parser, x53b_740_metrics_table, x53b_740_metrics_table_pos } from "../parser/x53b-740";

const t = x53b_740_metrics_table;
const e = x53b_740_metrics_table_pos;
const order = [
  e.engine_speed,
  e.map_sensor,
  e.ignition_advance,
  e.o2_sensor,
  e.injection_duration,
  e.idle_switch,
  ///////////////////
  e.batt_voltage,
  e.fuel_pump_relay,
  e.ect_sensor,
  e.iat_sensor,
  e.engine_pinking,
  e.detonation_correction,
  //////////////
  e.richness_regulation,
  e.engine_speed,
  e.richness_adaption_idle2low,
  e.richness_adaption_avg2high,
  e.idle_regulation,
  e.idle_adaption,
];

let labels = new Array(order.length);
for (let i = 0; i < order.length; ++i) {
  const table_entry = t[order[i]];
  labels[i] = { series_label: table_entry.short_name, axis_label: table_entry.unit };
}

export class x53b_740_chart {
  private yn_arr: any[][] = Array.from({ length: order.length }, () => []);

  get_info () {
    return { name: "X53B_740", description: "this is the R19-F3N740 (54kW, TBI, manual)" };
  }

  clear_chart_data () {
    this.yn_arr.forEach(subArray => subArray.length = 0);
  }

  get_chart_data () {
    return this.yn_arr;
  }
  get_labels () {
    return labels;
  }
  get_label (n) {
    return labels[n];
  }
  get_nmb_of_graphs () {
    return order.length;
  }

  process_frame (arr: Uint8Array, ct: number, append: boolean = false) {
    if (ct == 0 && !append) {
      this.clear_chart_data();
    }
    console.assert(this.yn_arr.length === order.length);
    let m = new x53b_740_parser(arr);
    let idx =  0;
    for (let i in order) {
      console.assert(this.yn_arr[idx]!== undefined);
      this.yn_arr[idx++].push(t[i].parse.call(m));
    }
    //this.yn_arr = this.yn_arr;
  }
};
