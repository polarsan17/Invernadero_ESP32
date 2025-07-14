#ifndef SOILMOISTURE_SENSOR_H
#define SOILMOISTURE_SENSOR_H

#include <Arduino.h>
#include "config/config.h"

#define SAMPLES_COUNT 5  // Número de muestras para promediado

class SoilMoistureSensor {
private:
    // Variables de lectura
    uint16_t rawValue;
    float moisturePercentage;
    uint16_t moistureSamples[SAMPLES_COUNT];
    int currentSample;
    
    // Control de tiempo
    unsigned long lastReading;
    unsigned long readInterval;
    
    // Estados
    bool isInitialized;
    bool lastReadValid;
    
    // Calibración
    uint16_t dryValue;    // Valor cuando está seco (aire)
    uint16_t wetValue;    // Valor cuando está húmedo (agua)
    
    // Métodos privados
    float calculateAverage(uint16_t samples[], int count);
    bool isValidReading(uint16_t value);
    void resetSamples();
    uint16_t readRawValue();
    float convertToPercentage(uint16_t rawValue);

public:
    // Constructor y destructor
    SoilMoistureSensor();
    ~SoilMoistureSensor();
    
    // Inicialización
    bool begin();
    bool begin(uint16_t dryCalibration, uint16_t wetCalibration);
    
    // Lectura del sensor
    bool readSensor();
    bool isDataValid();
    
    // Getters
    float getMoisturePercentage();
    uint16_t getRawValue();
    
    // Análisis de humedad
    String getMoistureLevel();  // "Seco", "Óptimo", "Húmedo"
    bool isDry();               // < 30%
    bool isOptimal();           // 30-70%
    bool isWet();               // > 70%
    
    // Configuración
    void setReadInterval(unsigned long interval);
    void setCalibration(uint16_t dryValue, uint16_t wetValue);
    void calibrateDry();        // Calibrar en aire seco
    void calibrateWet();        // Calibrar en agua
    
    // Control
    bool isReady();
    unsigned long getTimeSinceLastReading();
    bool shouldRead();
    
    // Debug
    void printMoistureData();
};

#endif
