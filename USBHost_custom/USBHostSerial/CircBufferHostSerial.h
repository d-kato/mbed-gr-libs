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

#include "stdint.h"

template<typename T, int size>
class CircBufferHostSerial {
public:

    CircBufferHostSerial() {
        write = 0;
        read = 0;
    }

    bool isFull() {
        return (((write + 1) % size) == read);
    }

    bool isEmpty() {
        return (read == write);
    }

    void flush() {
        write = 0;
        read = 0;
    }

    bool queue(T k) {
        if (isFull()) {
            return false;
        }
        buf[write] = k;
        write = (write + 1) % size;
        return true;
    }

    uint32_t available() {
        uint32_t a = (write >= read) ? (write - read) : (size - read + write);
        return a;
    }

    bool dequeue(T * c) {
        if (isEmpty()) {
            return false;
        }
        *c = buf[read];
        read = (read + 1) % size;
        return true;
    }

private:
    volatile uint32_t write;
    volatile uint32_t read;
    volatile T buf[size];
};

#endif

