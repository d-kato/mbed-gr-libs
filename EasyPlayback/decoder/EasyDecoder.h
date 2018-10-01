/* mbed EasyDecoder Library
 * Copyright (C) 2017 dkato
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

#ifndef __EASY_DECODER_H__
#define __EASY_DECODER_H__

#include "mbed.h"

class EasyDecoder {
public:

    virtual ~EasyDecoder(){}

    /** analyze header
     *
     * @param p_title title tag buffer
     * @param p_artist artist tag buffer
     * @param p_album album tag buffer
     * @param tag_size tag buffer size
     * @param fp file pointer
     * @return true = success, false = failure
     */
    virtual bool AnalyzeHeder(char* p_title, char* p_artist, char* p_album, uint16_t tag_size, FILE* fp) = 0;

    /** get next data
     *
     * @param buf data buffer address
     * @param len data buffer length
     * @return get data size
     */
    virtual size_t GetNextData(void *buf, size_t len) = 0;

    /** get channel
     *
     * @return channel
     */
    virtual uint16_t GetChannel() = 0;

    /** get block size
     *
     * @return block size
     */
    virtual uint16_t GetBlockSize() = 0;

    /** get sampling rate
     *
     * @return sampling rate
     */
    virtual uint32_t GetSamplingRate() = 0;

};

#endif
