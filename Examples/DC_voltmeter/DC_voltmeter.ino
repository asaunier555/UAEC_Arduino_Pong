// include libaries for screen
#include <Adafruit_ST7735.h>

// Define pins for LCD
#define TFT_CS 10
#define TFT_RST 8
#define TFT_DC 9

// init pins for LCD
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Declare global variables
int i = 0;
float voltage = 0;
float voltageAvg = 0;

void setup()
{
  Serial.begin(9600);
  Serial.print("Voltmeter: ");

  // init LCD
  tft.initR(INITR_BLACKTAB);

  Serial.println(F("Initalized"));

  // set up pin as input
  pinMode(A0, INPUT);
}

void loop()
{

  takeVoltageAverage();
  printVoltage();
}

void takeVoltageAverage()
{
  i = 0;
  voltage = 0;
  voltageAvg = 0;

  // Loop to take voltage
  while (i < 100)
  {
    voltage = 11.3 * 5 * analogRead(A0) / 1023.0;
    voltageAvg = voltageAvg + voltage;
    i++;
  }

  // Calc average
  voltageAvg = voltageAvg / i;

  //  Print Result to monitor
  Serial.println(voltageAvg);
}

void printVoltage()
{

  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 25);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(5);
  tft.println(voltage, 2);
  tft.print("VDC");
  delay(500);
}
