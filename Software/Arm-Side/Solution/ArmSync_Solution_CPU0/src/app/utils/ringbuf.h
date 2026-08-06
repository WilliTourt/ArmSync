#pragma once

#include <cstdint>
#include <cstring>

class RingBuf {
    public:
        RingBuf(uint8_t* buf, size_t size)
            : _buf(buf), _size(size), _head(0), _tail(0), _count(0), _hasLine(false) {}

        void put(uint8_t ch) {
            if (_count < _size) {
                _buf[_head] = ch;
                _head = (_head + 1) % _size;
                _count++;
            }
            if (ch == '\n') {
                _hasLine = true;
            }
        }

        int get() {
            if (isEmpty()) return -1;
            uint8_t ch = _buf[_tail];
            _tail = (_tail + 1) % _size;
            _count--;
            if (isEmpty()) _hasLine = false;    // 读空了就清标志
            return ch;
        }

        bool hasLine() const { return _hasLine; }

        size_t readLine(uint8_t* dst, size_t maxLen) {
            if (!_hasLine) return 0;

            size_t len = 0;
            int ch;
            while ((ch = get()) >= 0 && len < maxLen - 1) {
                dst[len++] = (uint8_t)ch;
                if (ch == '\n') break;      // 读到换行就停
            }
            dst[len] = '\0';
            _hasLine = false;               // 读完一行清标志
            return len;
        }

        bool isEmpty() const { return (_count == 0); }

    private:
        uint8_t* _buf;
        size_t _size;
        size_t _head;
        size_t _tail;
        size_t _count;
        bool _hasLine;
};
