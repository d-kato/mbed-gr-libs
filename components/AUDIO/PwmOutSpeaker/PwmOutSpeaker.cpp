/* mbed PwmOutSpeaker Library
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

#include "PwmOutSpeaker.h"

PwmOutSpeaker::PwmOutSpeaker(PinName pwm_l, PinName pwm_r) : 
 _speaker_l(NULL), _speaker_r(NULL), _audioThread(osPriorityHigh), _sound_out_req(0) {
    _bottom = 0;
    _top = 0;
    _playing = false;
    if (pwm_l != NC) {
        _speaker_l = new PwmOut(pwm_l);
    }
    if (pwm_r != NC) {
        _speaker_r = new PwmOut(pwm_r);
    }
    _audioThread.start(callback(this, &PwmOutSpeaker::audio_process));
    outputVolume(1.0f, 1.0f);
    format(16);
    frequency(44100);
}

PwmOutSpeaker::~PwmOutSpeaker() {
    if (_speaker_l != NULL) {
        delete _speaker_l;
    }
    if (_speaker_r != NULL) {
        delete _speaker_r;
    }
}

bool PwmOutSpeaker::format(char length) {
    switch (length) {
        case 8:
        case 16:
            break;
        default:
            return false;
    }
    _length = length;
    _data_cnt = 0;
    return true;
}

bool PwmOutSpeaker::frequency(int hz) {
    int wk_us;

    switch (hz) {
        case 48000:
            _hz_multi = 6;
            break;
        case 44100:
            _hz_multi = 5;
            break;
        case 32000:
            _hz_multi = 4;
            break;
        case 8021:
        case 8000:
            _hz_multi = 1;
            break;
        default:
            return false;
    }
    _playing = false;
    if (_speaker_l != NULL) {
        _speaker_l->write(0.5f);
        _speaker_l->period_us(10);  // 100kHz
    }
    if (_speaker_r != NULL) {
        _speaker_r->write(0.5f);
        _speaker_r->period_us(10);  // 100kHz
    }
    wk_us = (int)(1000000.0f / hz * _hz_multi + 0.5f);
    _timer.attach_us(Callback<void()>(this, &PwmOutSpeaker::sound_out), wk_us);
    _data_cnt = 0;

    return true;
}

int PwmOutSpeaker::write(void * const p_data, uint32_t data_size, const rbsp_data_conf_t * const p_data_conf) {
    int data_num;
    int i = 0;
    float wk_vol_l;
    float wk_ofs_l;
    float wk_vol_r;
    float wk_ofs_r;

    if (_length == 8) {
        data_num = data_size;
    } else {
        data_num = data_size / 2;
    }
    while (i < data_num) {
        if (_data_cnt < (_hz_multi - 1)) {
            _data_cnt++;
            i += 2;
        } else {
            _data_cnt = 0;
            while (((_bottom + 2) & MSK_RING_BUFF) == _top) {
                ThisThread::sleep_for(1);
            }

            wk_vol_l = _speaker_vol_l;
            wk_ofs_l = (1.0f - wk_vol_l) / 2;
            wk_vol_r = _speaker_vol_r;
            wk_ofs_r = (1.0f - wk_vol_r) / 2;
            if (_length == 8) {
                _pwm_duty_buf[_bottom] = ((float)((uint8_t *)p_data)[i++] / (float)0xff) * wk_vol_l + wk_ofs_l;
                _pwm_duty_buf[_bottom + 1] = ((float)((uint8_t *)p_data)[i++] / (float)0xff) * wk_vol_r + wk_ofs_r;
            } else {
                _pwm_duty_buf[_bottom] = ((float)(((int16_t *)p_data)[i++] + 0x8000) / (float)0xffff) * wk_vol_l + wk_ofs_l;
                _pwm_duty_buf[_bottom + 1] = ((float)(((int16_t *)p_data)[i++] + 0x8000) / (float)0xffff) * wk_vol_r + wk_ofs_r;
            }
            _bottom = (_bottom + 2) & MSK_RING_BUFF;
        }
    }

    if (p_data_conf != NULL) {
        return data_size;
    }

    if (p_data_conf->p_notify_func != NULL) {
        p_data_conf->p_notify_func(p_data, data_size, p_data_conf->p_app_data);
    }

    return 0;
}

bool PwmOutSpeaker::outputVolume(float leftVolumeOut, float rightVolumeOut) {
    if ((leftVolumeOut < 0.0) || (leftVolumeOut > 1.0)) {
        return false;
    }
    if ((rightVolumeOut < 0.0) || (rightVolumeOut > 1.0)) {
        return false;
    }
    _speaker_vol_l  = leftVolumeOut;
    _speaker_vol_r  = rightVolumeOut;
    return true;
}

void PwmOutSpeaker::sound_out(void) {
    _sound_out_req.release();
}

void PwmOutSpeaker::audio_process() {
    while (true) {
        _sound_out_req.wait();
        if (_top != _bottom) {
            if (_speaker_l != NULL) {
                _speaker_l->write(_pwm_duty_buf[_top + 0]);
            }
            if (_speaker_r != NULL) {
                _speaker_r->write(_pwm_duty_buf[_top + 1]);
            }
            _top = (_top + 2) & MSK_RING_BUFF;
            _playing = true;
        } else if (_playing) {
            if (_speaker_l != NULL) {
                _speaker_l->write(0.5f);
            }
            if (_speaker_r != NULL) {
                _speaker_r->write(0.5f);
            }
            _playing = false;
        } else {
            // do nothing
        }
    }
}

