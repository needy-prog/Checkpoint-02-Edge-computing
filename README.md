# Checkpoint 2 - Edge Computing

## 📘 Descrição do Projeto
Este projeto implementa um **sistema inteligente de monitoramento ambiental** que mede **luminosidade, temperatura e umidade**, utilizando conceitos de **Edge Computing** com **Arduino**.  
O sistema detecta quando as condições do ambiente ultrapassam limites seguros e emite **alertas visuais e sonoros**, além de registrar automaticamente os eventos em **memória EEPROM**.  
Os dados e configurações são acessados por um **menu interativo controlado via joystick**, com interface exibida em um **display LCD 16x2 I2C**.
---
## ⚙️ Por onde acessar:
- https://wokwi.com/projects/444890370237225985 
---

## 👨‍💻 Autores
- **Victor Nunes**  
- **Roger Paiva**  
- **Guilherme Segolin**  
- **Pedro Henrique**

---

## ⚙️ Funcionalidades Principais
✅ **Monitoramento em tempo real:**
- Temperatura e umidade (sensor **DHT22**)  
- Intensidade luminosa (sensor **LDR**)  

✅ **Interface interativa:**
- Menu controlado por **joystick analógico**
- Exibição em **display LCD I2C**

✅ **Alertas automáticos:**
- **LEDs** indicam o estado ambiental  
- **Buzzer** avisa quando há anomalias  

✅ **Armazenamento de logs:**
- Registros salvos em **EEPROM**
- Envio de logs pelo **Serial Monitor**

✅ **Configurações salvas:**
- Escolha da unidade de temperatura (**Celsius** ou **Fahrenheit**)  
- Limpeza completa dos logs  

---

## 🧩 Componentes Utilizados

| Componente | Função | Pino no Arduino |
|-------------|--------|----------------|
| **Arduino UNO/Nano** | Microcontrolador principal | — |
| **Sensor DHT22** | Mede temperatura e umidade | D6 |
| **LDR + Resistor 10kΩ** | Mede luminosidade ambiente | A0 |
| **Display LCD 16x2 I2C** | Interface visual | SDA/SCL |
| **Joystick analógico** | Controle do menu | VRx: A1 / VRy: A2 / SW: D7 |
| **LED Verde** | Ambiente normal | D4 |
| **LED Amarelo** | — | D3 |
| **LED Vermelho** | Alerta crítico | D2 |
| **Buzzer** | Alerta sonoro | D5 |
| **RTC DS1307** | Relógio em tempo real | SDA/SCL |
| **EEPROM interna** | Armazena logs e configurações | — |

---

---

## 🧠 Lógica de Funcionamento

1. **Inicialização**
   - Exibe o nome da empresa “Vinheria Agnello”.
   - Lê a configuração de unidade (C/F) salva na EEPROM.

2. **Menu Principal**
   - Navegação via joystick (cima/baixo).
   - Pressione o botão (SW) para selecionar uma opção.

3. **Opções do Menu**
   - **Sensores:** Mostra temperatura, umidade e luminosidade em tempo real.  
   - **Log Serial:** Envia registros salvos no EEPROM para o Serial Monitor.  
   - **Configurações:** Permite alterar unidade de temperatura ou apagar logs.

4. **Alertas**
   - Se **temperatura**, **umidade** ou **luminosidade** saírem da faixa segura:
     - LED vermelho acende.
     - Buzzer emite som.
     - Dados são salvos na EEPROM com timestamp do RTC.

---

## 🧾 Limites Padrão de Alerta

| Parâmetro | Mínimo | Máximo |
|------------|---------|---------|
| Temperatura | 20 °C | 30 °C |
| Umidade | 30 % | 60 % |
| Luminosidade | — | 80 % |

> Esses limites podem ser ajustados diretamente nas variáveis `trigger_t_min`, `trigger_t_max`, etc.

---

## 📊 Estrutura de Dados na EEPROM

Cada registro ocupa **10 bytes**:

| Endereço | Dado | Tipo | Tamanho |
|-----------|-------|------|----------|
| 0–3 | Timestamp (RTC) | long | 4 bytes |
| 4–5 | Temperatura ×100 | int | 2 bytes |
| 6–7 | Umidade ×100 | int | 2 bytes |
| 8–9 | Luminosidade ×100 | int | 2 bytes |

---

## 🧾 Diagrama de Menu (Fluxo de Navegação)

```
                ┌────────────────────────┐
                │      MENU PRINCIPAL     │
                └──────────┬─────────────┘
                           │
         ┌─────────────────┼─────────────────┐
         │                 │                 │
   [0] Sensores       [1] Log Serial    [2] Configurações
         │                 │                 │
         │                 │        ┌────────┴─────────┐
         │                 │        │                  │
   Exibe dados      Envia logs  Unidade Temp.     Apagar Logs
   T/U/Lux em LCD   via Serial  (C/F) toggle      (Confirmar/Cancelar)
```

---

## 💾 Bibliotecas Necessárias

Instale no **Arduino IDE**:

```text
LiquidCrystal_I2C
RTClib
EEPROM
DHT sensor library
Wire
```

---

## 🚀 Como Executar o Projeto

1. **Monte o circuito** conforme o esquema elétrico.  
2. **Conecte o Arduino** ao computador.  
3. **Carregue o código** no Arduino IDE.  
4. **Abra o Serial Monitor** (9600 baud).  
5. Use o **joystick** para navegar entre as opções no LCD.

---

## 🧹 Funcionalidades Extras

- **Limpeza de Logs:** Remove todos os registros da EEPROM.  
- **Unidade de Temperatura:** Alterna entre Celsius e Fahrenheit e salva a escolha permanentemente.  
- **EEPROM Circular:** Quando cheia, substitui os registros mais antigos pelos novos.  

---

## 🏢 Contexto do Projeto

Desenvolvido para a **Vinheria Agnello**, simulando um sistema de **monitoramento ambiental de adega**, garantindo que as condições de temperatura, umidade e luminosidade permaneçam ideais para a conservação de vinhos.

---

## 📸 Exemplo no LCD

```
T:25.3C  U:55%
Luz:42%
```

---

## 📅 Versão
**v1.0 – Novembro/2025**

---

