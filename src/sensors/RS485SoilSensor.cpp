#include "sensors/RS485SoilSensor.h"
#include "config/config.h"
#include <Arduino.h>

RS485SoilSensor::RS485SoilSensor() 
    : rs485Serial(nullptr)
    , txEnablePin(RS485_TX_ENABLE_PIN)
    , deviceAddress(0x01)  // Dirección por defecto
    , baudRate(4800)       // Baudrate por defecto
    , temperature(0.0)
    , moisture(0.0)
    , electricalConductivity(0.0)
    , pH(0.0)
    , nitrogen(0)
    , phosphorus(0)
    , potassium(0)
    , currentSample(0)
    , lastReading(0)
    , readInterval(RS485_READ_INTERVAL)
    , isInitialized(false)
    , lastReadValid(false)
{
    resetSamples();
}

RS485SoilSensor::RS485SoilSensor(HardwareSerial* serial, uint8_t txPin, uint8_t deviceAddr) 
    : rs485Serial(serial)
    , txEnablePin(txPin)
    , deviceAddress(deviceAddr)
    , baudRate(4800)
    , temperature(0.0)
    , moisture(0.0)
    , electricalConductivity(0.0)
    , pH(0.0)
    , nitrogen(0)
    , phosphorus(0)
    , potassium(0)
    , currentSample(0)
    , lastReading(0)
    , readInterval(RS485_READ_INTERVAL)
    , isInitialized(false)
    , lastReadValid(false)
{
    resetSamples();
}

RS485SoilSensor::~RS485SoilSensor() {
    // Cleanup if needed
}

bool RS485SoilSensor::begin() {
    return begin(baudRate);
}

bool RS485SoilSensor::begin(uint32_t baud) {
    if (rs485Serial == nullptr) {
        Serial.println("RS485: Error - Serial no configurado");
        return false;
    }
    
    baudRate = baud;
    
    // Configurar pin TX Enable
    pinMode(txEnablePin, OUTPUT);
    disableTransmission();
    
    // Inicializar comunicación serial
    rs485Serial->begin(baudRate, SERIAL_8N1, RS485_RX_PIN, RS485_TX_PIN);
    
    // Configurar trama de petición base
    requestFrame[0] = deviceAddress;  // Dirección del dispositivo
    requestFrame[1] = 0x03;          // Función: Read Holding Registers
    requestFrame[2] = 0x00;          // Dirección inicial alta
    requestFrame[3] = 0x00;          // Dirección inicial baja
    requestFrame[4] = 0x00;          // Número de registros alta
    requestFrame[5] = 0x07;          // Número de registros baja (7 para 7-en-1)
    
    // Calcular CRC
    uint16_t crc = calculateCRC16(requestFrame, 6);
    requestFrame[6] = crc & 0xFF;         // CRC bajo
    requestFrame[7] = (crc >> 8) & 0xFF;  // CRC alto
    
    isInitialized = true;
    
    // Hacer primera lectura para verificar comunicación
    delay(100);
    if (readSensor7in1()) {
        Serial.println("RS485: Sensor 7-en-1 detectado");
    } else if (readSensor4in1()) {
        Serial.println("RS485: Sensor 4-en-1 detectado");
    } else if (readSensor3in1()) {
        Serial.println("RS485: Sensor 3-en-1 detectado");
    } else {
        Serial.println("RS485: Advertencia - No se pudo comunicar con el sensor");
    }
    
    Serial.printf("RS485: Inicializado - TX: %d, RX: %d, Baud: %lu\\n", 
                  RS485_TX_PIN, RS485_RX_PIN, baudRate);
    return true;
}

// cppcheck-suppress unusedFunction
bool RS485SoilSensor::readSensor() {
    // Intentar lectura con sensor 7-en-1 primero
    if (readSensor7in1()) {
        return true;
    }
    // Si falla, intentar con 4-en-1
    if (readSensor4in1()) {
        return true;
    }
    // Si falla, intentar con 3-en-1
    if (readSensor3in1()) {
        return true;
    }
    
    lastReadValid = false;
    return false;
}

