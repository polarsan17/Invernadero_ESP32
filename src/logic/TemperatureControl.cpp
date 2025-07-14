#include "logic/TemperatureControl.h"

TemperatureControl::TemperatureControl() :
    targetTemperature(22.0),
    currentTemperature(20.0),
    tolerance(1.0),
    previousError(0.0),
    integral(0.0),
    derivative(0.0),
    kp(2.0),
    ki(0.1),
    kd(0.5),
    minTemp(10.0),
    maxTemp(35.0),
    maxIntegral(100.0),
    lastUpdate(0),
    deltaTime(0.0),
    enabled(false),
    heatingActive(false),
    coolingActive(false),
    totalActiveTime(0),
    lastActiveTime(0),
    adjustmentCount(0)
{
}

bool TemperatureControl::begin() {
    Serial.println("[TemperatureControl] Initializing temperature control system");
    
    // Reset PID variables
    previousError = 0.0;
    integral = 0.0;
    derivative = 0.0;
    lastUpdate = millis();
    
    // Set default PID constants optimized for greenhouse temperature control
    // Based on thermal mass and response characteristics
    kp = 2.0;  // Proportional gain
    ki = 0.1;  // Integral gain (slow to prevent overshoot)
    kd = 0.5;  // Derivative gain (moderate for stability)
    
    enabled = true;
    
    Serial.println("[TemperatureControl] Initialized successfully");
    Serial.println(String("[TemperatureControl] Target: ") + targetTemperature + "°C, Tolerance: ±" + tolerance + "°C");
    Serial.println(String("[TemperatureControl] PID Constants - Kp:") + kp + " Ki:" + ki + " Kd:" + kd);
    
    return true;
}

// cppcheck-suppress unusedFunction
void TemperatureControl::update(float currentTemp) {
    if (!enabled) return;
    
    unsigned long currentTime = millis();
    deltaTime = (currentTime - lastUpdate) / 1000.0; // Convert to seconds
    
    if (deltaTime <= 0) return; // Avoid division by zero
    
    currentTemperature = currentTemp;
    
    // Calculate error
    float error = targetTemperature - currentTemperature;
    
    // Update integral (with windup protection)
    integral += error * deltaTime;
    if (integral > maxIntegral) integral = maxIntegral;
    if (integral < -maxIntegral) integral = -maxIntegral;
    
    // Calculate derivative
    derivative = (error - previousError) / deltaTime;
    
    // Calculate PID output (for future advanced control)
    // float pidOutput = getPIDOutput();
    
    // Determine heating/cooling actions based on error and tolerance
    bool wasHeating = heatingActive;
    bool wasCooling = coolingActive;
    
    heatingActive = false;
    coolingActive = false;
    
    if (error > tolerance) {
        // Temperature too low - need heating
        heatingActive = true;
        if (!wasHeating) {
            lastActiveTime = currentTime;
            adjustmentCount++;
            Serial.println(String("[TemperatureControl] Heating activated. Current: ") + 
                         currentTemperature + "°C, Target: " + targetTemperature + "°C");
        }
    } else if (error < -tolerance) {
        // Temperature too high - need cooling  
        coolingActive = true;
        if (!wasCooling) {
            lastActiveTime = currentTime;
            adjustmentCount++;
            Serial.println(String("[TemperatureControl] Cooling activated. Current: ") + 
                         currentTemperature + "°C, Target: " + targetTemperature + "°C");
        }
    } else {
        // Temperature in acceptable range
        if (wasHeating || wasCooling) {
            totalActiveTime += (currentTime - lastActiveTime);
            Serial.println(String("[TemperatureControl] Temperature stable. Current: ") + 
                         currentTemperature + "°C, Target: " + targetTemperature + "°C");
        }
    }
    
    previousError = error;
    lastUpdate = currentTime;
}

// Configuration methods
// cppcheck-suppress unusedFunction
void TemperatureControl::setTarget(float target) {
    if (target >= minTemp && target <= maxTemp) {
        targetTemperature = target;
        // Reset integral when target changes to prevent windup
        integral = 0.0;
        Serial.println(String("[TemperatureControl] Target temperature set to: ") + target + "°C");
    } else {
        Serial.println(String("[TemperatureControl] Invalid target temperature: ") + target + 
                      "°C (Range: " + minTemp + "-" + maxTemp + "°C)");
    }
}

// cppcheck-suppress unusedFunction
float TemperatureControl::getTarget() const {
    return targetTemperature;
}

