#include "driver/adc.h"

class myADC {
    const int dataSize = 10;
    const uint16_t R1 = 8112;
    const uint16_t R2 = 906;
    // const float conversion_factor = (this->R1 + this->R2) * 1.317 / ((1 << 12) * this->R2);
    const float conversion_factor = 0.0017;
    uint16_t data[10] = {};

public:
    myADC();
    void nextMeasurement();
    float getVoltage();
};