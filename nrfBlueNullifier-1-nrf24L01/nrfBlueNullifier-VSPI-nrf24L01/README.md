# 🛜 nrfBlueNullifier

Herramienta que interfiere señales de Bluetooth Clásico usando 1 módulo nrf24L01+PA/LNA conectado por VSPI.

# 📦 Requisitos

- NodeMCU ESP-32S de 38 pines
- Un módulo nrf24L01 O módulo nrf24L01+PA/LNA
- `7` cables jumper hembra a hembra

# 🗒️ Nota

- Los módulos nrf24L01 y nrf24L01+PA/LNA tienen el mismo pinout.

# 🔌 Tabla de pines

| NODEMCU ESP-32S | nrf24l01 |
| --------------- | -------- |
| 3.3V            | VCC      |
| GND             | GND      |
| GPIO22          | CE       |
| GPIO21          | CSN      |
| GPIO18          | SCK      |
| GPIO23          | MOSI     |
| GPIO19          | MISO     |

# 🖼️ Imágenes

![NodeMCU ESP-32S](https://github.com/user-attachments/assets/b790a39f-dae6-4087-a740-148f6b272aa4)
![nrf24L01](https://github.com/user-attachments/assets/706db436-8ce3-431d-8b0e-51e8936e28ff)

# ⚙️ Configuración

1. Descargá el Arduino IDE desde [aquí](https://www.arduino.cc/en/software) según tu sistema operativo.
2. Instalalo.
3. Andá a `File` → `Preferences` → `Additional Boards Manager URLs`.
4. Pegá el siguiente enlace:

   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```

5. Hacé clic en `OK`.
6. Andá a `Tools` → `Board` → `Board Manager`.
7. Esperá unos momentos y buscá `esp32` de `Espressif Systems`.
8. Instalalo.
9. Esperá unos momentos hasta que quede instalado.
10. Andá a `Sketch` → `Include Library` → `Manage Libraries`.
11. Esperá unos momentos y buscá `rf24` de `TMRh20, Avamander`.
12. Instalala.
13. Esperá unos momentos hasta que quede instalada.
14. Reiniciá el Arduino IDE cerrándolo y volviéndolo a abrir.
15. ¡Listo!

# 🔧 Instalación

1. Descargá o cloná el repositorio.
2. Abrí la carpeta y hacé doble clic en el archivo `nrfBlueNullifier.ino`.
3. Se abre en el Arduino IDE.
4. Compilá el código.
5. Seleccioná la placa correcta en `Tools` → `Board` → `ESP32 Arduino`.
   - Generalmente es `NodeMCU-32S`.
6. Seleccioná el número de puerto correcto de esa placa.
7. Subí el código.
8. Cuando aparezca `Connecting.....`, mantené presionado el botón `BOOT`.
9. Cuando aparezca `Writing at`, soltá el botón `BOOT`.
10. ¡Listo!
    - El script empieza a ejecutarse automáticamente.

# 🔧 Instalación con ESP Web Flasher

1. Abrí el Adafruit ESP Web Flasher desde [aquí](https://adafruit.github.io/Adafruit_WebSerial_ESPTool/).
2. Configurá el Baud Rate en `115200 Baud`.
3. Conectá el `ESP32` con un cable USB y este a la PC.
4. Mantené presionado el botón `BOOT`.
5. Hacé clic en el botón `Connect`.
6. Seleccioná el puerto COM de tu dispositivo en la ventana emergente.
7. Soltá el botón `BOOT`.
   - Si la conexión es exitosa, se muestra esto <img width="386" height="217" alt="Adafruit ESP Web Flasher" src="https://github.com/user-attachments/assets/47b8888e-25d2-4a29-ab6d-25055cf3a033" />
8. Hacé clic en el botón `Erase`.
   - Esperá unos momentos hasta que se borre correctamente.
9. Descargá los `3` archivos de este directorio.
    - Los archivos son:
      1. `nrfBlueNullifier-VSPI-nrf24L01-bootloader.bin`
      2. `nrfBlueNullifier-VSPI-nrf24L01-partitions.bin`
      3. `nrfBlueNullifier-VSPI-nrf24L01.bin`
10. Seleccioná el archivo `nrfBlueNullifier-VSPI-nrf24L01-bootloader.bin` con el offset `0x1000`.
11. Seleccioná el archivo `nrfBlueNullifier-VSPI-nrf24L01-partitions.bin` con el offset `0x8000`.
12. Seleccioná el archivo `nrfBlueNullifier-VSPI-nrf24L01.bin` con el offset `0x10000`.
13. Hacé clic en el botón `Program`.
    - Esperá unos momentos hasta que se grabe correctamente.
14. Presioná y soltá el botón `BOOT`.
15. Desconectá y volvé a conectar el `ESP32` a la PC.
16. ¡Listo! `nrfBlueNullifier-VSPI-nrf24L01` está listo.

# 🤔 ¿Qué ocurre cuando el script se ejecuta?

- Corta el sonido.
- Bloquea el sonido incluso cuando el dispositivo está reproduciendo música.
- A veces desconecta el Bluetooth del dispositivo.

# 🛠️ Modificación en el código

- El código afecta al Bluetooth Clásico y al WiFi.
- Si querés concentrar el efecto en el Bluetooth Clásico, modificá el código reemplazando la línea 14 por lo siguiente:

```
byte hopping_channel[] = {32, 34, 46, 48, 50, 52, 26, 28, 30, 74, 76, 78};
```

- Guardá el código.
- Compilá el código y subilo.

# 🤔 ¿Qué ocurre cuando el script modificado se ejecuta?

- Bloquea el sonido incluso cuando el dispositivo está reproduciendo música.
- Reduce la velocidad del WiFi.
