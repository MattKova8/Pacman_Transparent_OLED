#include <Arduino.h>
#include <SPI.h>
#include <U8g2lib.h>
#include <math.h>
#ifndef PI
  #define PI 3.14159265358979323846
#endif

U8G2_SSD1309_128X64_NONAME0_F_4W_HW_SPI u8g2(
  U8G2_R0, /* cs=*/10, /* dc=*/9, /* reset=*/8
);

// animation parameters
const uint8_t  FRAME_DELAY    = 20;    // ~50 FPS
const float    BASE_SPEED     = 2.0;   // px/frame on‐screen
const float    ENTRY_MULT     = 3.0;   // speed × while off‐screen
const int      RADIUS         = 8;     // Pac-Man radius
const int      CARVE_RAD      = RADIUS + 1;
const int      CY             = 32;    // vertical center
const uint8_t  NUM_DOTS       = 6;
const int      DOT_SPACING    = 16;
const int      MAX_MOUTH_ANG  = 90;    // 90° max

int   dotX[NUM_DOTS];
float pacX = -RADIUS;

void setup() {
  u8g2.begin();
  for (uint8_t i = 0; i < NUM_DOTS; i++) {
    dotX[i] = DOT_SPACING * (i + 2);  // 32,48,…112
  }
}

void loop() {
  float speed = pacX < 0 ? BASE_SPEED*ENTRY_MULT : BASE_SPEED;
  pacX += speed;
  if (pacX > u8g2.getDisplayWidth() + RADIUS) {
    pacX = -RADIUS;
    for (uint8_t i = 0; i < NUM_DOTS; i++)
      dotX[i] = DOT_SPACING * (i + 2);
  }

  u8g2.clearBuffer();
  drawDots();
  drawPacMan();
  u8g2.sendBuffer();
  delay(FRAME_DELAY);
}

void drawDots() {
  for (uint8_t i = 0; i < NUM_DOTS; i++) {
    if (dotX[i] > pacX + RADIUS) {
      u8g2.drawDisc(dotX[i], CY, 2);
    }
  }
}

void drawPacMan() {
  int cx = int(pacX);
  float minDist = DOT_SPACING;
  for (uint8_t i = 0; i < NUM_DOTS; i++) {
    float d = dotX[i] - pacX;
    if (d > 0 && d < minDist) minDist = d;
  }
  float angleDeg = minDist < DOT_SPACING
    ? MAX_MOUTH_ANG * sin((minDist/DOT_SPACING)*PI)
    : 0;
  int halfAng = int(angleDeg/2);

  u8g2.setDrawColor(1);
  u8g2.drawDisc(cx, CY, RADIUS);

  if (halfAng > 0) {
    u8g2.setDrawColor(0);
    for (int d=-halfAng; d<=halfAng; d++) {
      float th = d*(PI/180.0);
      int x2 = cx + int(cos(th)*CARVE_RAD);
      int y2 = CY + int(sin(th)*CARVE_RAD);
      u8g2.drawLine(cx, CY, x2, y2);
    }
    u8g2.setDrawColor(1);
  }

  for (uint8_t i = 0; i < NUM_DOTS; i++) {
    if (dotX[i] <= cx + RADIUS) dotX[i] = -100;
  }
}
