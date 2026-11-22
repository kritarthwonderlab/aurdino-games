
#include <MCUFRIEND_kbv.h>
#include <Adafruit_GFX.h>

MCUFRIEND_kbv tft;

// =====================================
// HARDWARE - SAME AS YOUR ORIGINAL CODE
// =====================================

#define JOY_SW 10

#define SCREEN_W 480
#define SCREEN_H 320

// =====================================
// GOOGLE DINOSAUR COLORS
// =====================================

#define BLACK 0x0000
#define WHITE 0xFFFF

// =====================================
// GAME SETTINGS
// =====================================

const int groundY = 260;
const int dinoX = 80;

const unsigned long FRAME_TIME = 35;

int dinoY = 0;
int velocity = 0;

int cactusX = 520;
int speed = 6;

int score = 0;
int highScore = 0;

bool jumping = false;
bool started = false;
bool gameOver = false;
bool legFrame = false;

int oldDinoY = 0;
int oldCactusX = 520;

unsigned long lastFrame = 0;
unsigned long lastAnimation = 0;

int groundMarks[12] = {
  10, 55, 100, 145, 190, 235,
  280, 325, 370, 415, 460, 490
};

// =====================================
// BUTTON DEBOUNCE
// =====================================

bool stableButton = HIGH;
bool lastReading = HIGH;
unsigned long buttonTimer = 0;

bool buttonPressed() {

  bool reading = digitalRead(JOY_SW);

  if (reading != lastReading) {
    lastReading = reading;
    buttonTimer = millis();
  }

  if (millis() - buttonTimer > 25) {

    if (reading != stableButton) {

      stableButton = reading;

      if (stableButton == LOW) {
        return true;
      }
    }
  }

  return false;
}

// =====================================
// BACKGROUND
// =====================================

void drawBackground() {

  tft.fillScreen(WHITE);

  // Main ground line
  tft.drawFastHLine(
    0, groundY, SCREEN_W, BLACK
  );

  // Ground texture
  for (int i = 0; i < 12; i++) {

    tft.fillRect(
      groundMarks[i], 278 + (i % 2) * 8,
      5, 2, BLACK
    );
  }
}

// =====================================
// DINOSAUR - PIXEL STYLE
// =====================================

void drawDino(int x, int jump, bool frame) {

  int y = groundY - 55 + jump;

  // Tail
  tft.fillRect(
    x, y + 29, 12, 8, BLACK
  );

  tft.fillRect(
    x + 5, y + 25, 8, 8, BLACK
  );

  // Body
  tft.fillRect(
    x + 12, y + 24, 29, 20, BLACK
  );

  // Neck
  tft.fillRect(
    x + 34, y + 10, 12, 24, BLACK
  );

  // Head
  tft.fillRect(
    x + 34, y, 25, 20, BLACK
  );

  // Nose
  tft.fillRect(
    x + 52, y + 8, 10, 8, BLACK
  );

  // Eye
  tft.drawPixel(
    x + 49, y + 5, WHITE
  );

  // Mouth
  tft.drawFastHLine(
    x + 54, y + 17, 7, WHITE
  );

  // Legs animation
  if (frame) {

    tft.fillRect(
      x + 16, y + 43, 8, 12, BLACK
    );

    tft.fillRect(
      x + 34, y + 43, 8, 8, BLACK
    );

  } else {

    tft.fillRect(
      x + 16, y + 43, 8, 8, BLACK
    );

    tft.fillRect(
      x + 34, y + 43, 8, 12, BLACK
    );
  }

  // Feet
  if (frame) {

    tft.fillRect(
      x + 12, y + 53, 12, 3, BLACK
    );

    tft.fillRect(
      x + 34, y + 49, 12, 3, BLACK
    );

  } else {

    tft.fillRect(
      x + 12, y + 49, 12, 3, BLACK
    );

    tft.fillRect(
      x + 34, y + 53, 12, 3, BLACK
    );
  }
}

// =====================================
// ERASE DINOSAUR
// =====================================

void eraseDino(int jump) {

  int y = groundY - 60 + jump;

  tft.fillRect(
    dinoX - 3, y,
    68, 63, WHITE
  );
}

// =====================================
// CACTUS
// =====================================

void drawCactus(int x) {

  int top = groundY - 60;

  // Main stem
  tft.fillRect(
    x, top, 12, 60, BLACK
  );

  // Rounded top
  tft.fillCircle(
    x + 6, top, 6, BLACK
  );

  // Left branch
  tft.fillRect(
    x - 10, top + 24, 10, 7, BLACK
  );

  tft.fillRect(
    x - 10, top + 12, 7, 19, BLACK
  );

  tft.fillCircle(
    x - 7, top + 12, 4, BLACK
  );

  // Right branch
  tft.fillRect(
    x + 12, top + 34, 10, 7, BLACK
  );

  tft.fillRect(
    x + 18, top + 20, 7, 21, BLACK
  );

  tft.fillCircle(
    x + 21, top + 20, 4, BLACK
  );
}

// =====================================
// ERASE CACTUS
// =====================================

void eraseCactus(int x) {

  if (x < -40 || x > SCREEN_W) return;

  tft.fillRect(
    x - 14, groundY - 68,
    45, 68, WHITE
  );
}

// =====================================
// GROUND ANIMATION
// =====================================

void updateGround() {

  for (int i = 0; i < 12; i++) {

    // Erase old mark
    tft.fillRect(
      groundMarks[i],
      278 + (i % 2) * 8,
      5, 2, WHITE
    );

    // Move
    groundMarks[i] -= speed;

    if (groundMarks[i] < -5) {
      groundMarks[i] = SCREEN_W;
    }

    // Draw new mark
    tft.fillRect(
      groundMarks[i],
      278 + (i % 2) * 8,
      5, 2, BLACK
    );
  }
}

