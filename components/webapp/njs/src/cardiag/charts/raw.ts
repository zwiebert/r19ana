import { raw_parser } from "../parser/raw";
import type { Icar_chart, ILabel } from "./iface";
export type { Icar_chart, ILabel };

let nmbGraphs = 29;

const labels: ILabel[] = [];
for (let g = 0; g <= 64; ++g) {
  labels.push({ series_label: `Byte-${g}`, axis_label: `Byte-${g}`, range:[0, 255] });
}

export class raw_chart implements Icar_chart {
  private yn_arr: (number | boolean)[][] = Array.from({ length: 64 }, () => []);
  get_info() {
    return { name: "Raw", description: "shows all bytes of the frame as graphs." };
  }
  clear_chart_data() {
    this.yn_arr.forEach((subArray) => (subArray.length = 0));
  }

  get_chart_data() {
    return this.yn_arr;
  }
  get_nmb_of_graphs() {
    return nmbGraphs;
  }

  get_labels() {
    return labels;
  }

  get_label(n) {
    return labels[n];
  }

  process_frame(arr: Uint8Array, ct: number, append: boolean = false) {
    if (!this.yn_arr) return;
    nmbGraphs = arr.length;
    if (ct == 0 && !append) {
      this.clear_chart_data();
      console.log("raw-charts process_frame()");
    } else if (arr.length != nmbGraphs) {
      return false;
    }
    const m = new raw_parser(arr);
    let idx = 0;
    for (let g = 0; g < nmbGraphs; ++g) {
      console.assert(this.yn_arr.length >= nmbGraphs);
      this.yn_arr[idx++].push(m.get_byte(g));
    }
    return true;
  }
}

export function raw_chart_factory() {
  return new raw_chart() as Icar_chart;
}
