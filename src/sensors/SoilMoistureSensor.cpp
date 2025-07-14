#include "sensors/SoilMoistureSensor.h"
#include "config/config.h"
#include <Arduino.h>

SoilMoistureSensor::SoilMoistureSensor() 
    : rawValue(0)
    , moisturePercentage(0.0)
    , currentSample(0)
    , lastReading(0)
    , readInterval(SOIL_MOISTURE_READ_INTERVAL)
    , isInitialized(false)
    , lastReadValid(false)
    , dryValue(3000)    // Valor típico en aire (seco)
    , wetValue(1200)    // Valor típico en agua (húmedo)
{
    resetSamples();
}

SoilMoistureSensor::~SoilMoistureSensor() {
    // Cleanup if needed
}

bool SoilMoistureSensor::begin() {
    // Configurar pin como entrada analógica
    pinMode(SOIL_MOISTURE_PIN, INPUT);
    
    // Verificar que el pin funciona
    uint16_t testRead = analogRead(SOIL_MOISTURE_PIN);
    if (testRead == 0 || testRead == 4095) {
        // Lectura sospechosa, pero continuamos
        Serial.println("Advertencia: Lectura inicial del sensor de humedad fuera de rango normal");
    }
    
    isInitialized = true;
    
    // Hacer primera lectura para inicializar
    readSensor();
    
    return true;
}

bool SoilMoistureSensor::begin(uint16_t dryCalibration, uint16_t wetCalibration) {
    setCalibration(dryCalibration, wetCalibration);
    return begin();
}

bool SoilMoistureSensor::readSensor() {
    if (!isInitialized) {
        return false;
    }
    
    // Leer valor raw del sensor
    uint16_t newReading = readRawValue();
    
    // Verificar si la lectura es válida
    if (isValidReading(newReading)) {
        // Guardar muestra para promediado
        moistureSamples[currentSample] = newReading;
        currentSample = (currentSample + 1) % SAMPLES_COUNT;
        
        // Calcular promedio
        rawValue = (uint16_t)calculateAverage(moistureSamples, SAMPLES_COUNT);
        
        // Convertir a porcentaje
        moisturePercentage = convertToPercentage(rawValue);
        
        lastReading = millis();
        lastReadValid = true;
        
        return true;
    } else {
        lastReadValid = false;
        return false;
    }
}

bool SoilMoistureSensor::isDataValid() {
    return lastReadValid && isInitialized;
}

// cppcheck-suppress unusedFunction
float SoilMoistureSensor::getMoisturePercentage() {
    return moisturePercentage;
}

uint16_t SoilMoistureSensor::getRawValue() {
    return rawValue;
}

String SoilMoistureSensor::getMoistureLevel() {
    if (!isDataValid()) {
        return "Sin datos";
    }
    
    if (isDry()) {
        return "Seco";
    } else if (isWet()) {
        return "Húmedo";
    } else {
        return "Óptimo";
    }
}

bool SoilMoistureSensor::isDry() {
    return isDataValid() && moisturePercentage < 30.0;
}

// cppcheck-suppress unusedFunction
bool SoilMoistureSensor::isOptimal() {
    return isDataValid() && moisturePercentage >= 30.0 && moisturePercentage <= 70.0;
}

bool SoilMoistureSensor::isWet() {
    return isDataValid() && moisturePercentage > 70.0;
}

// cppcheck-suppress unusedFunction
void SoilMoistureSensor::setReadInterval(unsigned long interval) {
    readInterval = interval;
}

void SoilMoistureSensor::setCalibration(uint16_t dryValue, uint16_t wetValue) {
    this->dryValue = dryValue;
    this->wetValue = wetValue;
}

// cppcheck-suppress unusedFunction
void SoilMoistureSensor::calibrateDry() {
    if (isInitialized) {
        // Leer valor actual como punto seco
        uint16_t reading = readRawValue();
        dryValue = reading;
        Serial.printf("Calibración seco: %d\\n", dryValue);
    }
}

// cppcheck-suppress unusedFunction
void SoilMoistureSensor::calibrateWet() {
    if (isInitialized) {
        // Leer valor actual como punto húmedo
        uint16_t reading = readRawValue();
        wetValue = reading;
        Serial.printf("Calibración húmedo: %d\\n", wetValue);
    }
}

// cppcheck-suppress unusedFunction
bool SoilMoistureSensor::isReady() {
    return isInitialized;
}

unsigned long SoilMoistureSensor::getTimeSinceLastReading() {
    if (lastReading == 0) {
        return ULONG_MAX;
    }
    return millis() - lastReading;
}

// cppcheck-suppress unusedFunction
bool SoilMoistureSensor::shouldRead() {
    return getTimeSinceLastReading() >= readInterval;
}

// cppcheck-suppress unusedFunction
void SoilMoistureSensor::printMoistureData() {
    if (!isDataValid()) {
        Serial.println("Sensor humedad suelo: Datos no válidos");
        return;
    }
    
    Serial.println("=== Sensor Humedad del Suelo ===");
    Serial.printf("Valor raw: %d\\n", rawValue);
    Serial.printf("Humedad: %.1f%%\\n", moisturePercentage);
    Serial.printf("Estado: %s\\n", getMoistureLevel().c_str());
    Serial.printf("Calibración - Seco: %d, Húmedo: %d\\n", dryValue, wetValue);
}

float SoilMoistureSensor::calculateAverage(uint16_t samples[], int count) {
    uint32_t sum = 0;
    int validSamples = 0;
    
    for (int i = 0; i < count; i++) {
        if (samples[i] > 0) {  // Asumimos que 0 es valor no válido
            sum += samples[i];
            validSamples++;
        }
    }
    
    if (validSamples > 0) {
        return (float)sum / validSamples;
    }
    return 0.0;
}

bool SoilMoistureSensor::isValidReading(uint16_t value) {
    // Verificar que el valor esté en rango razonable
    // ADC ESP32: 0-4095 (12 bits)
    return (value > 50 && value < 4000);  // Excluir extremos que suelen ser errores
}

void SoilMoistureSensor::resetSamples() {
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        moistureSamples[i] = 0;
    }
    currentSample = 0;
}

uint16_t SoilMoistureSensor::readRawValue() {
    // Hacer múltiples lecturas para estabilidad
    uint32_t sum = 0;
    const int numReadings = 3;
    
    for (int i = 0; i < numReadings; i++) {
        sum += analogRead(SOIL_MOISTURE_PIN);
        delay(10);  // Pequeña pausa entre lecturas
    }
    
    return sum / numReadings;
}

float SoilMoistureSensor::convertToPercentage(uint16_t rawValue) {
    // Invertir la lógica: valores altos = seco, valores bajos = húmedo
    if (dryValue <= wetValue) {
        // Calibración incorrecta, usar valores por defecto
        dryValue = 3000;
        wetValue = 1200;
    }
    
    // Limitar valor dentro del rango calibrado
    if (rawValue >= dryValue) {
        return 0.0;  // Completamente seco
    }
    if (rawValue <= wetValue) {
        return 100.0;  // Completamente húmedo
    }
    
    // Calcular porcentaje (invertido)
    float percentage = 100.0 - ((float)(rawValue - wetValue) / (float)(dryValue - wetValue)) * 100.0;
    
    // Asegurar que esté en rango 0-100
    if (percentage < 0.0) percentage = 0.0;
    if (percentage > 100.0) percentage = 100.0;
    
    return percentage;
}
