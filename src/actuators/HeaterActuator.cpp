#include "actuators/HeaterActuator.h"
#include "config/config.h"

HeaterActuator::HeaterActuator(uint8_t pin) : relayPin(pin) {
    isOn = false;
    isInitialized = false;
    lastStateChange = 0;
    minStateChangeInterval = 15000; // 15 segundos mínimo entre cambios
    maxContinuousRunTime = 7200000; // 2 horas máximo continuo
    continuousRunStartTime = 0;
    isContinuousRunning = false;
    maxSafeTemperature = 40.0; // 40°C máximo por seguridad
    temperatureSafetyEnabled = true;
    totalRunTime = 0;
    activationCount = 0;
}

HeaterActuator::~HeaterActuator() {
    if (isInitialized) {
        emergencyShutdown();
    }
}

bool HeaterActuator::begin() {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW); // Asegurar que esté apagado al inicio
    
    isInitialized = true;
    
    Serial.printf("[HeaterActuator] Calefactor inicializado en pin %d\n", relayPin);
    Serial.printf("[HeaterActuator] Temperatura máxima de seguridad: %.1f°C\n", maxSafeTemperature);
    
    return true;
}

bool HeaterActuator::turnOn() {
    if (!isInitialized) {
        Serial.println("[HeaterActuator] ERROR: Calefactor no inicializado");
        return false;
    }
    
    unsigned long currentTime = millis();
    
    // Verificar intervalo mínimo entre cambios
    if (currentTime - lastStateChange < minStateChangeInterval) {
        Serial.println("[HeaterActuator] ADVERTENCIA: Intervalo mínimo no cumplido");
        return false;
    }
    
    if (!isOn) {
        digitalWrite(relayPin, HIGH);
        isOn = true;
        lastStateChange = currentTime;
        activationCount++;
        
        if (!isContinuousRunning) {
            continuousRunStartTime = currentTime;
            isContinuousRunning = true;
        }
        
        Serial.printf("[HeaterActuator] Calefactor encendido (Activación #%d)\n", activationCount);
    }
    
    return true;
}

bool HeaterActuator::turnOff() {
    if (!isInitialized) {
        Serial.println("[HeaterActuator] ERROR: Calefactor no inicializado");
        return false;
    }
    
    unsigned long currentTime = millis();
    
    // Verificar intervalo mínimo entre cambios (excepto en emergencia)
    if (currentTime - lastStateChange < minStateChangeInterval) {
        Serial.println("[HeaterActuator] ADVERTENCIA: Intervalo mínimo no cumplido");
        return false;
    }
    
    if (isOn) {
        // Actualizar estadísticas antes de apagar
        if (isContinuousRunning) {
            totalRunTime += currentTime - continuousRunStartTime;
            isContinuousRunning = false;
        }
        
        digitalWrite(relayPin, LOW);
        isOn = false;
        lastStateChange = currentTime;
        
        Serial.printf("[HeaterActuator] Calefactor apagado (Tiempo de funcionamiento: %lu ms)\n", 
                     currentTime - continuousRunStartTime);
    }
    
    return true;
}

// cppcheck-suppress unusedFunction
bool HeaterActuator::toggle() {
    if (isOn) {
        return turnOff();
    } else {
        return turnOn();
    }
}

// cppcheck-suppress unusedFunction
bool HeaterActuator::isRunning() {
    return isOn;
}

// cppcheck-suppress unusedFunction
bool HeaterActuator::isReady() {
    return isInitialized;
}

// cppcheck-suppress unusedFunction
void HeaterActuator::setMinStateChangeInterval(unsigned long interval) {
    minStateChangeInterval = interval;
    Serial.printf("[HeaterActuator] Intervalo mínimo configurado: %lu ms\n", interval);
}

// cppcheck-suppress unusedFunction
void HeaterActuator::setMaxContinuousRunTime(unsigned long maxTime) {
    maxContinuousRunTime = maxTime;
    Serial.printf("[HeaterActuator] Tiempo máximo continuo configurado: %lu ms\n", maxTime);
}

// cppcheck-suppress unusedFunction
void HeaterActuator::setMaxSafeTemperature(float maxTemp) {
    maxSafeTemperature = maxTemp;
    Serial.printf("[HeaterActuator] Temperatura máxima de seguridad configurada: %.1f°C\n", maxTemp);
}

