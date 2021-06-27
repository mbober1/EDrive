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
    int getValue();
    int getVoltage();
    int getSetpoint();
    int getPwmDuty();

private:
    int value, voltage, setpoint, pwmDuty;
};



const float multiplier = (100 * 60) / 224.4 / 4; 

inline int tickToRPM(int tick) {
    // * 100 -> tick / s
    // * 60 -> tick / min
    // / 224.4 -> revolution / min
    // / 4 (encoder)
    return tick * multiplier;
}


