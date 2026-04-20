/**
 * Renix ECU De-stuffer and Parser
 * Processes raw UART bytes, handles byte stuffing, and extracts 30-byte packets.
 */
export class RenixDestuffer {
  private buffer = new Uint8Array(256); // Working buffer for one packet
  private writeIndex = 0;
  private packetCount = 0;
  private lastWas255 = false;

  /**
   * @param onPacket Callback called for each valid 30-byte packet.
   */
  constructor(private onPacket: (packet: Uint8Array, count: number) => void) {}

  /**
   * Process a new chunk of raw data from the ECU.
   */
  public process_chunk(chunk: Uint8Array): void {
    for (let i = 0; i < chunk.length; i++) {
      const currentByte = chunk[i];

      if (this.lastWas255) {
        if (currentByte === 255) {
          // Rule 1: 255, 255 is a STUFFED DATA BYTE
          this.appendToPacket(255);
          this.lastWas255 = false;
        } else {
          // Rule 2: 255 followed by [Not 255] is the FRAME DELIMITER.
          // The currentByte is the START BYTE (0) of the NEW packet.
          this.finalizePacket();

          // Reset for new packet and start with the byte that followed the delimiter
          this.writeIndex = 0;
          this.appendToPacket(currentByte);
          this.lastWas255 = false;
        }
      } else {
        if (currentByte === 255) {
          this.lastWas255 = true;
        } else {
          this.appendToPacket(currentByte);
        }
      }
    }
  }

  private appendToPacket(byte: number): void {
    if (this.writeIndex < this.buffer.length) {
      this.buffer[this.writeIndex++] = byte;
    }
  }

  private finalizePacket(): void {
    // Standard Renix 2.5L packets are 30 bytes long including the 0 start byte
    if (this.writeIndex === 30 && this.buffer[0] === 0) {
      this.packetCount++;
      // Return a copy/slice of the valid packet
      this.onPacket(this.buffer.slice(0, 30), this.packetCount);
    }
    // Note: If writeIndex is not 30, it was likely a malformed/partial frame
  }
}