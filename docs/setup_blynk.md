# Guía de Configuración Blynk - Sistema de Invernadero ESP32

## Descripción General

Esta guía proporciona las instrucciones detalladas para configurar la aplicación móvil Blynk con el sistema de monitoreo y control del invernadero ESP32. El sistema utiliza pines virtuales específicos para cada sensor y actuador, requiriendo una configuración precisa de widgets para garantizar el funcionamiento óptimo.

## Requisitos Previos

### Software Necesario
- **Aplicación Blynk Legacy** (versión 2.27.x o superior)
- **Cuenta Blynk** activa con créditos suficientes
- **Dispositivo móvil** con Android 6.0+ o iOS 12.0+

### Hardware Requerido
- **ESP32 DevKit V1** configurado y operativo
- **Conexión WiFi** estable y confiable
- **Sensores y actuadores** instalados según especificaciones

## Configuración de Proyecto Blynk

### Paso 1: Creación del Proyecto

1. **Abrir aplicación Blynk** en dispositivo móvil
2. **Crear nuevo proyecto**:
   - Nombre: `Invernadero ESP32 - [Tu Nombre]`
   - Dispositivo: `ESP32 Dev Board`
   - Conexión: `WiFi`
3. **Anotar el Auth Token** generado automáticamente
4. **Configurar orientación**: Modo retrato recomendado

### Paso 2: Configuración de Auth Token

El Auth Token debe integrarse en el código ESP32:
```cpp
// En archivo credentials.h
#define BLYNK_AUTH_TOKEN "TuTokenAquí"
```

## Configuración de Widgets - Sensores

### Grupo 1: Sensor DHT22 - Monitoreo Ambiental

#### Widget 1: Temperatura del Aire
- **Tipo**: `Value Display`
- **Pin Virtual**: `V0`
- **Nombre**: `Temperatura`
- **Unidades**: `°C`
- **Rango**: `-10 a 50`
- **Frecuencia**: `1 segundo`
- **Color**: `Verde (#4CAF50)`

#### Widget 2: Humedad Relativa
- **Tipo**: `Gauge`
- **Pin Virtual**: `V1`
- **Nombre**: `Humedad`
- **Unidades**: `%`
- **Rango**: `0 a 100`
- **Frecuencia**: `1 segundo`
- **Color**: `Azul (#2196F3)`

#### Widget 3: Índice de Calor
- **Tipo**: `Value Display`
- **Pin Virtual**: `V2`
- **Nombre**: `Índice Calor`
- **Unidades**: `°C`
- **Rango**: `15 a 60`
- **Frecuencia**: `1 segundo`
- **Color**: `Naranja (#FF9800)`

### Grupo 2: Sensor AS7341 - Análisis Espectral

#### Widget 4: Intensidad Lumínica Total
- **Tipo**: `SuperChart`
- **Pin Virtual**: `V3`
- **Nombre**: `Lux Total`
- **Unidades**: `lux`
- **Rango**: `0 a 100000`
- **Período**: `1 hora`
- **Color**: `Amarillo (#FFEB3B)`

#### Widget 5: Temperatura de Color
- **Tipo**: `Value Display`
- **Pin Virtual**: `V4`
- **Nombre**: `Color Temp`
- **Unidades**: `K`
- **Rango**: `2000 a 10000`
- **Frecuencia**: `5 segundos`
- **Color**: `Violeta (#9C27B0)`

#### Widget 6-10: Espectro de Colores
- **Tipo**: `LED` (indicadores de intensidad)
- **Pines Virtuales**: `V5` (Rojo), `V6` (Verde), `V7` (Azul), `V8` (Clear), `V9` (NIR)
- **Nombres**: `R`, `G`, `B`, `Clear`, `NIR`
- **Configuración**: Intensidad proporcional al valor

### Grupo 3: Sensor de Humedad del Suelo

#### Widget 11: Humedad del Sustrato
- **Tipo**: `Gauge`
- **Pin Virtual**: `V10`
- **Nombre**: `Humedad Suelo`
- **Unidades**: `%`
- **Rango**: `0 a 100`
- **Zonas**: 
  - Rojo: 0-30 (Seco)
  - Amarillo: 30-60 (Óptimo)
  - Azul: 60-100 (Húmedo)

