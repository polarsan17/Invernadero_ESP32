
#include "logic/VentilationControl.h"

// Devuelve si la ventilación está habilitada
bool VentilationControl::isEnabled() const {
    return fanEnabled;
}

VentilationControl::VentilationControl() :
    targetAirExchangeRate(6.0),
    currentAirQuality(50.0),
    temperatureThreshold(25.0),
    humidityThreshold(80.0),
    co2Threshold(1000.0),
    targetTemperature(22.0),
    targetHumidity(65.0),
    currentLevel(VENTILATION_OFF),
    previousLevel(VENTILATION_OFF),
    currentMode(MODE_COMPREHENSIVE),
    lastUpdate(0),
    lastLevelChange(0),
    minRunTime(60000),
    maxRunTime(1800000),
    cooldownTime(180000),
    outsideTemperature(20.0),
    outsideHumidity(60.0),
    isWindy(false),
    isRaining(false),
    isDaytime(true),
    fanEnabled(true),
    servoEnabled(true),
    fanSpeed(0),
    servoPosition(0),
    damperOpen(false),
    temperatureHysteresis(1.0),
    humidityHysteresis(5.0),
    co2Hysteresis(100.0),
    adaptiveControl(true),
    predictiveMode(false),
    energySavingMode(false),
    totalRunTime(0),
    dailyRunTime(0),
    cycleCount(0),
    dailyCycleCount(0),
    energyConsumption(0.0),
    emergencyActive(false),
    emergencyTemperatureThreshold(35.0),
    emergencyHumidityThreshold(90.0),
    emergencyCO2Threshold(2000.0)
{
}

bool VentilationControl::begin() {
    Serial.println("[VentilationControl] Initializing ventilation control system");
    
    lastUpdate = millis();
    currentLevel = VENTILATION_OFF;
    fanSpeed = 0;
    servoPosition = 0; // Closed position
    
    Serial.println("[VentilationControl] Initialized successfully");
    Serial.println(String("[VentilationControl] Mode: COMPREHENSIVE"));
    Serial.println(String("[VentilationControl] Temperature threshold: ") + temperatureThreshold + "°C");
    Serial.println(String("[VentilationControl] Humidity threshold: ") + humidityThreshold + "%");
    
    return true;
}

// cppcheck-suppress unusedFunction
void VentilationControl::update(float temperature, float humidity, float co2Level, float lightLevel) {
    unsigned long currentTime = millis();
    
    // Calculate composite air quality index
    currentAirQuality = calculateCompositeAirQuality(temperature, humidity, co2Level);
    
    // Check emergency conditions first
    if (checkEmergency()) {
        emergencyVentilation();
        return;
    }
    
    // Determine required ventilation level based on current mode
    VentilationLevel requiredLevel = calculateRequiredLevel();
    
    // Apply adaptive control if enabled
    if (adaptiveControl) {
        requiredLevel = adaptLevelToConditions(requiredLevel);
    }
    
    // Check if level change is allowed
    if (isLevelChangeAllowed() && requiredLevel != currentLevel) {
        previousLevel = currentLevel;
        currentLevel = requiredLevel;
        lastLevelChange = currentTime;
        
        if (currentLevel != VENTILATION_OFF) {
            cycleCount++;
            dailyCycleCount++;
        }
        
        updateFanControl();
        updateServoControl();
        
        Serial.println(String("[VentilationControl] Level changed to: ") + getLevelString() + 
                      " (Fan: " + fanSpeed + "%, Servo: " + servoPosition + "°)");
    }
    
    // Update energy consumption
    updateEnergyConsumption();
    
    lastUpdate = currentTime;
}

// cppcheck-suppress unusedFunction
void VentilationControl::setTemperatureThresholds(float target, float threshold, float hysteresis) {
    targetTemperature = target;
    temperatureThreshold = threshold;
    temperatureHysteresis = hysteresis;
    Serial.println(String("[VentilationControl] Temperature thresholds set - Target: ") + target + 
                  "°C, Threshold: " + threshold + "°C");
}

