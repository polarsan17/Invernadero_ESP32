#include "sensors/SensorManager.h"
#include "config/config.h"

SensorManager::SensorManager(BlynkManager& blynk) : blynkManager(&blynk) {
    dht22Sensor = new DHT22Sensor(DHT22_PIN);
    as7341Sensor = new AS7341Sensor();
    soilMoistureSensor = new SoilMoistureSensor();
    bh1750Sensor = new BH1750Sensor();
    hcsr04Sensor = new HCSR04Sensor();
    rs485SoilSensor = new RS485SoilSensor(&Serial2, RS485_TX_ENABLE_PIN, 0x01);
    lastBlynkUpdate = 0;
    blynkUpdateInterval = BLYNK_UPDATE_INTERVAL;
    sensorsInitialized = false;
}

SensorManager::~SensorManager() {
    if (dht22Sensor) {
        delete dht22Sensor;
    }
    if (as7341Sensor) {
        delete as7341Sensor;
    }
    if (soilMoistureSensor) {
        delete soilMoistureSensor;
    }
    if (bh1750Sensor) {
        delete bh1750Sensor;
    }
    if (hcsr04Sensor) {
        delete hcsr04Sensor;
    }
    if (rs485SoilSensor) {
        delete rs485SoilSensor;
    }
}

bool SensorManager::begin() {
    Serial.println("Inicializando sensores...");
    
    // Inicializar DHT22
    if (dht22Sensor->begin()) {
        Serial.println("DHT22: OK");
    } else {
        Serial.println("DHT22: ERROR");
        return false;
    }
    
    // Inicializar AS7341
    if (as7341Sensor->begin()) {
        Serial.println("AS7341: OK");
    } else {
        Serial.println("AS7341: ERROR");
        return false;
    }
    
    // Inicializar sensor de humedad del suelo
    if (soilMoistureSensor->begin()) {
        Serial.println("Sensor humedad suelo: OK");
    } else {
        Serial.println("Sensor humedad suelo: ERROR");
        return false;
    }
    
    // Inicializar sensor BH1750
    if (bh1750Sensor->begin()) {
        Serial.println("Sensor BH1750: OK");
    } else {
        Serial.println("Sensor BH1750: ERROR");
        return false;
    }
    
    // Inicializar sensor HC-SR04
    if (hcsr04Sensor->begin()) {
        Serial.println("Sensor HC-SR04: OK");
    } else {
        Serial.println("Sensor HC-SR04: ERROR");
        return false;
    }
    
    // Inicializar sensor RS485 de suelo
    if (rs485SoilSensor->begin()) {
        Serial.println("Sensor RS485 Suelo: OK");
        sensorsInitialized = true;
    } else {
        Serial.println("Sensor RS485 Suelo: ERROR");
        return false;
    }
    
    Serial.println("Sensores inicializados correctamente");
    return true;
}

// cppcheck-suppress unusedFunction
void SensorManager::update() {
    if (!sensorsInitialized) {
        return;
    }
    
    // Leer sensores si es necesario
    readAllSensors();
    
    // Enviar datos a Blynk si es necesario
    if (shouldUpdateBlynk()) {
        sendDataToBlynk();
    }
}

bool SensorManager::readAllSensors() {
    bool success = true;
    
    // Leer DHT22 si es tiempo
    if (dht22Sensor->shouldRead()) {
        if (!dht22Sensor->readSensor()) {
            success = false;
        }
    }
    
    // Leer AS7341 si es tiempo
    if (as7341Sensor->shouldRead()) {
        if (!as7341Sensor->readSensor()) {
            success = false;
        }
    }
    
    // Leer sensor de humedad del suelo si es tiempo
    if (soilMoistureSensor->shouldRead()) {
        if (!soilMoistureSensor->readSensor()) {
            success = false;
        }
    }
    
    // Leer sensor BH1750 si es tiempo
    if (bh1750Sensor->shouldRead()) {
        if (!bh1750Sensor->readSensor()) {
            success = false;
        }
    }
    
    // Leer sensor HC-SR04 si es tiempo
    if (hcsr04Sensor->shouldRead()) {
        if (!hcsr04Sensor->readSensor()) {
            success = false;
        }
    }
    
    // Leer sensor RS485 de suelo si es tiempo
    if (rs485SoilSensor->shouldRead()) {
        if (!rs485SoilSensor->readSensor()) {
            success = false;
        }
    }
    
    return success;
}

