#include <Arduboy2.h>
#include "Game.h"

#define CLEAR_MEMORY false

Arduboy2 arduboy;
Game game;

byte gameSpeed = 16;
boolean isConfirmDialogShown = false;

void setup() {
  arduboy.begin();
  arduboy.setFrameRate(30); 
  arduboy.initRandomSeed();

  if (CLEAR_MEMORY) {
    game.clearMemory();
  }
}

void loop() {
  if (!(arduboy.nextFrame()))
    return;

  arduboy.pollButtons();
  arduboy.clear();

  if (game.getIsGameEnd()) {
    endGameDialog();
    handleEndGameDialogButtons();
  } else if (isConfirmDialogShown) {
    drawExitDialog();
    handleExitDialogButtons();
  } else if (game.getIsGameRunning()) {
    gameScreen();
  } else {
    startGameScreen();
  }

  arduboy.display();
}

void gameScreen() {
  
  if (arduboy.pressed(LEFT_BUTTON)) {
    if (arduboy.everyXFrames(3)) {
      game.moveLeft();
    }
  } else if (arduboy.pressed(RIGHT_BUTTON)) {
    if (arduboy.everyXFrames(3)) {
      game.moveRight();
    }
  } else if (arduboy.pressed(DOWN_BUTTON)) {
    if (arduboy.everyXFrames(3)) {
      game.moveDown();
    }
  } else if (arduboy.justPressed(UP_BUTTON)) {
    game.pauseGame();
  } else if (arduboy.justPressed(A_BUTTON)) {
    isConfirmDialogShown = true;
    game.pauseGame();
  } else if (arduboy.justPressed(B_BUTTON)) {
    game.rotateCurrentShape();
  }
  
  if (arduboy.everyXFrames(gameSpeed - game.getLevel())) {
    game.moveDown();
  }
  
  game.collisionDetection();
  game.dropLines();
  game.drawShape(arduboy);
  game.drawBoard(arduboy);
  game.drawNext(arduboy);
  
  displayPoints();
  displayLevel();
  displayBestScore();
}

void displayPoints() {
  arduboy.setCursor(0, 0);
  arduboy.print("Points");
  arduboy.setCursor(0, 10);
  arduboy.print(game.getScore());
}

void displayLevel() {
  arduboy.setCursor(0, 22);
  arduboy.print("Level");
  arduboy.setCursor(0, 32);
  arduboy.print(game.getLevel());
}

void displayBestScore() {
  arduboy.setCursor(0, 44);
  arduboy.print("Best");
  arduboy.setCursor(0, 54);
  if (game.getScore() < game.getBestScore()) {
    arduboy.print(game.getBestScore());
  } else {
    arduboy.print(game.getScore());
  }
}

void endGameDialog() {
  drawDialogBase();
  
  arduboy.setCursor(9, 11);
  arduboy.print("Game end!");
  arduboy.setCursor(9, 22);
  arduboy.print("Your score: ");
  arduboy.setCursor(78, 22);
  arduboy.print(game.getScore() );
  arduboy.setCursor(92, 46);
  arduboy.print("OK(B)");
  
  arduboy.setTextBackground(BLACK);
  arduboy.setTextColor(WHITE);
}

void handleEndGameDialogButtons() {
  if (arduboy.justPressed(B_BUTTON)) {
    game.resetGame();
  }
}

void startGameScreen() {
  if (arduboy.justPressed(B_BUTTON)) {
    game.startGame();
  }

  game.drawStartScreen(arduboy);
}

void handleExitDialogButtons() {
  // A button is exiting to menu
  if (arduboy.justPressed(A_BUTTON)) {
    isConfirmDialogShown = false;
    game.setIsGameRunning(false);
  } else if (arduboy.justPressed(B_BUTTON)) {
    isConfirmDialogShown = false;
    game.pauseGame();
  }
}

void drawExitDialog() {
  drawDialogBase();
  
  arduboy.setCursor(9, 11);
  arduboy.print("Exit game?");
  arduboy.setCursor(38, 46);
  arduboy.print("Yes(A)   No(B)");
  
  arduboy.setTextBackground(BLACK);
  arduboy.setTextColor(WHITE);
}

void drawDialogBase() {
  arduboy.fillRoundRect(2, 4, 124, 56, 4);
  arduboy.setCursor(7, 9);
  arduboy.setTextColor(BLACK);
  arduboy.setTextBackground(WHITE);
}
