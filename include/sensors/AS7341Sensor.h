#pragma once

#include <Wire.h>

// Registros AS7341
#define AS7341_ADDR 0x39
#define AS7341_ASTATUS 0x60
#define AS7341_CH0_DATA_L 0x61
#define AS7341_CH0_DATA_H 0x62
#define AS7341_ITIME_L 0x63
#define AS7341_ITIME_M 0x64
#define AS7341_ITIME_H 0x65
#define AS7341_CONFIG 0x70
#define AS7341_STAT 0x71
#define AS7341_EDGE 0x72
#define AS7341_GPIO 0x73
#define AS7341_LED 0x74
#define AS7341_ENABLE 0x80
#define AS7341_ATIME 0x81
#define AS7341_WTIME 0x83
#define AS7341_SP_TH_L_LSB 0x84
#define AS7341_SP_TH_L_MSB 0x85
#define AS7341_SP_TH_H_LSB 0x86
#define AS7341_SP_TH_H_MSB 0x87
#define AS7341_AUXID 0x90
#define AS7341_REVID 0x91
#define AS7341_ID 0x92
#define AS7341_STATUS 0x93
#define AS7341_ASTATUS 0x60
#define AS7341_CFG0 0xA9
#define AS7341_CFG1 0xAA
#define AS7341_CFG3 0xAC
#define AS7341_CFG6 0xAF
#define AS7341_CFG8 0xB1
#define AS7341_CFG9 0xB2
#define AS7341_CFG10 0xB3
#define AS7341_CFG12 0xB5
#define AS7341_PERS 0xBD
#define AS7341_GPIO2 0xBE
#define AS7341_ASTEP_L 0xCA
#define AS7341_ASTEP_H 0xCB

class AS7341Sensor {
private:
    // Valores de lectura espectral (11 canales)
    uint16_t spectralData[12]; // 12 canales incluido clear/NIR
    float violetReading;       // Canal 415nm
    float blueReading;         // Canal 445nm  
    float cyanReading;         // Canal 480nm
    float greenReading;        // Canal 515nm
    float yellowReading;       // Canal 555nm
    float orangeReading;       // Canal 590nm
    float redReading;          // Canal 630nm
    float nearIRReading;       // Canal 680nm
    float clearReading;        // Canal Clear
    float nirReading;          // Canal NIR
    
    // Control de lecturas
    unsigned long lastReading;
    unsigned long readInterval;
    bool isInitialized;
    bool lastReadValid;
    
    // Configuración del sensor
    uint8_t currentGain;
    uint16_t currentIntegrationTime;
    
    // Estadísticas para filtrado
    static const int SAMPLES_COUNT = 3;
    uint16_t spectralSamples[12][SAMPLES_COUNT];
    int currentSample;
    
public:
    AS7341Sensor();
    ~AS7341Sensor();
    
    // Inicialización
    bool begin();
    
    // Lectura de datos
    bool readSensor();
    bool isDataValid();
    
    // Getters para canales específicos
    float getViolet();     // 415nm
    float getBlue();       // 445nm
    float getCyan();       // 480nm
    float getGreen();      // 515nm
    float getYellow();     // 555nm
    float getOrange();     // 590nm
    float getRed();        // 630nm
    float getNearIR();     // 680nm
    float getClear();      // Clear channel
    float getNIR();        // NIR channel
    
    // Funciones de análisis
    float getLux();
    float getColorTemperature();
    void getRGBValues(float& r, float& g, float& b);
    
    // Configuración
    void setReadInterval(unsigned long interval);
    bool setGain(uint8_t gain);
    bool setIntegrationTime(uint16_t time);
    
    // Estado del sensor
    bool isReady();
    unsigned long getTimeSinceLastReading();
    bool shouldRead();
    
    // Utilidades
    void resetSamples();
    void printSpectralData();
    
private:
    float calculateAverage(const uint16_t samples[], int count);
    bool isValidReading(uint16_t* data);
    void updateFloatValues();
    
    // Comunicación I2C
    bool writeRegister(uint8_t reg, uint8_t value);
    uint8_t readRegister(uint8_t reg);
    uint16_t readRegister16(uint8_t reg);
    bool readMultipleRegisters(uint8_t reg, uint8_t* buffer, uint8_t length);
    bool checkDevice();
};
