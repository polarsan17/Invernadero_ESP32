#include "logic/LogicManager.h"
#include "config/Targets.h"

LogicManager::LogicManager() :
    temperatureControl(nullptr),
    humidityControl(nullptr),
    lightControl(nullptr),
    irrigationControl(nullptr),
    ventilationControl(nullptr),
    sensorManager(nullptr),
    actuatorManager(nullptr),
    blynkManager(nullptr),
    lastUpdate(0),
    autoMode(false),
    systemEnabled(false)
{
}

LogicManager::~LogicManager() {
    delete temperatureControl;
    delete humidityControl;
    delete lightControl;
    delete irrigationControl;
    delete ventilationControl;
}

bool LogicManager::begin(SensorManager* sensors, ActuatorManager* actuators, BlynkManager* blynk) {
    if (!sensors || !actuators || !blynk) {
        Serial.println("[LogicManager] Error: Invalid pointers provided");
        return false;
    }
    
    sensorManager = sensors;
    actuatorManager = actuators;
    blynkManager = blynk;
    
    // Initialize control modules
    temperatureControl = new TemperatureControl();
    humidityControl = new HumidityControl();
    lightControl = new LightControl();
    irrigationControl = new IrrigationControl();
    ventilationControl = new VentilationControl();
    
    bool success = true;
    success &= temperatureControl->begin();
    success &= humidityControl->begin();
    success &= lightControl->begin();
    success &= irrigationControl->begin();
    success &= ventilationControl->begin();
    
    if (success) {
        // Set initial targets from global struct
        temperatureControl->setTarget(targets.temperature);
        humidityControl->setTarget(targets.humidity);
        lightControl->setTarget(targets.luxMin);
        irrigationControl->setTarget(targets.soilMoisture);
        // Configure ventilation thresholds from global struct
        ventilationControl->setTemperatureThresholds(targets.ventTemp, targets.ventTemp + 3.0); // 3°C hysteresis
        ventilationControl->setHumidityThresholds(targets.humidity, targets.humidity + 15.0); // 15% hysteresis
        Serial.println("[LogicManager] Initialized successfully");
        systemEnabled = true;
    } else {
        Serial.println("[LogicManager] Failed to initialize control modules");
    }
    return success;
}

void LogicManager::update() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdate >= UPDATE_INTERVAL) {
        if (systemEnabled) {
            processLogic();
            sendStatusToBlynk();
        }
        
        lastUpdate = currentTime;
    }
}

void LogicManager::processLogic() {
    if (!autoMode || !systemEnabled) {
        return;
    }
    
    // Check for emergency conditions first
    if (checkAlerts()) {
        handleEmergency();
        return;
    }
    
    // Get current sensor readings
    float temperature = 20.0; // Default fallback
    float humidity = 50.0;
    float lightLevel = 1000.0;
    float soilMoisture = 50.0;
    // TODO: Get actual sensor readings from SensorManager
    // temperature = sensorManager->getTemperature();
    // humidity = sensorManager->getHumidity();
    // lightLevel = sensorManager->getLightLevel();
    // soilMoisture = sensorManager->getSoilMoisture();

    // --- Integrate global targets from Blynk ---
    temperatureControl->setTarget(targets.temperature);
    humidityControl->setTarget(targets.humidity);
    lightControl->setTarget(targets.luxMin);
    irrigationControl->setTarget(targets.soilMoisture);
    ventilationControl->setTemperatureThresholds(targets.ventTemp, targets.ventTemp + 3.0); // 3°C hysteresis
    ventilationControl->setHumidityThresholds(targets.humidity, targets.humidity + 15.0); // 15% hysteresis

    // Update each control module
    temperatureControl->update(temperature);
    humidityControl->update(humidity, temperature);
    lightControl->update(lightLevel);
    irrigationControl->update(soilMoisture, temperature, humidity, lightLevel);
    ventilationControl->update(temperature, humidity);
    
    // Apply control decisions to actuators
    applyTemperatureControl();
    applyHumidityControl();
    applyLightControl();
    applyIrrigationControl();
    applyVentilationControl();
}

void LogicManager::applyTemperatureControl() {
    if (!temperatureControl->isEnabled()) return;
    
    bool heatingNeeded = temperatureControl->isHeatingActive();
    bool coolingNeeded = temperatureControl->isCoolingActive();
    
    // Control real de calefactor
    if (heatingNeeded) {
        actuatorManager->getHeater()->turnOn();
    } else {
        actuatorManager->getHeater()->turnOff();
    }
    
    // Control de ventilación para refrigeración
    if (coolingNeeded) {
        actuatorManager->getFan()->turnOn();
        actuatorManager->getServo()->openVent();
    }
}

