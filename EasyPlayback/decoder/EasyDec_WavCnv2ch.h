/* mbed EasyDec_WavCnv2ch Library
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

/**************************************************************************//**
* @file          EasyDec_WavCnv2ch.h
* @brief         wav
******************************************************************************/
#ifndef __EASY_DEC_WAV_CNV_2CH_H__
#define __EASY_DEC_WAV_CNV_2CH_H__

#include "EasyDecoder.h"

/** A class to communicate a EasyDec_WavCnv2ch
 *
 */
class EasyDec_WavCnv2ch : public EasyDecoder {
public:

    static inline EasyDecoder* inst() { return new EasyDec_WavCnv2ch; }

    /** analyze header
     *
     * @param p_title title tag buffer
     * @param p_artist artist tag buffer
     * @param p_album album tag buffer
     * @param tag_size tag buffer size
     * @param fp file pointer
     * @return true = success, false = failure
     */
    virtual bool AnalyzeHeder(char* p_title, char* p_artist, char* p_album, uint16_t tag_size, FILE* fp);

    /** get next data
     *
     * @param buf data buffer address
     * @param len data buffer length
     * @return get data size
     */
    virtual size_t GetNextData(void *buf, size_t len);

    /** get channel
     *
     * @return channel
     */
    virtual uint16_t GetChannel();

    /** get block size
     *
     * @return block size
     */
    virtual uint16_t GetBlockSize();

    /** get sampling rate
     *
     * @return sampling rate
     */
    virtual uint32_t GetSamplingRate();

private:
    FILE * wav_fp;
    uint32_t music_data_size;
    uint32_t music_data_index;
    uint16_t channel;
    uint16_t block_size;
    uint32_t sampling_rate;
};

#endif
