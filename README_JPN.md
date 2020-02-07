# **Mbed GRボード用ライブラリ**  mbed-gr-libs
GR-PEACH、GR-LYCHEE、RZ/A2Mボード用のライブラリ群です。ライブラリには以下の機能が含まれます。  
* カメラとLCDの接続
* USBメモリとSDカードの接続
* キャッシュ制御
* JPEG変換
* RGAライブラリ (GR-PEACHのみ)
* オーディオ再生
* USBホスト
* ESP32 ATコマンド用ライブラリ
* DisplayApp (USB接続でPCディスプレイに画像表示)
* ASCIIフォント
* SDカードの通信速度
* DRP (RZ/A2Mのみ)

## カメラとLCDの接続
``EasyAttach_CameraAndLCD``を使うことで、カメラとLCDの接続を簡単に行うことができます。  
サンプルコード：[GR-Boads_Camera_LCD_sample](https://github.com/d-kato/GR-Boads_Camera_LCD_sample)  
詳細は [EasyAttach_CameraAndLCD/README.md](EasyAttach_CameraAndLCD/README.md) を参照ください。


## USBメモリとSDカードの接続
``SdUsbConnectクラス``を使うことで、USBメモリとSDカードの接続を簡単に行うことができます。  
両方のデバイスが挿入されている場合は、先に検出した方のデバイスに接続します。
```cpp
#include "SdUsbConnect.h"

int main() {
  SdUsbConnect storage("storage");

  // 接続待ち
  storage.wait_connect();

  while (1) {
    // 接続確認
    if (!storage.connected()) { // 切断
      printf("disconnect\r\n");
      break;
    }

    FILE * fp = fopen("/storage/test.txt", "rb");
    char buf[32];
    fread(buf, sizeof(char), 32, fp);
    printf("%s", buf);
    fclose(fp);
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


## オーディオ再生
``EasyPlaybackクラス``を使うことで、簡単にオーディオを再生することができます。  
サンプルコード：[GR-Boards_Audio_WAV](https://github.com/d-kato/GR-Boards_Audio_WAV)  


## USBホスト
mbed OSではオフィシャルのUSBホスト機能はまだ実装されていません。mbed classicで使用していたUSBHostをmbed OS 5で使用できるように変更しています。  
詳細は[USBHost](https://developer.mbed.org/handbook/USBHost)を参照ください。  
補足：USBHostMSDクラスはmbed OS 5.4の仕様に合わせて大幅に変更しています。USBメモリ接続の際は``SdUsbConnectクラス``を参照ください。  


## ESP32 ATコマンド用ライブラリ
``ESP32Interfaceクラス``を使うことで、WiFi通信を行うことができます。  
詳細は[TCPSocketWiFi_Example_for_ESP32](https://github.com/d-kato/TCPSocketWiFi_Example_for_ESP32)を参照ください。  


## DisplayApp (USB接続でPCディスプレイに画像表示)
GR-Board内のJPEG画像をPC上に表示するためのライブラリです。GR-BoardとPCをUSBで接続して使用します。USBケーブルは``MicroUSB Connector(RZ/A1 Ch.0)``に接続してください。  

![](docs/img/usb0_and_button.jpg)  

RZ/A2Mの場合は``USBチャネル1``に接続してください。  
Windows10以外ご使用の場合、ドライバのインストールが必要となります。下記サイトのからドライバーをダウンロードできます。  

https://os.mbed.com/handbook/USBSerial

但し、「署名なしドライバ」となっていますので、お使いのWindowsバージョンによってはそのままインストールすることはできません。お使いのPC毎に設定方法が異なるため、検索サイトで「署名なしドライバ」で検索してください。  

PC用アプリは以下よりダウンロードできます。  
* [For Windows](http://gadget.renesas.com/software/displayapp.zip)  
* [For Mac](http://gadget.renesas.com/software/DisplayApp.app.zip)  
* [For Linux](https://github.com/takjn/GRDisplayApp)  

使用例
```cpp
#include "DisplayApp.h"
#include "mbed.h"

static uint8_t jpeg_image[] = {/* JPEG image data */};

int main() {
    DisplayApp display_app;

    while (1) {
        display_app.SendJpeg(&jpeg_image[0], sizeof(jpeg_image));
        wait(1);
    }
}
```


## ASCIIフォント
``AsciiFontクラス``を使うことで、ASCIIコードの文字を描画できます。  

```cpp
#include "mbed.h"
#include "AsciiFont.h"

#define WIDTH           (12)
#define HEIGHT          (16)
#define BYTE_PER_PIXEL  (1u)
#define STRIDE          (((WIDTH * BYTE_PER_PIXEL) + 7u) & ~7u) //multiple of 8

uint8_t text_field[STRIDE * HEIGHT];

int main() {
    AsciiFont ascii_font(text_field, WIDTH, HEIGHT, STRIDE, BYTE_PER_PIXEL);

    ascii_font.Erase(0xcc);  // Erase text field
    ascii_font.DrawStr("AB", 0, 0, 0x11, 1);
    ascii_font.DrawChar('C', AsciiFont::CHAR_PIX_WIDTH, AsciiFont::CHAR_PIX_HEIGHT, 0x22, 1);
}
```

## SDカードの通信速度
RZ/A2MはSDスピードクラスで通信を行います。RZ/A2MでHSスピードクラス、または、UHSスピードクラスが必要な方は、[こちら]((https://www.renesas.com/jp/ja/support/contact.html)よりお問合せください。  
GR-PEACH、GR-LYCHEEは SPIバスを使ってSDカードにアクセスします。  
接続については``SdUsbConnectクラス``を参照ください。  


## DRP (RZ/A2Mのみ)
DRP(Dynamically Reconfigurable Processor)はソフトウェアの柔軟さとハードウェアの高速性を兼ね備えたプログラマブルハードウェアです。処理を定義するファームウェアの書き換えが瞬間に行えます。  
詳しくは ``drp-for-mbed/TARGET_RZ_A2XX/r_drp/doc`` を参照ください。  
