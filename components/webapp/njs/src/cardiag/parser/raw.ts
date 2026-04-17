export function get_raw_parser(data_frame: Uint8Array) {
  return raw.set_data_frame(data_frame);
}

let raw = {
  data_frame: null as Uint8Array | null,
  set_data_frame: function (data:Uint8Array) {
    this.data_frame = data;
    return this;
  },
  get_byte: function (idx: number) {
    return this.data_frame?.[idx];
  },
  get_length: function () {
    return this.data_frame?.length;
  },
};
