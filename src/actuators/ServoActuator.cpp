#include "actuators/ServoActuator.h"
#include "config/config.h"

ServoActuator::ServoActuator(uint8_t pin) : servoPin(pin) {
    currentPosition = 90; // Posición inicial central
    targetPosition = 90;
    openPosition = 90;   // Abierto (horizontal, no tapa)
    closedPosition = 0;  // Cerrado (vertical, tapa completamente)
    minPosition = 0;
    maxPosition = 180;
    isInitialized = false;
    isMoving = false;
    moveInterval = 50;   // 50ms entre pasos para movimiento suave
    lastMoveTime = 0;
    moveCount = 0;
    totalMoves = 0;
    moveSpeed = 1;       // Velocidad de movimiento
}

ServoActuator::~ServoActuator() {
    if (isInitialized) {
        servo.detach();
    }
}

bool ServoActuator::begin() {
    // Configurar servo con ESP32Servo
    ESP32PWM::allocateTimer(0);
    ESP32PWM::allocateTimer(1);
    ESP32PWM::allocateTimer(2);
    ESP32PWM::allocateTimer(3);
    
    servo.setPeriodHertz(50); // 50Hz para servos estándar
    
    if (servo.attach(servoPin, 500, 2400)) { // Pulsos de 500 a 2400 microsegundos
        isInitialized = true;
        
        // Mover a posición inicial (abierto)
        servo.write(currentPosition);
        delay(500); // Tiempo para estabilizar
        
        Serial.printf("[ServoActuator] Servo inicializado en pin %d\n", servoPin);
        Serial.printf("[ServoActuator] Posición inicial: %d grados\n", currentPosition);
        Serial.printf("[ServoActuator] Posición abierta: %d grados\n", openPosition);
        Serial.printf("[ServoActuator] Posición cerrada: %d grados\n", closedPosition);
        
        return true;
    }
    
    Serial.printf("[ServoActuator] ERROR: No se pudo inicializar servo en pin %d\n", servoPin);
    return false;
}

bool ServoActuator::moveToPosition(int position) {
    if (!isInitialized) {
        Serial.println("[ServoActuator] ERROR: Servo no inicializado");
        return false;
    }
    
    // Verificar límites
    if (position < minPosition || position > maxPosition) {
        Serial.printf("[ServoActuator] ERROR: Posición %d fuera de límites (%d-%d)\n", 
                     position, minPosition, maxPosition);
        return false;
    }
    
    if (position != currentPosition) {
        servo.write(position);
        currentPosition = position;
        targetPosition = position;
        lastMoveTime = millis();
        moveCount++;
        
        Serial.printf("[ServoActuator] Movido a posición: %d grados\n", position);
    }
    
    return true;
}

bool ServoActuator::openVent() {
    Serial.println("[ServoActuator] Abriendo ventilación (destapando ventilador)");
    return moveToPosition(openPosition);
}

bool ServoActuator::closeVent() {
    Serial.println("[ServoActuator] Cerrando ventilación (tapando ventilador)");
    return moveToPosition(closedPosition);
}

// cppcheck-suppress unusedFunction
bool ServoActuator::moveToAngle(int angle) {
    return moveToPosition(angle);
}

// cppcheck-suppress unusedFunction
void ServoActuator::setOpenPosition(int position) {
    if (position >= minPosition && position <= maxPosition) {
        openPosition = position;
        Serial.printf("[ServoActuator] Posición abierta configurada: %d grados\n", position);
    }
}

// cppcheck-suppress unusedFunction
void ServoActuator::setClosedPosition(int position) {
    if (position >= minPosition && position <= maxPosition) {
        closedPosition = position;
        Serial.printf("[ServoActuator] Posición cerrada configurada: %d grados\n", position);
    }
}

// cppcheck-suppress unusedFunction
void ServoActuator::setPositionLimits(int minPos, int maxPos) {
    if (minPos >= 0 && maxPos <= 180 && minPos < maxPos) {
        minPosition = minPos;
        maxPosition = maxPos;
        Serial.printf("[ServoActuator] Límites configurados: %d - %d grados\n", minPos, maxPos);
    }
}

// cppcheck-suppress unusedFunction
void ServoActuator::setMoveSpeed(int speed) {
    if (speed > 0 && speed <= 10) {
        moveSpeed = speed;
        Serial.printf("[ServoActuator] Velocidad configurada: %d grados/paso\n", speed);
    }
}

// cppcheck-suppress unusedFunction
void ServoActuator::setMoveInterval(unsigned long interval) {
    if (interval >= 10) {
        moveInterval = interval;
        Serial.printf("[ServoActuator] Intervalo de movimiento configurado: %lu ms\n", interval);
    }
}

int ServoActuator::getCurrentPosition() {
    return currentPosition;
}
// cppcheck-suppress unusedFunction
int ServoActuator::getTargetPosition() {
    return targetPosition;
}

