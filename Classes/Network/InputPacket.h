#ifndef INPUTPACKET_H
#define INPUTPACKET_H

#include <cstdint>

struct InputPacket {
public:
    InputPacket() {}
    InputPacket(
        uint64_t keyboardState_,
        uint64_t mouseState_,
        float timestamp_,
        float delta_);
    uint64_t keyboardState;
    uint64_t mouseState;
    float timestamp;
    float delta;
};

#endif // INPUTPACKET_H
