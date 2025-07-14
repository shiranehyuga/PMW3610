#ifndef PMW3610_H
#define PMW3610_H

#include <Arduino.h>
#include <SPI.h>

class PMW3610
{
public:
    // モーションデータを格納する構造体
    struct MotionData
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
    void readMotion(MotionData *data);
    void writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    void setCpi(uint16_t cpi);

private:
    uint8_t _ncs;
    uint8_t _mosi; // ArduinoのMOSIピン番号を保持
};

#endif