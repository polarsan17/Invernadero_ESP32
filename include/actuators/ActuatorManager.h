#ifndef ACTUATORMANAGER_H
#define ACTUATORMANAGER_H

#include <Arduino.h>
#include "actuators/FanActuator.h"
#include "actuators/WaterPumpActuator.h"
#include "actuators/HeaterActuator.h"
#include "actuators/LEDStripActuator.h"
#include "actuators/ServoActuator.h"
#include "blynk/BlynkManager.h"

/**
 * @brief Gestor centralizado de actuadores para invernadero ESP32
 * 
 * Esta clase maneja todos los actuadores del invernadero de manera centralizada,
 * siguiendo el mismo patrón de programación que el SensorManager.
 * 
 * Actuadores incluidos:
 * - Ventilador para ventilación
 * - Bomba de agua para riego
 * - Calefactor para control de temperatura
 * - Tira LED para iluminación de crecimiento
 * - Servomotor para tapar/destapar ventilador
 */
class ActuatorManager {
private:
    FanActuator* fanActuator;
    WaterPumpActuator* waterPumpActuator;
    HeaterActuator* heaterActuator;
    LEDStripActuator* ledStripActuator;
    ServoActuator* servoActuator;
    BlynkManager* blynkManager;
    
    // Control de envío de datos
    unsigned long lastBlynkUpdate;
    unsigned long blynkUpdateInterval;
    
    bool actuatorsInitialized;
    
public:
    explicit ActuatorManager(BlynkManager& blynk);
    ~ActuatorManager();
    
    // Inicialización
    bool begin();
    
    // Actualización periódica
    void update();
    
    // Gestión de actuadores
    void sendDataToBlynk();
    
    // Estado
    bool areActuatorsReady();
    
    // Getters para acceder a los actuadores individuales
    FanActuator* getFan();
    WaterPumpActuator* getWaterPump();
    HeaterActuator* getHeater();
    LEDStripActuator* getLEDStrip();
    ServoActuator* getServo();
    
    // Métodos de control directo (para compatibilidad)
    bool activarVentilador();
    bool desactivarVentilador();
    bool obtenerEstadoVentilador() const;
    
    bool activarBombaAgua();
    bool desactivarBombaAgua();
    bool obtenerEstadoBombaAgua() const;
    
    bool activarCalefactor();
    bool desactivarCalefactor();
    bool obtenerEstadoCalefactor() const;
    
    bool activarTiraLED();
    bool desactivarTiraLED();
    bool obtenerEstadoTiraLED() const;
    
    bool abrirVentilacion();
    bool cerrarVentilacion();
    bool obtenerEstadoVentilacion() const;
    
    // Control de emergencia
    void desactivarTodos();
    void modoSeguridad();
    
    // Funciones de utilidad
    void imprimirEstado() const;
    
    // Integración con Blynk
    bool controlarDesdeBlynk(uint8_t actuador, int estado);
};

#endif // ACTUATORMANAGER_H
