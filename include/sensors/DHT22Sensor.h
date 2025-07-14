#pragma once

#include <DHT.h>

class DHT22Sensor {
private:
    DHT* dht;
    uint8_t pin;
    
    // Valores de lectura
    float temperature;
    float humidity;
    float heatIndex;
    
    // Control de lecturas
    unsigned long lastReading;
    unsigned long readInterval;
    bool isInitialized;
    bool lastReadValid;
    
    // Estadísticas para mejorar la precisión
    static const int SAMPLES_COUNT = 3;
    float tempSamples[SAMPLES_COUNT];
    float humSamples[SAMPLES_COUNT];
    int currentSample;
    
public:
    explicit DHT22Sensor(uint8_t dataPin);
    ~DHT22Sensor();
    
    // Deshabilitar copia y asignación para manejo seguro de memoria dinámica
    DHT22Sensor(const DHT22Sensor&) = delete;
    DHT22Sensor& operator=(const DHT22Sensor&) = delete;
    
    // Inicialización
    bool begin();
    
    // Lectura de datos
    bool readSensor();
    bool isDataValid();
    
    // Getters
    float getTemperature();
    float getHumidity(); 
    float getHeatIndex();
    
    // Configuración
    void setReadInterval(unsigned long interval);
    
    // Estado del sensor
    bool isReady();
    unsigned long getTimeSinceLastReading();
    
    // Funciones de utilidad
    bool shouldRead();
    void resetSamples();
    
private:
    float calculateAverage(const float samples[], int count);
    bool isValidReading(float temp, float hum);
};
