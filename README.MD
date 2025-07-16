# PMW3610 Arduino Library

PMW3610光学センサー用のArduinoライブラリです。このライブラリを使用することで、PMW3610センサーからモーションデータを簡単に読み取ることができます。

## 特徴

- 3線SPI BitBang通信に対応
- 標準SPIライブラリ不要
- モーションデータの読み取り（X/Y移動量、品質情報）
- CPI（Count Per Inch）設定可能（200-3200）
- 12ビット符号付き移動量データ対応

## ファイル構成

```
PMW3610_Module/
├── src/
│   ├── PMW3610.h          # ヘッダファイル
│   └── PMW3610.cpp        # 実装ファイル
├── README.MD              # このファイル
├── library.properties     # ライブラリプロパティ
├── keywords.txt           # キーワード定義
└── examples/              # サンプルコード
    ├── BasicReading/
    │   └── BasicReading.ino
    ├── MouseExample/
    │   └── MouseExample.ino
    └── InterruptExample/
        └── InterruptExample.ino
```

## 対応ハードウェア

- PMW3610光学センサー
- Arduino Uno, Nano, Pro Mini など
- ESP32, ESP8266
- その他デジタルI/O対応マイコン

**注意**: PMW3610は3.3V動作です。5V Arduinoボードで使用する場合は、ロジックレベルシフターまたは分圧回路が必要です。

## 配線

| PMW3610ピン | Arduinoピン例 | 説明                                             |
| ----------- | ------------- | ------------------------------------------------ |
| VCC         | 3.3V          | 電源（3.3V）                                     |
| GND         | GND           | グランド                                         |
| NCS         | D10           | チップセレクト（任意のデジタルピン）             |
| SCLK        | D13           | SPIクロック（任意のデジタルピン）                |
| SDIO        | D11           | SPIデータ双方向（任意のデジタルピン）            |
| MOTION      | D2            | 動作検出割り込みピン（オプション）               |

**重要**: 3線SPI BitBang実装のため、標準SPIピンを使用する必要はありません。任意のデジタルピンが使用可能です。

## インストール

このリポジトリをダウンロードまたはクローンし、Arduinoのlibrariesフォルダに配置してください



## API リファレンス

### クラス: PMW3610

#### コンストラクタ

```cpp
PMW3610(uint8_t ncsPin, uint8_t sclkPin, uint8_t sdioPin)
```

- `ncsPin`: チップセレクトピン番号
- `sclkPin`: SPIクロックピン番号
- `sdioPin`: SPI双方向データピン番号

#### メソッド

##### `bool begin()`

センサーを初期化します。Product ID（0x42）を確認して初期化の成功を判定します。

**戻り値**
- `true`: 初期化成功
- `false`: 初期化失敗

##### `PMW3610_data readMotion()`

モーションデータを読み取ります。12ビット符号付き値として処理されます。

**戻り値**
- `PMW3610_data`: モーションデータ構造体

##### `uint16_t getCpi()`

現在のCPI設定を取得します。

**戻り値**
- 現在のCPI値

##### `void setCpi(uint16_t cpi)`

CPI（Count Per Inch）を設定します。入力値は最も近い有効値に丸められます。

**パラメータ**
- `cpi`: CPI値（200, 400, 600, 800, 1000, 1200, 1600, 3200）

##### `void writeReg(uint8_t reg, uint8_t value)`

レジスタに値を書き込みます。

**パラメータ**
- `reg`: レジスタアドレス
- `value`: 書き込む値

##### `uint8_t readReg(uint8_t reg)`

レジスタから値を読み取ります。

**パラメータ**
- `reg`: レジスタアドレス

**戻り値**
- レジスタの値

### 構造体: PMW3610_data

```cpp
struct PMW3610_data {
    bool isMotion;          // モーション検出フラグ
    int16_t dx;             // X軸移動量（12ビット符号付き、符号拡張済み）
    int16_t dy;             // Y軸移動量（12ビット符号付き、符号拡張済み）
    uint8_t squal;          // 表面品質（0-255）
};
```

### 主要レジスタ定数

```cpp
// 基本制御レジスタ
#define REG_Product_ID 0x00         // Product ID（0x42）
#define REG_Revision_ID 0x01        // Revision ID
#define REG_Motion 0x02             // Motion状態
#define REG_Delta_X_L 0x03          // X移動量下位8ビット
#define REG_Delta_Y_L 0x04          // Y移動量下位8ビット
#define REG_Delta_XY_H 0x05         // X/Y移動量上位4ビット
#define REG_SQUAL 0x06              // Surface Quality
#define REG_Power_Up_Reset 0x3A     // パワーアップリセット
#define REG_Shutdown 0x3B           // シャットダウン制御
#define REG_Res_Step 0x85           // CPI設定レジスタ

// その他の制御レジスタ
#define REG_Shutter_Higher 0x07     // シャッター上位
#define REG_Shutter_Lower 0x08      // シャッター下位
#define REG_Pix_Max 0x09           // 最大ピクセル値
#define REG_Burst_Read 0x12        // バースト読み取り
```


## 仕様

- **通信方式**: 3線SPI BitBang
- **電源電圧**: 3.3V（2.7V-3.6V）
- **消費電流**: 15mA（通常動作時）
- **CPI範囲**: 200, 400, 600, 800, 1000, 1200, 1600, 3200
- **データ解像度**: 12ビット符号付き
- **最大フレームレート**: 12,000 FPS
- **最大速度**: 60 IPS（Inch Per Second）
- **Product ID**: 0x42
- **動作温度**: -40°C to +85°C

## ライセンス

このライブラリはMITライセンスの下で公開されています。詳細は`LICENSE`ファイルを参照してください。

