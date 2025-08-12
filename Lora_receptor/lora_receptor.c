/* EMBARCATECH - Receptor LoRa para Estação Meteorológica
 * Recebe temperatura, pressão, umidade e altitude via LoRa
 * Adaptado para Raspberry Pi Pico W
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "lora.h"

// Definições de pinos para LoRa (SX1276)
#define PIN_MISO 16
#define PIN_MOSI 19
#define PIN_CS 17
#define PIN_SCK 18
#define PIN_RST 20
#define PIN_DIO0 8
#define SPI_PORT spi0

// Definição do LED
#define PIN_LED 25 // LED onboard do Pico W

// Variáveis globais - configuração LoRa
typedef enum {lmIdle, lmListening, lmSending} tLoRaMode;
static tLoRaMode LoRaMode;
static int SendingRTTY = 0;
static int InRTTYMode = 0;
static int ImplicitOrExplicit;
static char PayloadID[32];
static char Callsign[] = "RICK";

// Configurações LoRa (devem corresponder ao transmissor)
int payload_len = 40;
int preamble_len = 8;
int sf = 7;
int crc = 0;
int ih = 0;
int bw = 125000;
int cr = 1;
int de = 0;

// Protótipos de funções LoRa
void setLora(float Frequency, int Mode);
static inline void cs_select();
static inline void cs_deselect();
static void writeRegister(uint8_t reg, uint8_t data);
static uint8_t readRegister(uint8_t addr);
void SetDeviceMode(uint8_t newMode);
void SetModemToLoRaMode();
void SetFrequency(double Frequency);
void ReceiveLora();
void parseSensorData(const char* data);

// Função principal
int main() {
    stdio_init_all();
    sleep_ms(500);
    printf("Iniciando Receptor LoRa (Pico W)\n");

    // Inicializar SPI
    spi_init(SPI_PORT, 500*1000);
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_function(PIN_SCK, GPIO_FUNC_SPI);
    gpio_init(PIN_CS);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    gpio_init(PIN_RST);
    gpio_set_dir(PIN_RST, GPIO_OUT);
    gpio_put(PIN_RST, 1);
    gpio_init(PIN_DIO0);
    gpio_set_dir(PIN_DIO0, GPIO_IN);

    // Inicializar LED onboard
    gpio_init(PIN_LED);
    gpio_set_dir(PIN_LED, GPIO_OUT);

    // Configurar LoRa
    setLora(915.000, 1); // Mesma frequência e modo do transmissor
    strcpy(PayloadID, Callsign);
    printf("Rádio LoRa inicializado\n");
    printf("Aguardando dados...\n");

    while (true) {
        ReceiveLora();
        sleep_ms(100);
    }
}

// Função para receber dados LoRa
void ReceiveLora() {
    static uint8_t buf[256];
    static uint8_t length = 0;
    
    // Verificar se há dados disponíveis
    if (gpio_get(PIN_DIO0)) {
        // Piscar LED ao receber dados
        gpio_put(PIN_LED, 1);
        
        // Ler o pacote recebido
        SetDeviceMode(RF95_MODE_STANDBY);
        writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_CURRENT_ADDR));
        
        if (ImplicitOrExplicit == EXPLICIT_MODE) {
            length = readRegister(REG_RX_NB_BYTES);
        } else {
            length = payload_len;
        }
        
        // Ler os dados da FIFO
        for (int i = 0; i < length; i++) {
            buf[i] = readRegister(REG_FIFO);
        }
        
        // Processar os dados recebidos
        buf[length] = '\0'; // Adicionar terminador de string
        printf("Dados recebidos: %s\n", buf);
        
        // Parse dos dados dos sensores
        parseSensorData((const char*)buf);
        
        // Limpar flags de interrupção
        writeRegister(REG_IRQ_FLAGS, 0xFF);
        SetDeviceMode(RF95_MODE_RX_CONTINUOUS);
        
        gpio_put(PIN_LED, 0);
    }
}

// Função para parsear os dados dos sensores
void parseSensorData(const char* data) {
    float temp, press, umid, alt;
    
    if (sscanf(data, "%f;%f;%f;%f", &temp, &press, &umid, &alt) == 4) {
        printf("--- Dados dos Sensores ---\n");
        printf("Temperatura: %.1f °C\n", temp);
        printf("Pressão: %.1f kPa\n", press);
        printf("Umidade: %.1f %%\n", umid);
        printf("Altitude: %.1f m\n", alt);
        printf("--------------------------\n");
    } else {
        printf("Erro ao parsear dados!\n");
    }
}

// Funções LoRa (mantidas iguais ao transmissor)
static inline void cs_select() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 0);
    asm volatile("nop \n nop \n nop");
}
static inline void cs_deselect() {
    asm volatile("nop \n nop \n nop");
    gpio_put(PIN_CS, 1);
    asm volatile("nop \n nop \n nop");
}
static void writeRegister(uint8_t reg, uint8_t data) {
    uint8_t buf[2];
    buf[0] = reg | 0x80;
    buf[1] = data;
    cs_select();
    spi_write_blocking(SPI_PORT, buf, 2);
    cs_deselect();
    sleep_ms(1);
}
static uint8_t readRegister(uint8_t addr) {
    uint8_t buf[1];
    addr &= 0x7F;
    cs_select();
    spi_write_blocking(SPI_PORT, &addr, 1);
    sleep_ms(1);
    spi_read_blocking(SPI_PORT, 0, buf, 1);
    cs_deselect();
    sleep_ms(1);
    return buf[0];
}
void SetDeviceMode(uint8_t newMode) {
    static uint8_t currentMode = 0xFF;
    if (newMode == currentMode) return;
    switch (newMode) {
        case RF95_MODE_TX:
            writeRegister(REG_LNA, LNA_OFF_GAIN);
            writeRegister(REG_PA_CONFIG, PA_MAX_UK);
            writeRegister(REG_OPMODE, newMode);
            currentMode = newMode;
            break;
        case RF95_MODE_RX_CONTINUOUS:
            writeRegister(REG_PA_CONFIG, PA_OFF_BOOST);
            writeRegister(REG_LNA, LNA_MAX_GAIN);
            writeRegister(REG_OPMODE, newMode);
            currentMode = newMode;
            break;
        case RF95_MODE_SLEEP:
        case RF95_MODE_STANDBY:
            writeRegister(REG_OPMODE, newMode);
            currentMode = newMode;
            break;
        default: return;
    }
    if (newMode != RF95_MODE_SLEEP) sleep_ms(1);
}
void SetModemToLoRaMode() {
    SetDeviceMode(RF95_MODE_SLEEP);
    writeRegister(REG_OPMODE, 0x80);
}
void SetFrequency(double Frequency) {
    unsigned long FrequencyValue;
    Frequency = Frequency * 7110656 / 434;
    FrequencyValue = (unsigned long)(Frequency);
    writeRegister(REG_FRF_MSB, (FrequencyValue >> 16) & 0xFF);
    writeRegister(REG_FRF_MID, (FrequencyValue >> 8) & 0xFF);
    writeRegister(REG_FRF_LSB, FrequencyValue & 0xFF);
}
void setLora(float Frequency, int Mode) {
    int ErrorCoding, Bandwidth, SpreadingFactor, LowDataRateOptimize, PayloadLength;
    SetModemToLoRaMode();
    SetFrequency(Frequency);
    if (Mode == 1) {
        ImplicitOrExplicit = EXPLICIT_MODE;
        ErrorCoding = ERROR_CODING_4_5;
        Bandwidth = BANDWIDTH_125K;
        SpreadingFactor = SPREADING_7;
        LowDataRateOptimize = 0;
    } else {
        ImplicitOrExplicit = IMPLICIT_MODE;
        ErrorCoding = ERROR_CODING_4_8;
        Bandwidth = BANDWIDTH_20K8;
        SpreadingFactor = SPREADING_11;
        LowDataRateOptimize = 0x08;
    }
    PayloadLength = ImplicitOrExplicit == IMPLICIT_MODE ? 255 : 0;
    writeRegister(REG_MODEM_CONFIG, ImplicitOrExplicit | ErrorCoding | Bandwidth);
    writeRegister(REG_MODEM_CONFIG2, SpreadingFactor | CRC_ON);
    writeRegister(REG_MODEM_CONFIG3, 0x04 | LowDataRateOptimize);
    writeRegister(REG_DETECT_OPT, (readRegister(REG_DETECT_OPT) & 0xF8) | ((SpreadingFactor == SPREADING_6) ? 0x05 : 0x03));
    writeRegister(REG_DETECTION_THRESHOLD, (SpreadingFactor == SPREADING_6) ? 0x0C : 0x0A);
    writeRegister(REG_PAYLOAD_LENGTH, PayloadLength);
    writeRegister(REG_RX_NB_BYTES, PayloadLength);
    writeRegister(REG_DIO_MAPPING_1, 0x00); // DIO0 para RxDone
    writeRegister(REG_DIO_MAPPING_2, 0x00);
    SetDeviceMode(RF95_MODE_RX_CONTINUOUS);
}