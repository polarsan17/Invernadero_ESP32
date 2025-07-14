
#include "logic/IrrigationControl.h"

// Devuelve si el riego está activo
bool IrrigationControl::isIrrigationActive() const {
    return irrigationActive;
}

IrrigationControl::IrrigationControl() :
    targetSoilMoisture(70.0),
    currentSoilMoisture(50.0),
    tolerance(5.0),
    previousMoisture(50.0),
    moistureRate(0.0),
    lastMoistureCheck(0),
    defaultIrrigationDuration(30),
    maxIrrigationDuration(300),
    minIntervalBetweenIrrigation(1800),
    emergencyMoistureThreshold(20.0),
    soilRetentionCapacity(100.0),
    plantWaterConsumption(50.0),
    evapotranspirationRate(1.0),
    lastUpdate(0),
    lastIrrigationTime(0),
    currentIrrigationStart(0),
    irrigationActive(false),
    temperatureFactor(1.0),
    humidityFactor(1.0),
    lightFactor(1.0),
    isRainingOutside(false),
    enabled(false),
    scheduleEnabled(true),
    emergencyModeActive(false),
    waterFlowRate(100.0),
    totalWaterUsed(0.0),
    dailyWaterUsed(0.0),
    wateringSessionTime(0),
    totalIrrigations(0),
    dailyIrrigations(0),
    totalIrrigationTime(0),
    lastIrrigationDuration(0),
    morningSchedule({7, 0, 60, true, 0b01111111}),    // 7:00 AM, 60 seconds, every day
    eveningSchedule({19, 0, 45, true, 0b01111111}),   // 7:00 PM, 45 seconds, every day
    emergencySchedule({12, 0, 120, true, 0b01111111}) // Emergency at noon
{
    // Constructor body now empty of schedule initialization
}

bool IrrigationControl::begin() {
    Serial.println("[IrrigationControl] Initializing irrigation control system");
    
    lastUpdate = millis();
    lastIrrigationTime = 0;
    
    enabled = true;
    scheduleEnabled = true;
    
    Serial.println("[IrrigationControl] Initialized successfully");
    Serial.println(String("[IrrigationControl] Target: ") + targetSoilMoisture + "%");
    Serial.println(String("[IrrigationControl] Default duration: ") + defaultIrrigationDuration + " seconds");
    
    return true;
}

// cppcheck-suppress unusedFunction
void IrrigationControl::update(float currentMoisture, float temperature, float humidity, float lightLevel) {
    if (!enabled) return;
    
    unsigned long currentTime = millis();
    
    // Update current readings
    previousMoisture = currentSoilMoisture;
    currentSoilMoisture = currentMoisture;
    
    // Calculate moisture change rate
    if (lastMoistureCheck > 0) {
        float timeDiff = (currentTime - lastMoistureCheck) / 1000.0; // seconds
        moistureRate = (currentSoilMoisture - previousMoisture) / timeDiff;
    }
    lastMoistureCheck = currentTime;
    
    // Update environmental factors
    if (temperature > 0) {
        temperatureFactor = 1.0 + ((temperature - 25.0) * 0.03); // 3% per degree above 25°C
    }
    if (humidity > 0) {
        humidityFactor = 1.0 - ((humidity - 50.0) * 0.01); // Reduce need when humidity high
    }
    if (lightLevel > 0) {
        lightFactor = 1.0 + (lightLevel / 50000.0 * 0.2); // Increase need with more light
    }
    
    updateEvapotranspiration(temperature, humidity, lightLevel);
    
    // Check emergency conditions
    if (currentSoilMoisture <= emergencyMoistureThreshold && !irrigationActive) {
        emergencyIrrigation();
        return;
    }
    
    // Check if irrigation is currently active
    if (irrigationActive) {
        unsigned long irrigationDuration = currentTime - currentIrrigationStart;
        if (irrigationDuration >= wateringSessionTime) {
            stopIrrigation();
        }
        return;
    }
    
    // Check if irrigation is needed
    if (needsIrrigation() && isValidIrrigationTime()) {
        unsigned int duration = calculateOptimalDuration();
        startIrrigation(duration);
    }
    
    lastUpdate = currentTime;
}

// cppcheck-suppress unusedFunction
void IrrigationControl::setTarget(float targetMoisture) {
    if (targetMoisture >= 30.0 && targetMoisture <= 95.0) {
        targetSoilMoisture = targetMoisture;
        Serial.println(String("[IrrigationControl] Target soil moisture set to: ") + targetMoisture + "%");
    }
}

// cppcheck-suppress unusedFunction
float IrrigationControl::getTarget() const {
    return targetSoilMoisture;
}

// cppcheck-suppress unusedFunction
void IrrigationControl::enable() {
    enabled = true;
    Serial.println("[IrrigationControl] Enabled");
}

// cppcheck-suppress unusedFunction
void IrrigationControl::disable() {
    enabled = false;
    if (irrigationActive) {
        stopIrrigation();
    }
    Serial.println("[IrrigationControl] Disabled");
}

// cppcheck-suppress unusedFunction
bool IrrigationControl::isEnabled() const {
    return enabled;
}

