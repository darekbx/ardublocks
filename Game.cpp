#include <Arduboy2.h>
#include "Game.h"

#define TOP_OFFSET 0
#define LEFT_OFFSET 44
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 15
#define TOP_SCORE_ADDRESS 0

void Game::resetGame() {
  isGameEnd = false;
  isGameRunning = false;
  score = 0;
  level = 1;
}

void Game::startGame() {
  fillBoard();
  addShape();
  nextShapeIndex = random(0, 7);
  isGameRunning = true;
  isGameEnd = false;
}

bool Game::getIsGameRunning() {
  return isGameRunning;
}

void Game::setIsGameRunning(bool isRunning) {
  isGameRunning = isRunning;
}

bool Game::getIsGameEnd() {
  return isGameEnd;
}

int Game::getScore() {
  return score;
}

int Game::getBestScore() {
  return readValue(TOP_SCORE_ADDRESS);
}

int Game::getLevel() {
  return level;
}

void Game::pauseGame() {
  isPaused = !isPaused;
}

void Game::addShape() {
  cShapeX = 4; // Default start position
  cShapeY = 0;
  isCShapeMoving = true;

  if (nextShapeIndex == -1) {
    nextShapeIndex = random(0, 7);
  }
  prepareShape(cShape, nextShapeIndex);
  nextShapeIndex = random(0, 7);
}

void Game::prepareShape(byte destination[3][3], byte index) {
  switch (index) {
    case 0: memcpy(destination, shape_1, sizeof(shape_1)); break;
    case 1: memcpy(destination, shape_2, sizeof(shape_2)); break;
    case 2: memcpy(destination, shape_3, sizeof(shape_3)); break;
    case 3: memcpy(destination, shape_4, sizeof(shape_4)); break;
    case 4: memcpy(destination, shape_5, sizeof(shape_5)); break;
    case 5: memcpy(destination, shape_6, sizeof(shape_6)); break;
    case 6: memcpy(destination, shape_7, sizeof(shape_7)); break;
  }
}

void Game::dropLines() {
  bool wasLineDropped = false;
  byte droppedLinesCount = 0;
  
  do {
    wasLineDropped = false;
    for (int y = BOARD_HEIGHT - 1; y >= 0; y--) {
      // Check for filled lines
      byte filledCount = 0;
      for (int x = 0; x < BOARD_WIDTH; x++) {
        if (board[y][x] == 1) {
          filledCount++;
        }
      }
      // Found filled line
      if (filledCount == BOARD_WIDTH) {
        // Drop line
        for (int x = 0; x < BOARD_WIDTH; x++) {
          board[y][x] = 0;
        }
        // Move top blocks down
        for (int yi = y - 1; yi >= 0; yi--) {
          for (int x = 0; x < BOARD_WIDTH; x++) {   
            board[yi + 1][x] = board[yi][x];
          }
        }
        droppedLinesCount++;
        wasLineDropped = true;
        break;
      }
    }
  } while (wasLineDropped);
  
  if (droppedLinesCount > 0) {
    switch (droppedLinesCount) {
      case 1: score += ONE_LINE_POINTS; break;
      case 2: score += TWO_LINES_POINTS; break;
      case 3: score += THREE_LINE_POINTS; break;
    }
    increaseLevel();
  }
}

void Game::increaseLevel() {
  if (score > 0 && score >= level * 1000) {
    level++;
  }
}

void Game::collisionDetection() {
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 3; x++) {
      if (cShapeY + y + 1 > BOARD_HEIGHT) continue;
      if (cShape[y][x] == 1 && board[cShapeY + y + 1][cShapeX + x] == 1) {

        if (cShapeY == 0) {
          isGameEnd = true;
          if (score > getBestScore()) {
            saveValue(TOP_SCORE_ADDRESS, score);
          }
          return;
        }
 
        newShape();
      }
    }
  }
}

bool Game::canMoveLeft() {
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {
      if (cShape[y][x] == 1 && board[cShapeY + y][cShapeX + x - 1] == 1) {
        return false;
      }
    }
  }
  return true;
}

bool Game::canMoveRight() {
  for (int y = 0; y < 3; y++) {
    for (int x = 0; x < 3; x++) {
      if (cShape[y][x] == 1 && board[cShapeY + y][cShapeX + x + 1] == 1) {
        return false;
      }
    }
  }
  return true;
}

bool Game::canRotate() {
  byte rotated[3][3];
  rotateArray(cShape, rotated);
  
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 3; x++) {
      if (cShapeY + y + 1 > BOARD_HEIGHT) continue;
      if (rotated[y][x] == 1 && board[cShapeY + y + 1][cShapeX + x] == 1) {
        return false;
      }
    }
  }
  
  return true; 
}

void Game::persistShape() {
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 3; x++) {
      if (cShape[y][x] == 1) {
          board[cShapeY + y][cShapeX + x] = 1;
      }
    }
  }
}

void Game::newShape() {
  isCShapeMoving = false;
  persistShape();
  addShape();
}

void Game::moveLeft() {
  if (isPaused) { 
    return;
  }
  byte minX = shapeMinX();
  if (canMoveLeft() && isCShapeMoving && cShapeX + (minX - 1) > 0) {
    cShapeX -= 1;
  }
}

void Game::moveRight() {
  if (isPaused) { 
    return;
  }
  if (canMoveRight() && isCShapeMoving && cShapeX < BOARD_WIDTH - shapeMaxX()) {
    cShapeX += 1;
  }
}

