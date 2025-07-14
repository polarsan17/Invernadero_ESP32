#ifndef BH1750_SENSOR_H
#define BH1750_SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include "config/config.h"

// Direcciones I2C del BH1750
#define BH1750_DEFAULT_ADDR  0x23    // Dirección por defecto (ADDR pin LOW)
#define BH1750_ALT_ADDR      0x5C    // Dirección alternativa (ADDR pin HIGH)

// Comandos del BH1750
#define BH1750_POWER_DOWN    0x00    // Apagar sensor
#define BH1750_POWER_ON      0x01    // Encender sensor
#define BH1750_RESET         0x07    // Reset sensor

// Modos de medición
#define BH1750_CONT_HIGH_RES 0x10    // Medición continua, alta resolución (1 lx)
#define BH1750_CONT_HIGH_RES2 0x11   // Medición continua, alta resolución 2 (0.5 lx)
#define BH1750_CONT_LOW_RES  0x13    // Medición continua, baja resolución (4 lx)
#define BH1750_ONE_HIGH_RES  0x20    // Una medición, alta resolución (1 lx)
#define BH1750_ONE_HIGH_RES2 0x21    // Una medición, alta resolución 2 (0.5 lx)
#define BH1750_ONE_LOW_RES   0x23    // Una medición, baja resolución (4 lx)

#define BH1750_SAMPLES_COUNT 5  // Número de muestras para promediado

class BH1750Sensor {
private:
    // Variables de configuración
    uint8_t deviceAddress;
    uint8_t currentMode;
    
    // Variables de lectura
    float luxValue;
    float luxSamples[BH1750_SAMPLES_COUNT];
    int currentSample;
    
    // Control de tiempo
    unsigned long lastReading;
    unsigned long readInterval;
    
    // Estados
    bool isInitialized;
    bool lastReadValid;
    
    // Métodos privados
    float calculateAverage(const float samples[], int count);
    bool isValidReading(float value);
    void resetSamples();
    bool writeCommand(uint8_t command);
    uint16_t readRawValue();
    float convertToLux(uint16_t rawValue);
    unsigned long getMeasurementTime(uint8_t mode);

public:
    // Constructor y destructor
    BH1750Sensor();
    explicit BH1750Sensor(uint8_t address);
    ~BH1750Sensor();
    
    // Inicialización
    bool begin();
    bool begin(uint8_t mode);
    
    // Lectura del sensor
    bool readSensor();
    bool isDataValid();
    
    // Getters
    float getLux();
    float getLuxRaw();
    
    // Análisis de luz
    String getLightLevel();     // "Oscuro", "Tenue", "Brillante", "Muy brillante"
    bool isDark();              // < 10 lx
    bool isDim();               // 10-200 lx
    bool isBright();            // 200-1000 lx
    bool isVeryBright();        // > 1000 lx
    
    // Configuración
    void setReadInterval(unsigned long interval);
    bool setMode(uint8_t mode);
    bool setAddress(uint8_t address);
    
    // Control
    bool isReady();
    bool powerOn();
    bool powerDown();
    bool reset();
    unsigned long getTimeSinceLastReading();
    bool shouldRead();
    
    // Debug
    void printLightData();
    bool checkConnection();
};

#endif
