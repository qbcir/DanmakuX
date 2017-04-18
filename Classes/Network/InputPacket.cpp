#include "InputPacket.h"
#include "../Data/DataParser.h"

InputPacket::InputPacket(
        uint64_t keyboardState_,
        uint64_t mouseState_,
        float timestamp_,
        float delta_) :
    keyboardState(keyboardState_),
    mouseState(mouseState_),
    timestamp(timestamp_),
    delta(delta_)
{}

DX_DEF_STRUCT_P(InputPacket)
DX_ADD_FIELD_P(InputPacket, keyboardState)
DX_ADD_FIELD_P(InputPacket, mouseState)
DX_ADD_FIELD_P(InputPacket, timestamp)
DX_ADD_FIELD_P(InputPacket, delta)
