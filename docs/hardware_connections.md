# Guía de Conexiones Hardware - Sistema de Invernadero ESP32

## Descripción General

Esta guía proporciona las instrucciones detalladas para realizar las conexiones físicas entre el ESP32 y todos los sensores y actuadores del sistema de invernadero. Se incluyen diagramas de conexión, especificaciones técnicas y recomendaciones de instalación para garantizar un funcionamiento óptimo y seguro del sistema.

## Especificaciones Técnicas Generales

### Microcontrolador Principal
- **Modelo**: ESP32 DevKit V1
- **Voltaje de operación**: 3.3V (tolerancia 5V en algunos pines)
- **Corriente máxima por pin**: 40mA
- **Pines GPIO disponibles**: 34 pines digitales
- **Resolución ADC**: 12 bits (0-4095)
- **Frecuencia de reloj**: 240MHz

### Requerimientos de Alimentación
- **Voltaje de entrada**: 5V DC (vía USB) o 7-12V DC (vía Vin)
- **Consumo estimado**: 250mA en operación normal
- **Consumo máximo**: 500mA con todos los periféricos activos
- **Fuente recomendada**: 5V/2A mínimo

## Conexiones de Sensores Ambientales

### Sensor DHT22 - Temperatura y Humedad

#### Especificaciones Técnicas
- **Voltaje**: 3.3V - 5V
- **Corriente**: 1-1.5mA
- **Rango temperatura**: -40°C a +80°C
- **Rango humedad**: 0% a 100% RH
- **Precisión**: ±0.5°C, ±2% RH

#### Conexiones
```
DHT22          ESP32
------         -----
VCC     <-->   3.3V
GND     <-->   GND
DATA    <-->   GPIO 4
```

#### Consideraciones de Instalación
- **Resistencia pull-up**: 4.7kΩ entre DATA y VCC
- **Cable máximo**: 20 metros con cable blindado
- **Ubicación**: Protegido de luz solar directa
- **Altura**: 1.5m del suelo para medición representativa

### Sensor BH1750 - Luminosidad Ambiente

#### Especificaciones Técnicas
- **Voltaje**: 2.4V - 3.6V
- **Corriente**: 120µA (activo), 1µA (power down)
- **Rango**: 1 - 65535 lux
- **Resolución**: 0.5 lux
- **Interfaz**: I2C

#### Conexiones
```
BH1750         ESP32
------         -----
VCC     <-->   3.3V
GND     <-->   GND
SCL     <-->   GPIO 22
SDA     <-->   GPIO 21
ADDR    <-->   GND (dirección 0x23)
```

#### Configuración I2C
- **Dirección**: 0x23 (ADDR a GND) o 0x5C (ADDR a VCC)
- **Velocidad**: 100kHz (estándar) o 400kHz (rápido)
- **Resistencias pull-up**: 4.7kΩ en SDA y SCL (usualmente internas)

### Sensor AS7341 - Análisis Espectral

#### Especificaciones Técnicas
- **Voltaje**: 2.5V - 3.6V
- **Corriente**: 65µA (normal), 0.6µA (standby)
- **Canales**: 8 canales espectrales + Clear + NIR
- **Interfaz**: I2C
- **Tiempo de integración**: 2.78ms - 711ms

#### Conexiones
```
AS7341         ESP32
------         -----
VCC     <-->   3.3V
GND     <-->   GND
SCL     <-->   GPIO 22
SDA     <-->   GPIO 21
INT     <-->   GPIO 34 (opcional)
LED     <-->   GPIO 25 (LED blanco integrado)
```

#### Configuración Avanzada
- **Dirección I2C**: 0x39 (fija)
- **Interrupt pin**: GPIO 34 para detección de eventos
- **LED control**: GPIO 25 para iluminación de referencia

## Conexiones de Sensores del Suelo

### Sensor Capacitivo de Humedad del Suelo

#### Especificaciones Técnicas
- **Voltaje**: 3.3V - 5V
- **Corriente**: 5mA
- **Salida**: Análoga 0-3.3V
- **Rango**: 0% - 100% humedad volumétrica
- **Resistencia**: IP65 (resistente al agua)

