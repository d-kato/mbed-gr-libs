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
            "help": "Options are CAMERA_CVBS, CAMERA_MT9V111, CAMERA_OV7725",
            "value": "CAMERA_CVBS"
        },
        "lcd":{
            "help": "0:disable 1:enable",
            "value": "1"
        },
        "lcd-type":{
            "help": "Options are GR_PEACH_4_3INCH_SHIELD, GR_PEACH_7_1INCH_SHIELD, GR_PEACH_RSK_TFT, GR_PEACH_DISPLAY_SHIELD, GR_LYCHEE_LCD",
            "value": "GR_PEACH_4_3INCH_SHIELD"
        }
    }
}
```

If camera-type and lcd-type are not specified, the general one that operates on that board will be specified.


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
