#include <SPI.h>
#include "PMW3610.h"

const int PIN_NCS = 10;
PMW3610 sensor(PIN_NCS);

void setup()
{
    Serial.begin(115200);
    while (!Serial);
    
    Serial.println("PMW3610 Simple Example");
    
    if (sensor.begin()) {
        Serial.println("Sensor ready!");
        sensor.setCpi(1000); // 標準解像度
    } else {
        Serial.println("Failed to initialize sensor!");
        while (1);
    }
}

void loop()
{
    // 最もシンプルな使用方法
    if (sensor.readMotion().isMotion) {
        PMW3610::PMW3610_data data = sensor.readMotion();
        Serial.print("Move: X=");
        Serial.print(data.dx);
        Serial.print(", Y=");
        Serial.println(data.dy);
    }
    
    delay(20);
}
