# 🎨 Sensor de Cor com DMA no Raspberry Pi Pico

Sistema embarcado com leitura de cores via sensor GY-33, exibição em display SSD1306 e controle de LED RGB com efeito PWM usando DMA.

## 🎯 Funcionalidades

- **Leitura de Cores com GY-33**
  - Captura dos valores RGB e Clear
  - Classificação de cores: Vermelho, Amarelo, Verde, Azul
  - Impressão dos valores no display SSD1306

- **Display OLED SSD1306**
  - Exibição do nome da cor detectada
  - Exibição dos valores RGB em tempo real

- **Controle de LED RGB**
  - Acendimento do LED conforme cor detectada
  - Feedback visual direto no hardware

- **PWM com DMA**
  - Efeito de fade suave via buffer
  - Transferência automática sem carga da CPU
  - Interrupção DMA para reinício contínuo

---

## 📚 Documentação

- **TUTORIAL_SENSOR_COR.md** – Guia passo a passo  
- **CONFIG_PICO_SDK.md** – Instalação do SDK  
- **DMA_EXPLICACAO.md** – Como funciona o DMA no projeto  

---

## 🚀 Quick Start

### 1️⃣ Compilar Firmware
```bash
cd sensor-cor-dma/build
cmake ..
make
```

### 2️⃣ Gravar no Pico
Conecte o Pico em modo **BOOTSEL** e grave o `.uf2` gerado.

### 3️⃣ Conectar Hardware

| Dispositivo | Pico |
|-------------|------|
| GY-33 SDA   | GP0  |
| GY-33 SCL   | GP1  |
| SSD1306 SDA | GP14 |
| SSD1306 SCL | GP15 |
| LED Vermelho | GP13 |
| LED Verde | GP11 |
| LED Azul | GP12 |

---

## 🏗️ Arquitetura

```
Sensor GY-33 → Pico (Classificação + DMA PWM) → Display SSD1306
```

---

## 🛠️ Tecnologias

- Raspberry Pi Pico  
- Sensor GY-33 (I²C)  
- Display SSD1306 (I²C)  
- DMA + PWM  
- Pico SDK  

---

## 📦 Estrutura do Projeto

```
sensor-cor-dma/
├── main.c
├── dma_pwm.c
├── gy33.c
├── ssd1306.c
├── build/
└── README.md
```

---

## 🔧 Configuração

### Sensor GY-33
```
ENABLE_REG  0x80  
ATIME_REG   0xD5  
CONTROL_REG 0x02  
```

### LED RGB
```
RED_PIN    13  
GREEN_PIN  11  
BLUE_PIN   12  
```

---

## 🎮 Como Usar

1. Aponte o sensor para uma cor sólida  
2. Observe os valores RGB no display  
3. Veja a cor identificada  
4. LED acende conforme a cor detectada  

---

## 🐛 Troubleshooting

| Problema | Solução |
|----------|---------|
| Valores RGB muito baixos | Aumente o ganho (CONTROL_REG) |
| Display não mostra nada | Verifique conexões I²C |
| LED não acende | Verifique pinos e lógica |
| Pico não compila | Confirme instalação do SDK |

---

## 📄 Licença
MIT License

