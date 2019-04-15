# Easy Attach Camera And LCD - Easily add all supported Camera and LCD to your mbed OS project

Just declare the desired camera and LCD in your `mbed_app.json` file, and call `EasyAttach_Init()` from your application.

## Enable camera and LCD

Add the following to your ``mbed_app.json`` file:

```json
{
    "config": {
        "camera":{
            "help": "0:disable 1:enable",
            "value": "1"
        },
        "lcd":{
            "help": "0:disable 1:enable",
            "value": "1"
        }
    }
}
```

## Specifying Camera and LCD

Add the following to your ``mbed_app.json`` file:

```json
{
    "config": {
        "camera":{
            "help": "0:disable 1:enable",
            "value": "1"
        },
        "camera-type":{
            "help": "Please see EasyAttach_CameraAndLCD/README.md",
            "value": "CAMERA_CVBS"
        },
        "lcd":{
            "help": "0:disable 1:enable",
            "value": "1"
        },
        "lcd-type":{
            "help": "Please see EasyAttach_CameraAndLCD/README.md",
            "value": "GR_PEACH_4_3INCH_SHIELD"
        }
    }
}
```

| camera-type "value"        | Description                        |
|:---------------------------|:-----------------------------------|
| CAMERA_CVBS                | GR-PEACH NTSC signal               |
| CAMERA_MT9V111             | GR-PEACH MT9V111                   |
| CAMERA_OV7725              | GR-LYHCEE included camera          |
| CAMERA_OV5642              | GR-PEACH OV5642                    |
| CAMERA_WIRELESS_CAMERA     | GR-PEACH Wireless/Camera shield (OV7725) |
| CAMERA_RASPBERRY_PI        | RZ/A2M Raspberry Pi camera         |

| lcd-type "value"           | Description                        |
|:---------------------------|:-----------------------------------|
| GR_PEACH_4_3INCH_SHIELD    | GR-PEACH 4.3 inch LCD shield       |
| GR_PEACH_7_1INCH_SHIELD    | GR-PEACH 7.1 inch LCD shield       |
| GR_PEACH_RSK_TFT           | GR-PEACH RSK board LCD             |
| GR_PEACH_DISPLAY_SHIELD    | GR-PEACH Display Shield            |
| GR_LYCHEE_TF043HV001A0     | GR-LYHCEE TF043HV001A0             |
| GR_LYCHEE_ATM0430D25       | GR-LYHCEE ATM0430D25               |
| GR_LYCHEE_FG040346DSSWBG03 | GR-LYHCEE FG040346DSSWBG03         |
| GR_LYCHEE_LCD              | GR-LYHCEE TF043HV001A0 (For compatibility) |
| RZ_A2M_LVDS_TO_HDMI        | RZ/A2M LVDS To HDMI Board          |
| RZ_A2M_EVB_RSK_TFT         | RZ/A2M RSK TFT APP BOARD           |
| RZ_A2M_DVI_STICK           | RZ/A2M DVI STICK                   |

If camera-type and lcd-type are not specified, the following are specified.
* ``GR-PEACH``    camera:CAMERA_MT9V111, LCD:GR_PEACH_4_3INCH_SHIELD  
* ``GR-LYCHEE``   camera:CAMERA_OV7725,  LCD:GR_LYCHEE_LCD  
* ``RZ/A2M Evaluation Board Kit`` camera:CAMERA_RASPBERRY_PI, LCD:RZ_A2M_DVI_STICK
* ``SBEV-RZ/A2M`` camera:CAMERA_RASPBERRY_PI, LCD:RZ_A2M_LVDS_TO_HDMI
* ``SEMB1402``    camera:CAMERA_RASPBERRY_PI, LCD:RZ_A2M_LVDS_TO_HDMI


## Using Easy Connect Camera from your application

Easy Connect Camera has just one function:

```cpp
#include "EasyAttach_CameraAndLCD.h"

int main(void) {
    DisplayBase Display;

    EasyAttach_Init(Display);

    // Required processing of your program

    EasyAttach_CameraStart(Display, DisplayBase::VIDEO_INPUT_CHANNEL_0);

    // Required processing of your program

    EasyAttach_LcdBacklight(true);

    // Rest of your program
}
```
