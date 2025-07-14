#include <SPI.h>
#include "PMW3610.h"

// Arduinoのピンに合わせて設定してください
// 5V Arduinoの場合はロジックレベルシフタが必要です
const int PIN_NCS = 10; // チップセレクトピン

// PMW3610センサーオブジェクトを作成
PMW3610 sensor(PIN_NCS);

// 累積移動量を追跡
long totalX = 0;
long totalY = 0;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ; // シリアルポートが開くのを待つ

    Serial.println("PMW3610 Advanced Example");
    Serial.println("=======================");

    // センサーの初期化
    if (sensor.begin())
    {
        Serial.println("Sensor initialization successful.");
        
        // センサー情報を表示
        uint8_t productId = sensor.readReg(0x00);
        uint8_t revisionId = sensor.readReg(0x01);
        
        Serial.print("Product ID: 0x");
        Serial.println(productId, HEX);
        Serial.print("Revision ID: 0x");
        Serial.println(revisionId, HEX);
        
        // 高解像度CPI設定
        sensor.setCpi(3200);
        Serial.print("CPI set to: ");
        Serial.println(sensor.getCpi());
        
        Serial.println("\nOutput format:");
        Serial.println("Motion | dx | dy | Total X | Total Y | Quality | Shutter | MaxPixel");
        Serial.println("--------------------------------------------------------------------");
    }
    else
    {
        Serial.println("Sensor initialization failed!");
        Serial.println("Please check:");
        Serial.println("- Wiring connections");
        Serial.println("- Power supply (3.3V)");
        Serial.println("- Logic level shifter (if using 5V Arduino)");
        while (1)
            ; // 失敗した場合はここで停止
    }
}

void loop()
{
    // 方法1: 戻り値版を使用（推奨）
    PMW3610::PMW3610_data data = sensor.readMotion();
    
    if (data.isMotion)
    {
        // 累積移動量を更新
        totalX += data.dx;
        totalY += data.dy;
        
        // 詳細情報を表示
        Serial.print("  YES  | ");
        Serial.print(data.dx);
        Serial.print(" | ");
        Serial.print(data.dy);
        Serial.print(" | ");
        Serial.print(totalX);
        Serial.print(" | ");
        Serial.print(totalY);
        Serial.print(" | ");
        Serial.print(data.squal);
        Serial.print(" | ");
        Serial.print((data.shutterUpper << 8) | data.shutterLower);
        Serial.print(" | ");
        Serial.println(data.maxPixel);
    }
    
    // 従来版の使用例は削除（APIが変更されたため）

    delay(5); // 高速ポーリング
}
