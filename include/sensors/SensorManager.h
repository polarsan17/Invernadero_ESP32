#pragma once

#include "sensors/DHT22Sensor.h"
#include "sensors/AS7341Sensor.h"
#include "sensors/SoilMoistureSensor.h"
#include "sensors/BH1750Sensor.h"
#include "sensors/HCSR04Sensor.h"
#include "sensors/RS485SoilSensor.h"
#include "blynk/BlynkManager.h"

class SensorManager {
private:
    DHT22Sensor* dht22Sensor;
    AS7341Sensor* as7341Sensor;
    SoilMoistureSensor* soilMoistureSensor;
    BH1750Sensor* bh1750Sensor;
    HCSR04Sensor* hcsr04Sensor;
    RS485SoilSensor* rs485SoilSensor;
    BlynkManager* blynkManager;
    
    // Control de envío de datos
    unsigned long lastBlynkUpdate;
    unsigned long blynkUpdateInterval;
    
    bool sensorsInitialized;
    
public:
    explicit SensorManager(BlynkManager& blynk);
    ~SensorManager();
    
    // Deshabilitar copia y asignación para manejo seguro de memoria dinámica
    SensorManager(const SensorManager&) = delete;
    SensorManager& operator=(const SensorManager&) = delete;
    
    // Inicialización
    bool begin();
    
    // Actualización periódica
    void update();
    
    // Gestión de sensores
    bool readAllSensors();
    void sendDataToBlynk();
    
    // Estado
    bool areSensorsReady();
    
    // Getters para acceder a los datos DHT22
    float getTemperature();
    float getHumidity();
    float getHeatIndex();
    
    // Getters para acceder a los datos AS7341
    float getLux();
    float getColorTemperature();
    float getRedLight();
    float getGreenLight(); 
    float getBlueLight();
    float getClearLight();
    float getNIRLight();
    
    // Getters para acceder a los datos del sensor de humedad del suelo
    float getSoilMoisture();
    uint16_t getSoilRawValue();
    String getSoilMoistureLevel();
    
    // Getters para acceder a los datos del sensor BH1750
    float getLightLux();
    String getLightLevel();
    
    // Getters para acceder a los datos del sensor HC-SR04
    float getWaterLevel();
    float getWaterPercentage();
    String getWaterStatus();
    
    // Getters para acceder a los datos del sensor RS485 de suelo
    float getSoilTemperature();
    float getSoilMoistureRS485();
    float getSoilEC();
    float getSoilPH();
    uint16_t getSoilNitrogen();
    uint16_t getSoilPhosphorus();
    uint16_t getSoilPotassium();
    String getSoilStatus();
    String getNutrientStatus();
    
    // Configuración
    void setBlynkUpdateInterval(unsigned long interval);
    
    // Utilidades
    void printAllSensorData();
    
private:
    bool shouldUpdateBlynk();
};
