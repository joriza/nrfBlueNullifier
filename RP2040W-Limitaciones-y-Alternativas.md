# Limitaciones del RP2040W para Interferencia Bluetooth y Alternativas

## Limitaciones Técnicas del RP2040W

### 1. **Arquitectura del Chip CYW43439**
El RP2040W utiliza el chip Infineon CYW43439 para conectividad WiFi y Bluetooth. Este chip tiene las siguientes limitaciones:

| Aspecto | Limitación |
|---------|------------|
| **Firmware Propietario** | El firmware es binario cerrado de Infineon/Broadcom |
| **API Restringida** | Las bibliotecas estándar (pico-sdk, Arduino Pico) exponen solo APIs de alto nivel |
| **Sin Acceso RAW** | No hay acceso directo al hardware de radio para transmisión de portadora continua |
| **Sin Control de Potencia** | No se puede controlar la potencia de transmisión en tiempo real |
| **Sin Modo Promiscuo** | No hay modo de transmisión sin protocolo (raw TX) |

### 2. **Por Qué No Funciona la Interferencia con WiFi**

El sketch original nrfBlueNullifier funciona porque:

```cpp
// nrf24L01+ permite:
radio.startConstCarrier(RF24_PA_MAX, i);  // Transmisión de portadora continua
radio.setChannel(j);                        // Control directo del canal
```

El CYW43439 NO permite:

- ❌ `startConstCarrier()` - No existe API para transmisión de portadora continua
- ❌ Control directo de canal RF sin protocolo WiFi/Bluetooth
- ❌ Transmisión fuera del contexto de una conexión WiFi o Bluetooth
- ❌ Modificación de parámetros PHY en tiempo real

### 3. **Espectro WiFi vs Bluetooth**

| Característica | WiFi 2.4GHz | Bluetooth Clásico |
|----------------|-------------|-------------------|
| **Canales** | 14 (1-14) | 79 (0-78) |
| **Ancho de banda** | 20/40 MHz | 1 MHz |
| **Superposición** | Cada canal WiFi cubre ~4 canales Bluetooth | - |
| **Frecuencia** | 2412-2484 MHz | 2402-2480 MHz |

**Problema:** Aunque ambos operan en 2.4GHz, el CYW43439 solo puede transmitir usando protocolos WiFi (802.11) o Bluetooth, no ruido continuo.

---

## Alternativas de Hardware

### Opción 1: **nrf24L01+ con RP2040 (Recomendada)**

**Ventajas:**
- ✅ Hardware idéntico al sketch original
- ✅ API completa para control de radio
- ✅ Transmisión de portadora continua disponible
- ✅ Control directo de todos los 79 canales Bluetooth
- ✅ Bajo costo (~$2-5 USD)

**Conexión RP2040 a nrf24L01+:**

```
nrf24L01+    RP2040
---------    ------
VCC     →    3.3V
GND     →    GND
CE      →    GPIO 7
CSN     →    GPIO 5
SCK     →    GPIO 6 (SPI0 SCK)
MOSI    →    GPIO 4 (SPI0 TX)
MISO    →    GPIO 3 (SPI0 RX)
IRQ     →    GPIO 2 (opcional)
```

**Sketch Adaptado:**

```cpp
/*
 * nrfBlueNullifier para RP2040
 * Interferencia de Bluetooth usando nrf24L01+PA/LNA
 * Adaptado de WireBits original para ESP32
 */

#include <RF24.h>
#include <SPI.h>

// Pines RP2040 (SPI0)
#define CE_PIN   7
#define CSN_PIN  5

RF24 radio(CE_PIN, CSN_PIN);
byte i = 45, ptr_hop = 0, flag = 0;
byte hopping_channel[] = {32, 34, 46, 48, 50, 52, 0, 1, 2, 4, 6, 8, 22, 24, 26, 28, 30, 74, 76, 78, 80, 82, 84, 86};

void nrfInit() {
    SPI.begin();
    if (radio.begin()) {
        radio.setAutoAck(false);
        radio.stopListening();
        radio.setRetries(0, 0);
        radio.setPayloadSize(31);
        radio.setAddressWidth(4);
        radio.setPALevel(RF24_PA_MAX);
        radio.setDataRate(RF24_2MBPS);
        radio.setCRCLength(RF24_CRC_DISABLED);
        radio.startConstCarrier(RF24_PA_MAX, i);
        Serial.println("nrf24L01+ inicializado correctamente");
    } else {
        Serial.println("Error: No se detectó nrf24L01+");
    }
}

void adjustAndSweepChannels() {
    flag = (i > 79) ? 1 : (i < 2 ? 0 : flag);
    i += flag ? -2 : 2;
    for (int j = 0; j <= 79; j++) radio.setChannel(j);
}

void setup() {
    Serial.begin(115200);
    while (!Serial);
    
    Serial.println("nrfBlueNullifier - RP2040");
    nrfInit();
}

void loop() {
    adjustAndSweepChannels();
    ptr_hop = (ptr_hop + 1) % sizeof(hopping_channel);
    radio.setChannel(hopping_channel[ptr_hop]);
}
```

