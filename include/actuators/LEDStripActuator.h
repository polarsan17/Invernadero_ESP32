#pragma once

#include <Arduino.h>

class LEDStripActuator {
private:
    uint8_t relayPin;
    
    // Estado de la tira LED
    bool isOn;
    bool isInitialized;
    
    // Control de estados
    unsigned long lastStateChange;
    unsigned long minStateChangeInterval;
    
    // Control de ciclos de iluminación
    unsigned long maxContinuousRunTime;
    unsigned long continuousRunStartTime;
    bool isContinuousRunning;
    
    // Configuración de intensidad (si se usa PWM)
    uint8_t brightness;
    bool supportsPWM;
    uint8_t pwmChannel;
    
    // Estadísticas de uso
    unsigned long totalRunTime;
    unsigned int activationCount;
    
public:
    explicit LEDStripActuator(uint8_t relayPin, bool enablePWM = false, uint8_t pwmCh = 0);
    ~LEDStripActuator();
    
    // Inicialización
    bool begin();
    
    // Control de la tira LED
    bool turnOn();
    bool turnOff();
    bool toggle();
    
    // Control de intensidad (solo si PWM está habilitado)
    bool setBrightness(uint8_t brightness);
    uint8_t getBrightness();
    bool fadeIn(unsigned long duration = 1000);
    bool fadeOut(unsigned long duration = 1000);
    
    // Estado
    bool isRunning();
    bool isReady();
    bool supportsBrightness();
    
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
    bool setBrightnessFromBlynk(int brightness);
};
