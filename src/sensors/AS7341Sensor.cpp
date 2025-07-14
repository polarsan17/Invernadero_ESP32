#include "sensors/AS7341Sensor.h"
#include "config/config.h"
#include <Arduino.h>
#include <Wire.h>

AS7341Sensor::AS7341Sensor() {
    // Inicializar variables
    for (int i = 0; i < 12; i++) {
        spectralData[i] = 0;
    }
    
    violetReading = 0.0;
    blueReading = 0.0;
    cyanReading = 0.0;
    greenReading = 0.0;
    yellowReading = 0.0;
    orangeReading = 0.0;
    redReading = 0.0;
    nearIRReading = 0.0;
    clearReading = 0.0;
    nirReading = 0.0;
    
    lastReading = 0;
    readInterval = AS7341_READ_INTERVAL;
    isInitialized = false;
    lastReadValid = false;
    currentSample = 0;
    
    // Configuración predeterminada
    currentGain = 9; // Ganancia 256x
    currentIntegrationTime = 100;
    
    resetSamples();
}

AS7341Sensor::~AS7341Sensor() {
    // Cleanup if needed
}

bool AS7341Sensor::begin() {
    // Inicializar I2C
    Wire.begin(AS7341_SDA_PIN, AS7341_SCL_PIN);
    
    // Verificar si el dispositivo está presente
    if (!checkDevice()) {
        return false;
    }
    
    // Configurar sensor
    writeRegister(AS7341_ENABLE, 0x01); // Habilitar sensor
    writeRegister(AS7341_ATIME, currentIntegrationTime);
    writeRegister(AS7341_ASTEP_L, 0xE7); // ASTEP = 999
    writeRegister(AS7341_ASTEP_H, 0x03);
    writeRegister(AS7341_CFG1, currentGain); // Configurar ganancia
    
    delay(100); // Tiempo de estabilización
    
    isInitialized = true;
    
    // Hacer primera lectura para inicializar
    readSensor();
    
    return true;
}

bool AS7341Sensor::readSensor() {
    if (!isInitialized) {
        return false;
    }
    
    // Iniciar medición
    writeRegister(AS7341_ENABLE, 0x03); // Habilitar sensor y medición
    
    // Esperar a que termine la medición (simplificado)
    delay(200);
    
    // Leer canales espectrales
    uint8_t buffer[24]; // 12 canales * 2 bytes
    if (!readMultipleRegisters(AS7341_CH0_DATA_L, buffer, 24)) {
        lastReadValid = false;
        return false;
    }
    
    // Convertir bytes a valores de 16 bits
    for (int i = 0; i < 12; i++) {
        spectralData[i] = buffer[i*2] | (buffer[i*2 + 1] << 8);
    }
    
    // Verificar si la lectura es válida
    if (isValidReading(spectralData)) {
        // Guardar muestras para promediado
        for (int i = 0; i < 12; i++) {
            spectralSamples[i][currentSample] = spectralData[i];
        }
        currentSample = (currentSample + 1) % SAMPLES_COUNT;
        
        // Calcular promedios y actualizar valores float
        for (int i = 0; i < 12; i++) {
            spectralData[i] = (uint16_t)calculateAverage(spectralSamples[i], SAMPLES_COUNT);
        }
        
        updateFloatValues();
        
        lastReading = millis();
        lastReadValid = true;
        
        return true;
    } else {
        lastReadValid = false;
        return false;
    }
}

bool AS7341Sensor::isDataValid() {
    return lastReadValid && isInitialized;
}

// Getters para canales específicos
// cppcheck-suppress unusedFunction
float AS7341Sensor::getViolet() { return violetReading; }
// cppcheck-suppress unusedFunction
float AS7341Sensor::getBlue() { return blueReading; }
// cppcheck-suppress unusedFunction
float AS7341Sensor::getCyan() { return cyanReading; }
// cppcheck-suppress unusedFunction
float AS7341Sensor::getGreen() { return greenReading; }
// cppcheck-suppress unusedFunction
float AS7341Sensor::getYellow() { return yellowReading; }
// cppcheck-suppress unusedFunction
float AS7341Sensor::getOrange() { return orangeReading; }
// cppcheck-suppress unusedFunction
float AS7341Sensor::getRed() { return redReading; }
// cppcheck-suppress unusedFunction
float AS7341Sensor::getNearIR() { return nearIRReading; }
// cppcheck-suppress unusedFunction
float AS7341Sensor::getClear() { return clearReading; }
// cppcheck-suppress unusedFunction
float AS7341Sensor::getNIR() { return nirReading; }

float AS7341Sensor::getLux() {
    if (!isDataValid()) {
        return NAN;
    }
    
    // Cálculo aproximado de lux basado en canal clear
    // Fórmula simplificada para AS7341
    return clearReading * 0.25; // Factor de conversión aproximado
}

float AS7341Sensor::getColorTemperature() {
    if (!isDataValid()) {
        return NAN;
    }
    
    // Cálculo aproximado de temperatura de color
    // Basado en relación rojo/azul
    if (blueReading > 0) {
        float ratio = redReading / blueReading;
        return 4000.0 / ratio; // Aproximación simplificada
    }
    return NAN;
}

// cppcheck-suppress unusedFunction
void AS7341Sensor::getRGBValues(float& r, float& g, float& b) {
    if (!isDataValid()) {
        r = g = b = 0;
        return;
    }
    
    // Convertir canales espectrales a RGB aproximado
    r = (redReading + orangeReading) / 2.0;
    g = (greenReading + yellowReading) / 2.0;
    b = (blueReading + cyanReading) / 2.0;
    
    // Normalizar a 0-255
    float maxVal = (r > g) ? ((r > b) ? r : b) : ((g > b) ? g : b);
    if (maxVal > 0) {
        r = (r / maxVal) * 255.0;
        g = (g / maxVal) * 255.0;
        b = (b / maxVal) * 255.0;
    }
}