void IrrigationControl::startIrrigation(unsigned int duration) {
    if (!enabled || irrigationActive) return;
    
    if (duration == 0) {
        duration = defaultIrrigationDuration;
    }
    
    // Apply environmental factors to duration
    float adjustedDuration = duration * temperatureFactor * humidityFactor * lightFactor;
    adjustedDuration = min(adjustedDuration, (float)maxIrrigationDuration);
    
    wateringSessionTime = (unsigned long)adjustedDuration * 1000; // Convert to milliseconds
    currentIrrigationStart = millis();
    irrigationActive = true;
    lastIrrigationTime = currentIrrigationStart;
    
    totalIrrigations++;
    dailyIrrigations++;
    
    float sessionWater = (waterFlowRate * adjustedDuration) / 60.0; // ml
    totalWaterUsed += sessionWater;
    dailyWaterUsed += sessionWater;
    
    Serial.println(String("[IrrigationControl] Irrigation started - Duration: ") + 
                  (adjustedDuration) + "s, Water: " + sessionWater + "ml");
}

void IrrigationControl::stopIrrigation() {
    if (!irrigationActive) return;
    
    unsigned long actualDuration = (millis() - currentIrrigationStart) / 1000;
    lastIrrigationDuration = actualDuration;
    totalIrrigationTime += actualDuration;
    
    irrigationActive = false;
    emergencyModeActive = false;
    
    Serial.println(String("[IrrigationControl] Irrigation stopped - Actual duration: ") + actualDuration + "s");
}

void IrrigationControl::emergencyIrrigation() {
    if (irrigationActive) return;
    
    emergencyModeActive = true;
    unsigned int emergencyDuration = min((unsigned int)(defaultIrrigationDuration * 2), maxIrrigationDuration);
    startIrrigation(emergencyDuration);
    
    Serial.println("[IrrigationControl] EMERGENCY IRRIGATION ACTIVATED");
}

bool IrrigationControl::needsIrrigation() const {
    if (!enabled || isRainingOutside) return false;
    
    float moistureDeficit = targetSoilMoisture - currentSoilMoisture;
    
    // Check if soil moisture is below target minus tolerance
    if (moistureDeficit > tolerance) {
        return true;
    }
    
    // Check if soil is drying rapidly
    if (moistureRate < -0.5 && currentSoilMoisture < targetSoilMoisture) {
        return true;
    }
    
    return false;
}

bool IrrigationControl::isValidIrrigationTime() const {
    unsigned long currentTime = millis();
    
    // Check minimum interval between irrigations
    if ((currentTime - lastIrrigationTime) < (minIntervalBetweenIrrigation * 1000)) {
        return false;
    }
    
    return true;
}

unsigned int IrrigationControl::calculateOptimalDuration() const {
    float moistureDeficit = targetSoilMoisture - currentSoilMoisture;
    
    // Base duration calculation
    float baseDuration = (moistureDeficit / 10.0) * defaultIrrigationDuration;
    
    // Adjust for environmental factors
    baseDuration *= temperatureFactor * humidityFactor * lightFactor;
    
    // Apply limits
    baseDuration = max(baseDuration, 10.0f); // Minimum 10 seconds
    baseDuration = min(baseDuration, (float)maxIrrigationDuration);
    
    return (unsigned int)baseDuration;
}

// cppcheck-suppress unusedFunction
String IrrigationControl::getStatusString() const {
    String status = String(currentSoilMoisture, 1) + "%";
    
    if (!enabled) {
        status += " (OFF)";
    } else if (irrigationActive) {
        unsigned long remaining = (wateringSessionTime - (millis() - currentIrrigationStart)) / 1000;
        status += " (IRRIGATING: " + String(remaining) + "s)";
    } else if (emergencyModeActive) {
        status += " (EMERGENCY)";
    } else {
        status += " (MONITORING)";
    }
    
    status += " [Target: " + String(targetSoilMoisture, 1) + "%]";
    
    return status;
}

// cppcheck-suppress unusedFunction
bool IrrigationControl::checkEmergency() const {
    if (!enabled) return false;
    
    if (currentSoilMoisture <= emergencyMoistureThreshold) {
        Serial.println("[IrrigationControl] EMERGENCY: Critical low soil moisture: " + String(currentSoilMoisture) + "%");
        return true;
    }
    
    return false;
}

void IrrigationControl::updateEvapotranspiration(float temperature, float humidity, float light) {
    // Simple ET calculation based on environmental factors
    float baseET = plantWaterConsumption; // ml/hour
    
    if (temperature > 0) {
        baseET *= (1.0 + (temperature - 20.0) * 0.05); // 5% per degree above 20°C
    }
    
    if (humidity > 0) {
        baseET *= (1.0 - (humidity - 50.0) * 0.01); // Reduce with high humidity
    }
    
    if (light > 0) {
        baseET *= (1.0 + light / 50000.0 * 0.3); // Increase with light
    }
    
    evapotranspirationRate = baseET;
}

void IrrigationControl::resetDailyStatistics() {
    dailyWaterUsed = 0.0;
    dailyIrrigations = 0;
    Serial.println("[IrrigationControl] Daily statistics reset");
}

// cppcheck-suppress unusedFunction
void IrrigationControl::resetStatistics() {
    totalWaterUsed = 0.0;
    totalIrrigations = 0;
    totalIrrigationTime = 0;
    resetDailyStatistics();
    Serial.println("[IrrigationControl] All statistics reset");
}
