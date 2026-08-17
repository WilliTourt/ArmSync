#pragma once

#include <cstdint>
#include <cstddef>

class RingBuf {
    public:
        RingBuf(uint8_t* buf, size_t size);

        void put(uint8_t ch);
        int  get();

        size_t available() const;
        bool   isEmpty() const;

        // For '\n' termination
        bool   hasLine() const;
        size_t readLine(uint8_t* dst, size_t maxLen);

        // For custom frame
        uint8_t peek() const;
        size_t  peek(uint8_t* dst, size_t n) const;

    private:
        uint8_t* _buf;
        size_t   _size;
        size_t   _head;
        size_t   _tail;
        size_t   _count;
        bool     _hasLine;
};