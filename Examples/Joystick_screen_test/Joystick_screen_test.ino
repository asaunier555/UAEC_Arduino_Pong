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
float stickX = 0;
float stickY = 0;

void setup()
{
  Serial.begin(9600);
  Serial.print("JOYSTICK: ");

  // init LCD
  tft.initR(INITR_BLACKTAB);

  Serial.println(F("Initalized"));

  // set up pin as input
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
}

void loop()
{
  readStick();
  printStick();
}

void readStick()
{
 
 stickX = analogRead(A1);
 stickY = analogRead(A0);
  
  //  Print Result to monitor
  Serial.println(stickX);
  Serial.println(stickY);
}

void printStick()
{

  tft.setRotation(1);
  tft.setTextWrap(false);
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 25);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(5);
  tft.println(stickX, 2);
  tft.println(stickY, 2);
  delay(500);
}
