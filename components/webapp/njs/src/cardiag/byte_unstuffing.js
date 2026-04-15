export default {
  m_arr: null,
  m_cb: null,
  set_callback: function (cb) {
    this.m_cb = cb;
  },
  xr25: {
    m_last_byte_was_ff: false,
    m_header_found: false,
    m_frame_complete: false,
    m_invalid_frame_ct: 0,
    m_rbuf: [],
    FRAME_MIN_SIZE: 29,
    FRAME_MAX_SIZE: 64,

    rbuf_clear: function () {
      this.m_rbuf = [];
      this.m_last_byte_was_ff = false;
      this.m_header_found = false;
      this.m_frame_complete = false;
    },
    add: function (b) {
      if (this.m_frame_complete) this.rbuf_clear();

      if (b == 0xff && !this.m_last_byte_was_ff) {
        this.m_last_byte_was_ff = true;
        return null;
      }

      if (b == 0x00 && this.m_last_byte_was_ff) {
        if (!this.m_header_found) {
          // this is the first frame.
          this.rbuf_clear();
          this.m_header_found = true;
          return null;
        }

        let frame_length = this.m_rbuf.length;

        if (frame_length < this.FRAME_MIN_SIZE) {
          // discard too short frames
          if (frame_length > 0) ++this.m_invalid_frame_ct;
          this.rbuf_clear();
          return null;
        }

        // we have a valid frame, and a new frame header
        this.m_complete_frame_length = frame_length;
        this.m_header_found = true;
        this.m_frame_complete = true;
        return this.m_rbuf;
      }

      // now all special bytes are handled.
      this.m_last_byte_was_ff = false;

      if (this.m_rbuf.length >= this.FRAME_MAX_SIZE) {
        // drop all data, frame is too long
        this.rbuf_clear();
        return null;
      }

      // store data byte

      this.m_rbuf.push(b);
      return null;
    },
  },

  set_data: function (arr) {
    this.m_arr = arr;
  },

  add_data: function (arr) {
    this.m_arr.push(...arr);
  },

  get_frame_len: function () {
    let lmap = {};
    for (let i = 0; i < 10000 && i < this.m_arr.length; ++i) {
      let data_frame = this.xr25.add(this.m_arr[i]);
      if (!data_frame) continue;
      const key = `${data_frame.length}`;
      if (key in lmap) {
        lmap[key] += 1;
      } else {
        lmap[key] = 1;
      }
    }
    let most_found_key = "";
    let most_found_val = 0;
    for (let key in lmap) {
      let val = lmap[key];
      console.log("len:", key, val);
      if (most_found_val > val) continue;
      most_found_key = key;
      most_found_val = val;
    }
    return parseInt(most_found_key, 10);
  },

  process_data: function () {
    let blockCounter = 0;
    const frame_length = this.get_frame_len();
    this.xr25.rbuf_clear();

    for (let b of this.m_arr) {
      let data_frame = this.xr25.add(b);
      if (!data_frame) continue;
      if (frame_length != data_frame.length) {
        continue;
      }
      if (blockCounter++ > 0) {
        if (this.m_cb) {
          this.m_cb(data_frame, blockCounter - 2);
        }
      }
    }
    console.log("nBlocks:", blockCounter, "frameLen:", frame_length);
  },
};
