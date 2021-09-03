#include <Arduboy2.h>
#include "Game.h"

#define TOP_OFFSET 0
#define LEFT_OFFSET 44
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 15

void Game::startGame() {
  isGameRunning = true;
}

bool Game::getIsGameRunning() {
  return isGameRunning;
}

bool Game::getIsGameEnd() {
  return isGameEnd;
}

void Game::addShape() {
  cShapeX = 4; // Default start position
  cShapeY = 0;
  isCShapeMoving = true;

  int shapeIndex = random(0, 7);
  switch (shapeIndex) {
    case 0: memcpy(cShape, shape_1, sizeof(shape_1)); break;
    case 1: memcpy(cShape, shape_2, sizeof(shape_2)); break;
    case 2: memcpy(cShape, shape_3, sizeof(shape_3)); break;
    case 3: memcpy(cShape, shape_4, sizeof(shape_4)); break;
    case 4: memcpy(cShape, shape_5, sizeof(shape_5)); break;
    case 5: memcpy(cShape, shape_6, sizeof(shape_6)); break;
    case 6: memcpy(cShape, shape_7, sizeof(shape_7)); break;
  } 
}

void Game::dropLines() {
  
}

void Game::collisionDetection() {
  for (byte y = 0; y < 3; y++) {
    for (byte x = 0; x < 3; x++) {
      if (cShapeY + y + 1 > BOARD_HEIGHT) continue;
      if (cShape[y][x] == 1 && board[cShapeY + y + 1][cShapeX + x] == 1) {

        if (cShapeY == 0 /* TODO: change dummy end game */) {
          isGameEnd = true;
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
  byte minX = shapeMinX();
  if (canMoveLeft() && isCShapeMoving && cShapeX + (minX - 1) > 0) {
    cShapeX -= 1;
  }
}

void Game::moveRight() {
  if (canMoveRight() && isCShapeMoving && cShapeX < BOARD_WIDTH - shapeMaxX()) {
    cShapeX += 1;
  }
}

void Game::moveDown() {
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
