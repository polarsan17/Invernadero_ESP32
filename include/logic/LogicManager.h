#ifndef LOGIC_MANAGER_H
#define LOGIC_MANAGER_H

#include <Arduino.h>
#include "TemperatureControl.h"
#include "HumidityControl.h"
#include "LightControl.h"
#include "IrrigationControl.h"
#include "VentilationControl.h"
#include "../sensors/SensorManager.h"
#include "../actuators/ActuatorManager.h"
#include "../blynk/BlynkManager.h"

class LogicManager {
private:
    TemperatureControl* temperatureControl;
    HumidityControl* humidityControl;
    LightControl* lightControl;
    IrrigationControl* irrigationControl;
    VentilationControl* ventilationControl;
    
    SensorManager* sensorManager;
    ActuatorManager* actuatorManager;
    BlynkManager* blynkManager;
    
    unsigned long lastUpdate;
    const unsigned long UPDATE_INTERVAL = 5000; // 5 segundos
    
    bool autoMode;
    bool systemEnabled;

public:
    LogicManager();
    ~LogicManager();
    
    bool begin(SensorManager* sensors, ActuatorManager* actuators, BlynkManager* blynk);
    void update();
    void processLogic();
    
    // Control modes
    void setAutoMode(bool enabled);
    bool isAutoMode() const;
    void setSystemEnabled(bool enabled);
    bool isSystemEnabled() const;
    
    // Target settings from Blynk
    void setTemperatureTarget(float target);
    void setHumidityTarget(float target);
    void setLightTarget(float target);
    void setSoilMoistureTarget(float target);
    
    // Get current targets
    float getTemperatureTarget() const;
    float getHumidityTarget() const;
    float getLightTarget() const;
    float getSoilMoistureTarget() const;
    
    // Status
    void getSystemStatus(String& status);
    void sendStatusToBlynk();
    
    // Emergency handling
    void handleEmergency();
    bool checkAlerts();
    
private:
    // Apply control actions to actuators
    void applyTemperatureControl();
    void applyHumidityControl();
    void applyLightControl();
    void applyIrrigationControl();
    void applyVentilationControl();
};

#endif
