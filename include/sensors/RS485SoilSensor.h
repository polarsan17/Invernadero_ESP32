#ifndef RS485_SOIL_SENSOR_H
#define RS485_SOIL_SENSOR_H

#include <Arduino.h>
#include "config/config.h"

#define RS485_SAMPLES_COUNT 3  // Número de muestras para promediado (sensor industrial)

class RS485SoilSensor {
private:
    // Configuración RS485
    HardwareSerial* rs485Serial;
    uint8_t txEnablePin;
    uint8_t deviceAddress;
    uint32_t baudRate;
    
    // Variables de medición
    float temperature;           // Temperatura del suelo en °C
    float moisture;             // Humedad del suelo en %
    float electricalConductivity; // Conductividad eléctrica en uS/cm
    float pH;                   // pH del suelo
    uint16_t nitrogen;          // Nitrógeno en mg/kg
    uint16_t phosphorus;        // Fósforo en mg/kg  
    uint16_t potassium;         // Potasio en mg/kg
    
    // Arrays para promediado
    float temperatureSamples[RS485_SAMPLES_COUNT];
    float moistureSamples[RS485_SAMPLES_COUNT];
    float ecSamples[RS485_SAMPLES_COUNT];
    float phSamples[RS485_SAMPLES_COUNT];
    uint16_t nSamples[RS485_SAMPLES_COUNT];
    uint16_t pSamples[RS485_SAMPLES_COUNT];
    uint16_t kSamples[RS485_SAMPLES_COUNT];
    int currentSample;
    
    // Control de tiempo
    unsigned long lastReading;
    unsigned long readInterval;
    
    // Estados
    bool isInitialized;
    bool lastReadValid;
    
    // Buffer de comunicación
    uint8_t requestFrame[8];
    uint8_t responseBuffer[20];
    
    // Métodos privados
    bool sendRequest();
    bool receiveResponse(int expectedBytes);
    uint16_t calculateCRC16(const uint8_t* data, uint8_t length);
    bool validateCRC(uint8_t* data, uint8_t length);
    void parseResponse7in1(uint8_t* data);
    void parseResponse4in1(uint8_t* data);
    void parseResponse3in1(uint8_t* data);
    void resetSamples();
    float calculateAverageFloat(const float samples[], int count);
    uint16_t calculateAverageUint16(const uint16_t samples[], int count);
    void enableTransmission();
    void disableTransmission();
    bool isValidReading();

public:
    // Constructor
    RS485SoilSensor();
    RS485SoilSensor(HardwareSerial* serial, uint8_t txPin, uint8_t deviceAddr = 0x01);
    ~RS485SoilSensor();
    
    // Inicialización
    bool begin();
    bool begin(uint32_t baud);
    
    // Lectura de datos
    bool readSensor();
    bool readSensor7in1();  // Para sensor 7-en-1 (Temp, Hum, EC, pH, N, P, K)
    bool readSensor4in1();  // Para sensor 4-en-1 (Temp, Hum, EC, pH)
    bool readSensor3in1();  // Para sensor 3-en-1 (Temp, Hum, EC)
    
    // Getters de datos
    float getTemperature();
    float getMoisture();
    float getElectricalConductivity();
    float getPH();
    uint16_t getNitrogen();
    uint16_t getPhosphorus();
    uint16_t getPotassium();
    
    // Estado del sensor
    bool isDataValid();
    bool isReady();
    String getSoilStatus();
    String getNutrientStatus();
    String getMoistureLevel();
    String getTemperatureStatus();
    
    // Análisis de suelo
    bool isSoilDry();
    bool isSoilOptimal();
    bool isSoilWet();
    bool isTemperatureOptimal();
    bool isPHOptimal();
    bool isECOptimal();
    bool areNutrientsOptimal();
    
    // Configuración
    void setReadInterval(unsigned long interval);
    void setDeviceAddress(uint8_t addr);
    void setBaudRate(uint32_t baud);
    void setSerial(HardwareSerial* serial, uint8_t txPin);
    
    // Utilidades
    unsigned long getTimeSinceLastReading();
    bool shouldRead();
    void printSoilData();
    void printNutrientData();
    void printAllData();
};

#endif