bool RS485SoilSensor::readSensor7in1() {
    if (!isInitialized) {
        return false;
    }
    
    // Configurar para sensor 7-en-1
    requestFrame[5] = 0x07;  // 7 registros
    uint16_t crc = calculateCRC16(requestFrame, 6);
    requestFrame[6] = crc & 0xFF;
    requestFrame[7] = (crc >> 8) & 0xFF;
    
    if (sendRequest() && receiveResponse(19)) {  // 19 bytes para 7-en-1
        if (validateCRC(responseBuffer, 19)) {
            parseResponse7in1(responseBuffer);
            lastReading = millis();
            lastReadValid = isValidReading();
            return true;
        }
    }
    
    return false;
}

bool RS485SoilSensor::readSensor4in1() {
    if (!isInitialized) {
        return false;
    }
    
    // Configurar para sensor 4-en-1
    requestFrame[5] = 0x04;  // 4 registros
    uint16_t crc = calculateCRC16(requestFrame, 6);
    requestFrame[6] = crc & 0xFF;
    requestFrame[7] = (crc >> 8) & 0xFF;
    
    if (sendRequest() && receiveResponse(13)) {  // 13 bytes para 4-en-1
        if (validateCRC(responseBuffer, 13)) {
            parseResponse4in1(responseBuffer);
            lastReading = millis();
            lastReadValid = isValidReading();
            return true;
        }
    }
    
    return false;
}

bool RS485SoilSensor::readSensor3in1() {
    if (!isInitialized) {
        return false;
    }
    
    // Configurar para sensor 3-en-1
    requestFrame[5] = 0x03;  // 3 registros
    uint16_t crc = calculateCRC16(requestFrame, 6);
    requestFrame[6] = crc & 0xFF;
    requestFrame[7] = (crc >> 8) & 0xFF;
    
    if (sendRequest() && receiveResponse(11)) {  // 11 bytes para 3-en-1
        if (validateCRC(responseBuffer, 11)) {
            parseResponse3in1(responseBuffer);
            lastReading = millis();
            lastReadValid = isValidReading();
            return true;
        }
    }
    
    return false;
}

bool RS485SoilSensor::sendRequest() {
    if (!rs485Serial) {
        return false;
    }
    
    enableTransmission();
    delay(2);
    
    size_t bytesWritten = rs485Serial->write(requestFrame, 8);
    rs485Serial->flush();
    
    delay(2);
    disableTransmission();
    
    return (bytesWritten == 8);
}

bool RS485SoilSensor::receiveResponse(int expectedBytes) {
    unsigned long startTime = millis();
    int bytesReceived = 0;
    
    // Esperar respuesta (timeout 1000ms)
    while (bytesReceived < expectedBytes && (millis() - startTime) < 1000) {
        if (rs485Serial->available()) {
            responseBuffer[bytesReceived] = rs485Serial->read();
            bytesReceived++;
        }
        delay(1);
    }
    
    return (bytesReceived == expectedBytes);
}

uint16_t RS485SoilSensor::calculateCRC16(const uint8_t* data, uint8_t length) {
    uint16_t crc = 0xFFFF;
    
    for (uint8_t i = 0; i < length; i++) {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc = crc >> 1;
            }
        }
    }
    
    return crc;
}

bool RS485SoilSensor::validateCRC(uint8_t* data, uint8_t length) {
    if (length < 3) return false;
    
    uint16_t receivedCRC = data[length-2] | (data[length-1] << 8);
    uint16_t calculatedCRC = calculateCRC16(data, length-2);
    
    return (receivedCRC == calculatedCRC);
}

