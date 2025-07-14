#include "sensors/BH1750Sensor.h"
#include "config/config.h"
#include <Arduino.h>
#include <Wire.h>

BH1750Sensor::BH1750Sensor() 
    : deviceAddress(BH1750_DEFAULT_ADDR)
    , currentMode(BH1750_CONT_HIGH_RES)
    , luxValue(0.0)
    , currentSample(0)
    , lastReading(0)
    , readInterval(BH1750_READ_INTERVAL)
    , isInitialized(false)
    , lastReadValid(false)
{
    resetSamples();
}

BH1750Sensor::BH1750Sensor(uint8_t address) 
    : deviceAddress(address)
    , currentMode(BH1750_CONT_HIGH_RES)
    , luxValue(0.0)
    , currentSample(0)
    , lastReading(0)
    , readInterval(BH1750_READ_INTERVAL)
    , isInitialized(false)
    , lastReadValid(false)
{
    resetSamples();
}

BH1750Sensor::~BH1750Sensor() {
    if (isInitialized) {
        powerDown();
    }
}

bool BH1750Sensor::begin() {
    return begin(BH1750_CONT_HIGH_RES);
}

bool BH1750Sensor::begin(uint8_t mode) {
    // Inicializar I2C si no está inicializado
    Wire.begin(BH1750_SDA_PIN, BH1750_SCL_PIN);
    
    // Verificar conexión
    if (!checkConnection()) {
        Serial.println("BH1750: No se pudo conectar al sensor");
        return false;
    }
    
    // Encender sensor
    if (!powerOn()) {
        Serial.println("BH1750: Error al encender sensor");
        return false;
    }
    
    // Configurar modo
    if (!setMode(mode)) {
        Serial.println("BH1750: Error al configurar modo");
        return false;
    }
    
    // Esperar tiempo de estabilización
    delay(200);
    
    isInitialized = true;
    
    // Hacer primera lectura para inicializar
    readSensor();
    
    Serial.printf("BH1750: Inicializado en dirección 0x%02X, modo 0x%02X\\n", deviceAddress, currentMode);
    return true;
}

bool BH1750Sensor::readSensor() {
    if (!isInitialized) {
        return false;
    }
    
    // Leer valor raw del sensor
    uint16_t rawValue = readRawValue();
    
    if (rawValue == 0xFFFF) {
        // Error en la lectura
        lastReadValid = false;
        return false;
    }
    
    // Convertir a lux
    float newLux = convertToLux(rawValue);
    
    // Verificar si la lectura es válida
    if (isValidReading(newLux)) {
        // Guardar muestra para promediado
        luxSamples[currentSample] = newLux;
        currentSample = (currentSample + 1) % BH1750_SAMPLES_COUNT;
        
        // Calcular promedio
        luxValue = calculateAverage(luxSamples, BH1750_SAMPLES_COUNT);
        
        lastReading = millis();
        lastReadValid = true;
        
        return true;
    } else {
        lastReadValid = false;
        return false;
    }
}

bool BH1750Sensor::isDataValid() {
    return lastReadValid && isInitialized;
}

// cppcheck-suppress unusedFunction
float BH1750Sensor::getLux() {
    return luxValue;
}

// cppcheck-suppress unusedFunction
float BH1750Sensor::getLuxRaw() {
    if (!isInitialized) {
        return NAN;
    }
    
    uint16_t rawValue = readRawValue();
    if (rawValue == 0xFFFF) {
        return NAN;
    }
    
    return convertToLux(rawValue);
}

String BH1750Sensor::getLightLevel() {
    if (!isDataValid()) {
        return "Sin datos";
    }
    
    if (isDark()) {
        return "Oscuro";
    } else if (isDim()) {
        return "Tenue";
    } else if (isBright()) {
        return "Brillante";
    } else {
        return "Muy brillante";
    }
}

bool BH1750Sensor::isDark() {
    return isDataValid() && luxValue < 10.0;
}

bool BH1750Sensor::isDim() {
    return isDataValid() && luxValue >= 10.0 && luxValue < 200.0;
}

bool BH1750Sensor::isBright() {
    return isDataValid() && luxValue >= 200.0 && luxValue < 1000.0;
}

// cppcheck-suppress unusedFunction
bool BH1750Sensor::isVeryBright() {
    return isDataValid() && luxValue >= 1000.0;
}

// cppcheck-suppress unusedFunction
void BH1750Sensor::setReadInterval(unsigned long interval) {
    readInterval = interval;
}

bool BH1750Sensor::setMode(uint8_t mode) {
    // Solo permitir ciertos modos
    if (mode == BH1750_CONT_HIGH_RES || mode == BH1750_CONT_HIGH_RES2 || 
        mode == BH1750_CONT_LOW_RES || mode == BH1750_ONE_HIGH_RES ||
        mode == BH1750_ONE_HIGH_RES2 || mode == BH1750_ONE_LOW_RES) {
        
        if (writeCommand(mode)) {
            currentMode = mode;
            
            // Esperar tiempo de medición
            delay(getMeasurementTime(mode));
            
            return true;
        }
    }
    return false;
}

