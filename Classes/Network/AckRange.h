#ifndef __dx_ACKRANGE_H__
#define __dx_ACKRANGE_H__

#include <cstdint>
#include "NetTypes.h"
#include "../Data/ByteParser.h"

struct AckRange {
    AckRange() {}
    AckRange(packet_seq_num_t sn);
    bool extend(packet_seq_num_t sn);
    packet_seq_num_t start = 0;
    uint32_t count = 0;
};

template<>
inline AckRange dx_read_bytes<AckRange>(uint8_t* &p) {
    AckRange ar;
    ar.start = dx_read_bytes<packet_seq_num_t>(p);
    bool hasCount = dx_read_bytes<bool>(p);
    if (hasCount) {
        auto countMinusOne = dx_read_bytes<uint8_t>(p);
        ar.count = countMinusOne + 1;
    } else {
        ar.count = 1;
    }
    return ar;
}

template<>
inline size_t dx_write_bytes<AckRange>(BytesT& buf, const AckRange& ar) {
    size_t nb = dx_write_bytes<packet_seq_num_t>(buf, ar.start);
    bool hasCount = ar.count > 1;
    nb += dx_write_bytes<bool>(buf, hasCount);
    if (hasCount) {
        uint32_t countMinusOne = ar.count - 1;
        uint8_t countToAck = countMinusOne > 255 ? 255 : uint8_t(countMinusOne);
        nb += dx_write_bytes<uint8_t>(buf, countToAck);
    }
    return nb;
}

#endif /* __dx_ACKRANGE_H__ */
