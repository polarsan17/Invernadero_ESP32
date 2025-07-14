#include "logic/HumidityControl.h"

HumidityControl::HumidityControl() :
    targetHumidity(65.0),
    currentHumidity(50.0),
    tolerance(5.0),
    previousError(0.0),
    errorSum(0.0),
    kp(1.5),
    ki(0.05),
    kd(0.3),
    minHumidity(30.0),
    maxHumidity(90.0),
    criticalLowHumidity(20.0),
    criticalHighHumidity(95.0),
    lastUpdate(0),
    lastAction(0),
    enabled(false),
    humidifyingActive(false),
    dehumidifyingActive(false),
    ventilationActive(false),
    temperatureFactor(1.0),
    isDaytime(true),
    totalActiveTime(0),
    adjustmentCount(0),
    lastActiveTime(0)
{
}

bool HumidityControl::begin() {
    Serial.println("[HumidityControl] Initializing humidity control system");
    
    // Reset control variables
    previousError = 0.0;
    errorSum = 0.0;
    lastUpdate = millis();
    lastAction = 0;
    
    // Set PID constants optimized for humidity control
    // Humidity changes more slowly than temperature
    kp = 1.5;  // Moderate proportional gain
    ki = 0.05; // Low integral gain (humidity is slow to change)
    kd = 0.3;  // Low derivative gain
    
    enabled = true;
    
    Serial.println("[HumidityControl] Initialized successfully");
    Serial.println(String("[HumidityControl] Target: ") + targetHumidity + "%, Tolerance: ±" + tolerance + "%");
    Serial.println(String("[HumidityControl] PID Constants - Kp:") + kp + " Ki:" + ki + " Kd:" + kd);
    
    return true;
}

// cppcheck-suppress unusedFunction
void HumidityControl::update(float currentHum, float currentTemp) {
    if (!enabled) return;
    
    unsigned long currentTime = millis();
    float deltaTime = (currentTime - lastUpdate) / 1000.0;
    
    if (deltaTime <= 0) return;
    
    currentHumidity = currentHum;
    
    // Adjust target based on temperature (warmer air can hold more moisture)
    float adjustedTarget = targetHumidity;
    if (currentTemp > 0) {
        temperatureFactor = 1.0 + ((currentTemp - 20.0) * 0.02); // 2% per degree above 20°C
        adjustedTarget = targetHumidity * temperatureFactor;
        
        // Keep within reasonable bounds
        if (adjustedTarget > maxHumidity) adjustedTarget = maxHumidity;
        if (adjustedTarget < minHumidity) adjustedTarget = minHumidity;
    }
    
    // Calculate error
    float error = adjustedTarget - currentHumidity;
    
    // Update error sum (with windup protection)
    errorSum += error * deltaTime;
    if (errorSum > 100.0) errorSum = 100.0;
    if (errorSum < -100.0) errorSum = -100.0;
    
    // Calculate derivative
    float derivative = (error - previousError) / deltaTime;
    
    // Calculate control output (for future advanced control)
    // float controlOutput = (kp * error) + (ki * errorSum) + (kd * derivative);
    
    // Determine control actions
    bool wasActive = humidifyingActive || dehumidifyingActive || ventilationActive;
    
    humidifyingActive = false;
    dehumidifyingActive = false;
    ventilationActive = false;
    
    // Check if minimum time between actions has passed
    bool actionAllowed = (currentTime - lastAction) >= MIN_ACTION_INTERVAL;
    
    if (actionAllowed) {
        if (error > tolerance) {
            // Humidity too low - need humidification
            if (currentHumidity < criticalLowHumidity) {
                // Critical low humidity - immediate action
                humidifyingActive = true;
                lastAction = currentTime;
            } else if (isDaytime && error > tolerance * 1.5) {
                // Normal humidification during day
                humidifyingActive = true;
                lastAction = currentTime;
            }
            
            if (!wasActive && humidifyingActive) {
                lastActiveTime = currentTime;
                adjustmentCount++;
                Serial.println(String("[HumidityControl] Humidification activated. Current: ") + 
                             currentHumidity + "%, Target: " + adjustedTarget + "%");
            }
        } else if (error < -tolerance) {
            // Humidity too high - need dehumidification
            if (currentHumidity > criticalHighHumidity) {
                // Critical high humidity - ventilation + dehumidification
                dehumidifyingActive = true;
                ventilationActive = true;
                lastAction = currentTime;
            } else if (error < -tolerance * 1.5) {
                // Moderate high humidity - primarily ventilation
                ventilationActive = true;
                if (error < -tolerance * 2.0) {
                    dehumidifyingActive = true;
                }
                lastAction = currentTime;
            }
            
            if (!wasActive && (dehumidifyingActive || ventilationActive)) {
                lastActiveTime = currentTime;
                adjustmentCount++;
                Serial.println(String("[HumidityControl] Dehumidification activated. Current: ") + 
                             currentHumidity + "%, Target: " + adjustedTarget + "%");
            }
        } else {
            // Humidity in acceptable range
            if (wasActive) {
                totalActiveTime += (currentTime - lastActiveTime);
                Serial.println(String("[HumidityControl] Humidity stable. Current: ") + 
                             currentHumidity + "%, Target: " + adjustedTarget + "%");
            }
        }
    }
    
    previousError = error;
    lastUpdate = currentTime;
}

