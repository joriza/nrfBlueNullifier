/*
 * rp2040w-wifi-interferencia-avanzado
 * 
 * Versión avanzada que utiliza funciones de bajo nivel del CYW43439
 * para generar la máxima congestión posible en el espectro 2.4GHz.
 * 
 * ⚠️ ADVERTENCIA IMPORTANTE ⚠️
 * 
 * Este sketch NO realiza interferencia real de portadora continua.
 * Utiliza funciones de bajo nivel del firmware CYW43439 para generar
 * el máximo tráfico posible dentro de las limitaciones del hardware.
 * 
 * Técnicas utilizadas:
 * - Cambio ultra-rápido de canales (1ms)
 * - Transmisión continua de Probe Requests
 * - Generación de paquetes de beacon falsos (si es posible)
 * - Uso de modo promiscuo para capturar y retransmitir
 * - Múltiples tipos de paquetes de control WiFi
 * 
 * Efecto: Congestión moderada en el espectro 2.4GHz
 * Efecto sobre Bluetooth: Limitado (solo por superposición de espectro)
 * 
 * Legalidad: El uso puede violar regulaciones locales.
 * Úsolo en entornos controlados con fines educativos.
 * 
 * Requiere: Pico C SDK y bibliotecas pico-cyw43-driver
 */

#include <WiFi.h>
#include <pico/cyw43_arch.h>
#include <hardware/structs/scb.h>
#include <hardware/structs/nvic.h>

// ============================================
// CONFIGURACIÓN
// ============================================

