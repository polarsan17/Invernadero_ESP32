#include "logic/LightControl.h"

LightControl::LightControl() :
    targetLightIntensity(15000.0),
    currentLightIntensity(1000.0),
    tolerance(2000.0),
    dailyLightHours(14),
    currentLightHours(0),
    dayStartTime(0),
    currentPhotoperiod(0),
    previousError(0.0),
    errorIntegral(0.0),
    kp(0.8),
    ki(0.02),
    kd(0.1),
    minLux(500.0),
    maxLux(50000.0),
    maxLEDIntensity(100.0),
    lastUpdate(0),
    lastAdjustment(0),
    enabled(false),
    artificialLightActive(false),
    photoperiodActive(true),
    isDaytime(true),
    cloudinessDetection(true),
    naturalLightLevel(0.0),
    supplementalLightLevel(0.0),
    totalLightTime(0),
    dailyLightTime(0),
    adjustmentCount(0),
    dailyEnergyConsumption(0.0),
    morningSchedule({6, 0, 8, 0, 30.0, true}),
    daySchedule({8, 0, 18, 0, 80.0, true}),
    eveningSchedule({18, 0, 20, 0, 50.0, true}),
    nightSchedule({20, 0, 6, 0, 0.0, false})
{
    // Constructor body now empty of schedule initialization
}

bool LightControl::begin() {
    Serial.println("[LightControl] Initializing light control system");
    
    // Reset control variables
    previousError = 0.0;
    errorIntegral = 0.0;
    lastUpdate = millis();
    dayStartTime = millis();
    
    // Set PID constants for light control
    kp = 0.8;  // Moderate proportional gain
    ki = 0.02; // Low integral gain (lights respond quickly)
    kd = 0.1;  // Low derivative gain
    
    enabled = true;
    photoperiodActive = true;
    
    Serial.println("[LightControl] Initialized successfully");
    Serial.println(String("[LightControl] Target: ") + targetLightIntensity + " lux");
    Serial.println(String("[LightControl] Photoperiod: ") + dailyLightHours + " hours");
    
    return true;
}

// cppcheck-suppress unusedFunction
void LightControl::update(float currentLux, uint8_t currentHour, uint8_t currentMinute) {
    if (!enabled) return;
    
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastUpdate) / 1000.0;
    
    if (deltaTime <= 0) return;
    
    currentLightIntensity = currentLux;
    naturalLightLevel = currentLux; // Assume current reading is natural light
    
    // Determine current schedule
    LightSchedule currentSchedule = getCurrentSchedule(currentHour, currentMinute);
    
    // Calculate target based on schedule and natural light
    float scheduleTarget = (targetLightIntensity * currentSchedule.intensity) / 100.0;
    float effectiveTarget = scheduleTarget;        // If we have natural light, reduce artificial light need
        if (cloudinessDetection && naturalLightLevel > minLux) {
            float naturalContribution = min(naturalLightLevel, scheduleTarget);
            effectiveTarget = max(0.0f, scheduleTarget - naturalContribution);
        }
    
    // Calculate error
    float totalLightAvailable = naturalLightLevel + supplementalLightLevel;
    float error = scheduleTarget - totalLightAvailable;
    
    // Update integral
    errorIntegral += error * deltaTime;
    if (errorIntegral > 100.0) errorIntegral = 100.0;
    if (errorIntegral < -100.0) errorIntegral = -100.0;
    
    // Calculate derivative
    float derivative = (error - previousError) / deltaTime;
    
    // Calculate LED intensity needed (for future advanced control)
    // float pidOutput = (kp * error) + (ki * errorIntegral) + (kd * derivative);
    
    // Check if adjustment is allowed
    bool adjustmentAllowed = (currentTime - lastAdjustment) >= MIN_ADJUST_INTERVAL;
    
    bool wasActive = artificialLightActive;
    artificialLightActive = false;
    
    if (adjustmentAllowed && currentSchedule.enabled) {
        if (error > tolerance && effectiveTarget > 0) {
            artificialLightActive = true;
            supplementalLightLevel = min(effectiveTarget, (float)(maxLEDIntensity * targetLightIntensity / 100.0));
            lastAdjustment = currentTime;
            
            if (!wasActive) {
                adjustmentCount++;
                Serial.println(String("[LightControl] Artificial light activated. Target: ") + 
                             scheduleTarget + " lux, Natural: " + naturalLightLevel + " lux");
            }
        } else if (error < -tolerance || effectiveTarget <= 0) {
            supplementalLightLevel = 0.0;
            if (wasActive) {
                lastAdjustment = currentTime;
                Serial.println("[LightControl] Artificial light deactivated - sufficient natural light");
            }
        }
    }
    
    // Update photoperiod
    updatePhotoperiod();
    
    // Update energy consumption
    if (artificialLightActive) {
        float powerConsumption = (supplementalLightLevel / targetLightIntensity) * 50.0; // 50W max LED strip
        dailyEnergyConsumption += (powerConsumption * deltaTime) / 3600.0; // Wh
    }
    
    previousError = error;
    lastUpdate = currentTime;
}

