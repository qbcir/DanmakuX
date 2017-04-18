#ifndef __dx_NETTYPES_H__
#define __dx_NETTYPES_H__

#include <cstdint>
#include <cstddef>
#include <vector>
#include "asio.hpp"

typedef uint16_t packet_seq_num_t;
typedef uint64_t net_id_t;
typedef uint32_t state_mask_t;
typedef uint32_t packet_t;
typedef int player_id_t;
typedef std::vector<uint8_t> bytes_t;
typedef asio::ip::udp::endpoint endpoint_t;

#endif /* __dx_NETTYPES_H__ */
