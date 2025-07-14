#ifndef PMW3610_H
#define PMW3610_H

#include <Arduino.h>
#include <SPI.h>

class PMW3610
{
public:
    // モーションデータを格納する構造体
    struct PMW3610_data
    {
        bool isMotion;
        int16_t dx;
        int16_t dy;
        uint8_t squal;
        uint8_t shutterUpper;
        uint8_t shutterLower;
        uint8_t maxPixel;
    };

    PMW3610(uint8_t ncsPin);
    bool begin();
    void setCpi(uint16_t cpi);
    unsigned int getCpi(); // CPIを取得するメソッド
    PMW3610_data readMotion();
    void writeReg(uint8_t reg, uint8_t value);
    uint8_t readReg(uint8_t reg);

private:
    uint8_t _ncs;
    uint8_t _mosi;            // ArduinoのMOSIピン番号を保持
    PMW3610_data _motionData; // 内部で使用するMotionDataインスタンス
};

#endif