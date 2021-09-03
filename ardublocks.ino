#include <Arduboy2.h>
#include "Game.h"

Arduboy2 arduboy;
Game game;

byte gameSpeed = 20;

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30); 
  arduboy.initRandomSeed();

  game.fillBoard();
  game.addShape();
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  arduboy.clear();

  if (game.getIsGameEnd()) {
    endGameScreen();
  } else if (game.getIsGameRunning()) {
    gameScreen();
  } else {
    startGameScreen();
  }

  arduboy.display();
}

void endGameScreen() {
  arduboy.setCursor(2, 2);
  arduboy.print("Game End"); 
}

void gameScreen() {
  if (arduboy.everyXFrames(4)) {
    if (arduboy.pressed(LEFT_BUTTON)) {
      game.moveLeft();
    } else if (arduboy.pressed(RIGHT_BUTTON)) {
      game.moveRight();
    } else if (arduboy.pressed(DOWN_BUTTON)) {
      game.moveDown();
    } else if (arduboy.pressed(B_BUTTON)) {
      game.rotateCurrentShape();
    }
  }
  
  if (arduboy.everyXFrames(gameSpeed)) {
    game.moveDown();
  }

  game.collisionDetection();
  game.drawShape(arduboy);

  game.drawBoard(arduboy);
}

void startGameScreen() {
  if (arduboy.everyXFrames(4)) {
    if (arduboy.pressed(A_BUTTON)) {
      game.startGame();
    }
  }
  
  arduboy.setCursor(6, 25);
  arduboy.print("Press 'A' to start"); 
}