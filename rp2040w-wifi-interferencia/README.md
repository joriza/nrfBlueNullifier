# rp2040w-wifi-interferencia

Sketches para RP2040W que utilizan comandos WiFi para generar la máxima interferencia posible dentro de las limitaciones del hardware CYW43439.

## ⚠️ ADVERTENCIA IMPORTANTE

**Este código NO realiza interferencia real de portadora continua.**

El RP2040W utiliza el chip CYW43439 con firmware cerrado que **NO permite**:
- Transmisión de portadora continua
- Control directo del hardware de radio
- Transmisión de paquetes raw
- Modificación de parámetros PHY en tiempo real

Estos sketches generan **congestión WiFi estándar** mediante:
- Cambio rápido de canales
- Transmisión de Probe Requests
- Simulación de paquetes de beacon
- Generación de tráfico de control WiFi

**Efecto esperado:**
- ✅ Congestión moderada en redes WiFi cercanas
- ⚠️ Efecto muy limitado sobre Bluetooth (solo por superposición de espectro)
- ❌ NO es un jammer real

## Legalidad

El uso de este código puede violar regulaciones locales de espectro radioeléctrico:

| Región | Regulación | Penalidades |
|--------|------------|-------------|
| EE.UU. | FCC Part 15 | Hasta $112,000 USD por violación |
| Unión Europea | ETSI EN 300 440 | Multas y confiscación de equipos |
| Argentina | ENACOM | Multas y penas legales |
| México | COFETEL | Multas y sanciones administrativas |

**Úsolo en:**
- ✅ Laboratorios aislados con jaula de Faraday
- ✅ Entornos controlados con fines educativos
- ✅ Investigación académica autorizada

**NO usar para:**
- ❌ Interferir con redes públicas o privadas
- ❌ Disruptar comunicaciones de emergencia
- ❌ Cualquier propósito malicioso

## Requisitos de Hardware

- **Placa:** Raspberry Pi Pico W (RP2040W)
- **Conexión USB:** Para alimentación y carga de código

## Requisitos de Software

- **Arduino IDE** 2.0 o superior
- **Plataforma:** Raspberry Pi Pico / RP2040
- **Bibliotecas:**
  - `WiFi.h` (incluida en el núcleo RP2040)
  - `pico/cyw43_arch.h` (incluida en el núcleo RP2040)

### Instalación del núcleo RP2040

1. Abrir Arduino IDE
2. Ir a **File > Preferences**
3. En "Additional Boards Manager URLs", agregar:
   ```
   https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json
   ```
4. Ir a **Tools > Board > Boards Manager**
5. Buscar "Raspberry Pi Pico" e instalar

## Sketches Disponibles

### 1. `rp2040w-wifi-interferencia.ino`

**Versión básica** que utiliza comandos WiFi estándar.

**Características:**
- Cambio de canal cada 10ms
- Escaneo activo de redes
- Transmisión de paquetes de beacon simulados
- Estadísticas cada 5 segundos

**Uso:**
1. Abrir el sketch en Arduino IDE
2. Seleccionar la placa: **Raspberry Pi Pico W**
3. Seleccionar el puerto USB correcto
4. Cargar el sketch
5. Abrir el Monitor Serial (115200 baudios)
6. Presionar cualquier tecla para comenzar
7. Presionar 's' para detener

### 2. `rp2040w-wifi-interferencia-avanzado.ino`

**Versión avanzada** que utiliza funciones de bajo nivel del CYW43439.

**Características:**
- Cambio ultra-rápido de canal (1ms)
- Transmisión continua de Probe Requests
- Simulación de paquetes de beacon
- Modo promiscuo (si es soportado)
- Configuración de interrupciones para máxima velocidad

**Uso:**
1. Abrir el sketch en Arduino IDE
2. Seleccionar la placa: **Raspberry Pi Pico W**
3. Seleccionar el puerto USB correcto
4. Cargar el sketch
5. Abrir el Monitor Serial (115200 baudios)
6. Presionar cualquier tecla para comenzar

**Comandos:**
- `s` - Detener interferencia
- `i` - Mostrar información del sistema

## Comparación de Versiones

| Característica | Básica | Avanzada |
|----------------|--------|----------|
| Cambio de canal | 10ms | 1ms |
| Probe Requests | Cada 50ms | Cada 5ms |
| Modo promiscuo | No | Sí |
| Estadísticas | 5s | 3s |
| Comandos | 's' | 's', 'i' |
| Complejidad | Baja | Media |

## Espectro de Frecuencias

### Canales WiFi y Superposición con Bluetooth

| Canal WiFi | Frecuencia (MHz) | Canales Bluetooth Afectados |
|------------|------------------|------------------------------|
| 1 | 2412 | 0-3 |
| 2 | 2417 | 4-7 |
| 3 | 2422 | 8-11 |
| 4 | 2427 | 12-15 |
| 5 | 2432 | 16-19 |
| 6 | 2437 | 20-23 |
| 7 | 2442 | 24-27 |
| 8 | 2447 | 28-31 |
| 9 | 2452 | 32-35 |
| 10 | 2457 | 36-39 |
| 11 | 2462 | 40-43 |
| 12 | 2467 | 44-47 |
| 13 | 2472 | 48-51 |

