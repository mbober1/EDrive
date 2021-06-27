#pragma once

class Engine
{
public:
    Engine();
    ~Engine();

    void setValue(const int value);
    void setVoltage(const int voltage);
    void setSetpoint(const int setpoint);
    void setPwmDuty(const int duty);
    void setTorque(const float torque);
    int getValue();
    int getVoltage();
    int getSetpoint();
    int getPwmDuty();
    float getTorque();

private:
    int value, voltage, setpoint, pwmDuty;
    float torque;
};



const float RPM_Multiplier = (100 * 60) / 224.4 / 4; 

inline int tickToRPM(int tick) {
    // * 100 -> tick / s
    // * 60 -> tick / min
    // / 224.4 -> revolution / min
    // / 4 (encoder)
    return tick * RPM_Multiplier;
}


const float Torque_Multiplier = 1 / (800/3.6); 

inline float mAToKgCm(int current) {
    return current * Torque_Multiplier;
}