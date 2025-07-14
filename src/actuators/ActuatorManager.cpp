#include "actuators/ActuatorManager.h"
#include "config/config.h"

ActuatorManager::ActuatorManager(BlynkManager& blynk) {
    fanActuator = nullptr;
    waterPumpActuator = nullptr;
    heaterActuator = nullptr;
    ledStripActuator = nullptr;
    servoActuator = nullptr;
    blynkManager = &blynk;
    
    lastBlynkUpdate = 0;
    blynkUpdateInterval = 5000; // 5 segundos
    actuatorsInitialized = false;
}

ActuatorManager::~ActuatorManager() {
    if (actuatorsInitialized) {
        desactivarTodos();
    }
    
    delete fanActuator;
    delete waterPumpActuator;
    delete heaterActuator;
    delete ledStripActuator;
    delete servoActuator;
}

bool ActuatorManager::begin() {
    Serial.println("[ActuatorManager] Inicializando gestor de actuadores...");
    
    // Configuración de pines por defecto para ESP32
    // Estos pines pueden ser ajustados según el hardware específico
    
    // Inicializar ventilador
    fanActuator = new FanActuator(25); // Pin GPIO 25
    if (!fanActuator->begin()) {
        Serial.println("[ActuatorManager] ERROR: No se pudo inicializar el ventilador");
        return false;
    }
    
    // Inicializar bomba de agua
    waterPumpActuator = new WaterPumpActuator(26); // Pin GPIO 26
    if (!waterPumpActuator->begin()) {
        Serial.println("[ActuatorManager] ERROR: No se pudo inicializar la bomba de agua");
        return false;
    }
    
    // Inicializar calefactor
    heaterActuator = new HeaterActuator(27); // Pin GPIO 27
    if (!heaterActuator->begin()) {
        Serial.println("[ActuatorManager] ERROR: No se pudo inicializar el calefactor");
        return false;
    }
    
    // Inicializar tira LED con soporte PWM
    ledStripActuator = new LEDStripActuator(32, true, 0); // Pin GPIO 32, PWM canal 0
    if (!ledStripActuator->begin()) {
        Serial.println("[ActuatorManager] ERROR: No se pudo inicializar la tira LED");
        return false;
    }
    
    // Inicializar servomotor
    servoActuator = new ServoActuator(33); // Pin GPIO 33
    if (!servoActuator->begin()) {
        Serial.println("[ActuatorManager] ERROR: No se pudo inicializar el servomotor");
        return false;
    }
    
    // Configurar posiciones del servo
    servoActuator->setOpenPosition(0);   // 0° = ventilador libre
    servoActuator->setClosedPosition(90); // 90° = ventilador tapado
    
    actuatorsInitialized = true;
    
    Serial.println("[ActuatorManager] Todos los actuadores inicializados correctamente");
    
    // Imprimir estado inicial
    imprimirEstado();
    
    return true;
}

void ActuatorManager::update() {
    if (!actuatorsInitialized) {
        return;
    }
    
    // Actualizar servo para movimiento suave
    if (servoActuator) {
        servoActuator->update();
    }
    
    // Verificar seguridad del calefactor (ejemplo básico)
    if (heaterActuator && heaterActuator->hasExceededMaxRunTime()) {
        Serial.println("[ActuatorManager] ADVERTENCIA: Calefactor ha excedido tiempo máximo de funcionamiento");
        heaterActuator->turnOff();
    }
    
    // Verificar seguridad de la bomba
    if (waterPumpActuator && waterPumpActuator->hasExceededMaxRunTime()) {
        Serial.println("[ActuatorManager] ADVERTENCIA: Bomba ha excedido tiempo máximo de funcionamiento");
        waterPumpActuator->turnOff();
    }
    
    // Enviar datos a Blynk periódicamente
    unsigned long currentTime = millis();
    if (currentTime - lastBlynkUpdate >= blynkUpdateInterval) {
        sendDataToBlynk();
        lastBlynkUpdate = currentTime;
    }
}

