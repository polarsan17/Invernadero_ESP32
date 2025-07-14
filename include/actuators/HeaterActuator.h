#pragma once

#include <Arduino.h>

class HeaterActuator {
private:
    uint8_t relayPin;
    
    // Estado del calefactor
    bool isOn;
    bool isInitialized;
    
    // Control de estados
    unsigned long lastStateChange;
    unsigned long minStateChangeInterval;
    
    // Control de seguridad y ciclos
    unsigned long maxContinuousRunTime;
    unsigned long continuousRunStartTime;
    bool isContinuousRunning;
    
    // Control de temperatura de seguridad
    float maxSafeTemperature;
    bool temperatureSafetyEnabled;
    
    // Estadísticas de uso
    unsigned long totalRunTime;
    unsigned int activationCount;
    
public:
    explicit HeaterActuator(uint8_t relayPin);
    ~HeaterActuator();
    
    // Inicialización
    bool begin();
    
    // Control del calefactor
    bool turnOn();
    bool turnOff();
    bool toggle();
    
    // Estado
    bool isRunning();
    bool isReady();
    
    // Configuración
    void setMinStateChangeInterval(unsigned long interval);
    void setMaxContinuousRunTime(unsigned long maxTime);
    void setMaxSafeTemperature(float maxTemp);
    void enableTemperatureSafety(bool enable);
    
    // Control de seguridad
    bool checkTemperatureSafety(float currentTemperature);
    bool emergencyShutdown();
    
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
