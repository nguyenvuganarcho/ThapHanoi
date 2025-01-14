#include <graphics.h>

#include <iostream>
#include <string>
using namespace std;

#define BOARD_SIZE 15
#define CELL_SIZE 40
#define MENU_NEW_GAME 1
#define MENU_RULES 2
#define MENU_EXIT 3

class Player {
 private:
  string name;  // player name
  char symbol;  // X or O

 public:
  Player(const string& playerName, char playerSymbol)
      : name(playerName), symbol(playerSymbol) {
    cout << "The name created. Name: " << name
         << ". The symbol created. Symbol " << symbol << endl;
  }
  string getPlayerName() const {
    cout << "The player name is " << name << endl;
    return name;
  }
  char getPlayerSymbol() const {
    cout << "The player symbol is " << symbol << endl;
    return symbol;
  }
};

class Table {
 private:
  char grid[BOARD_SIZE][BOARD_SIZE];  // grid

 public:
  // Initialize grid with empty space
  Table() {
    cout << "Initializing the grid..." << endl;
    for (int x = 0; x < BOARD_SIZE; x++) {
      for (int y = 0; y < BOARD_SIZE; y++) {
        grid[x][y] = ' ';
      }
    }
    cout << "Initial the grid succesfull" << endl;
  }

  bool makeMove(int x, int y, char symbol) {
    // Make a move on the board
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
      cout << "the move is not on the board" << endl;
      return false;
    } else if (grid[x][y] != ' ') {
      cout << " Cell is already occupied" << endl;
      return false;
    } else {
      grid[x][y] = symbol;
      cout << "Move succesfull" << endl;
      return true;
    }
  }

  char getCell(int x, int y) const {
    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
      return grid[x][y];
    }
    return '\0';
  }

  void resetBoard() {
    for (int x = 0; x < BOARD_SIZE; x++) {
      for (int y = 0; y < BOARD_SIZE; y++) {
        grid[x][y] = ' ';
      }
    }      
  }
};

class Rules : public Table {
 public:
  bool checkWinner(char symbol) {
    // check for vertical
    for (int x = 0; x < BOARD_SIZE; x++) {
      int count = 0;
      for (int y = 0; y < BOARD_SIZE; y++) {
        if (getCell(x, y) == symbol) {
          count++;

          if (count == 5) {
            cout << "Vertical win!" << endl;
            return true;
          }
        } else {
          count = 0;
        }
      }
    }

    // check for horizontal
    for (int y = 0; y < BOARD_SIZE; y++) {
      int count = 0;
      for (int x = 0; x < BOARD_SIZE; x++) {
        if (getCell(x, y) == symbol) {
          count++;

          if (count == 5) {
            cout << "Horizontal win" << endl;
            return true;
          }
        } else {
          count = 0;
        }
      }
    }

    // check for diagonal (top left to bottom right)
    for (int x = 0; x < BOARD_SIZE - 4; x++) {
      for (int y = 0; y < BOARD_SIZE - 4; y++) {
        int count = 0;
        for (int i = 0; i < 5; i++) {
          if (getCell(x + i, y + i) == symbol) {
            count++;
            if (count == 5) {
              cout << "diagonal (top left to bottom right) win" << endl;
              return true;
            }
          } else {
            count = 0;
          }
        }
      }
    }

    // check for diagonal (top right to bottom left)
    for (int x = 0; x < BOARD_SIZE - 4; x++) {
      for (int y = 4; y < BOARD_SIZE; y++) {
        int count = 0;
        for (int i = 0; i < 5; i++) {
          if (getCell(x + i, y - i) == symbol) {
            count++;
            if (count == 5) {
              cout << "top right to bottom left" << endl;
              return true;
            }
          } else {
            count = 0;
          }
        }
      }
    }

    return false;
  }
};

class Interface {
 public:
  void drawGrid(int startX, int startY) {
    // loop through rows and columns to draw horizontal and vertical line
    for (int i = 0; i < BOARD_SIZE + 1; i++) {
      // draw vertial
      line(startX + (i * CELL_SIZE), startY, startX + (i * CELL_SIZE),
           startY + (BOARD_SIZE * CELL_SIZE));
      // draw horizontal
      line(startX, startY + (i * CELL_SIZE), startX + (BOARD_SIZE * CELL_SIZE),
           startY + (i * CELL_SIZE));
    }
  }

  // Function to handle clicking events
  bool handleClick(int startX, int startY, int& gridX, int& gridY) {
    int mouseX, mouseY;
    if (ismouseclick(WM_LBUTTONDBLCLK)) {
      getmouseclick(WM_LBUTTONDBLCLK, mouseX, mouseY);

      cout << "Mouse clicked at " << mouseX << ", " << mouseY << endl;

      // Calculate grid position
      gridX = (mouseX - startX) / CELL_SIZE;
      gridY = (mouseY - startY) / CELL_SIZE;

      if (gridX >= 0 && gridX < BOARD_SIZE && gridY >= 0 &&
          gridY < BOARD_SIZE) {
        return true;  // valid move
         
      }
    }
    return false;  // invalid move
  }

