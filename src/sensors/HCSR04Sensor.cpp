#include "sensors/HCSR04Sensor.h"
#include "config/config.h"
#include <Arduino.h>

HCSR04Sensor::HCSR04Sensor() 
    : triggerPin(HCSR04_TRIGGER_PIN)
    , echoPin(HCSR04_ECHO_PIN)
    , distanceCm(0.0)
    , waterLevelCm(0.0)
    , waterLevelPercentage(0.0)
    , currentSample(0)
    , tankHeightCm(100.0)  // Configuración por defecto del tanque (100cm de altura, sensor a 5cm del tope)
    , sensorHeightCm(5.0)
    , minWaterLevelCm(5.0)   // 5cm desde el fondo
    , maxWaterLevelCm(95.0)  // 95cm desde el fondo
    , lastReading(0)
    , readInterval(HCSR04_READ_INTERVAL)
    , isInitialized(false)
    , lastReadValid(false)
{
    resetSamples();
}

HCSR04Sensor::HCSR04Sensor(uint8_t trigPin, uint8_t echoPin) 
    : triggerPin(trigPin)
    , echoPin(echoPin)
    , distanceCm(0.0)
    , waterLevelCm(0.0)
    , waterLevelPercentage(0.0)
    , currentSample(0)
    , tankHeightCm(100.0)  // Configuración por defecto del tanque
    , sensorHeightCm(5.0)
    , minWaterLevelCm(5.0)
    , maxWaterLevelCm(95.0)
    , lastReading(0)
    , readInterval(HCSR04_READ_INTERVAL)
    , isInitialized(false)
    , lastReadValid(false)
{
    resetSamples();
}

HCSR04Sensor::~HCSR04Sensor() {
    // Cleanup if needed
}

bool HCSR04Sensor::begin() {
    return begin(tankHeightCm, sensorHeightCm);
}

bool HCSR04Sensor::begin(float tankHeight, float sensorHeight) {
    // Configurar pines
    pinMode(triggerPin, OUTPUT);
    pinMode(echoPin, INPUT);
    
    // Asegurar que trigger esté en LOW
    digitalWrite(triggerPin, LOW);
    
    // Configurar dimensiones del tanque
    setTankDimensions(tankHeight, sensorHeight);
    
    // Verificar funcionamiento del sensor
    if (!testSensor()) {
        Serial.println("HC-SR04: Error en el test del sensor");
        return false;
    }
    
    isInitialized = true;
    
    // Hacer primera lectura para inicializar
    readSensor();
    
    Serial.printf("HC-SR04: Inicializado - Trigger: %d, Echo: %d\\n", triggerPin, echoPin);
    Serial.printf("Tanque: %.1fcm, Sensor altura: %.1fcm\\n", tankHeightCm, sensorHeightCm);
    return true;
}

void HCSR04Sensor::setTankDimensions(float tankHeight, float sensorHeight) {
    tankHeightCm = tankHeight;
    sensorHeightCm = sensorHeight;
    
    // Calcular límites automáticamente
    minWaterLevelCm = 5.0;  // 5cm desde el fondo
    maxWaterLevelCm = tankHeightCm - sensorHeightCm - 2.0;  // 2cm de margen desde el sensor
}

// cppcheck-suppress unusedFunction
void HCSR04Sensor::setWaterLevelLimits(float minLevel, float maxLevel) {
    minWaterLevelCm = minLevel;
    maxWaterLevelCm = maxLevel;
}

bool HCSR04Sensor::readSensor() {
    if (!isInitialized) {
        return false;
    }
    
    // Medir distancia
    float newDistance = measureDistance();
    
    // Verificar si la lectura es válida
    if (isValidReading(newDistance)) {
        // Guardar muestra para promediado
        distanceSamples[currentSample] = newDistance;
        currentSample = (currentSample + 1) % HCSR04_SAMPLES_COUNT;
        
        // Calcular promedio
        distanceCm = calculateAverage(distanceSamples, HCSR04_SAMPLES_COUNT);
        
        // Calcular nivel de agua
        waterLevelCm = calculateWaterLevel(distanceCm);
        waterLevelPercentage = calculateWaterPercentage(waterLevelCm);
        
        lastReading = millis();
        lastReadValid = true;
        
        return true;
    } else {
        lastReadValid = false;
        return false;
    }
}

bool HCSR04Sensor::isDataValid() {
    return lastReadValid && isInitialized;
}

// cppcheck-suppress unusedFunction
float HCSR04Sensor::getDistance() {
    return distanceCm;
}

// cppcheck-suppress unusedFunction
float HCSR04Sensor::getDistanceRaw() {
    if (!isInitialized) {
        return NAN;
    }
    return measureDistance();
}

// cppcheck-suppress unusedFunction
float HCSR04Sensor::getWaterLevel() {
    return waterLevelCm;
}

// cppcheck-suppress unusedFunction
float HCSR04Sensor::getWaterPercentage() {
    return waterLevelPercentage;
}

String HCSR04Sensor::getWaterStatus() {
    if (!isDataValid()) {
        return "Sin datos";
    }
    
    if (isEmpty()) {
        return "Vacío";
    } else if (isLow()) {
        return "Bajo";
    } else if (isMedium()) {
        return "Medio";
    } else if (isHigh()) {
        return "Alto";
    } else {
        return "Lleno";
    }
}

bool HCSR04Sensor::isEmpty() {
    return isDataValid() && waterLevelPercentage < 10.0;
}

bool HCSR04Sensor::isLow() {
    return isDataValid() && waterLevelPercentage >= 10.0 && waterLevelPercentage < 30.0;
}

bool HCSR04Sensor::isMedium() {
    return isDataValid() && waterLevelPercentage >= 30.0 && waterLevelPercentage < 70.0;
}

