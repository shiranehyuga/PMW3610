// PMW3610 3線SPI ベタ書きテストコード

const int PIN_SDIO = 19; // SPIデータ双方向ピン
const int PIN_SCLK = 18; // SPIクロックピン
const int PIN_NCS = 17;  // チップセレクト

// PMW3610レジスタ定義
#define REG_Product_ID 0x00
#define REG_Revision_ID 0x01
#define REG_Motion 0x02
#define REG_Delta_X_L 0x03
#define REG_Delta_Y_L 0x04
#define REG_Delta_XY_H 0x05
#define REG_SQUAL 0x06
#define REG_Power_Up_Reset 0x3A
#define REG_Res_Step 0x85

void setup()
{
    Serial.begin(115200);
    delay(2000);

    Serial.println("PMW3610 3-Wire SPI Test (BitBang)");

    // ピン初期化
    pinMode(PIN_NCS, OUTPUT);
    pinMode(PIN_SCLK, OUTPUT);
    // PIN_SDIOは動的に切り替え

    digitalWrite(PIN_NCS, HIGH);
    digitalWrite(PIN_SCLK, LOW);

    delay(100);

    // センサー初期化
    Serial.println("\nInitializing PMW3610...");

    // ソフトウェアリセット
    writeRegBitBang(REG_Power_Up_Reset, 0x5A);
    delay(50);

    // Product IDを読み取り
    uint8_t productId = readRegBitBang(REG_Product_ID);
    uint8_t revisionId = readRegBitBang(REG_Revision_ID);

    Serial.print("Product ID: 0x");
    Serial.println(productId, HEX);
    Serial.print("Revision ID: 0x");
    Serial.println(revisionId, HEX);

    // PMW3610の期待されるProduct ID (通常0x42)
    if (productId == 0x42 || (productId != 0x00 && productId != 0xFF))
    {
        Serial.println("✓ PMW3610 detected!");

        // 初期設定
        writeRegBitBang(0x3B, 0x00); // Power-down register
        readRegBitBang(REG_Motion);  // モーションレジスタをクリア

        // CPI設定 (1000 CPI = 0x07)
        setCpiBitBang(1000);

        Serial.println("✓ Initialization complete!");
    }
    else
    {
        Serial.println("✗ PMW3610 not detected!");
        Serial.println("Check wiring and power supply");
        while (1)
            ;
    }
}

void loop()
{
    // モーションデータを読み取り
    readMotionBitBang();

    delay(5);
}

// 3線SPI書き込み関数
void writeRegBitBang(uint8_t reg, uint8_t value)
{
    digitalWrite(PIN_NCS, LOW);
    delayMicroseconds(2);

    // SDIO を出力モードに設定
    pinMode(PIN_SDIO, OUTPUT);

    // レジスタアドレス送信（書き込みモード: MSB=1）
    shiftOut(PIN_SDIO, PIN_SCLK, MSBFIRST, reg | 0x80);

    // データ送信
    shiftOut(PIN_SDIO, PIN_SCLK, MSBFIRST, value);

    digitalWrite(PIN_NCS, HIGH);
    delayMicroseconds(50);

    Serial.print("Write Reg 0x");
    Serial.print(reg, HEX);
    Serial.print(" = 0x");
    Serial.println(value, HEX);
}

// 3線SPI読み取り関数
uint8_t readRegBitBang(uint8_t reg)
{
    digitalWrite(PIN_NCS, LOW);
    delayMicroseconds(2);

    // 送信フェーズ: SDIO を出力モードに設定
    pinMode(PIN_SDIO, OUTPUT);

    // レジスタアドレス送信（読み取りモード: MSB=0）
    shiftOut(PIN_SDIO, PIN_SCLK, MSBFIRST, reg & 0x7F);

    // PMW3610応答待機時間
    delayMicroseconds(35);

    // 受信フェーズ: SDIO を入力モードに切り替え
    pinMode(PIN_SDIO, INPUT);
    delayMicroseconds(5);

    // データ受信
    uint8_t result = shiftIn(PIN_SDIO, PIN_SCLK, MSBFIRST);

    digitalWrite(PIN_NCS, HIGH);
    delayMicroseconds(20);

    // Serial.print("Read Reg 0x");
    // Serial.print(reg, HEX);
    // Serial.print(" = 0x");
    // Serial.println(result, HEX);

    return result;
}

// CPI設定関数
void setCpiBitBang(uint16_t cpi)
{
    uint8_t cpiValue;

    // CPI値をレジスタ値に変換
    if (cpi <= 200)
        cpiValue = 0x00;
    else if (cpi <= 400)
        cpiValue = 0x01;
    else if (cpi <= 600)
        cpiValue = 0x02;
    else if (cpi <= 800)
        cpiValue = 0x03;
    else if (cpi <= 1000)
        cpiValue = 0x04;
    else if (cpi <= 1200)
        cpiValue = 0x05;
    else if (cpi <= 1600)
        cpiValue = 0x06;
    else
        cpiValue = 0x07; // 3200 CPI

    writeRegBitBang(REG_Res_Step, cpiValue);

    Serial.print("CPI set to approximately ");
    Serial.println(cpi);
}

// モーション読み取り関数
void readMotionBitBang()
{
    // モーションレジスタを読み取り
    uint8_t motion = readRegBitBang(REG_Motion);

    if (motion & 0x80) // MOT bit (motion detected)
    {
        // デルタ値を読み取り
        uint8_t deltaXL = readRegBitBang(REG_Delta_X_L);    // 0x03
        uint8_t deltaYL = readRegBitBang(REG_Delta_Y_L);    // 0x04
        uint8_t deltaXY_H = readRegBitBang(REG_Delta_XY_H); // 0x05
        uint8_t squal = readRegBitBang(REG_SQUAL);          // 0x06

        int16_t deltaX_12bit = (((deltaXY_H >> 4) & 0x0F) << 8) | deltaXL;
        int16_t deltaY_12bit = ((deltaXY_H & 0x0F) << 8) | deltaYL;

        // 12ビット符号付きを16ビット符号付きに変換（符号拡張）
        if (deltaX_12bit & 0x800)
            deltaX_12bit |= 0xF000;
        if (deltaY_12bit & 0x800)
            deltaY_12bit |= 0xF000;

        // Serial.println("=== Motion Detected ===");
        // Serial.print("Delta X: ");
        // Serial.println(deltaX);
        // Serial.print("Delta Y: ");
        // Serial.println(deltaY);
        // Serial.print("Quality: ");
        // Serial.println(squal);
        // Serial.print("Motion Reg: 0x");
        // Serial.println(motion, HEX);
        // Serial.println("=====================");
        Serial.print(deltaX_12bit);
        Serial.print(",");
        Serial.print(deltaY_12bit);
        Serial.print(",");
        Serial.println(squal);
    }
    // else
    // {
    //     // モーションなし（デバッグ用）
    //     static unsigned long lastNoMotion = 0;
    //     if (millis() - lastNoMotion > 5000) // 5秒ごと
    //     {
    //         Serial.println("No motion detected...");
    //         lastNoMotion = millis();
    //     }
    // }
}