void LogicManager::applyHumidityControl() {
    if (!humidityControl->isEnabled()) return;
    
    bool humidifyingNeeded = humidityControl->isHumidifyingActive();
    bool dehumidifyingNeeded = humidityControl->isDehumidifyingActive();
    bool ventilationNeeded = humidityControl->isVentilationActive();
    
    // Control de deshumidificación mediante ventilación
    if (dehumidifyingNeeded || ventilationNeeded) {
        actuatorManager->getFan()->turnOn();
        actuatorManager->getServo()->openVent();
    }
    
    // Control de humidificación (si hay sistema de nebulización)
    if (humidifyingNeeded) {
        // Implementar cuando se agregue sistema de nebulización
    }
}

void LogicManager::applyLightControl() {
    if (!lightControl->isEnabled()) return;
    
    float ledIntensity = lightControl->getLEDIntensity();
    bool lightNeeded = lightControl->isArtificialLightActive();
    
    // Control real de tira LED
    if (lightNeeded) {
        actuatorManager->getLEDStrip()->turnOn();
        if (actuatorManager->getLEDStrip()->supportsBrightness()) {
            actuatorManager->getLEDStrip()->setBrightness(ledIntensity);
        }
    } else {
        actuatorManager->getLEDStrip()->turnOff();
    }
}

void LogicManager::applyIrrigationControl() {
    if (!irrigationControl->isEnabled()) return;
    
    bool irrigationNeeded = irrigationControl->needsIrrigation();
    bool irrigationActive = irrigationControl->isIrrigationActive();
    
    // Control real de bomba de agua
    if (irrigationNeeded && !irrigationActive) {
        unsigned int duration = irrigationControl->calculateOptimalDuration();
        actuatorManager->getWaterPump()->turnOn();
        // Iniciar ciclo de riego sin el método inexistente
        irrigationControl->startIrrigation();
        Serial.println(String("[LogicManager] Iniciando riego por ") + duration + " segundos");
    } else if (!irrigationNeeded && irrigationActive) {
        actuatorManager->getWaterPump()->turnOff();
        irrigationControl->stopIrrigation();
    }
}

void LogicManager::applyVentilationControl() {
    if (!ventilationControl->isEnabled()) return;
    
    uint8_t fanSpeed = ventilationControl->getFanSpeed();
    uint8_t servoPosition = ventilationControl->getServoPosition();
    bool damperOpen = ventilationControl->isDamperOpen();
    
    // Control real de ventilador y servo
    if (fanSpeed > 0) {
        actuatorManager->getFan()->turnOn();
        // Si el ventilador soporta control de velocidad PWM
        // actuatorManager->getFan()->setSpeed(fanSpeed);
    } else {
        actuatorManager->getFan()->turnOff();
    }
    
    // Control de posición del servo usando método correcto
    if (servoPosition > 90) {
        actuatorManager->getServo()->openVent();
    } else if (servoPosition < 10) {
        actuatorManager->getServo()->closeVent();
    } else {
        // Para posiciones intermedias, usar el método moveToPosition si existe
        // actuatorManager->getServo()->moveToPosition(servoPosition);
        Serial.println(String("[LogicManager] Posición servo: ") + servoPosition + "°");
    }
}

// Control modes
// cppcheck-suppress unusedFunction
void LogicManager::setAutoMode(bool enabled) {
    autoMode = enabled;
    Serial.println(String("[LogicManager] Auto mode: ") + (enabled ? "ENABLED" : "DISABLED"));
}

// cppcheck-suppress unusedFunction
bool LogicManager::isAutoMode() const {
    return autoMode;
}

// cppcheck-suppress unusedFunction
void LogicManager::setSystemEnabled(bool enabled) {
    systemEnabled = enabled;
    Serial.println(String("[LogicManager] System: ") + (enabled ? "ENABLED" : "DISABLED"));
}

// cppcheck-suppress unusedFunction
bool LogicManager::isSystemEnabled() const {
    return systemEnabled;
}

// Target settings from Blynk
// cppcheck-suppress unusedFunction
void LogicManager::setTemperatureTarget(float target) {
    if (temperatureControl) {
        temperatureControl->setTarget(target);
        Serial.println(String("[LogicManager] Temperature target set to: ") + target + "°C");
    }
}

// cppcheck-suppress unusedFunction
void LogicManager::setHumidityTarget(float target) {
    if (humidityControl) {
        humidityControl->setTarget(target);
        Serial.println(String("[LogicManager] Humidity target set to: ") + target + "%");
    }
}

