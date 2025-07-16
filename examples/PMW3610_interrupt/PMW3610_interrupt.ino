#include "PMW3610.h"

// 3線SPI用ピン定義
const int PIN_NCS = 17;     // チップセレクト
const int PIN_SCLK = 18;    // SPIクロック
const int PIN_SDIO = 19;    // SPIデータ（双方向）
const int PIN_MOTION = 21;  // モーション検出用割り込みピン

// PMW3610センサーインスタンス作成
PMW3610 sensor(PIN_NCS, PIN_SCLK, PIN_SDIO);
PMW3610_data data;  // データ格納用構造体
void motionInterrupt();
volatile bool motionDetected = false;  // モーション検出フラグ
void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;  // シリアルポートが開くまで待機
  if (sensor.begin()) {
    Serial.println("PMW3610 initialized successfully!");
  } else {
    Serial.println("PMW3610 initialization failed!");
    while (1)
      ;
  }
  pinMode(PIN_MOTION, INPUT_PULLUP);                      // モーション検出ピンを入力プルアップに設定
  attachInterrupt(digitalPinToInterrupt(PIN_MOTION), motionInterrupt, FALLING);  // モーション検出割り込み設定
}

void loop() {

  if (motionDetected) {
    data = sensor.readMotion();
    Serial.print(data.dx);
    Serial.print(",");
    Serial.print(data.dy);
    Serial.print(",");
    Serial.println(data.squal);
    motionDetected = false;  // フラグをリセット
  }

}
void motionInterrupt() {
  motionDetected = true;  // モーション検出フラグをセット
}