// cppcheck-suppress unusedFunction
void VentilationControl::setHumidityThresholds(float target, float threshold, float hysteresis) {
    targetHumidity = target;
    humidityThreshold = threshold;
    humidityHysteresis = hysteresis;
    Serial.println(String("[VentilationControl] Humidity thresholds set - Target: ") + target + 
                  "%, Threshold: " + threshold + "%");
}

void VentilationControl::emergencyVentilation() {
    emergencyActive = true;
    currentLevel = VENTILATION_MAX;
    updateFanControl();
    updateServoControl();
    
    Serial.println("[VentilationControl] EMERGENCY VENTILATION ACTIVATED");
}

// cppcheck-suppress unusedFunction
void VentilationControl::stopEmergencyVentilation() {
    emergencyActive = false;
    Serial.println("[VentilationControl] Emergency ventilation stopped");
}

VentilationControl::VentilationLevel VentilationControl::calculateRequiredLevel() const {
    // Nota: tempError y humError podrían usarse en el futuro para algoritmos más sofisticados
    // float tempError = abs(targetTemperature - outsideTemperature);
    // float humError = abs(targetHumidity - outsideHumidity);
    
    // Temperature-based control
    if (outsideTemperature > temperatureThreshold + temperatureHysteresis) {
        return VENTILATION_HIGH;
    } else if (outsideTemperature > temperatureThreshold) {
        return VENTILATION_MEDIUM;
    }
    
    // Humidity-based control
    if (outsideHumidity > humidityThreshold + humidityHysteresis) {
        return VENTILATION_HIGH;
    } else if (outsideHumidity > humidityThreshold) {
        return VENTILATION_LOW;
    }
    
    // Air quality based control
    if (currentAirQuality > 80.0) {
        return VENTILATION_MEDIUM;
    } else if (currentAirQuality > 60.0) {
        return VENTILATION_LOW;
    }
    
    return VENTILATION_OFF;
}

void VentilationControl::updateFanControl() {
    switch (currentLevel) {
        case VENTILATION_OFF:
            fanSpeed = 0;
            break;
        case VENTILATION_LOW:
            fanSpeed = 25;
            break;
        case VENTILATION_MEDIUM:
            fanSpeed = 50;
            break;
        case VENTILATION_HIGH:
            fanSpeed = 75;
            break;
        case VENTILATION_MAX:
            fanSpeed = 100;
            break;
    }
    
    // Apply energy saving mode
    if (energySavingMode && !emergencyActive) {
        fanSpeed = (fanSpeed * 80) / 100; // Reduce by 20%
    }
}

void VentilationControl::updateServoControl() {
    damperOpen = (currentLevel != VENTILATION_OFF);
    
    if (damperOpen) {
        // Servo position based on ventilation level (0° = closed, 90° = fully open)
        switch (currentLevel) {
            case VENTILATION_LOW:
                servoPosition = 30;
                break;
            case VENTILATION_MEDIUM:
                servoPosition = 60;
                break;
            case VENTILATION_HIGH:
                servoPosition = 90;
                break;
            case VENTILATION_MAX:
                servoPosition = 90;
                break;
            default:
                servoPosition = 0;
                break;
        }
    } else {
        servoPosition = 0; // Closed
    }
}

bool VentilationControl::isLevelChangeAllowed() const {
    unsigned long currentTime = millis();
    
    // Don't change too frequently
    if ((currentTime - lastLevelChange) < 30000) { // 30 seconds minimum
        return false;
    }
    
    // Check minimum run time
    if (currentLevel != VENTILATION_OFF && (currentTime - lastLevelChange) < minRunTime) {
        return false;
    }
    
    return true;
}

