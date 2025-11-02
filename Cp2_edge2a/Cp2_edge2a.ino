// ====================================================
// Projeto: Sensor de Luminosidade - Edge Computing CP1
// Menu com joystick integrado + configuração de unidade e limpeza de logs
// Autores: Victor Nunes, Roger Paiva, Guilherme Segolin, Pedro Henrique
// ====================================================

#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Wire.h>
#include <EEPROM.h>
#include "DHT.h"

// =====================
// ==== CONFIGURAÇÕES ====
// =====================
#define VRx A1
#define VRy A2
#define SW 7

#define DHTPIN 6
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

LiquidCrystal_I2C lcd(0x27,16,2);
RTC_DS1307 RTC;

const int LED_VERMELHO = 2;
const int LED_AMARELO  = 3;
const int LED_VERDE    = 4;
const int LDR_PIN      = A0;  
const int BUZZER       = 5;

const int NUM_LEITURAS = 10;

// Limites
float trigger_t_min = 20.0;
float trigger_t_max = 30.0;
float trigger_u_min = 30.0;
float trigger_u_max = 60.0;
float trigger_lux_max = 80.0;

const int maxRecords = 100;
const int recordSize = 10;
int startAddress = 10; // bytes 0-9 reservados para configs
int endAddress = startAddress + maxRecords * recordSize;
int currentAddress = startAddress;

unsigned long lastMove = 0;
const int debounceDelay = 200;
int menuIndex = 0;
const int totalMenus = 3;

bool inMenu = true;

// Configuração de unidade
bool usarFahrenheit = false; 
const int enderecoConfig = 0; 

// =====================
// ==== SETUP ====
// =====================
void setup() {
  pinMode(LDR_PIN, INPUT);
  pinMode(VRx, INPUT);
  pinMode(VRy, INPUT);
  pinMode(SW, INPUT_PULLUP);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(LED_AMARELO, OUTPUT);
  pinMode(LED_VERDE, OUTPUT);
  pinMode(BUZZER, OUTPUT);

  dht.begin();
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  RTC.begin();

  // ==============================
  // Nome da empresa
  // ==============================
  String empresa = "Vinheria Agnello";
  lcd.setCursor(0,0);
  for (int i = 0; i < empresa.length(); i++) {
    lcd.print(empresa[i]);
    delay(200);
  }
  delay(2000);
  lcd.clear();

  lcd.setCursor(0,0);
  lcd.print("Seja Bem-Vindo!");
  delay(3000);
  lcd.clear();

  EEPROM.get(enderecoConfig, usarFahrenheit);
  if (usarFahrenheit != true && usarFahrenheit != false) usarFahrenheit = false;

  lcd.setCursor(0,0);
  lcd.print("Menu iniciado");
  delay(1000);
  lcd.clear();
}

// =====================
// ==== LOOP ====
// =====================
void loop() {
  if (inMenu) navegarMenu();
}

// =====================
// ==== MENU ====
// =====================
void navegarMenu() {
  int yValue = analogRead(VRy);
  bool buttonPressed = !digitalRead(SW);

  if (millis() - lastMove > debounceDelay) {
    if (yValue < 400) { 
      menuIndex--;
      if (menuIndex < 0) menuIndex = totalMenus - 1;
      lastMove = millis();
    } else if (yValue > 600) { 
      menuIndex++;
      if (menuIndex >= totalMenus) menuIndex = 0;
      lastMove = millis();
    }
  }

  lcd.setCursor(0,0);
  lcd.print("Selecionar:     ");
  lcd.setCursor(0,1);
  switch(menuIndex) {
    case 0: lcd.print("> Sensores       "); break;
    case 1: lcd.print("> Log Serial     "); break;
    case 2: lcd.print("> Configuracoes  "); break;
  }

  if (buttonPressed) {
    inMenu = false;
    lcd.clear();
    executarOpcao(menuIndex);
  }
}

// =====================
// ==== EXECUÇÃO ====
// =====================
void executarOpcao(int opcao) {
  bool exitOption = false;

  if (opcao == 1) mostrarLogSerial();
  if (opcao == 2) menuConfiguracoes();

  while (!exitOption) {
    bool buttonPressed = !digitalRead(SW);
    if (buttonPressed) {
      exitOption = true;
      lcd.clear();
      inMenu = true;
      delay(300);
      break;
    }

    if (opcao == 0) mostrarSensoresLCD();
  }
}

// =====================
// ==== SENSOR E LOG ====
// =====================
void mostrarSensoresLCD() {
  int soma = 0;
  for (int i = 0; i < NUM_LEITURAS; i++) {
    int valorLido = analogRead(LDR_PIN);
    int valorConvertido = map(valorLido, 0, 1023, 100, 0);
    soma += valorConvertido;
    delay(30);
  }
  int luminosity = soma / NUM_LEITURAS;

  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();

  if (usarFahrenheit) temperature = temperature * 1.8 + 32.0;

  if (isnan(humidity) || isnan(temperature)) {
    lcd.setCursor(0,0);
    lcd.print("Erro sensor DHT ");
    lcd.setCursor(0,1);
    lcd.print("Verifique conexao");
    delay(2000);
    return;
  }

  bool foraDoPadrao = (
    temperature < trigger_t_min || temperature > trigger_t_max ||
    humidity < trigger_u_min || humidity > trigger_u_max ||
    luminosity > trigger_lux_max
  );

  if(foraDoPadrao){
    long timestamp = RTC.now().unixtime();
    int tempInt = (int)(temperature*100);
    int humiInt = (int)(humidity*100);
    int luxInt  = (int)(luminosity*100);
    EEPROM.put(currentAddress, timestamp);
    EEPROM.put(currentAddress+4, tempInt);
    EEPROM.put(currentAddress+6, humiInt);
    EEPROM.put(currentAddress+8, luxInt);
    getNextAddress();

    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_AMARELO, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    tone(BUZZER,1000);
    delay(50);
    noTone(BUZZER);
  } else {
    digitalWrite(LED_VERDE,HIGH);
    digitalWrite(LED_AMARELO,LOW);
    digitalWrite(LED_VERMELHO,LOW);
  }

  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temperature,1);
  lcd.print(usarFahrenheit ? "F " : "C ");
  lcd.print("U:");
  lcd.print(humidity,0);
  lcd.print("%   ");

  lcd.setCursor(0,1);
  lcd.print("Luz:");
  lcd.print(luminosity);
  lcd.print("%     ");
}

