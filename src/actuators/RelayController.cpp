#include "actuators/RelayController.h"

RelayController::RelayController() 
    : activeChannels(0), isInitialized(false), debounceDelay(50) {
    // Inicializar todos los canales como inactivos
    for (uint8_t i = 0; i < MAX_RELAYS; i++) {
        relays[i].pin = 255;
        relays[i].isActive = false;
        relays[i].isInverted = false;
        relays[i].name = "";
        relays[i].lastToggleTime = 0;
    }
}

RelayController::~RelayController() {
    if (isInitialized) {
        deactivateAllRelays();
    }
}

bool RelayController::begin(unsigned long debounceMs) {
    debounceDelay = debounceMs;
    isInitialized = true;
    
    Serial.println("[RelayController] Controlador de relés inicializado");
    Serial.printf("[RelayController] Tiempo de debounce configurado: %lu ms\n", debounceDelay);
    
    // Asegurar que todos los relés estén desactivados al inicio (modo seguridad)
    deactivateAllRelays();
    
    return true;
}

// cppcheck-suppress unusedFunction
bool RelayController::configureRelay(uint8_t channel, uint8_t pin, const String& name, bool inverted) {
    if (!isValidChannel(channel)) {
        Serial.printf("[RelayController] ERROR: Canal inválido %d\n", channel);
        return false;
    }
    
    if (pin > 39) {  // ESP32 GPIO válidos
        Serial.printf("[RelayController] ERROR: Pin GPIO inválido %d\n", pin);
        return false;
    }
    
    // Configurar el pin como salida
    pinMode(pin, OUTPUT);
    
    // Configurar el canal
    relays[channel].pin = pin;
    relays[channel].name = name;
    relays[channel].isInverted = inverted;
    relays[channel].isActive = false;
    relays[channel].lastToggleTime = 0;
    
    // Establecer estado inicial (desactivado)
    writeRelayState(channel, false);
    
    activeChannels++;
    
    Serial.printf("[RelayController] Relé configurado - Canal: %d, Pin: %d, Nombre: %s, Invertido: %s\n", 
                  channel, pin, name.c_str(), inverted ? "Sí" : "No");
    
    return true;
}

bool RelayController::activateRelay(uint8_t channel) {
    if (!isValidChannel(channel) || relays[channel].pin == 255) {
        return false;
    }
    
    unsigned long currentTime = millis();
    if (currentTime - relays[channel].lastToggleTime < debounceDelay) {
        return false; // Debounce activo
    }
    
    relays[channel].isActive = true;
    relays[channel].lastToggleTime = currentTime;
    writeRelayState(channel, true);
    
    Serial.printf("[RelayController] Relé activado - Canal: %d (%s)\n", 
                  channel, relays[channel].name.c_str());
    
    return true;
}

bool RelayController::deactivateRelay(uint8_t channel) {
    if (!isValidChannel(channel) || relays[channel].pin == 255) {
        return false;
    }
    
    unsigned long currentTime = millis();
    if (currentTime - relays[channel].lastToggleTime < debounceDelay) {
        return false; // Debounce activo
    }
    
    relays[channel].isActive = false;
    relays[channel].lastToggleTime = currentTime;
    writeRelayState(channel, false);
    
    Serial.printf("[RelayController] Relé desactivado - Canal: %d (%s)\n", 
                  channel, relays[channel].name.c_str());
    
    return true;
}

// cppcheck-suppress unusedFunction
bool RelayController::toggleRelay(uint8_t channel) {
    if (!isValidChannel(channel) || relays[channel].pin == 255) {
        return false;
    }
    
    if (relays[channel].isActive) {
        return deactivateRelay(channel);
    } else {
        return activateRelay(channel);
    }
}

bool RelayController::getRelayState(uint8_t channel) const {
    if (!isValidChannel(channel)) {
        return false;
    }
    return relays[channel].isActive;
}