// =====================================
// SCORE
// =====================================

void drawScore() {

  tft.fillRect(
    285, 5, 190, 32, WHITE
  );

  tft.setTextColor(BLACK);
  tft.setTextSize(2);
  tft.setTextWrap(false);

  tft.setCursor(290, 12);
  tft.print("HI ");
  
  if (highScore < 1000) tft.print("0");
  if (highScore < 100) tft.print("0");
  if (highScore < 10) tft.print("0");

  tft.print(highScore);

  tft.setCursor(400, 12);
  tft.print(" ");
  if (score < 1000) tft.print("0");
  if (score < 100) tft.print("0");
  if (score < 10) tft.print("0");
  tft.print(score);
}

// =====================================
// START SCREEN
// =====================================

void showStartScreen() {

  drawBackground();

  tft.setTextColor(BLACK);
  tft.setTextSize(4);

  tft.setCursor(125, 105);
  tft.print("DINO RUN");

  tft.setTextSize(2);

  tft.setCursor(105, 160);
  tft.print("PRESS SW TO START");

  tft.setTextSize(1);
  tft.setCursor(165, 195);
  tft.print("GOOGLE STYLE RUNNER");

  drawDino(dinoX, 0, false);
}

// =====================================
// GAME OVER
// =====================================

void showGameOver() {

  tft.fillRect(
    75, 95, 330, 120, WHITE
  );

  tft.drawRect(
    75, 95, 330, 120, BLACK
  );

  tft.setTextColor(BLACK);
  tft.setTextSize(3);

  tft.setCursor(125, 112);
  tft.print("GAME OVER");

  tft.setTextSize(2);

  tft.setCursor(140, 155);
  tft.print("SCORE: ");
  tft.print(score);

  tft.setTextSize(1);

  tft.setCursor(145, 190);
  tft.print("PRESS SW TO RESTART");
}

// =====================================
// RESET GAME
// =====================================

void resetGame() {

  score = 0;
  speed = 6;

  cactusX = 520;

  dinoY = 0;
  velocity = 0;

  oldDinoY = 0;
  oldCactusX = cactusX;

  jumping = false;
  gameOver = false;

  legFrame = false;

  for (int i = 0; i < 12; i++) {
    groundMarks[i] = i * 45 + 10;
  }

  drawBackground();
  drawScore();
  drawDino(dinoX, 0, false);
  drawCactus(cactusX);
}

// =====================================
// COLLISION DETECTION
// =====================================

bool checkCollision() {

  int dinoLeft = dinoX + 8;
  int dinoRight = dinoX + 60;

  int dinoTop = groundY - 53 + dinoY;
  int dinoBottom = groundY + dinoY;

  int cactusLeft = cactusX - 8;
  int cactusRight = cactusX + 22;
  int cactusTop = groundY - 60;

  return (
    dinoRight > cactusLeft &&
    dinoLeft < cactusRight &&
    dinoBottom > cactusTop &&
    dinoTop < groundY
  );
}

// =====================================
// SETUP
// =====================================

void setup() {

  Serial.begin(9600);

  pinMode(JOY_SW, INPUT_PULLUP);

  uint16_t ID = tft.readID();

tft.setRotation(1);

// Try maximum display brightness
analogWrite(10, 255);
  Serial.print("TFT ID: 0x");
  Serial.println(ID, HEX);

  if (ID == 0xD3D3 ||
      ID == 0xFFFF ||
      ID == 0x0000) {

    ID = 0x9486;
  }

  tft.begin(ID);
  tft.setRotation(1);

  tft.setTextWrap(false);

  randomSeed(millis());

  showStartScreen();
}

// =====================================
// MAIN GAME LOOP
// =====================================

void loop() {

  bool pressed = buttonPressed();

  // Start
  if (!started) {

    if (pressed) {

      started = true;
      resetGame();

      lastFrame = millis();
    }

    return;
  }

  // Restart
  if (gameOver) {

    if (pressed) {

      resetGame();
      lastFrame = millis();
    }

    return;
  }

  // Frame timing
  if (millis() - lastFrame < FRAME_TIME) {
    return;
  }

  lastFrame = millis();

  // Jump
  if (pressed && !jumping) {

    velocity = -15;
    jumping = true;
  }

  // Save old positions
  oldDinoY = dinoY;
  oldCactusX = cactusX;

  // Jump physics
  if (jumping) {

    dinoY += velocity;
    velocity++;

    if (dinoY >= 0) {

      dinoY = 0;
      velocity = 0;

      jumping = false;
    }
  }

  // Move cactus
  cactusX -= speed;

  if (cactusX < -35) {

    cactusX = 520 + random(0, 120);

    score++;

    // Increase speed gradually
    if (score % 5 == 0 && speed < 13) {
      speed++;
    }
  }

  // Collision
  if (checkCollision()) {

    gameOver = true;

    if (score > highScore) {
      highScore = score;
    }

    showGameOver();
    return;
  }

  // Erase previous objects
  eraseDino(oldDinoY);
  eraseCactus(oldCactusX);

  // Animate legs
  if (!jumping &&
      millis() - lastAnimation > 110) {

    lastAnimation = millis();
    legFrame = !legFrame;
  }

  // Draw new objects
  drawDino(dinoX, dinoY, legFrame);
  drawCactus(cactusX);

  // Ground and score
  updateGround();

  tft.drawFastHLine(
    0, groundY, SCREEN_W, BLACK
  );

  drawScore();
}