// cppcheck-suppress unusedFunction
void AS7341Sensor::setReadInterval(unsigned long interval) {
    readInterval = interval;
}

// cppcheck-suppress unusedFunction
bool AS7341Sensor::setGain(uint8_t gain) {
    if (isInitialized && gain <= 10) {
        currentGain = gain;
        return writeRegister(AS7341_CFG1, gain);
    }
    return false;
}

// cppcheck-suppress unusedFunction
bool AS7341Sensor::setIntegrationTime(uint16_t time) {
    if (isInitialized && time <= 255) {
        currentIntegrationTime = time;
        return writeRegister(AS7341_ATIME, (uint8_t)time);
    }
    return false;
}

// cppcheck-suppress unusedFunction
bool AS7341Sensor::isReady() {
    return isInitialized;
}

unsigned long AS7341Sensor::getTimeSinceLastReading() {
    if (lastReading == 0) {
        return ULONG_MAX;
    }
    return millis() - lastReading;
}

// cppcheck-suppress unusedFunction
bool AS7341Sensor::shouldRead() {
    return getTimeSinceLastReading() >= readInterval;
}

void AS7341Sensor::resetSamples() {
    for (int i = 0; i < 12; i++) {
        for (int j = 0; j < SAMPLES_COUNT; j++) {
            spectralSamples[i][j] = 0;
        }
    }
    currentSample = 0;
}

// cppcheck-suppress unusedFunction
void AS7341Sensor::printSpectralData() {
    if (!isDataValid()) {
        Serial.println("AS7341: Datos no válidos");
        return;
    }
    
    Serial.println("=== AS7341 Datos Espectrales ===");
    Serial.printf("Violeta (415nm): %.2f\\n", violetReading);
    Serial.printf("Azul (445nm): %.2f\\n", blueReading);
    Serial.printf("Cian (480nm): %.2f\\n", cyanReading);
    Serial.printf("Verde (515nm): %.2f\\n", greenReading);
    Serial.printf("Amarillo (555nm): %.2f\\n", yellowReading);
    Serial.printf("Naranja (590nm): %.2f\\n", orangeReading);
    Serial.printf("Rojo (630nm): %.2f\\n", redReading);
    Serial.printf("Infrarrojo cercano (680nm): %.2f\\n", nearIRReading);
    Serial.printf("Clear: %.2f\\n", clearReading);
    Serial.printf("NIR: %.2f\\n", nirReading);
    Serial.printf("Lux: %.2f\\n", getLux());
    Serial.printf("Temperatura color: %.0fK\\n", getColorTemperature());
}

float AS7341Sensor::calculateAverage(const uint16_t samples[], int count) {
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

bool AS7341Sensor::isValidReading(uint16_t* data) {
    // Verificar que no todos los valores sean 0 o máximo
    bool hasValidData = false;
    
    for (int i = 0; i < 12; i++) {
        if (data[i] > 0 && data[i] < 65535) {
            hasValidData = true;
            break;
        }
    }
    
    return hasValidData;
}

void AS7341Sensor::updateFloatValues() {
    violetReading = (float)spectralData[0];    // Canal 415nm
    blueReading = (float)spectralData[1];      // Canal 445nm
    cyanReading = (float)spectralData[2];      // Canal 480nm
    greenReading = (float)spectralData[3];     // Canal 515nm
    yellowReading = (float)spectralData[4];    // Canal 555nm
    orangeReading = (float)spectralData[5];    // Canal 590nm
    redReading = (float)spectralData[6];       // Canal 630nm
    nearIRReading = (float)spectralData[7];    // Canal 680nm
    clearReading = (float)spectralData[10];    // Canal Clear
    nirReading = (float)spectralData[11];      // Canal NIR
}

// Comunicación I2C
bool AS7341Sensor::writeRegister(uint8_t reg, uint8_t value) {
    Wire.beginTransmission(AS7341_ADDR);
    Wire.write(reg);
    Wire.write(value);
    return Wire.endTransmission() == 0;
}

uint8_t AS7341Sensor::readRegister(uint8_t reg) {
    Wire.beginTransmission(AS7341_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom((uint8_t)AS7341_ADDR, (uint8_t)1);
    if (Wire.available()) {
        return Wire.read();
    }
    return 0;
}

// cppcheck-suppress unusedFunction
uint16_t AS7341Sensor::readRegister16(uint8_t reg) {
    Wire.beginTransmission(AS7341_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom((uint8_t)AS7341_ADDR, (uint8_t)2);
    if (Wire.available() >= 2) {
        uint8_t low = Wire.read();
        uint8_t high = Wire.read();
        return low | (high << 8);
    }
    return 0;
}

bool AS7341Sensor::readMultipleRegisters(uint8_t reg, uint8_t* buffer, uint8_t length) {
    Wire.beginTransmission(AS7341_ADDR);
    Wire.write(reg);
    Wire.endTransmission();
    
    Wire.requestFrom((uint8_t)AS7341_ADDR, length);
    
    for (uint8_t i = 0; i < length; i++) {
        if (Wire.available()) {
            buffer[i] = Wire.read();
        } else {
            return false;
        }
    }
    return true;
}

bool AS7341Sensor::checkDevice() {
    uint8_t id = readRegister(AS7341_ID);
    return (id == 0x09 || id == 0x40); // IDs típicos del AS7341
}