// cppcheck-suppress unusedFunction
bool BH1750Sensor::setAddress(uint8_t address) {
    if (address == BH1750_DEFAULT_ADDR || address == BH1750_ALT_ADDR) {
        deviceAddress = address;
        return true;
    }
    return false;
}

// cppcheck-suppress unusedFunction
bool BH1750Sensor::isReady() {
    return isInitialized;
}

bool BH1750Sensor::powerOn() {
    return writeCommand(BH1750_POWER_ON);
}

bool BH1750Sensor::powerDown() {
    return writeCommand(BH1750_POWER_DOWN);
}

// cppcheck-suppress unusedFunction
bool BH1750Sensor::reset() {
    return writeCommand(BH1750_RESET);
}

unsigned long BH1750Sensor::getTimeSinceLastReading() {
    if (lastReading == 0) {
        return ULONG_MAX;
    }
    return millis() - lastReading;
}

// cppcheck-suppress unusedFunction
bool BH1750Sensor::shouldRead() {
    return getTimeSinceLastReading() >= readInterval;
}

// cppcheck-suppress unusedFunction
void BH1750Sensor::printLightData() {
    if (!isDataValid()) {
        Serial.println("BH1750: Datos no válidos");
        return;
    }
    
    Serial.println("=== BH1750 Sensor de Luz ===");
    Serial.printf("Luminosidad: %.2f lux\\n", luxValue);
    Serial.printf("Nivel: %s\\n", getLightLevel().c_str());
    Serial.printf("Dirección I2C: 0x%02X\\n", deviceAddress);
    Serial.printf("Modo actual: 0x%02X\\n", currentMode);
}

bool BH1750Sensor::checkConnection() {
    Wire.beginTransmission(deviceAddress);
    return Wire.endTransmission() == 0;
}

float BH1750Sensor::calculateAverage(const float samples[], int count) {
    float sum = 0.0;
    int validSamples = 0;
    
    for (int i = 0; i < count; i++) {
        if (samples[i] >= 0.0) {  // Asumimos que valores negativos son no válidos
            sum += samples[i];
            validSamples++;
        }
    }
    
    if (validSamples > 0) {
        return sum / validSamples;
    }
    return 0.0;
}

bool BH1750Sensor::isValidReading(float value) {
    // Verificar que el valor esté en rango razonable para BH1750
    // BH1750 puede medir de 0 a 65535 lux
    return (value >= 0.0 && value <= 65535.0);
}

void BH1750Sensor::resetSamples() {
    for (int i = 0; i < BH1750_SAMPLES_COUNT; i++) {
        luxSamples[i] = -1.0;  // Valor inicial inválido
    }
    currentSample = 0;
}

bool BH1750Sensor::writeCommand(uint8_t command) {
    Wire.beginTransmission(deviceAddress);
    Wire.write(command);
    return Wire.endTransmission() == 0;
}

uint16_t BH1750Sensor::readRawValue() {
    Wire.requestFrom(deviceAddress, (uint8_t)2);
    
    if (Wire.available() >= 2) {
        uint8_t high = Wire.read();
        uint8_t low = Wire.read();
        return (high << 8) | low;
    }
    
    return 0xFFFF;  // Error
}

float BH1750Sensor::convertToLux(uint16_t rawValue) {
    float lux = 0.0;
    
    switch (currentMode) {
        case BH1750_CONT_HIGH_RES:
        case BH1750_ONE_HIGH_RES:
            lux = rawValue / 1.2;  // 1 lx resolución
            break;
            
        case BH1750_CONT_HIGH_RES2:
        case BH1750_ONE_HIGH_RES2:
            lux = rawValue / 2.4;  // 0.5 lx resolución
            break;
            
        case BH1750_CONT_LOW_RES:
        case BH1750_ONE_LOW_RES:
            lux = rawValue / 1.2;  // 4 lx resolución (pero misma fórmula)
            break;
            
        default:
            lux = rawValue / 1.2;  // Por defecto
            break;
    }
    
    return lux;
}

unsigned long BH1750Sensor::getMeasurementTime(uint8_t mode) {
    switch (mode) {
        case BH1750_CONT_HIGH_RES:
        case BH1750_CONT_HIGH_RES2:
        case BH1750_ONE_HIGH_RES:
        case BH1750_ONE_HIGH_RES2:
            return 180;  // 180ms para alta resolución
            
        case BH1750_CONT_LOW_RES:
        case BH1750_ONE_LOW_RES:
            return 24;   // 24ms para baja resolución
            
        default:
            return 180;  // Por defecto
    }
}