void RS485SoilSensor::parseResponse7in1(uint8_t* data) {
    // Formato: [Addr][Func][ByteCount][Moisture][Temp][EC][pH][N][P][K][CRC]
    // Índices:    0     1       2      3-4    5-6  7-8 9-10 11-12 13-14 15-16
    
    uint16_t moistureRaw = (data[3] << 8) | data[4];
    uint16_t tempRaw = (data[5] << 8) | data[6];
    uint16_t ecRaw = (data[7] << 8) | data[8];
    uint16_t phRaw = (data[9] << 8) | data[10];
    uint16_t nRaw = (data[11] << 8) | data[12];
    uint16_t pRaw = (data[13] << 8) | data[14];
    uint16_t kRaw = (data[15] << 8) | data[16];
    
    // Guardar muestras para promediado
    moistureSamples[currentSample] = moistureRaw * 0.1;
    temperatureSamples[currentSample] = tempRaw * 0.1;
    ecSamples[currentSample] = ecRaw;
    phSamples[currentSample] = phRaw * 0.1;
    nSamples[currentSample] = nRaw;
    pSamples[currentSample] = pRaw;
    kSamples[currentSample] = kRaw;
    
    currentSample = (currentSample + 1) % RS485_SAMPLES_COUNT;
    
    // Calcular promedios
    moisture = calculateAverageFloat(moistureSamples, RS485_SAMPLES_COUNT);
    temperature = calculateAverageFloat(temperatureSamples, RS485_SAMPLES_COUNT);
    electricalConductivity = calculateAverageFloat(ecSamples, RS485_SAMPLES_COUNT);
    pH = calculateAverageFloat(phSamples, RS485_SAMPLES_COUNT);
    nitrogen = calculateAverageUint16(nSamples, RS485_SAMPLES_COUNT);
    phosphorus = calculateAverageUint16(pSamples, RS485_SAMPLES_COUNT);
    potassium = calculateAverageUint16(kSamples, RS485_SAMPLES_COUNT);
}

void RS485SoilSensor::parseResponse4in1(uint8_t* data) {
    // Formato: [Addr][Func][ByteCount][Moisture][Temp][EC][pH][CRC]
    
    uint16_t moistureRaw = (data[3] << 8) | data[4];
    uint16_t tempRaw = (data[5] << 8) | data[6];
    uint16_t ecRaw = (data[7] << 8) | data[8];
    uint16_t phRaw = (data[9] << 8) | data[10];
    
    // Guardar muestras para promediado
    moistureSamples[currentSample] = moistureRaw * 0.1;
    temperatureSamples[currentSample] = tempRaw * 0.1;
    ecSamples[currentSample] = ecRaw;
    phSamples[currentSample] = phRaw * 0.1;
    
    // NPK no disponible en sensor 4-en-1
    nSamples[currentSample] = 0;
    pSamples[currentSample] = 0;
    kSamples[currentSample] = 0;
    
    currentSample = (currentSample + 1) % RS485_SAMPLES_COUNT;
    
    // Calcular promedios
    moisture = calculateAverageFloat(moistureSamples, RS485_SAMPLES_COUNT);
    temperature = calculateAverageFloat(temperatureSamples, RS485_SAMPLES_COUNT);
    electricalConductivity = calculateAverageFloat(ecSamples, RS485_SAMPLES_COUNT);
    pH = calculateAverageFloat(phSamples, RS485_SAMPLES_COUNT);
    nitrogen = 0;
    phosphorus = 0;
    potassium = 0;
}

void RS485SoilSensor::parseResponse3in1(uint8_t* data) {
    // Formato: [Addr][Func][ByteCount][Moisture][Temp][EC][CRC]
    
    uint16_t moistureRaw = (data[3] << 8) | data[4];
    uint16_t tempRaw = (data[5] << 8) | data[6];
    uint16_t ecRaw = (data[7] << 8) | data[8];
    
    // Guardar muestras para promediado
    moistureSamples[currentSample] = moistureRaw * 0.1;
    temperatureSamples[currentSample] = tempRaw * 0.1;
    ecSamples[currentSample] = ecRaw;
    
    // pH y NPK no disponibles en sensor 3-en-1
    phSamples[currentSample] = 7.0;  // Valor neutro por defecto
    nSamples[currentSample] = 0;
    pSamples[currentSample] = 0;
    kSamples[currentSample] = 0;
    
    currentSample = (currentSample + 1) % RS485_SAMPLES_COUNT;
    
    // Calcular promedios
    moisture = calculateAverageFloat(moistureSamples, RS485_SAMPLES_COUNT);
    temperature = calculateAverageFloat(temperatureSamples, RS485_SAMPLES_COUNT);
    electricalConductivity = calculateAverageFloat(ecSamples, RS485_SAMPLES_COUNT);
    pH = 7.0;  // Valor neutro para sensor 3-en-1
    nitrogen = 0;
    phosphorus = 0;
    potassium = 0;
}