void Game::moveDown() {
  if (isPaused) {
    return;
  }
  if (cShapeY + shapeMaxY() < BOARD_HEIGHT) {
    cShapeY += 1;
  } else {
    newShape();
  }
}

byte Game::shapeMaxX() {
  byte maximumX = 0;
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 3; x++) {
      if (cShape[y][x] == 1) {
        maximumX = max(x, maximumX);
      }
    }
  }
  return maximumX + 1;
}

byte Game::shapeMinX() {
  byte minimumX = BOARD_WIDTH;
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 3; x++) {
      if (cShape[y][x] == 1) {
        minimumX = min(x, minimumX);
      }
    }
  }
  return minimumX + 1;
}

byte Game::shapeMaxY() {
  byte maximumY = 0;
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 3; x++) {
      if (cShape[y][x] == 1) {
        maximumY = max(y, maximumY);
      }
    }
  }
  return maximumY + 1;
}

void Game::drawNext(Arduboy2 arduboy) {
  
  arduboy.setCursor(100, 10);
  arduboy.print("Next");
  arduboy.drawRoundRect(100, 21, 21, 21, 2);

  byte paddingX;
  byte paddingY;
  byte nextShape[3][3];
  prepareShape(nextShape, nextShapeIndex);

  switch (nextShapeIndex) {
    case 0:
    case 1: 
    case 4:
    case 6: 
      paddingX = 104; break;
    case 2: 
      paddingX = 103; break;
    case 3: 
    case 5: 
      paddingX = 106; break;
  }

  switch (nextShapeIndex) {
    case 0: 
    case 1: 
    case 4: 
    case 5: 
      paddingY = 27; break;
    case 2: 
    case 3: 
    case 6: 
      paddingY = 25; break;
  }
  
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 3; x++) {
      byte xOffset = paddingX + x * 4 + 1;
      byte yOffset = paddingY + y * 4 + 1;
      if (nextShape[y][x] == 1) {
        arduboy.drawRect(xOffset, yOffset, 3, 3);
      }
    }
  }
}

void Game::drawShape(Arduboy2 arduboy) {
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 3; x++) {
      byte xOffset = LEFT_OFFSET + (cShapeX + x) * 4 + 1;
      byte yOffset = TOP_OFFSET + (cShapeY + y) * 4 + 1;
      if (cShape[y][x] == 1) {
        arduboy.drawRect(xOffset, yOffset, 3, 3);
      }
    }
  }
}

void Game::drawBoard(Arduboy2 arduboy) {
  for (byte y = 0; y < BOARD_HEIGHT; y++) {
    for (byte x = 0; x < BOARD_WIDTH; x++) {
      byte xPos = LEFT_OFFSET + x * 4 + 1;
      byte yPos = TOP_OFFSET + y * 4 + 1;
      if (board[y][x] == 1) {
        arduboy.drawRect(xPos, yPos, 3, 3);
      }
    }
  }
  byte l1x = LEFT_OFFSET - 1;
  byte l2x = LEFT_OFFSET + (BOARD_WIDTH * 4) + 1;
  byte lt = 1; // Top frame position
  byte lb = 61; // Bottom frame position
  arduboy.drawLine(l1x, lt, l1x, lb);
  arduboy.drawLine(l2x, lt, l2x, lb);
  arduboy.drawLine(l1x, lb, l2x, lb);
}

void Game::rotateCurrentShape() {
  if (isPaused) { 
    return;
  }
  if (canRotate()) {
    byte rotated[3][3];
    rotateArray(cShape, rotated);
    memcpy(cShape, rotated, sizeof(rotated));
  
    // Fix shape position after rotate
    if (cShapeX + shapeMaxX() > BOARD_WIDTH) {
      cShapeX -= 1;
    } else if (cShapeX - shapeMinX() < 0) {
      cShapeX += 1;
    }
  }
}

void Game::rotateArray(byte input[3][3], byte output[3][3]) {
  for (byte i = 0; i < 3; i++) {
    for (byte j = 0; j < 3; j++) {
      output[i][j] = input[3 - j - 1][i];
    }
  }
}

// Fill board with "zeros"
void Game::fillBoard() { 
  for (byte y = 0; y < BOARD_HEIGHT; y++) {
    for (byte x = 0; x < BOARD_WIDTH; x++) {
      board[y][x] = 0;
    }
  }
}

void Game::drawStartScreen(Arduboy2 arduboy) {

  arduboy.setTextSize(2);
  arduboy.setCursor(28, 18);
  arduboy.print("Blocks"); 
  
  arduboy.setTextSize(1);
  arduboy.setCursor(10, 48);
  arduboy.print("Press 'B' to start"); 

  for (byte y = 0; y < 15; y++) {
    byte yPos = y * 4;
    arduboy.drawRect(0, yPos, 3, 3);
    arduboy.drawRect(124, yPos, 3, 3);
  }
  
  for (byte x = 0; x < 32; x++) {
    byte xPos = x * 4;
    arduboy.drawRect(xPos, 0, 3, 3);
    arduboy.drawRect(xPos, 60, 3, 3);
  }
}

void Game::clearMemory() {
  byte memorySize = 2;
  for (byte i = 0; i <= memorySize; i++) {
    EEPROM.write(i, 0);
  }
}

void Game::saveValue(int address, int number) {
  byte byte1 = number >> 8;
  byte byte2 = number & 0xFF;
  EEPROM.write(address, byte1);
  EEPROM.write(address + 1, byte2);
}

int Game::readValue(int address) {
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}
