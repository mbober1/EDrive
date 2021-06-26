#include "engine.hpp"


Engine::Engine() : value(0), voltage(0), setpoint(0) {}

Engine::~Engine() {}

void Engine::setValue(const int value) { this->value = value; }

void Engine::setVoltage(const int voltage) { this->voltage = voltage; }

void Engine::setSetpoint(const int setpoint) { this->setpoint = setpoint; }

void Engine::setPwmDuty(const int duty) { this->pwmDuty = duty; }

int Engine::getValue() { return this->value; }

int Engine::getVoltage() { return this->voltage; }

int Engine::getSetpoint() { return this->setpoint; }

int Engine::getPwmDuty() { return this->pwmDuty; }

