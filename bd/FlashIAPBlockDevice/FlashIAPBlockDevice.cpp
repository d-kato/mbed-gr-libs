/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
 * Copyright (c) 2018 Renesas Electronics Corporation
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

#include "FlashIAPBlockDevice.h"

FlashIAPBlockDevice::FlashIAPBlockDevice(uint32_t start_addr, uint32_t end_addr)
    : _ofs(start_addr), _size(end_addr - start_addr + 1)
{
}

FlashIAPBlockDevice::~FlashIAPBlockDevice()
{
}

int FlashIAPBlockDevice::init()
{
    return _flashiap.init();
}

int FlashIAPBlockDevice::deinit()
{
    return _flashiap.deinit();
}

int FlashIAPBlockDevice::read(void *b, bd_addr_t addr, bd_size_t size)
{
    return _flashiap.read(b, addr + _ofs, size);
}

int FlashIAPBlockDevice::program(const void *b, bd_addr_t addr, bd_size_t size)
{
    return _flashiap.program(b, addr + _ofs, size);
}

int FlashIAPBlockDevice::erase(bd_addr_t addr, bd_size_t size)
{
    return _flashiap.erase(addr + _ofs, size);
}

bd_size_t FlashIAPBlockDevice::get_read_size() const
{
    return 1;
}

bd_size_t FlashIAPBlockDevice::get_program_size() const
{
    uint32_t program_size = _flashiap.get_page_size();

    if (program_size < 256) {
        program_size = 256;
    }
    return program_size;
}

bd_size_t FlashIAPBlockDevice::get_erase_size() const
{
    return _flashiap.get_sector_size(_flashiap.get_flash_start());
}

bd_size_t FlashIAPBlockDevice::get_erase_size(bd_addr_t addr) const
{
    return _flashiap.get_sector_size(addr);
}

int FlashIAPBlockDevice::get_erase_value() const
{
    return 0xFF;
}

bd_size_t FlashIAPBlockDevice::size() const
{
    return _size;
}

