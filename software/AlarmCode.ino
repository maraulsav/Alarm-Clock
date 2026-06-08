#include "Adafruit_GFX.h"
#include "Adafruit_ST7735.h"
#include "Adafruit_AHTX0.h"
#include "ScioSense_ENS160.h"
#include <WiFi.h>

#define TFT_SCL 5
#define TFT_SDA 6
#define TFT_RES 7
#define TFT_DC 8
#define TFT_CS 9
#define TFT_A 10
#define TFT_B 20
#define TFT_Push 21
#define TFT_KO 0
#define ENS_SCL 2
#define ENS_SDA 1
#define LED 4
#define Buz 3

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RES);
Adafruit_AHTX0 aht;
ScioSense_ENS160 ens160(0x53);

#include <Fonts/FreeSerif18pt7b.h>
const char* ssid = "Dawn";
const char* password = "sasha101";


//Menu
enum UIMode{
  MODE_MENU = 0,
  MODE_CLOCK,
  MODE_ALARM,
  MODE_LED
};

UIMode currentMode = MODE_CLOCK;
int menuIndex = 0;
const int MENU_ITEMS = 3;

//The Environment Sensor Thingy Values
float curTemp = 0;
float curHum = 0;
float curTVOC = 0;
float curECO2 = 400;
unsigned long lastEnv = 0;

//Rotary Encoder
int lastEncA = HIGH;
int lastEncB = HIGH;
bool lastEncBtn = HIGH;
bool lastKey0 = HIGH;
unsigned long lastBtn = 0;

// Alarm
bool alarmEn = false;
int alarmHour = 7;
int alarmMinute = 0;
bool alarmRinging = false;
int alarmSlcField = 0;
int lastAlarmDay = -1;


bool buttonPress(int pin, bool &lastState){
  bool cur = digitalRead(pin);
  bool press = false;
  unsigned long now = millis();
  if (cur == LOW && lastState == HIGH && (now-lastBtn) > 150){
    press = true;
    lastBtn = now;
  }
  lastState = cur;
  return press;
}

void updateEnvSens(bool force = false){
  unsigned long now = millis();
  if (!force && (now - lastEnv) < 5000) return;
  lastEnv = now;

  sensors_event_t hum, temp;
  if (aht.getEvent(&hum, &temp)){
    curTemp = temp.temperature;
    curHum = hum.relative_humidity;
  }
  ens160.set_envdata(curTemp, curHum);
  ens160.measure();

  float newTVOC = ens160.getTVOC();
  float newCO2 = ens160.getCO2();

  if (newTVOC != 0xFFFF) curTVOC = newTVOC;
  if (newCO2  != 0xFFFF) curECO2 = newCO2;

  Serial.print("ENS160: TVOC=");
  Serial.print(curTVOC);
  Serial.print(" eCO2=");
  Serial.print(curCO2);
}


// WiFi & Time
void connectWiFiAndSyncTime(){
  
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(10, 55);
  tft.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  const char* ntpServer = "pool.ntp.org";
  const long gmtOffset_sec = 7 * 3600;
  const int daylightOffset_sec = 0;

  int retry = 0;
  while (WiFi.status() != WL_CONNECTED && retry < 20){
    delay(300);
    tft.print(".");
    retry++;
  }

  if(WiFi.status() == WL_CONNECTED){
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
    tft.fillScreen(ST7735_BLACK); 
    tft.setTextColor(ST7735_WHITE);
    tft.setCursor(10, 55);
    tft.print("Syncing time...");
    delay(800);
  } else{
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_RED);
    tft.setCursor(10, 55);
    tft.print("WiFi Failed");
    delay(1200);
  }
}

String getTime(char type){
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) return "--";
  char buf[8];
  if (type == 'H') strftime(buf, sizeof(buf), "%H", &timeinfo);
  else if (type == 'M') strftime(buf, sizeof(buf), "%M", &timeinfo);
  else if (type == 'S') strftime(buf, sizeof(buf), "%S", &timeinfo);
  else return "--";
  return String(buf);
}

int readEncoderStep() {
  int a = digitalRead(TFT_A);
  int b = digitalRead(TFT_B);
  int step = 0;
  if (a != lastEncA) {
    if (a == LOW) {
      step = (b == HIGH) ? 1 : -1;
    }
  }
  lastEncA = a;
  lastEncB = b;
  return step;
}

