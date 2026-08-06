#pragma once

#include <cstdint>

namespace Utils {
    void f2Bytes(float data, uint8_t* out);
    float bytes2F(const uint8_t* buffer);
}
