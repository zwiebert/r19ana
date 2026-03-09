#pragma once

#include <bitset>
#include <cstdint>

namespace spp {
constexpr std::size_t JUST_CONNECTED = 1;
using status_t = std::bitset<16>;
}  // namespace spp

/**
 * @brief this callback is called periodically to get data to write to remote client
 * @param dst          the data buffer
 * @param dst_size     the size of dst
 * @param status       status bit reference
 * @returns            like snprintf(). data bytes written. if greater than dst_size, data was only partial written
 *
 */
typedef int (*spp_write_cb_t)(char* dst, std::size_t dst_size,
                              spp::status_t& status);
/**
 * @brief this callback is called when data was read from remote client
 * @param src_dst      the data buffer. in src, the valid data is followed by a zero byte. in dst this is not required.
 * @param src_len      number of bytes received
 * @param dst_size     the size of src_dst usable for holding the reply data
 * @param status       status bit reference
 * 
 * @returns            byte length of reply (0..dst_size)
 *
 */
typedef std::size_t (*spp_read_cb_t)(char* src_dst, std::size_t src_len,
                              std::size_t dst_size, spp::status_t& status);

/// @brief 
/// @param rcb 
/// @param wcb 
void spp_main(spp_read_cb_t rcb, spp_write_cb_t wcb);

void spp2_main();


/// @brief       Add new data to transmitter queue.
/// @param data  data to send. it must be allocated with malloc. free() will be called on it after msg is sent
/// @param len   byte length of data
/// @return      success (fails if queue is full)
bool spp_tx_enqueue(uint8_t *data, size_t len, bool block = false); 

bool spp_is_connected();
