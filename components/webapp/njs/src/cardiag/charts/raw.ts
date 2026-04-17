import { raw_parser } from "../parser/raw";

let nmbGraphs = 29;

export default {
  yn_arr: Array.from({ length: nmbGraphs }, () => []) as any[][],
  get_info: function () {
    return { name: "Raw", description: "shows all bytes of the frame as graphs." };
  },
  clear_chart_data: function () {
    this.yn_arr.forEach((subArray) => (subArray.length = 0));
  },

  get_chart_data: function () {
    return this.yn_arr;
  },
  get_nmb_of_graphs: function () {
    return nmbGraphs;
  },

  get_labels: function () {
    let ret = [];
    for (let g = 0; g <= nmbGraphs; ++g) {
      ret.push({ series_label: `Byte-${g}`, axis_label: `Byte-${g}` });
    }
    return ret;
  },

  get_label: function (n) {
    return { series_label: `Byte-${n}`, axis_label: `Byte-${n}` };
  },

  process_frame: function (arr: Uint8Array, ct: number, append: boolean = false) {
    if (!this.yn_arr) return;
      nmbGraphs = arr.length;
    if (ct == 0 && !append) {
      this.clear_chart_data();
      console.log("raw-charts process_frame()");
    } else if (arr.length != nmbGraphs) {
      return false;
    }
    let m = new raw_parser(arr);
    let idx = 0;
    for (let g = 0; g < nmbGraphs; ++g) {
      this.yn_arr[idx++].push(m.get_byte(g));
    }
    return true;
  },
};