// cppcheck-suppress unusedFunction
void LogicManager::setLightTarget(float target) {
    if (lightControl) {
        lightControl->setTarget(target);
        Serial.println(String("[LogicManager] Light target set to: ") + target + " lux");
    }
}

// cppcheck-suppress unusedFunction
void LogicManager::setSoilMoistureTarget(float target) {
    if (irrigationControl) {
        irrigationControl->setTarget(target);
        Serial.println(String("[LogicManager] Soil moisture target set to: ") + target + "%");
    }
}

// Get current targets
// cppcheck-suppress unusedFunction
float LogicManager::getTemperatureTarget() const {
    return temperatureControl ? temperatureControl->getTarget() : 0.0;
}

// cppcheck-suppress unusedFunction
float LogicManager::getHumidityTarget() const {
    return humidityControl ? humidityControl->getTarget() : 0.0;
}

// cppcheck-suppress unusedFunction
float LogicManager::getLightTarget() const {
    return lightControl ? lightControl->getTarget() : 0.0;
}

// cppcheck-suppress unusedFunction
float LogicManager::getSoilMoistureTarget() const {
    return irrigationControl ? irrigationControl->getTarget() : 0.0;
}

// Status
// cppcheck-suppress unusedFunction
void LogicManager::getSystemStatus(String& status) {
    status = "System: " + String(systemEnabled ? "ON" : "OFF");
    status += " | Auto: " + String(autoMode ? "ON" : "OFF");
    
    if (temperatureControl) {
        status += " | Temp: " + temperatureControl->getStatusString();
    }
    if (humidityControl) {
        status += " | Hum: " + humidityControl->getStatusString();
    }
    if (lightControl) {
        status += " | Light: " + lightControl->getStatusString();
    }
    if (irrigationControl) {
        status += " | Irrigation: " + irrigationControl->getStatusString();
    }
    if (ventilationControl) {
        status += " | Ventilation: " + ventilationControl->getStatusString();
    }
}

void LogicManager::sendStatusToBlynk() {
    if (!blynkManager) return;
    
    // Send current targets and status to Blynk virtual pins
    // TODO: Implement specific virtual pin assignments
    
    // Example virtual pin assignments:
    // V20 - Temperature target
    // V21 - Humidity target  
    // V22 - Light target
    // V23 - Soil moisture target
    // V24 - System enabled status
    // V25 - Auto mode status
    
    blynkManager->sendVirtualPin(24, systemEnabled ? 1 : 0);
    blynkManager->sendVirtualPin(25, autoMode ? 1 : 0);
    
    if (temperatureControl) {
        blynkManager->sendVirtualPin(20, temperatureControl->getTarget());
    }
    if (humidityControl) {
        blynkManager->sendVirtualPin(21, humidityControl->getTarget());
    }
    if (lightControl) {
        blynkManager->sendVirtualPin(22, lightControl->getTarget());
    }
    if (irrigationControl) {
        blynkManager->sendVirtualPin(23, irrigationControl->getTarget());
    }
}

// Emergency handling
void LogicManager::handleEmergency() {
    Serial.println("[LogicManager] EMERGENCY: Taking protective actions");
    
    // Check individual control emergencies
    if (temperatureControl && temperatureControl->checkEmergency()) {
        // Emergency cooling
        ventilationControl->emergencyVentilation();
        Serial.println("[LogicManager] Emergency temperature control activated");
    }
    
    if (humidityControl && humidityControl->checkEmergency()) {
        // Emergency dehumidification
        ventilationControl->emergencyVentilation();
        Serial.println("[LogicManager] Emergency humidity control activated");
    }
    
    if (irrigationControl && irrigationControl->checkEmergency()) {
        // Emergency irrigation
        irrigationControl->emergencyIrrigation();
        Serial.println("[LogicManager] Emergency irrigation activated");
    }
    
    if (ventilationControl && ventilationControl->checkEmergency()) {
        // Maximum ventilation
        ventilationControl->emergencyVentilation();
        Serial.println("[LogicManager] Emergency ventilation activated");
    }
}

bool LogicManager::checkAlerts() {
    bool alertDetected = false;
    
    if (temperatureControl && temperatureControl->checkEmergency()) {
        alertDetected = true;
    }
    
    if (humidityControl && humidityControl->checkEmergency()) {
        alertDetected = true;
    }
    
    if (irrigationControl && irrigationControl->checkEmergency()) {
        alertDetected = true;
    }
    
    if (ventilationControl && ventilationControl->checkEmergency()) {
        alertDetected = true;
    }
    
    return alertDetected;
}
