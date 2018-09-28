
#ifndef OV7725_CONFIG_H
#define OV7725_CONFIG_H

#include "mbed.h"
#include "camera_config.h"

class OV7725_config : public camera_config {

public:

    /** Initialise
     *
     * @return true = success, false = failure
     */
    virtual bool Initialise() {
        /* OV7725 camera input config */
        const char OV7725_InitRegTable[][2] = {
            {0x0D, 0x41}, /* COM4 */
            {0x0F, 0xC5}, /* COM6 */
        #if 0 /* 30fps(24MHz) */
            {0x11, 0x01}, /* CLKRC */
        #else /* 60fps(48MHz) */
            {0x11, 0x00}, /* CLKRC */
        #endif
            {0x14, 0x1F}, /* COM9 Drop VSYNC/HREF */
            {0x15, 0x40}, /* COM10 HSYNC*/
            {0x17, 0x22}, /* HSTART */
            {0x18, 0xA4}, /* HSIZE */
            {0x19, 0x07}, /* VSTRT */
            {0x1A, 0xF0}, /* VSIZE */
            {0x22, 0x99}, /* BDBase */
            {0x23, 0x02}, /* BDMStep */
            {0x24, 0x60}, /* AEW */
            {0x25, 0x50}, /* AEB */
            {0x26, 0xA1}, /* VPT */
            {0x29, 0xA0}, /* HOutSize */
            {0x2A, 0x00}, /* EXHCH */
            {0x2B, 0x00}, /* EXHCL */
            {0x2C, 0xF0}, /* VOutSize */
            {0x32, 0x00}, /* HREF */
            {0x33, 0x01}, /* DM_LNL */
            {0x3D, 0x03}, /* COM12 */
            {0x42, 0x7F}, /* TGT_B */
            {0x4D, 0x09}, /* FixGain */
            {0x63, 0xE0}, /* AWB_Ctrl0 */
            {0x64, 0xFF}, /* DSP_Ctrl1 */
            {0x65, 0x20}, /* DSP_Ctrl2 */
            {0x66, 0x00}, /* DSP_Ctrl3 */
            {0x67, 0x48}, /* DSP_Ctrl4 */
            {0x6B, 0xAA}, /* AWBCtrl3 */
            {0x7E, 0x04}, /* GAM1 */
            {0x7F, 0x0E}, /* GAM2 */
            {0x80, 0x20}, /* GAM3 */
            {0x81, 0x43}, /* GAM4 */
            {0x82, 0x53}, /* GAM5 */
            {0x83, 0x61}, /* GAM6 */
            {0x84, 0x6D}, /* GAM7 */
            {0x85, 0x76}, /* GAM8 */
            {0x86, 0x7E}, /* GAM9 */
            {0x87, 0x86}, /* GAM10 */
            {0x88, 0x94}, /* GAM11 */
            {0x89, 0xA1}, /* GAM12 */
            {0x8A, 0xBA}, /* GAM13 */
            {0x8B, 0xCF}, /* GAM14 */
            {0x8C, 0xE3}, /* GAM15 */
            {0x8D, 0x26}, /* SLOP */
            {0x90, 0x05}, /* EDGE1 */
            {0x91, 0x01}, /* DNSOff */
            {0x92, 0x05}, /* EDGE2 */
            {0x93, 0x00}, /* EDGE3 */
            {0x94, 0x80}, /* MTX1 */
            {0x95, 0x7B}, /* MTX2 */
            {0x96, 0x06}, /* MTX3 */
            {0x97, 0x1E}, /* MTX4 */
            {0x98, 0x69}, /* MTX5 */
            {0x99, 0x86}, /* MTX6 */
            {0x9A, 0x1E}, /* MTX_Ctrl */
            {0x9B, 0x00}, /* BRIGHT */
            {0x9C, 0x20}, /* CNST */
            {0x9E, 0x81}, /* UVADJ0 */
            {0xA6, 0x04}, /* SDE */
        };
        const char sw_reset_cmd[2] = {0x12, 0x80};
        int ret;
        I2C mI2c_(I2C_SDA, I2C_SCL);
        mI2c_.frequency(150000);

        if (mI2c_.write(0x42, sw_reset_cmd, 2) != 0) {
            return false;
        }
        ThisThread::sleep_for(1);

        for (uint32_t i = 0; i < (sizeof(OV7725_InitRegTable) / 2) ; i++) {
            ret = mI2c_.write(0x42, OV7725_InitRegTable[i], 2);
            if (ret != 0) {
                return false;
            }
        }

        return true;
    }