// cppcheck-suppress unusedFunction
void TemperatureControl::setTolerance(float tol) {
    if (tol > 0.0 && tol <= 5.0) {
        tolerance = tol;
        Serial.println(String("[TemperatureControl] Tolerance set to: ±") + tolerance + "°C");
    }
}

// cppcheck-suppress unusedFunction
void TemperatureControl::setPIDConstants(float p, float i, float d) {
    kp = p;
    ki = i;
    kd = d;
    // Reset integral when PID constants change
    integral = 0.0;
    Serial.println(String("[TemperatureControl] PID constants updated - Kp:") + kp + " Ki:" + ki + " Kd:" + kd);
}

// cppcheck-suppress unusedFunction
void TemperatureControl::setTemperatureLimits(float min, float max) {
    if (min < max && min >= 0.0 && max <= 50.0) {
        minTemp = min;
        maxTemp = max;
        Serial.println(String("[TemperatureControl] Temperature limits set: ") + min + "-" + max + "°C");
    }
}

// Control methods
// cppcheck-suppress unusedFunction
void TemperatureControl::enable() {
    enabled = true;
    // Reset state when enabling
    integral = 0.0;
    previousError = 0.0;
    lastUpdate = millis();
    Serial.println("[TemperatureControl] Enabled");
}

// cppcheck-suppress unusedFunction
void TemperatureControl::disable() {
    enabled = false;
    heatingActive = false;
    coolingActive = false;
    Serial.println("[TemperatureControl] Disabled");
}

// cppcheck-suppress unusedFunction
bool TemperatureControl::isEnabled() const {
    return enabled;
}

// Status methods
// cppcheck-suppress unusedFunction
float TemperatureControl::getCurrentTemperature() const {
    return currentTemperature;
}

float TemperatureControl::getError() const {
    return targetTemperature - currentTemperature;
}

// cppcheck-suppress unusedFunction
bool TemperatureControl::isHeatingActive() const {
    return enabled && heatingActive;
}

// cppcheck-suppress unusedFunction
bool TemperatureControl::isCoolingActive() const {
    return enabled && coolingActive;
}

// cppcheck-suppress unusedFunction
bool TemperatureControl::isInRange() const {
    float error = abs(getError());
    return error <= tolerance;
}

float TemperatureControl::getPIDOutput() {
    if (!enabled) return 0.0;
    
    float error = getError();
    float output = (kp * error) + (ki * integral) + (kd * derivative);
    
    // Limit output to reasonable range
    if (output > 100.0) output = 100.0;
    if (output < -100.0) output = -100.0;
    
    return output;
}

// Statistics methods
// cppcheck-suppress unusedFunction
unsigned long TemperatureControl::getTotalActiveTime() const {
    unsigned long total = totalActiveTime;
    if (heatingActive || coolingActive) {
        total += (millis() - lastActiveTime);
    }
    return total;
}

// cppcheck-suppress unusedFunction
unsigned int TemperatureControl::getAdjustmentCount() const {
    return adjustmentCount;
}

// cppcheck-suppress unusedFunction
void TemperatureControl::resetStatistics() {
    totalActiveTime = 0;
    adjustmentCount = 0;
    Serial.println("[TemperatureControl] Statistics reset");
}

// cppcheck-suppress unusedFunction
String TemperatureControl::getStatusString() const {
    String status = String(currentTemperature, 1) + "°C";
    
    if (!enabled) {
        status += " (OFF)";
    } else if (heatingActive) {
        status += " (HEATING)";
    } else if (coolingActive) {
        status += " (COOLING)";
    } else {
        status += " (STABLE)";
    }
    
    status += " [Target: " + String(targetTemperature, 1) + "°C]";
    
    return status;
}

// cppcheck-suppress unusedFunction
bool TemperatureControl::checkEmergency() const {
    if (!enabled) return false;
    
    // Emergency conditions:
    // 1. Temperature too high (>40°C)
    // 2. Temperature too low (<5°C)  
    // 3. Large deviation from target (>10°C)
    
    if (currentTemperature > 40.0) {
        Serial.println("[TemperatureControl] EMERGENCY: Critical high temperature: " + String(currentTemperature) + "°C");
        return true;
    }
    
    if (currentTemperature < 5.0) {
        Serial.println("[TemperatureControl] EMERGENCY: Critical low temperature: " + String(currentTemperature) + "°C");
        return true;
    }
    
    float error = abs(getError());
    if (error > 10.0) {
        Serial.println("[TemperatureControl] EMERGENCY: Large temperature deviation: " + String(error) + "°C");
        return true;
    }
    
    return false;
}
