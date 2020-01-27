
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
        const char RaspberryPi_InitRegTable_1[][3] = {
            {0x01, 0x00, 0x00}, {0x01, 0x03, 0x01}
        };
        const char RaspberryPi_InitRegTable_2[][3] = {
            {0x01, 0x14, 0x01}, {0x01, 0x28, 0x01}, {0x01, 0x2A, 0x0C}, {0x01, 0x2B, 0x00},
            {0x01, 0x60, 0x0A}, {0x01, 0x61, 0x28}, {0x01, 0x64, 0x01}, {0x01, 0x65, 0x80},
            {0x01, 0x66, 0x0B}, {0x01, 0x67, 0xFF}, {0x01, 0x68, 0x02}, {0x01, 0x69, 0x44},
            {0x01, 0x6A, 0x07}, {0x01, 0x6B, 0xE3}, {0x01, 0x6C, 0x05}, {0x01, 0x6D, 0x40},
            {0x01, 0x6E, 0x04}, {0x01, 0x6F, 0x38}, {0x01, 0x8C, 0x08}, {0x01, 0x8D, 0x08},
            {0x01, 0x74, 0x01}, {0x01, 0x75, 0x01}, {0x01, 0x76, 0x01}, {0x01, 0x77, 0x01},
            {0x01, 0x57, 0x00}, {0x01, 0x5A, 0x0A}, {0x01, 0x5B, 0x28}, {0x01, 0x1C, 0x00},
            {0x01, 0x1D, 0x57}, {0x01, 0x24, 0x00}, {0x01, 0x25, 0xBF}, {0x03, 0x01, 0x05},
            {0x03, 0x04, 0x02}, {0x03, 0x05, 0x02}, {0x03, 0x06, 0x00}, {0x03, 0x07, 0x2B},
            {0x03, 0x09, 0x08}, {0x03, 0x0C, 0x00}, {0x03, 0x0D, 0x2E}, {0x01, 0x00, 0x01}
        };
        int ret;
#if defined(TARGET_GR_MANGO)
        I2C mI2c_(I2C_SDA, I2C_SCL);
        DigitalOut cam_gpio0(P6_6);
        DigitalIn  cam_gpio1(P6_7);
#elif defined(TARGET_RZ_A2M_EVB) || defined(TARGET_RZ_A2M_EVB_HF) || defined(TARGET_RZ_A2M_SBEV) || defined(TARGET_SEMB1402)
        I2C mI2c_(PD_5, PD_4);
        DigitalOut cam_gpio0(PD_2);
        DigitalIn  cam_gpio1(PD_3);