// cppcheck-suppress unusedFunction
float RS485SoilSensor::getTemperature() {
    return temperature;
}

// cppcheck-suppress unusedFunction
float RS485SoilSensor::getMoisture() {
    return moisture;
}

// cppcheck-suppress unusedFunction
float RS485SoilSensor::getElectricalConductivity() {
    return electricalConductivity;
}

// cppcheck-suppress unusedFunction
float RS485SoilSensor::getPH() {
    return pH;
}

uint16_t RS485SoilSensor::getNitrogen() {
    return nitrogen;
}

uint16_t RS485SoilSensor::getPhosphorus() {
    return phosphorus;
}

uint16_t RS485SoilSensor::getPotassium() {
    return potassium;
}

bool RS485SoilSensor::isDataValid() {
    return lastReadValid && isInitialized;
}

// cppcheck-suppress unusedFunction
bool RS485SoilSensor::isReady() {
    return isInitialized;
}

String RS485SoilSensor::getSoilStatus() {
    if (!isDataValid()) return "Sin datos";
    
    bool moistureOK = isSoilOptimal();
    bool tempOK = isTemperatureOptimal();
    bool phOK = isPHOptimal();
    bool ecOK = isECOptimal();
    
    if (moistureOK && tempOK && phOK && ecOK) {
        return "Óptimo";
    } else if (!moistureOK && isSoilDry()) {
        return "Seco";
    } else if (!moistureOK && isSoilWet()) {
        return "Húmedo";
    } else {
        return "Requiere atención";
    }
}

String RS485SoilSensor::getNutrientStatus() {
    if (!isDataValid() || nitrogen == 0) return "No disponible";
    
    if (areNutrientsOptimal()) {
        return "Óptimo";
    } else {
        return "Requiere fertilización";
    }
}

String RS485SoilSensor::getMoistureLevel() {
    if (!isDataValid()) return "Sin datos";
    
    if (isSoilDry()) return "Seco";
    if (isSoilWet()) return "Húmedo";
    return "Óptimo";
}

String RS485SoilSensor::getTemperatureStatus() {
    if (!isDataValid()) return "Sin datos";
    
    if (temperature < 15.0) return "Frío";
    if (temperature > 30.0) return "Caliente";
    return "Óptimo";
}

bool RS485SoilSensor::isSoilDry() {
    return isDataValid() && moisture < 30.0;
}

bool RS485SoilSensor::isSoilOptimal() {
    return isDataValid() && moisture >= 30.0 && moisture <= 70.0;
}

bool RS485SoilSensor::isSoilWet() {
    return isDataValid() && moisture > 70.0;
}

bool RS485SoilSensor::isTemperatureOptimal() {
    return isDataValid() && temperature >= 15.0 && temperature <= 30.0;
}

bool RS485SoilSensor::isPHOptimal() {
    return isDataValid() && pH >= 6.0 && pH <= 7.5;
}

bool RS485SoilSensor::isECOptimal() {
    return isDataValid() && electricalConductivity >= 200 && electricalConductivity <= 2000;
}

bool RS485SoilSensor::areNutrientsOptimal() {
    if (!isDataValid() || nitrogen == 0) return false;
    
    // Rangos óptimos aproximados para NPK en mg/kg
    bool nOK = nitrogen >= 30 && nitrogen <= 200;
    bool pOK = phosphorus >= 15 && phosphorus <= 100;
    bool kOK = potassium >= 50 && potassium <= 300;
    
    return nOK && pOK && kOK;
}

// cppcheck-suppress unusedFunction
void RS485SoilSensor::setReadInterval(unsigned long interval) {
    readInterval = interval;
}

// cppcheck-suppress unusedFunction
void RS485SoilSensor::setDeviceAddress(uint8_t addr) {
    deviceAddress = addr;
    requestFrame[0] = deviceAddress;
}

// cppcheck-suppress unusedFunction
void RS485SoilSensor::setBaudRate(uint32_t baud) {
    baudRate = baud;
}

