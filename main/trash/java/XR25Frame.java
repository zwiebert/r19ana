package bertw.xr25x53;


/**
 * Created by bertw on 21.08.2017.
 */


public class XR25Frame {
    private byte[] buffer;
    private byte[] dampbuf;
    static final int BUF_SIZE = 500;
    static final int FRAME_SIZE = 32;
    static final byte[] FRAME_START_ID = {(byte) 0xff, (byte) 0x00, (byte) 0x10, (byte) 0x77};
    int tail = 0;
    int head = BUF_SIZE - 2;
    int frameCounter = 0;


    public XR25Frame() {
        buffer = new byte[BUF_SIZE];
        dampbuf = new byte[FRAME_SIZE];
    }

    void dampbufFill() {
        for (int i = 0; i < FRAME_SIZE; ++i) {
            int b = getIntByIndex(i);
            int d = dampbuf[i] & 0xff;
            if (b == d) {
                continue;
            }
            d = (b > d) ? d + 1 : d - 1;
            if (d >= 0x00 && d <= 0xff)
                dampbuf[i] = (byte) d;
        }
    }

    public String dampbufAsString() {
        StringBuilder sb = new StringBuilder(FRAME_SIZE * 3);

        for (int i = 0; i < FRAME_SIZE; ++i) {
            byte c = dampbuf[i];
            sb.append(Character.forDigit((c >> 4) & 0xF, 16));
            sb.append(Character.forDigit((c & 0xF), 16));
            sb.append(" ");
        }
        return sb.toString();
    }

    int distance() {
        return (tail < head)
                ? (tail + BUF_SIZE) - head
                : tail - head;
    }

    int find(byte[] data) {
        for (int i = 0, n = (distance() - data.length); n > 0; --n, ++i) {
            boolean match = true;
            for (int k = 0; match && k < data.length; ++k) {
                if (getByteByIndex(k + i) != data[k])
                    match = false;
            }
            if (match == true)
                return i;
        }
        return -1;
    }

    boolean adjustHead() {
        int last_good_head = -1;
        int frames_found = 0;

        while (distance() > FRAME_SIZE) {
            int pos = find(FRAME_START_ID);
            if ((distance() - pos) < FRAME_SIZE) { //found beginning of a frame, but frame incomplete
                break;
            }
            if (pos == 0) {  // we have a valid frame at current position
                last_good_head = head;
                head = (head + 1 + BUF_SIZE) % BUF_SIZE;
                continue;
            } else if (pos < 0) {  // no frame
                head = (last_good_head < 0) ? ((tail + BUF_SIZE - 4) % BUF_SIZE) : last_good_head;
                break;
            } else {  // a new frame was found. count it and adjust the head
                head = last_good_head = (head + pos) % BUF_SIZE;
                ++frames_found;
                dampbufFill();
            }

        }
        if (last_good_head >= 0)
            head = last_good_head;

        frameCounter += frames_found;
        return frames_found > 0;
    }

    byte getByteByIndex(int idx) {
        idx = (head + idx + BUF_SIZE) % BUF_SIZE;
        return buffer[idx];
    }

    int getIntByIndex(int idx) {
        int result = 0xff & getByteByIndex(idx);
        return result;
    }

    int getDampedIntByIndex(int idx) {
        int result = 0xff & dampbuf[idx];
        return result;
    }

    void add(byte b) {
        buffer[tail] = b;
        tail = (tail + 1) % BUF_SIZE;
    }


    public boolean append(byte[] data) {
        for (byte b : data) {
            add(b);
        }
        return adjustHead();
    }

    public boolean append(byte[] data, int nmb) {
        for (int i = 0; i < nmb; ++i) {
            add(data[i]);
        }
        return adjustHead();
    }

    public static byte[] hexStringToByteArray(String s) {
        s = s.replaceAll("\\s", " ");

        int len = s.length();
        byte[] data = new byte[len / 2];
        for (int i = 0; i < len; i += 2) {

            data[i / 2] = (byte) ((Character.digit(s.charAt(i), 16) << 4)
                    + Character.digit(s.charAt(i + 1), 16));
        }
        return data;
    }

    public String toString() {
        StringBuilder sb = new StringBuilder(FRAME_SIZE * 3);

        for (int i = 0; i < FRAME_SIZE; ++i) {
            byte c = getByteByIndex(i);
            sb.append(Character.forDigit((c >> 4) & 0xF, 16));
            sb.append(Character.forDigit((c & 0xF), 16));
            sb.append(" ");
        }
        return sb.toString();
    }

    public int getFrameCounter() {
        return frameCounter;
    }


    boolean test() {
        byte[] test_data;
        boolean result;


        test_data = hexStringToByteArray("ff00107710447d79bf1aa45fc608080c");
        result = this.append(test_data);
        test_data = hexStringToByteArray("0073ffff0100006b3004048079811288");
        result = this.append(test_data);
        test_data = hexStringToByteArray("ff00107710437d79ba19705fc608040c");
        result = this.append(test_data);


        test_data = hexStringToByteArray("0073ffff0100006b3004048079811288");
        result = this.append(test_data);
        test_data = hexStringToByteArray("ff00107710437d79b8198c5fc408050c");
        result = this.append(test_data);


/*
        test_data = hexStringToByteArray("ff00107710ffff7f8063010000000000");
        result = this.append(test_data);

        test_data = hexStringToByteArray("000000ffff0801006b200009a282810d");
        result = this.append(test_data);
        test_data = hexStringToByteArray("88ff00107710ffff7f80630100000000");
        result = this.append(test_data);
        test_data = hexStringToByteArray("00000000ffff0801006b200009a28281");
        result = this.append(test_data);
        test_data = hexStringToByteArray("0d88ff00107710ffff7f806301000000");
        result = this.append(test_data);
        test_data = hexStringToByteArray("0000000000ffff0801006b200009a282");
        result = this.append(test_data);
        test_data = hexStringToByteArray("810d88ff00107710ffff7f8063010000");
        result = this.append(test_data);
        test_data = hexStringToByteArray("000000000000ffff0801006b200009a2");
        result = this.append(test_data);

*/
        return result;


    }

}