void SensorManager::sendDataToBlynk() {
    if (!blynkManager->isConnected()) {
        return;
    }
    
    // Enviar datos DHT22
    if (dht22Sensor->isDataValid()) {
        float temp = dht22Sensor->getTemperature();
        blynkManager->sendVirtualPin(BLYNK_VPIN_TEMPERATURE, temp);
        
        float hum = dht22Sensor->getHumidity();
        blynkManager->sendVirtualPin(BLYNK_VPIN_HUMIDITY, hum);
        
        float heatIdx = dht22Sensor->getHeatIndex();
        blynkManager->sendVirtualPin(BLYNK_VPIN_HEAT_INDEX, heatIdx);
    }
    
    // Enviar datos AS7341
    if (as7341Sensor->isDataValid()) {
        float lux = as7341Sensor->getLux();
        blynkManager->sendVirtualPin(BLYNK_VPIN_LUX, lux);
        
        float colorTemp = as7341Sensor->getColorTemperature();
        blynkManager->sendVirtualPin(BLYNK_VPIN_COLOR_TEMP, colorTemp);
        
        float redLight = as7341Sensor->getRed();
        blynkManager->sendVirtualPin(BLYNK_VPIN_RED_LIGHT, redLight);
        
        float greenLight = as7341Sensor->getGreen();
        blynkManager->sendVirtualPin(BLYNK_VPIN_GREEN_LIGHT, greenLight);
        
        float blueLight = as7341Sensor->getBlue();
        blynkManager->sendVirtualPin(BLYNK_VPIN_BLUE_LIGHT, blueLight);
        
        float clearLight = as7341Sensor->getClear();
        blynkManager->sendVirtualPin(BLYNK_VPIN_CLEAR_LIGHT, clearLight);
        
        float nirLight = as7341Sensor->getNIR();
        blynkManager->sendVirtualPin(BLYNK_VPIN_NIR_LIGHT, nirLight);
    }
    
    // Enviar datos sensor de humedad del suelo
    if (soilMoistureSensor->isDataValid()) {
        float soilMoisture = soilMoistureSensor->getMoisturePercentage();
        blynkManager->sendVirtualPin(BLYNK_VPIN_SOIL_MOISTURE, soilMoisture);
        
        uint16_t soilRaw = soilMoistureSensor->getRawValue();
        blynkManager->sendVirtualPin(BLYNK_VPIN_SOIL_RAW, soilRaw);
    }
    
    // Enviar datos sensor BH1750
    if (bh1750Sensor->isDataValid()) {
        float lightLux = bh1750Sensor->getLux();
        blynkManager->sendVirtualPin(BLYNK_VPIN_LIGHT_LUX, lightLux);
        
        String lightLevel = bh1750Sensor->getLightLevel();
        blynkManager->sendVirtualPin(BLYNK_VPIN_LIGHT_LEVEL, lightLevel);
    }
    
    // Enviar datos sensor HC-SR04
    if (hcsr04Sensor->isDataValid()) {
        float waterLevel = hcsr04Sensor->getWaterLevel();
        blynkManager->sendVirtualPin(BLYNK_VPIN_WATER_LEVEL, waterLevel);
        
        float waterPercentage = hcsr04Sensor->getWaterPercentage();
        blynkManager->sendVirtualPin(BLYNK_VPIN_WATER_PERCENTAGE, waterPercentage);
        
        String waterStatus = hcsr04Sensor->getWaterStatus();
        blynkManager->sendVirtualPin(BLYNK_VPIN_WATER_STATUS, waterStatus);
    }
    
    // Enviar datos sensor RS485 de suelo
    if (rs485SoilSensor->isDataValid()) {
        float soilTemp = rs485SoilSensor->getTemperature();
        blynkManager->sendVirtualPin(BLYNK_VPIN_SOIL_TEMP, soilTemp);
        
        float soilMoisture = rs485SoilSensor->getMoisture();
        blynkManager->sendVirtualPin(BLYNK_VPIN_SOIL_MOISTURE_RS485, soilMoisture);
        
        float soilEC = rs485SoilSensor->getElectricalConductivity();
        blynkManager->sendVirtualPin(BLYNK_VPIN_SOIL_EC, soilEC);
        
        float soilPH = rs485SoilSensor->getPH();
        blynkManager->sendVirtualPin(BLYNK_VPIN_SOIL_PH, soilPH);
        
        uint16_t nitrogen = rs485SoilSensor->getNitrogen();
        blynkManager->sendVirtualPin(BLYNK_VPIN_SOIL_NPK_N, nitrogen);
        
        uint16_t phosphorus = rs485SoilSensor->getPhosphorus();
        blynkManager->sendVirtualPin(BLYNK_VPIN_SOIL_NPK_P, phosphorus);
        
        uint16_t potassium = rs485SoilSensor->getPotassium();
        blynkManager->sendVirtualPin(BLYNK_VPIN_SOIL_NPK_K, potassium);
    }
    
    lastBlynkUpdate = millis();
    
    Serial.println("Datos de sensores enviados a Blynk");
}

