# Sistema de Actuadores - Invernadero ESP32

## Descripción General

El sistema de control del invernadero integra seis actuadores especializados para la automatización completa del ambiente de cultivo. Cada actuador ejecuta acciones específicas que permiten mantener las condiciones óptimas de crecimiento de las plantas de forma automática y precisa.

## Lista de Actuadores Integrados

### 1. Ventilador (FanActuator) - Control de Ventilación

**Funciones que realiza:**
- Activación/desactivación automática de ventilación
- Control de flujo de aire interno
- Renovación del aire del invernadero
- Reducción de humedad excesiva

**Para qué sirve:**
- Prevención de enfermedades fúngicas por humedad estancada
- Control de temperatura mediante circulación de aire
- Renovación de CO₂ para optimizar la fotosíntesis
- Fortalecimiento del tallo de las plantas (tigmomorfogénesis)

**Por qué es necesario en un invernadero:**
La ventilación adecuada previene la acumulación de humedad que puede causar hongos como botrytis o mildiu. Además, el movimiento del aire ayuda a mantener temperaturas uniformes y proporciona el intercambio gaseoso necesario para la respiración y fotosíntesis de las plantas.

### 2. Calefactor (HeaterActuator) - Control de Temperatura

**Funciones que realiza:**
- Calentamiento automático del ambiente interno
- Mantenimiento de temperatura mínima nocturna
- Protección contra heladas
- Control de temperatura con límites de seguridad

**Para qué sirve:**
- Extensión de la temporada de cultivo
- Protección de plantas sensibles al frío
- Optimización de la germinación de semillas
- Aceleración del crecimiento en condiciones frías

**Por qué es necesario en un invernadero:**
Las temperaturas bajas pueden detener el crecimiento, causar estrés térmico o incluso matar las plantas. El calefactor permite mantener un rango térmico óptimo independientemente de las condiciones externas, garantizando productividad durante todo el año.

### 3. Tira LED (LEDStripActuator) - Iluminación Suplementaria

**Funciones que realiza:**
- Iluminación artificial programable
- Control de intensidad luminosa (PWM)
- Extensión del fotoperiodo
- Suplemento lumínico en días nublados

**Para qué sirve:**
- Promoción de crecimiento vegetativo
- Control de floración mediante fotoperiodo
- Compensación de luz insuficiente en invierno
- Optimización de la fotosíntesis

**Por qué es necesario en un invernadero:**
La luz es el factor limitante más común en invernaderos. La iluminación suplementaria permite maximizar la productividad, controlar los ciclos de crecimiento y mantener cultivos durante períodos de baja luminosidad natural.

### 4. Bomba de Agua (WaterPumpActuator) - Sistema de Riego

**Funciones que realiza:**
- Riego automático programado
- Distribución de agua a las plantas
- Control de ciclos de riego
- Gestión del suministro hídrico

**Para qué sirve:**
- Mantenimiento de humedad óptima del sustrato
- Distribución uniforme de nutrientes disueltos
- Automatización completa del riego
- Prevención de estrés hídrico

**Por qué es necesario en un invernadero:**
El riego manual es impreciso y laborioso. La bomba automática asegura que las plantas reciban la cantidad exacta de agua en el momento adecuado, optimizando el crecimiento y reduciendo el desperdicio de agua y nutrientes.

### 5. Servo Motor (ServoActuator) - Control de Ventilación Dirigida

**Funciones que realiza:**
- Apertura/cierre de conductos de ventilación
- Control direccional del flujo de aire
- Graduación de la intensidad de ventilación
- Posicionamiento preciso de elementos móviles

**Para qué sirve:**
- Control fino de la ventilación
- Dirección específica del flujo de aire
- Apertura gradual para evitar cambios bruscos
- Automatización de elementos mecánicos móviles

**Por qué es necesario en un invernadero:**
Permite un control más sofisticado de la ventilación que simplemente encender/apagar un ventilador. El servo puede regular gradualmente la apertura de conductos, proporcionando un control más preciso del microclima.

### 6. Controlador de Relés (RelayController) - Gestión Centralizada

**Funciones que realiza:**
- Control centralizado de todos los actuadores
- Gestión de alimentación eléctrica
- Protección contra sobrecargas
- Secuenciación de activación de equipos

**Para qué sirve:**
- Coordinación de múltiples actuadores
- Protección del sistema eléctrico
- Optimización del consumo energético
- Gestión segura de cargas eléctricas

**Por qué es necesario en un invernadero:**
Centraliza el control eléctrico del sistema, evitando sobrecargas y permitiendo secuencias de activación inteligentes. Es fundamental para la seguridad eléctrica y la coordinación eficiente de todos los componentes.

## Integración del Sistema

Todos los actuadores están integrados en un sistema unificado que:

- Ejecuta acciones automáticas basadas en datos de sensores
- Implementa algoritmos de control inteligente (PID, lógica difusa)
- Coordina múltiples actuadores para acciones complejas
- Mantiene protocolos de seguridad y límites operativos
- Proporciona control manual remoto a través de Blynk
- Registra estadísticas de uso y eficiencia energética

## Automatización y Control

El sistema permite:

- **Control Reactivo:** Respuesta automática a cambios ambientales
- **Control Predictivo:** Acciones preventivas basadas en tendencias
- **Control Manual:** Intervención remota a través de la aplicación
- **Control Programado:** Horarios y rutinas personalizadas
- **Control de Seguridad:** Límites y protecciones automáticas

## Secuencias de Control Típicas

### Control de Temperatura
1. **Calentamiento:** Activación del calefactor cuando la temperatura es baja
2. **Ventilación:** Activación del ventilador cuando la temperatura es alta
3. **Graduación:** Uso del servo para control fino de ventilación

### Control de Humedad
1. **Ventilación:** Activación de ventiladores para reducir humedad
2. **Calentamiento:** Uso del calefactor para reducir humedad relativa
3. **Riego:** Control de la bomba para mantener humedad del suelo

### Control de Iluminación
1. **Suplemento:** Activación de LEDs cuando la luz natural es insuficiente
2. **Fotoperiodo:** Control de ciclos día/noche para floración
3. **Intensidad:** Ajuste gradual según las necesidades del cultivo

Este conjunto de actuadores proporciona una solución completa para la automatización profesional de invernaderos, cubriendo todos los aspectos del control ambiental necesarios para un cultivo óptimo y automatizado.
