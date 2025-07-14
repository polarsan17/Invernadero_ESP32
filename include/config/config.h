#ifndef CONFIG_H
#define CONFIG_H

// ===========================================
// CONFIGURACIÓN DE BLYNK
// ===========================================

// Configuración de Blynk Template
#define BLYNK_TEMPLATE_ID "TMPL51jNapVeq"
#define BLYNK_TEMPLATE_NAME "InvernaderoESP32"

// Timeouts de conexión
#define WIFI_CONNECTION_TIMEOUT 20000    // 20 segundos
#define WIFI_RETRY_INTERVAL 5000         // 5 segundos
#define BLYNK_RECONNECT_INTERVAL 10000   // 10 segundos

// Configuración del monitor serie
#define SERIAL_BAUDRATE 115200

// ===========================================
// CONFIGURACIÓN DEL SISTEMA DE LOGGING
// ===========================================

// Nivel de logging por defecto
#define LOG_LEVEL_DEFAULT 3              // INFO level (1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=VERBOSE)

// Configuración de archivos de log
#define LOG_ENABLE_FILE_OUTPUT true      // Habilitar logging a archivo SPIFFS
#define LOG_ENABLE_SERIAL_OUTPUT true    // Habilitar logging por Serial
#define LOG_MAX_FILE_SIZE 512000         // Tamaño máximo archivo log (512KB)
#define LOG_MAX_FILES 3                  // Número máximo de archivos de log

// Configuración de componentes específicos
#define LOG_SENSOR_LEVEL 3               // Nivel de log para sensores
#define LOG_ACTUATOR_LEVEL 3             // Nivel de log para actuadores
#define LOG_NETWORK_LEVEL 2              // Nivel de log para red (solo WARN y ERROR)
#define LOG_SYSTEM_LEVEL 3               // Nivel de log para sistema

// ===========================================
// CONFIGURACIÓN DE SENSORES
// ===========================================

// DHT22 Sensor
#define DHT22_PIN 4                      // Pin GPIO del sensor DHT22
#define DHT22_READ_INTERVAL 5000         // Intervalo de lectura en ms (5 segundos)

// AS7341 Sensor Espectral
#define AS7341_READ_INTERVAL 3000        // Intervalo de lectura en ms (3 segundos)
#define AS7341_SDA_PIN 21                // Pin SDA para I2C (GPIO 21)
#define AS7341_SCL_PIN 22                // Pin SCL para I2C (GPIO 22)

// Sensor de Humedad del Suelo
#define SOIL_MOISTURE_PIN 35             // Pin GPIO analógico del sensor (GPIO 35)
#define SOIL_MOISTURE_READ_INTERVAL 8000 // Intervalo de lectura en ms (8 segundos)

// Sensor BH1750 de Luz
#define BH1750_READ_INTERVAL 4000        // Intervalo de lectura en ms (4 segundos)
#define BH1750_SDA_PIN 21                // Pin SDA para I2C (GPIO 21)
#define BH1750_SCL_PIN 22                // Pin SCL para I2C (GPIO 22)

// Sensor HC-SR04 Ultrasónico para Nivel de Agua
#define HCSR04_TRIGGER_PIN 5             // Pin GPIO Trigger del HC-SR04 (GPIO 5)
#define HCSR04_ECHO_PIN 18               // Pin GPIO Echo del HC-SR04 (GPIO 18)
#define HCSR04_READ_INTERVAL 5000        // Intervalo de lectura en ms (5 segundos)

// Sensor RS485 de Suelo (NPK-EC-PH-Temperatura-Humedad)
#define RS485_TX_PIN 17                  // Pin GPIO TX para RS485 (GPIO 17)
#define RS485_RX_PIN 16                  // Pin GPIO RX para RS485 (GPIO 16)
#define RS485_TX_ENABLE_PIN 4            // Pin GPIO TX Enable para RS485 (GPIO 4)
#define RS485_READ_INTERVAL 10000        // Intervalo de lectura en ms (10 segundos)

// Intervalos de actualización
#define BLYNK_UPDATE_INTERVAL 10000      // Envío a Blynk cada 10 segundos

// Pines virtuales de Blynk para sensores
#define BLYNK_VPIN_TEMPERATURE 0         // Pin virtual para temperatura (V0)
#define BLYNK_VPIN_HUMIDITY 1            // Pin virtual para humedad (V1)
#define BLYNK_VPIN_HEAT_INDEX 2          // Pin virtual para índice de calor (V2)

