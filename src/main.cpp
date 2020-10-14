// "Bounce Ball" ('Breakout') created by Volos Projects.
// YouTube Channel: https://www.youtube.com/channel/UCit2rVgOvhyuAD-VH5H_IHg

// Video: ESP32 TTGO Display Board - Introduction and Free Game.
// https://www.youtube.com/watch?v=N6V7ZJkhSbc

// TTGO Diplay Board resolution: 135 x 240

// TTGO-TS v1.0 is // 132 x 160

// Modifications by @kosso. 13 Oct, 2020.

// Moved to PlatformIO IDE. 14 Oct, 2020

#define SCREEN_WIDTH 132
#define SCREEN_HEIGHT 160
#define PIN_LEFT 35
#define PIN_MID 34
#define PIN_RIGHT 39

#include "Speaker.h"
#include "bootsound.h"

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library
#include "bootlogo.h"
#define TFT_GREY 0x5AEB
#define lightblue 0x2D18
#define orange 0xFB60
#define purple 0xFB9B

float ys = 1;
float x = random(30, 100); // coordinates of ball
float y = 70;
int ny = y; // coordinates of previous position
int nx = x;

float px = 45; //67 je sredina (the middle)   pozicija igrača  (player position)
int pxn = px;
int vrije[2] = {1, -1};
int enx[16] = {8, 33, 58, 83, 108, 8, 33, 58, 83, 108, 22, 47, 72, 97, 47, 72};
int eny[16] = {37, 37, 37, 37, 37, 45, 45, 45, 45, 45, 53, 53, 53, 53, 61, 61};
int enc[16] = {TFT_RED, TFT_RED, TFT_RED, TFT_RED, TFT_RED, TFT_GREEN, TFT_GREEN, TFT_GREEN, TFT_GREEN, TFT_GREEN, orange, orange, orange, orange, lightblue, lightblue};
int score = 0;
int level = 1;
float amount[4] = {0.25, 0.50, 0.75, 1};
float xs = amount[random(4)] * vrije[random(2)];
int fase = 0;

float xstep = 1;
int spe = 0;
int pom = 0;
int gameSpeed = 20000;

void gameInit()
{
  // floats
  ys = 1;
  x = random(30, 100);
  y = 70;
  // ints
  ny = y;
  nx = x;
  px = 45; // player position
  pxn = px;
  vrije[0] = 1;
  vrije[1] = -1;
  // enemies
  int enx_[16] = {8, 33, 58, 83, 108, 8, 33, 58, 83, 108, 22, 47, 72, 97, 47, 72};
  int eny_[16] = {37, 37, 37, 37, 37, 45, 45, 45, 45, 45, 53, 53, 53, 53, 61, 61};
  int enc_[16] = {TFT_RED, TFT_RED, TFT_RED, TFT_RED, TFT_RED, TFT_GREEN, TFT_GREEN, TFT_GREEN, TFT_GREEN, TFT_GREEN, orange, orange, orange, orange, lightblue, lightblue};
  for (int n = 0; n < 16; n++)
  {
    enx[n] = enx_[n];
    eny[n] = eny_[n];
    enc[n] = enc_[n];
  }

  score = 0;
  level = 1;
  xs = amount[random(4)] * vrije[random(2)];
  fase = 0;

  SPEAKER().playMusic(startup_music, 25000);
}

void setup(void)
{
  Serial.begin(115200);
  Serial.print("");
  Serial.println(" ----- BREAKOUT ----- ");

  pinMode(PIN_RIGHT, INPUT); // right
  pinMode(PIN_LEFT, INPUT);  // left
  pinMode(PIN_MID, INPUT);   // middle

  SPEAKER().begin();

  tft.init();
  tft.setRotation(0);

  tft.setSwapBytes(true);
  tft.pushImage(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, bootlogo);

  SPEAKER().playMusic(startup_music, 25000);

}

void hitBatSound()
{
  SPEAKER().tone(250, 50);
  delay(50);
  SPEAKER().tone(0);
}

void hitEnemySound()
{
  SPEAKER().setBeep(150, 70);
  SPEAKER().beep();
  delay(70);
  SPEAKER().setBeep(0, 0);
  SPEAKER().beep();
}

void hitWallSound()
{
  SPEAKER().tone(50, 50);
  delay(50);
  SPEAKER().tone(0);
}

void loseLifeSound()
{
  SPEAKER().setBeep(150, 70);
  SPEAKER().beep();
  delay(70);
  SPEAKER().setBeep(100, 70);
  SPEAKER().beep();
  delay(70);
  SPEAKER().setBeep(50, 150);
  SPEAKER().beep();
  delay(150);
  SPEAKER().setBeep(0, 0);
  SPEAKER().beep();
}

