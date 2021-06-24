#include "motor.hpp"

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

/**
 * Constructor.
 * @param in1 input1 GPIO pin.
 * @param in2 input2 GPIO pin.
 * @param encoderA encoder first GPIO pin.
 * @param encoderB encoder second GPIO pin.
 * @param pwmPin enable GPIO pin.
 * @param pwmChannel PWM channel.
 * @param pcntUnit PCNT unit.
 */
motor::motor(gpio_num_t in1, gpio_num_t in2, gpio_num_t encoderA, gpio_num_t encoderB, gpio_num_t enPin, ledc_channel_t pwmChannel, pcnt_unit_t pcntUnit) : in1(in1), in2(in2), enPin(enPin) {
    esp_err_t err = ESP_OK;

    err += mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0A, in2);
    err += mcpwm_gpio_init(MCPWM_UNIT_0, MCPWM0B, in1);

    gpio_config_t en_conf = {};
    en_conf.mode = GPIO_MODE_OUTPUT;
    en_conf.pin_bit_mask = (1ULL<<enPin);
    err += gpio_config(&en_conf);
    err += gpio_set_level(this->enPin, 1);

    mcpwm_config_t pwm_config;
    pwm_config.frequency = 25000;
    pwm_config.cmpr_a = 0;
    pwm_config.cmpr_b = 0;
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    err += mcpwm_init(MCPWM_UNIT_0, MCPWM_TIMER_0, &pwm_config); 


    if(!err) printf("Motor %d PWM initialized, freq %d\n", pcntUnit, ledc_get_freq(LEDC_HIGH_SPEED_MODE, LEDC_TIMER_0));
    else printf("Motor %d PWM failed with error: %d\n", pcntUnit, err);

    err = ESP_OK;

    printf("Encoder %d Config: \n", pcntUnit);
    printf("- ENCA = %d\n", encoderA);
    printf("- ENCB = %d\n", encoderB);

    pcnt_config_t pcnt_config = {};
    pcnt_config.pulse_gpio_num = encoderA;
    pcnt_config.ctrl_gpio_num = encoderB;
    pcnt_config.channel = PCNT_CHANNEL_0;
    pcnt_config.unit = pcntUnit;
    pcnt_config.pos_mode = PCNT_COUNT_DEC;
    pcnt_config.neg_mode = PCNT_COUNT_INC;
    pcnt_config.lctrl_mode = PCNT_MODE_REVERSE;
    pcnt_config.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_config.counter_h_lim = 1000;
    pcnt_config.counter_l_lim = -1000;
    err += pcnt_unit_config(&pcnt_config);

    pcnt_config_t pcnt_config2 = {};
    pcnt_config2.pulse_gpio_num = encoderB;
    pcnt_config2.ctrl_gpio_num = encoderA;
    pcnt_config2.channel = PCNT_CHANNEL_1;
    pcnt_config2.unit = pcntUnit;
    pcnt_config2.pos_mode = PCNT_COUNT_INC;
    pcnt_config2.neg_mode = PCNT_COUNT_DEC;
    pcnt_config2.lctrl_mode = PCNT_MODE_REVERSE;
    pcnt_config2.hctrl_mode = PCNT_MODE_KEEP;
    pcnt_config2.counter_h_lim = 1000;
    pcnt_config2.counter_l_lim = -1000;
    err += pcnt_unit_config(&pcnt_config2);

    err += pcnt_counter_pause(pcntUnit);
    err += pcnt_counter_clear(pcntUnit);
    err += pcnt_filter_disable(pcntUnit);
    err += pcnt_intr_disable(pcntUnit);
    err +=  pcnt_counter_resume(pcntUnit);

    if(!err) printf("Encoder %d initialized\n", pcntUnit);
    else printf("Encoder %d failed with error: %d\n", pcntUnit, err);

    this->encoder = pcntUnit;
    this->integralError = 0;
    this->epsilonOld = 0;
    this->epsilonSuma = 0;
    this->derivativeError = 0;
}


/**
 * Compute PID.
 * @param setpoint Setpoint
 */
void IRAM_ATTR motor::compute(const int &setpoint) {
    float pow = 0; 
    int16_t input;
    pcnt_get_counter_value(this->encoder, &input);
    this->countedPulses += input;
    pcnt_counter_clear(this->encoder);



    int epsilon = setpoint - input;

    this->integralError+= epsilon;
    if(this->integralError > MAX_INTEGRAL) this->integralError = MAX_INTEGRAL;
    else if(this->integralError < -MAX_INTEGRAL) this->integralError = -MAX_INTEGRAL;
    this->derivativeError = epsilon - epsilonOld;
    this->epsilonOld = epsilon;


    int16_t p = this->kp*epsilon;
    int16_t i = this->ki*this->integralError;
    int16_t d = this->kd*this->derivativeError;


    float pid = p + i + d;

    // printf("PID: %4.4f, SETPOINT: %3d, INPUT: %3d, %d,%d,%d\n", pid, setpoint, input, this->kp, this->ki, this->kd);


    if(pid > 0) {
        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B);
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, pid);
        mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A, MCPWM_DUTY_MODE_0);

    } else if(pid < 0) {
        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A);
        mcpwm_set_duty(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, -pid);
        mcpwm_set_duty_type(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B, MCPWM_DUTY_MODE_0); 

    } else {
        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_A);
        mcpwm_set_signal_low(MCPWM_UNIT_0, MCPWM_TIMER_0, MCPWM_GEN_B);
    }

    // xQueueSendToBack(powerQueue, &pid, 0);
}

 
 void motor::setKP(const int kp) { this->kp = kp; }
 void motor::setKI(const int ki) { this->ki = ki; }
 void motor::setKD(const int kd) { this->kd = kd; }

 int16_t motor::getPulses() {
    int16_t tmp = this->countedPulses;
    this->countedPulses = 0;
    return tmp;
 }