bool HCSR04Sensor::isHigh() {
    return isDataValid() && waterLevelPercentage >= 70.0 && waterLevelPercentage < 90.0;
}

// cppcheck-suppress unusedFunction
bool HCSR04Sensor::isFull() {
    return isDataValid() && waterLevelPercentage >= 90.0;
}

// cppcheck-suppress unusedFunction
void HCSR04Sensor::setReadInterval(unsigned long interval) {
    readInterval = interval;
}

// cppcheck-suppress unusedFunction
void HCSR04Sensor::setPins(uint8_t trigPin, uint8_t echoPin) {
    triggerPin = trigPin;
    echoPin = echoPin;
}

// cppcheck-suppress unusedFunction
bool HCSR04Sensor::isReady() {
    return isInitialized;
}

unsigned long HCSR04Sensor::getTimeSinceLastReading() {
    if (lastReading == 0) {
        return ULONG_MAX;
    }
    return millis() - lastReading;
}

// cppcheck-suppress unusedFunction
bool HCSR04Sensor::shouldRead() {
    return getTimeSinceLastReading() >= readInterval;
}

// cppcheck-suppress unusedFunction
void HCSR04Sensor::calibrateEmpty() {
    if (isInitialized) {
        float emptyDistance = measureDistance();
        // Cuando está vacío, el nivel de agua sería minWaterLevelCm
        minWaterLevelCm = 0.0;  // Ajustar según la medición
        Serial.printf("Calibración vacío: %.2f cm de distancia\\n", emptyDistance);
    }
}

// cppcheck-suppress unusedFunction
void HCSR04Sensor::calibrateFull() {
    if (isInitialized) {
        float fullDistance = measureDistance();
        // Cuando está lleno, calcular el nivel máximo
        maxWaterLevelCm = tankHeightCm - sensorHeightCm - fullDistance;
        Serial.printf("Calibración lleno: %.2f cm de distancia, nivel: %.2f cm\\n", fullDistance, maxWaterLevelCm);
    }
}

// cppcheck-suppress unusedFunction
void HCSR04Sensor::printWaterLevelData() {
    if (!isDataValid()) {
        Serial.println("HC-SR04: Datos no válidos");
        return;
    }
    
    Serial.println("=== HC-SR04 Nivel de Agua ===");
    Serial.printf("Distancia sensor: %.2f cm\\n", distanceCm);
    Serial.printf("Nivel de agua: %.2f cm\\n", waterLevelCm);
    Serial.printf("Porcentaje llenado: %.1f%%\\n", waterLevelPercentage);
    Serial.printf("Estado: %s\\n", getWaterStatus().c_str());
    Serial.printf("Configuración tanque: %.1f cm (altura), %.1f cm (sensor)\\n", tankHeightCm, sensorHeightCm);
}

bool HCSR04Sensor::testSensor() {
    // Hacer una medición de prueba
    float testDistance = measureDistance();
    
    // Verificar que esté en rango razonable (2cm a 400cm)
    return (testDistance >= 2.0 && testDistance <= 400.0);
}

float HCSR04Sensor::calculateAverage(const float samples[], int count) {
    float sum = 0.0;
    int validSamples = 0;
    
    for (int i = 0; i < count; i++) {
        if (samples[i] > 0.0) {  // Asumimos que valores <= 0 son no válidos
            sum += samples[i];
            validSamples++;
        }
    }
    
    if (validSamples > 0) {
        return sum / validSamples;
    }
    return 0.0;
}

bool HCSR04Sensor::isValidReading(float distance) {
    // Verificar que la distancia esté en rango válido para HC-SR04
    // HC-SR04 puede medir de 2cm a 400cm
    return (distance >= 2.0 && distance <= 400.0);
}

void HCSR04Sensor::resetSamples() {
    for (int i = 0; i < HCSR04_SAMPLES_COUNT; i++) {
        distanceSamples[i] = 0.0;
    }
    currentSample = 0;
}

float HCSR04Sensor::measureDistance() {
    // Limpiar trigger
    digitalWrite(triggerPin, LOW);
    delayMicroseconds(2);
    
    // Enviar pulso trigger
    digitalWrite(triggerPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(triggerPin, LOW);
    
    // Medir el tiempo del pulso echo
    unsigned long duration = pulseIn(echoPin, HIGH, 30000); // Timeout de 30ms
    
    if (duration == 0) {
        return -1.0; // Error de lectura
    }
    
    // Calcular distancia en cm
    // Velocidad del sonido: 343 m/s = 0.0343 cm/µs
    // Distancia = (tiempo * velocidad) / 2 (ida y vuelta)
    float distance = (duration * 0.0343) / 2.0;
    
    return distance;
}

float HCSR04Sensor::calculateWaterLevel(float distance) {
    // El nivel de agua se calcula como:
    // nivel = altura_tanque - altura_sensor - distancia_medida
    float level = tankHeightCm - sensorHeightCm - distance;
    
    // Asegurar que esté en rango válido
    if (level < 0.0) level = 0.0;
    if (level > tankHeightCm) level = tankHeightCm;
    
    return level;
}

float HCSR04Sensor::calculateWaterPercentage(float waterLevel) {
    // Calcular porcentaje basado en los límites configurados
    float usableRange = maxWaterLevelCm - minWaterLevelCm;
    
    if (usableRange <= 0.0) {
        return 0.0;
    }
    
    float adjustedLevel = waterLevel - minWaterLevelCm;
    float percentage = (adjustedLevel / usableRange) * 100.0;
    
    // Limitar a 0-100%
    if (percentage < 0.0) percentage = 0.0;
    if (percentage > 100.0) percentage = 100.0;
    
    return percentage;
}
