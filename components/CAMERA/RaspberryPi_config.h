
#ifndef RASPBERRY_PI_CONFIG_H
#define RASPBERRY_PI_CONFIG_H

#include "mbed.h"
#include "DisplayBace.h"
#include "r_mipi_api.h"


class RaspberryPi_config {

public:

    /** Initialise
     *
     * @return true = success, false = failure
     */
    virtual bool Initialise() {
        /* OV5642 camera input config */
        const char RaspberryPi_InitRegTable[][3] = {
        /* Initialize */
            /* 2-Lane */
            {0x01, 0x14, 0x01},
            /* timing setting:manual mode */
            {0x01, 0x28, 0x01},
            /* INCK = 12MHz */
            {0x01, 0x2A, 0x0C},
            {0x01, 0x2B, 0x00},
            /* frame length(height) = 2600(=A28H) */
            /* line  length(width)  = 3448(=D78H)(default) */
            {0x01, 0x60, 0x0A},
            {0x01, 0x61, 0x28},
            /* X range */
            {0x01, 0x64, 0x02},
            {0x01, 0x65, 0xA8},
            {0x01, 0x66, 0x0A},
            {0x01, 0x67, 0x27},
            /* Y range */
            {0x01, 0x68, 0x02},
            {0x01, 0x69, 0xB4},
            {0x01, 0x6A, 0x06},
            {0x01, 0x6B, 0xEB},
            /* signal range */
            {0x01, 0x6C, 0x05},
            {0x01, 0x6D, 0x00},
            {0x01, 0x6E, 0x04},
            {0x01, 0x6F, 0x38},
            /* output format RAW8 */
            {0x01, 0x8C, 0x08},
            {0x01, 0x8D, 0x08},
            /* binning (digital) mode */
            {0x01, 0x74, 0x01},
            {0x01, 0x75, 0x01},
            {0x01, 0x76, 0x01},
            {0x01, 0x77, 0x01},
            /* analog gain(LONG) control */
            {0x01, 0x57, 0xC0},
        /* Timing settings */
            {0x01, 0x1C, 0x01}, /* THS_ZERO_MIN */
            {0x01, 0x1D, 0xFF}, /* THS_ZERO_MIN */
            {0x01, 0x24, 0x00}, /* TCLK_ZERO */
            {0x01, 0x25, 0xFF}, /* TCLK_ZERO */
        /* clock setting */
            {0x03, 0x01, 0x05},
            {0x03, 0x04, 0x02},
            {0x03, 0x05, 0x02},
            {0x03, 0x06, 0x00},
            {0x03, 0x07, 0x3A},
            {0x03, 0x09, 0x08},
            {0x03, 0x0C, 0x00},
            {0x03, 0x0D, 0x2A},
        };

        const char sw_stanby_in_cmd[3]  = {0x01, 0x00, 0x00};    /* Software standby in setting */
        const char sw_stanby_out_cmd[3] = {0x01, 0x00, 0x01};    /* Software standby out setting */
        const char sw_reset_cmd[3]      = {0x01, 0x03, 0x01};    /* Software reset setting */
        int ret;
#if defined(TARGET_RZ_A2M_EVB) || defined(TARGET_RZ_A2M_SBEV) || defined(TARGET_SEMB1402)
        I2C mI2c_(PD_5, PD_4);
#else
        I2C mI2c_(I2C_SDA, I2C_SCL);
#endif
        mI2c_.frequency(150000);

        if (mI2c_.write(0x20, sw_stanby_in_cmd, 3) != 0) {
            return false;
        }

        if (mI2c_.write(0x20, sw_reset_cmd, 3) != 0) {
            return false;
        }

        ThisThread::sleep_for(1);

        for (uint32_t i = 0; i < (sizeof(RaspberryPi_InitRegTable) / 3) ; i++) {
            ret = mI2c_.write(0x20, RaspberryPi_InitRegTable[i], 3);
            if (ret != 0) {
                return false;
            }
        }

        if (mI2c_.write(0x20, sw_stanby_out_cmd, 3) != 0) {
            return false;
        }

        return true;
    }

    void SetMipiConfig(DisplayBase::video_mipi_param_t * p_cfg) {
        p_cfg->mipi_lanenum    = 2;
        p_cfg->mipi_vc         = 0;
        p_cfg->mipi_interlace  = 0;
        p_cfg->mipi_laneswap   = 0; /* Progressive */
        p_cfg->mipi_frametop   = 0;
        p_cfg->mipi_outputrate = 80;

        p_cfg->mipi_phy_timing.mipi_ths_prepare  = 0x00000012u;
        p_cfg->mipi_phy_timing.mipi_ths_settle   = 0x00000019u;
        p_cfg->mipi_phy_timing.mipi_tclk_prepare = 0x0000000Fu;
        p_cfg->mipi_phy_timing.mipi_tclk_settle  = 0x0000001Eu;
        p_cfg->mipi_phy_timing.mipi_tclk_miss    = 0x00000008u;
        p_cfg->mipi_phy_timing.mipi_t_init_slave = 0x0000338Fu;
    }

    void SetVinSetup(DisplayBase::video_vin_setup_t * p_setup) {
        p_setup->vin_preclip.vin_preclip_starty = 24u;
        p_setup->vin_preclip.vin_preclip_endy   = 503u;
        p_setup->vin_preclip.vin_preclip_startx = 100u;
        p_setup->vin_preclip.vin_preclip_endx   = 739u;

        p_setup->vin_scale.vin_scaleon          = 0;
        p_setup->vin_scale.vin_interpolation    = 0;
        p_setup->vin_scale.vin_scale_h          = 0;
        p_setup->vin_scale.vin_scale_v          = 0;

        p_setup->vin_afterclip.vin_afterclip_size_x = 0;
        p_setup->vin_afterclip.vin_afterclip_size_y = 0;

        p_setup->vin_yuv_clip                   = VIN_CLIP_NONE;
        p_setup->vin_lut                        = VIN_LUT_OFF;
        p_setup->vin_inputformat                = VIN_INPUT_RAW8;
        p_setup->vin_outputformat               = VIN_OUTPUT_RAW8;
        p_setup->vin_outputendian               = VIN_OUUPUT_EN_LITTLE;
        p_setup->vin_dither                     = VIN_DITHER_CUMULATIVE;
        p_setup->vin_interlace                  = VIN_PROGRESSIVE;
        p_setup->vin_alpha_val8                 = 0;
        p_setup->vin_alpha_val1                 = 0;
        p_setup->vin_stride                     = (p_setup->vin_preclip.vin_preclip_endx - p_setup->vin_preclip.vin_preclip_startx + 0x3F) & ~0x3F;
        p_setup->vin_ycoffset                   = 0;
    }

};

#endif

