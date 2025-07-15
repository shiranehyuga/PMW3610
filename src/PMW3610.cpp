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

PMW3610::PMW3610(uint8_t ncsPin, uint8_t sclkPin, uint8_t sdioPin) {
    _ncsPin = ncsPin;
    _sclkPin = sclkPin;
    _sdioPin = sdioPin;
    _currentCpi = 1000;
}

bool PMW3610::begin() {
    // ピン初期化
    pinMode(_ncsPin, OUTPUT);
    pinMode(_sclkPin, OUTPUT);
    // _sdioPin は動的に切り替え

    digitalWrite(_ncsPin, HIGH);
    digitalWrite(_sclkPin, LOW);

    delay(100);

    // ソフトウェアリセット
    writeRegBitBang(REG_Power_Up_Reset, 0x5A);
    delay(50);

    // Product IDを読み取り
    uint8_t productId = readRegBitBang(REG_Product_ID);
    uint8_t revisionId = readRegBitBang(REG_Revision_ID);

    // PMW3610の期待されるProduct ID (通常0x42)
    if (productId == 0x42 || (productId != 0x00 && productId != 0xFF)) {
        // 初期設定
        writeRegBitBang(0x3B, 0x00);  // Power-down register
        readRegBitBang(REG_Motion);   // モーションレジスタをクリア

        // デフォルトCPI設定
        setCpiBitBang(_currentCpi);

        return true;
    }

    return false;
}

PMW3610::PMW3610_data PMW3610::readMotion() {
    return readMotionBitBang();
}

void PMW3610::setCpi(uint16_t cpi) {
    setCpiBitBang(cpi);
    _currentCpi = cpi;
}

uint16_t PMW3610::getCpi() {
    return _currentCpi;
}

void PMW3610::writeReg(uint8_t reg, uint8_t value) {
    writeRegBitBang(reg, value);
}

uint8_t PMW3610::readReg(uint8_t reg) {
    return readRegBitBang(reg);
}

// 3線SPI書き込み関数
void PMW3610::writeRegBitBang(uint8_t reg, uint8_t value) {
    digitalWrite(_ncsPin, LOW);
    delayMicroseconds(2);

    // SDIO を出力モードに設定
    pinMode(_sdioPin, OUTPUT);

    // レジスタアドレス送信（書き込みモード: MSB=1）
    shiftOut(_sdioPin, _sclkPin, MSBFIRST, reg | 0x80);

    // データ送信
    shiftOut(_sdioPin, _sclkPin, MSBFIRST, value);

    digitalWrite(_ncsPin, HIGH);
    delayMicroseconds(50);
}

// 3線SPI読み取り関数
uint8_t PMW3610::readRegBitBang(uint8_t reg) {
    digitalWrite(_ncsPin, LOW);
    delayMicroseconds(2);

    // 送信フェーズ: SDIO を出力モードに設定
    pinMode(_sdioPin, OUTPUT);

    // レジスタアドレス送信（読み取りモード: MSB=0）
    shiftOut(_sdioPin, _sclkPin, MSBFIRST, reg & 0x7F);

    // PMW3610応答待機時間
    delayMicroseconds(35);

    // 受信フェーズ: SDIO を入力モードに切り替え
    pinMode(_sdioPin, INPUT);
    delayMicroseconds(5);

    // データ受信
    uint8_t result = shiftIn(_sdioPin, _sclkPin, MSBFIRST);

    digitalWrite(_ncsPin, HIGH);
    delayMicroseconds(20);

    return result;
}

// CPI設定関数
void PMW3610::setCpiBitBang(uint16_t cpi) {
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
        cpiValue = 0x07;  // 3200 CPI

    writeRegBitBang(REG_Resolution, cpiValue);
}

// モーション読み取り関数
PMW3610::PMW3610_data PMW3610::readMotionBitBang() {
    PMW3610_data data = {false, 0, 0, 0, 0, 0, 0};

    // モーションレジスタを読み取り
    uint8_t motion = readRegBitBang(REG_Motion);

    if (motion & 0x80) { // MOT bit (motion detected)
        data.isMotion = true;

        // デルタ値を読み取り
        uint8_t deltaXL = readRegBitBang(REG_Delta_X_L);
        uint8_t deltaXH = readRegBitBang(REG_Delta_X_H);
        uint8_t deltaYL = readRegBitBang(REG_Delta_Y_L);
        uint8_t deltaYH = readRegBitBang(REG_Delta_Y_H);
        data.squal = readRegBitBang(REG_SQUAL);

        // 16ビット符号付き値に変換
        data.dx = (int16_t)((deltaXH << 8) | deltaXL);
        data.dy = (int16_t)((deltaYH << 8) | deltaYL);
    }

    return data;
}