#include "AckRange.h"
#include "../Data/ByteParser.h"

AckRange::AckRange(packet_seq_num_t sn) :
    start(sn),
    count(1)
{}

bool AckRange::extend(packet_seq_num_t sn) {
    if (sn == start + count) {
        ++count;
        return true;
    }
    return false;
}