// Configuration methods
// cppcheck-suppress unusedFunction
void HumidityControl::setTarget(float target) {
    if (target >= minHumidity && target <= maxHumidity) {
        targetHumidity = target;
        errorSum = 0.0; // Reset integral
        Serial.println(String("[HumidityControl] Target humidity set to: ") + target + "%");
    } else {
        Serial.println(String("[HumidityControl] Invalid target humidity: ") + target + 
                      "% (Range: " + minHumidity + "-" + maxHumidity + "%)");
    }
}

// cppcheck-suppress unusedFunction
float HumidityControl::getTarget() const {
    return targetHumidity;
}

// cppcheck-suppress unusedFunction
void HumidityControl::setTolerance(float tol) {
    if (tol > 0.0 && tol <= 20.0) {
        tolerance = tol;
        Serial.println(String("[HumidityControl] Tolerance set to: ±") + tolerance + "%");
    }
}

// cppcheck-suppress unusedFunction
void HumidityControl::setControlConstants(float p, float i, float d) {
    kp = p;
    ki = i;
    kd = d;
    errorSum = 0.0; // Reset integral
    Serial.println(String("[HumidityControl] Control constants updated - Kp:") + kp + " Ki:" + ki + " Kd:" + kd);
}

// cppcheck-suppress unusedFunction
void HumidityControl::setHumidityLimits(float min, float max) {
    if (min < max && min >= 0.0 && max <= 100.0) {
        minHumidity = min;
        maxHumidity = max;
        Serial.println(String("[HumidityControl] Humidity limits set: ") + min + "-" + max + "%");
    }
}

// cppcheck-suppress unusedFunction
void HumidityControl::setCriticalLimits(float criticalLow, float criticalHigh) {
    criticalLowHumidity = criticalLow;
    criticalHighHumidity = criticalHigh;
    Serial.println(String("[HumidityControl] Critical limits set: ") + criticalLow + "-" + criticalHigh + "%");
}

// cppcheck-suppress unusedFunction
void HumidityControl::setDaytimeStatus(bool isDay) {
    isDaytime = isDay;
}

// cppcheck-suppress unusedFunction
void HumidityControl::setTemperatureFactor(float factor) {
    temperatureFactor = factor;
}

// cppcheck-suppress unusedFunction
void HumidityControl::enable() {
    enabled = true;
    errorSum = 0.0;
    previousError = 0.0;
    lastUpdate = millis();
    Serial.println("[HumidityControl] Enabled");
}

// cppcheck-suppress unusedFunction
void HumidityControl::disable() {
    enabled = false;
    humidifyingActive = false;
    dehumidifyingActive = false;
    ventilationActive = false;
    Serial.println("[HumidityControl] Disabled");
}

// cppcheck-suppress unusedFunction
bool HumidityControl::isEnabled() const {
    return enabled;
}

// cppcheck-suppress unusedFunction
float HumidityControl::getCurrentHumidity() const {
    return currentHumidity;
}

float HumidityControl::getError() const {
    return targetHumidity - currentHumidity;
}

// cppcheck-suppress unusedFunction
bool HumidityControl::isHumidifyingActive() const {
    return enabled && humidifyingActive;
}

// cppcheck-suppress unusedFunction
bool HumidityControl::isDehumidifyingActive() const {
    return enabled && dehumidifyingActive;
}

