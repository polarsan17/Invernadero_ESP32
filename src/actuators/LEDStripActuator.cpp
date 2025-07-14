#include "actuators/LEDStripActuator.h"
#include "config/config.h"

LEDStripActuator::LEDStripActuator(uint8_t pin, bool enablePWM, uint8_t pwmCh) 
    : relayPin(pin),
      isOn(false),
      isInitialized(false),
      lastStateChange(0),
      minStateChangeInterval(1000),
      maxContinuousRunTime(43200000),
      continuousRunStartTime(0),
      isContinuousRunning(false),
      brightness(255),
      supportsPWM(enablePWM),
      pwmChannel(pwmCh),
      totalRunTime(0),
      activationCount(0) {
    // Constructor completado con lista de inicialización
}

LEDStripActuator::~LEDStripActuator() {
    if (isInitialized) {
        turnOff();
    }
}

bool LEDStripActuator::begin() {
    if (supportsPWM) {
        // Configurar PWM para control de intensidad
        ledcSetup(pwmChannel, 5000, 8); // 5kHz, 8 bits de resolución
        ledcAttachPin(relayPin, pwmChannel);
        ledcWrite(pwmChannel, 0); // Apagado inicial
        
        Serial.printf("[LEDStripActuator] Tira LED con PWM inicializada en pin %d (Canal PWM: %d)\n", 
                     relayPin, pwmChannel);
    } else {
        // Configurar como salida digital simple
        pinMode(relayPin, OUTPUT);
        digitalWrite(relayPin, LOW); // Apagado inicial
        
        Serial.printf("[LEDStripActuator] Tira LED digital inicializada en pin %d\n", relayPin);
    }
    
    isInitialized = true;
    
    return true;
}

bool LEDStripActuator::turnOn() {
    if (!isInitialized) {
        Serial.println("[LEDStripActuator] ERROR: Tira LED no inicializada");
        return false;
    }
    
    unsigned long currentTime = millis();
    
    // Verificar intervalo mínimo entre cambios
    if (currentTime - lastStateChange < minStateChangeInterval) {
        Serial.println("[LEDStripActuator] ADVERTENCIA: Intervalo mínimo no cumplido");
        return false;
    }
    
    if (!isOn) {
        if (supportsPWM) {
            ledcWrite(pwmChannel, brightness);
        } else {
            digitalWrite(relayPin, HIGH);
        }
        
        isOn = true;
        lastStateChange = currentTime;
        activationCount++;
        
        if (!isContinuousRunning) {
            continuousRunStartTime = currentTime;
            isContinuousRunning = true;
        }
        
        Serial.printf("[LEDStripActuator] Tira LED encendida (Activación #%d", activationCount);
        if (supportsPWM) {
            Serial.printf(", Brillo: %d", brightness);
        }
        Serial.println(")");
    }
    
    return true;
}

bool LEDStripActuator::turnOff() {
    if (!isInitialized) {
        Serial.println("[LEDStripActuator] ERROR: Tira LED no inicializada");
        return false;
    }
    
    unsigned long currentTime = millis();
    
    // Verificar intervalo mínimo entre cambios
    if (currentTime - lastStateChange < minStateChangeInterval) {
        Serial.println("[LEDStripActuator] ADVERTENCIA: Intervalo mínimo no cumplido");
        return false;
    }
    
    if (isOn) {
        // Actualizar estadísticas antes de apagar
        if (isContinuousRunning) {
            totalRunTime += currentTime - continuousRunStartTime;
            isContinuousRunning = false;
        }
        
        if (supportsPWM) {
            ledcWrite(pwmChannel, 0);
        } else {
            digitalWrite(relayPin, LOW);
        }
        
        isOn = false;
        lastStateChange = currentTime;
        
        Serial.printf("[LEDStripActuator] Tira LED apagada (Tiempo de funcionamiento: %lu ms)\n", 
                     currentTime - continuousRunStartTime);
    }
    
    return true;
}

// cppcheck-suppress unusedFunction
bool LEDStripActuator::toggle() {
    if (isOn) {
        return turnOff();
    } else {
        return turnOn();
    }
}

bool LEDStripActuator::setBrightness(uint8_t newBrightness) {
    if (!supportsPWM) {
        Serial.println("[LEDStripActuator] ADVERTENCIA: Control de brillo no disponible sin PWM");
        return false;
    }
    
    brightness = newBrightness;
    
    if (isOn) {
        ledcWrite(pwmChannel, brightness);
    }
    
    Serial.printf("[LEDStripActuator] Brillo configurado: %d\n", brightness);
    
    return true;
}

uint8_t LEDStripActuator::getBrightness() {
    return brightness;
}

// cppcheck-suppress unusedFunction
bool LEDStripActuator::fadeIn(unsigned long duration) {
    if (!supportsPWM || !isInitialized) {
        return false;
    }
    
    Serial.printf("[LEDStripActuator] Iniciando fade in (%lu ms)\n", duration);
    
    unsigned long startTime = millis();
    unsigned long elapsed;
    uint8_t currentBrightness;
    
    isOn = true;
    if (!isContinuousRunning) {
        continuousRunStartTime = startTime;
        isContinuousRunning = true;
        activationCount++;
    }
    
    while ((elapsed = millis() - startTime) < duration) {
        currentBrightness = map(elapsed, 0, duration, 0, brightness);
        ledcWrite(pwmChannel, currentBrightness);
        delay(10);
    }
    
    ledcWrite(pwmChannel, brightness);
    lastStateChange = millis();
    
    Serial.println("[LEDStripActuator] Fade in completado");
    return true;
}

