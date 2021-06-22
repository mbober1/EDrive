#include "adc.hpp"

myADC::myADC() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_6);

    for (int i = 0; i < dataSize; i++) nextMeasurement();
}

void myADC::nextMeasurement() {
    static int idx;
    this->data[idx++] = adc1_get_raw(ADC1_CHANNEL_0);
    if(idx >= dataSize) idx = 0;
}

float myADC::getVoltage() {
    uint32_t adc_reading = 0;
    for (size_t i = 0; i < dataSize; i++) adc_reading += this->data[i];
    return ((float)adc_reading / dataSize) / this->conversion_factor;
}