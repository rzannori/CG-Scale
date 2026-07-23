/*
   CG scale - versione corretta per Arduino Nano ESP32
   CG Scale main components:
   1 pc load sensor front YZC-133
   1 pc load sensor rear YZC-133
   2 pc HX711 ADC, one for each load sensor (128bit resolution)
   1 pc Arduino Nano ESP32
   1 pc OLED display SSD1306 0.96" I2C

   MODIFICHE rispetto all'originale:
   - Display sostituito da LiquidCrystal_I2C (LCD HD44780) a Adafruit_SSD1306 (OLED),
     perché l'hardware collegato è un OLED I2C, non un LCD a caratteri.
   - Risolto conflitto di pin: ledPin era 7, uguale al pin SCK di LoadCell_2.
     ledPin ora è 8.
   - SDA/SCL sul Nano ESP32 di default sono A4/A5 (Wire.begin() senza argomenti li usa già).
*/

#include <HX711_ADC.h> //https://github.com/olkal/HX711_ADC

//HX711 constructor (dout pin, sck pin):
HX711_ADC LoadCell_1(5, 4); //HX711 pins front sensor (DOUT, PD_SCK)
HX711_ADC LoadCell_2(7, 6); //HX711 pins rear sensor (DOUT, PD_SCK)

#include <Wire.h>