// cppcheck-suppress unusedFunction
void RS485SoilSensor::setSerial(HardwareSerial* serial, uint8_t txPin) {
    rs485Serial = serial;
    txEnablePin = txPin;
}

unsigned long RS485SoilSensor::getTimeSinceLastReading() {
    if (lastReading == 0) {
        return ULONG_MAX;
    }
    return millis() - lastReading;
}

// cppcheck-suppress unusedFunction
bool RS485SoilSensor::shouldRead() {
    return getTimeSinceLastReading() >= readInterval;
}

void RS485SoilSensor::printSoilData() {
    if (!isDataValid()) {
        Serial.println("RS485: Datos no válidos");
        return;
    }
    
    Serial.println("=== Sensor RS485 Suelo ===");
    Serial.printf("Temperatura: %.1f°C (%s)\\n", temperature, getTemperatureStatus().c_str());
    Serial.printf("Humedad: %.1f%% (%s)\\n", moisture, getMoistureLevel().c_str());
    Serial.printf("pH: %.1f (%s)\\n", pH, isPHOptimal() ? "Óptimo" : "Requiere ajuste");
    Serial.printf("EC: %.0f uS/cm (%s)\\n", electricalConductivity, isECOptimal() ? "Óptimo" : "Revisar");
    Serial.printf("Estado general: %s\\n", getSoilStatus().c_str());
}

void RS485SoilSensor::printNutrientData() {
    if (!isDataValid()) {
        Serial.println("RS485: Datos no válidos");
        return;
    }
    
    if (nitrogen == 0) {
        Serial.println("=== Nutrientes ===");
        Serial.println("No disponible (sensor 3-en-1 o 4-en-1)");
        return;
    }
    
    Serial.println("=== Nutrientes NPK ===");
    Serial.printf("Nitrógeno (N): %d mg/kg\\n", nitrogen);
    Serial.printf("Fósforo (P): %d mg/kg\\n", phosphorus);
    Serial.printf("Potasio (K): %d mg/kg\\n", potassium);
    Serial.printf("Estado: %s\\n", getNutrientStatus().c_str());
}

// cppcheck-suppress unusedFunction
void RS485SoilSensor::printAllData() {
    printSoilData();
    printNutrientData();
}

void RS485SoilSensor::resetSamples() {
    for (int i = 0; i < RS485_SAMPLES_COUNT; i++) {
        temperatureSamples[i] = 0.0;
        moistureSamples[i] = 0.0;
        ecSamples[i] = 0.0;
        phSamples[i] = 7.0;  // pH neutro por defecto
        nSamples[i] = 0;
        pSamples[i] = 0;
        kSamples[i] = 0;
    }
    currentSample = 0;
}

float RS485SoilSensor::calculateAverageFloat(const float samples[], int count) {
    float sum = 0.0;
    int validSamples = 0;
    
    for (int i = 0; i < count; i++) {
        if (samples[i] > 0.0) {
            sum += samples[i];
            validSamples++;
        }
    }
    
    if (validSamples > 0) {
        return sum / validSamples;
    }
    return 0.0;
}

uint16_t RS485SoilSensor::calculateAverageUint16(const uint16_t samples[], int count) {
    uint32_t sum = 0;
    int validSamples = 0;
    
    for (int i = 0; i < count; i++) {
        sum += samples[i];
        validSamples++;
    }
    
    if (validSamples > 0) {
        return sum / validSamples;
    }
    return 0;
}

void RS485SoilSensor::enableTransmission() {
    digitalWrite(txEnablePin, HIGH);
}

void RS485SoilSensor::disableTransmission() {
    digitalWrite(txEnablePin, LOW);
}

// cppcheck-suppress unusedFunction
bool RS485SoilSensor::isValidReading() {
    // Verificar que los valores estén en rangos razonables
    bool tempValid = temperature >= -40.0 && temperature <= 80.0;
    bool moistureValid = moisture >= 0.0 && moisture <= 100.0;
    bool phValid = pH >= 3.0 && pH <= 9.0;
    bool ecValid = electricalConductivity >= 0.0 && electricalConductivity <= 20000.0;
    
    return tempValid && moistureValid && phValid && ecValid;
}
