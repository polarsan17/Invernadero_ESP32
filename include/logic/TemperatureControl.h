#ifndef TEMPERATURE_CONTROL_H
#define TEMPERATURE_CONTROL_H

#include <Arduino.h>

/**
 * TemperatureControl - Control de temperatura automático para invernadero
 * Implementa control PID para mantener temperatura objetivo
 * Basado en algoritmos de control ambiental para invernaderos
 */
class TemperatureControl {
private:
    float targetTemperature;
    float currentTemperature;
    float tolerance;
    
    // PID Controller variables
    float previousError;
    float integral;
    float derivative;
    float kp, ki, kd; // PID constants
    
    // Control limits
    float minTemp, maxTemp;
    float maxIntegral;
    
    // Timing
    unsigned long lastUpdate;
    float deltaTime;
    
    // Status
    bool enabled;
    bool heatingActive;
    bool coolingActive;
    
    // Statistics
    unsigned long totalActiveTime;
    unsigned long lastActiveTime;
    unsigned int adjustmentCount;

public:
    TemperatureControl();
    
    bool begin();
    void update(float currentTemp);
    
    // Configuration
    void setTarget(float target);
    float getTarget() const;
    void setTolerance(float tol);
    void setPIDConstants(float p, float i, float d);
    void setTemperatureLimits(float min, float max);
    
    // Control
    void enable();
    void disable();
    bool isEnabled() const;
    
    // Status
    float getCurrentTemperature() const;
    float getError() const;
    bool isHeatingActive() const;
    bool isCoolingActive() const;
    bool isInRange() const;
    
    // PID output
    float getPIDOutput();
    
    // Statistics
    unsigned long getTotalActiveTime() const;
    unsigned int getAdjustmentCount() const;
    void resetStatistics();
    
    // Status string
    String getStatusString() const;
    
    // Emergency check
    bool checkEmergency() const;
};

#endif
