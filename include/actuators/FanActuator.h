#pragma once

#include <Arduino.h>

class FanActuator {
private:
    uint8_t relayPin;
    
    // Estado del ventilador
    bool isOn;
    bool isInitialized;
    
    // Control de estados
    unsigned long lastStateChange;
    unsigned long minStateChangeInterval;
    
    // Control de seguridad
    unsigned long maxContinuousRunTime;
    unsigned long continuousRunStartTime;
    bool isContinuousRunning;
    
public:
    explicit FanActuator(uint8_t relayPin);
    ~FanActuator();
    
    // Inicialización
    bool begin();
    
    // Control del ventilador
    bool turnOn();
    bool turnOff();
    bool toggle();
    
    // Estado
    bool isRunning();
    bool isReady();
    
    // Configuración
    void setMinStateChangeInterval(unsigned long interval);
    void setMaxContinuousRunTime(unsigned long maxTime);
    
    // Gestión de tiempo
    unsigned long getTimeSinceLastStateChange();
    unsigned long getContinuousRunTime();
    bool hasExceededMaxRunTime();
    
    // Funciones de utilidad
    void printStatus();
    
    // Integración con Blynk
    int getBlynkState();
    bool setFromBlynk(int state);
};