#### Conexiones
```
Sensor Humedad    ESP32
--------------    -----
VCC        <-->   3.3V
GND        <-->   GND
AOUT       <-->   GPIO 36 (ADC1_CH0)
```

#### Calibración y Instalación
- **Profundidad**: 10-15cm en el sustrato
- **Orientación**: Vertical, evitar burbujas de aire
- **Calibración en aire**: Valor máximo ≈ 4095
- **Calibración en agua**: Valor mínimo ≈ 1500

### Sensor HC-SR04 - Nivel de Agua

#### Especificaciones Técnicas
- **Voltaje**: 5V DC
- **Corriente**: 15mA
- **Rango**: 2cm - 400cm
- **Resolución**: 0.3cm
- **Ángulo**: 15° (cono de detección)

#### Conexiones
```
HC-SR04        ESP32
-------        -----
VCC     <-->   5V (Vin)
GND     <-->   GND
TRIG    <-->   GPIO 5
ECHO    <-->   GPIO 18
```

#### Montaje y Configuración
- **Altura de montaje**: 5cm sobre nivel máximo de agua
- **Orientación**: Perpendicular a la superficie del agua
- **Protección**: Carcasa impermeable IP65
- **Distancia mínima**: 2cm para lecturas confiables

### Sensor RS485 - Análisis Completo del Suelo

#### Especificaciones Técnicas
- **Voltaje**: 5V - 12V DC
- **Corriente**: 50mA (lectura), 20mA (standby)
- **Parámetros**: Temperatura, Humedad, pH, EC, NPK
- **Interfaz**: RS485 (Modbus RTU)
- **Precisión**: ±0.5°C, ±3% RH, ±0.3 pH

#### Conexiones con Módulo RS485
```
Sensor RS485      Módulo RS485    ESP32
------------      ------------    -----
VCC (12V)  <-->   VCC      
GND        <-->   GND             GND
A+         <-->   A+       
B-         <-->   B-       
                  VCC      <-->   3.3V
                  GND      <-->   GND
                  RO       <-->   GPIO 16 (RX)
                  DI       <-->   GPIO 17 (TX)
                  RE/DE    <-->   GPIO 2
```

#### Configuración Modbus
- **Velocidad**: 9600 bps
- **Paridad**: None
- **Bits de datos**: 8
- **Bits de parada**: 1
- **Dirección del sensor**: 0x01 (configurable)

## Conexiones de Actuadores

### Módulo de Relés de 8 Canales

#### Especificaciones Técnicas
- **Voltaje de control**: 5V DC
- **Corriente por bobina**: 15-20mA
- **Capacidad de contactos**: 10A @ 250V AC, 10A @ 30V DC
- **Aislamiento**: Opto-acoplado
- **Tiempo de conmutación**: 10ms

#### Conexiones
```
Módulo Relés      ESP32
------------      -----
VCC        <-->   5V (Vin)
GND        <-->   GND
IN1        <-->   GPIO 13 (Bomba Principal)
IN2        <-->   GPIO 12 (Ventilador Extracción)
IN3        <-->   GPIO 14 (Ventilador Circulación)
IN4        <-->   GPIO 27 (Iluminación LED)
IN5        <-->   GPIO 26 (Bomba Nutrientes)
IN6        <-->   GPIO 25 (Válvula Zona 1)
IN7        <-->   GPIO 33 (Válvula Zona 2)
IN8        <-->   GPIO 32 (Calefacción)
```

#### Configuración de Seguridad
- **Lógica**: Activo bajo (LOW = activado)
- **Protección**: Fusibles en cada canal de potencia
- **Aislamiento**: Nunca conectar directamente a la red eléctrica
- **Contactores**: Usar contactores auxiliares para cargas >10A

### Bomba de Agua Principal

#### Especificaciones Técnicas
- **Voltaje**: 12V DC
- **Corriente**: 2A (máximo)
- **Caudal**: 1000L/h
- **Altura**: 3m máximo
- **Protección**: IP65

