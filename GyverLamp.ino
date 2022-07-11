#define UDP_TX_PACKET_MAX_SIZE 860
// ============= НАСТРОЙКИ =============
// -------- ВРЕМЯ -------
#define GMT 3              // смещение (москва 3)
#define NTP_ADDRESS  "europe.pool.ntp.org"    // сервер времени

// -------- РАССВЕТ -------
#define DAWN_BRIGHT 200       // макс. яркость рассвета
#define DAWN_TIMEOUT 1        // сколько рассвет светит после времени будильника, минут

// ---------- МАТРИЦА ---------
#define CURRENT_LIMIT 3000    // лимит по току в миллиамперах, автоматически управляет яркостью (пожалей свой блок питания!) 0 - выключить лимит

#define WIDTH 16              // ширина матрицы
#define HEIGHT 24             // высота матрицы

#define COLOR_ORDER GRB       // порядок цветов на ленте. Если цвет отображается некорректно - меняйте. Начать можно с RGB

#define MATRIX_TYPE 0         // тип матрицы: 0 - зигзаг, 1 - параллельная
#define CONNECTION_ANGLE 3    // угол подключения: 0 - левый нижний, 1 - левый верхний, 2 - правый верхний, 3 - правый нижний
#define STRIP_DIRECTION 1     // направление ленты из угла: 0 - вправо, 1 - вверх, 2 - влево, 3 - вниз
// при неправильной настройке матрицы вы получите предупреждение "Wrong matrix parameters! Set to default"
// шпаргалка по настройке матрицы здесь! https://alexgyver.ru/matrix_guide/

// --------- ESP --------

#define ESP_MODE 0
// 0 - точка доступа
// 1 - локальный
byte IP_AP[] = {192, 168, 4, 66};   // статический IP точки доступа (менять только последнюю цифру)
byte IP_STA[] = {192, 168, 1, 66};  // статический IP локальный (менять только последнюю цифру)

// ----- AP (точка доступа) -------
#define AP_SSID "FireLamp"
#define AP_PASS ""
#define AP_PORT 8888

// -------- Менеджер WiFi ---------
#define AC_SSID "AutoConnectAP"
#define AC_PASS "12345678"

// ============= ДЛЯ РАЗРАБОТЧИКОВ =============
#define LED_PIN 4
#define POWER_LED_PIN 16
#define MODE_AMOUNT 18

#define NUM_LEDS WIDTH * HEIGHT
#define SEGMENTS 1            // диодов в одном "пикселе" (для создания матрицы из кусков ленты)
// ---------------- БИБЛИОТЕКИ -----------------
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define NTP_INTERVAL 60 * 1000    // обновление (1 минута)

#include "timerMinim.h"
#include <FastLED.h>

#include <WiFiUdp.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <GyverButton.h>

// ------------------- ТИПЫ --------------------
CRGB leds[NUM_LEDS];
//WiFiServer server(80);
WiFiUDP Udp;
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, GMT * 3600, NTP_INTERVAL);
timerMinim timeTimer(3000);
//GButton touch(BTN_PIN, LOW_PULL, NORM_OPEN);
GButton plusButton(13, HIGH_PULL, NORM_OPEN);
GButton minusButton(14, HIGH_PULL, NORM_OPEN);
GButton powerButton(12, HIGH_PULL, NORM_OPEN);
GButton changeEffectButton(5, HIGH_PULL, NORM_OPEN);

// ----------------- ПЕРЕМЕННЫЕ ------------------
const char* autoConnectSSID = AC_SSID;
const char* autoConnectPass = AC_PASS;
const char AP_NameChar[] = AP_SSID;
const char WiFiPassword[] = AP_PASS;
unsigned int localPort = AP_PORT;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet
String inputBuffer;
static const byte maxDim = max(WIDTH, HEIGHT);
struct {
  byte brightness = 50;
  byte speed = 30;
  byte scale = 40;
} modes[MODE_AMOUNT];

byte dawnOffsets[] = {5, 10, 15, 20, 25, 30, 40, 50, 60};
byte dawnMode;
long thisTime;
boolean manualOff = false;

int8_t currentMode = 0;
boolean loadingFlag = true;
boolean ONflag = true;
uint32_t eepromTimer;
boolean settChanged = false;
unsigned long turnOffTime = 0;
// Конфетти, Огонь, Радуга верт., Радуга гориз., Смена цвета,
// Безумие 3D, Облака 3D, Лава 3D, Плазма 3D, Радуга 3D,
// Павлин 3D, Зебра 3D, Лес 3D, Океан 3D,

unsigned char matrixValue[8][16];


//////////////////////////////////// add by lakec ////////////////////////////////////

uint8_t effect_changed = 0;
uint8_t brig_changed = 0;
uint8_t speed_changed = 0;
uint8_t scale_changed = 0;

#define AND	  && 			//	
#define OR	  ||			//	