// cppcheck-suppress unusedFunction
void LightControl::setTarget(float targetLux) {
    if (targetLux >= minLux && targetLux <= maxLux) {
        targetLightIntensity = targetLux;
        errorIntegral = 0.0;
        Serial.println(String("[LightControl] Target light intensity set to: ") + targetLux + " lux");
    }
}

// cppcheck-suppress unusedFunction
float LightControl::getTarget() const {
    return targetLightIntensity;
}

// cppcheck-suppress unusedFunction
void LightControl::setPhotoperiod(unsigned int hours) {
    if (hours >= 8 && hours <= 18) {
        dailyLightHours = hours;
        Serial.println(String("[LightControl] Photoperiod set to: ") + hours + " hours");
    }
}

// cppcheck-suppress unusedFunction
void LightControl::enable() {
    enabled = true;
    errorIntegral = 0.0;
    lastUpdate = millis();
    Serial.println("[LightControl] Enabled");
}

// cppcheck-suppress unusedFunction
void LightControl::disable() {
    enabled = false;
    artificialLightActive = false;
    supplementalLightLevel = 0.0;
    Serial.println("[LightControl] Disabled");
}

// cppcheck-suppress unusedFunction
bool LightControl::isEnabled() const {
    return enabled;
}

// cppcheck-suppress unusedFunction
float LightControl::getCurrentLightIntensity() const {
    return currentLightIntensity;
}

// cppcheck-suppress unusedFunction
bool LightControl::isArtificialLightActive() const {
    return enabled && artificialLightActive;
}

float LightControl::getLEDIntensity() const {
    if (!artificialLightActive) return 0.0;
    
    float intensity = (supplementalLightLevel / targetLightIntensity) * 100.0;
    return min(intensity, (float)maxLEDIntensity);
}

// cppcheck-suppress unusedFunction
String LightControl::getStatusString() const {
    String status = String(currentLightIntensity, 0) + " lux";
    
    if (!enabled) {
        status += " (OFF)";
    } else if (artificialLightActive) {
        status += " (LED: " + String(getLEDIntensity(), 0) + "%)";
    } else {
        status += " (NATURAL)";
    }
    
    status += " [Target: " + String(targetLightIntensity, 0) + " lux]";
    
    return status;
}

// cppcheck-suppress unusedFunction
bool LightControl::checkEmergency() const {
    // No typical emergency conditions for lighting
    return false;
}

LightControl::LightSchedule LightControl::getCurrentSchedule(uint8_t hour, uint8_t minute) const {
    if (isTimeInSchedule(hour, minute, morningSchedule)) return morningSchedule;
    if (isTimeInSchedule(hour, minute, daySchedule)) return daySchedule;
    if (isTimeInSchedule(hour, minute, eveningSchedule)) return eveningSchedule;
    return nightSchedule;
}

bool LightControl::isTimeInSchedule(uint8_t hour, uint8_t minute, const LightSchedule& schedule) const {
    int currentMinutes = hour * 60 + minute;
    int startMinutes = schedule.startHour * 60 + schedule.startMinute;
    int endMinutes = schedule.endHour * 60 + schedule.endMinute;
    
    if (endMinutes < startMinutes) { // Schedule crosses midnight
        return (currentMinutes >= startMinutes) || (currentMinutes < endMinutes);
    } else {
        return (currentMinutes >= startMinutes) && (currentMinutes < endMinutes);
    }
}

void LightControl::updatePhotoperiod() {
    unsigned long currentTime = millis();
    unsigned long elapsedTime = currentTime - dayStartTime;
    currentPhotoperiod = elapsedTime / (60 * 1000); // Convert to minutes
    
    // Reset daily stats at midnight (approximately)
    if (currentPhotoperiod > 24 * 60) {
        resetDailyStatistics();
        dayStartTime = currentTime;
        currentPhotoperiod = 0;
    }
}

void LightControl::resetDailyStatistics() {
    dailyLightTime = 0;
    dailyEnergyConsumption = 0.0;
    Serial.println("[LightControl] Daily statistics reset");
}

// cppcheck-suppress unusedFunction
void LightControl::resetStatistics() {
    totalLightTime = 0;
    adjustmentCount = 0;
    resetDailyStatistics();
    Serial.println("[LightControl] All statistics reset");
}