void mostrarLogSerial() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enviando logs...");
  lcd.setCursor(0, 1);
  lcd.print("Ver Serial");

  Serial.println("===== LOG EEPROM =====");
  Serial.println("Timestamp\t\tTemperature\tHumidity\tLuminosity");

  for(int address = startAddress; address < endAddress; address += recordSize) {
    long timeStamp;
    int tempInt, humiInt, luxInt;
    EEPROM.get(address, timeStamp);
    EEPROM.get(address + 4, tempInt);
    EEPROM.get(address + 6, humiInt);
    EEPROM.get(address + 8, luxInt);

    if (timeStamp != 0xFFFFFFFF) {
      DateTime dt(timeStamp);
      float temperature = tempInt / 100.0;
      float humidity = humiInt / 100.0;
      int luminosity = luxInt / 100.0;

      Serial.print(dt.timestamp(DateTime::TIMESTAMP_FULL));
      Serial.print("\t");
      Serial.print(temperature); Serial.print("\t\t");
      Serial.print(humidity); Serial.print("%\t\t");
      Serial.print(luminosity); Serial.println("%");
    }
  }

  Serial.println("----- Fim do Log -----");
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Logs enviados!");
  lcd.setCursor(0,1);
  lcd.print("Press. p/ voltar");
}

void getNextAddress() {
  currentAddress += recordSize;
  if(currentAddress >= endAddress) currentAddress = startAddress;
}

// =====================
// ==== MENU CONFIGURAÇÕES ====
// =====================
void menuConfiguracoes() {
  int configIndex = 0;
  const int totalConfigs = 2;
  bool selecionando = true;

  while (selecionando) {
    int yValue = analogRead(VRy);
    bool buttonPressed = !digitalRead(SW);

    if (millis() - lastMove > debounceDelay) {
      if (yValue < 400) { 
        configIndex--;
        if (configIndex < 0) configIndex = totalConfigs - 1;
        lastMove = millis();
      } else if (yValue > 600) { 
        configIndex++;
        if (configIndex >= totalConfigs) configIndex = 0;
        lastMove = millis();
      }
    }

    lcd.setCursor(0,0);
    lcd.print("Configuracoes:  ");
    lcd.setCursor(0,1);
    if (configIndex == 0) lcd.print("> Unidade Temp. ");
    else lcd.print("> Apagar Logs   ");

    if (buttonPressed) {
      lcd.clear();
      if (configIndex == 0) configurarUnidade();
      else confirmarLimparLogs();
      selecionando = false;
    }
  }
}

// =====================
// ==== SUBFUNÇÕES CONFIG ====
// =====================
void configurarUnidade() {
  int opcao = usarFahrenheit ? 1 : 0;
  bool selecionado = false;

  while (!selecionado) {
    int yValue = analogRead(VRy);
    bool buttonPressed = !digitalRead(SW);

    if (millis() - lastMove > debounceDelay) {
      if (yValue < 400 || yValue > 600) {
        opcao = !opcao;
        lastMove = millis();
      }
    }

    lcd.setCursor(0,0);
    lcd.print("Unidade Temp:");
    lcd.setCursor(0,1);
    if (opcao == 0) lcd.print("> Celsius       ");
    else lcd.print("> Fahrenheit    ");

    if (buttonPressed) {
      usarFahrenheit = opcao;
      EEPROM.put(enderecoConfig, usarFahrenheit);
      lcd.clear();
      lcd.print("Configuracao");
      lcd.setCursor(0,1);
      lcd.print("salva!");
      delay(1500);
      selecionado = true;
    }
  }

  lcd.clear();
  inMenu = true;
}

void confirmarLimparLogs() {
  bool confirmar = false;
  int escolha = 1; // 1 = Não, 0 = Sim

  while (!confirmar) {
    int yValue = analogRead(VRy);
    bool buttonPressed = !digitalRead(SW);

    if (millis() - lastMove > debounceDelay) {
      if (yValue < 400 || yValue > 600) {
        escolha = !escolha;
        lastMove = millis();
      }
    }

    lcd.setCursor(0,0);
    lcd.print("Apagar todos os ");
    lcd.setCursor(0,1);
    if (escolha == 0) lcd.print("> Sim    Nao    ");
    else lcd.print("  Sim   >Nao    ");

    if (buttonPressed) {
      if (escolha == 0) {
        for (int i = startAddress; i < endAddress; i++) EEPROM.write(i, 0xFF);
        lcd.clear();
        lcd.print("Logs apagados!");
        delay(1500);
      } else {
        lcd.clear();
        lcd.print("Cancelado");
        delay(1000);
      }
      confirmar = true;
    }
  }

  lcd.clear();
  inMenu = true;
}