// cppcheck-suppress unusedFunction
bool LEDStripActuator::fadeOut(unsigned long duration) {
    if (!supportsPWM || !isInitialized || !isOn) {
        return false;
    }
    
    Serial.printf("[LEDStripActuator] Iniciando fade out (%lu ms)\n", duration);
    
    unsigned long startTime = millis();
    unsigned long elapsed;
    uint8_t currentBrightness;
    
    while ((elapsed = millis() - startTime) < duration) {
        currentBrightness = map(elapsed, 0, duration, brightness, 0);
        ledcWrite(pwmChannel, currentBrightness);
        delay(10);
    }
    
    ledcWrite(pwmChannel, 0);
    
    // Actualizar estadísticas
    if (isContinuousRunning) {
        totalRunTime += millis() - continuousRunStartTime;
        isContinuousRunning = false;
    }
    
    isOn = false;
    lastStateChange = millis();
    
    Serial.println("[LEDStripActuator] Fade out completado");
    return true;
}

// cppcheck-suppress unusedFunction
bool LEDStripActuator::isRunning() {
    return isOn;
}

// cppcheck-suppress unusedFunction
bool LEDStripActuator::isReady() {
    return isInitialized;
}

// cppcheck-suppress unusedFunction
bool LEDStripActuator::supportsBrightness() {
    return supportsPWM;
}

// cppcheck-suppress unusedFunction
void LEDStripActuator::setMinStateChangeInterval(unsigned long interval) {
    minStateChangeInterval = interval;
    Serial.printf("[LEDStripActuator] Intervalo mínimo configurado: %lu ms\n", interval);
}

// cppcheck-suppress unusedFunction
void LEDStripActuator::setMaxContinuousRunTime(unsigned long maxTime) {
    maxContinuousRunTime = maxTime;
    Serial.printf("[LEDStripActuator] Tiempo máximo continuo configurado: %lu ms\n", maxTime);
}

unsigned long LEDStripActuator::getTimeSinceLastStateChange() {
    return millis() - lastStateChange;
}

unsigned long LEDStripActuator::getContinuousRunTime() {
    if (isContinuousRunning) {
        return millis() - continuousRunStartTime;
    }
    return 0;
}

bool LEDStripActuator::hasExceededMaxRunTime() {
    if (isContinuousRunning && maxContinuousRunTime > 0) {
        return getContinuousRunTime() > maxContinuousRunTime;
    }
    return false;
}

unsigned long LEDStripActuator::getTotalRunTime() {
    unsigned long currentTotal = totalRunTime;
    if (isContinuousRunning) {
        currentTotal += getContinuousRunTime();
    }
    return currentTotal;
}

// cppcheck-suppress unusedFunction
unsigned int LEDStripActuator::getActivationCount() {
    return activationCount;
}

// cppcheck-suppress unusedFunction
void LEDStripActuator::resetStatistics() {
    totalRunTime = 0;
    activationCount = 0;
    Serial.println("[LEDStripActuator] Estadísticas reiniciadas");
}

// cppcheck-suppress unusedFunction
void LEDStripActuator::printStatus() {
    Serial.println("========== Estado de la Tira LED ==========");
    Serial.printf("Inicializada: %s\n", isInitialized ? "Sí" : "No");
    Serial.printf("Pin: %d\n", relayPin);
    Serial.printf("Soporte PWM: %s\n", supportsPWM ? "Sí" : "No");
    if (supportsPWM) {
        Serial.printf("Canal PWM: %d\n", pwmChannel);
        Serial.printf("Brillo: %d\n", brightness);
    }
    Serial.printf("Estado: %s\n", isOn ? "ENCENDIDA" : "APAGADA");
    Serial.printf("Tiempo desde último cambio: %lu ms\n", getTimeSinceLastStateChange());
    Serial.printf("Número de activaciones: %d\n", activationCount);
    Serial.printf("Tiempo total de funcionamiento: %lu ms\n", getTotalRunTime());
    
    if (isContinuousRunning) {
        Serial.printf("Tiempo de funcionamiento continuo: %lu ms\n", getContinuousRunTime());
        Serial.printf("¿Excede tiempo máximo?: %s\n", hasExceededMaxRunTime() ? "SÍ" : "No");
    }
    
    Serial.println("==========================================");
}

// cppcheck-suppress unusedFunction
int LEDStripActuator::getBlynkState() {
    return isOn ? 1 : 0;
}

// cppcheck-suppress unusedFunction
bool LEDStripActuator::setFromBlynk(int state) {
    Serial.printf("[LEDStripActuator] Comando desde Blynk: %d\n", state);
    
    if (state == 1) {
        return turnOn();
    } else {
        return turnOff();
    }
}

// cppcheck-suppress unusedFunction
bool LEDStripActuator::setBrightnessFromBlynk(int blynkBrightness) {
    if (!supportsPWM) {
        return false;
    }
    
    // Blynk normalmente envía valores 0-100, convertir a 0-255
    uint8_t newBrightness = map(blynkBrightness, 0, 100, 0, 255);
    
    Serial.printf("[LEDStripActuator] Brillo desde Blynk: %d -> %d\n", blynkBrightness, newBrightness);
    
    return setBrightness(newBrightness);
}