#### Widget 12: Valor Analógico Bruto
- **Tipo**: `Value Display`
- **Pin Virtual**: `V11`
- **Nombre**: `Suelo Raw`
- **Rango**: `0 a 4095`
- **Frecuencia**: `5 segundos`

### Grupo 4: Sensor BH1750 - Luxómetro

#### Widget 13: Luminosidad Ambiente
- **Tipo**: `SuperChart`
- **Pin Virtual**: `V12`
- **Nombre**: `Lux Ambiente`
- **Unidades**: `lux`
- **Período**: `24 horas`
- **Resolución**: `1 punto/minuto`

#### Widget 14: Nivel de Luz
- **Tipo**: `Value Display`
- **Pin Virtual**: `V13`
- **Nombre**: `Nivel Luz`
- **Valores**: `Bajo/Medio/Alto`

### Grupo 5: Sensor HC-SR04 - Nivel de Agua

#### Widget 15: Distancia al Agua
- **Tipo**: `Value Display`
- **Pin Virtual**: `V14`
- **Nombre**: `Nivel Agua`
- **Unidades**: `cm`
- **Rango**: `0 a 50`

#### Widget 16: Porcentaje de Capacidad
- **Tipo**: `Level V`
- **Pin Virtual**: `V15`
- **Nombre**: `Capacidad %`
- **Rango**: `0 a 100`
- **Color**: Gradiente azul

#### Widget 17: Estado del Reservorio
- **Tipo**: `Value Display`
- **Pin Virtual**: `V16`
- **Nombre**: `Estado Agua`
- **Valores**: `Vacío/Bajo/Medio/Lleno`

### Grupo 6: Sensor RS485 - Análisis del Suelo

#### Widget 18: Temperatura del Suelo
- **Tipo**: `Value Display`
- **Pin Virtual**: `V17`
- **Nombre**: `Temp Suelo`
- **Unidades**: `°C`
- **Rango**: `5 a 45`

#### Widget 19: Humedad RS485
- **Tipo**: `Gauge`
- **Pin Virtual**: `V18`
- **Nombre**: `Hum RS485`
- **Unidades**: `%`
- **Rango**: `0 a 100`

#### Widget 20: Conductividad Eléctrica
- **Tipo**: `Value Display`
- **Pin Virtual**: `V19`
- **Nombre**: `EC`
- **Unidades**: `µS/cm`
- **Rango**: `0 a 5000`

#### Widget 21: pH del Suelo
- **Tipo**: `Gauge`
- **Pin Virtual**: `V20`
- **Nombre**: `pH`
- **Rango**: `4.0 a 9.0`
- **Zonas**:
  - Rojo: 4.0-5.5 (Ácido)
  - Verde: 5.5-7.5 (Óptimo)
  - Azul: 7.5-9.0 (Alcalino)

#### Widget 22-24: Nutrientes NPK
- **Tipo**: `Value Display`
- **Pines Virtuales**: `V21` (N), `V22` (P), `V23` (K)
- **Nombres**: `Nitrógeno`, `Fósforo`, `Potasio`
- **Unidades**: `mg/kg`
- **Rangos**: `0 a 2000`

## Configuración de Widgets - Actuadores

### Grupo 7: Control de Relés

#### Widget 25-32: Relés de Potencia
- **Tipo**: `Button`
- **Modo**: `Switch`
- **Pines Virtuales**: `V24` a `V31`
- **Nombres**:
  - V24: `Bomba Principal`
  - V25: `Ventilador Extracción`
  - V26: `Ventilador Circulación`
  - V27: `Iluminación LED`
  - V28: `Bomba Nutrientes`
  - V29: `Válvula Zona 1`
  - V30: `Válvula Zona 2`
  - V31: `Calefacción`
- **Configuración**:
  - Texto ON: `ACTIVO`
  - Texto OFF: `INACTIVO`
  - Color ON: `Verde`
  - Color OFF: `Gris`

