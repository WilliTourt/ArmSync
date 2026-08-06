#include "utils.h"

void Utils::f2Bytes(float data, uint8_t* out) {
    union {
        float f;
        uint8_t bytes[4];
    } converter;

    converter.f = data;

    //  bigEndian output
    out[0] = converter.bytes[3];
    out[1] = converter.bytes[2];
    out[2] = converter.bytes[1];
    out[3] = converter.bytes[0];
}

float Utils::bytes2F(const uint8_t* buffer) {
    union {
        float f;
        uint8_t bytes[4];
    } converter;

    // bigEndian input
    converter.bytes[3] = buffer[0];
    converter.bytes[2] = buffer[1];
    converter.bytes[1] = buffer[2];
    converter.bytes[0] = buffer[3];

    return converter.f;
}
