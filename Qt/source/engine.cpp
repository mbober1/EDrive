#include "engine.hpp"



/**
 * A constructor
 */
Engine::Engine() : value(0), voltage(0), setpoint(0) {}


/**
 * A destructor
 */
Engine::~Engine() {}


/**
 * Set motor revolutions value
 * @param[in] value Value to set
 */
void Engine::setValue(const int value) { this->value = value; }


/**
 * Set motor voltage
 * @param[in] value Motor voltage
 */
void Engine::setVoltage(const int voltage) { this->voltage = voltage; }


/**
 * Set PID setpoint
 * @param[in] value Setpoint
 */
void Engine::setSetpoint(const int setpoint) { this->setpoint = setpoint; }


/**
 * Set PWM duty
 * @param[in] value PWM duty
 */
void Engine::setPwmDuty(const int duty) { this->pwmDuty = duty; }


/**
 * Set motor torque
 * @param[in] current Motor current
 */
void Engine::setTorque(const float torque) { this->torque = torque; }


/**
 * Get motor revolutions value
 * @return Value
 */
int Engine::getValue() { return this->value; }


/**
 * Get motor voltage
 * @return Motor voltage
 */
int Engine::getVoltage() { return this->voltage; }


/**
 * Get PID setpoint
 * @return Setpoint
 */
int Engine::getSetpoint() { return this->setpoint; }


/**
 * Get PWM duty
 * @return PWM duty
 */
int Engine::getPwmDuty() { return this->pwmDuty; }


/**
 * Get motor torque
 * @return Motor current
 */
float Engine::getTorque() { return this->torque; }