// cppcheck-suppress unusedFunction
bool SensorManager::areSensorsReady() {
    return sensorsInitialized && dht22Sensor->isReady() && as7341Sensor->isReady() && soilMoistureSensor->isReady() && bh1750Sensor->isReady() && hcsr04Sensor->isReady() && rs485SoilSensor->isReady();
}

// Getters DHT22
float SensorManager::getTemperature() {
    if (dht22Sensor->isDataValid()) {
        return dht22Sensor->getTemperature();
    }
    return NAN;
}

float SensorManager::getHumidity() {
    if (dht22Sensor->isDataValid()) {
        return dht22Sensor->getHumidity();
    }
    return NAN;
}

float SensorManager::getHeatIndex() {
    if (dht22Sensor->isDataValid()) {
        return dht22Sensor->getHeatIndex();
    }
    return NAN;
}

// Getters AS7341
float SensorManager::getLux() {
    if (as7341Sensor->isDataValid()) {
        return as7341Sensor->getLux();
    }
    return NAN;
}

float SensorManager::getColorTemperature() {
    if (as7341Sensor->isDataValid()) {
        return as7341Sensor->getColorTemperature();
    }
    return NAN;
}

float SensorManager::getRedLight() {
    if (as7341Sensor->isDataValid()) {
        return as7341Sensor->getRed();
    }
    return NAN;
}

float SensorManager::getGreenLight() {
    if (as7341Sensor->isDataValid()) {
        return as7341Sensor->getGreen();
    }
    return NAN;
}

float SensorManager::getBlueLight() {
    if (as7341Sensor->isDataValid()) {
        return as7341Sensor->getBlue();
    }
    return NAN;
}

float SensorManager::getClearLight() {
    if (as7341Sensor->isDataValid()) {
        return as7341Sensor->getClear();
    }
    return NAN;
}

float SensorManager::getNIRLight() {
    if (as7341Sensor->isDataValid()) {
        return as7341Sensor->getNIR();
    }
    return NAN;
}

// Getters para sensor de humedad del suelo
float SensorManager::getSoilMoisture() {
    if (soilMoistureSensor->isDataValid()) {
        return soilMoistureSensor->getMoisturePercentage();
    }
    return NAN;
}

uint16_t SensorManager::getSoilRawValue() {
    if (soilMoistureSensor->isDataValid()) {
        return soilMoistureSensor->getRawValue();
    }
    return 0;
}

String SensorManager::getSoilMoistureLevel() {
    if (soilMoistureSensor->isDataValid()) {
        return soilMoistureSensor->getMoistureLevel();
    }
    return "Sin datos";
}

// Getters para sensor BH1750
float SensorManager::getLightLux() {
    if (bh1750Sensor->isDataValid()) {
        return bh1750Sensor->getLux();
    }
    return NAN;
}

String SensorManager::getLightLevel() {
    if (bh1750Sensor->isDataValid()) {
        return bh1750Sensor->getLightLevel();
    }
    return "Sin datos";
}

// Getters para sensor HC-SR04
float SensorManager::getWaterLevel() {
    if (hcsr04Sensor->isDataValid()) {
        return hcsr04Sensor->getWaterLevel();
    }
    return NAN;
}

float SensorManager::getWaterPercentage() {
    if (hcsr04Sensor->isDataValid()) {
        return hcsr04Sensor->getWaterPercentage();
    }
    return NAN;
}

String SensorManager::getWaterStatus() {
    if (hcsr04Sensor->isDataValid()) {
        return hcsr04Sensor->getWaterStatus();
    }
    return "Sin datos";
}

// cppcheck-suppress unusedFunction
void SensorManager::setBlynkUpdateInterval(unsigned long interval) {
    blynkUpdateInterval = interval;
}

