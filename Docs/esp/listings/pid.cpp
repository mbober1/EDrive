
/**
 * Compute PID.
 * @param setpoint Setpoint
 */
void IRAM_ATTR motor::compute(const int &setpoint) {
    int16_t input;
    pcnt_get_counter_value(this->encoder, &input);
    pcnt_counter_clear(this->encoder);

    int epsilon = setpoint - input;

    this->integralError+= epsilon;
    if(this->integralError > MAX_INTEGRAL) this->integralError = MAX_INTEGRAL;
    else if(this->integralError < -MAX_INTEGRAL) this->integralError = -MAX_INTEGRAL;
    this->derivativeError = epsilon - epsilonOld;
    this->epsilonOld = epsilon;
    

    int16_t p = this->kp * epsilon;
    int16_t i = this->ki * this->integralError;
    int16_t d = this->kd * this->derivativeError;

    int16_t pid = p + i + d;
    if(pid > MAX_PID) pid = MAX_PID;
    else if(pid < MIN_PID) pid = MIN_PID;

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

    xQueueSendToBack(pulsesQueue, &input, 0);
    xQueueSendToBack(powerQueue, &pid, 0);
}