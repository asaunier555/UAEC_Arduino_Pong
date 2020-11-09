#include <Adafruit_ST7735.h>

// Define pins for LCD
#define TFT_CS 10
#define TFT_RST 8
#define TFT_DC 9

// init pins for LCD
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Set up Joystick
float jStick = 0;

// Color definitions
#define BLACK 0x0000
#define BLUE 0x001F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF

//  Define const for game physics
const int PADDLE_RATE = 1;
const int PADDLE__REFRESH_RATE = 20;
const int BALL_RATE = 20;
const int PADDLE_HEIGHT = 35;
int MAX_SCORE = 10;

// init scores
int CPU_SCORE = 0;
int PLAYER_SCORE = 0;

// init Ball
int ball_x = 64, ball_y = 32;
int ball_dir_x = 1, ball_dir_y = 1;

// init game states
bool gameIsRunning = true;
bool resetBall = false;
bool TIMEOUT = false;

int ball_update;
int paddle_update;

// spawn Locations
const int CPU_X = 12;
int cpu_y = 32;
const int PLAYER_X = 148;
int player_y = 32;

void setup()
{

  // Set up Joystick
  pinMode(A0, INPUT);

  // init LCD
  tft.initR(INITR_BLACKTAB);
  tft.setRotation(1);

  // Init screen
  tft.fillScreen(BLACK);
  tft.setCursor(0, 25);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(5);
  tft.println("UAEC");
  tft.println("PONG");

  int timeoutcount = 0;
  // wait for game to start
  while (readStick() > 400 && readStick() < 600)
  {
    delay(100);
    timeoutcount += 100;
    if(timeoutcount == 5000){
      
      TIMEOUT = true;
      break;
    }
  }
  int start = millis();

  // black screen
  tft.fillScreen(BLACK);
  // draw court
  drawCourt();

  // start ball in rand location
  while (millis() - start < 2000)
    ;
  ball_update = millis();
  paddle_update = ball_update;
  ball_x = random(25, 135);
  ball_y = random(3, 125);
}

void loop()
{

  // clear last movement
  int time = millis();
  bool up_state = false;
  bool down_state = false;

  // read joystick
  up_state |= (readStick() < 400);
  down_state |= (readStick() > 600);

  if (resetBall)
  {
    ball_x = random(25, 135);
    ball_y = random(3, 125);
    do
    {
      ball_dir_x = random(-1, 2);
    } while (ball_dir_x == 0);

    do
    {
      ball_dir_y = random(-1, 2);
    } while (ball_dir_y == 0);

    resetBall = false;
  }

  if (time > ball_update && gameIsRunning)
  {
    int new_x = ball_x + ball_dir_x;
    int new_y = ball_y + ball_dir_y;

    // Check if we hit the vertical walls
    if (new_x == 0) //Player Gets a Point
    {
      PLAYER_SCORE++;
      if (PLAYER_SCORE == MAX_SCORE)
      {
        gameOver();
      }
      else
      {
        showScore();
      }
    }

    // Check if we hit the vertical walls
    if (new_x == 160) //CPU Gets a Point
    {
      CPU_SCORE++;
      if (CPU_SCORE == MAX_SCORE)
      {
        gameOver();
      }
      else
      {
        showScore();
      }
    }

    // Check if we hit the horizontal walls.
    if (new_y == 0 || new_y == 128)
    {
      ball_dir_y = -ball_dir_y;
      new_y += ball_dir_y + ball_dir_y;
    }

    // Check if we hit the CPU paddle
    if (new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    // Check if we hit the player paddle
    if (new_x == PLAYER_X && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += ball_dir_x + ball_dir_x;
    }

    tft.drawPixel(ball_x, ball_y, BLACK);
    tft.drawPixel(new_x, new_y, WHITE);
    ball_x = new_x;
    ball_y = new_y;

    ball_update += BALL_RATE;
  }

  if (time > paddle_update && gameIsRunning)
  {
    paddle_update += PADDLE__REFRESH_RATE;

    // CPU1 paddle
    tft.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, BLACK);
    const int half_paddle = PADDLE_HEIGHT >> 1;
    if (cpu_y + half_paddle > ball_y)
    {
      cpu_y -= PADDLE_RATE;
    }
    if (cpu_y + half_paddle < ball_y)
    {
      cpu_y += PADDLE_RATE;
    }
    if (cpu_y < 1)
      cpu_y = 1;
    if (cpu_y + PADDLE_HEIGHT > 128)
      cpu_y = 128 - PADDLE_HEIGHT;
    tft.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, BLUE);

    // Player paddle
    if (!TIMEOUT)
    {
      tft.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
      if (up_state)
      {
        player_y -= PADDLE_RATE;
      }
      if (down_state)
      {
        player_y += PADDLE_RATE;
      }
      up_state = down_state = false;
      if (player_y < 1)
        player_y = 1;
      if (player_y + PADDLE_HEIGHT > 128)
        player_y = 128 - PADDLE_HEIGHT;
      tft.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, YELLOW);
    }
    // CPU2 Only if splash screen times out
    if (TIMEOUT)
    {
      tft.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
      const int half_paddle = PADDLE_HEIGHT >> 1;
      if (player_y + half_paddle > ball_y)
      {
        player_y -= PADDLE_RATE;
      }
      if (player_y + half_paddle < ball_y)
      {
        player_y += PADDLE_RATE;
      }
      if (player_y < 1)
        player_y = 1;
      if (player_y + PADDLE_HEIGHT > 128)
        player_y = 128 - PADDLE_HEIGHT;
      tft.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, YELLOW);
    }
  }
}

void drawCourt()
{
  tft.drawRect(0, 0, 160, 128, WHITE);
}

void gameOver()
{
  gameIsRunning = false;
  tft.fillScreen(BLACK);
  drawCourt();
  if (PLAYER_SCORE > CPU_SCORE)
  {
    tft.setCursor(5, 4);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("You Won");
  }
  else
  {
    tft.setCursor(5, 4);
    tft.setTextColor(WHITE);
    tft.setTextSize(2);
    tft.print("CPU WON");
  }

  tft.setCursor(20, 30);
  tft.setTextColor(BLUE);
  tft.setTextSize(3);
  tft.print(String(CPU_SCORE));

  tft.setCursor(60, 30);
  tft.setTextColor(YELLOW);
  tft.setTextSize(3);
  tft.print(String(PLAYER_SCORE));

  delay(2000);

  while (readStick() > 400 && readStick() < 600)
  {
    delay(100);
  }
  gameIsRunning = true;

  CPU_SCORE = PLAYER_SCORE = 0;

  int start = millis();
  tft.fillScreen(BLACK);
  drawCourt();
  while (millis() - start < 2000)
    ;
  ball_update = millis();
  paddle_update = ball_update;
  gameIsRunning = true;
  resetBall = true;
}

void showScore()
{

  // clear screen
  gameIsRunning = false;
  tft.fillScreen(BLACK);
  drawCourt();

  // draw text
  tft.setCursor(32, 4);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("Score");

  tft.setCursor(40, 30);
  tft.setTextColor(BLUE);
  tft.setTextSize(4);
  tft.print(String(CPU_SCORE));

  tft.setCursor(80, 30);
  tft.setTextColor(YELLOW);
  tft.setTextSize(4);
  tft.print(String(PLAYER_SCORE));

  delay(2000);
  int start = millis();

  tft.fillScreen(BLACK);
  drawCourt();
  while (millis() - start < 2000)
    ;
  ball_update = millis();
  paddle_update = ball_update;
  gameIsRunning = true;
  resetBall = true;
}

float readStick()
{
  return analogRead(A0);
}
