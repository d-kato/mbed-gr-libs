/* mbed Microcontroller Library
 * Copyright (c) 2017 ARM Limited
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
#ifndef MBED_ROM_RAM_BLOCK_DEVICE_H
#define MBED_ROM_RAM_BLOCK_DEVICE_H

#include "BlockDevice.h"
#include "mbed.h"


/** Lazily allocated heap-backed block device
 *
 * When writing data of ROM address, heap memory is not used
 * Useful for simulating a block device and tests
 *
 * @code
 * #include "mbed.h"
 * #include "RomRamBlockDevice.h"
 *
 * RomRamBlockDevice bd(2048, 512); // 2048 bytes with a block size of 512 bytes
 * uint8_t block[512] = "Hello World!\n";
 *
 * int main() {
 *     bd.SetRomAddr(0x18000000, 0x1FFFFFFF); // ROM Address 0x18000000 - 0x1FFFFFFF
 *     bd.init();
 *     bd.program(block, 0);
 *     bd.read(block, 0);
 *     printf("%s", block);
 *     bd.deinit();
 * }
 */
class RomRamBlockDevice : public BlockDevice
{
public:

    /** Lifetime of the memory block device
     */
    RomRamBlockDevice(bd_size_t size, bd_size_t block=512);
    RomRamBlockDevice(bd_size_t size, bd_size_t read, bd_size_t program, bd_size_t erase);
    virtual ~RomRamBlockDevice();

    /** Set the ROM address range
     * 
     *  When writing data of ROM address, heap memory is not used
     * 
     *  @param rom_start_addr   Rom start address
     *  @param rom_end_addr     Rom end address
     */
    void SetRomAddr(uint32_t rom_start_addr, uint32_t rom_end_addr);

    /** Initialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int init();

    /** Deinitialize a block device
     *
     *  @return         0 on success or a negative error code on failure
     */
    virtual int deinit();

    /** Read blocks from a block device
     *
     *  @param buffer   Buffer to read blocks into
     *  @param addr     Address of block to begin reading from
     *  @param size     Size to read in bytes, must be a multiple of read block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int read(void *buffer, bd_addr_t addr, bd_size_t size);

    /** Program blocks to a block device
     *
     *  The blocks must have been erased prior to being programmed
     *
     *  @param buffer   Buffer of data to write to blocks
     *  @param addr     Address of block to begin writing to
     *  @param size     Size to write in bytes, must be a multiple of program block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int program(const void *buffer, bd_addr_t addr, bd_size_t size);

    /** Erase blocks on a block device
     *
     *  The state of an erased block is undefined until it has been programmed
     *
     *  @param addr     Address of block to begin erasing
     *  @param size     Size to erase in bytes, must be a multiple of erase block size
     *  @return         0 on success, negative error code on failure
     */
    virtual int erase(bd_addr_t addr, bd_size_t size);

    /** Get the size of a readable block
     *
     *  @return         Size of a readable block in bytes
     */
    virtual bd_size_t get_read_size() const;

    /** Get the size of a programable block
     *
     *  @return         Size of a programable block in bytes
     */
    virtual bd_size_t get_program_size() const;

    /** Get the size of a eraseable block
     *
     *  @return         Size of a eraseable block in bytes
     */
    virtual bd_size_t get_erase_size() const;

    /** Get the total size of the underlying device
     *
     *  @return         Size of the underlying device in bytes
     */
    virtual bd_size_t size() const;

    /** Get the BlockDevice class type.
     *
     *  @return         A string represent the BlockDevice class type.
     */
    virtual const char *get_type() const;

private:
    bd_size_t _read_size;
    bd_size_t _program_size;
    bd_size_t _erase_size;
    bd_size_t _count;
    uint8_t **_blocks;
    uint32_t _rom_start;
    uint32_t _rom_end;

    bool isRomAddress(const uint8_t *address);
};


#endif
