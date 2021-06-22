#include "driver/adc.h"
#define dataSize 50

class myADC {
    const int conversion_factor = 415;
    uint16_t data[dataSize] = {};

public:
    myADC();
    void nextMeasurement();
    float getVoltage();
};