#### Conexiones
```
Bomba 12V         Relé 1          Fuente 12V
---------         ------          ----------
+ (Rojo)   <-->   COM/NO   <-->   + 12V
- (Negro)  <-->   Directo  <-->   - 12V (GND)
```

#### Protecciones
- **Fusible**: 3A en línea positiva
- **Diodo**: 1N4007 en paralelo con la bomba (protección bobina)
- **Filtro**: Capacitor 100µF en paralelo para reducir ruido

### Ventiladores de Circulación

#### Especificaciones Técnicas
- **Voltaje**: 12V DC
- **Corriente**: 0.5A cada uno
- **Velocidad**: 2000 RPM
- **Flujo**: 50 CFM
- **Rodamientos**: Blindados, larga duración

#### Conexiones
```
Ventilador 12V    Relé 2/3        Fuente 12V
--------------    --------        ----------
+ (Rojo)   <-->   COM/NO   <-->   + 12V
- (Negro)  <-->   Directo  <-->   - 12V (GND)
```

### Sistema de Iluminación LED

#### Especificaciones Técnicas
- **Voltaje**: 12V DC
- **Potencia**: 50W (panel completo)
- **Corriente**: 4.2A
- **Espectro**: Full spectrum (380-780nm)
- **Vida útil**: 50,000 horas

#### Conexiones
```
Panel LED 12V     Relé 4          Fuente 12V
-------------     ------          ----------
+ (Rojo)   <-->   COM/NO   <-->   + 12V
- (Negro)  <-->   Directo  <-->   - 12V (GND)
```

#### Control PWM (Opcional)
```
Driver LED        ESP32
----------        -----
PWM        <-->   GPIO 19 (PWM)
VCC        <-->   3.3V
GND        <-->   GND
```

## Alimentación y Distribución de Energía

### Fuente de Alimentación Principal

#### Especificaciones
- **Voltaje**: 12V DC regulado
- **Corriente**: 10A mínimo
- **Potencia**: 120W
- **Protección**: Sobre-corriente, sobre-voltaje, corto-circuito
- **Eficiencia**: >85%

#### Distribución
```
Fuente 12V        Distribución
----------        ------------
+ 12V      <-->   Barra positiva (10A)
- 12V      <-->   Barra negativa (GND)
```

### Regulador 5V para ESP32

#### Especificaciones
- **Entrada**: 12V DC
- **Salida**: 5V DC / 2A
- **Regulador**: LM2596 o equivalente
- **Eficiencia**: >90%
- **Protección**: Térmica y sobre-corriente

#### Conexiones
```
Regulador 5V      ESP32
------------      -----
VIN        <-->   12V
GND        <-->   GND común
VOUT       <-->   5V (Vin ESP32)
GND        <-->   GND ESP32
```

### Regulador 3.3V para Sensores

#### Especificaciones
- **Entrada**: 5V DC
- **Salida**: 3.3V DC / 1A
- **Regulador**: AMS1117-3.3 o equivalente
- **Dropout**: <1.2V
- **Capacitores**: 100µF entrada, 10µF salida

#### Conexiones
```
Regulador 3.3V    Sensores
--------------    --------
VIN        <-->   5V
GND        <-->   GND común
VOUT       <-->   3.3V sensores
GND        <-->   GND sensores
```

## Instalación y Montaje

### Gabinete de Control

#### Especificaciones
- **Material**: Policarbonato IP65
- **Dimensiones**: 300x400x200mm
- **Ventilación**: Rejillas con filtros
- **Acceso**: Puerta frontal con cerradura
- **Montaje**: Fijación a pared o poste

#### Distribución Interna
```
Nivel Superior:  ESP32, Módulo RS485, Reguladores
Nivel Medio:     Módulo de Relés, Borneras
Nivel Inferior:  Fuente de alimentación, Fusibles
```

### Cableado Estructurado

