#ifndef PMW3610_H
#define PMW3610_H

#include <Arduino.h>

// PMW3610レジスタ定義
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

class PMW3610 {
public:
    // モーションデータ構造体
    struct PMW3610_data {
        bool isMotion;          // モーション検出フラグ
        int16_t dx;             // X軸移動量
        int16_t dy;             // Y軸移動量
        uint8_t squal;          // 表面品質
        uint8_t shutterUpper;   // シャッター上位バイト
        uint8_t shutterLower;   // シャッター下位バイト
        uint8_t maxPixel;       // 最大ピクセル値
    };

    // コンストラクタ（3線SPI BitBang用）
    PMW3610(uint8_t ncsPin, uint8_t sclkPin, uint8_t sdioPin);

    // 初期化
    bool begin();

    // モーション読み取り
    PMW3610_data readMotion();

    // CPI設定
    void setCpi(uint16_t cpi);
    uint16_t getCpi();

    // レジスタアクセス
    void writeReg(uint8_t reg, uint8_t value);
    uint8_t readReg(uint8_t reg);

private:
    uint8_t _ncsPin;
    uint8_t _sclkPin;
    uint8_t _sdioPin;
    uint16_t _currentCpi;

    // 3線SPI BitBang実装
    void writeRegBitBang(uint8_t reg, uint8_t value);
    uint8_t readRegBitBang(uint8_t reg);
    void setCpiBitBang(uint16_t cpi);
    PMW3610_data readMotionBitBang();
};

#endif // PMW3610_H