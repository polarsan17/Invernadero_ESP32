#include "actuators/WaterPumpActuator.h"
#include "config/config.h"

WaterPumpActuator::WaterPumpActuator(uint8_t pin) : relayPin(pin) {
    isOn = false;
    isInitialized = false;
    lastStateChange = 0;
    minStateChangeInterval = 10000; // 10 segundos mínimo entre cambios
    maxContinuousRunTime = 1800000; // 30 minutos máximo continuo
    continuousRunStartTime = 0;
    isContinuousRunning = false;
    totalRunTime = 0;
    activationCount = 0;
}

WaterPumpActuator::~WaterPumpActuator() {
    if (isInitialized) {
        turnOff();
    }
}

bool WaterPumpActuator::begin() {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW); // Asegurar que esté apagada al inicio
    
    isInitialized = true;
    
    Serial.printf("[WaterPumpActuator] Bomba de agua inicializada en pin %d\n", relayPin);
    
    return true;
}

bool WaterPumpActuator::turnOn() {
    if (!isInitialized) {
        Serial.println("[WaterPumpActuator] ERROR: Bomba no inicializada");
        return false;
    }
    
    unsigned long currentTime = millis();
    
    // Verificar intervalo mínimo entre cambios
    if (currentTime - lastStateChange < minStateChangeInterval) {
        Serial.println("[WaterPumpActuator] ADVERTENCIA: Intervalo mínimo no cumplido");
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
        
        Serial.printf("[WaterPumpActuator] Bomba encendida (Activación #%d)\n", activationCount);
    }
    
    return true;
}

bool WaterPumpActuator::turnOff() {
    if (!isInitialized) {
        Serial.println("[WaterPumpActuator] ERROR: Bomba no inicializada");
        return false;
    }
    
    unsigned long currentTime = millis();
    
    // Verificar intervalo mínimo entre cambios
    if (currentTime - lastStateChange < minStateChangeInterval) {
        Serial.println("[WaterPumpActuator] ADVERTENCIA: Intervalo mínimo no cumplido");
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
        
        Serial.printf("[WaterPumpActuator] Bomba apagada (Tiempo de funcionamiento: %lu ms)\n", 
                     currentTime - continuousRunStartTime);
    }
    
    return true;
}

// cppcheck-suppress unusedFunction
bool WaterPumpActuator::toggle() {
    if (isOn) {
        return turnOff();
    } else {
        return turnOn();
    }
}

// cppcheck-suppress unusedFunction
bool WaterPumpActuator::isRunning() {
    return isOn;
}

// cppcheck-suppress unusedFunction
bool WaterPumpActuator::isReady() {
    return isInitialized;
}

// cppcheck-suppress unusedFunction
void WaterPumpActuator::setMinStateChangeInterval(unsigned long interval) {
    minStateChangeInterval = interval;
    Serial.printf("[WaterPumpActuator] Intervalo mínimo configurado: %lu ms\n", interval);
}

// cppcheck-suppress unusedFunction
void WaterPumpActuator::setMaxContinuousRunTime(unsigned long maxTime) {
    maxContinuousRunTime = maxTime;
    Serial.printf("[WaterPumpActuator] Tiempo máximo continuo configurado: %lu ms\n", maxTime);
}

unsigned long WaterPumpActuator::getTimeSinceLastStateChange() {
    return millis() - lastStateChange;
}

unsigned long WaterPumpActuator::getContinuousRunTime() {
    if (isContinuousRunning) {
        return millis() - continuousRunStartTime;
    }
    return 0;
}

bool WaterPumpActuator::hasExceededMaxRunTime() {
    if (isContinuousRunning && maxContinuousRunTime > 0) {
        return getContinuousRunTime() > maxContinuousRunTime;
    }
    return false;
}

unsigned long WaterPumpActuator::getTotalRunTime() {
    unsigned long currentTotal = totalRunTime;
    if (isContinuousRunning) {
        currentTotal += getContinuousRunTime();
    }
    return currentTotal;
}

// cppcheck-suppress unusedFunction
unsigned int WaterPumpActuator::getActivationCount() {
    return activationCount;
}

// cppcheck-suppress unusedFunction
void WaterPumpActuator::resetStatistics() {
    totalRunTime = 0;
    activationCount = 0;
    Serial.println("[WaterPumpActuator] Estadísticas reiniciadas");
}

// cppcheck-suppress unusedFunction
void WaterPumpActuator::printStatus() {
    Serial.println("========== Estado de la Bomba de Agua ==========");
    Serial.printf("Inicializada: %s\n", isInitialized ? "Sí" : "No");
    Serial.printf("Pin de relé: %d\n", relayPin);
    Serial.printf("Estado: %s\n", isOn ? "ENCENDIDA" : "APAGADA");
    Serial.printf("Tiempo desde último cambio: %lu ms\n", getTimeSinceLastStateChange());
    Serial.printf("Número de activaciones: %d\n", activationCount);
    Serial.printf("Tiempo total de funcionamiento: %lu ms\n", getTotalRunTime());
    
    if (isContinuousRunning) {
        Serial.printf("Tiempo de funcionamiento continuo: %lu ms\n", getContinuousRunTime());
        Serial.printf("¿Excede tiempo máximo?: %s\n", hasExceededMaxRunTime() ? "SÍ" : "No");
    }
    
    Serial.println("===============================================");
}

// cppcheck-suppress unusedFunction
int WaterPumpActuator::getBlynkState() {
    return isOn ? 1 : 0;
}

// cppcheck-suppress unusedFunction
bool WaterPumpActuator::setFromBlynk(int state) {
    Serial.printf("[WaterPumpActuator] Comando desde Blynk: %d\n", state);
    
    if (state == 1) {
        return turnOn();
    } else {
        return turnOff();
    }
}
