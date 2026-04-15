export function get_raw_parser(data_frame = []) {
  return raw.set_data_frame(data_frame);
}

let raw = {
  data_frame: [],
  set_data_frame: function (data) {
    this.data_frame = data;
    return this;
  },
  get_byte: function (idx) {
    return this.data_frame[idx];
  },
  get_length: function () {
    return this.data_frame.length;
  },
};
