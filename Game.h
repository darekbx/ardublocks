#include <Arduboy2.h>

#define TOP_OFFSET 1
#define LEFT_OFFSET 44
#define BOARD_WIDTH 10
#define BOARD_HEIGHT 15

class Game {

  private:
    byte board[BOARD_HEIGHT][BOARD_WIDTH];

    byte shape_1[3][3] = {
        { 0, 1, 1 },
        { 1, 1, 0 },
        { 0, 0, 0 },
    };
    byte shape_2[3][3] = {
        { 1, 1, 0 },
        { 0, 1, 1 },
        { 0, 0, 0 },
    };  
    byte shape_3[3][3] = {
        { 0, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
    };
    byte shape_4[3][3] = {
        { 0, 1, 0 },
        { 0, 1, 0 },
        { 1, 1, 0 },
    };
    byte shape_5[3][3] = {
        { 0, 1, 0 },
        { 1, 1, 1 },
        { 0, 0, 0 },
    };
    byte shape_6[3][3] = {
        { 1, 1, 0 },
        { 1, 1, 0 },
        { 0, 0, 0 },
    };
    byte shape_7[3][3] = {
        { 0, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 0 },
    };
    
    int cShapeX; // Current shape X position
    int cShapeY; // Current shape Y position

    byte cShape[3][3];

    bool isGameRunning = false;
    bool isCShapeMoving = true;
    bool isGameEnd = false;
    
    void rotateArray(byte input[3][3], byte output[3][3]);
    byte shapeMaxX();
    byte shapeMaxY();
    byte shapeMinX();
    void persistShape();
    void newShape();
    bool canMoveLeft();
    bool canMoveRight();
    void dropLines();

  public:
    void startGame();
    bool getIsGameEnd();
    bool getIsGameRunning();
    void collisionDetection();
    void addShape();
    void moveLeft();
    void moveRight();
    void moveDown();
    void drawShape(Arduboy2 arduboy);
    void drawBoard(Arduboy2 arduboy);
    void rotateCurrentShape();
    // Fill board with "zeros"
    void fillBoard();
};
