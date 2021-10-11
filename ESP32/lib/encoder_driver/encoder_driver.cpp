#include "encoder_driver.hpp"



/**
 * @brief Encoder class constructor.
 * @param encoderA encoder A channel GPIO pin.
 * @param encoderB encoder B channel GPIO pin.
 * @param pcnt_unit PCNT unit.
 */
Encoder::Encoder(gpio_num_t encoderA, gpio_num_t encoderB, pcnt_unit_t pcnt_unit) :
    pcnt_unit(pcnt_unit)
{
    pcnt_config_t pcnt_config = {};
    pcnt_config.pulse_gpio_num = encoderA;
    pcnt_config.ctrl_gpio_num = encoderB;
    pcnt_config.channel = PCNT_CHANNEL_0;
    pcnt_config.unit = pcnt_unit;
    pcnt_config.pos_mode = PCNT_COUNT_DEC;
    pcnt_config.neg_mode = PCNT_COUNT_INC;
    pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_config.counter_h_lim = PCNT_COUNT_LIMIT;
    pcnt_config.counter_l_lim = -PCNT_COUNT_LIMIT;
    
    pcnt_config_t pcnt_config2 = {};
    pcnt_config2.pulse_gpio_num = encoderB;
    pcnt_config2.ctrl_gpio_num = encoderA;
    pcnt_config2.channel = PCNT_CHANNEL_1;
    pcnt_config2.unit = pcnt_unit;
    pcnt_config2.pos_mode = PCNT_COUNT_INC;
    pcnt_config2.neg_mode = PCNT_COUNT_DEC;
    pcnt_config2.lctrl_mode = PCNT_MODE_REVERSE;
    pcnt_config2.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_config2.counter_h_lim = PCNT_COUNT_LIMIT;
    pcnt_config2.counter_l_lim = -PCNT_COUNT_LIMIT;

    ESP_ERROR_CHECK(pcnt_unit_config(&pcnt_config));
    ESP_ERROR_CHECK(pcnt_unit_config(&pcnt_config2));
    
    ESP_ERROR_CHECK(pcnt_counter_pause(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_counter_clear(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_filter_disable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_intr_disable(pcnt_unit));
    ESP_ERROR_CHECK(pcnt_counter_resume(pcnt_unit));

    ESP_LOGI(ENCODER_TAG, "Encoder %d initialized", pcnt_unit);
}