bool ServoActuator::isAtPosition(int position) {
    return abs(currentPosition - position) <= 2; // Tolerancia de 2 grados
}

bool ServoActuator::isOpen() {
    return isAtPosition(openPosition);
}

bool ServoActuator::isClosed() {
    return isAtPosition(closedPosition);
}

// cppcheck-suppress unusedFunction
bool ServoActuator::isReady() {
    return isInitialized;
}

// cppcheck-suppress unusedFunction
bool ServoActuator::isInMotion() {
    return isMoving;
}

// cppcheck-suppress unusedFunction
void ServoActuator::update() {
    // Esta función permite movimiento suave en el loop principal
    if (!isInitialized || !isMoving) {
        return;
    }
    
    unsigned long currentTime = millis();
    
    if (currentTime - lastMoveTime >= moveInterval) {
        if (currentPosition != targetPosition) {
            int direction = (targetPosition > currentPosition) ? 1 : -1;
            int step = min(moveSpeed, abs(targetPosition - currentPosition));
            
            currentPosition += direction * step;
            servo.write(currentPosition);
            lastMoveTime = currentTime;
            
            if (currentPosition == targetPosition) {
                isMoving = false;
                Serial.printf("[ServoActuator] Movimiento suave completado a: %d grados\n", currentPosition);
            }
        } else {
            isMoving = false;
        }
    }
}

// cppcheck-suppress unusedFunction
bool ServoActuator::smoothMoveTo(int position) {
    if (!isInitialized) {
        return false;
    }
    
    // Verificar límites
    if (position < minPosition || position > maxPosition) {
        return false;
    }
    
    targetPosition = position;
    isMoving = true;
    moveCount++;
    
    Serial.printf("[ServoActuator] Iniciando movimiento suave a: %d grados\n", position);
    
    return true;
}

// cppcheck-suppress unusedFunction
bool ServoActuator::calibrate() {
    if (!isInitialized) {
        return false;
    }
    
    Serial.println("[ServoActuator] Iniciando calibración...");
    
    // Mover a posición mínima
    moveToPosition(minPosition);
    delay(1000);
    
    // Mover a posición máxima
    moveToPosition(maxPosition);
    delay(1000);
    
    // Mover a posición central
    moveToPosition((minPosition + maxPosition) / 2);
    delay(1000);
    
    Serial.println("[ServoActuator] Calibración completada");
    
    return true;
}

// cppcheck-suppress unusedFunction
bool ServoActuator::testMovement() {
    if (!isInitialized) {
        return false;
    }
    
    Serial.println("[ServoActuator] Iniciando test de movimiento...");
    
    // Test abrir y cerrar
    openVent();
    delay(2000);
    
    closeVent();
    delay(2000);
    
    openVent();
    delay(1000);
    
    Serial.println("[ServoActuator] Test de movimiento completado");
    
    return true;
}

// cppcheck-suppress unusedFunction
unsigned int ServoActuator::getMoveCount() {
    return moveCount;
}

// cppcheck-suppress unusedFunction
void ServoActuator::printStatus() {
    Serial.println("========== Estado del Servo ==========");
    Serial.printf("Inicializado: %s\n", isInitialized ? "Sí" : "No");
    Serial.printf("Pin: %d\n", servoPin);
    Serial.printf("Posición actual: %d grados\n", currentPosition);
    Serial.printf("Posición objetivo: %d grados\n", targetPosition);
    Serial.printf("En movimiento: %s\n", isMoving ? "Sí" : "No");
    Serial.printf("Estado ventilación: %s\n", isOpen() ? "ABIERTA" : (isClosed() ? "CERRADA" : "INTERMEDIA"));
    Serial.printf("Posición abierta: %d grados\n", openPosition);
    Serial.printf("Posición cerrada: %d grados\n", closedPosition);
    Serial.printf("Límites: %d - %d grados\n", minPosition, maxPosition);
    Serial.printf("Número de movimientos: %d\n", moveCount);
    Serial.println("======================================");
}

// cppcheck-suppress unusedFunction
int ServoActuator::getBlynkState() {
    // Devolver 1 si está abierto, 0 si está cerrado
    return isOpen() ? 1 : 0;
}

// cppcheck-suppress unusedFunction
bool ServoActuator::setFromBlynk(int state) {
    Serial.printf("[ServoActuator] Comando desde Blynk: %d\n", state);
    
    if (state == 1) {
        return openVent();
    } else {
        return closeVent();
    }
}

// cppcheck-suppress unusedFunction
bool ServoActuator::setPositionFromBlynk(int position) {
    // Blynk puede enviar valores 0-100, convertir a rango del servo
    int servoPosition = map(position, 0, 100, minPosition, maxPosition);
    
    Serial.printf("[ServoActuator] Posición desde Blynk: %d -> %d grados\n", position, servoPosition);
    
    return moveToPosition(servoPosition);
}