---

### Opción 2: **ESP32 con WiFi (Limitado)**

El ESP32 tiene un chip WiFi/Bluetooth más flexible, pero aún así tiene limitaciones similares.

**Posible enfoque:**
- Usar la API `esp_wifi_80211_tx()` para transmitir paquetes WiFi raw
- Transmitir en canales que se superpongan con Bluetooth
- **Limitación:** No es portadora continua, son paquetes WiFi

---

### Opción 3: **SDR (Software Defined Radio)**

Para interferencia real y control total, se necesita hardware SDR:

| Dispositivo | Costo | Capacidad |
|-------------|-------|-----------|
| **HackRF One** | ~$300 USD | Full duplex, 1 MHz - 6 GHz |
| **RTL-SDR** | ~$30 USD | Solo RX, no TX |
| **LimeSDR** | ~$150 USD | Full duplex, 100 kHz - 3.8 GHz |
| **ADALM-PLUTO** | ~$150 USD | Full duplex, 70 MHz - 6 GHz |

**Ejemplo con HackRF y GNU Radio:**

```python
# GNU Radio Companion flowgraph conceptual
import numpy as np
from gnuradio import gr, analog, blocks, uhd

# Generar ruido blanco
noise_source = analog.noise_source_c(gr.GR_GAUSSIAN, 1.0, 0)

# Configurar HackRF para transmitir
hackrf_sink = uhd.usrp_sink(
    device_addr="hackrf=0",
    stream_args=uhd.stream_args(
        cpu_format="fc32",
        channels=[0]
    )
)

# Configurar frecuencia y potencia
hackrf_sink.set_center_freq(2.45e9)  # 2.45 GHz (centro Bluetooth)
hackrf_sink.set_sample_rate(20e6)     # 20 MHz
hackrf_sink.set_gain(14)             # Máxima ganancia

# Conectar y ejecutar
noise_source.connect(hackrf_sink)
```

---

### Opción 4: **CC1352P / CC2652P (Texas Instruments)**

Estos chips SoC de TI tienen capacidades de radio más flexibles:

- ✅ RF Core programable
- ✅ Soporte para protocolos propietarios
- ✅ Transmisión de paquetes personalizados
- ⚠️ Requiere TI-RTOS y CCS (Code Composer Studio)
- ⚠️ Curva de aprendizaje alta

---

## Comparación de Opciones

| Opción | Costo | Dificultad | Efectividad | Legalidad |
|--------|-------|------------|-------------|-----------|
| **RP2040 + nrf24L01+** | ~$10 | Baja | Alta | ⚠️ Regulado |
| **ESP32 (WiFi raw)** | ~$5 | Media | Media | ⚠️ Regulado |
| **HackRF One** | ~$300 | Alta | Muy Alta | ⚠️ Regulado |
| **CC1352P** | ~$15 | Muy Alta | Alta | ⚠️ Regulado |

---

## Advertencias Legales

⚠️ **IMPORTANTE:** El uso de equipos de interferencia de radiofrecuencia está regulado y puede ser ilegal en:

| Región | Regulación | Penalidades |
|--------|------------|-------------|
| **EE.UU.** | FCC Part 15 | Hasta $112,000 USD por violación |
| **Unión Europea** | ETSI EN 300 440 | Multas y confiscación de equipos |
| **Argentina** | CNC/ENACOM | Multas y penas legales |
| **México** | COFETEL | Multas y sanciones administrativas |

**Usos permitidos:**
- ✅ Investigación académica en laboratorio aislado
- ✅ Pruebas de seguridad con autorización
- ✅ Desarrollo con jaula de Faraday

**Usos prohibidos:**
- ❌ Interferir con redes públicas o privadas
- ❌ Disruptar comunicaciones de emergencia
- ❌ Uso sin licencia o autorización

---

## Recursos Adicionales

### Para RP2040 + nrf24L01+:
- [RF24 Library](https://github.com/nRF24/RF24)
- [Pico SDK SPI Documentation](https://raspberrypi.github.io/pico-sdk-doxygen/)
- [RP2040 Datasheet](https://datasheets.raspberrypi.com/rp2040/rp2040-datasheet.pdf)

### Para SDR:
- [GNU Radio](https://www.gnuradio.org/)
- [HackRF Project](https://greatscottgadgets.com/hackrf/)
- [SDR#](https://sdrsharp.com/)

### Regulaciones:
- [FCC Part 15](https://www.ecfr.gov/current/title-47/chapter-I/subchapter-A/part-15)
- [ETSI Standards](https://www.etsi.org/)
- [ITU Radio Regulations](https://www.itu.int/en/ITU-R/Pages/default.aspx)

---

## Conclusión

**Para su objetivo de interferencia Bluetooth:**

1. **Mejor opción práctica:** RP2040 + nrf24L01+ (mismo hardware que el sketch original)
2. **Opción profesional:** SDR (HackRF, LimeSDR)
3. **No viable:** RP2040W usando WiFi directamente

El RP2040W no es adecuado para esta tarea debido a las limitaciones del chip CYW43439 y la falta de APIs de bajo nivel para control de radio.
