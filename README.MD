# PMW3610 Arduino Library

PMW3610光学センサー用のArduinoライブラリです。このライブラリを使用することで、PMW3610センサーからモーションデータを簡単に読み取ることができます。

## 特徴

- 3線SPI通信に対応
- モーションデータの読み取り（X/Y移動量、品質情報）
- CPI（Count Per Inch）設定可能（200-3200）
- Motion Burstモードでの高速データ読み取り
- 簡単な初期化とセットアップ

## 対応ハードウェア

- PMW3610光学センサー
- Arduino Uno, Nano, Pro Mini など（3.3V推奨）
- ESP32, ESP8266
- その他SPI対応マイコン

**注意**: 5V ArduinoボードでPMW3610（3.3V）を使用する場合は、ロジックレベルシフターが必要です。

## 配線

| PMW3610ピン | Arduinoピン | 説明 |
|-------------|-------------|------|
| VCC | 3.3V | 電源（3.3V） |
| GND | GND | グランド |
| SCLK | D13 (SCK) | SPIクロック |
| SDIO | D11 (MOSI) | SPIデータ（双方向） |
| NCS | D10 | チップセレクト（任意のデジタルピン） |

## インストール

1. このライブラリをArduino IDEのlibrariesフォルダにコピー
2. Arduino IDEを再起動
3. `スケッチ` → `ライブラリをインクルード` → `PMW3610_Module`を選択

## 使用方法

### 基本的な使用例

```cpp
#include <SPI.h>
#include "PMW3610.h"

const int PIN_NCS = 10; // チップセレクトピン

PMW3610 sensor(PIN_NCS);

void setup() {
    Serial.begin(115200);
    
    if (sensor.begin()) {
        Serial.println("センサー初期化成功");
    } else {
        Serial.println("センサー初期化失敗");
        while (1);
    }
}

void loop() {
    PMW3610::PMW3610_data data = sensor.readMotion();
    
    if (data.isMotion) {
        Serial.print("dx: ");
        Serial.print(data.dx);
        Serial.print(", dy: ");
        Serial.println(data.dy);
    }
    
    delay(10);
}
```

### CPI設定

```cpp
// CPIを1600に設定
sensor.setCpi(1600);
```

## API リファレンス

### クラス: PMW3610

#### コンストラクタ

```cpp
PMW3610(uint8_t ncsPin)
```

- `ncsPin`: チップセレクトピン番号

#### メソッド

##### `bool begin()`

センサーを初期化します。

**戻り値**
- `true`: 初期化成功
- `false`: 初期化失敗

##### `PMW3610_data readMotion()`

モーションデータを読み取ります。

**戻り値**
- `PMW3610_data`: モーションデータ構造体

##### `unsigned int getCpi()`

現在のCPI設定を取得します。

**戻り値**
- 現在のCPI値

##### `void setCpi(uint16_t cpi)`

CPI（Count Per Inch）を設定します。

**パラメータ**
- `cpi`: CPI値（200-3200、200の倍数）

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
    int16_t dx;             // X軸移動量
    int16_t dy;             // Y軸移動量
    uint8_t squal;          // 表面品質
    uint8_t shutterUpper;   // シャッター上位バイト
    uint8_t shutterLower;   // シャッター下位バイト
    uint8_t maxPixel;       // 最大ピクセル値
};
```

## トラブルシューティング

### センサーが初期化されない

1. **配線の確認**: 特にSPI接続とNCSピンを確認
2. **電圧レベル**: 3.3V電源を使用しているか確認
3. **ロジックレベルシフター**: 5V Arduinoの場合は必要

### モーションが検出されない

1. **表面**: センサーの下に適切な表面があるか確認
2. **CPI設定**: 使用環境に適したCPI値に調整
3. **ポーリング頻度**: delay時間を調整

### データが不正確

1. **SPI通信**: ノイズや接続不良を確認
2. **電源の安定性**: 3.3V電源が安定しているか確認

## 仕様

- **通信方式**: 3線SPI
- **電源電圧**: 3.3V
- **CPI範囲**: 200-3200（200の倍数）
- **最大フレームレート**: 12,000 FPS
- **最大速度**: 60 IPS（Inch Per Second）

## ライセンス

このライブラリはMITライセンスの下で公開されています。

## 貢献

バグ報告や機能要望は、GitHubのIssuesページでお願いします。プルリクエストも歓迎します。

## 更新履歴

### v1.0.0
- 初回リリース
- 基本的なモーション読み取り機能
- CPI設定機能
- Motion Burstモード対応
