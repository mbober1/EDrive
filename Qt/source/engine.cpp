#include "engine.hpp"


Engine::Engine() : value(0), voltage(0) {}

Engine::~Engine() {}

void Engine::setValue(const int value) { this->value = value; }

void Engine::setVoltage(const int voltage) { this->voltage = voltage; }

int Engine::getValue() { return this->value; }

int Engine::getVoltage() { return this->voltage; }