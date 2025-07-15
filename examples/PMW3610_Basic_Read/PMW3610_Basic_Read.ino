#include "PMW3610.h"

// 3線SPI用ピン定義
const int PIN_NCS = 17;   // チップセレクト
const int PIN_SCLK = 18;  // SPIクロック
const int PIN_SDIO = 19;  // SPIデータ（双方向）

// PMW3610センサーインスタンス作成
PMW3610 sensor(PIN_NCS, PIN_SCLK, PIN_SDIO);

void setup() {
    Serial.begin(115200);
    delay(2000);

    Serial.println("PMW3610 Basic Read Example");
    Serial.println("===========================");

    // ピン設定を表示
    Serial.println("Pin Configuration:");
    Serial.print("  NCS:  GPIO");
    Serial.println(PIN_NCS);
    Serial.print("  SCLK: GPIO");
    Serial.println(PIN_SCLK);
    Serial.print("  SDIO: GPIO");
    Serial.println(PIN_SDIO);

    // センサー初期化
    Serial.println("\nInitializing PMW3610...");
    
    if (sensor.begin()) {
        Serial.println("✓ PMW3610 initialization successful!");
        
        // Product IDとRevision IDを表示
        uint8_t productId = sensor.readReg(REG_Product_ID);
        uint8_t revisionId = sensor.readReg(REG_Revision_ID);
        
        Serial.print("Product ID: 0x");
        Serial.println(productId, HEX);
        Serial.print("Revision ID: 0x");
        Serial.println(revisionId, HEX);
        
        // CPI設定
        sensor.setCpi(1000);
        Serial.print("CPI set to: ");
        Serial.println(sensor.getCpi());
        
        Serial.println("\nStarting motion detection...");
        Serial.println("Move the sensor to see motion data:");
        Serial.println("Format: dx,dy");
    } else {
        Serial.println("✗ PMW3610 initialization failed!");
        Serial.println("Check wiring and power supply");
        while (1) {
            delay(1000);
        }
    }
}

void loop() {
    // モーションデータを読み取り
    PMW3610::PMW3610_data motion = sensor.readMotion();
    
    if (motion.isMotion) {
        // CSV形式で出力（シリアルプロッターに対応）
        Serial.print(motion.dx);
        Serial.print(",");
        Serial.println(motion.dy);
        
        // 詳細情報も表示したい場合（コメントアウトを外す）
        /*
        Serial.println("=== Motion Detected ===");
        Serial.print("Delta X: ");
        Serial.println(motion.dx);
        Serial.print("Delta Y: ");
        Serial.println(motion.dy);
        Serial.print("Surface Quality: ");
        Serial.println(motion.squal);
        Serial.println("=====================");
        */
    }
    
    delay(5);  // 高速サンプリング
}