// cppcheck-suppress unusedFunction
String RelayController::getRelayName(uint8_t channel) const {
    if (!isValidChannel(channel)) {
        return "Canal Inválido";
    }
    return relays[channel].name.isEmpty() ? "Sin Nombre" : relays[channel].name;
}

uint8_t RelayController::getRelayPin(uint8_t channel) const {
    if (!isValidChannel(channel)) {
        return 255;
    }
    return relays[channel].pin;
}

void RelayController::deactivateAllRelays() {
    Serial.println("[RelayController] Desactivando todos los relés (modo seguridad)");
    
    for (uint8_t i = 0; i < MAX_RELAYS; i++) {
        if (relays[i].pin != 255) {
            relays[i].isActive = false;
            writeRelayState(i, false);
        }
    }
}

// cppcheck-suppress unusedFunction
void RelayController::setRelayMask(uint8_t mask) {
    Serial.printf("[RelayController] Configurando máscara de relés: 0x%02X\n", mask);
    
    for (uint8_t i = 0; i < MAX_RELAYS; i++) {
        if (relays[i].pin != 255) {
            bool shouldActivate = (mask & (1 << i)) != 0;
            if (shouldActivate) {
                activateRelay(i);
            } else {
                deactivateRelay(i);
            }
        }
    }
}

uint8_t RelayController::getRelayMask() const {
    uint8_t mask = 0;
    for (uint8_t i = 0; i < MAX_RELAYS; i++) {
        if (relays[i].isActive) {
            mask |= (1 << i);
        }
    }
    return mask;
}

uint8_t RelayController::getActiveChannelCount() const {
    return activeChannels;
}

// cppcheck-suppress unusedFunction
bool RelayController::isReady() const {
    return isInitialized;
}

// cppcheck-suppress unusedFunction
void RelayController::update() {
    // Método para tareas de mantenimiento en el loop principal
    // Actualmente no requiere procesamiento continuo, pero está disponible
    // para futuras funcionalidades como timers automáticos, etc.
}

// cppcheck-suppress unusedFunction
void RelayController::printStatus() const {
    Serial.println("========== Estado del Controlador de Relés ==========");
    Serial.printf("Inicializado: %s\n", isInitialized ? "Sí" : "No");
    Serial.printf("Canales configurados: %d\n", activeChannels);
    Serial.printf("Máscara actual: 0x%02X\n", getRelayMask());
    Serial.println("----------------------------------------------------");
    
    for (uint8_t i = 0; i < MAX_RELAYS; i++) {
        if (relays[i].pin != 255) {
            Serial.printf("Canal %d: %s | Pin: %d | Estado: %s | Invertido: %s\n",
                         i,
                         relays[i].name.c_str(),
                         relays[i].pin,
                         relays[i].isActive ? "ACTIVO" : "INACTIVO",
                         relays[i].isInverted ? "Sí" : "No");
        }
    }
    Serial.println("==================================================");
}

// cppcheck-suppress unusedFunction
int RelayController::getBlynkState(uint8_t channel) const {
    return getRelayState(channel) ? 1 : 0;
}

// cppcheck-suppress unusedFunction
bool RelayController::setFromBlynk(uint8_t channel, int state) {
    if (!isValidChannel(channel) || relays[channel].pin == 255) {
        return false;
    }
    
    Serial.printf("[RelayController] Comando desde Blynk - Canal: %d, Estado: %d\n", channel, state);
    
    if (state == 1) {
        return activateRelay(channel);
    } else {
        return deactivateRelay(channel);
    }
}

// Métodos privados

void RelayController::writeRelayState(uint8_t channel, bool state) {
    if (!isValidChannel(channel) || relays[channel].pin == 255) {
        return;
    }
    
    // Aplicar lógica invertida si está configurada
    bool physicalState = relays[channel].isInverted ? !state : state;
    digitalWrite(relays[channel].pin, physicalState ? HIGH : LOW);
}

bool RelayController::isValidChannel(uint8_t channel) const {
    return channel < MAX_RELAYS;
}