#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#include <FS.h>
//#include <SPIFFS.h>

#include "json.h"
#include "fs.h"
#include "websocket.h"
#include "wifi_init.h"
#include "url_init.h"
#include "http_init.h"


//////////////////////////////////// add by lakec ////////////////////////////////////

void setup() {
  Serial.begin(115200);
  
  pinMode(13, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);
  pinMode(12, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(POWER_LED_PIN, OUTPUT);
  minusButton.setStepTimeout(250);
  plusButton.setStepTimeout(250);
  
  // ЛЕНТА
  FastLED.addLeds<WS2812B, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)/*.setCorrection( TypicalLEDStrip )*/;
  FastLED.setBrightness(0);
  if (CURRENT_LIMIT > 0) FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
  FastLED.clear();
  delay(20);
  FastLED.show();

  Serial.println();

  // WI-FI
//////////////////////////////////// add by lakec ////////////////////////////////////

	SPIFFS.begin();
	Serial.println();
	configSetup = readFile("config.json", 4096);
	Serial.println(configSetup);

	WIFIinit();
	
	if (MDNS.begin("firelamp")) {
		Serial.println("MDNS responder started");
		MDNS.addService("http", "tcp", 80);
		MDNS.addService("ws", "tcp", 81);
	} else {
		Serial.println("MDNS.begin failed");
	}
	Serial.print("Connect to http://firelamp.local or http://");
	Serial.println(WiFi.localIP());
	
	url_init();
	HTTP_init();
	

	webSocket.begin();
	webSocket.onEvent(webSocketEvent);


//////////////////////////////////// add by lakec ////////////////////////////////////


  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);

  // EEPROM
  EEPROM.begin(202);
  delay(50);
  if (EEPROM.read(198) != 20) {   // первый запуск
    EEPROM.write(198, 20);
    EEPROM.commit();

    for (byte i = 0; i < MODE_AMOUNT; i++) {
      EEPROM.put(3 * i + 40, modes[i]);
      EEPROM.commit();
    }
    EEPROM.write(199, 0);   // рассвет
    EEPROM.write(200, 0);   // режим
    EEPROM.commit();
  }
  for (byte i = 0; i < MODE_AMOUNT; i++) {
    EEPROM.get(3 * i + 40, modes[i]);
  }
  dawnMode = EEPROM.read(199);
  currentMode = (int8_t)EEPROM.read(200);
  changePower();

  // отправляем настройки
  sendCurrent();
  uint8_t reply[inputBuffer.length() + 1];
  inputBuffer.toCharArray((char *)reply, inputBuffer.length() + 1);
  Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
  Udp.write(reply, inputBuffer.length());
  Udp.endPacket();

  timeClient.begin();
  memset(matrixValue, 0, sizeof(matrixValue));

  randomSeed(micros());
//
//  Serial.println("CPU0 reset reason:");
//  verbose_print_reset_reason(rtc_get_reset_reason(0));
//
//  Serial.println("CPU1 reset reason:");
//  verbose_print_reset_reason(rtc_get_reset_reason(1));
}

void loop() {

//////////////////////////////////// add by lakec ////////////////////////////////////
	webSocket.loop();
  MDNS.update();
	server.handleClient();

	if(effect_changed == 1){
		String effect = jsonRead(configSetup, "effect");
		settChanged = true;
		currentMode = (byte)effect.toInt();
		loadingFlag = true;
		FastLED.clear();
		delay(1);
		sendCurrent();
		FastLED.setBrightness(modes[currentMode].brightness);
		effect_changed = 0;
    webSocketSendCurrentMode();
	}
	
	if(brig_changed == 1){
		String brig = jsonRead(configSetup, "volume");
		modes[currentMode].brightness = brig.toInt();
		FastLED.setBrightness(modes[currentMode].brightness);
		settChanged = true;
		eepromTimer = millis();
		brig_changed = 0;
    webSocketSendCurrentMode();
	}
	
	if(speed_changed == 1){
		String spd = jsonRead(configSetup, "speed");
		modes[currentMode].speed = spd.toInt();
		loadingFlag = true;
		settChanged = true;
		eepromTimer = millis();
		speed_changed = 0;
    webSocketSendCurrentMode();
	}	
	
	if(scale_changed == 1){
		String sca = jsonRead(configSetup, "scale");
		modes[currentMode].scale = sca.toInt();
		loadingFlag = true;
		settChanged = true;
		eepromTimer = millis();
		scale_changed = 0;
    webSocketSendCurrentMode();
	}
  if (turnOffTime && turnOffTime < millis()) {
    ONflag = false;
    changePower();
    turnOffTime = 0;	
  }
	//////////////////////////////////// add by lakec ////////////////////////////////////
	
  parseUDP();
  effectsTick();
  eepromTick();
  timeTick();
  buttonTick();
  yield();
}