// cppcheck-suppress unusedFunction
bool HumidityControl::isVentilationActive() const {
    return enabled && ventilationActive;
}

// cppcheck-suppress unusedFunction
bool HumidityControl::isInRange() const {
    float error = abs(getError());
    return error <= tolerance;
}

// cppcheck-suppress unusedFunction
int HumidityControl::getVentilationLevel() {
    // Calculate ventilation level based on control state
    if (!enabled || !ventilationActive) {
        return 0;
    }
    
    // Calculate ventilation level based on humidity error (0-100%)
    float error = abs(currentHumidity - targetHumidity);
    float maxError = maxHumidity - minHumidity;
    int level = (int)((error / maxError) * 100.0f);
    
    // Constrain to 0-100%
    return constrain(level, 0, 100);
}

// cppcheck-suppress unusedFunction
float HumidityControl::getControlOutput() {
    // Return calculated PID output for monitoring purposes
    float error = getError();
    return (kp * error) + (ki * errorSum) + (kd * (error - previousError));
}

// cppcheck-suppress unusedFunction
bool HumidityControl::isCriticalCondition() const {
    return (currentHumidity <= criticalLowHumidity) || (currentHumidity >= criticalHighHumidity);
}

// Statistics methods
// cppcheck-suppress unusedFunction
unsigned long HumidityControl::getTotalActiveTime() const {
    unsigned long total = totalActiveTime;
    if (humidifyingActive || dehumidifyingActive || ventilationActive) {
        total += (millis() - lastActiveTime);
    }
    return total;
}

// cppcheck-suppress unusedFunction
unsigned int HumidityControl::getAdjustmentCount() const {
    return adjustmentCount;
}

// cppcheck-suppress unusedFunction
void HumidityControl::resetStatistics() {
    totalActiveTime = 0;
    adjustmentCount = 0;
    Serial.println("[HumidityControl] Statistics reset");
}

// cppcheck-suppress unusedFunction
String HumidityControl::getStatusString() const {
    String status = String(currentHumidity, 1) + "%";
    
    if (!enabled) {
        status += " (OFF)";
    } else if (humidifyingActive) {
        status += " (HUMIDIFYING)";
    } else if (dehumidifyingActive) {
        status += " (DEHUMIDIFYING)";
    } else if (ventilationActive) {
        status += " (VENTILATING)";
    } else {
        status += " (STABLE)";
    }
    
    float adjustedTarget = targetHumidity * temperatureFactor;
    status += " [Target: " + String(adjustedTarget, 1) + "%]";
    
    if (isCriticalCondition()) {
        status += " [CRITICAL]";
    }
    
    return status;
}

// cppcheck-suppress unusedFunction
bool HumidityControl::checkEmergency() const {
    if (!enabled) return false;
    
    // Emergency conditions:
    // 1. Critical low humidity (<20%)
    // 2. Critical high humidity (>95%)
    // 3. Rapid humidity changes
    
    if (currentHumidity <= criticalLowHumidity) {
        Serial.println("[HumidityControl] EMERGENCY: Critical low humidity: " + String(currentHumidity) + "%");
        return true;
    }
    
    if (currentHumidity >= criticalHighHumidity) {
        Serial.println("[HumidityControl] EMERGENCY: Critical high humidity: " + String(currentHumidity) + "%");
        return true;
    }
    
    return false;
}

// cppcheck-suppress unusedFunction
void HumidityControl::adaptToWeather(bool isRaining, float outsideHumidity) {
    if (isRaining) {
        // Reduce target humidity when it's raining outside
        temperatureFactor = 0.9;
        Serial.println("[HumidityControl] Adapted for rainy weather");
    } else if (outsideHumidity > 0 && outsideHumidity < 30.0) {
        // Increase target humidity when outside air is dry
        temperatureFactor = 1.1;
        Serial.println("[HumidityControl] Adapted for dry weather");
    }
}

// cppcheck-suppress unusedFunction
float HumidityControl::calculateIdealHumidity(float temperature) const {
    // Calculate ideal humidity based on temperature
    // General rule: relative humidity should decrease as temperature increases
    float idealHumidity = 80.0 - (temperature - 15.0) * 1.5;
    
    // Keep within reasonable bounds
    if (idealHumidity > maxHumidity) idealHumidity = maxHumidity;
    if (idealHumidity < minHumidity) idealHumidity = minHumidity;
    
    return idealHumidity;
}