void ActuatorManager::sendDataToBlynk() {
    if (!blynkManager || !actuatorsInitialized) {
        return;
    }
    
    // Enviar estados de actuadores a Blynk usando pines virtuales
    // Estos valores pueden ser ajustados según la configuración de Blynk
    
    if (fanActuator) {
        blynkManager->sendVirtualPin(10, fanActuator->getBlynkState());
    }
    
    if (waterPumpActuator) {
        blynkManager->sendVirtualPin(11, waterPumpActuator->getBlynkState());
    }
    
    if (heaterActuator) {
        blynkManager->sendVirtualPin(12, heaterActuator->getBlynkState());
    }
    
    if (ledStripActuator) {
        blynkManager->sendVirtualPin(13, ledStripActuator->getBlynkState());
        if (ledStripActuator->supportsBrightness()) {
            // Enviar brillo como porcentaje
            int brightness = map(ledStripActuator->getBrightness(), 0, 255, 0, 100);
            blynkManager->sendVirtualPin(14, brightness);
        }
    }
    
    if (servoActuator) {
        blynkManager->sendVirtualPin(15, servoActuator->getBlynkState());
        // Enviar posición actual como porcentaje
        int position = map(servoActuator->getCurrentPosition(), 0, 180, 0, 100);
        blynkManager->sendVirtualPin(16, position);
    }
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::areActuatorsReady() {
    return actuatorsInitialized &&
           fanActuator && fanActuator->isReady() &&
           waterPumpActuator && waterPumpActuator->isReady() &&
           heaterActuator && heaterActuator->isReady() &&
           ledStripActuator && ledStripActuator->isReady() &&
           servoActuator && servoActuator->isReady();
}

// Getters para acceso directo a actuadores
// cppcheck-suppress unusedFunction
FanActuator* ActuatorManager::getFan() {
    return fanActuator;
}

// cppcheck-suppress unusedFunction
WaterPumpActuator* ActuatorManager::getWaterPump() {
    return waterPumpActuator;
}

// cppcheck-suppress unusedFunction
HeaterActuator* ActuatorManager::getHeater() {
    return heaterActuator;
}

// cppcheck-suppress unusedFunction
LEDStripActuator* ActuatorManager::getLEDStrip() {
    return ledStripActuator;
}

// cppcheck-suppress unusedFunction
ServoActuator* ActuatorManager::getServo() {
    return servoActuator;
}

// Métodos de control directo (para compatibilidad)
// cppcheck-suppress unusedFunction
bool ActuatorManager::activarVentilador() {
    return fanActuator ? fanActuator->turnOn() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::desactivarVentilador() {
    return fanActuator ? fanActuator->turnOff() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::obtenerEstadoVentilador() const {
    return fanActuator ? fanActuator->isRunning() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::activarBombaAgua() {
    return waterPumpActuator ? waterPumpActuator->turnOn() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::desactivarBombaAgua() {
    return waterPumpActuator ? waterPumpActuator->turnOff() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::obtenerEstadoBombaAgua() const {
    return waterPumpActuator ? waterPumpActuator->isRunning() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::activarCalefactor() {
    return heaterActuator ? heaterActuator->turnOn() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::desactivarCalefactor() {
    return heaterActuator ? heaterActuator->turnOff() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::obtenerEstadoCalefactor() const {
    return heaterActuator ? heaterActuator->isRunning() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::activarTiraLED() {
    return ledStripActuator ? ledStripActuator->turnOn() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::desactivarTiraLED() {
    return ledStripActuator ? ledStripActuator->turnOff() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::obtenerEstadoTiraLED() const {
    return ledStripActuator ? ledStripActuator->isRunning() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::abrirVentilacion() {
    return servoActuator ? servoActuator->openVent() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::cerrarVentilacion() {
    return servoActuator ? servoActuator->closeVent() : false;
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::obtenerEstadoVentilacion() const {
    return servoActuator ? servoActuator->isOpen() : false;
}

void ActuatorManager::desactivarTodos() {
    Serial.println("[ActuatorManager] Desactivando todos los actuadores");
    
    if (fanActuator) fanActuator->turnOff();
    if (waterPumpActuator) waterPumpActuator->turnOff();
    if (heaterActuator) heaterActuator->turnOff();
    if (ledStripActuator) ledStripActuator->turnOff();
    // El servo mantiene su posición actual
}

// cppcheck-suppress unusedFunction
void ActuatorManager::modoSeguridad() {
    Serial.println("[ActuatorManager] ¡MODO SEGURIDAD ACTIVADO!");
    
    // Apagar actuadores críticos
    if (heaterActuator) {
        heaterActuator->emergencyShutdown();
    }
    
    if (waterPumpActuator) {
        waterPumpActuator->turnOff();
    }
    
    if (fanActuator) {
        fanActuator->turnOff();
    }
    
    if (ledStripActuator) {
        ledStripActuator->turnOff();
    }
    
    // Abrir ventilación en caso de emergencia
    if (servoActuator) {
        servoActuator->openVent();
    }
}

void ActuatorManager::imprimirEstado() const {
    Serial.println("========== Estado del Gestor de Actuadores ==========");
    Serial.printf("Inicializado: %s\n", actuatorsInitialized ? "Sí" : "No");
    
    if (actuatorsInitialized) {
        Serial.println("Estado de actuadores:");
        
        if (fanActuator) {
            Serial.printf("  Ventilador: %s\n", fanActuator->isRunning() ? "ACTIVO" : "INACTIVO");
        }
        
        if (waterPumpActuator) {
            Serial.printf("  Bomba de Agua: %s\n", waterPumpActuator->isRunning() ? "ACTIVA" : "INACTIVA");
        }
        
        if (heaterActuator) {
            Serial.printf("  Calefactor: %s\n", heaterActuator->isRunning() ? "ACTIVO" : "INACTIVO");
        }
        
        if (ledStripActuator) {
            Serial.printf("  Tira LED: %s", ledStripActuator->isRunning() ? "ACTIVA" : "INACTIVA");
            if (ledStripActuator->supportsBrightness()) {
                Serial.printf(" (Brillo: %d)", ledStripActuator->getBrightness());
            }
            Serial.println();
        }
        
        if (servoActuator) {
            Serial.printf("  Servo Ventilación: %d° (%s)\n", 
                         servoActuator->getCurrentPosition(),
                         servoActuator->isOpen() ? "ABIERTO" : 
                         (servoActuator->isClosed() ? "CERRADO" : "INTERMEDIO"));
        }
    }
    Serial.println("==================================================");
}

// cppcheck-suppress unusedFunction
bool ActuatorManager::controlarDesdeBlynk(uint8_t actuador, int estado) {
    Serial.printf("[ActuatorManager] Control desde Blynk - Actuador: %d, Estado: %d\n", actuador, estado);
    
    switch (actuador) {
        case 0: // Ventilador
            return fanActuator ? fanActuator->setFromBlynk(estado) : false;
            
        case 1: // Bomba de agua
            return waterPumpActuator ? waterPumpActuator->setFromBlynk(estado) : false;
            
        case 2: // Calefactor
            return heaterActuator ? heaterActuator->setFromBlynk(estado) : false;
            
        case 3: // Tira LED
            return ledStripActuator ? ledStripActuator->setFromBlynk(estado) : false;
            
        case 4: // Servo
            return servoActuator ? servoActuator->setFromBlynk(estado) : false;
            
        default:
            Serial.printf("[ActuatorManager] ERROR: Actuador %d no válido\n", actuador);
            return false;
    }
}
