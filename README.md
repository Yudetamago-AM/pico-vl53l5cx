# pico-vl53l5cx

Raspberry Pi Pico（や他のRP2040ボード）でSTのToFセンサ「VL53L5CX」を使うためのライブラリとその使用例です。

このセンサは、RAMにファームウェアを保持するため、起動時にファームウェアを書き込まないと使えません。しかし、書き込むファームウェア自体は、バイナリファイルの形でしか提供されていません。また、レジスタの情報なども非公開です。
なので、このライブラリはSTの [Ultra Lite Driver (ULD) for VL53L5CX multi-zone sensor](https://www.st.com/ja/embedded-software/stsw-img023.html) が内容のほとんどです。

## 使用例で遊ぶ

1. ```git clone https://github.com/Yudetamago-AM/pico-vl53l5cx.git```あるいはCode>Download Zip
1. ダウンロードしてきたものをビルドする（環境によりやり方が異なると思います）
1. ```build/examples/```内に各使用例のフォルダ（と実行ファイル）が生成されます。

## 自分のコードで使う

1. 自分のプロジェクトにこのライブラリの```api```フォルダをコピペする
1. プロジェクトの（一番上の階層の）CMakeLists.txtに```add_subdirectory("api")```と追記
1. ビルドするソースコードの階層のCMakeLists.txtに```target_link_libraries(「targetの名前」 pico_stdlib hardware_i2c vl53l5cx)```と追記
1. これでビルド・書き込み・実行がうまくいくと思います。

なお、上と違うディレクトリ構造にした時は、適宜パスを変更してください。

## リファレンス

### ULDの機能

apiフォルダ内の各ヘッダーファイルに関数の説明があります。

- （必須）基本的な測距は [vl53l5cx_api.h](./api/vl53l5cx_api.h)
- （任意）キャリブレーション等は [vl53l5cx_plugin_xtalk.h](./api/vl53l5cx_plugin_xtalk.h)
- （任意）しきい値を使った検出は [vl53l5cx_plugin_detection_thresholds.h](./api/vl53l5cx_plugin_detection_thresholds.h)
　注意：センサーのINTピンをRP2040に接続する必要があります。
- （任意）動作の検出は [vl53l5cx_plugin_motion_indicator.h](./api/vl53l5cx_plugin_motion_indicator.h)

### このライブラリとしての機能

[使用例](./examples/)を見てもらった方がわかりやすいかもしれません。

```C
i2c_inst_t vl53l5cx_i2c = {i2c0_hw, false};
あるいは
i2c_inst_t vl53l5cx_i2c = {i2c1_hw, false};
```

のようにしてI2Cバスを指定してあげる必要があります。
また、VL53L5CX_Configurationという構造体型のメンバに```i2c```があるので、それに宣言したI2Cバスのアドレスを指定してあげる必要があります。

```C
VL53L5CX_Configuration 	Dev;

Dev.platform.address = (uint8_t)((VL53L5CX_DEFAULT_I2C_ADDRESS >> 1) & 0xFF);
Dev.platform.i2c     = &vl53l5cx_i2c; // set i2c bus
```

なお、RP2040では7bitのI2Cアドレスを用いるので、1バイト右シフトしています。VL53L5CXは標準で8bitアドレスが0x52（書き込み）・0x53（書き込み）なので、7bitだと0x29となります。

## リンク

- データシート（DS13754 Rev 8）<https://www.st.com/resource/en/datasheet/vl53l5cx.pdf>
- ユーザーマニュアル（UM2284）<https://www.st.com/resource/en/user_manual/um2884-a-guide-to-using-the-vl53l5cx-multizone-timeofflight-ranging-sensor-with-wide-field-of-view-ultra-lite-driver-uld-stmicroelectronics.pdf>

## 実装上の注意

ハマったところをメモ代わりに書いておきます。（ちょっと理解が足りてない感じもあります）
platform.cのWrMulti関数をそのまま実装したり、ある一定のチャンクに区切って実装したりしても、vl53l5cx_api.cのvl53l5cx_init関数中で_vl53l5cx_poll_for_answer関数を呼ぶ部分でエラーとなります。これは、ファームウェアを正常に書き込めていないためです。というのも、Raspbery Pi Pico SDKのI2Cの実装では、「メッセージ」ごとにStop Conditionの代わりにRepeated Start Conditionを送りますが、これにVL53L5CXが対応していないためです。複数バイトを書き込む際には、[VL53L5CXのデータシート](https://www.st.com/resource/en/datasheet/vl53l5cx.pdf) のp.12、Figure.14にあるように、 メッセージごとのStop ConditionもRepeated Start Conditionも送らず、一気にアドレス・レジスタ・データの順で送る必要があります（もちろん「転送」の最後にはStop Conditionが必要です）。なお、ST Communityでも [VL53L5CX driver vl53l5cx_init() fails](https://community.st.com/s/question/0D53W000010uFcTSAU/vl53l5cx-driver-vl53l5cxinit-fails) として同様の問題が議論されています。

## ライセンス

BSD3条項ライセンス。詳しくはLISENCEを見てください。なお、使用したSTのULDならびに改変して使用したRaspberry Pi Pico SDKのコードの一部もBSD3条項ライセンスの下で配布されています。これらについては、コード中に明記してあります。
