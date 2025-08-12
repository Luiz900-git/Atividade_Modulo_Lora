# lora_receptor.md

## Descrição Geral

Este projeto implementa um receptor LoRa para uma estação meteorológica utilizando o Raspberry Pi Pico W. O código recebe dados de sensores (temperatura, pressão, umidade e altitude) transmitidos via rádio LoRa, processa e exibe essas informações no console.

---

## Funcionamento

### 1. Inicialização

- **SPI e GPIO:** Inicializa a comunicação SPI e configura os pinos necessários para o módulo LoRa SX1276.
- **LED:** Configura o LED onboard do Pico W para indicar a recepção de dados.
- **LoRa:** Configura o rádio LoRa na frequência e modo desejados.

### 2. Loop Principal

O programa entra em um loop infinito, chamando a função `ReceiveLora()` para verificar continuamente se há novos dados recebidos via LoRa.

---

## Principais Funções

### `main()`

- Inicializa hardware e LoRa.
- Entra em loop chamando `ReceiveLora()`.

### `ReceiveLora()`

- Verifica se o pino DIO0 está ativo (indicando que um pacote foi recebido).
- Lê os dados do buffer FIFO do LoRa.
- Pisca o LED onboard ao receber dados.
- Chama `parseSensorData()` para processar os dados recebidos.

### `parseSensorData(const char* data)`

- Faz o parsing dos dados recebidos no formato `temp;press;umid;alt`.
- Exibe os valores dos sensores no console.

### Funções auxiliares

- **SPI:** `cs_select()`, `cs_deselect()`, `writeRegister()`, `readRegister()` para comunicação com o módulo LoRa.
- **Configuração LoRa:** `setLora()`, `SetDeviceMode()`, `SetModemToLoRaMode()`, `SetFrequency()` para configurar o rádio conforme os parâmetros desejados.

---

## Estrutura dos Dados Recebidos

Os dados dos sensores devem chegar no formato:

```
temperatura;pressao;umidade;altitude
```

Exemplo:  
`25.3;101.2;60.5;150.0`

---

## Observações

- Os parâmetros de configuração LoRa (frequência, spreading factor, bandwidth, etc.) devem ser compatíveis com o transmissor.
- O código utiliza funções básicas de manipulação de hardware do Raspberry Pi Pico W.
- O LED onboard serve como indicação visual de recepção de dados.

---

## Dependências

- Biblioteca LoRa (lora.h)
- SDK do Raspberry Pi Pico (pico/stdlib.h, hardware/gpio.h, hardware/spi.h)

---

## Possíveis Melhorias

- Implementar tratamento de erros mais robusto.
- Adicionar interface gráfica ou envio dos dados para um servidor.
- Suporte a múltiplos