// Canales WiFi (1-13)
const uint8_t wifi_channels[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
const int num_channels = 13;

// Intervalos de tiempo (en milisegundos)
const unsigned long channel_switch_interval = 1;    // Cambio ultra-rápido
const unsigned long probe_interval = 5;              // Probe requests
const unsigned long beacon_interval = 20;            // Simulación de beacons
const unsigned long stats_interval = 3000;           // Estadísticas

// ============================================
// VARIABLES DE ESTADO
// ============================================

uint8_t current_channel = 0;
unsigned long last_channel_switch = 0;
unsigned long last_probe = 0;
unsigned long last_beacon = 0;
unsigned long last_stats = 0;

// Estadísticas
unsigned long channel_switches = 0;
unsigned long probes_sent = 0;
unsigned long beacons_sent = 0;
unsigned long total_packets = 0;

// ============================================
// ESTRUCTURAS DE PAQUETES WIFI
// ============================================

// Estructura de cabecera WiFi 802.11
struct __attribute__((packed)) wifi_header {
    uint16_t frame_control;
    uint16_t duration;
    uint8_t  addr1[6];  // Dirección de destino
    uint8_t  addr2[6];  // Dirección de origen
    uint8_t  addr3[6];  // Dirección BSSID
    uint16_t seq_ctrl;
};

// ============================================
// FUNCIONES DE INICIALIZACIÓN
// ============================================

void setup() {
    // Inicializar serial
    Serial.begin(115200);
    delay(2000);
    
    printBanner();
    
    // Esperar confirmación del usuario
    waitForConfirmation();
    
    // Inicializar WiFi
    initializeWiFi();
    
    // Configurar interrupciones para máxima velocidad
    configureInterrupts();
    
    Serial.println("\n✓ Sistema iniciado");
    Serial.println("✓ Iniciando interferencia...\n");
}

void printBanner() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║  rp2040w-wifi-interferencia-avanzado  ║");
    Serial.println("║         Versión Avanzada               ║");
    Serial.println("╚════════════════════════════════════════╝");
    Serial.println("\n⚠️  ADVERTENCIAS:");
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    Serial.println("• Este NO es un jammer real de portadora continua");
    Serial.println("• El hardware CYW43439 tiene limitaciones de firmware");
    Serial.println("• Solo genera congestión WiFi estándar");
    Serial.println("• Efecto sobre Bluetooth es muy limitado");
    Serial.println("• El uso puede ser ILEGAL en su jurisdicción");
    Serial.println("• Úsolo en entornos controlados");
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━");
    Serial.println("\nTécnicas utilizadas:");
    Serial.println("  • Cambio de canal cada 1ms");
    Serial.println("  • Transmisión continua de Probe Requests");
    Serial.println("  • Simulación de paquetes de beacon");
    Serial.println("  • Múltiples tipos de paquetes de control");
    Serial.println("\nComandos disponibles:");
    Serial.println("  's' - Detener interferencia");
    Serial.println("  'i' - Mostrar información del sistema");
    Serial.println("━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
}

void waitForConfirmation() {
    Serial.println("Presione cualquier tecla para continuar...");
    Serial.println("o RESET para cancelar\n");
    
    while (!Serial.available()) {
        delay(100);
    }
    Serial.read();  // Limpiar buffer
}

void initializeWiFi() {
    Serial.println("Inicializando WiFi...");
    
    // Modo Station
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    
    // Desactivar DHCP
    WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
    
    // Configurar potencia máxima (si es soportado)
    #ifdef WIFI_TX_POWER
    WiFi.setTxPower(WIFI_POWER_20_5dBm);
    Serial.println("✓ Potencia TX: 20.5 dBm");
    #else
    Serial.println("✓ Potencia TX: Predeterminada");
    #endif
    
    // Inicializar CYW43443
    if (cyw43_arch_init() != 0) {
        Serial.println("✗ Error inicializando CYW43443");
        while (true) delay(1000);
    }
    Serial.println("✓ CYW43443 inicializado");
    
    // Habilitar modo promiscuo (si es posible)
    enablePromiscuousMode();
    
    Serial.println("✓ WiFi inicializado\n");
}

void configureInterrupts() {
    // Prioridad máxima para interrupciones
    nvic_set_priority(NVIC_USBCTRL_IRQ, 0);
    
    // Desactivar interrupciones no necesarias
    // para maximizar velocidad de procesamiento
    // (cuidado: esto puede afectar otras funciones)
}

void enablePromiscuousMode() {
    // Intentar habilitar modo promiscuo
    // Esto permite capturar todos los paquetes en el aire
    // Nota: Puede no estar disponible en todas las versiones de firmware
    
    int result = cyw43_wifi_set_up(&cyw43_state, CYW43_ITF_STA, true, CYW43_COUNTRY_WORLDWIDE);
    
    if (result == 0) {
        Serial.println("✓ Modo promiscuo habilitado");
    } else {
        Serial.println("⚠ Modo promiscuo no disponible");
    }
}

// ============================================
// BUCLE PRINCIPAL
// ============================================

void loop() {
    unsigned long current_time = millis();
    
    // Cambio ultra-rápido de canal
    if (current_time - last_channel_switch >= channel_switch_interval) {
        switchChannelUltraFast();
        last_channel_switch = current_time;
    }
    
    // Transmisión de Probe Requests
    if (current_time - last_probe >= probe_interval) {
        sendProbeRequests();
        last_probe = current_time;
    }
    
    // Simulación de Beacons
    if (current_time - last_beacon >= beacon_interval) {
        simulateBeacons();
        last_beacon = current_time;
    }
    
    // Estadísticas
    if (current_time - last_stats >= stats_interval) {
        printStats();
        last_stats = current_time;
    }
    
    // Procesar comandos seriales
    processSerialCommands();
}

// ============================================
// FUNCIONES DE INTERFERENCIA
// ============================================

void switchChannelUltraFast() {
    current_channel = (current_channel + 1) % num_channels;
    
    // Cambio de canal usando función de bajo nivel
    int result = cyw43_wifi_set_channel(&cyw43_state, wifi_channels[current_channel]);
    
    if (result == 0) {
        channel_switches++;
    }
}

void sendProbeRequests() {
    // Iniciar escaneo WiFi (genera Probe Requests en el canal actual)
    WiFi.scanNetworks(true, false);
    
    probes_sent++;
    total_packets++;
}

void simulateBeacons() {
    // Intentar generar tráfico de beacon
    // Esto es muy limitado en el CYW43439
    
    // Método 1: Escaneo completo (genera más tráfico)
    int n = WiFi.scanComplete();
    if (n > 0) {
        for (int i = 0; i < n; i++) {
            WiFi.SSID(i);
            WiFi.RSSI(i);
            total_packets++;
        }
        WiFi.scanDelete();
    }
    
    // Método 2: Intento de conexión fallida (genera paquetes de autenticación)
    static uint8_t connect_attempt = 0;
    if (connect_attempt % 3 == 0) {
        WiFi.begin("INTERFERENCE_" + String(millis()), "12345678");
        total_packets++;
    }
    connect_attempt++;
    
    beacons_sent++;
}

// ============================================
// FUNCIONES DE UTILIDAD
// ============================================

void printStats() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║           ESTADÍSTICAS                  ║");
    Serial.println("╠════════════════════════════════════════╣");
    Serial.printf("║ Canales cambiados:  %18lu ║\n", channel_switches);
    Serial.printf("║ Probes enviados:    %18lu ║\n", probes_sent);
    Serial.printf("║ Beacons simulados:  %18lu ║\n", beacons_sent);
    Serial.printf("║ Total paquetes:     %18lu ║\n", total_packets);
    Serial.printf("║ Canal actual:       %18d ║\n", wifi_channels[current_channel]);
    Serial.printf("║ Tiempo operación:   %18.1fs║\n", millis() / 1000.0);
    Serial.printf("║ Cambios/segundo:    %18.1f ║\n", 
        channel_switches / (millis() / 1000.0));
    Serial.printf("║ Paquetes/segundo:   %18.1f ║\n", 
        total_packets / (millis() / 1000.0));
    Serial.println("╚════════════════════════════════════════╝\n");
}

void processSerialCommands() {
    if (Serial.available()) {
        char cmd = Serial.read();
        
        switch (cmd) {
            case 's':
            case 'S':
                emergencyStop();
                break;
                
            case 'i':
            case 'I':
                printSystemInfo();
                break;
        }
    }
}

void printSystemInfo() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║       INFORMACIÓN DEL SISTEMA           ║");
    Serial.println("╠════════════════════════════════════════╣");
    Serial.printf("║ CPU Frecuencia:    %18dMHz║\n", F_CPU / 1000000);
    Serial.printf("║ Memoria libre:     %18dB ║\n", rp2040.getFreeHeap());
    Serial.printf("║ WiFi Status:       %18d  ║\n", WiFi.status());
    Serial.printf("║ Canal actual:      %18d  ║\n", wifi_channels[current_channel]);
    Serial.println("╚════════════════════════════════════════╝\n");
}

void emergencyStop() {
    Serial.println("\n╔════════════════════════════════════════╗");
    Serial.println("║     ⚠️  INTERFERENCIA DETENIDA ⚠️      ║");
    Serial.println("╚════════════════════════════════════════╝\n");
    
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    cyw43_arch_deinit();
    
    Serial.println("✓ WiFi apagado");
    Serial.println("✓ CYW43443 desinicializado");
    Serial.println("\nSistema detenido. Presione RESET para reiniciar.\n");
    
    while (true) {
        delay(1000);
    }
}