**Nota:** Cada canal WiFi tiene un ancho de banda de ~22MHz, cubriendo aproximadamente 4 canales Bluetooth.

## Salida del Monitor Serial

### Versión Básica

```
========================================
  rp2040w-wifi-interferencia
========================================

⚠️  ADVERTENCIAS:
- Este NO es un jammer real de portadora continua
- El hardware CYW43439 tiene limitaciones de firmware
- Solo genera congestión WiFi estándar
- Efecto sobre Bluetooth es muy limitado
- El uso puede ser ilegal en su jurisdicción

Presione cualquier tecla para continuar o RESET para cancelar
========================================

Iniciando interferencia WiFi...
Canales WiFi: 1-13
Cada canal WiFi cubre ~4 canales Bluetooth
========================================

Canal WiFi: 1 (Bluetooth: 0-3)
Canal WiFi: 2 (Bluetooth: 4-7)
...

========== ESTADÍSTICAS ==========
Canales cambiados:  5000
Escaneos realizados: 1000
Paquetes generados:  6000
Canal actual:        13
Tiempo de operación:  50.0 segundos
Canales/segundo:     100.0
==================================
```

### Versión Avanzada

```
╔════════════════════════════════════════╗
║  rp2040w-wifi-interferencia-avanzado  ║
║         Versión Avanzada               ║
╚════════════════════════════════════════╝

⚠️  ADVERTENCIAS:
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
• Este NO es un jammer real de portadora continua
• El hardware CYW43439 tiene limitaciones de firmware
• Solo genera congestión WiFi estándar
• Efecto sobre Bluetooth es muy limitado
• El uso puede ser ILEGAL en su jurisdicción
• Úsolo en entornos controlados
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

Técnicas utilizadas:
  • Cambio de canal cada 1ms
  • Transmisión continua de Probe Requests
  • Simulación de paquetes de beacon
  • Múltiples tipos de paquetes de control

Comandos disponibles:
  's' - Detener interferencia
  'i' - Mostrar información del sistema
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━

✓ Sistema iniciado
✓ Iniciando interferencia...

╔════════════════════════════════════════╗
║           ESTADÍSTICAS                  ║
╠════════════════════════════════════════╣
║ Canales cambiados:  3000              ║
║ Probes enviados:    600               ║
║ Beacons simulados:  150               ║
║ Total paquetes:     3750              ║
║ Canal actual:       7                 ║
║ Tiempo operación:   3.0s              ║
║ Cambios/segundo:    1000.0            ║
║ Paquetes/segundo:   1250.0            ║
╚════════════════════════════════════════╝
```

## Limitaciones Conocidas

1. **No es portadora continua:** El CYW43439 no permite transmisión de portadora continua
2. **Firmware cerrado:** No hay acceso directo al hardware de radio
3. **Efecto limitado:** Solo genera congestión WiFi estándar
4. **Sin control de potencia:** La potencia de transmisión está limitada por el firmware
5. **Sin paquetes raw:** No se pueden transmitir paquetes personalizados

## Alternativas para Interferencia Real

Si necesita interferencia real de Bluetooth, considere:

1. **nrf24L01+ con RP2040** - Hardware idéntico al sketch original
2. **HackRF One** - SDR con control total del espectro (~$300 USD)
3. **LimeSDR** - SDR más económico (~$150 USD)
4. **ESP32 + nrf24L01+** - Similar al original pero con ESP32

Consulte el documento `RP2040W-Limitaciones-y-Alternativas.md` para más detalles.

## Solución de Problemas

### El sketch no se carga

- Asegúrese de seleccionar "Raspberry Pi Pico W" como placa
- Mantenga presionado el botón BOOTSEL mientras conecta el USB
- Verifique que el puerto USB sea correcto

### No se ve nada en el Monitor Serial

- Asegúrese de que la velocidad sea 115200 baudios
- Verifique que el puerto USB sea correcto
- Presione RESET en la placa

### El sketch se ejecuta pero no parece hacer nada

- Esto es esperado: la interferencia WiFi es invisible
- Use un analizador de espectro para verificar la actividad
- Verifique que el Monitor Serial muestre estadísticas

## Recursos

- [Arduino Pico Core](https://github.com/earlephilhower/arduino-pico)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)
- [CYW43439 Datasheet](https://www.infineon.com/dgdl/Infineon-AIROC-CYW43439-DataSheet-v01_00-EN.pdf)
- [Pico SDK](https://github.com/raspberrypi/pico-sdk)

## Licencia

Este código se proporciona solo con fines educativos. El autor no es responsable del mal uso de este software.

## Agradecimientos

- Basado en el proyecto **nrfBlueNullifier** de WireBits
- Adaptado para RP2040W por el usuario

## Descargo de Responsabilidad

Este código se proporciona "tal cual" sin garantía de ningún tipo. El uso de este código es responsabilidad exclusiva del usuario. El autor no se hace responsable de ningún daño, legal o de otro tipo, que pueda resultar del uso de este software.
