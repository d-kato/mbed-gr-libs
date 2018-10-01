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

#include "SoundlessSpeaker.h"

SoundlessSpeaker::SoundlessSpeaker() {
    _length = 16;
    _hz = 44100;
    _byte_per_sec = (_hz * (_length / 8) * 2);
    _t.reset();
    _next_time = 0;
}

bool SoundlessSpeaker::format(char length) {
    switch (length) {
        case 8:
        case 16:
            break;
        default:
            return false;
    }
    _length = length;
    _byte_per_sec = (_hz * (_length / 8) * 2);
    return true;
}

bool SoundlessSpeaker::frequency(int hz) {
    switch (hz) {
        case 48000:
        case 44100:
        case 32000:
        case 8021:
        case 8000:
            break;
        default:
            return false;
    }
    _hz = hz;
    _byte_per_sec = (_hz * (_length / 8) * 2);
    return true;
}

int SoundlessSpeaker::write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf) {
    int wk_next_time = _next_time;
    int wk_time = _t.read_ms();
    int wk_over_time;

    if (wk_time < wk_next_time) {
        ThisThread::sleep_for(wk_next_time - wk_time);
    }
    _next_time = (data_size * 1000) / _byte_per_sec;
    wk_over_time = _t.read_ms() - wk_next_time;
    if (wk_over_time > 0) {
        if (wk_over_time > _next_time) {
            _next_time = 0;
        } else {
            _next_time -= wk_over_time;
        }
    }
    _t.reset();
    _t.start();

    if (p_data_conf != NULL) {
        return data_size;
    }
    if (p_data_conf->p_notify_func != NULL) {
        p_data_conf->p_notify_func(p_data, data_size, p_data_conf->p_app_data);
    }
    return 0;
}