## Configuración de Widgets - Targets Ajustables

### Grupo 8: Control de Objetivos del Sistema

#### Widget 33: Target Temperatura
- **Tipo**: `Slider`
- **Pin Virtual**: `V40`
- **Nombre**: `Temp. Objetivo`
- **Unidades**: `°C`
- **Rango**: `15 a 35`
- **Paso**: `0.5`
- **Valor por defecto**: `22.0`
- **Color**: `Rojo (#F44336)`

#### Widget 34: Target Humedad
- **Tipo**: `Slider`
- **Pin Virtual**: `V41`
- **Nombre**: `Humedad Objetivo`
- **Unidades**: `%`
- **Rango**: `40 a 80`
- **Paso**: `1`
- **Valor por defecto**: `65`
- **Color**: `Azul (#2196F3)`

#### Widget 35: Target Humedad del Suelo
- **Tipo**: `Slider`
- **Pin Virtual**: `V42`
- **Nombre**: `Hum. Suelo Objetivo`
- **Unidades**: `%`
- **Rango**: `30 a 80`
- **Paso**: `1`
- **Valor por defecto**: `60`
- **Color**: `Marrón (#795548)`

#### Widget 36: Target Lux Mínimo
- **Tipo**: `Slider`
- **Pin Virtual**: `V43`
- **Nombre**: `Lux Mínimo`
- **Unidades**: `lux`
- **Rango**: `1000 a 20000`
- **Paso**: `500`
- **Valor por defecto**: `5000`
- **Color**: `Amarillo (#FFEB3B)`

#### Widget 37: Target Temperatura Ventilación
- **Tipo**: `Slider`
- **Pin Virtual**: `V44`
- **Nombre**: `Temp. Ventilación`
- **Unidades**: `°C`
- **Rango**: `20 a 35`
- **Paso**: `0.5`
- **Valor por defecto**: `28.0`
- **Color**: `Cyan (#00BCD4)`

#### Widget 38: Target pH del Suelo
- **Tipo**: `Slider`
- **Pin Virtual**: `V45`
- **Nombre**: `pH Objetivo`
- **Rango**: `5.5 a 7.5`
- **Paso**: `0.1`
- **Valor por defecto**: `6.5`
- **Color**: `Violeta (#9C27B0)`

#### Widget 39: Target EC del Suelo
- **Tipo**: `Slider`
- **Pin Virtual**: `V46`
- **Nombre**: `EC Objetivo`
- **Unidades**: `µS/cm`
- **Rango**: `800 a 2500`
- **Paso**: `50`
- **Valor por defecto**: `1500`
- **Color**: `Índigo (#3F51B5)`

#### Widget 40: Target Nivel Agua Crítico
- **Tipo**: `Slider`
- **Pin Virtual**: `V47`
- **Nombre**: `Nivel Crítico`
- **Unidades**: `%`
- **Rango**: `10 a 30`
- **Paso**: `1`
- **Valor por defecto**: `20`
- **Color**: `Teal (#009688)`

## Configuración de Notificaciones

### Alertas Críticas

#### Temperatura Extrema
- **Condición**: Temperatura < 10°C o > 35°C
- **Mensaje**: `⚠️ ALERTA: Temperatura crítica en invernadero`
- **Frecuencia**: Inmediata

#### Humedad Crítica
- **Condición**: Humedad > 90% por >30 min
- **Mensaje**: `💧 ALERTA: Humedad excesiva - Riesgo de hongos`
- **Frecuencia**: Una vez por hora

#### Nivel de Agua Bajo
- **Condición**: Nivel < 20%
- **Mensaje**: `🚰 ALERTA: Nivel de agua bajo - Recargar reservorio`
- **Frecuencia**: Cada 2 horas

#### pH Fuera de Rango
- **Condición**: pH < 5.5 o > 7.5
- **Mensaje**: `⚗️ ALERTA: pH del suelo fuera de rango óptimo`
- **Frecuencia**: Cada 6 horas

## Organización de Pantallas

