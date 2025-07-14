#include "sensors/DHT22Sensor.h"
#include "config/config.h"

DHT22Sensor::DHT22Sensor(uint8_t dataPin) : pin(dataPin) {
    dht = new DHT(pin, DHT22);
    temperature = 0.0;
    humidity = 0.0;
    heatIndex = 0.0;
    lastReading = 0;
    readInterval = DHT22_READ_INTERVAL;
    isInitialized = false;
    lastReadValid = false;
    currentSample = 0;
    resetSamples();
}

DHT22Sensor::~DHT22Sensor() {
    if (dht) {
        delete dht;
    }
}

bool DHT22Sensor::begin() {
    if (dht) {
        dht->begin();
        isInitialized = true;
        
        // Esperar tiempo de estabilización
        delay(2000);
        
        // Hacer primera lectura para inicializar
        readSensor();
        
        return true;
    }
    return false;
}

bool DHT22Sensor::readSensor() {
    if (!isInitialized) {
        return false;
    }
    
    // Leer temperatura y humedad
    float newTemp = dht->readTemperature();
    float newHum = dht->readHumidity();
    
    // Verificar si la lectura es válida
    if (isValidReading(newTemp, newHum)) {
        // Guardar muestra
        tempSamples[currentSample] = newTemp;
        humSamples[currentSample] = newHum;
        currentSample = (currentSample + 1) % SAMPLES_COUNT;
        
        // Calcular promedios
        temperature = calculateAverage(tempSamples, SAMPLES_COUNT);
        humidity = calculateAverage(humSamples, SAMPLES_COUNT);
        
        // Calcular índice de calor
        heatIndex = dht->computeHeatIndex(temperature, humidity, false);
        
        lastReading = millis();
        lastReadValid = true;
        
        return true;
    } else {
        lastReadValid = false;
        return false;
    }
}

// cppcheck-suppress unusedFunction
bool DHT22Sensor::isDataValid() {
    return lastReadValid && isInitialized;
}

// cppcheck-suppress unusedFunction
float DHT22Sensor::getTemperature() {
    return temperature;
}

// cppcheck-suppress unusedFunction
float DHT22Sensor::getHumidity() {
    return humidity;
}

// cppcheck-suppress unusedFunction
float DHT22Sensor::getHeatIndex() {
    return heatIndex;
}

// cppcheck-suppress unusedFunction
void DHT22Sensor::setReadInterval(unsigned long interval) {
    readInterval = interval;
}

// cppcheck-suppress unusedFunction
bool DHT22Sensor::isReady() {
    return isInitialized;
}

unsigned long DHT22Sensor::getTimeSinceLastReading() {
    if (lastReading == 0) {
        return ULONG_MAX;
    }
    return millis() - lastReading;
}

// cppcheck-suppress unusedFunction
bool DHT22Sensor::shouldRead() {
    return getTimeSinceLastReading() >= readInterval;
}

void DHT22Sensor::resetSamples() {
    for (int i = 0; i < SAMPLES_COUNT; i++) {
        tempSamples[i] = 0.0;
        humSamples[i] = 0.0;
    }
    currentSample = 0;
}

float DHT22Sensor::calculateAverage(const float samples[], int count) {
    float sum = 0.0;
    int validSamples = 0;
    
    for (int i = 0; i < count; i++) {
        if (samples[i] != 0.0) {  // Asumimos que 0.0 es valor no válido
            sum += samples[i];
            validSamples++;
        }
    }
    
    if (validSamples > 0) {
        return sum / validSamples;
    }
    return 0.0;
}

bool DHT22Sensor::isValidReading(float temp, float hum) {
    // Verificar si no son NaN
    if (isnan(temp) || isnan(hum)) {
        return false;
    }
    
    // Verificar rangos razonables para DHT22
    // Temperatura: -40°C a +80°C
    // Humedad: 0% a 100%
    if (temp < -40.0 || temp > 80.0) {
        return false;
    }
    
    if (hum < 0.0 || hum > 100.0) {
        return false;
    }
    
    return true;
}