float VentilationControl::calculateCompositeAirQuality(float temp, float humidity, float co2) const {
    float tempScore = 0.0;
    float humScore = 0.0;
    float co2Score = 0.0;
    
    // Temperature score (0-100, lower is better)
    float tempDiff = abs(temp - targetTemperature);
    tempScore = min(tempDiff * 10.0f, 100.0f);
    
    // Humidity score (0-100, lower is better)
    float humDiff = abs(humidity - targetHumidity);
    humScore = min(humDiff * 2.0f, 100.0f);
    
    // CO2 score (0-100, lower is better)
    if (co2 > 0) {
        co2Score = min((co2 - 400.0f) / 10.0f, 100.0f);
    }
    
    // Weighted average
    float composite = (tempScore * 0.4f) + (humScore * 0.4f) + (co2Score * 0.2f);
    return min(composite, 100.0f);
}

void VentilationControl::updateEnergyConsumption() {
    if (currentLevel != VENTILATION_OFF) {
        float powerUsage = (fanSpeed / 100.0f) * 30.0f; // 30W max fan
        float deltaTime = (millis() - lastUpdate) / 1000.0f / 3600.0f; // hours
        energyConsumption += powerUsage * deltaTime; // Wh
    }
}

VentilationControl::VentilationLevel VentilationControl::adaptLevelToConditions(VentilationLevel baseLevel) const {
    // Reduce ventilation when it's raining
    if (isRaining && baseLevel > VENTILATION_LOW) {
        return (VentilationLevel)(baseLevel - 25);
    }
    
    // Increase ventilation when it's windy (better air exchange)
    if (isWindy && baseLevel < VENTILATION_HIGH) {
        return (VentilationLevel)(baseLevel + 25);
    }
    
    // Reduce ventilation at night for energy saving
    if (!isDaytime && energySavingMode && baseLevel > VENTILATION_LOW) {
        return VENTILATION_LOW;
    }
    
    return baseLevel;
}

// cppcheck-suppress unusedFunction
String VentilationControl::getStatusString() const {
    String status = getLevelString();
    
    if (emergencyActive) {
        status += " (EMERGENCY)";
    }
    
    status += " [Fan: " + String(fanSpeed) + "%, Servo: " + String(servoPosition) + "°]";
    
    return status;
}

String VentilationControl::getLevelString() const {
    switch (currentLevel) {
        case VENTILATION_OFF: return "OFF";
        case VENTILATION_LOW: return "LOW";
        case VENTILATION_MEDIUM: return "MEDIUM";
        case VENTILATION_HIGH: return "HIGH";
        case VENTILATION_MAX: return "MAX";
        default: return "UNKNOWN";
    }
}

bool VentilationControl::checkEmergency() const {
    if (outsideTemperature > emergencyTemperatureThreshold) {
        Serial.println("[VentilationControl] EMERGENCY: Critical temperature: " + String(outsideTemperature) + "°C");
        return true;
    }
    
    if (outsideHumidity > emergencyHumidityThreshold) {
        Serial.println("[VentilationControl] EMERGENCY: Critical humidity: " + String(outsideHumidity) + "%");
        return true;
    }
    
    return false;
}

uint8_t VentilationControl::getFanSpeed() const {
    return fanSpeed;
}

uint8_t VentilationControl::getServoPosition() const {
    return servoPosition;
}

// cppcheck-suppress unusedFunction
bool VentilationControl::isDamperOpen() const {
    return damperOpen;
}

// cppcheck-suppress unusedFunction
VentilationControl::VentilationLevel VentilationControl::getCurrentLevel() const {
    return currentLevel;
}

void VentilationControl::resetDailyStatistics() {
    dailyRunTime = 0;
    dailyCycleCount = 0;
    Serial.println("[VentilationControl] Daily statistics reset");
}

// cppcheck-suppress unusedFunction
void VentilationControl::resetStatistics() {
    totalRunTime = 0;
    cycleCount = 0;
    energyConsumption = 0.0;
    resetDailyStatistics();
    Serial.println("[VentilationControl] All statistics reset");
}
