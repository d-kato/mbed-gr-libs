
#ifndef MT9V111_CONFIG_H
#define MT9V111_CONFIG_H

#include "mbed.h"
#include "camera_config.h"

class MT9V111_config : public camera_config {

public:

    /** Initialise
     *
     * @return true = success, false = failure
     */
    virtual bool Initialise() {
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
        p_cfg->inp_hs_inv     = DisplayBase::SIG_POL_INVERTED;         /* External input DV_HSYNC inversion control     */
        p_cfg->inp_f525_625   = DisplayBase::EXTIN_LINE_525;           /* Number of lines for BT.656 external input */
        p_cfg->inp_h_pos      = DisplayBase::EXTIN_H_POS_CRYCBY;       /* Y/Cb/Y/Cr data string start timing to Hsync reference */
        p_cfg->cap_vs_pos     = 6;                                     /* Capture start position from Vsync */
        p_cfg->cap_hs_pos     = 150;                                   /* Capture start position form Hsync */
        p_cfg->cap_width      = 640;                                   /* Capture width Max */
        p_cfg->cap_height     = 468u;                                  /* Capture height Max 468[line]
                                                                          Due to CMOS(MT9V111) output signal timing and VDC5 specification */
    }

};

#endif

