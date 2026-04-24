#include <cstdint>
#include <functional>
#include <vector>

class RenixDestuffer {
 public:
  static constexpr int PACKET_MAX_SIZE = 64;  // cannot store larger packets
  static constexpr int PACKET_MIN_SIZE = 30;  // reject smaller packets
  using PacketCallback = std::function<void(const uint8_t*, size_t)>;

  explicit RenixDestuffer(PacketCallback callback)
      : onPacket(callback), lastWas255(false) {
    packetBuffer.reserve(PACKET_MAX_SIZE);  // Pre-allocate
  }

  /**
   * Process a raw chunk of data from the UART
   */
  void processChunk(const uint8_t* data, size_t length) {
    for (size_t i = 0; i < length; ++i) {
      uint8_t currentByte = data[i];

      if (packetBuffer.size() > PACKET_MAX_SIZE) {
        packetBuffer.clear();
        lastWas255 = false;
        return;  // malformatted input data
      }
      if (lastWas255) {
        if (currentByte == 255) {
          // Rule 1: 255, 255 is a STUFFED DATA BYTE (Value 255)
          packetBuffer.push_back(255);
          lastWas255 = false;
        } else {
          // Rule 2: 255 followed by [Not 255] is the FRAME DELIMITER.
          // The 'currentByte' is the START BYTE (0) of the NEW packet.
          if (packetBuffer.size() >= PACKET_MIN_SIZE) finalizePacket();

          packetBuffer.clear();
          packetBuffer.push_back(currentByte);  // This is the '0'
          lastWas255 = false;
        }
      } else {
        if (currentByte == 255) {
          lastWas255 = true;
        } else {
          packetBuffer.push_back(currentByte);
        }
      }
    }
  }

 private:
  std::vector<uint8_t> packetBuffer;
  PacketCallback onPacket;
  bool lastWas255;

  void finalizePacket() { onPacket(packetBuffer.data(), packetBuffer.size()); }
};
