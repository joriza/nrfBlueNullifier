/*
 * rp2040w-wifi-interferencia
 * 
 * ⚠️ ADVERTENCIA IMPORTANTE ⚠️
 * 
 * Este sketch NO realiza interferencia real de portadora continua.
 * El RP2040W (chip CYW43439) tiene limitaciones de hardware que
 * impiden la transmisión de portadora continua o control directo
 * del hardware de radio.
 * 
 * Este sketch utiliza comandos WiFi estándar para generar la
 * MÁXIMA congestión posible en el espectro 2.4GHz mediante:
 * - Cambio rápido de canales
 * - Transmisión de paquetes de escaneo
 * - Uso de máximo ancho de banda disponible
 * 
 * Efecto: Puede causar congestión leve en redes WiFi cercanas
 * Efecto sobre Bluetooth: Muy limitado (solo por superposición de espectro)
 * 
 * Legalidad: El uso de este código puede violar regulaciones locales
 * de espectro radioeléctrico. Úsolo solo en entornos controlados
 * con fines educativos.
 * 
 * Autor: Adaptación para RP2040W
 * Basado en: nrfBlueNullifier de WireBits
 */

#include <WiFi.h>
#include <pico/cyw43_arch.h>

// Configuración de canales WiFi (1-13, 14 no disponible en muchas regiones)
// Cada canal WiFi cubre ~4 canales Bluetooth
const uint8_t wifi_channels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
const int num_channels = 13;

// Variables de estado
uint8_t current_channel = 0;
unsigned long last_channel_switch = 0;
unsigned long last_scan = 0;
const unsigned long channel_switch_interval = 10;  // ms entre cambios de canal
const unsigned long scan_interval = 50;            // ms entre escaneos

// Estadísticas
unsigned long packets_sent = 0;
unsigned long channel_switches = 0;
unsigned long scans_performed = 0;

// Configuración de potencia (máxima disponible)
const int8_t tx_power = 20;  // dBm (si es soportado)

void setup() {
    // Inicializar serial
    Serial.begin(115200);
    delay(2000);
    
    Serial.println("\n========================================");
    Serial.println("  rp2040w-wifi-interferencia");
    Serial.println("========================================");
    Serial.println("\n⚠️  ADVERTENCIAS:");
    Serial.println("- Este NO es un jammer real de portadora continua");
    Serial.println("- El hardware CYW43439 tiene limitaciones de firmware");
    Serial.println("- Solo genera congestión WiFi estándar");
    Serial.println("- Efecto sobre Bluetooth es muy limitado");
    Serial.println("- El uso puede ser ilegal en su jurisdicción");
    Serial.println("\nPresione cualquier tecla para continuar o RESET para cancelar");
    Serial.println("========================================\n");
    
    // Esperar confirmación del usuario
    while (!Serial.available()) {
        delay(100);
    }
    Serial.read();  // Limpiar buffer
    
    // Inicializar WiFi en modo Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    // Intentar configurar potencia máxima (si es soportado)
    #ifdef WIFI_TX_POWER
    WiFi.setTxPower(WIFI_POWER_20_5dBm);
    Serial.println("Potencia TX: 20.5 dBm (máxima)");
    #else
    Serial.println("Potencia TX: Predeterminada");
    #endif
    
    // Desactivar DHCP para evitar conexiones no deseadas
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    
    Serial.println("\nIniciando interferencia WiFi...");
    Serial.println("Canales WiFi: 1-13");
    Serial.println("Cada canal WiFi cubre ~4 canales Bluetooth");
    Serial.println("========================================\n");
    
    delay(1000);
}

void loop() {
    unsigned long current_time = millis();
    
    // Cambiar de canal WiFi rápidamente
    if (current_time - last_channel_switch >= channel_switch_interval) {
        switchWifiChannel();
        last_channel_switch = current_time;
    }
    
    // Realizar escaneo activo para generar tráfico
    if (current_time - last_scan >= scan_interval) {
        performActiveScan();
        last_scan = current_time;
    }
    
    // Transmitir paquetes de beacon simulados (si es posible)
    transmitBeaconPackets();
    
    // Imprimir estadísticas cada 5 segundos
    static unsigned long last_stats = 0;
    if (current_time - last_stats >= 5000) {
        printStats();
        last_stats = current_time;
    }
}

/**
 * Cambia el canal WiFi actual
 */
void switchWifiChannel() {
    current_channel = (current_channel + 1) % num_channels;
    
    // Usar la función de bajo nivel del CYW43439 para cambiar de canal
    // Esto genera tráfico de control en el nuevo canal
    int result = cyw43_wifi_set_channel(&cyw43_state, wifi_channels[current_channel]);
    
    if (result == 0) {
        channel_switches++;
        
        // Debug: mostrar canal actual cada 50 cambios
        if (channel_switches % 50 == 0) {
            Serial.printf("Canal WiFi: %d (Bluetooth: %d-%d)\n", 
                wifi_channels[current_channel],
                (wifi_channels[current_channel] - 1) * 4,
                (wifi_channels[current_channel] * 4) - 1);
        }
    }
}

/**
 * Realiza escaneo activo de redes
 * Esto genera paquetes de Probe Request en el canal actual
 */
void performActiveScan() {
    // Iniciar escaneo WiFi (genera Probe Requests)
    WiFi.scanNetworks(true, false);  // Async, sin ocultar SSIDs
    
    // El escaneo asíncrono genera tráfico sin bloquear
    scans_performed++;
}

/**
 * Intenta transmitir paquetes de beacon o datos
 * Nota: Esto es muy limitado en el CYW43439
 */
void transmitBeaconPackets() {
    // El CYW43439 no permite transmisión de paquetes raw
    // Intentamos generar tráfico mediante:
    
    // 1. Verificar si hay resultados de escaneo y procesarlos
    int n = WiFi.scanComplete();
    if (n > 0) {
        // Procesar resultados (genera tráfico de lectura)
        for (int i = 0; i < n; i++) {
            WiFi.SSID(i);
            WiFi.RSSI(i);
            packets_sent++;
        }
        WiFi.scanDelete();
    }
    
    // 2. Intentar conectar a una red inexistente (genera paquetes de autenticación)
    // Esto se hace muy raramente para no saturar con paquetes de conexión fallida
    static unsigned long last_connect_attempt = 0;
    if (millis() - last_connect_attempt > 1000) {
        WiFi.begin("FAKE_NETWORK_" + String(millis()), "password123");
        last_connect_attempt = millis();
        packets_sent++;
    }
}

/**
 * Imprime estadísticas de operación
 */
void printStats() {
    Serial.println("\n========== ESTADÍSTICAS ==========");
    Serial.printf("Canales cambiados:  %lu\n", channel_switches);
    Serial.printf("Escaneos realizados: %lu\n", scans_performed);
    Serial.printf("Paquetes generados:  %lu\n", packets_sent);
    Serial.printf("Canal actual:        %d\n", wifi_channels[current_channel]);
    Serial.printf("Tiempo de operación:  %.1f segundos\n", millis() / 1000.0);
    Serial.printf("Canales/segundo:     %.1f\n", channel_switches / (millis() / 1000.0));
    Serial.println("==================================\n");
}

/**
 * Función de emergencia: detener interferencia
 */
void emergencyStop() {
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    Serial.println("\n⚠️  INTERFERENCIA DETENIDA ⚠️\n");
    while (true) {
        delay(1000);
    }
}

// Detectar comando de parada por serial
void serialEvent() {
    if (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == 's' || cmd == 'S') {
            emergencyStop();
        }
    }
}