#### Especificaciones de Cables
- **Sensores**: Cable blindado 4x0.5mm² (hasta 20m)
- **Actuadores**: Cable no blindado 2x1.5mm² (hasta 50m)
- **Alimentación**: Cable 2x2.5mm² (hasta 100m)
- **Comunicaciones**: Cable UTP Cat5e (hasta 100m)

#### Identificación
- **Rojo**: Positivo (+12V, +5V, +3.3V)
- **Negro**: Negativo (GND)
- **Azul**: Datos (TX, SDA, DATA)
- **Blanco**: Clock (RX, SCL, CLK)
- **Verde**: Control (Enable, Reset, Control)

### Protecciones Eléctricas

#### Fusibles Principales
- **Fuente 12V**: 15A
- **Bomba principal**: 3A
- **Iluminación**: 5A
- **Ventiladores**: 2A cada uno
- **Electrónica**: 1A

#### Protecciones ESD
- **Varistores**: 275V en entrada AC
- **Capacitores**: 100nF en cada línea digital
- **Ferrites**: En cables de sensores largos

## Configuración de Software

### Definición de Pines
```cpp
// Sensores digitales
#define DHT22_PIN 4
#define TRIG_PIN 5
#define ECHO_PIN 18

// Sensores analógicos
#define SOIL_MOISTURE_PIN 36

// I2C
#define SDA_PIN 21
#define SCL_PIN 22

// RS485
#define RS485_RX 16
#define RS485_TX 17
#define RS485_DE 2

// Relés (activo bajo)
#define RELAY_PUMP 13
#define RELAY_FAN1 12
#define RELAY_FAN2 14
#define RELAY_LED 27
#define RELAY_NUTRIENT 26
#define RELAY_VALVE1 25
#define RELAY_VALVE2 33
#define RELAY_HEATER 32
```

### Inicialización
```cpp
void setup() {
  // Configurar pines de salida
  pinMode(RELAY_PUMP, OUTPUT);
  pinMode(RELAY_FAN1, OUTPUT);
  // ... resto de relés
  
  // Estado inicial (relés desactivados)
  digitalWrite(RELAY_PUMP, HIGH);
  digitalWrite(RELAY_FAN1, HIGH);
  // ... resto de relés
  
  // Inicializar I2C
  Wire.begin(SDA_PIN, SCL_PIN);
  
  // Inicializar sensores
  dht.begin();
  bh1750.begin();
  as7341.begin();
}
```

## Mantenimiento y Calibración

### Rutina de Mantenimiento Semanal
1. **Verificar conexiones**: Revisar apriete de borneras
2. **Limpiar sensores**: Especialmente los expuestos a humedad
3. **Probar actuadores**: Verificar funcionamiento de relés
4. **Revisar alimentación**: Medir voltajes de salida

### Calibración Mensual
1. **Sensor de humedad**: Calibrar con aire seco y agua destilada
2. **Sensor de pH**: Calibrar con soluciones buffer 4.0, 7.0, 10.0
3. **Sensor de nivel**: Verificar medición con recipiente graduado
4. **Sensor de luz**: Comparar con luxómetro profesional

### Reemplazo de Componentes
- **Sensores**: Vida útil 2-3 años en ambiente húmedo
- **Relés**: Reemplazar cada 100,000 ciclos
- **Cables**: Inspeccionar anualmente, reemplazar si hay oxidación
- **Fusibles**: Tener repuestos de todos los valores

## Solución de Problemas

### Problema: Lecturas Inestables
- **Verificar**: Calidad de conexiones, interferencia electromagnética
- **Solución**: Cables blindados, ferrites, separación de cables de potencia

### Problema: Actuadores No Responden
- **Verificar**: Voltaje de alimentación, estado de fusibles
- **Solución**: Revisar cableado, reemplazar fusibles, probar relés

### Problema: Comunicación I2C Fallida
- **Verificar**: Conexiones SDA/SCL, resistencias pull-up
- **Solución**: Verificar direcciones, velocidad de comunicación

Esta guía proporciona toda la información necesaria para realizar las conexiones hardware del sistema de invernadero ESP32 de manera segura y profesional.
