#include <SPI.h>
#include "PMW3610.h"

// Arduinoのピンに合わせて設定してください
// 5V Arduinoの場合はロジックレベルシフタが必要です
const int PIN_NCS = 10; // チップセレクトピン

// PMW3610センサーオブジェクトを作成
PMW3610 sensor(PIN_NCS);

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ; // シリアルポートが開くのを待つ

    Serial.println("PMW3610 Basic Read Example");

    // センサーの初期化
    if (sensor.begin())
    {
        Serial.println("Sensor initialization successful.");
    }
    else
    {
        Serial.println("Sensor initialization failed!");
        while (1)
            ; // 失敗した場合はここで停止
    }
}

void loop()
{
    // モーションデータを格納する構造体
    PMW3610::MotionData data;

    // モーションデータを読み取る
    sensor.readMotion(&data);

    // isMotionフラグが立っている場合（動きがあった場合）にのみ出力
    if (data.isMotion)
    {
        Serial.print("Motion detected! ");
        Serial.print("dx: ");
        Serial.print(data.dx);
        Serial.print(", dy: ");
        Serial.println(data.dy);
    }

    delay(10); // ポーリング間隔
}