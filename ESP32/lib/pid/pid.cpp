#include "pid.hpp"
#include "config.hpp"
#include "task_mngm.hpp"
#include "mqtt.hpp"


#define PID_LIMIT 100
#define INTEGRAL_LIMIT 50

#define NORMALIZE_I(integral_error) ({\
    if(integral_error > INTEGRAL_LIMIT) integral_error = INTEGRAL_LIMIT;\
    else if(integral_error < -INTEGRAL_LIMIT) integral_error = -INTEGRAL_LIMIT; })

#define NORMALIZE_PID(pid) ({\
    if(pid > PID_LIMIT) pid = PID_LIMIT;\
    else if(pid < -PID_LIMIT) pid = -PID_LIMIT; })


inline void GET_ENCODER_VALUE(pcnt_unit_t pcnt_unit, int16_t *input)
{
    pcnt_get_counter_value(pcnt_unit, input);
    pcnt_counter_clear(pcnt_unit);
    xQueueSendToBack(pulsesQueue, input, 0);
}

/**
 * @brief PID class constructor.
 * @param encoder Encoder object to use.
 */
Pid::Pid(const Encoder &encoder, const Pid_config_t *config) : pcnt_unit(encoder.pcnt_unit) 
{
    this->set_parameters(config);
}


/**
 * @brief Compute PID result.
 * @param setpoint Setpoint.
 */
int16_t Pid::compute(const int &setpoint) {
    int16_t input;
    GET_ENCODER_VALUE(PCNT_UNIT_0, &input);

    int epsilon = setpoint - input;

    // calculate integral error
    this->integral_error+= epsilon;
    NORMALIZE_I(this->integral_error);

    // calculate derivative error
    this->derivative_error = epsilon - last_epsilon;

    // save last epsilon
    this->last_epsilon = epsilon;

    // calculate PID
    int16_t p = this->config.kp * epsilon;
    int16_t i = this->config.ki * this->integral_error;
    int16_t d = this->config.kd * this->derivative_error;
    int16_t pid = p + i + d;
    NORMALIZE_PID(pid);

    return pid;
}


/**
 * @brief Setup PID parameters
 * @param config Pointer to config struct.
 */
void Pid::set_parameters(const Pid_config_t *config) 
{
    memcpy(&(this->config), config, sizeof(Pid_config_t));
}


/**
 * @brief Motor control task.
 */
void motorTask(void*)
{
    esp_log_level_set(MOTOR_TAG, ESP_LOG_VERBOSE);

    Pid_message_t mess;
    Pid_config_t config =                                        // create PID config object
    {
        .kp = DEFAULT_PID_KP, 
        .ki = DEFAULT_PID_KI, 
        .kd = DEFAULT_PID_KD, 
    };


    Motor engine(MOTOR_UNIT, MOTOR_IN1, MOTOR_IN2, MOTOR_TIMER); // init engine object
    Encoder encoder(ENC_A, ENC_B, ENC_PCNT_UNIT);                // init encoder object
    Pid pid(encoder, &config);                                   // init PID object

    TickType_t xLastWakeTime;
    int16_t pid_result;

    int setpoint = 0;

    while (true)
    {
        xLastWakeTime = xTaskGetTickCount();

        if (pdTRUE == xQueueReceive(pidQueue, &mess, 0L)) 
        {
            switch (mess.parameter)
            {
            case SETPOINT_PARAMETER:
                setpoint = mess.value;
                break;
            
            case KP_PARAMETER:
                config.kp = mess.value;
                break;
            
            case KI_PARAMETER:
                config.ki = mess.value;
                break;
            
            case KD_PARAMETER:
                config.kd = mess.value;
                break;
            
            default:
                break;
            }

            pid.set_parameters(&config);
        }

        pid_result = pid.compute(setpoint);
        engine.set_duty(pid_result);
        xQueueSendToBack(powerQueue, &pid_result, 0);

        vTaskDelayUntil(&xLastWakeTime, PID_LOOP_PERIOD);         // set PID loop to 10ms
    }

   vTaskDelete(NULL);
}