// Pines virtuales para AS7341 (sensor espectral)
#define BLYNK_VPIN_LUX 3                 // Pin virtual para lux (V3)
#define BLYNK_VPIN_COLOR_TEMP 4          // Pin virtual para temperatura de color (V4)
#define BLYNK_VPIN_RED_LIGHT 5           // Pin virtual para luz roja (V5)
#define BLYNK_VPIN_GREEN_LIGHT 6         // Pin virtual para luz verde (V6)
#define BLYNK_VPIN_BLUE_LIGHT 7          // Pin virtual para luz azul (V7)
#define BLYNK_VPIN_CLEAR_LIGHT 8         // Pin virtual para luz clear (V8)
#define BLYNK_VPIN_NIR_LIGHT 9           // Pin virtual para luz NIR (V9)

// Pines virtuales para sensor de humedad del suelo
#define BLYNK_VPIN_SOIL_MOISTURE 10      // Pin virtual para humedad del suelo (V10)
#define BLYNK_VPIN_SOIL_RAW 11           // Pin virtual para valor raw del suelo (V11)

// Pines virtuales para sensor BH1750
#define BLYNK_VPIN_LIGHT_LUX 12          // Pin virtual para luminosidad en lux (V12)
#define BLYNK_VPIN_LIGHT_LEVEL 13        // Pin virtual para nivel de luz (V13)

// Pines virtuales para sensor HC-SR04 (nivel de agua)
#define BLYNK_VPIN_WATER_LEVEL 14        // Pin virtual para nivel de agua en cm (V14)
#define BLYNK_VPIN_WATER_PERCENTAGE 15   // Pin virtual para porcentaje de agua (V15)
#define BLYNK_VPIN_WATER_STATUS 16       // Pin virtual para estado del agua (V16)

// Pines virtuales para sensor RS485 (suelo NPK-EC-PH-Temp-Hum)
#define BLYNK_VPIN_SOIL_TEMP 17          // Pin virtual para temperatura del suelo (V17)
#define BLYNK_VPIN_SOIL_MOISTURE_RS485 18 // Pin virtual para humedad del suelo RS485 (V18)
#define BLYNK_VPIN_SOIL_EC 19            // Pin virtual para conductividad eléctrica (V19)
#define BLYNK_VPIN_SOIL_PH 20            // Pin virtual para pH del suelo (V20)
#define BLYNK_VPIN_SOIL_NPK_N 21         // Pin virtual para nitrógeno (V21)
#define BLYNK_VPIN_SOIL_NPK_P 22         // Pin virtual para fósforo (V22)
#define BLYNK_VPIN_SOIL_NPK_K 23         // Pin virtual para potasio (V23)

// ===========================================
// CONFIGURACIÓN DE ACTUADORES
// ===========================================

// Relé Multicanal para Actuadores
#define RELAY_CHANNELS 8                 // Número de canales del módulo relé (ajustable 1-16)
#define RELAY_TRIGGER_TYPE 0             // 0 = ACTIVE_LOW, 1 = ACTIVE_HIGH
#define RELAY_MAX_SIMULTANEOUS 6         // Máximo de relés activos simultáneamente (protección)

// Pines GPIO para relés (ajustar según necesidades)
#define RELAY_PIN_1 25                   // Canal 1 - Bomba de agua principal
#define RELAY_PIN_2 26                   // Canal 2 - Ventilador de extracción
#define RELAY_PIN_3 27                   // Canal 3 - Ventilador de circulación  
#define RELAY_PIN_4 14                   // Canal 4 - Iluminación LED
#define RELAY_PIN_5 12                   // Canal 5 - Bomba de nutrientes
#define RELAY_PIN_6 13                   // Canal 6 - Válvula de riego zona 1
#define RELAY_PIN_7 32                   // Canal 7 - Válvula de riego zona 2
#define RELAY_PIN_8 33                   // Canal 8 - Sistema de calefacción

// Pines virtuales Blynk para control de relés
#define BLYNK_VPIN_RELAY_1 24            // Pin virtual para relé 1 (V24)
#define BLYNK_VPIN_RELAY_2 25            // Pin virtual para relé 2 (V25)
#define BLYNK_VPIN_RELAY_3 26            // Pin virtual para relé 3 (V26)
#define BLYNK_VPIN_RELAY_4 27            // Pin virtual para relé 4 (V27)
#define BLYNK_VPIN_RELAY_5 28            // Pin virtual para relé 5 (V28)
#define BLYNK_VPIN_RELAY_6 29            // Pin virtual para relé 6 (V29)
#define BLYNK_VPIN_RELAY_7 30            // Pin virtual para relé 7 (V30)
#define BLYNK_VPIN_RELAY_8 31            // Pin virtual para relé 8 (V31)

