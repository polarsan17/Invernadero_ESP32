#pragma once

#include <Arduino.h>

class WaterPumpActuator {
private:
    uint8_t relayPin;
    
    // Estado de la bomba
    bool isOn;
    bool isInitialized;
    
    // Control de estados
    unsigned long lastStateChange;
    unsigned long minStateChangeInterval;
    
    // Control de seguridad y ciclos
    unsigned long maxContinuousRunTime;
    unsigned long continuousRunStartTime;
    bool isContinuousRunning;
    
    // Estadísticas de uso
    unsigned long totalRunTime;
    unsigned int activationCount;
    
public:
    explicit WaterPumpActuator(uint8_t relayPin);
    ~WaterPumpActuator();
    
    // Inicialización
    bool begin();
    
    // Control de la bomba
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
    
    // Estadísticas
    unsigned long getTotalRunTime();
    unsigned int getActivationCount();
    void resetStatistics();
    
    // Funciones de utilidad
    void printStatus();
    
    // Integración con Blynk
    int getBlynkState();
    bool setFromBlynk(int state);
};
