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



