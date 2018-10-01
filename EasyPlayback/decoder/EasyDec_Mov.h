/* mbed EasyDec_Mov Library
 * Copyright (C) 2018 dkato
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
* @file          EasyDec_Mov.h
* @brief         mov
******************************************************************************/
#ifndef __EASY_DEC_MOV_H__
#define __EASY_DEC_MOV_H__

#include "EasyDecoder.h"

/** A class to communicate a EasyDec_Mov
 *
 */
class EasyDec_Mov : public EasyDecoder {
public:

    static inline EasyDecoder* inst() { return new EasyDec_Mov; }

    /** Attach a function to be called when acquiring an image
     *
     * @param func pointer to the function to be called
     * @param video_buf video buffer address
     * @param video_buf_size video buffer size
     */
    static void attach(Callback<void()> func, uint8_t * video_buf, uint32_t video_buf_size);

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
    union __attribute__((packed)) Buffer {
        uint8_t  array[4];
        uint32_t value;
    };
    static uint8_t * _videoBuf;
    static uint32_t _videoBufSize;
    static Callback<void()> _function;  /**< Callback. */

    static const int bufSize = 32;
    uint32_t frameSizes[bufSize];
    uint32_t audioSizes[bufSize];
    FILE * mov_fp;
    uint32_t numOfFrames;
    uint32_t *frameSizesP;
    uint32_t *audioSizesP;
    uint32_t stszAddress;
    uint32_t stcoAddress;
    uint32_t lastFrameAddress;
    int availableCount;
    bool _video_flg;

    void search(uint32_t pattern);
    void fillCaches();
};

#endif

