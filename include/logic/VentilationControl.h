#ifndef VENTILATION_CONTROL_H
#define VENTILATION_CONTROL_H

#include <Arduino.h>

/**
 * VentilationControl - Control de ventilación automático para invernadero
 * Implementa algoritmos de intercambio de aire y control de microclima
 * Basado en sistemas de ventilación para agricultura controlada
 */
class VentilationControl {
private:
    // Targets and current values
    float targetAirExchangeRate; // renovaciones por hora
    float currentAirQuality; // CO2 equivalent or composite index
    
    // Multi-parameter control
    float temperatureThreshold;
    float humidityThreshold;
    float co2Threshold;
    float targetTemperature;
    float targetHumidity;
    
    // Ventilation levels
    enum VentilationLevel {
        VENTILATION_OFF = 0,
        VENTILATION_LOW = 25,
        VENTILATION_MEDIUM = 50,
        VENTILATION_HIGH = 75,
        VENTILATION_MAX = 100
    };
    
    VentilationLevel currentLevel;
    VentilationLevel previousLevel;
    
    // Control modes
    enum VentilationMode {
        MODE_MANUAL,
        MODE_TEMPERATURE,
        MODE_HUMIDITY,
        MODE_AIR_QUALITY,
        MODE_COMPREHENSIVE,
        MODE_EMERGENCY
    };
    
    VentilationMode currentMode;
    
    // Timing and scheduling
    unsigned long lastUpdate;
    unsigned long lastLevelChange;
    unsigned long minRunTime; // Tiempo mínimo de funcionamiento
    unsigned long maxRunTime; // Tiempo máximo continuo
    unsigned long cooldownTime; // Tiempo de descanso entre ciclos
    
    // Environmental conditions
    float outsideTemperature;
    float outsideHumidity;
    bool isWindy;
    bool isRaining;
    bool isDaytime;
    
    // Fan and servo control
    bool fanEnabled;
    bool servoEnabled;
    uint8_t fanSpeed; // 0-100%
    uint8_t servoPosition; // 0-180 degrees
    bool damperOpen;
    
    // Control parameters
    float temperatureHysteresis;
    float humidityHysteresis;
    float co2Hysteresis;
    
    // Smart features
    bool adaptiveControl;
    bool predictiveMode;
    bool energySavingMode;
    
    // Statistics
    unsigned long totalRunTime;
    unsigned long dailyRunTime;
    unsigned int cycleCount;
    unsigned int dailyCycleCount;
    float energyConsumption;
    
    // Emergency conditions
    bool emergencyActive;
    float emergencyTemperatureThreshold;
    float emergencyHumidityThreshold;
    float emergencyCO2Threshold;

public:
    // bool isEnabled() const; // Eliminada duplicidad
    VentilationControl();
    
    bool begin();
    void update(float temperature, float humidity, float co2Level = -1, float lightLevel = -1);
    
    // Configuration
    void setTemperatureThresholds(float target, float threshold, float hysteresis = 1.0);
    void setHumidityThresholds(float target, float threshold, float hysteresis = 5.0);
    void setCO2Threshold(float threshold, float hysteresis = 100.0);
    void setAirExchangeRate(float rate);
    void setTimingParameters(unsigned long minRun, unsigned long maxRun, unsigned long cooldown);
    void setEmergencyThresholds(float tempThresh, float humThresh, float co2Thresh);
    
    // Control modes
    void setMode(VentilationMode mode);
    VentilationMode getMode() const;
    void setManualLevel(VentilationLevel level);
    void enableAdaptiveControl(bool enable);
    void enablePredictiveMode(bool enable);
    void enableEnergySaving(bool enable);
    
    // Environmental input
    void setOutsideConditions(float temperature, float humidity, bool windy = false, bool raining = false);
    void setDaytimeStatus(bool isDay);
    
    // Control
    void enable();
    void disable();
    bool isEnabled() const;
    void emergencyVentilation();
    void stopEmergencyVentilation();
    
    // Status
    VentilationLevel getCurrentLevel() const;
    uint8_t getFanSpeed() const;
    uint8_t getServoPosition() const;
    bool isDamperOpen() const;
    bool isEmergencyActive() const;
    bool needsVentilation() const;
    float getCurrentAirQuality() const;
    
    // Ventilation calculations
    float calculateOptimalLevel(float temp, float humidity, float co2) const;
    float getEffectiveAirExchange() const;
    unsigned long getEstimatedRunTime() const;
    
    // Statistics
    unsigned long getTotalRunTime() const;
    unsigned long getDailyRunTime() const;
    unsigned int getCycleCount() const;
    unsigned int getDailyCycleCount() const;
    float getEnergyConsumption() const;
    void resetDailyStatistics();
    void resetStatistics();
    
    // Status strings
    String getStatusString() const;
    String getModeString() const;
    String getLevelString() const;
    
    // Advanced features
    void adaptToWeather(float outsideTemp, float outsideHum, bool windy, bool raining);
    void optimizeForPlantStage(uint8_t stage); // 1=seedling, 2=vegetative, 3=flowering, 4=fruiting
    bool predictVentilationNeed(unsigned int minutesAhead) const;
    void scheduleVentilation(uint8_t startHour, uint8_t startMinute, uint8_t endHour, uint8_t endMinute, VentilationLevel level);
    
    // Emergency checks
    bool checkEmergency() const;
    bool checkAirStagnation() const;
    bool checkExcessiveHumidity() const;
    bool checkExcessiveTemperature() const;
    
private:
    VentilationLevel calculateRequiredLevel() const;
    void updateFanControl();
    void updateServoControl();
    bool isLevelChangeAllowed() const;
    float calculateCompositeAirQuality(float temp, float humidity, float co2) const;
    void updateEnergyConsumption();
    VentilationLevel adaptLevelToConditions(VentilationLevel baseLevel) const;
};

#endif