// cppcheck-suppress unusedFunction
void HeaterActuator::enableTemperatureSafety(bool enable) {
    temperatureSafetyEnabled = enable;
    Serial.printf("[HeaterActuator] Sistema de seguridad por temperatura: %s\n", 
                  enable ? "HABILITADO" : "DESHABILITADO");
}

// cppcheck-suppress unusedFunction
bool HeaterActuator::checkTemperatureSafety(float currentTemperature) {
    if (!temperatureSafetyEnabled) {
        return true;
    }
    
    if (currentTemperature > maxSafeTemperature) {
        Serial.printf("[HeaterActuator] ¡ALERTA DE SEGURIDAD! Temperatura actual: %.1f°C > Máximo: %.1f°C\n", 
                     currentTemperature, maxSafeTemperature);
        
        if (isOn) {
            emergencyShutdown();
        }
        
        return false;
    }
    
    return true;
}

bool HeaterActuator::emergencyShutdown() {
    Serial.println("[HeaterActuator] ¡APAGADO DE EMERGENCIA ACTIVADO!");
    
    if (isOn) {
        // Actualizar estadísticas
        if (isContinuousRunning) {
            totalRunTime += millis() - continuousRunStartTime;
            isContinuousRunning = false;
        }
        
        digitalWrite(relayPin, LOW);
        isOn = false;
        lastStateChange = millis();
        
        Serial.println("[HeaterActuator] Calefactor apagado por seguridad");
    }
    
    return true;
}

unsigned long HeaterActuator::getTimeSinceLastStateChange() {
    return millis() - lastStateChange;
}

unsigned long HeaterActuator::getContinuousRunTime() {
    if (isContinuousRunning) {
        return millis() - continuousRunStartTime;
    }
    return 0;
}

bool HeaterActuator::hasExceededMaxRunTime() {
    if (isContinuousRunning && maxContinuousRunTime > 0) {
        return getContinuousRunTime() > maxContinuousRunTime;
    }
    return false;
}

unsigned long HeaterActuator::getTotalRunTime() {
    unsigned long currentTotal = totalRunTime;
    if (isContinuousRunning) {
        currentTotal += getContinuousRunTime();
    }
    return currentTotal;
}

// cppcheck-suppress unusedFunction
unsigned int HeaterActuator::getActivationCount() {
    return activationCount;
}

// cppcheck-suppress unusedFunction
void HeaterActuator::resetStatistics() {
    totalRunTime = 0;
    activationCount = 0;
    Serial.println("[HeaterActuator] Estadísticas reiniciadas");
}

// cppcheck-suppress unusedFunction
void HeaterActuator::printStatus() {
    Serial.println("========== Estado del Calefactor ==========");
    Serial.printf("Inicializado: %s\n", isInitialized ? "Sí" : "No");
    Serial.printf("Pin de relé: %d\n", relayPin);
    Serial.printf("Estado: %s\n", isOn ? "ENCENDIDO" : "APAGADO");
    Serial.printf("Tiempo desde último cambio: %lu ms\n", getTimeSinceLastStateChange());
    Serial.printf("Número de activaciones: %d\n", activationCount);
    Serial.printf("Tiempo total de funcionamiento: %lu ms\n", getTotalRunTime());
    Serial.printf("Temperatura máxima de seguridad: %.1f°C\n", maxSafeTemperature);
    Serial.printf("Sistema de seguridad: %s\n", temperatureSafetyEnabled ? "HABILITADO" : "DESHABILITADO");
    
    if (isContinuousRunning) {
        Serial.printf("Tiempo de funcionamiento continuo: %lu ms\n", getContinuousRunTime());
        Serial.printf("¿Excede tiempo máximo?: %s\n", hasExceededMaxRunTime() ? "SÍ" : "No");
    }
    
    Serial.println("==========================================");
}

// cppcheck-suppress unusedFunction
int HeaterActuator::getBlynkState() {
    return isOn ? 1 : 0;
}

// cppcheck-suppress unusedFunction
bool HeaterActuator::setFromBlynk(int state) {
    Serial.printf("[HeaterActuator] Comando desde Blynk: %d\n", state);
    
    if (state == 1) {
        return turnOn();
    } else {
        return turnOff();
    }
}
