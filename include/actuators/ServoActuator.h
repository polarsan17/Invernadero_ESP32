#pragma once

#include <Arduino.h>
#include <ESP32Servo.h>

class ServoActuator {
private:
    Servo servo;
    uint8_t servoPin;
    
    // Estado del servo
    int currentPosition;
    int targetPosition;
    bool isInitialized;
    bool isMoving;
    
    // Configuración de posiciones
    int openPosition;    // Posición abierta (ventilador libre)
    int closedPosition;  // Posición cerrada (ventilador tapado)
    
    // Control de movimiento
    unsigned long lastMoveTime;
    unsigned long moveInterval;
    int moveSpeed;  // grados por paso
    
    // Límites de seguridad
    int minPosition;
    int maxPosition;
    
    // Estadísticas
    unsigned int moveCount;
    unsigned long totalMoves;
    
public:
    explicit ServoActuator(uint8_t pin);
    ~ServoActuator();
    
    // Inicialización
    bool begin();
    
    // Control del servo
    bool moveToPosition(int position);
    bool openVent();    // Abrir ventilación (destapar ventilador)
    bool closeVent();   // Cerrar ventilación (tapar ventilador)
    bool moveToAngle(int angle);
    
    // Configuración
    void setOpenPosition(int position);
    void setClosedPosition(int position);
    void setPositionLimits(int minPos, int maxPos);
    void setMoveSpeed(int speed);
    void setMoveInterval(unsigned long interval);
    
    // Estado
    int getCurrentPosition();
    int getTargetPosition();
    bool isAtPosition(int position);
    bool isOpen();
    bool isClosed();
    bool isReady();
    bool isInMotion();
    
    // Movimiento suave
    void update(); // Llamar en loop para movimiento suave
    bool smoothMoveTo(int position);
    
    // Calibración
    bool calibrate();
    bool testMovement();
    
    // Estadísticas
    unsigned int getMoveCount();
    void resetStatistics();
    
    // Funciones de utilidad
    void printStatus();
    
    // Integración con Blynk
    int getBlynkState();
    bool setFromBlynk(int state);
    bool setPositionFromBlynk(int position);
};
