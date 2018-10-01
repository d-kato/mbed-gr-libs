/* mbed SoundlessSpeaker Library
 * Copyright (C) 2016 dkato
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

#ifndef SOUNDLESS_SPEAKER_H
#define SOUNDLESS_SPEAKER_H

#include "mbed.h"
#include "AUDIO_RBSP.h"

/** SoundlessSpeaker class
*
*/
class SoundlessSpeaker : public AUDIO_RBSP {
public:
    /** Create a SoundlessSpeaker
     * 
     */
    SoundlessSpeaker();

    virtual ~SoundlessSpeaker() {}

    virtual void power(bool type = true) {
        return;
    }

    /** Set I2S interface bit length and mode
     *
     * @param length Set bit length to 8 or 16 bits
     * @return true = success, false = failure
     */
    virtual bool format(char length);

    /** Set sample frequency
     *
     * @param frequency Sample frequency of data in Hz
     * @return true = success, false = failure
     * 
     * Supports the following frequencies: 8kHz, 8.021kHz, 32kHz, 44.1kHz, 48kHz
     * Default is 44.1kHz
     */
    virtual bool frequency(int hz);

    /** Enqueue asynchronous write request
     *
     * @param p_data Location of the data
     * @param data_size Number of bytes to write
     * @return Number of bytes written on success. negative number on error.
     */
    virtual int write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL);

    virtual int read(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf = NULL) {
        return -1;
    }

    virtual bool outputVolume(float leftVolumeOut, float rightVolumeOut) {
        return false;
    }

    virtual bool micVolume(float VolumeIn) {
        return false;
    }

private:
    Timer    _t;
    int      _next_time;
    int      _length;
    int      _hz;
    int      _byte_per_sec;
};

#endif // SOUNDLESS_SPEAKER_H
