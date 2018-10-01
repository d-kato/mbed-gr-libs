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

#include "EasyDec_Mov.h"

#define FourConstant(a, b, c, d) ((a) << 24 | (b) << 16 | (c) << 8 | (d))

#ifndef htonl
#   define htonl(x) __REV(x)
#endif
#ifndef ntohl
#   define ntohl(x) __REV(x)
#endif

uint8_t * EasyDec_Mov::_videoBuf = NULL;
uint32_t EasyDec_Mov::_videoBufSize = 0;
Callback<void()> EasyDec_Mov::_function = NULL;

void EasyDec_Mov::attach(Callback<void()> func, uint8_t * video_buf, uint32_t video_buf_size) {
    _function = func;
    _videoBuf = video_buf;
    _videoBufSize = video_buf_size;
}

bool EasyDec_Mov::AnalyzeHeder(char* p_title, char* p_artist, char* p_album, uint16_t tag_size, FILE* fp) {
    Buffer buf;

    if (fp == NULL) {
        return false;
    }
    if (p_title != NULL) {
        p_title[0] = '\0';
    }
    if (p_artist != NULL) {
        p_artist[0] = '\0';
    }
    if (p_album != NULL) {
        p_album[0] = '\0';
    }

    mov_fp = fp;
    availableCount = bufSize;

    frameSizesP = frameSizes;
    audioSizesP = audioSizes;

    search(htonl(FourConstant('s', 't', 's', 'z')));
    fseek(mov_fp, 8, SEEK_CUR);
    fread(&buf, sizeof(Buffer), 1, mov_fp);
    numOfFrames = htonl(buf.value);
    stszAddress = ftell(mov_fp);
    fread(&buf, sizeof(Buffer), 1, mov_fp);

    search(htonl(FourConstant('s', 't', 'c', 'o')));
    fseek(mov_fp, 4, SEEK_CUR);
    fread(&buf, sizeof(Buffer), 1, mov_fp);
    if (numOfFrames != htonl(buf.value)) {
        return false;
    }
    fread(&buf, sizeof(Buffer), 1, mov_fp);
    stcoAddress = ftell(mov_fp);
    lastFrameAddress = htonl(buf.value);

    fillCaches();

    _video_flg = true;

    return true;
}

size_t EasyDec_Mov::GetNextData(void *buf, size_t len) {
    size_t ret;
    uint32_t rest_size = 0;
    uint32_t aSize = *audioSizesP;

    if (numOfFrames <= 0) {
        return 0;
    }

    if (_video_flg) {
        _video_flg = false;
        if ((_videoBuf == NULL) || (*frameSizesP > _videoBufSize)) {
            fseek(mov_fp, *frameSizesP, SEEK_CUR);
        } else {
            fread(_videoBuf, 1, *frameSizesP, mov_fp);
            if (_function) {
                _function();
            }
        }
        frameSizesP++;
    }

    if ((int)aSize < 0) {
        ret = 0;
    } else {
        if (aSize > len) {
            rest_size = aSize - len;
            *audioSizesP = rest_size;
            aSize = len;
        }
        if (buf == NULL) {
            fseek(mov_fp, aSize, SEEK_CUR);
            ret = aSize;
        } else {
            ret = (uint32_t)fread(buf, 1, aSize, mov_fp);
        }
    }
    if (rest_size == 0) {
        _video_flg = true;
        audioSizesP++;
        if (--availableCount == 0) {
            fillCaches();
        }
        numOfFrames--;
    }

    return ret;
}

uint16_t EasyDec_Mov::GetChannel() {
    return 2;
}

uint16_t EasyDec_Mov::GetBlockSize() {
    return 16;
}

uint32_t EasyDec_Mov::GetSamplingRate() {
    return 44100;
}

void EasyDec_Mov::search(uint32_t pattern) {
    Buffer buf;
    uint8_t first = pattern & 0xFF;
    buf.value = 0;
    while (true) {
        size_t size = fread(buf.array, sizeof(int8_t), 1, mov_fp);
        if (size == 0) {
            return;
        }
        if (buf.array[0] == first) {
            fread(&buf.array[1], sizeof(int8_t), 3, mov_fp);
            if (buf.value == pattern) {
                break;
            }
        }
    }
}

void EasyDec_Mov::fillCaches() {
    Buffer buf[bufSize];
    Buffer *bufP = buf;
    uint32_t lastFrame = lastFrameAddress;

    fseek(mov_fp, stszAddress, SEEK_SET);
    fread(frameSizes, sizeof(uint32_t), bufSize, mov_fp);
    stszAddress += sizeof(uint32_t) * bufSize;

    fseek(mov_fp, stcoAddress, SEEK_SET);
    stcoAddress += sizeof(uint32_t) * bufSize;
    fread(buf, sizeof(Buffer), bufSize, mov_fp);
    availableCount = bufSize;
    frameSizesP = frameSizes;
    audioSizesP = audioSizes;
    do {
        uint32_t frameAddress = htonl(bufP->value);
        *frameSizesP = htonl(*frameSizesP);
        *audioSizesP++ = frameAddress - lastFrameAddress - *frameSizesP;
        lastFrameAddress = frameAddress;
        ++frameSizesP;
        ++bufP;
    } while (--availableCount);

    fseek(mov_fp, lastFrame, SEEK_SET);
    availableCount = bufSize;
    frameSizesP = frameSizes;
    audioSizesP = audioSizes;
}


