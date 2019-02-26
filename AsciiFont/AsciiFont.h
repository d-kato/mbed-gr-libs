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
/**************************************************************************//**
* @file          AsciiFont.h
* @brief         AsciiFont API
******************************************************************************/

#ifndef ASCII_FONT_H
#define ASCII_FONT_H

#include "mbed.h"

/** Draw the character of the ASCII code.
 *
 * Example
 * @code
 * #include "mbed.h"
 * #include "AsciiFont.h"
 * 
 * #define WIDTH           (12)
 * #define HEIGHT          (16)
 * #define BYTE_PER_PIXEL  (1u)
 * #define STRIDE          (((WIDTH * BYTE_PER_PIXEL) + 7u) & ~7u) //multiple of 8
 * 
 * uint8_t text_field[STRIDE * HEIGHT];
 * 
 * //for debug
 * void print_text_field() {
 *     int idx = 0;
 * 
 *     for (int i = 0; i < HEIGHT; i++) {
 *         for (int j = 0; j < STRIDE; j++) {
 *             printf("%02x", text_field[idx++]);
 *         }
 *         printf("\r\n");
 *     }
 *     printf("\r\n");
 * }
 * 
 * int main() {
 *     AsciiFont ascii_font(text_field, WIDTH, HEIGHT, STRIDE, BYTE_PER_PIXEL);
 * 
 *     ascii_font.Erase(0xcc);
 *     ascii_font.DrawStr("AB", 0, 0, 0x11, 1);
 *     ascii_font.DrawChar('C', AsciiFont::CHAR_PIX_WIDTH,
 *                         AsciiFont::CHAR_PIX_HEIGHT, 0x22, 1);
 *     print_text_field(); //debug print
 * 
 *     ascii_font.Erase();
 *     ascii_font.DrawStr("D", 0, 0, 0xef, 2);
 *     print_text_field(); //debug print
 * 
 *     ascii_font.Erase(0x11, 6, 0, 6, 8);
 *     print_text_field(); //debug print
 * }
 * @endcode
 */

class AsciiFont {
public:

    /** Constructor: Initializes AsciiFont.
     *
     * @param p_buf Text field address
     * @param width Text field width
     * @param height Text field height
     * @param stride Buffer stride
     * @param colour Background color
     * @param byte_per_pixel Byte per pixel
     */
    AsciiFont(uint8_t * p_buf, int width, int height, int stride, int byte_per_pixel, uint32_t const colour = 0);

    /** Erase text field
     *
     */
    void Erase();

    /** Erase text field
     *
     * @param colour Background color
     */
    void Erase(uint32_t const colour);

    /** Erase text field
     *
     * @param colour Background color
     * @param x Erase start position of x coordinate
     * @param y Erase start position of y coordinate
     * @param width Erase field width
     * @param height Erase field height
     */
    void Erase(uint32_t const colour, int x, int y, int width, int height);

    /** Draw a string
     *
     * @param str String
     * @param x Drawing start position of x coordinate
     * @param y Drawing start position of y coordinate
     * @param color Font color
     * @param font_size Font size (>=1)
     * @param max_char_num The maximum number of characters
     * @return The drawn number of characters
     */
    int DrawStr(const char * str, int x, int y, uint32_t const colour, int font_size = 1, uint16_t const max_char_num = 0xffff);

    /** Draw a character
     *
     * @param x Drawing start position of x coordinate
     * @param y Drawing start position of y coordinate
     * @param color Font color
     * @param font_size Font size (>=1)
     * @return true if successfull
     */
    bool DrawChar(char c, int x, int y, uint32_t const colour, int font_size = 1);

    /** The pixel width of a character. (font_size=1)
     *
     */
    static const int CHAR_PIX_WIDTH  = 6;

    /** The pixel height of a character. (font_size=1)
     *
     */
    static const int CHAR_PIX_HEIGHT = 8;

private:
    uint8_t * p_text_field;
    int max_width;
    int max_height;
    int buf_stride;
    int pixel_num;
    uint32_t background_colour;
};
#endif