  // Function to draw x and y
  void drawMove(int startX, int startY, int x, int y, char symbol) {
    int centerCellX = startX + (x * CELL_SIZE) + (CELL_SIZE / 2);
    int centerCellY = startY + (y * CELL_SIZE) + (CELL_SIZE / 2);

    // draw X
    if (symbol == 'X') {
      line(centerCellX - 15, centerCellY - 15, centerCellX + 15,
           centerCellY + 15);
      line(centerCellX + 15, centerCellY - 15, centerCellX - 15,
           centerCellY + 15);
      cout << "draw X succesfull" << endl;
    } else if (symbol == 'O') {
      circle(centerCellX, centerCellY, 15);
      cout << "draw O successfull" << endl;
    }
  }
  
};

class Menu {
 public:
  void drawMenu(int centerX, int centerY) {
    settextstyle(8, 0, 5);
    outtextxy(centerX - 100, centerY - 100, (char*)"1. New Game");
    outtextxy(centerX - 100, centerY - 50, (char*)"2. Rules");
    outtextxy(centerX - 100, centerY, (char*)"3. Exit");
  }

  int handleMenuClick(int centerX, int centerY) {
    int mouseX, mouseY;
    while (true) {
      if (ismouseclick(WM_LBUTTONDOWN)) {
        getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);

        if (mouseX >= centerX - 100 && mouseX <= centerX + 200) {
          if (mouseY >= centerY - 100 && mouseY <= centerY - 50) {
            return MENU_NEW_GAME;
          }
          if (mouseY >= centerY - 50 && mouseY <= centerY) {
            return MENU_RULES;
          }
          if (mouseY >= centerY && mouseY <= centerY + 50) {
            return MENU_EXIT;
          }
        }
      }
    }
  }

  void displayRules(int centerX, int centerY) {
    cleardevice();
    settextstyle(8, 0, 3);
    outtextxy(centerX - 200, centerY - 100, (char*)"Rules of XO Game:");
    outtextxy(centerX - 300, centerY - 50, (char*)"1. Players take turns placing their symbol (X or O).");
    outtextxy(centerX - 300, centerY, (char*)"2. The first to form a line of 5 wins.");
    outtextxy(centerX - 300, centerY + 50, (char*)"3. Lines can be horizontal, vertical, or diagonal.");
    outtextxy(centerX - 200, centerY + 150, (char*)"Click anywhere to return to menu.");

    while (!ismouseclick(WM_LBUTTONDOWN)) {
      // Wait for a click
    }
    cleardevice();
  }
};

// Your existing Player, Table, Rules, and Interface classes go here...

int main() {
  initwindow(1280, 720);

  Menu menu;
  Player player1("Player1", 'X');
  Player player2("Player2", 'O');
  Interface grid;
  Rules theRules;

  Player* currentPlayer = &player1;
  bool gameOver = false;

  int centerX = 1280 / 2;
  int centerY = 720 / 2;
  int startX = centerX - (BOARD_SIZE * CELL_SIZE) / 2;
  int startY = centerY - (BOARD_SIZE * CELL_SIZE) / 2;
  int gridX, gridY;

  while (true) {
    cleardevice();
    menu.drawMenu(centerX, centerY);

    int choice = menu.handleMenuClick(centerX, centerY);

    if (choice == MENU_NEW_GAME) {
      while (true) {
        cleardevice();
        theRules.resetBoard();
        grid.drawGrid(startX, startY);
        gameOver = false;
        currentPlayer = &player1;

        while (!gameOver) {
          if (grid.handleClick(startX, startY, gridX, gridY)) {
            if (theRules.makeMove(gridX, gridY, currentPlayer->getPlayerSymbol())) {
              grid.drawMove(startX, startY, gridX, gridY, currentPlayer->getPlayerSymbol());
              if (theRules.checkWinner(currentPlayer->getPlayerSymbol())) {
                string winnerMessage = currentPlayer->getPlayerName() + " wins!";
                settextstyle(8, 0, 5);
                outtextxy(centerX - 120,     centerY + (BOARD_SIZE * CELL_SIZE) / 2 + 20,
                          (char*)winnerMessage.c_str());
                delay(3000);
                gameOver = true;
              }
              currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
            }
          }
        }

        cleardevice();
        settextstyle(8, 0, 3);
        outtextxy(centerX - 150, centerY - 50, (char*)"1. New Game");
        outtextxy(centerX - 150, centerY, (char*)"2. Back to Menu");

        while (true) {
          if (ismouseclick(WM_LBUTTONDOWN)) {
            getmouseclick(WM_LBUTTONDOWN, gridX, gridY);

            if (gridX >= centerX - 150 && gridX <= centerX + 150) {
              if (gridY >= centerY - 50 && gridY <= centerY) {
                break; // Start a new game
              }
              if (gridY >= centerY && gridY <= centerY + 50) {
                goto menu_loop; // Go back to menu
              }
            }
          }
        }
      }
    } else if (choice == MENU_RULES) {
      menu.displayRules(centerX, centerY);
    } else if (choice == MENU_EXIT) {
      break;
    }
  menu_loop:;
  }

  closegraph();
  return 0;
}   