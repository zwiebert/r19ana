import { get_raw_parser } from "../parser/raw";

export default {
  nmbGraphs: 29,
  yn_arr: null as Array<any>| null,
  get_info: function () {
    return { name: "Raw", description: "shows all bytes of the frame as graphs." };
  },
  clear_parsed_data: function () {
    this.yn_arr = Array(this.nmbGraphs)
      .fill()
      .map((e) => []);
  },

  get_parsed_data: function () {
    if (!this.yn_arr) this.clear_parsed_data();
    return this.yn_arr;
  },
  get_nmb_of_graphs: function () {
    return this.nmbGraphs;
  },

  get_labels: function () {
    let ret = [];
    for (let g = 0; g <= this.nmbGraphs; ++g) {
      ret.push({ series_label: `Byte-${g}`, axis_label: `Byte-${g}` });
    }
    return ret;
  },

  get_label: function (n) {
      return { series_label: `Byte-${n}`, axis_label: `Byte-${n}` };
  },

  process_frame: function (arr:Uint8Array, ct:number) {
    if (!this.yn_arr) return;
    if (ct == 0) {
      this.nmbGraphs = arr.length;
      this.clear_parsed_data();
      console.log("raw-charts process_frame()");
    } else if (arr.length != this.nmbGraphs) {
      return false;
    }
    let m = get_raw_parser(arr);
    let idx = 0;
    for (let g = 0; g < this.nmbGraphs; ++g) {
      this.yn_arr[idx++].push(m.get_byte(g));
    }
    return true;
  },
};
