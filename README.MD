# PMW3610 Arduino Library

PMW3610光学センサー用のArduinoライブラリです。このライブラリを使用することで、PMW3610センサーからモーションデータを簡単に読み取ることができます。

## 特徴

- 3線SPI BitBang通信に対応
- 標準SPIライブラリ不要
- モーションデータの読み取り（X/Y移動量、品質情報）
- CPI（Count Per Inch）設定可能（200-3200）
- 簡単な初期化とセットアップ
- ESP32、Arduino対応

## 対応ハードウェア

- PMW3610光学センサー
- Arduino Uno, Nano, Pro Mini など
- ESP32, ESP8266
- その他デジタルI/O対応マイコン

**注意**: PMW3610は3.3V動作です。5V Arduinoボードで使用する場合は、ロジックレベルシフターまたは分圧回路が必要です。

## 配線

| PMW3610ピン | Arduinoピン例 | 説明 |
|-------------|---------------|------|
| VCC | 3.3V | 電源（3.3V） |
| GND | GND | グランド |
| NCS | D17 | チップセレクト（任意のデジタルピン） |
| SCLK | D18 | SPIクロック（任意のデジタルピン） |
| SDIO | D19 | SPIデータ双方向（任意のデジタルピン） |

**重要**: 3線SPI BitBang実装のため、標準SPIピンを使用する必要はありません。任意のデジタルピンが使用可能です。

## インストール

1. このライブラリをArduino IDEのlibrariesフォルダにコピー
2. Arduino IDEを再起動
3. `スケッチ` → `ライブラリをインクルード` → `PMW3610_Module`を選択

## 使用方法

### 基本的な使用例

```cpp
#include "PMW3610.h"

// 3線SPI用ピン定義
const int PIN_NCS = 17;   // チップセレクト
const int PIN_SCLK = 18;  // SPIクロック
const int PIN_SDIO = 19;  // SPIデータ（双方向）

PMW3610 sensor(PIN_NCS, PIN_SCLK, PIN_SDIO);

void setup() {
    Serial.begin(115200);
    
    if (sensor.begin()) {
        Serial.println("センサー初期化成功");
        sensor.setCpi(1000);  // CPI設定
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
    
    delay(5);
}
```

### CPI設定

```cpp
// CPIを1600に設定
sensor.setCpi(1600);

// 現在のCPI設定を取得
uint16_t currentCpi = sensor.getCpi();
```

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

センサーを初期化します。

**戻り値**
- `true`: 初期化成功
- `false`: 初期化失敗

##### `PMW3610_data readMotion()`

モーションデータを読み取ります。

**戻り値**
- `PMW3610_data`: モーションデータ構造体

##### `uint16_t getCpi()`

現在のCPI設定を取得します。

**戻り値**
- 現在のCPI値

##### `void setCpi(uint16_t cpi)`

CPI（Count Per Inch）を設定します。

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
    int16_t dx;             // X軸移動量
    int16_t dy;             // Y軸移動量
    uint8_t squal;          // 表面品質
    uint8_t shutterUpper;   // シャッター上位バイト
    uint8_t shutterLower;   // シャッター下位バイト
    uint8_t maxPixel;       // 最大ピクセル値
};
```

### レジスタ定数

```cpp
#define REG_Product_ID 0x00
#define REG_Revision_ID 0x01
#define REG_Motion 0x02
#define REG_Delta_X_L 0x03
#define REG_Delta_X_H 0x04
#define REG_Delta_Y_L 0x05
#define REG_Delta_Y_H 0x06
#define REG_SQUAL 0x07
#define REG_Power_Up_Reset 0x3A
#define REG_Resolution 0x47
```

## トラブルシューティング

### センサーが初期化されない

1. **配線の確認**: NCS、SCLK、SDIOピンの接続を確認
2. **電圧レベル**: 3.3V電源を使用しているか確認
3. **ロジックレベル**: 5V Arduinoの場合はレベルシフターが必要
4. **Product ID**: readReg(REG_Product_ID)で0x42が返されるか確認

### モーションが検出されない

1. **表面**: センサーの下に適切な表面があるか確認（光沢のない表面推奨）
2. **CPI設定**: 使用環境に適したCPI値に調整
3. **ポーリング頻度**: delay時間を5ms程度に設定
4. **品質**: data.squal値を確認（低い場合は表面を変更）

### データが不正確

1. **配線**: ノイズや接続不良を確認
2. **電源の安定性**: 3.3V電源が安定しているか確認
3. **ピン設定**: 正しいピン番号が設定されているか確認

## 仕様

- **通信方式**: 3線SPI BitBang
- **電源電圧**: 3.3V
- **CPI範囲**: 200, 400, 600, 800, 1000, 1200, 1600, 3200
- **最大フレームレート**: 12,000 FPS
- **最大速度**: 60 IPS（Inch Per Second）
- **Product ID**: 0x42

## ライセンス

このライブラリはMITライセンスの下で公開されています。

## 貢献

バグ報告や機能要望は、GitHubのIssuesページでお願いします。プルリクエストも歓迎します。

## 更新履歴

### v1.0.0
- 初回リリース
- 3線SPI BitBang実装
- 基本的なモーション読み取り機能
- CPI設定機能（8段階）