bool checkButtonPressed(int pin, bool &lastState) {
  return buttonPress(pin, lastState);
}


// MENU
void drawMenu(){
  tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_WHITE);
  tft.setCursor(10, 10);
  tft.print("SELECT MODE");

  const char* items[MENU_ITEMS] = {
    "CLOCK",
    "ALARM",
    "LED"
  };

  for (int i = 0; i < MENU_ITEMS; i++){
    int y = 32 + i * 18;
    if (i == menuIndex) {
      tft.fillRect(6, y-2, 148, 14, ST7735_YELLOW);
      tft.setTextColor(ST7735_BLACK);
    } 
    else{
      tft.fillRect(6, y-2, 148,14, ST7735_BLACK);
      tft.setTextColor(ST7735_WHITE);
    }
    tft.setCursor(12, y);
    tft.print(items[i]);
  } 
}



// LED

void drawLedMenu(bool fullRedraw) {
if (fullRedraw) tft.fillScreen(ST7735_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST7735_CYAN);
  tft.setCursor(20, 10);
  tft.print("LED CONTROL");

  tft.fillRect(5, 30, 150, 40, ST7735_BLACK);
  tft.setTextColor(ledOption == 0 ? ST7735_YELLOW : ST7735_WHITE);
  tft.setCursor(10, 33);
  tft.print(ledOption == 0 ? "> Turn ON" : "  Turn ON");

  tft.setTextColor(ledOption == 1 ? ST7735_YELLOW : ST7735_WHITE);
  tft.setCursor(10, 50);
  tft.print(ledOption == 1 ? "> Turn OFF" : "  Turn OFF");

  tft.setTextColor(ST7735_GREEN);
  tft.setCursor(10, 75);
  tft.print("LED: ");
  tft.print(ledState ? "ON " : "OFF");
}

void handleButton() {

  if (currentMenu == LED) {

    if (ledOption == 0) {
      digitalWrite(LED_PIN, HIGH);
      ledState = true;
    }

    else if (ledOption == 1) {
      digitalWrite(LED_PIN, LOW);
      ledState = false;
    }
  }
}

//Alarm
void checkAlarmTrigger(){
  if (!alarmEn || alarmRinging) return;
  struct tm timeinfo;  
  if (!getLocalTime(&timeinfo)) return;
  if (timeinfo.tm_hour == alarmHour &&
      timeinfo.tm_min == alarmMinute &&
      timeinfo.tm_sec == 0 &&
      timeinfo.tm_mday != lastAlarmDay) {
    alarmRinging = true;
    lastAlarmDay = timeinfo.tm_mday;
    currentMode = MODE_ALARM;
  }
}

void updateAlertStateAndLED(){
  static unsigned long lastBlink = 0;
  static bool blinkState = false;
  if (alarmRinging){
    if (millis() - lastBlink > 300){
      lastBlink = millis();
      blinkState = !blinkState;
      digitalWrite(LED, blinkState ? HIGH : LOW);
    }
  } else {
    digitalWrite(LED, LOW);
  }
}

void setup() {


  Serial.begin(115200);
  delay (1500);

  pinMode(TFT_A, INPUT_PULLUP);
  pinMode(TFT_B, INPUT_PULLUP);
  pinMode(TFT_Push, INPUT_PULLUP);
  pinMode(TFT_KO, INPUT_PULLUP);

  pinMode(LED, OUTPUT);
  pinMode(Buz, OUTPUT);
  digitalWrite(LED, LOW);

  Wire.begin(ENS_SDA, ENS_SCL);
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS);

  tft.initR(INITR_BLACKTAB);
  tft.fillScreen(ST7735_BLACK);
  tft.setRotation(1);
  tft.setTextWrap(true);

  connectWifiAndSyncTime();
  if(!aht.begin()) Serial.println("AHT not found TRY AGAIN");
  if(!ens160.begin()) Serial.println("ENS160 FAILED TRY AGAIN");
  else ens160.setMode(ENS160_OPMODE_STD);

  updateEnvSens(true);

  initClockStaticUI();
  prevTimeStr = "";
  drawClockTime(getTimeStr('H'), getTimeStr('M'), getTimeStr('S'));
  drawEnvDynamic(curTemp, curHum curTVOC, curECO2);
}

