#ifndef IRRIGATION_CONTROL_H
#define IRRIGATION_CONTROL_H

#include <Arduino.h>

/**
 * IrrigationControl - Control de riego automático para invernadero
 * Implementa algoritmos de riego inteligente basado en humedad del suelo
 * Basado en sistemas de agricultura de precisión y riego por goteo
 */
class IrrigationControl {
private:
    float targetSoilMoisture; // % humedad objetivo
    float currentSoilMoisture;
    float tolerance;
    
    // Irrigation scheduling
    struct IrrigationSchedule {
        uint8_t hour;
        uint8_t minute;
        unsigned int durationSeconds;
        bool enabled;
        uint8_t weekDays; // Bit mask: 0b01111111 = todos los días
    };
    
    IrrigationSchedule morningSchedule;
    IrrigationSchedule eveningSchedule;
    IrrigationSchedule emergencySchedule;
    
    // Control variables
    float previousMoisture;
    float moistureRate; // Rate of change
    unsigned long lastMoistureCheck;
    
    // Irrigation parameters
    unsigned int defaultIrrigationDuration; // segundos
    unsigned int maxIrrigationDuration;
    unsigned int minIntervalBetweenIrrigation; // segundos
    float emergencyMoistureThreshold;
    
    // Soil and plant factors
    float soilRetentionCapacity;
    float plantWaterConsumption; // ml/hour estimated
    float evapotranspirationRate;
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastIrrigationTime;
    unsigned long currentIrrigationStart;
    bool irrigationActive;
    
    // Environmental factors
    float temperatureFactor;
    float humidityFactor;
    float lightFactor;
    bool isRainingOutside;
    
    // Status
    bool enabled;
    bool scheduleEnabled;
    bool emergencyModeActive;
    
    // Water management
    float waterFlowRate; // ml/minute
    float totalWaterUsed; // ml
    float dailyWaterUsed;
    unsigned long wateringSessionTime;
    
    // Statistics
    unsigned int totalIrrigations;
    unsigned int dailyIrrigations;
    unsigned long totalIrrigationTime;
    unsigned long lastIrrigationDuration;

public:
    bool isIrrigationActive() const;
    IrrigationControl();
    
    bool begin();
    void update(float currentMoisture, float temperature = -1, float humidity = -1, float lightLevel = -1);
    
    // Configuration
    void setTarget(float targetMoisture);
    float getTarget() const;
    void setTolerance(float tol);
    void setIrrigationDuration(unsigned int seconds);
    void setMaxIrrigationDuration(unsigned int seconds);
    void setMinInterval(unsigned int seconds);
    void setEmergencyThreshold(float threshold);
    
    // Soil and plant configuration
    void setSoilRetentionCapacity(float capacity);
    void setPlantWaterConsumption(float consumption);
    void setWaterFlowRate(float flowRate);
    
    // Scheduling
    void setMorningSchedule(uint8_t hour, uint8_t minute, unsigned int duration, uint8_t weekDays = 0b01111111);
    void setEveningSchedule(uint8_t hour, uint8_t minute, unsigned int duration, uint8_t weekDays = 0b01111111);
    void enableSchedule(bool enable);
    bool isScheduleEnabled() const;
    
    // Environmental factors
    void setTemperatureFactor(float factor);
    void setHumidityFactor(float factor);
    void setLightFactor(float factor);
    void setRainStatus(bool isRaining);
    
    // Control
    void enable();
    void disable();
    bool isEnabled() const;
    void startIrrigation(unsigned int duration = 0);
    void stopIrrigation();
    void emergencyIrrigation();
    
    // Status
    float getCurrentSoilMoisture() const;
    float getMoistureDeficit() const;
    bool isEmergencyModeActive() const;
    bool needsIrrigation() const;
    bool isInRange() const;
    unsigned long getRemainingIrrigationTime() const;
    
    // Water calculations
    unsigned int calculateOptimalDuration() const;
    float estimateWaterNeeded() const; // ml
    float getEstimatedEvapotranspiration() const;
    
    // Statistics
    float getTotalWaterUsed() const;
    float getDailyWaterUsed() const;
    unsigned int getTotalIrrigations() const;
    unsigned int getDailyIrrigations() const;
    unsigned long getTotalIrrigationTime() const;
    unsigned long getLastIrrigationDuration() const;
    void resetDailyStatistics();
    void resetStatistics();
    
    // Status string
    String getStatusString() const;
    String getScheduleString() const;
    String getWaterUsageString() const;
    
    // Advanced features
    void adaptToWeather(float outsideTemperature, float outsideHumidity, bool isRaining);
    void adaptToPlantGrowthStage(uint8_t stage); // 1=seedling, 2=vegetative, 3=flowering, 4=fruiting
    bool predictIrrigationNeed(unsigned int hoursAhead) const;
    
    // Emergency check
    bool checkEmergency() const;
    bool checkWaterShortage() const;
    
private:
    bool shouldIrrigateNow() const;
    bool isScheduledTime(uint8_t currentHour, uint8_t currentMinute) const;
    bool isValidIrrigationTime() const;
    float calculateAdjustedDuration() const;
    void updateEvapotranspiration(float temperature, float humidity, float light);
};

#endif