    virtual void SetExtInConfig(DisplayBase::video_ext_in_config_t * p_cfg) {
        p_cfg->inp_format     = DisplayBase::VIDEO_EXTIN_FORMAT_BT601; /* BT601 8bit YCbCr format */
        p_cfg->inp_pxd_edge   = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing data          */
        p_cfg->inp_vs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Vsync signals */
        p_cfg->inp_hs_edge    = DisplayBase::EDGE_RISING;              /* Clock edge select for capturing Hsync signals */
        p_cfg->inp_endian_on  = DisplayBase::OFF;                      /* External input bit endian change on/off       */
        p_cfg->inp_swap_on    = DisplayBase::OFF;                      /* External input B/R signal swap on/off         */
        p_cfg->inp_vs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_VSYNC inversion control     */
        p_cfg->inp_hs_inv     = DisplayBase::SIG_POL_NOT_INVERTED;     /* External input DV_HSYNC inversion control     */
        p_cfg->inp_f525_625   = DisplayBase::EXTIN_LINE_525;           /* Number of lines for BT.656 external input */
        p_cfg->inp_h_pos      = DisplayBase::EXTIN_H_POS_YCBYCR;       /* Y/Cb/Y/Cr data string start timing to Hsync reference */
        p_cfg->cap_vs_pos     = 4+21;                                  /* Capture start position from Vsync */
        p_cfg->cap_hs_pos     = 68;                                    /* Capture start position form Hsync */
        p_cfg->cap_width      = 640;                                   /* Capture width Max */
        p_cfg->cap_height     = 480;                                   /* Capture height Max */
    }

    /** Exposure and Gain Setting
     *
     * @param[in]      bAuto             : Automatic adjustment ON/OFF(AEC/AGC)
     * @param[in]      usManualExposure  : Exposure time at automatic adjustment OFF  (number of lines)
     * @param[in]      usManualGain      : Gain at automatic adjustment OFF i0x00-0xFF)
     * @return true = success, false = failure
     */
    static bool SetExposure(bool bAuto, uint16_t usManualExposure, uint8_t usManualGain) {
        int ret;
        char cmd[2];
        I2C mI2c_(I2C_SDA, I2C_SCL);
        mI2c_.frequency(150000);

        /* COM8(AGC Enable/AEC Enable) */
        cmd[0] = 0x13;
        ret = mI2c_.write(0x42, &cmd[0], 1);
        if (ret != 0) {
            return false;
        }
        ret = mI2c_.read(0x42, &cmd[1], 1);
        if (ret != 0) {
            return false;
        }

        cmd[0] = 0x13;
        if (bAuto) {
            cmd[1] |= (uint8_t)0x05;
        } else {
            cmd[1] &= (uint8_t)~0x05;
        }
        ret = mI2c_.write(0x42, &cmd[0], 2);
        if (ret != 0) {
            return false;
        }

        if (!bAuto) {
            /* AECH/AECL(exposure) */
            cmd[0] = 0x08;
            cmd[1] = (uint8_t)((usManualExposure & 0xFF00) >> 8);
            ret = mI2c_.write(0x42, &cmd[0], 2);
            if (ret != 0) {
                return false;
            }

            cmd[0] = 0x10;
            cmd[1] = (uint8_t)(usManualExposure & 0x00FF);
            ret = mI2c_.write(0x42, &cmd[0], 2);
            if (ret != 0) {
                return false;
            }

            /* GAIN */
            cmd[0] = 0x00;
            cmd[1] = usManualGain;
            ret = mI2c_.write(0x42, &cmd[0], 2);
            if (ret != 0) {
                return false;
            }
        }

        return true;
    }
};

#endif