void loop() {
  // put your main code here, to run repeatedly:
  static unsigned long lastWifiCheck = 0;
  if (millis() - lastWifiCheck > 10000){
    lastWifiCheck = millis();
    if (WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, password);
    }
  }
  
  int encStep = readEncoderStep();
  bool encPress = checkButtonPressed(ENC_BTN_PIN, lastEncBtn);
  bool k0Pressed = checkButtonPressed(KEY0_PIN, lastKey0);

  checkAlarmTrigger();
  updateAlertStateAndLED();

  switch (currentMode){
    case MODE_MENU: {
      if (encStep != 0) {
        menuIndex += encStep;
        if (menuIndex < 0 ) menuIndex = MENU_ITEMS - 1
        if (menuIndex >= MENU_ITEMS) menuIndex = 0;
        drawMenu();
      }
      if (encPressed){
        if (menuIndex == 0){
          currentMode = MODE_CLOCK;
          initClockStaticUI();
          prevTimeStr = "";
          updateEnvSens(true);
          drawClockTime (getTimeStr ('H'), getTimeStr('M'), getTimeStr ('S'));
          drawEnvDynamic(curTemp, curHum, curTVOC, curECO2);
        }
        else if(menuIndex == 1){
          currentMode = MODE_ALARM;
          alarmSelectedField = 0;
          drawAlarmScreen(true);
        }
        else if (menuIndex == 2){
          currentMode = MODE_LED;
          drawLEDMenu(true);
        }
      }
      break
    }
    case MODE_CLOCK: {
      struct tm timinfo;
      if(getLocalTime(&timinfo)){
        int sec = timinfo.tm_sec;
        if (sec != prevSecond){
          prevSecond = sec;
          drawClockTime(getTimeStr('H'), getTimeStr('M'), getTimeStr('S'));
          if(sec% 5 == 0){
            updateEnvSensors(true);
            drawEnvDynamic(curTemp, curHum, curTVOC, curECO2);

          }
        }
      }
      if (k0Pressed){
        currentMode = MODE_MENU;
        drawMenu();
      }
      break
    }

    case MODE_ALARM: {
      if (alarmRInging) {
        static unsigned long lastBeep = 0;
        if (millis()- lastBeep > 1000){
          lastBeep = millis();
          tone(Buz, 2000, 400);
        }
        if (encPressed || k0Pressed) {
          alarmRinging = false;
          lastAlarmDayTrigged = -1;
          noTone(Buz);
          drawAlarmScreen(true);
        }
        break;
      }
      bool changed = false;
      if (encStep != 0) {
        if (alarmSelectedField == 0){
          if(encStep > 0) alarmHour = (alarmHour + 1) % 24;
          else alarmHour = (alarmHour+23) % 24;
          changed = true;
        }
        else if (alarmSelectedField == 1) {
          if (encStep > 0) alarmMinute = (alarmMinute + 1) % 60;
          else alarmMinute = (alarmMinute + 59) % 60;
          changed = true;
        }
        else if (alarmSelectedField == 2){
          alarmEnabled = !alarmEnabled;
          changed = true;
        }
        if(changed){
          lastAlarmDay = -1
        }
      }
      if (encPressed){
        alarmSelectedField = (alarmSelectedField + 1)% 3;
        changed = true;
      }
    }
    if (encPressed){
      alarmSelectedField = (alarmSelectedField + 1) % 3 ;
      changed = true;
    }
    if (k0Pressed){
      currentMode = MODE_MENU;
      drawMenu();
      break
    }
    if(changed){
      drawAlarmScreen(false);
      drawAlarmIcon();
    }

    case MODE_LED: {
      if (encStep != 0){
        ledOption += encStep;
        if (ledOption < 0) ledOption = 1;
        if (ledOption > 1) ledOption = 0;
        drawLEDMenu(false);
      }
      if (encPress) {
        if (ledOption == 0){
          ledState = true;
          digitalWrite(LED, HIGH);
        } else{
          ledState = false;
          digitalWrite(LED, LOW);
        }
        drawLEDMenu(false);
      }
      if (k0Press){
        currentMode = MODE_MENU;
        drawMenu();
      }
      break;
    }
  }
  break;
}
