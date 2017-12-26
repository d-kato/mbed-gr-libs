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
            "help": "Please see README.md",
            "value": "CAMERA_CVBS"
        },
        "lcd":{
            "help": "0:disable 1:enable",
            "value": "1"
        },
        "lcd-type":{
            "help": "Please see README.md",
            "value": "GR_PEACH_4_3INCH_SHIELD"
        }
    }
}
```

| camera-type "value"     | Description                        |
|:------------------------|:-----------------------------------|
| CAMERA_CVBS             | GR-PEACH NTSC signal               |
| CAMERA_MT9V111          | GR-PEACH MT9V111                   |
| CAMERA_OV7725           | GR-LYHCEE included camera          |
| CAMERA_OV5642           | GR-PEACH OV5642                    |
| CAMERA_WIRELESS_CAMERA  | GR-PEACH Wireless/Camera shield (MT9V111) |

| lcd-type "value"        | Description                        |
|:------------------------|:-----------------------------------|
| GR_PEACH_4_3INCH_SHIELD | GR-PEACH 4.3 inch LCD shield       |
| GR_PEACH_7_1INCH_SHIELD | GR-PEACH 7.1 inch LCD shield       |
| GR_PEACH_RSK_TFT        | GR-PEACH RSK board LCD             |
| GR_PEACH_DISPLAY_SHIELD | GR-PEACH Display Shield            |
| GR_LYCHEE_LCD           | GR-LYHCEE TF043HV001A0..etc(40pin) |


If camera-type and lcd-type are not specified, the following are specified.
* GR-PEACH   camera：CAMERA_MT9V111, LCD：GR_PEACH_4_3INCH_SHIELD  
* GR-LYCHEE  camera：CAMERA_OV7725,  LCD：GR_LYCHEE_LCD  


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
