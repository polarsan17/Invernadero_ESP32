#include "actuators/FanActuator.h"
#include "config/config.h"

FanActuator::FanActuator(uint8_t pin) : relayPin(pin) {
    isOn = false;
    isInitialized = false;
    lastStateChange = 0;
    minStateChangeInterval = 5000; // 5 segundos mínimo entre cambios
    maxContinuousRunTime = 3600000; // 1 hora máximo continuo
    continuousRunStartTime = 0;
    isContinuousRunning = false;
}

FanActuator::~FanActuator() {
    if (isInitialized) {
        turnOff();
    }
}

bool FanActuator::begin() {
    pinMode(relayPin, OUTPUT);
    digitalWrite(relayPin, LOW); // Asegurar que esté apagado al inicio
    
    isInitialized = true;
    
    Serial.printf("[FanActuator] Ventilador inicializado en pin %d\n", relayPin);
    
    return true;
}

bool FanActuator::turnOn() {
    if (!isInitialized) {
        Serial.println("[FanActuator] ERROR: Ventilador no inicializado");
        return false;
    }
    
    unsigned long currentTime = millis();
    
    // Verificar intervalo mínimo entre cambios
    if (currentTime - lastStateChange < minStateChangeInterval) {
        Serial.println("[FanActuator] ADVERTENCIA: Intervalo mínimo no cumplido");
        return false;
    }
    
    if (!isOn) {
        digitalWrite(relayPin, HIGH);
        isOn = true;
        lastStateChange = currentTime;
        
        if (!isContinuousRunning) {
            continuousRunStartTime = currentTime;
            isContinuousRunning = true;
        }
        
        Serial.println("[FanActuator] Ventilador encendido");
    }
    
    return true;
}

bool FanActuator::turnOff() {
    if (!isInitialized) {
        Serial.println("[FanActuator] ERROR: Ventilador no inicializado");
        return false;
    }
    
    unsigned long currentTime = millis();
    
    // Verificar intervalo mínimo entre cambios
    if (currentTime - lastStateChange < minStateChangeInterval) {
        Serial.println("[FanActuator] ADVERTENCIA: Intervalo mínimo no cumplido");
        return false;
    }
    
    if (isOn) {
        digitalWrite(relayPin, LOW);
        isOn = false;
        lastStateChange = currentTime;
        isContinuousRunning = false;
        
        Serial.println("[FanActuator] Ventilador apagado");
    }
    
    return true;
}

// cppcheck-suppress unusedFunction
bool FanActuator::toggle() {
    if (isOn) {
        return turnOff();
    } else {
        return turnOn();
    }
}

// cppcheck-suppress unusedFunction
bool FanActuator::isRunning() {
    return isOn;
}

// cppcheck-suppress unusedFunction
bool FanActuator::isReady() {
    return isInitialized;
}

// cppcheck-suppress unusedFunction
void FanActuator::setMinStateChangeInterval(unsigned long interval) {
    minStateChangeInterval = interval;
    Serial.printf("[FanActuator] Intervalo mínimo configurado: %lu ms\n", interval);
}

// cppcheck-suppress unusedFunction
void FanActuator::setMaxContinuousRunTime(unsigned long maxTime) {
    maxContinuousRunTime = maxTime;
    Serial.printf("[FanActuator] Tiempo máximo continuo configurado: %lu ms\n", maxTime);
}

unsigned long FanActuator::getTimeSinceLastStateChange() {
    return millis() - lastStateChange;
}

unsigned long FanActuator::getContinuousRunTime() {
    if (isContinuousRunning) {
        return millis() - continuousRunStartTime;
    }
    return 0;
}

bool FanActuator::hasExceededMaxRunTime() {
    if (isContinuousRunning && maxContinuousRunTime > 0) {
        return getContinuousRunTime() > maxContinuousRunTime;
    }
    return false;
}

// cppcheck-suppress unusedFunction
void FanActuator::printStatus() {
    Serial.println("========== Estado del Ventilador ==========");
    Serial.printf("Inicializado: %s\n", isInitialized ? "Sí" : "No");
    Serial.printf("Pin de relé: %d\n", relayPin);
    Serial.printf("Estado: %s\n", isOn ? "ENCENDIDO" : "APAGADO");
    Serial.printf("Tiempo desde último cambio: %lu ms\n", getTimeSinceLastStateChange());
    
    if (isContinuousRunning) {
        Serial.printf("Tiempo de funcionamiento continuo: %lu ms\n", getContinuousRunTime());
        Serial.printf("¿Excede tiempo máximo?: %s\n", hasExceededMaxRunTime() ? "SÍ" : "No");
    }
    
    Serial.println("==========================================");
}

// cppcheck-suppress unusedFunction
int FanActuator::getBlynkState() {
    return isOn ? 1 : 0;
}

// cppcheck-suppress unusedFunction
bool FanActuator::setFromBlynk(int state) {
    Serial.printf("[FanActuator] Comando desde Blynk: %d\n", state);
    
    if (state == 1) {
        return turnOn();
    } else {
        return turnOff();
    }
}
