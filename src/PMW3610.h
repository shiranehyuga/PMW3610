#ifndef PMW3610_H
#define PMW3610_H

#include <Arduino.h>

// PMW3610レジスタ定義（データシートベース）
#define REG_Product_ID 0x00
#define REG_Revision_ID 0x01
#define REG_Motion 0x02
#define REG_Delta_X_L 0x03
#define REG_Delta_Y_L 0x04
#define REG_Delta_XY_H 0x05
#define REG_SQUAL 0x06
#define REG_Shutter_Higher 0x07
#define REG_Shutter_Lower 0x08
#define REG_Pix_Max 0x09
#define REG_Pix_Avg 0x0A
#define REG_Pix_Min 0x0B
#define REG_CRC0 0x0C
#define REG_CRC1 0x0D
#define REG_CRC2 0x0E
#define REG_CRC3 0x0F
#define REG_Self_Test 0x10
#define REG_Performance 0x11
#define REG_Burst_Read 0x12
// 0x13-0x1a: RESERVED
#define REG_Run_Downshift 0x1B
#define REG_Rest1_Rate 0x1C
#define REG_Rest1_Downshift 0x1D
#define REG_Rest2_Rate 0x1E
#define REG_Rest2_Downshift 0x1F
#define REG_Rest3_Rate 0x20
// 0x21-0x2c: RESERVED
#define REG_Observation1 0x2D
// 0x2e-0x31: RESERVED
#define REG_Dtest2_Pad 0x32
// 0x33-0x34: RESERVED
#define REG_Pixel_Grab 0x35
#define REG_Frame_Grab 0x36
// 0x37-0x39: RESERVED
#define REG_Power_Up_Reset 0x3A
#define REG_Shutdown 0x3B
// 0x3c-0x3d: RESERVED
#define REG_Not_Rev_ID 0x3E
#define REG_Not_Prod_ID 0x3F
// 0x40: RESERVED
#define REG_SPI_CLK_ON_REQ 0x41
// 0x42-0x46: RESERVED
#define REG_PRBS_Test_Ctl 0x47
// 0x48-0x7e: RESERVED
#define REG_SPI_Page0 0x7F
// 0x80-0x84: RESERVED
#define REG_Res_Step 0x85
// 0x86-0x9d: RESERVED
#define REG_Vosel_Ctl 0x9E
#define REG_LSR_Control 0x9F
// 0xa0-0xfe: RESERVED
#define REG_SPI_Page1 0xFF

// 後方互換性のため
#define REG_Delta_X_H REG_Delta_XY_H // 0x05の上位4bitがDelta_X上位
#define REG_Delta_Y_H REG_Delta_XY_H // 0x05の下位4bitがDelta_Y上位
#define REG_Maximum_Pixel REG_Pix_Max
#define REG_Shutter_Upper REG_Shutter_Higher
#define REG_Motion_Burst REG_Burst_Read

// モーションデータ構造体
struct PMW3610_data
{
    bool isMotion; // モーション検出フラグ
    int16_t dx;    // X軸移動量
    int16_t dy;    // Y軸移動量
    uint8_t squal; // Surface Quality
};

class PMW3610
{
public:
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