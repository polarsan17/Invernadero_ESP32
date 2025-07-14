#ifndef RELAYCONTROLLER_H
#define RELAYCONTROLLER_H

#include <Arduino.h>

/**
 * @brief Controlador de relés multi-canal para invernadero ESP32
 * 
 * Esta clase maneja un sistema de relés de hasta 8 canales para controlar
 * diferentes actuadores en el invernadero como bombas, ventiladores, 
 * calentadores, luces, etc.
 * 
 * Características:
 * - Soporte para relés de lógica positiva (HIGH = activado) y negativa (LOW = activado)
 * - Control individual de cada canal
 * - Estado de seguridad (todos los relés apagados al iniciar)
 * - Métodos para control grupal e individual
 * - Integración con Blynk IoT
 */
class RelayController {
private:
    static const uint8_t MAX_RELAYS = 8;
    
    struct RelayChannel {
        uint8_t pin;
        bool isActive;
        bool isInverted;  // true para lógica negativa (LOW = ON)
        String name;
        unsigned long lastToggleTime;
    };
    
    RelayChannel relays[MAX_RELAYS];
    uint8_t activeChannels;
    bool isInitialized;
    unsigned long debounceDelay;
    
    void writeRelayState(uint8_t channel, bool state);
    bool isValidChannel(uint8_t channel) const;
    
public:
    /**
     * @brief Constructor del controlador de relés
     */
    RelayController();
    
    /**
     * @brief Destructor
     */
    ~RelayController();
    
    /**
     * @brief Inicializa el controlador de relés
     * @param debounceMs Tiempo de debounce en milisegundos (por defecto 50ms)
     * @return true si la inicialización fue exitosa
     */
    bool begin(unsigned long debounceMs = 50);
    
    /**
     * @brief Configura un canal de relé
     * @param channel Número del canal (0-7)
     * @param pin Pin GPIO del ESP32
     * @param name Nombre descriptivo del relé
     * @param inverted true para lógica negativa (LOW = ON)
     * @return true si la configuración fue exitosa
     */
    bool configureRelay(uint8_t channel, uint8_t pin, const String& name, bool inverted = false);
    
    /**
     * @brief Activa un relé específico
     * @param channel Número del canal (0-7)
     * @return true si el relé fue activado exitosamente
     */
    bool activateRelay(uint8_t channel);
    
    /**
     * @brief Desactiva un relé específico
     * @param channel Número del canal (0-7)
     * @return true si el relé fue desactivado exitosamente
     */
    bool deactivateRelay(uint8_t channel);
    
    /**
     * @brief Alterna el estado de un relé
     * @param channel Número del canal (0-7)
     * @return true si el estado fue cambiado exitosamente
     */
    bool toggleRelay(uint8_t channel);
    
    /**
     * @brief Obtiene el estado actual de un relé
     * @param channel Número del canal (0-7)
     * @return true si el relé está activo, false si está inactivo o el canal es inválido
     */
    bool getRelayState(uint8_t channel) const;
    
    /**
     * @brief Obtiene el nombre de un relé
     * @param channel Número del canal (0-7)
     * @return String con el nombre del relé
     */
    String getRelayName(uint8_t channel) const;
    
    /**
     * @brief Obtiene el pin GPIO de un relé
     * @param channel Número del canal (0-7)
     * @return Pin GPIO o 255 si el canal es inválido
     */
    uint8_t getRelayPin(uint8_t channel) const;
    
    /**
     * @brief Desactiva todos los relés (modo de seguridad)
     */
    void deactivateAllRelays();
    
    /**
     * @brief Activa múltiples relés usando una máscara de bits
     * @param mask Máscara de bits donde cada bit representa un canal (bit 0 = canal 0)
     */
    void setRelayMask(uint8_t mask);
    
    /**
     * @brief Obtiene la máscara de bits actual de todos los relés
     * @return Máscara de bits con el estado actual
     */
    uint8_t getRelayMask() const;
    
    /**
     * @brief Obtiene el número de canales configurados
     * @return Número de canales activos
     */
    uint8_t getActiveChannelCount() const;
    
    /**
     * @brief Verifica si el controlador está inicializado
     * @return true si está inicializado
     */
    bool isReady() const;
    
    /**
     * @brief Actualiza el estado del controlador (llamar en loop principal)
     * Maneja el debounce y otras tareas de mantenimiento
     */
    void update();
    
    /**
     * @brief Imprime el estado actual de todos los relés
     */
    void printStatus() const;
    
    /**
     * @brief Obtiene información de estado para envío a Blynk
     * @param channel Número del canal (0-7)
     * @return Estado del relé para Blynk (1 = activo, 0 = inactivo)
     */
    int getBlynkState(uint8_t channel) const;
    
    /**
     * @brief Controla un relé desde Blynk
     * @param channel Número del canal (0-7)
     * @param state Estado desde Blynk (1 = activar, 0 = desactivar)
     * @return true si el comando fue ejecutado exitosamente
     */
    bool setFromBlynk(uint8_t channel, int state);
};

#endif // RELAYCONTROLLER_H