// cppcheck-suppress unusedFunction
void SensorManager::printAllSensorData() {
    Serial.println("\\n=== DATOS DE SENSORES ===");
    
    // Datos DHT22
    if (dht22Sensor->isDataValid()) {
        Serial.printf("Temperatura: %.1f°C\\n", getTemperature());
        Serial.printf("Humedad: %.1f%%\\n", getHumidity());
        Serial.printf("Índice de calor: %.1f°C\\n", getHeatIndex());
    } else {
        Serial.println("DHT22: Sin datos válidos");
    }
    
    // Datos AS7341
    if (as7341Sensor->isDataValid()) {
        Serial.printf("Lux: %.2f\\n", getLux());
        Serial.printf("Temperatura color: %.0fK\\n", getColorTemperature());
        Serial.printf("Luz roja: %.2f\\n", getRedLight());
        Serial.printf("Luz verde: %.2f\\n", getGreenLight());
        Serial.printf("Luz azul: %.2f\\n", getBlueLight());
        Serial.printf("Luz clear: %.2f\\n", getClearLight());
        Serial.printf("Luz NIR: %.2f\\n", getNIRLight());
    } else {
        Serial.println("AS7341: Sin datos válidos");
    }
    
    // Datos sensor de humedad del suelo
    if (soilMoistureSensor->isDataValid()) {
        Serial.printf("Humedad suelo: %.1f%% (%s)\\n", getSoilMoisture(), getSoilMoistureLevel().c_str());
        Serial.printf("Valor raw suelo: %d\\n", getSoilRawValue());
    } else {
        Serial.println("Sensor humedad suelo: Sin datos válidos");
    }
    
    // Datos sensor BH1750
    if (bh1750Sensor->isDataValid()) {
        Serial.printf("Luminosidad: %.2f lux (%s)\\n", getLightLux(), getLightLevel().c_str());
    } else {
        Serial.println("Sensor BH1750: Sin datos válidos");
    }
    
    // Datos sensor HC-SR04
    if (hcsr04Sensor->isDataValid()) {
        Serial.printf("Nivel agua: %.1f cm (%.1f%%) - %s\\n", getWaterLevel(), getWaterPercentage(), getWaterStatus().c_str());
    } else {
        Serial.println("Sensor HC-SR04: Sin datos válidos");
    }
    
    // Datos sensor RS485 de suelo
    if (rs485SoilSensor->isDataValid()) {
        Serial.printf("Suelo RS485 - Temp: %.1f°C, Humedad: %.1f%%, pH: %.1f, EC: %.0f uS/cm\\n", 
                     getSoilTemperature(), getSoilMoistureRS485(), getSoilPH(), getSoilEC());
        if (getSoilNitrogen() > 0) {  // Solo mostrar NPK si está disponible
            Serial.printf("NPK - N: %d mg/kg, P: %d mg/kg, K: %d mg/kg\\n", 
                         getSoilNitrogen(), getSoilPhosphorus(), getSoilPotassium());
        }
        Serial.printf("Estado suelo: %s\\n", getSoilStatus().c_str());
    } else {
        Serial.println("Sensor RS485 Suelo: Sin datos válidos");
    }
    
    Serial.println("========================\\n");
}

bool SensorManager::shouldUpdateBlynk() {
    return (millis() - lastBlynkUpdate) >= blynkUpdateInterval;
}

// Getters para sensor RS485 de suelo
float SensorManager::getSoilTemperature() {
    if (rs485SoilSensor->isDataValid()) {
        return rs485SoilSensor->getTemperature();
    }
    return NAN;
}

float SensorManager::getSoilMoistureRS485() {
    if (rs485SoilSensor->isDataValid()) {
        return rs485SoilSensor->getMoisture();
    }
    return NAN;
}

float SensorManager::getSoilEC() {
    if (rs485SoilSensor->isDataValid()) {
        return rs485SoilSensor->getElectricalConductivity();
    }
    return NAN;
}

float SensorManager::getSoilPH() {
    if (rs485SoilSensor->isDataValid()) {
        return rs485SoilSensor->getPH();
    }
    return NAN;
}

uint16_t SensorManager::getSoilNitrogen() {
    if (rs485SoilSensor->isDataValid()) {
        return rs485SoilSensor->getNitrogen();
    }
    return 0;
}

uint16_t SensorManager::getSoilPhosphorus() {
    if (rs485SoilSensor->isDataValid()) {
        return rs485SoilSensor->getPhosphorus();
    }
    return 0;
}

uint16_t SensorManager::getSoilPotassium() {
    if (rs485SoilSensor->isDataValid()) {
        return rs485SoilSensor->getPotassium();
    }
    return 0;
}

String SensorManager::getSoilStatus() {
    if (rs485SoilSensor->isDataValid()) {
        return rs485SoilSensor->getSoilStatus();
    }
    return "Sin datos";
}

String SensorManager::getNutrientStatus() {
    if (rs485SoilSensor->isDataValid()) {
        return rs485SoilSensor->getNutrientStatus();
    }
    return "Sin datos";
}
