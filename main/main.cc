// main.cc
#include "FrameProcessor.hh"
#include "bt_classic/spp_acceptor.hh"
#include <iostream>
#include <thread>
#include <chrono>
#include <stdio.h>
#include <string.h>
std::vector<uint8_t> hexStringToByteArray(const std::string &hex)
{
    std::vector<uint8_t> bytes;

    for (unsigned int i = 0; i < hex.length(); i += 2)
    {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = (uint8_t)strtol(byteString.c_str(), NULL, 16);
        bytes.push_back(byte);
    }

    return bytes;
}

bool test2(FrameProcessor &processor)
{
    processor.feedBytes(hexStringToByteArray("ff00107710447d79bf1aa45fc608080c"));
    processor.feedBytes(hexStringToByteArray("0073ffff0100006b3004048079811288"));
    processor.feedBytes(hexStringToByteArray("ff00107710437d79ba19705fc608040c"));
    processor.feedBytes(hexStringToByteArray("0073ffff0100006b3004048079811288"));
    processor.feedBytes(hexStringToByteArray("ff00107710437d79b8198c5fc408050c"));

    return true;
}

R19Frame R19_frame;

inline const char *btoa(bool v) { return v ? "yes" : "no"; }

int spp_write_r19_frame(char *dst, size_t dst_size)
{
    static R19Frame c;
    int ret = 0;

    if (c != R19_frame)
    {

        const auto &d = R19_frame;

        ret = snprintf(dst, dst_size, "rpm=%d, map=%d mBar, IAT=%d C, ECT=%d C\r\n"         //
                                      "O2=%d mV, AP=%d mBar, Batt=%0.1f V, ID=%0.2f ms\r\n" //
                                      "TO=%s, TC=%s, EGR/EVap=%s, IdleCorr=%d, Knock=%d\r\n",
                       d.EngineSpeed_RPM, d.MAP_mBar, d.IAT_Celsius, d.ECT_Celsius,                                           //
                       d.O2_Sensor_mV, d.AP_mBar, d.BatteryVoltage_V, d.ID_msec,                                              //
                       btoa(d.isThrottleOpen), btoa(d.isThrottleClosed), btoa(d.isAGR_AKF), d.IdleSpeedCorr, d.EngineKnocking //
        );

        c = R19_frame;
    }

    return ret;
}

int spp_write_cb(char *dst, size_t dst_size)
{
    int ret = 0;

    ret = spp_write_r19_frame(dst, dst_size);
    if (0 < ret && ret < dst_size)
        return ret;

    return ret;
}

void spp_read_cb(const char *src, size_t src_len)
{
    printf("spp-in: %.*s", src_len, src);
}

extern "C" int app_main()
{
    FrameProcessor processor([](const std::string &hex, R19Frame &&r19_frame)
                             {
        R19_frame = r19_frame; // store latest frame for later use
        std::cout << "HEX: " << hex << "\n";
        std::cout << r19_frame.getDataAsText() << "\n----------------\n"; });

    test2(processor);

    spp_main(spp_read_cb, spp_write_cb);

#if 0
    UsbTransport usb;

    usb.start([&processor](auto data){ processor.feedBytes(data); });

    // simple loop – replace with real event loop or RTOS task
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    spp.stop();
#endif

    return 0;
}

int main()
{
    return app_main();
}