#endif
        cam_gpio0 = 1;
        ThisThread::sleep_for(10);

        mI2c_.frequency(150000);

        for (uint32_t i = 0; i < (sizeof(RaspberryPi_InitRegTable_1) / 3) ; i++) {
            ret = mI2c_.write(0x20, RaspberryPi_InitRegTable_1[i], 3);
            if (ret != 0) {
                return false;
            }
        }

        ThisThread::sleep_for(1);

        for (uint32_t i = 0; i < (sizeof(RaspberryPi_InitRegTable_2) / 3) ; i++) {
            ret = mI2c_.write(0x20, RaspberryPi_InitRegTable_2[i], 3);
            if (ret != 0) {
                return false;
            }
        }

        return true;
    }

    /* MIPI camera setting */
    #define     CAM_MIPI_LANE_NUM        ( 2u)          /* Number of data lane */
    #define     CAM_MIPI_LANE_SWAP       ( 0u)          /* Lane swap enable */
    #define     CAM_MIPI_SEL_VC          ( 0u)          /* Vertual channel number */
    #define     CAM_MIPI_TRANSFER_RATE   (80u)          /* Data transfer rate (Mbit per sec) */

    /* PHY timing */
    #define     CAM_MIPI_THS_PREPARE     (0x00000009u)  /* Setting of the duration of the LP-00 state (immediately before entry to the HS-0 state) */ /* @suppress("Line length formatting") */
    #define     CAM_MIPI_THS_SETTLE      (0x00000014u)  /* Setting of the period in which a transition to the HS state is ignored after the TTHS_PREPARE period begins */ /* @suppress("Line length formatting") */
    #define     CAM_MIPI_TCLK_PREPARE    (0x0000000Au)  /* Setting of the duration of the LP-00 state (immediately before entry to the HS-0) */ /* @suppress("Line length formatting") */
    #define     CAM_MIPI_TCLK_SETTLE     (0x00000014u)  /* Setting of the period in which a transition to the HS state is ignored after the TCLK_PREPARE period begins */ /* @suppress("Line length formatting") */
    #define     CAM_MIPI_TCLK_MISS       (0x00000003u)  /* Setting of the period in which the absence of the clock is detected, and the HS-RX is disabled */ /* @suppress("Line length formatting") */
    #define     CAM_MIPI_T_INIT_SLAVE    (0x000033F3u)  /* Minimum duration of the INIT state */

    /* VIN capture setting */
    #define     CAM_VIN_PRECLIP_START_Y  (  0u)                 /* Start line  of pre clip area */
    #define     CAM_VIN_PRECLIP_WIDTH_Y  (720u)                 /* Line width of pre clip area */
    #define     CAM_VIN_PRECLIP_START_X  (  0u)                 /* Start pixel of pre clip area */
    #define     CAM_VIN_PRECLIP_WIDTH_X  (1280u)                /* Pixel width of pre clip area */
    #define     CAM_VIN_INPUT_YCAL       (DisplayBase::VIN_Y_UPPER)          /* YCbCr422 data alignment */
    #define     CAM_VIN_INPUT_FORMAT     (VIN_INPUT_RAW8)       /* Input image format */
    #define     CAM_VIN_OUTPUT_FORMAT    (VIN_OUTPUT_RAW8)      /* Output image format */
    #define     CAM_VIN_OUTPUT_ENDIAN    (VIN_OUTPUT_EN_LITTLE) /* Output data endian */
    #define     CAM_VIN_OUTPUT_BPSM      (DisplayBase::VIN_SWAP_OFF)         /* Output data byte swap mode */
    #define     CAM_VIN_OUTPUT_IS        (1280u)                 /* Image Stride size */

    void SetMipiConfig(DisplayBase::video_mipi_param_t * p_cfg) {
        memset(p_cfg, 0, sizeof(DisplayBase::video_mipi_param_t));
        p_cfg->mipi_lanenum    = CAM_MIPI_LANE_NUM;
        p_cfg->mipi_vc         = CAM_MIPI_SEL_VC;
        p_cfg->mipi_interlace  = MIPI_PROGRESSIVE;
        p_cfg->mipi_laneswap   = CAM_MIPI_LANE_SWAP;
        p_cfg->mipi_frametop   = 0;
        p_cfg->mipi_outputrate = CAM_MIPI_TRANSFER_RATE;
        p_cfg->mipi_phy_timing.mipi_ths_prepare  = CAM_MIPI_THS_PREPARE;
        p_cfg->mipi_phy_timing.mipi_ths_settle   = CAM_MIPI_THS_SETTLE;
        p_cfg->mipi_phy_timing.mipi_tclk_prepare = CAM_MIPI_TCLK_PREPARE;
        p_cfg->mipi_phy_timing.mipi_tclk_settle  = CAM_MIPI_TCLK_SETTLE;
        p_cfg->mipi_phy_timing.mipi_tclk_miss    = CAM_MIPI_TCLK_MISS;
        p_cfg->mipi_phy_timing.mipi_t_init_slave = CAM_MIPI_T_INIT_SLAVE;
    }

    void SetVinSetup(DisplayBase::video_vin_setup_t * p_setup) {
        memset(p_setup, 0, sizeof(DisplayBase::video_vin_setup_t));
        p_setup->vin_preclip.vin_preclip_starty = CAM_VIN_PRECLIP_START_Y;
        p_setup->vin_preclip.vin_preclip_endy   = ((CAM_VIN_PRECLIP_START_Y + CAM_VIN_PRECLIP_WIDTH_Y) - 1);
        p_setup->vin_preclip.vin_preclip_startx = CAM_VIN_PRECLIP_START_X;
        p_setup->vin_preclip.vin_preclip_endx   = ((CAM_VIN_PRECLIP_START_X + CAM_VIN_PRECLIP_WIDTH_X) - 1);
        p_setup->vin_inputformat  = CAM_VIN_INPUT_FORMAT;
        p_setup->vin_outputformat = CAM_VIN_OUTPUT_FORMAT;
        p_setup->vin_outputendian = CAM_VIN_OUTPUT_ENDIAN;
        p_setup->vin_interlace    = VIN_PROGRESSIVE;
        p_setup->vin_stride       = CAM_VIN_OUTPUT_IS;
        p_setup->vin_ycoffset     = (CAM_VIN_OUTPUT_IS * CAM_VIN_PRECLIP_WIDTH_Y);
        p_setup->vin_input_align  = CAM_VIN_INPUT_YCAL;
        p_setup->vin_output_swap  = CAM_VIN_OUTPUT_BPSM;
    }

};

#endif