void newLevel()
{
  score = score + 1;
  delay(3000);
  gameSpeed = gameSpeed - 500;
  level = level + 1;
  tft.setCursor(99, 2, 1);
  tft.println("LEV" + String(level));
  y = 75;
  ys = 1;
  x = random(30, 100);

  int enx2[16] = {8, 33, 58, 83, 108, 8, 33, 58, 83, 108, 22, 47, 72, 97, 47, 72};
  for (int n = 0; n < 16; n++)
  {
    enx[n] = enx2[n];
  }
}

void setupGameScreen()
{
  tft.fillScreen(TFT_BLACK);
  tft.drawLine(0, 17, 0, SCREEN_HEIGHT, TFT_GREY);
  tft.drawLine(0, 17, SCREEN_WIDTH, 17, TFT_GREY);
  tft.drawLine(SCREEN_WIDTH - 1, 17, SCREEN_WIDTH - 1, SCREEN_HEIGHT, TFT_GREY);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(1);

  tft.setCursor(3, 2, 1);
  tft.println("SCORE " + String(score));

  tft.setCursor(99, 2, 1);
  tft.println("LEV" + String(level));

}

void loop()
{
  if (fase == 0)
  {
    //if (digitalRead(PIN_LEFT) == 0 || digitalRead(PIN_RIGHT) == 0)
    if (digitalRead(PIN_MID) == 0)
    {
      if (pom == 0)
      {
        setupGameScreen();
        fase = fase + 1;
        pom = 1;
      }
    }
    else
    {
      pom = 0;
    }
  }

  if (fase == 1)
  {
    if (y != ny)
    {
      tft.fillEllipse(nx, ny, 2, 2, TFT_BLACK); // brisanje loptice  ('clear ball')
      ny = y;
      nx = x;
    }
    if (int(px) != pxn)
    {
      tft.fillRect(pxn, SCREEN_HEIGHT - 6, 24, 4, TFT_BLACK); // briasnje igraca  ('clear players')
      pxn = px;
    }

    // spe=spe+1;

    if (px >= 2 && px <= 109)
    {
      if (digitalRead(PIN_LEFT) == 0)
        px = px - 1;
      if (digitalRead(PIN_RIGHT) == 0)
        px = px + 1;
    }
    if (px <= 3)
      px = 4;

    if (px >= 108)
      px = 107;

    if (y > SCREEN_HEIGHT - 8 && x > px && x < px + 24)
    {
      // Hit player bat
      ys = ys * -1;
      xs = amount[random(4)] * vrije[random(2)];
      
      hitBatSound();
    }

    for (int j = 0; j < 16; j++)
    {
      // Hit enemy
      if (x > enx[j] && x < enx[j] + 20 && y > eny[j] && y < eny[j] + 5)
      {
        tft.fillRect(enx[j], eny[j], 20, 4, TFT_BLACK);
        enx[j] = 400;
        ys = ys * -1;
        xs = amount[random(4)] * vrije[random(2)];

        score = score + 1;

        tft.setCursor(3, 2, 1);
        tft.println("SCORE " + String(score));

        hitEnemySound();
      }
    }

    // Hit top
    if (y < 21)
    {
      Serial.println("Hit top");
      ys = ys * -1.00;
      hitWallSound();
    }

    // Off the bottom of the screen
    if (y > SCREEN_HEIGHT)
    {
      Serial.println("OH NO!");
      fase = fase + 1;
      loseLifeSound();
    }

    // Hit right side
    if (x >= SCREEN_WIDTH)
    {
      Serial.println("Hit right");
      xs = xs * -1.00;
      hitWallSound();
    }

    // Hit left side
    if (x <= 4)
    {
      Serial.println("Hit left");
      xs = xs * -1.00;
      hitWallSound();
    }

    for (int i = 0; i < 16; i++) // draw enemies
      tft.fillRect(enx[i], eny[i], 20, 4, enc[i]);

    tft.fillEllipse(int(x), y, 2, 2, TFT_WHITE); // draw ball

    //if(spe>10){  // change coordinates of ball
    y = y + ys;
    x = x + xs;
    //spe=0;
    //}

    tft.fillRect(px, SCREEN_HEIGHT - 6, 24, 4, TFT_WHITE);

    if (score == 16 || score == 33 || score == 50 || score == 67 || score == 84 || score == 101 || score == 118 || score == 135 || score == 152 || score == 169)
    {
      newLevel();
    }

    delayMicroseconds(gameSpeed);
  }
  if (fase == 2)
  {

    Serial.println(" ----- GAME OVER -----  ");

    tft.fillScreen(TFT_BLACK);

    tft.setCursor(13, 30, 2);

    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    //tft.setTextSize(1);

    tft.println("GAME OVER");

    tft.setCursor(13, 50, 1);
    tft.println("SCORE:" + String(score));

    tft.setCursor(13, 70, 1);
    tft.println("LEVEL:" + String(level));

    tft.setCursor(13, 90, 1);
    tft.println("SCORE:" + String(score));

    delay(300);

    if (digitalRead(PIN_MID) == 0)
    {

      gameInit();
      setupGameScreen();

    }
  }
}
