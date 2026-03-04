// main.cc
#include "FrameProcessor.hh"
#include "UsbTransport.hh"
#include <iostream>
#include <thread>
#include <chrono>

std::vector<uint8_t> hexStringToByteArray(const std::string& hex) {
  std::vector<uint8_t> bytes;

  for (unsigned int i = 0; i < hex.length(); i += 2) {
    std::string byteString = hex.substr(i, 2);
    uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
    bytes.push_back(byte);
  }

  return bytes;
}

bool append(std::vector<uint8_t>& data, const std::vector<uint8_t>& bytes) {
    data.insert(data.end(), bytes.begin(), bytes.end());
    return true; // Simulate successful append
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


extern "C" int app_main()
{
    FrameProcessor processor([](const std::string& hex,
                                const std::string& text) {
        std::cout << "HEX: " << hex << "\n";
        std::cout << text << "\n----------------\n";
    });
    
    test2(processor);

    #if 0
    UsbTransport usb;

    usb.start([&processor](auto data){ processor.feedBytes(data); });

    // simple loop – replace with real event loop or RTOS task
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    usb.stop();
    #endif
    return 0;
}


int main() {
    return app_main();
}