// Pines virtuales Blynk para configuración de targets/objetivos
#define BLYNK_VPIN_TARGET_TEMPERATURE 40     // Pin virtual para temperatura objetivo (V40)
#define BLYNK_VPIN_TARGET_HUMIDITY 41        // Pin virtual para humedad objetivo (V41)
#define BLYNK_VPIN_TARGET_SOIL_MOISTURE 42   // Pin virtual para humedad suelo objetivo (V42)
#define BLYNK_VPIN_TARGET_LUX_MIN 43         // Pin virtual para lux mínimo (V43)
#define BLYNK_VPIN_TARGET_VENT_TEMP 44       // Pin virtual para temperatura ventilación (V44)
#define BLYNK_VPIN_TARGET_SOIL_PH 45         // Pin virtual para pH objetivo (V45)
#define BLYNK_VPIN_TARGET_SOIL_EC 46         // Pin virtual para EC objetivo (V46)
#define BLYNK_VPIN_TARGET_WATER_LEVEL 47     // Pin virtual para nivel agua crítico (V47)

// Configuración de tiempos para actuadores
#define RELAY_UPDATE_INTERVAL 1000       // Intervalo de actualización de timers en ms
#define DEFAULT_PUMP_DURATION 300000     // Duración por defecto bomba (5 minutos)
#define DEFAULT_FAN_DURATION 600000      // Duración por defecto ventilador (10 minutos)
#define DEFAULT_IRRIGATION_DURATION 180000 // Duración por defecto riego (3 minutos)

// ===========================================
// CONFIGURACIÓN DE VENTILADORES
// ===========================================

// Ventilador Principal (Extracción)
#define FAN_MAIN_PIN 26                  // Pin del ventilador principal (relé o PWM)
#define FAN_MAIN_CONTROL_TYPE 0          // 0 = RELAY_CONTROL, 1 = PWM_CONTROL
#define FAN_MAIN_PWM_FREQUENCY 1000      // Frecuencia PWM en Hz (solo si PWM_CONTROL)
#define FAN_MAIN_PWM_CHANNEL 0           // Canal PWM ESP32 (0-15)

// Ventilador Secundario (Circulación)
#define FAN_CIRCULATION_PIN 27           // Pin del ventilador de circulación
#define FAN_CIRCULATION_CONTROL_TYPE 1   // Control PWM para velocidad variable
#define FAN_CIRCULATION_PWM_FREQUENCY 1000
#define FAN_CIRCULATION_PWM_CHANNEL 1

// Control automático por temperatura
#define FAN_AUTO_TEMP_LOW 22.0          // Temperatura baja (°C) - ventilador OFF
#define FAN_AUTO_TEMP_HIGH 28.0         // Temperatura alta (°C) - ventilador MAX
#define FAN_AUTO_SPEED_MIN 30           // Velocidad mínima en modo auto (%)
#define FAN_AUTO_SPEED_MAX 100          // Velocidad máxima en modo auto (%)

// Configuración de seguridad
#define FAN_MAX_RUN_TIME 3600000        // Tiempo máximo funcionamiento (1 hora)
#define FAN_MIN_RUN_TIME 5000           // Tiempo mínimo funcionamiento (5 segundos)
#define FAN_OVERHEAT_TEMP 80.0          // Temperatura de protección (°C)
#define FAN_SOFT_START_TIME 3000        // Tiempo de arranque suave (ms)

// Pines virtuales Blynk para ventiladores
#define BLYNK_VPIN_FAN_MAIN_STATE 32    // Pin virtual estado ventilador principal (V32)
#define BLYNK_VPIN_FAN_MAIN_SPEED 33    // Pin virtual velocidad ventilador principal (V33)
#define BLYNK_VPIN_FAN_CIRCULATION_STATE 34 // Pin virtual estado ventilador circulación (V34)
#define BLYNK_VPIN_FAN_CIRCULATION_SPEED 35 // Pin virtual velocidad ventilador circulación (V35)
#define BLYNK_VPIN_FAN_AUTO_MODE 36     // Pin virtual modo automático (V36)

// ===========================================
// CONFIGURACIÓN DE CALEFACTORES
// ===========================================

