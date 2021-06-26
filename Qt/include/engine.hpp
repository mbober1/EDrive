#pragma once

class Engine
{
public:
    Engine();
    ~Engine();

    void setValue(const int value);
    void setVoltage(const int voltage);
    void setSetpoint(const int setpoint);
    int getValue();
    int getVoltage();
    int getSetpoint();

private:
    int value, voltage, setpoint;
};



