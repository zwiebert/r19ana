
export class raw_parser {
  private data_frame: Uint8Array;

  constructor(data : Uint8Array) {
    this.data_frame = data;
  }
  set_data_frame (data:Uint8Array) {
    this.data_frame = data;
    return this;
  }
  get_byte (idx: number) {
    return this.data_frame?.[idx];
  }
  get_length () {
    return this.data_frame?.length ?? 0;
  }
};

export default raw_parser;