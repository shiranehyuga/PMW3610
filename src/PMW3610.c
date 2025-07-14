#include "PMW3610.h"

// レジスタ定義
#define REG_Product_ID 0x00
#define REG_Revision_ID 0x01
#define REG_Motion 0x02
#define REG_Delta_X_L 0x03
#define REG_Delta_X_H 0x04
#define REG_Delta_Y_L 0x05
#define REG_Delta_Y_H 0x06
#define REG_Power_Up_Reset 0x3A
#define REG_Resolution 0x47
#define REG_Motion_Burst 0x50

PMW3610::PMW3610(uint8_t ncsPin)
{
    _ncs = ncsPin;
    _mosi = MOSI; // Arduinoの標準MOSIピンを使用
}

bool PMW3610::begin()
{
    pinMode(_ncs, OUTPUT);
    digitalWrite(_ncs, HIGH);

    SPI.begin();

    // ソフトウェアリセット
    writeReg(REG_Power_Up_Reset, 0x5A);
    delay(50);

    // IDを読み取って通信を確認
    uint8_t productId = readReg(REG_Product_ID);
    uint8_t revisionId = readReg(REG_Revision_ID);

    // PMW3610の期待されるID (例: 0x42)
    if (productId != 0x42)
    {
        return false;
    }

    // 初期設定
    writeReg(0x3B, 0x00); // Power-down register
    readReg(REG_Motion);  // モーションレジスタをクリア
    setCpi(1000);         // デフォルトCPIを1000に設定

    return true;
}

void PMW3610::writeReg(uint8_t reg, uint8_t value)
{
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));

    digitalWrite(_ncs, LOW);
    delayMicroseconds(1); // NCS setup time

    SPI.transfer(reg | 0x80); // 書き込みモード（MSBを1に設定）
    SPI.transfer(value);

    digitalWrite(_ncs, HIGH);
    SPI.endTransaction();

    // 書き込み完了待機時間
    delayMicroseconds(50);
}

uint8_t PMW3610::readReg(uint8_t reg)
{
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));

    digitalWrite(_ncs, LOW);
    delayMicroseconds(1); // NCS setup time

    // レジスタアドレスを送信（読み取りモード）
    SPI.transfer(reg & 0x7F);

    // アドレス送信後の待機時間（PMW3610仕様）
    delayMicroseconds(35);

    // 3線SPIのため、MOSIを入力に切り替えてデータを読み取る
    pinMode(_mosi, INPUT);
    delayMicroseconds(1); // ピン切り替え安定化時間

    uint8_t value = SPI.transfer(0x00);

    pinMode(_mosi, OUTPUT);
    delayMicroseconds(1); // ピン切り替え安定化時間

    digitalWrite(_ncs, HIGH);
    SPI.endTransaction();

    // 次のアクセスまでの最小間隔
    delayMicroseconds(20);

    return value;
}

void PMW3610::setCpi(uint16_t cpi)
{
    if (cpi < 200)
        cpi = 200;
    if (cpi > 3200)
        cpi = 3200;
    // CPIは200の倍数で設定
    uint8_t val = (cpi / 200) - 1;
    writeReg(REG_Resolution, val);
}

// CPIを取得するメソッド
unsigned int PMW3610::getCpi()
{
    uint8_t val = readReg(REG_Resolution);
    return (val + 1) * 200;
}

// readMotion関数（戻り値版）
PMW3610::PMW3610_data PMW3610::readMotion()
{
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE3));
    digitalWrite(_ncs, LOW);
    SPI.transfer(REG_Motion_Burst & 0x7F);
    delayMicroseconds(35);

    // 3線SPIのため、MOSIを入力に切り替えてデータを読み取る
    pinMode(_mosi, INPUT);

    uint8_t burstData[12];
    for (int i = 0; i < 12; i++)
    {
        burstData[i] = SPI.transfer(0x00);
    }

    pinMode(_mosi, OUTPUT);
    digitalWrite(_ncs, HIGH);
    SPI.endTransaction();
    delayMicroseconds(1);

    _motionData.isMotion = (burstData[0] & 0x80) != 0;
    _motionData.dx = (int16_t)((burstData[2] << 8) | burstData[1]);
    _motionData.dy = (int16_t)((burstData[4] << 8) | burstData[3]);
    _motionData.squal = burstData[5];
    _motionData.shutterUpper = burstData[6];
    _motionData.shutterLower = burstData[7];
    _motionData.maxPixel = burstData[8];

    return _motionData;
}