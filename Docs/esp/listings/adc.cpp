#include "adc.hpp"

/**
 * Contructs ADC object and set it up.
 */
myADC::myADC() {
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_2_5);

    for (int i = 0; i < dataSize; i++) nextMeasurement();
}

/**
 * Adds another measurement to the buffer
 */
void myADC::nextMeasurement() {
    static int idx;
    this->data[idx++] = adc1_get_raw(ADC1_CHANNEL_0);
    if(idx >= dataSize) idx = 0;
}

/**
 * Calculates the average of the measurements in the buffer 
 * and convert the result into SI units.
 * @return Voltage in V
 */
float myADC::getVoltage() {
    uint32_t adc_reading = 0;
    for (size_t i = 0; i < dataSize; i++) 
    {
        adc_reading += this->data[i];
    }
    return ((float)((adc_reading/dataSize) + this->conversion_offset) / this->conversion_factor);
}