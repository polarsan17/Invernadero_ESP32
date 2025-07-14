#ifndef HUMIDITY_CONTROL_H
#define HUMIDITY_CONTROL_H

#include <Arduino.h>

/**
 * HumidityControl - Control de humedad automático para invernadero
 * Implementa control adaptativo para mantener humedad relativa objetivo
 * Basado en sistemas de control ambiental para agricultura de precisión
 */
class HumidityControl {
private:
    float targetHumidity;
    float currentHumidity;
    float tolerance;
    
    // Control variables
    float previousError;
    float errorSum;
    float kp, ki, kd; // Control constants
    
    // Humidity limits
    float minHumidity, maxHumidity;
    float criticalLowHumidity, criticalHighHumidity;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastAction;
    const unsigned long MIN_ACTION_INTERVAL = 30000; // 30 segundos mínimo entre acciones
    
    // Status
    bool enabled;
    bool humidifyingActive;
    bool dehumidifyingActive;
    bool ventilationActive;
    
    // Environmental factors
    float temperatureFactor; // Ajuste basado en temperatura
    bool isDaytime;
    
    // Statistics
    unsigned long totalActiveTime;
    unsigned int adjustmentCount;
    unsigned long lastActiveTime;

public:
    HumidityControl();
    
    bool begin();
    void update(float currentHum, float currentTemp = -1);
    
    // Configuration
    void setTarget(float target);
    float getTarget() const;
    void setTolerance(float tol);
    void setControlConstants(float p, float i, float d);
    void setHumidityLimits(float min, float max);
    void setCriticalLimits(float criticalLow, float criticalHigh);
    
    // Environmental context
    void setDaytimeStatus(bool isDay);
    void setTemperatureFactor(float tempFactor);
    
    // Control
    void enable();
    void disable();
    bool isEnabled() const;
    
    // Status
    float getCurrentHumidity() const;
    float getError() const;
    bool isHumidifyingActive() const;
    bool isDehumidifyingActive() const;
    bool isVentilationActive() const;
    bool isInRange() const;
    bool isCriticalCondition() const;
    
    // Control output
    float getControlOutput();
    int getVentilationLevel(); // 0-100%
    
    // Statistics
    unsigned long getTotalActiveTime() const;
    unsigned int getAdjustmentCount() const;
    void resetStatistics();
    
    // Status string
    String getStatusString() const;
    
    // Emergency check
    bool checkEmergency() const;
    
    // Advanced features
    void adaptToWeather(bool isRaining, float outsideHumidity = -1);
    float calculateIdealHumidity(float temperature) const;
};

#endif