// Calefactor Principal
#define HEATER_MAIN_PIN 32              // Pin del calefactor principal (relé o PWM)
#define HEATER_MAIN_CONTROL_TYPE 0      // 0 = RELAY_CONTROL, 1 = PWM_CONTROL
#define HEATER_MAIN_PWM_FREQUENCY 1000  // Frecuencia PWM en Hz (solo si PWM_CONTROL)
#define HEATER_MAIN_PWM_CHANNEL 2       // Canal PWM ESP32 (0-15)

// Calefactor Auxiliar (opcional)
#define HEATER_AUX_PIN 33               // Pin del calefactor auxiliar
#define HEATER_AUX_CONTROL_TYPE 1       // Control PWM para potencia variable
#define HEATER_AUX_PWM_FREQUENCY 1000
#define HEATER_AUX_PWM_CHANNEL 3

// Control automático por temperatura
#define HEATER_TARGET_TEMP 25.0         // Temperatura objetivo por defecto (°C)
#define HEATER_AUTO_TEMP_MIN 20.0       // Temperatura mínima (°C) - calefactor ON
#define HEATER_AUTO_TEMP_MAX 30.0       // Temperatura máxima (°C) - calefactor OFF
#define HEATER_TEMP_HYSTERESIS 1.0      // Histéresis de temperatura (°C)
#define HEATER_AUTO_POWER_MIN 20        // Potencia mínima en modo auto (%)
#define HEATER_AUTO_POWER_MAX 100       // Potencia máxima en modo auto (%)

// Configuración de seguridad
#define HEATER_MAX_HEATING_TIME 1800000 // Tiempo máximo calentamiento (30 minutos)
#define HEATER_MIN_HEATING_TIME 10000   // Tiempo mínimo calentamiento (10 segundos)
#define HEATER_OVERHEAT_TEMP 60.0       // Temperatura de protección (°C)
#define HEATER_EMERGENCY_SHUTDOWN true  // Activar parada de emergencia

// Operación cíclica (para control por relé)
#define HEATER_CYCLE_ON_TIME 300000     // Tiempo encendido en ciclo (5 minutos)
#define HEATER_CYCLE_OFF_TIME 60000     // Tiempo apagado en ciclo (1 minuto)

// Control PID (para control preciso de temperatura)
#define HEATER_PID_KP 2.0               // Constante proporcional
#define HEATER_PID_KI 0.1               // Constante integral  
#define HEATER_PID_KD 0.05              // Constante derivativa

// Pines virtuales Blynk para calefactores
#define BLYNK_VPIN_HEATER_MAIN_STATE 37    // Pin virtual estado calefactor principal (V37)
#define BLYNK_VPIN_HEATER_MAIN_POWER 38    // Pin virtual potencia calefactor principal (V38)
#define BLYNK_VPIN_HEATER_TARGET_TEMP 39   // Pin virtual temperatura objetivo (V39)
#define BLYNK_VPIN_HEATER_AUX_STATE 40     // Pin virtual estado calefactor auxiliar (V40)
#define BLYNK_VPIN_HEATER_AUX_POWER 41     // Pin virtual potencia calefactor auxiliar (V41)
#define BLYNK_VPIN_HEATER_AUTO_MODE 42     // Pin virtual modo automático (V42)

// ===== CONFIGURACIÓN BOMBA DE AGUA =====

// Pin de control
#define WATER_PUMP_PIN 19                  // Pin del relé para bomba de agua

// Configuración temporal (en segundos)
#define WATER_PUMP_MAX_RUN_TIME 300        // Tiempo máximo funcionamiento continuo (5 min)
#define WATER_PUMP_MIN_INTERVAL 60         // Intervalo mínimo entre activaciones (1 min)
#define WATER_PUMP_AUTO_INTERVAL 3600      // Intervalo riego automático (1 hora)
#define WATER_PUMP_AUTO_DURATION 60        // Duración riego automático (1 min)

// Configuración de seguridad
#define WATER_PUMP_SAFETY_TIMEOUT true     // Habilitar timeout de seguridad

// Pines virtuales Blynk para bomba de agua
#define BLYNK_VPIN_WATER_PUMP_STATE 43     // Pin virtual estado bomba (V43)
#define BLYNK_VPIN_WATER_PUMP_MODE 44      // Pin virtual modo automático (V44)
#define BLYNK_VPIN_WATER_PUMP_TIMER 45     // Pin virtual programador (V45)
#define BLYNK_VPIN_WATER_PUMP_STATS 46     // Pin virtual estadísticas (V46)

#endif
