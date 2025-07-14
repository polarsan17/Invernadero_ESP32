#ifndef LIGHT_CONTROL_H
#define LIGHT_CONTROL_H

#include <Arduino.h>

/**
 * LightControl - Control de iluminación automático para invernadero
 * Implementa fotoperiodismo y control de intensidad lumínica
 * Basado en algoritmos de agricultura de precisión y LED horticulture
 */
class LightControl {
private:
    float targetLightIntensity; // Lux target
    float currentLightIntensity;
    float tolerance;
    
    // Photoperiod control
    unsigned int dailyLightHours; // Horas de luz al día
    unsigned int currentLightHours;
    unsigned long dayStartTime;
    unsigned long currentPhotoperiod;
    
    // Light scheduling
    struct LightSchedule {
        uint8_t startHour;
        uint8_t startMinute;
        uint8_t endHour;
        uint8_t endMinute;
        float intensity; // 0-100%
        bool enabled;
    };
    
    LightSchedule morningSchedule;
    LightSchedule daySchedule;
    LightSchedule eveningSchedule;
    LightSchedule nightSchedule;
    
    // Control variables
    float previousError;
    float errorIntegral;
    float kp, ki, kd;
    
    // Light limits
    float minLux, maxLux;
    float maxLEDIntensity; // 0-100%
    
    // Timing
    unsigned long lastUpdate;
    unsigned long lastAdjustment;
    const unsigned long MIN_ADJUST_INTERVAL = 10000; // 10 segundos
    
    // Status
    bool enabled;
    bool artificialLightActive;
    bool photoperiodActive;
    bool isDaytime;
    
    // Environmental adaptation
    bool cloudinessDetection;
    float naturalLightLevel;
    float supplementalLightLevel;
    
    // Statistics
    unsigned long totalLightTime;
    unsigned long dailyLightTime;
    unsigned int adjustmentCount;
    float dailyEnergyConsumption;

public:
    LightControl();
    
    bool begin();
    void update(float currentLux, uint8_t currentHour = 255, uint8_t currentMinute = 255);
    
    // Configuration
    void setTarget(float targetLux);
    float getTarget() const;
    void setTolerance(float tol);
    void setPIDConstants(float p, float i, float d);
    void setLightLimits(float minLux, float maxLux);
    void setPhotoperiod(unsigned int hours);
    
    // Scheduling
    void setSchedule(const LightSchedule& morning, const LightSchedule& day, 
                    const LightSchedule& evening, const LightSchedule& night);
    void setMorningSchedule(uint8_t startH, uint8_t startM, uint8_t endH, uint8_t endM, float intensity);
    void setDaySchedule(uint8_t startH, uint8_t startM, uint8_t endH, uint8_t endM, float intensity);
    void setEveningSchedule(uint8_t startH, uint8_t startM, uint8_t endH, uint8_t endM, float intensity);
    void setNightSchedule(uint8_t startH, uint8_t startM, uint8_t endH, uint8_t endM, float intensity);
    
    // Control modes
    void enable();
    void disable();
    bool isEnabled() const;
    void enablePhotoperiod(bool enable);
    bool isPhotoperiodEnabled() const;
    void enableCloudinessDetection(bool enable);
    
    // Status
    float getCurrentLightIntensity() const;
    float getError() const;
    bool isArtificialLightActive() const;
    bool isDaytimeActive() const;
    bool isInRange() const;
    unsigned int getCurrentPhotoperiod() const; // minutos
    
    // Light output control
    float getLEDIntensity() const; // 0-100%
    float getSupplementalLight() const;
    float getNaturalLight() const;
    
    // Statistics
    unsigned long getTotalLightTime() const;
    unsigned long getDailyLightTime() const;
    float getDailyEnergyConsumption() const;
    unsigned int getAdjustmentCount() const;
    void resetDailyStatistics();
    void resetStatistics();
    
    // Status string
    String getStatusString() const;
    String getScheduleString() const;
    
    // Advanced features
    void adaptToSeason(uint8_t month); // 1-12
    void adaptToPlantGrowthStage(uint8_t stage); // 1=seedling, 2=vegetative, 3=flowering, 4=fruiting
    bool checkPhotoperiodComplete() const;
    
    // Emergency check
    bool checkEmergency() const;
    
private:
    LightSchedule getCurrentSchedule(uint8_t hour, uint8_t minute) const;
    bool isTimeInSchedule(uint8_t hour, uint8_t minute, const LightSchedule& schedule) const;
    void updatePhotoperiod();
    float calculateOptimalIntensity(uint8_t hour, uint8_t minute) const;
};

#endif
