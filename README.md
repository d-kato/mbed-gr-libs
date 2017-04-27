# **mbed GRボード用ライブラリ**  mbed-gr-libs
GR-PEACH、GR-LYCHEE用のライブラリ群です。ライブラリには以下の機能が含まれます。  
* カメラとLCDの接続
* USBメモリとSDカードの接続
* キャッシュ制御
* JPEG変換
* RGAライブラリ(GR-PEACHのみ)
* オーディオコーデック
* USBホスト
* USBファンクション

GR-LYCHEEの開発環境については、[GR-LYCHEE用オフライン開発環境の手順](https://developer.mbed.org/users/dkato/notebook/offline-development-lychee-langja/)を参照ください。


## カメラとLCDの接続
``EasyAttach_CameraAndLCD``を使うことで、カメラとLCDの接続を簡単に行うことができます。  
サンプルコード：[GR-Boads_Camera_LCD_sample](https://github.com/d-kato/GR-Boads_Camera_LCD_sample)  

***カメラとLCDの有効化***  
プロジェクトに``mbed_app.json``ファイルを追加します。
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

***カメラとLCDの指定***  
カメラとLCDの指定を行う場合は``mbed_app.json``に以下を追加してください。
camera-typeとlcd-typeを指定しない場合はそのボードで一般的なデバイスが選択されます。
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

***mbed CLI以外の環境で使用する場合***  
mbed CLI以外の環境をお使いの場合、``mbed_app.json``の変更は反映されません。  
``mbed_config.h``に以下のようにマクロを追加してください。  
```cpp
#define MBED_CONF_APP_CAMERA                        1    // set by application
#define MBED_CONF_APP_CAMERA_TYPE                   CAMERA_CVBS             // set by application
#define MBED_CONF_APP_LCD                           0    // set by application
#define MBED_CONF_APP_LCD_TYPE                      GR_PEACH_4_3INCH_SHIELD // set by application
```

***使い方***  
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


## USBメモリとSDカードの接続
``SdUsbConnectクラス``を使うことで、USBメモリとSDカードの接続を簡単に行うことができます。  
両方のデバイスが挿入されている場合は、先に検出した方のデバイスに接続します。
```cpp
#include "SdUsbConnect.h"
#define MOUNT_NAME             "storage"

int main() {
  SdUsbConnect storage(MOUNT_NAME);

  //接続完了待ち
  storage.wait_connect();

  while (1) {
    //接続確認
    if (!storage.connected()) { //切断
      printf("disconnect\r\n");
      break;
    }
  }
}
```
サンプルコード：[GR-Boards_Audio_WAV](https://github.com/d-kato/GR-Boards_Audio_WAV)  


## キャッシュ制御
DMAを使用する際はキャッシュ制御を意識する必要があります。DMAは、Direct Memory Accessの略で、CPUを介さずにメモリにアクセスします。  
`buf[0] = 0x01;`や`memcpy(buf, data, 64)`など、CPUを使ったデータ書き込みを行う場合、通常は実メモリではなくキャッシュへの書き込みが行われます。  
この状態で`buf`のデータをDMAで転送しようとした場合、まだ実メモリには書き込みが行われていないため、意図したデータの転送ができません。  
また逆に、DMAで転送されたデータに対してCPUを使ったアクセスをする場合、キャッシュ上にゴミが残ったままですと実メモリではなくキャッシュ上のゴミを読み込んでしまいます。  
mbedコードを使用する場合、GR-PEACH、GR-LYCHEEは共に、1MBの非キャッシュ領域(`NC_BSS`セクション)を用意しています。  
DMAを使用する場合はこの非キャッシュメモリを使用すると制御が楽になります。  
```cpp
//非キャッシュメモリ
#if defined(__ICCARM__)
static uint8_t buf[64]@ ".mirrorram";
#else
static uint8_t buf[64]__attribute((section("NC_BSS")));
#endif

void dma_send_func() {
  buf[0] = 0x01;
  DMA_send(buf); //DMA送信
}

void dma_recv_func() {
  DMA_recv(buf); //DMA受信
  printf("%d\r\n", buf[0]);
}
```

非キャッシュメモリをを使用しない場合は、以下のようなキャッシュ制御が必要となります。  
キャッシュ制御を行うメモリは必ず32byteアラインにし、サイズを32byteの倍数にしておく必要があります。

```cpp
#include "dcache-control.h" //★キャッシュ制御用のヘッダを追加

//通常メモリに配置 キャッシュ制御を行うメモリは必ず32byteにアライン、32byteの倍数にする
#if defined(__ICCARM__)
#pragma data_alignment=32
static uint8_t buf[64];
#else
static uint8_t buf[64]__attribute((aligned(32)));
#endif

void dma_send_func() {
  buf[0] = 0x01;
  dcache_clean(buf, sizeof(buf)); //★キャッシュ上のデータを実メモリに書き込む
  DMA_send(buf); //DMA送信
}

void dma_recv_func() {
  dcache_invalid(buf, sizeof(buf)); //★あらかじめキャッシュ上のデータを破棄しておく
  DMA_recv(buf); //DMA受信
  printf("%d\r\n", buf[0]);
}
```


## JPEG変換
JPEG変換にはDMAが使用されます。上記``キャッシュ制御`` を参照ください。  
その他の詳細は[GraphicsFramework](https://developer.mbed.org/teams/Renesas/code/GraphicsFramework/)を参照ください。  
サンプルコード：[GR-Boads_Camera_sample](https://github.com/d-kato/GR-Boads_Camera_sample)  


## RGAライブラリ(GR-PEACHのみ)
[GraphicsFramework](https://developer.mbed.org/teams/Renesas/code/GraphicsFramework/)を参照ください。


## オーディオコーデック
``AUDIO_GRBoardクラス``を使うことで、オーディオコーデックとの接続を簡単に行うことができます。  
オーディオデータの転送にはDMAが使用されます。上記``キャッシュ制御`` を参照ください。  
サンプルコード：[GR-Boards_Audio_WAV](https://github.com/d-kato/GR-Boards_Audio_WAV)  


## USBホスト
mbed OSではオフィシャルのUSBホスト機能はまだ実装されていません。mbed classicで使用していたUSBHostをmbed OS 5で使用できるように変更しています。  
詳細は[USBHost](https://developer.mbed.org/handbook/USBHost)を参照ください。  
補足：USBHostMSDクラスはmbed OS 5.4の仕様に合わせて大幅に変更しています。USBメモリ接続の際は``SdUsbConnectクラス``を参照ください。  

## USBファンクション
詳細は[USBDevice](https://developer.mbed.org/handbook/USBDevice)を参照ください。  
