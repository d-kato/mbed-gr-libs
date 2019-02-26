/* Copyright (c) 2016 dkato
 * SPDX-License-Identifier: Apache-2.0
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
#include "mbed.h"
#include "AsciiFont.h"
#include "ascii.h"

AsciiFont::AsciiFont(uint8_t * p_buf, int width, int height, int stride, int byte_per_pixel, uint32_t const colour) : 
    p_text_field(p_buf), max_width(width), max_height(height), buf_stride(stride), pixel_num(byte_per_pixel), background_colour(colour) {
    
}

void AsciiFont::Erase() {
    Erase(background_colour, 0, 0, max_width, max_height);
}

void AsciiFont::Erase(uint32_t const colour) {
    Erase(colour, 0, 0, max_width, max_height);
}

void AsciiFont::Erase(uint32_t const colour, int x, int y, int width, int height) {
    int idx_base;
    int wk_idx, i, j ,k;

    background_colour = colour;
    if ((x + width) > max_width) {
        width = max_width - x;
    }
    if ((y + height) > max_height) {
        height = max_height - y;
    }
    idx_base = (x * pixel_num) + (buf_stride * y);
    for (i = 0; i < height; i++) {
        wk_idx = idx_base + (buf_stride * i);
        for (j = 0; j < width; j++) {
            for (k = (pixel_num - 1); k >= 0; k--) {
                p_text_field[wk_idx++] = (uint8_t)(background_colour >> (8 * k));
            }
        }
    }
}

int AsciiFont::DrawStr(const char * str, int x, int y, uint32_t const colour, int font_size, uint16_t const max_char_num) {
    int char_num = 0;

    if ((str == NULL) || (font_size <= 0)) {
        return 0;
    }
    while ((*str != '\0') && (char_num < max_char_num)) {
        if (DrawChar(*str, x, y, colour, font_size) == false) {
            break;
        }
        str++;
        x += CHAR_PIX_WIDTH * font_size;
        char_num++;
    }
    return char_num;
}

bool AsciiFont::DrawChar(char c, int x, int y, uint32_t const colour, int font_size) {
    int idx_base;
    int idx_y = 0;
    int wk_idx, i, j ,k, fw, fh;
    char * p_pattern;
    uint8_t mask = 0x80;
    uint32_t wk_colour;

    if (font_size <= 0) {
        return false;
    }
    if ((x + (CHAR_PIX_WIDTH * font_size)) > max_width) {
        return false;
    }
    if ((y + (CHAR_PIX_HEIGHT * font_size)) > max_height) {
        return false;
    }

    if ((c >= 0x20) && (c <= 0x7e)) {
        p_pattern = (char *)&g_ascii_table[c - 0x20][0];
    } else {
        p_pattern = (char *)&g_ascii_table[10][0]; /* '*' */
    }
    idx_base = (x * pixel_num) + (buf_stride * y);

    /* Drawing */
    for (i = 0; i < CHAR_PIX_HEIGHT; i++) {
        for (fh = 0; fh < font_size; fh++) {
            wk_idx = idx_base + (buf_stride * idx_y);
            for (j = 0; j < CHAR_PIX_WIDTH; j++) {
                if (p_pattern[j] & mask) {
                    wk_colour = colour;
                } else {
                    wk_colour = background_colour;
                }
                for (fw = 0; fw < font_size; fw++) {
                    for (k = (pixel_num - 1); k >= 0; k--) {
                        p_text_field[wk_idx++] = (uint8_t)(wk_colour >> (8 * k));
                    }
                }
            }
            idx_y++;
        }
        mask = (uint8_t)(mask >> 1);
    }
    return true;
}

