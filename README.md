# Atividade com módulo LoRa - Transmissor e Receptor

**Autor:** [Grupo 01]

**▶️ [Assista à demonstração do projeto no YouTube](https://www.youtube.com/seu-link-do-video)**

Este projeto utiliza o microcontrolador **Raspberry Pi Pico (RP2040)** em conjunto com os sensores **BMP280** (pressão/temperatura) e **AHT20** (umidade/temperatura), além de um módulo **LoRa SX1276**. O sistema realiza a leitura dos dados ambientais e transmite via rádio LoRa para outro Pico W, que recebe e exibe as informações.

## Estrutura do Projeto

- **Lora_transmissor/**  
  Código-fonte do transmissor LoRa, incluindo leitura dos sensores e envio dos dados.
- **Lora_receptor/**  
  Código-fonte do receptor LoRa, responsável por receber, processar e exibir os dados.
- **README.md**  
  Este arquivo de documentação principal.

## Funcionalidades

- Leitura de temperatura, pressão e umidade dos sensores BMP280 e AHT20 via I2C.
- Cálculo da altitude baseada na pressão atmosférica.
- Transmissão dos dados ambientais via rádio LoRa.
- Recepção dos dados em outro dispositivo e exibição no terminal serial.
- Indicação visual de transmissão/recepção usando o LED onboard do Pico W.

## Hardware Necessário

- 2x **Raspberry Pi Pico W**
- 2x **Módulo LoRa SX1276**
- 1x **Sensor BMP280** (I2C)
- 1x **Sensor AHT20** (I2C)
- Cabos de conexão e extensores conforme necessário

## Como Funciona

1. O transmissor lê os dados dos sensores BMP280 e AHT20.
2. Os dados são formatados como:  
   `temperatura;pressao;umidade;altitude`
3. O transmissor envia os dados via LoRa.
4. O receptor recebe os dados, faz o parsing e exibe no terminal serial.

## Como Compilar

Cada pasta possui um arquivo `CMakeLists.txt` para facilitar a compilação com o SDK do Raspberry Pi Pico.

Exemplo para compilar o transmissor:
```sh
cd Lora_transmissor
mkdir build && cd build
cmake ..
make
```