// OLED I2C display
#define I2CDISP
#ifdef I2CDISP
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C // se non funziona, prova 0x3D
Adafruit_SSD1306 lcd(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Logo di avvio (128x30 pixel, monocromatico)
#define LOGO_WIDTH 128
#define LOGO_HEIGHT 30
const unsigned char PROGMEM logo_bmp[] = {
  0x00, 0x07, 0xFF, 0xFF, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x01,
  0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x07, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x7F, 0xC0, 0x00, 0xFF, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x80, 0x01, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x00,
  0x07, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x0F, 0xFC, 0x00, 0x3F, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x3F, 0xF8, 0x01, 0xFF, 0xE0, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 0xF0, 0x1F,
  0xFF, 0x00, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x01, 0xFF, 0xC0, 0xFF, 0xF0, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0x81, 0xFF, 0x00, 0x00, 0x07, 0xC0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0xFF, 0x01, 0xFF,
  0xC0, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x3F, 0xFC, 0x00, 0xFF, 0xFF, 0xFF, 0x80, 0x03, 0xF0, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xFF, 0xF8, 0x00, 0x3F, 0xFF, 0xFF, 0xFE, 0x00,
  0x1F, 0xC7, 0xFE, 0x00, 0x00, 0x00, 0x00, 0x01, 0x07, 0xF0, 0x00, 0x00,
  0x2F, 0xFF, 0xFF, 0x00, 0x00, 0xFF, 0x70, 0x00, 0x00, 0x00, 0x00, 0x03,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xFE, 0x00, 0x00, 0x07, 0xFC, 0x03,
  0xF0, 0x00, 0x3F, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x87, 0xFC, 0x00,
  0x00, 0x00, 0x3F, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x30, 0x3F, 0xFC, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xC0, 0x00, 0x00, 0x00, 0x38, 0x7F,
  0xF8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1F, 0xFF, 0x00, 0x00,
  0x00, 0x00, 0x3F, 0xF1, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00, 0xF8, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x03, 0xFF, 0xC7, 0x00, 0x00, 0x00, 0x1F, 0x80, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xFF, 0x00, 0x00, 0x00,
  0x0F, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0F,
  0xFE, 0x00, 0x00, 0xFF, 0xFC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x0F, 0xFF, 0xFF, 0xFF, 0xFF, 0x80, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0xFF, 0xFF, 0xE0,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x3F, 0xFF, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
#endif

byte ledPin = 8; // CAMBIATO da 7 a 8 per evitare conflitto con LoadCell_2 (pin 7)
char toLCD[20];
byte output;
boolean ledState;
long t1;
long t2;

const int printInterval = 500; // display/Serial refresh interval

//*** configuration:
//*** set dimensional calibration values:
const long WingPegDist = 1210; //calibration value in 1/10mm
const long LEstopperDist = 300; //calibration value 1/10mm
//*** set scale calibration values:
const float ldcell_1_calfactor = 462.55
; // user set calibration factor load cell front
const float ldcell_2_calfactor =  440.25; // user set calibration factor load cell rear
//***
const long stabilisingtime = 3000; // tare precision improves with a few seconds of stabilising time
//***
const long CGoffset = ((WingPegDist / 2) + LEstopperDist) * 10;

void setup() {
  //***
  // 0: Serial-Console (Your PC);
  // 1: Serial-LCD (2nd Arduino with SimpleSerialDisplay)
  // 2: I2C-OLED (define I2CDISP above!)
  output = 2;
  //***

  Serial.begin(9600);
  delay(500);
  Serial.println("F3X COG scale");

  if (output == 0) {
    Serial.println();
    Serial.println("Wait for stabilising and tare...");
  }

#ifdef I2CDISP
  if (output != 2)
    Serial.println("You have I2CDISP defined, but output is not 2. Are you sure thats right?");

  Serial.println("Initializing I2C OLED display");
  Wire.begin(); // Nano ESP32: SDA=A4, SCL=A5 di default

  if (!lcd.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED non trovato! Controlla indirizzo I2C e collegamenti.");
  } else {
    lcd.setRotation(2); // ruota lo schermo di 180 gradi

    // Splash screen: logo per 3-4 secondi
    lcd.clearDisplay();
    lcd.drawBitmap((SCREEN_WIDTH - LOGO_WIDTH) / 2, (SCREEN_HEIGHT - LOGO_HEIGHT) / 2,
                    logo_bmp, LOGO_WIDTH, LOGO_HEIGHT, SSD1306_WHITE);
    lcd.display();
    delay(3500);

    // Schermata di benvenuto, testo ingrandito (size 2)
    lcd.clearDisplay();
    lcd.setTextSize(2);
    lcd.setTextColor(SSD1306_WHITE);
    lcd.setCursor(0, 0);
    lcd.println("F3X COG");
    lcd.println("scale");
   // lcd.setCursor(0, 32);
    //lcd.println("at your");
    //lcd.println("service!");
    lcd.display();
    delay(1500);
  }
#endif

  // Messaggio di attesa per tare/stabilizzazione (su seriale sempre, su OLED se disponibile)
  Serial.println("Wait for stabilising and tare...");
#ifdef I2CDISP
  lcd.clearDisplay();
  lcd.setTextSize(2);
  lcd.setTextColor(SSD1306_WHITE);
  lcd.setCursor(0, 16);
  lcd.println("Taring...");
 // lcd.setCursor(0, 40);
  //lcd.println("wait");
  lcd.display();
#endif

  LoadCell_1.begin();
  LoadCell_2.begin();
  byte loadcell_1_rdy = 0;
  byte loadcell_2_rdy = 0;
  while ((loadcell_1_rdy + loadcell_2_rdy) < 2) { //run startup, stabilisation and tare, both modules simultaneously
    if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilisingtime);
    if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilisingtime);
  }
  Serial.println("Tare completato!");
  LoadCell_1.setCalFactor(ldcell_1_calfactor);
  LoadCell_2.setCalFactor(ldcell_2_calfactor);

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, HIGH);

  if (output == 1) {
    Serial.write(254);
    Serial.write(127); // clear lcd
  }
}

void flashLED() {
  if (t2 < millis()) {
    if (ledState) {
      t2 = millis() + 2000;
      ledState = 0;
    }
    else {
      t2 = millis() + 100;
      ledState = 1;
    }
    digitalWrite(ledPin, ledState);
  }
}

