# Atividade com módulo Lora - Transmissor
# Exibição de dados de temperatura, umidade e pressão

**Autor:** [Grupo 01]  

Este projeto utiliza o microcontrolador **Raspberry Pi Pico (RP2040)** em conjunto com um sensores bmp280 e aht20 e um módulo Lora. Os dados são coletados e emitidos. 

## Funcionalidades  

- Leitura do temperatura e pressão com sensor 
- Leitura de umidade e temperatura  
- Simples Menu com os dados no monitor serial  
- Transmissão de dados para outra BitDogLab  
- Comunicação via barramento **I2C**  
    

## Hardware Necessário  

- **Raspberry Pi Pico (RP2040)**  
- Sensores **[BMP280 e AHT20]** – I2C    
- Extensor   
- Cabos de conexão  
- Módulo Lora

## Como Funciona  

O código inicializa os sensores conectados ao Raspberry Pi Pico por barramentos **I2C**. A leitura dos dados ambientais é feita, e os resultados são:  

- Transmitidos para outro módulo Lora 
- Impressos no terminal serial para fins de monitoramento 
  

## Observações  

- Certifique-se de que os sensores estejam conectados corretamente ao barramento **I2C**.     
- Usar um módulo extensor
  

---  
