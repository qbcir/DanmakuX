#ifndef INPUTPACKET_H
#define INPUTPACKET_H

#include <cstdint>
#include "../Input/KeyboardManager.h"

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
    bool processed = false;
    bool isKeyPressed(InputKey key) const;
};

#endif // INPUTPACKET_H