### Pantalla Principal (Dashboard)
- **Widgets esenciales**: Temperatura, Humedad, Humedad del suelo, Nivel de agua
- **Controles rápidos**: Bomba principal, Ventiladores, LED
- **Indicadores de estado**: Conexión, Estado general del sistema

### Pantalla de Sensores Detallados
- **Gráficos históricos**: SuperCharts con datos de 24h
- **Análisis espectral**: Widgets de color AS7341
- **Monitoreo del suelo**: RS485 completo con NPK

### Pantalla de Control de Actuadores
- **Todos los relés**: Control individual con estados
- **Programación**: Timers y secuencias automáticas
- **Estadísticas**: Tiempo de uso y consumo

### Pantalla de Configuración de Targets
- **Sliders de objetivos**: Todos los targets ajustables
- **Indicadores de rango**: Valores mínimos y máximos permitidos
- **Estado de configuración**: Validación en tiempo real

## Configuración de Targets del Sistema

### Configuración Inicial de Targets
Al configurar los sliders de targets, es importante establecer los valores correctos según el tipo de cultivo:

#### Para Hortalizas de Hoja (Lechuga, Espinaca)
- **Temperatura**: 18-22°C
- **Humedad**: 60-70%
- **Humedad Suelo**: 65-75%
- **pH**: 6.0-6.5

#### Para Tomates y Pimientos
- **Temperatura**: 22-26°C
- **Humedad**: 50-65%
- **Humedad Suelo**: 55-65%
- **pH**: 6.2-6.8

#### Para Hierbas Aromáticas
- **Temperatura**: 20-24°C
- **Humedad**: 45-60%
- **Humedad Suelo**: 50-60%
- **pH**: 6.0-7.0

### Sincronización de Targets
El sistema automáticamente:
1. **Guarda cambios** en memoria EEPROM cada 5 minutos
2. **Valida rangos** antes de aplicar nuevos valores
3. **Sincroniza** con la app al reconectarse
4. **Notifica cambios** mediante eventos Blynk

## Configuración de Eventos Automáticos

### Regla 1: Riego Automático
- **Condición**: Humedad del suelo < 40%
- **Acción**: Activar bomba por 3 minutos
- **Cooldown**: 2 horas mínimo entre riegos

### Regla 2: Ventilación por Temperatura
- **Condición**: Temperatura > 28°C
- **Acción**: Activar ventiladores
- **Duración**: Hasta que temperatura < 26°C

### Regla 3: Iluminación Nocturna
- **Condición**: Lux < 1000 AND hora entre 18:00-22:00
- **Acción**: Activar LEDs al 70%
- **Duración**: 4 horas máximo

### Regla 4: Calefacción Nocturna
- **Condición**: Temperatura < 15°C AND hora entre 22:00-06:00
- **Acción**: Activar calefacción
- **Duración**: Hasta temperatura > 18°C

## Recomendaciones de Uso

### Monitoreo Diario
1. **Verificar dashboard** cada mañana y tarde
2. **Revisar alertas** inmediatamente al recibirlas
3. **Analizar gráficos** semanalmente para optimización

### Mantenimiento Semanal
1. **Calibrar sensores** según sea necesario
2. **Verificar funcionamiento** de todos los actuadores
3. **Revisar consumo** de agua y energía

### Optimización Mensual
1. **Ajustar umbrales** basado en rendimiento de plantas
2. **Actualizar reglas** de automatización
3. **Analizar datos históricos** para mejoras

## Solución de Problemas

### Problema: Dispositivo Desconectado
- **Verificar**: Conexión WiFi del ESP32
- **Solución**: Reiniciar dispositivo, verificar credenciales

### Problema: Lecturas Erróneas
- **Verificar**: Cableado de sensores
- **Solución**: Recalibrar sensores, verificar alimentación

### Problema: Actuadores No Responden
- **Verificar**: Estado de relés, alimentación
- **Solución**: Verificar configuración de pines, probar manualmente

Esta guía proporciona la configuración completa para un sistema de invernadero profesional con monitoreo integral y control automatizado a través de la plataforma Blynk.
