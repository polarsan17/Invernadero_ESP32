#ifndef HCSR04_SENSOR_H
#define HCSR04_SENSOR_H

#include <Arduino.h>
#include "config/config.h"

#define HCSR04_SAMPLES_COUNT 3  // Número de muestras para promediado (menos para sensor más rápido)

class HCSR04Sensor {
private:
    // Pines del sensor
    uint8_t triggerPin;
    uint8_t echoPin;
    
    // Variables de medición
    float distanceCm;
    float waterLevelCm;
    float waterLevelPercentage;
    float distanceSamples[HCSR04_SAMPLES_COUNT];
    int currentSample;
    
    // Configuración del tanque
    float tankHeightCm;      // Altura total del tanque
    float sensorHeightCm;    // Altura donde está montado el sensor desde el fondo
    float minWaterLevelCm;   // Nivel mínimo de agua (sensor a superficie del agua)
    float maxWaterLevelCm;   // Nivel máximo de agua 
    
    // Control de tiempo
    unsigned long lastReading;
    unsigned long readInterval;
    
    // Estados
    bool isInitialized;
    bool lastReadValid;
    
    // Métodos privados
    float calculateAverage(const float samples[], int count);
    bool isValidReading(float distance);
    void resetSamples();
    float measureDistance();
    float calculateWaterLevel(float distance);
    float calculateWaterPercentage(float waterLevel);

public:
    // Constructor y destructor
    HCSR04Sensor();
    HCSR04Sensor(uint8_t trigPin, uint8_t echoPin);
    ~HCSR04Sensor();
    
    // Inicialización
    bool begin();
    bool begin(float tankHeight, float sensorHeight);
    
    // Configuración del tanque
    void setTankDimensions(float tankHeight, float sensorHeight);
    void setWaterLevelLimits(float minLevel, float maxLevel);
    
    // Lectura del sensor
    bool readSensor();
    bool isDataValid();
    
    // Getters de distancia
    float getDistance();        // Distancia medida por el sensor
    float getDistanceRaw();     // Distancia sin promediado
    
    // Getters de nivel de agua
    float getWaterLevel();      // Nivel de agua en cm
    float getWaterPercentage(); // Porcentaje de llenado del tanque
    
    // Análisis del nivel de agua
    String getWaterStatus();    // "Vacío", "Bajo", "Medio", "Alto", "Lleno"
    bool isEmpty();             // < 10%
    bool isLow();               // 10-30%
    bool isMedium();            // 30-70%
    bool isHigh();              // 70-90%
    bool isFull();              // > 90%
    
    // Configuración
    void setReadInterval(unsigned long interval);
    void setPins(uint8_t trigPin, uint8_t echoPin);
    
    // Control
    bool isReady();
    unsigned long getTimeSinceLastReading();
    bool shouldRead();
    
    // Calibración y configuración
    void calibrateEmpty();     // Calibrar cuando el tanque está vacío
    void calibrateFull();      // Calibrar cuando el tanque está lleno
    
    // Debug
    void printWaterLevelData();
    bool testSensor();         // Verificar funcionamiento del sensor
};

#endif