void loop() {
  //library function update() should be called at least as often as HX711 sample rate
  LoadCell_1.update();
  LoadCell_2.update();

  // calculate CG and update serial/display
  if (t1 < millis()) {
    t1 = millis() + printInterval;
    float a = LoadCell_1.getData();
    float b = LoadCell_2.getData();
    long weightAvr[3];
    float CGratio;
    long CG;
    weightAvr[0] = a * 100;
    weightAvr[1] = b * 100;
    long weightTot = weightAvr[0] + weightAvr[1];

    if (weightAvr[0] > 500 && weightAvr[1] > 500) {
      long a2 = weightAvr[1] / 10;
      long b2 = weightAvr[0] / 10;
      CGratio = (((a2 * 10000) / (a2 + b2)));
      CG = ((((WingPegDist) * CGratio) / 1000) - ((WingPegDist * 10) / 2) + CGoffset);
    }
    else {
      CG = 0;
    }

    if (output == 0) {
      // print result to serial terminal
      for (byte a3 = 0; a3 < 2; a3++) {
        Serial.print("weight_LdCell_");
        Serial.print(a3 + 1);
        Serial.print(": ");
        long i = weightAvr[a3];
        if (i < 0) {
          Serial.print('-');
          i = ~weightAvr[a3];
        }
        Serial.print(i / 100);
        Serial.print('.');
        if ((i % 100) < 10) {
          Serial.print("0");
        }
        Serial.print(i % 100);
        Serial.print("      ");
      }
      Serial.print("CG:");
      Serial.print(CG / 100);
      Serial.print('.');
      Serial.println(CG % 100);
    }
    else if (output == 1) {
      // print to serial LCD (2nd Arduino) - invariato
      toLCD[0] = 254;
      toLCD[1] = 192;
      toLCD[2] = 'W';
      toLCD[3] = 't';
      toLCD[4] = ':';
      if (weightTot < 0 && weightTot >= -100) {
        weightTot = 0;
      }
      if (weightTot < -100) {
        toLCD[5] = 'E';
        toLCD[6] = 'r';
        toLCD[7] = 'r';
        toLCD[8] = '.';
      }
      else {
        toLCD[5] = (char)(weightTot / 100000) + 48;
        toLCD[6] = (char)((weightTot % 100000) / 10000) + 48;
        toLCD[7] = (char)((weightTot % 10000) / 1000) + 48;
        toLCD[8] = (char)((weightTot % 1000) / 100) + 48;
      }
      toLCD[9] = ' ';
      if (CG != 0) {
        toLCD[10] = 'C';
        toLCD[11] = 'G';
        toLCD[12] = ':';
        toLCD[13] = (char)(CG / 10000) + 48;
        toLCD[14] = (char)((CG % 10000) / 1000) + 48;
        toLCD[15] = (char)((CG % 1000) / 100) + 48;
        toLCD[16] = '.';
        toLCD[17] = (char)((CG % 100) / 10) + 48;
      }
      else {
        for (byte s = 10; s < 18; s++) {
          toLCD[s] = ' ';
        }
      }
      toLCD[18] = ';';
      for (byte i = 0; i < sizeof(toLCD) - 1; i++) {
        Serial.write(toLCD[i]);
      }
    }
    else if (output == 2) {
      // print to I2C OLED display
#ifdef I2CDISP
      lcd.clearDisplay();
      lcd.setTextSize(2); // testo piu' grande per leggibilita'
      lcd.setTextColor(SSD1306_WHITE);

      // 1st line: Weight
      lcd.setCursor(0, 0);
      if (weightTot < 0 && weightTot >= -100)
        weightTot = 0;
      if (weightTot < -100) {
        lcd.print("Wt:Err!");
      } else {
        sprintf(toLCD, "Wt:%ldg", weightTot / 100);
        lcd.print(toLCD);
      }

      // 2nd line: CG
      lcd.setCursor(0, 32); // 32px sotto: seconda "riga" a dimensione 2 (16px di altezza carattere x2)
      if (CG == 0) {
        lcd.print("CG:wait");
      } else {
        sprintf(toLCD, "CG:%ld.%ld", CG / 100, CG % 100);
        lcd.print(toLCD);
      }

      lcd.display();
#endif
    }
  }
  flashLED();
}
