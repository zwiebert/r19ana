// BleTransport.hh
#pragma once

#include "Transport.hh"

// platform‑specific BLE reader (BlueZ/NimBLE/dispatcher)
class BleTransport : public Transport {
public:
    void start(ReadCallback cb) override;
    void stop() override;
};