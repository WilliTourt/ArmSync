#include "ringbuf.h"

RingBuf::RingBuf(uint8_t* buf, size_t size)
    : _buf(buf), _size(size), _head(0), _tail(0), _count(0), _hasLine(false) {}

void RingBuf::put(uint8_t ch) {
    if (_count < _size) {
        _buf[_head] = ch;
        _head = (_head + 1) % _size;
        _count++;
    }
    if (ch == '\n') {
        _hasLine = true;
    }
}

/**
 * @brief Reads (or discards) a character from the buffer, move the head forward 1 byte.
 * 
 * @return The character read, or -1 if the buffer is empty.
 */
int RingBuf::get() {
    if (isEmpty()) return -1;
    uint8_t ch = _buf[_tail];
    _tail = (_tail + 1) % _size;
    _count--;
    if (isEmpty()) _hasLine = false;
    return ch;
}

size_t RingBuf::available() const {
    return _count;
}

bool RingBuf::isEmpty() const {
    return (_count == 0);
}

bool RingBuf::hasLine() const {
    return _hasLine;
}

size_t RingBuf::readLine(uint8_t* dst, size_t maxLen) {
    if (!_hasLine) return 0;

    size_t len = 0;
    int ch;
    while ((ch = get()) >= 0 && len < maxLen - 1) {
        dst[len++] = (uint8_t)ch;
        if (ch == '\n') break;
    }
    dst[len] = '\0';
    _hasLine = false;
    return len;
}

uint8_t RingBuf::peek() const {
    if (isEmpty()) return 0;
    return _buf[_tail];
}

size_t RingBuf::peek(uint8_t* dst, size_t n) const {
    if (n > _count) n = _count;
    size_t t = _tail;
    for (size_t i = 0; i < n; i++) {
        dst[i] = _buf[t];
        t = (t + 1) % _size;
    }
    return n;
}
