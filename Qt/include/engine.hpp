#pragma once

class Engine
{
public:
    Engine();
    ~Engine();

    void setValue(const int value);
    void setVoltage(const int voltage);
    int getValue();
    int getVoltage();

private:
    int value, voltage;
};



