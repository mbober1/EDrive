#include "motor_driver.hpp"

/**
 * @brief Motor class constructor.
 * @param mcpwm_unit MC_PWM unit.
 * @param in1 input1 GPIO pin.
 * @param in2 input2 GPIO pin.
 * @param timer_num PWM timer.
 */
Motor::Motor(mcpwm_unit_t mcpwm_unit, gpio_num_t in1, gpio_num_t in2, mcpwm_timer_t timer_num) :
    mcpwm_unit(mcpwm_unit),
    timer_num(timer_num)
{
    ESP_ERROR_CHECK(mcpwm_gpio_init(mcpwm_unit, MCPWM0A, in1));
    ESP_ERROR_CHECK(mcpwm_gpio_init(mcpwm_unit, MCPWM0B, in2));

    mcpwm_config_t mcpwm_config = {};
    mcpwm_config.frequency = MOTOR_PWM_FREQUENCY;
    mcpwm_config.counter_mode = MCPWM_UP_DOWN_COUNTER;
    mcpwm_config.duty_mode = MCPWM_DUTY_MODE_0;

    ESP_ERROR_CHECK(mcpwm_init(mcpwm_unit, timer_num, &mcpwm_config));
    ESP_LOGI(MOTOR_TAG, "Motor %d initialized", mcpwm_unit);
}


/**
 * @brief Set motor duty cycle.
 * @param duty_cycle - float - Duty cycle to set. Range 0-100.
 */
void Motor::set_duty(float duty_cycle)
{
    /* motor moves in forward direction, with duty cycle = duty % */
    if (duty_cycle > 0) 
    {
        mcpwm_set_signal_low(this->mcpwm_unit, this->timer_num, MCPWM_GEN_A);
        mcpwm_set_duty(this->mcpwm_unit, this->timer_num, MCPWM_GEN_B, duty_cycle);
        mcpwm_set_duty_type(this->mcpwm_unit, this->timer_num, MCPWM_GEN_B, MCPWM_DUTY_MODE_0);  
    }
    /* motor moves in backward direction, with duty cycle = -duty % */
    else 
    {
        mcpwm_set_signal_low(this->mcpwm_unit, this->timer_num, MCPWM_GEN_B);
        mcpwm_set_duty(this->mcpwm_unit, this->timer_num, MCPWM_GEN_A, -duty_cycle);
        mcpwm_set_duty_type(this->mcpwm_unit, this->timer_num, MCPWM_GEN_A, MCPWM_DUTY_MODE_0); 
    }
}