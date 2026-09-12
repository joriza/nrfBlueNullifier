# 🛜 nrfBlueNullifier

<img width="1024" height="1024" alt="nrfBlueNullifier" src="https://github.com/user-attachments/assets/5fa6548b-a4e1-41a5-904e-4f024bfe2050" />

Herramienta que interfiere señales de **Bluetooth Clásico** utilizando uno o dos módulos **nrf24L01+ (PA/LNA)** conectados a una placa **NodeMCU ESP-32S (ESP32)**.

> [!WARNING]
> **Advertencia legal y ética:** interferir comunicaciones por radiofrecuencia está regulado y es ilegal en la mayoría de las jurisdicciones (FCC, ETSI, ENACOM, COFETEL, etc.).
> Utilizá este código únicamente con fines **educativos o de investigación**, en **laboratorios aislados** (idealmente con jaula de Faraday) y nunca contra redes o dispositivos de terceros.

## ✨ Características principales

- Configuración mínima: un ESP32 y uno o dos módulos nrf24L01+.
- Transmisión de **portadora continua** (`startConstCarrier`) con barrido de los 79 canales de Bluetooth Clásico.
- Variantes de cableado **VSPI** y **HSPI**, y variante de **doble módulo** (ambos SPI en paralelo).
- Instalación por **Arduino IDE** o por **ESP Web Flasher** (imágenes `.bin` precompiladas incluidas).

## 🔗 Variantes

| Variante | Módulos nrf24L01+ | Código |
| ---------- | ------------------- | -------- |
| 1 módulo por VSPI | 1 | [`nrfBlueNullifier-VSPI-nrf24L01`](nrfBlueNullifier-1-nrf24L01/nrfBlueNullifier-VSPI-nrf24L01) |
| 1 módulo por HSPI | 1 | [`nrfBlueNullifier-HSPI-nrf24L01`](nrfBlueNullifier-1-nrf24L01/nrfBlueNullifier-HSPI-nrf24L01) |
| 2 módulos (VSPI + HSPI) | 2 | [`nrfBlueNullifier-2-nrf24L01`](nrfBlueNullifier-2-nrf24L01) |

Otros detalles (requisitos, tabla de pines, compilación, flasheo y ajustes) están en el README de cada variante.

## 🔌 Pinout (ESP32 ↔ nrf24L01+)

| Señal nrf24L01 | VSPI   | HSPI   |
| -------------- | ------ | ------ |
| VCC            | 3.3V   | 3.3V   |
| GND            | GND    | GND    |
| CE             | GPIO22 | GPIO16 |
| CSN            | GPIO21 | GPIO15 |
| SCK            | GPIO18 | GPIO14 |
| MOSI           | GPIO23 | GPIO13 |
| MISO           | GPIO19 | GPIO12 |

> [!NOTE]
> Los módulos nrf24L01 y nrf24L01+PA/LNA comparten el mismo pinout y se alimentan a **3.3V**.

## 🚀 Instalación (resumen)

1. **Arduino IDE:** instalá el soporte de ESP32 y la biblioteca `RF24` (TMRh20), abrí el `.ino` de la variante elegida, compilá y subí el código.
2. **ESP Web Flasher:** flasheá con el [Adafruit ESP WebSerial ESPTool](https://adafruit.github.io/Adafruit_WebSerial_ESPTool/) los tres archivos `.bin` de la variante:
   - `bootloader.bin` → offset `0x1000`
   - `partitions.bin` → offset `0x8000`
   - `nrfBlueNullifier-*.bin` → offset `0x10000`

Las instrucciones completas paso a paso están en el README de cada variante.

## 📡 Variante experimental RP2040W

La carpeta [`rp2040w-wifi-interferencia`](rp2040w-wifi-interferencia) contiene sketches para **Raspberry Pi Pico W (RP2040W)** que intentan generar congestión WiFi en la banda de 2.4 GHz.

**Limitación clave:** el chip CYW43439 del Pico W tiene firmware cerrado y **no permite transmisión de portadora continua**, por lo que **no funciona como jammer real**; solo genera congestión WiFi estándar con efecto muy limitado sobre Bluetooth.

Más detalles en:

- [`rp2040w-wifi-interferencia/README.md`](rp2040w-wifi-interferencia/README.md)
- [`RP2040W-Limitaciones-y-Alternativas.md`](RP2040W-Limitaciones-y-Alternativas.md)

## 🗂️ Estructura del repositorio

```text
nrfBlueNullifier/
├── nrfBlueNullifier-1-nrf24L01/          # Variante con 1 módulo
│   ├── nrfBlueNullifier-VSPI-nrf24L01/   # Código + .bin por VSPI
│   └── nrfBlueNullifier-HSPI-nrf24L01/   # Código + .bin por HSPI
├── nrfBlueNullifier-2-nrf24L01/          # Variante con 2 módulos (VSPI + HSPI)
├── rp2040w-wifi-interferencia/           # Experimento RP2040W (congestión WiFi)
├── RP2040W-Limitaciones-y-Alternativas.md
└── LICENSE
```

## 🤔 ¿Qué ocurre cuando el script se ejecuta?

- Corta el audio de dispositivos con Bluetooth Clásico, incluso mientras reproducen música.
- A veces desconecta el Bluetooth del dispositivo.
- También degrada la velocidad del WiFi 2.4 GHz y puede llegar a desconectarlo.

> [!TIP]
> Para concentrar el efecto en Bluetooth Clásico y minimizar el impacto sobre WiFi, reemplazá la lista de canales en el código:
>
> ```cpp
> byte hopping_channel[] = {32, 34, 46, 48, 50, 52, 26, 28, 30, 74, 76, 78};
> ```

## 📄 Licencia

Este proyecto se distribuye bajo la licencia [GPL-3.0](LICENSE). Basado en el proyecto original de **WireBits**.
