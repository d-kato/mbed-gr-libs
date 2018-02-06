/* mbed USBHost Library
 * Copyright (c) 2006-2013 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef CIRCBUFFERHOSTSERIAL_H
#define CIRCBUFFERHOSTSERIAL_H

template <class T>
class CircBufferHostSerial {
public:
    CircBufferHostSerial(uint32_t buf_size):write(0), read(0), size(buf_size + 1) {
        _buf = new T [size];
    }

    ~CircBufferHostSerial() {
        delete [] _buf;
    }

    bool isFull() {
        return ((write + 1) % size == read);
    }

    bool isEmpty() {
        return (read == write);
    }

    void flush() {
        write = 0;
        read = 0;
    }

    void queue(T k) {
        if (isFull()) {
            read++;
            read %= size;
        }
        _buf[write++] = k;
        write %= size;
    }

    uint32_t available() {
        return (write >= read) ? write - read : size - read + write;
    }

    bool dequeue(T * c) {
        bool empty = isEmpty();
        if (!empty) {
            *c = _buf[read++];
            read %= size;
        }
        return(!empty);
    }

private:
    volatile uint32_t write;
    volatile uint32_t read;
    int size;
    T * _buf;
};
#endif

