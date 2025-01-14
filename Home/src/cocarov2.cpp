  #include <conio.h>     // For kbhit(), getch()
  #include <graphics.h>  // For initwindow(), line(), circle(), etc.

  #include <ctime>
  #include <iostream>
  #include <sstream>
  #include <string>
  #include <vector>
  using namespace std;

  // Global Defines
  #define BOARD_SIZE 15
  #define CELL_SIZE 40

  // Menu constants
  #define MENU_NEW_GAME 1
  #define MENU_RULES 2
  #define MENU_EXIT 3
  #define MENU_PLAY_BOT 4
  #define TURN_TIME 60

  class Player {
  private:
    string name;
    char symbol;
    int score;

  public:
    Player(const string &playerName = "Player1", char playerSymbol = 'X')
        : name(playerName), symbol(playerSymbol), score(0) {}

    string getPlayerName() const { return name; }
    char getPlayerSymbol() const { return symbol; }
    int getPlayerScore() const { return score; }

    void setPlayerName(const string &playerName) { name = playerName; }
    void setPlayerSymbol(char playerSymbol) { symbol = playerSymbol; }
    void incrementScore() { score++; }
    void resetScore() { score = 0; }
  };

  class Table {
  private:
    char grid[BOARD_SIZE][BOARD_SIZE];

  public:
    Table() { resetBoard(); }

    bool makeMove(int x, int y, char symbol) {
      if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return false;  // out of bounds
      }
      if (grid[x][y] != ' ') {
        return false;  // occupied
      }
      grid[x][y] = symbol;
      return true;
    }

    void revertMove(int x, int y) {
      if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
        grid[x][y] = ' ';
      }
    }

    char getCell(int x, int y) const {
      if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return '\0';
      }
      return grid[x][y];
    }

    void resetBoard() {
      for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
          grid[i][j] = ' ';
        }
      }
    }
  };

  class Rules : public Table {
  public:
    bool checkWinner(char symbol) {
      // 1. Check vertical
      for (int x = 0; x < BOARD_SIZE; x++) {
        int count = 0;
        for (int y = 0; y < BOARD_SIZE; y++) {
          if (getCell(x, y) == symbol) {
            count++;
            if (count == 5) return true;
          } else {
            count = 0;
          }
        }
      }

      // 2. Check horizontal
      for (int y = 0; y < BOARD_SIZE; y++) {
        int count = 0;
        for (int x = 0; x < BOARD_SIZE; x++) {
          if (getCell(x, y) == symbol) {
            count++;
            if (count == 5) return true;
          } else {
            count = 0;
          }
        }
      }

      // 3. Diagonal (top-left to bottom-right)
      for (int x = 0; x < BOARD_SIZE - 4; x++) {
        for (int y = 0; y < BOARD_SIZE - 4; y++) {
          bool win = true;
          for (int i = 0; i < 5; i++) {
            if (getCell(x + i, y + i) != symbol) {
              win = false;
              break;
            }
          }
          if (win) return true;
        }
      }

      // 4. Diagonal (top-right to bottom-left)
      for (int x = 0; x < BOARD_SIZE - 4; x++) {
        for (int y = 4; y < BOARD_SIZE; y++) {
          bool win = true;
          for (int i = 0; i < 5; i++) {
            if (getCell(x + i, y - i) != symbol) {
              win = false;
              break;
            }
          }
          if (win) return true;
        }
      }

      return false;
    }
  };

  class Interface {
  public:
    // Draw the 15x15 grid
    void drawGrid(int startX, int startY) {
      setcolor(WHITE);
      for (int i = 0; i <= BOARD_SIZE; i++) {
        // vertical lines
        line(startX + i * CELL_SIZE, startY, startX + i * CELL_SIZE,
            startY + BOARD_SIZE * CELL_SIZE);
        // horizontal lines
        line(startX, startY + i * CELL_SIZE, startX + BOARD_SIZE * CELL_SIZE,
            startY + i * CELL_SIZE);
      }
    }

    // Handle single-click: returns true if valid cell
    bool handleClick(int startX, int startY, int &gridX, int &gridY) {
      if (ismouseclick(WM_LBUTTONDOWN)) {
        int mouseX, mouseY;
        getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);
        // Convert screen coords to grid coords
        gridX = (mouseX - startX) / CELL_SIZE;
        gridY = (mouseY - startY) / CELL_SIZE;

        clearmouseclick(WM_LBUTTONDOWN);  // clear event
        if (gridX >= 0 && gridX < BOARD_SIZE && gridY >= 0 &&
            gridY < BOARD_SIZE) {
          return true;
        }
      }
      return false;
    }

    // Draw X or O
    void drawMove(int startX, int startY, int x, int y, char symbol) {
      int centerX = startX + x * CELL_SIZE + CELL_SIZE / 2;
      int centerY = startY + y * CELL_SIZE + CELL_SIZE / 2;

      if (symbol == 'X') {
        line(centerX - CELL_SIZE / 4, centerY - CELL_SIZE / 4,
            centerX + CELL_SIZE / 4, centerY + CELL_SIZE / 4);
        line(centerX + CELL_SIZE / 4, centerY - CELL_SIZE / 4,
            centerX - CELL_SIZE / 4, centerY + CELL_SIZE / 4);
      } else if (symbol == 'O') {
        circle(centerX, centerY, CELL_SIZE / 4);
      }
    }

    // Show players info
    void displayPlayerInfo(int infoX, int infoY, const Player &p1,
                          const Player &p2, const Player *currentPlayer) {
      settextstyle(8, 0, 3);

      // Player 1
      string p1Info = p1.getPlayerName() + " (" + p1.getPlayerSymbol() +
                      ") - Score: " + to_string(p1.getPlayerScore());
      outtextxy(infoX - 110, infoY, (char *)p1Info.c_str());

      // Player 2
      string p2Info = p2.getPlayerName() + " (" + p2.getPlayerSymbol() +
                      ") - Score: " + to_string(p2.getPlayerScore());
      outtextxy(infoX - 110, infoY + 30, (char *)p2Info.c_str());

    }

    // Show countdown timer
    void displayTimer(int timerX, int timerY, int secondsLeft) {
      settextstyle(8, 0, 3);
      int minutes = secondsLeft / 60;
      int seconds = secondsLeft % 60;
      stringstream ss;
      ss << "Time Left: " << (minutes < 10 ? "0" : "") << minutes << ":"
        << (seconds < 10 ? "0" : "") << seconds;

      outtextxy(timerX, timerY, (char *)ss.str().c_str());
    }

    // Prompt user for text input on-screen
    string getTextInput(const string &prompt, int x, int y) {
      string input;
      settextstyle(8, 0, 2);
      outtextxy(x, y, (char *)prompt.c_str());

      int inputX = x;
      int inputY = y + 30;

      setcolor(WHITE);
      rectangle(inputX - 5, inputY - 5, inputX + 300, inputY + 20);
      setcolor(WHITE);

      bool done = false;
      while (!done) {
        if (kbhit()) {
          char ch = getch();
          if (ch == 13) {  // Enter
            done = true;
          } else if (ch == 8) {  // Backspace
            if (!input.empty()) {
              input.pop_back();
              // Clear area to redraw text
              setfillstyle(SOLID_FILL, BLACK);
              bar(inputX, inputY - 3, inputX + 295, inputY + 17);
              settextstyle(8, 0, 2);
              outtextxy(x, y, (char *)prompt.c_str());
              setcolor(WHITE);
              outtextxy(inputX, inputY, (char *)input.c_str());
            }
          } else if (isprint(ch) && input.size() < 25) {
            input.push_back(ch);
            outtextxy(inputX, inputY, (char *)input.c_str());
          }
        }
        delay(10);
      }
      return input;
    }
  };

  class Bot : public Player {
  public:
    Bot(const string &botName = "Bot", char botSymbol = 'O')
        : Player(botName, botSymbol) {}

  private:
    // Basic "AI" checks for 5 in a row, or block opponent

    bool canFormN(Rules &rules, int x, int y, char symbol, int N) {
      if (!rules.makeMove(x, y, symbol)) {
        return false;
      }
      bool result = hasNInARow(rules, x, y, symbol, N);
      rules.revertMove(x, y);
      return result;
    }

    bool hasNInARow(Rules &rules, int startX, int startY, char symbol, int N) {
      const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
      for (auto &dir : directions) {
        int count = 1;  // includes the current cell
        count +=
            countOneDirection(rules, startX, startY, -dir[0], -dir[1], symbol);
        count += countOneDirection(rules, startX, startY, dir[0], dir[1], symbol);
        if (count >= N) return true;
      }
      return false;
    }

    int countOneDirection(Rules &rules, int x, int y, int dx, int dy,
                          char symbol) {
      int count = 0;
      int newX = x + dx;
      int newY = y + dy;
      while (true) {
        if (newX < 0 || newX >= BOARD_SIZE || newY < 0 || newY >= BOARD_SIZE) {
          break;
        }
        if (rules.getCell(newX, newY) == symbol) {
          count++;
          newX += dx;
          newY += dy;
        } else {
          break;
        }
      }
      return count;
    }

    bool canFormThreeWithGap(Rules &rules, int x, int y, char symbol) {
      if (!rules.makeMove(x, y, symbol)) {
        return false;
      }
      bool result = hasNWithOneGap(rules, x, y, symbol, 3);
      rules.revertMove(x, y);
      return result;
    }

    bool hasNWithOneGap(Rules &rules, int startX, int startY, char symbol,
                        int N) {
      static const int directions[4][2] = {{1, 0}, {0, 1}, {1, 1}, {1, -1}};
      for (auto &dir : directions) {
        if (checkWindowForGap(rules, startX, startY, dir[0], dir[1], symbol, N)) {
          return true;
        }
      }
      return false;
    }

    bool checkWindowForGap(Rules &rules, int startX, int startY, int dx, int dy,
                          char symbol, int N) {
      vector<char> window(5, '#');
      for (int i = -2; i <= 2; i++) {
        int idx = i + 2;
        int newX = startX + i * dx;
        int newY = startY + i * dy;
        if (newX < 0 || newX >= BOARD_SIZE || newY < 0 || newY >= BOARD_SIZE) {
          window[idx] = '#';
        } else {
          window[idx] = rules.getCell(newX, newY);
        }
      }

      char oppSymbol = (symbol == 'X') ? 'O' : 'X';
      // Slide sub-window of size 4 => [0..3], [1..4]
      for (int startPos = 0; startPos <= 1; startPos++) {
        int symCount = 0;
        int gapCount = 0;
        bool blocked = false;
        for (int offset = 0; offset < 4; offset++) {
          char c = window[startPos + offset];
          if (c == symbol) {
            symCount++;
          } else if (c == ' ') {
            gapCount++;
          } else {
            // either oppSymbol or '#'
            blocked = true;
          }
        }
        if (!blocked && symCount >= N) {
          // gap must be <= (4 - N)
          if (gapCount <= (4 - N)) {
            return true;
          }
        }
      }
      return false; 
    }

  public:
    int calcScore(Rules &rules, int x, int y, char mySymbol, char oppSymbol) {
      // 1) My immediate 5
      if (canFormN(rules, x, y, mySymbol, 5)) {
        return 100000;
      }
      // 2) Opponent 5
      if (canFormN(rules, x, y, oppSymbol, 5)) {
        return 99999;
      }
      // 3) My 4
      if (canFormN(rules, x, y, mySymbol, 4)) {
        return 95000;
      }
      // 4) Opponent 4
      if (canFormN(rules, x, y, oppSymbol, 4)) {
        return 90000;
      }
      // 5) Opponent 3
      if (canFormN(rules, x, y, oppSymbol, 3)) {
        return 75000;
      }
      // 6) Opponent 3 with gap
      if (canFormThreeWithGap(rules, x, y, oppSymbol)) {
        return 82000;
      }
      // 7) My 3 with gap
      if (canFormThreeWithGap(rules, x, y, mySymbol)) {
        return 81000;
      }

      // Otherwise prefer center
      int center = BOARD_SIZE / 2;
      int distX = x - center;
      int distY = y - center;
      int dist = abs(distX) + abs(distY);
      int centerScore = 1000 - (dist * 10);
      return centerScore;
    }

    pair<int, int> getBestMove(Rules &theRules, char opponentSymbol) {
      int bestScore =   -999999;
      pair<int, int> bestCell(-1, -1);

      for (int x = 0; x < BOARD_SIZE; x++) {
        for (int y = 0; y < BOARD_SIZE; y++) {
          if (theRules.getCell(x, y) == ' ') {
            int score =
                calcScore(theRules, x, y, getPlayerSymbol(), opponentSymbol);
            if (score > bestScore) {
              bestScore = score;
              bestCell = {x, y};
            }
          }
        }
      }
      return bestCell;
    }
  };

  class Menu {
  public:
    // Main Menu
    void drawMenu(int centerX, int centerY) {
      settextstyle(8, 0, 4);
      outtextxy(centerX - 100, centerY - 100, (char *)"1. Player vs Player");
      outtextxy(centerX - 100, centerY - 50, (char *)"2. Play vs Bot");
      outtextxy(centerX - 100, centerY, (char *)"3. Rules");
      outtextxy(centerX - 100, centerY + 50, (char *)"4. Exit");
    }

    int handleMenuClick(int centerX, int centerY) {
      while (true) {
        if (ismouseclick(WM_LBUTTONDOWN)) {
          int mouseX, mouseY;
          getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);

          // We'll check bounding boxes
          if (mouseX >= centerX - 150 && mouseX <= centerX + 150) {
            // 1. New Game
            if (mouseY >= centerY - 100 && mouseY <= centerY - 50) {
              return MENU_NEW_GAME;
            }
            // 2. Play vs Bot
            if (mouseY >= centerY - 50 && mouseY <= centerY) {
              return MENU_PLAY_BOT;
            }
            // 3. Rules
            if (mouseY >= centerY && mouseY <= centerY + 50) {
              return MENU_RULES;
            }
            // 4. Exit
            if (mouseY >= centerY + 50 && mouseY <= centerY + 100) {
              return MENU_EXIT;
            }
          }
        }
        delay(10);
      }
    }

    void displayRules(int centerX, int centerY) {
      cleardevice();
      settextstyle(8, 0, 3);
      outtextxy(centerX - 200, centerY - 100, (char *)"Rules of XO Game:");
      outtextxy(centerX - 300, centerY - 50,
                (char *)"1. Players take turns placing their symbol (X or O).");
      outtextxy(centerX - 300, centerY,
                (char *)"2. The first to form a line of 5 wins.");
      outtextxy(centerX - 300, centerY + 50,
                (char *)"3. Lines can be horizontal, vertical, or diagonal.");
      outtextxy(centerX - 200, centerY + 150,
                (char *)"Click anywhere to return to menu.");

      // Wait for a click
      while (!ismouseclick(WM_LBUTTONDOWN)) {
        delay(10);
      }
      int dummyX, dummyY;
      getmouseclick(WM_LBUTTONDOWN, dummyX, dummyY);
      cleardevice();
    }

    //   Post-game mini-menu
    // Returns true if user picks "Play again", false if "Back to Menu"
    bool showPostGameMenu(int centerX, int centerY) {
      cleardevice();
      settextstyle(8, 0, 4);
      outtextxy(centerX - 100, centerY - 50, (char *)"1. Play again");
      outtextxy(centerX - 100, centerY, (char *)"2. Back to Menu");

      // Clear leftover clicks
      clearmouseclick(WM_LBUTTONDOWN);
      bool validChoice = false;
      bool playAgain = false;

      while (!validChoice) {
        if (ismouseclick(WM_LBUTTONDOWN)) {
          int postGameX, postGameY;
          getmouseclick(WM_LBUTTONDOWN, postGameX, postGameY);

          if (postGameX >= centerX - 150 && postGameX <= centerX + 150) {
            // "Play again"
            if (postGameY >= centerY - 50 && postGameY <= centerY) {
              validChoice = true;
              playAgain = true;
            }
            // "Back to Menu"
            else if (postGameY >= centerY && postGameY <= centerY + 50) {
              validChoice = true;
              playAgain = false;
            }
          }
        }
        delay(10);
      }
      return playAgain;
    }
  };

  class GameManager {
  public:
    // Player vs Player
    void runGame(Interface &ui, Menu &menu, Rules &theRules, Table &table,
                Player &player1, Player &player2, int centerX, int centerY,
                int gridStartX, int gridStartY, int infoX, int timerX,
                int infoY) {
      bool continuePlaying = true;
      while (continuePlaying) {
        bool gameOver = false;
        Player *currentPlayer = &player1;

        theRules.resetBoard();
        table.resetBoard();

        cleardevice();
        ui.drawGrid(gridStartX, gridStartY);

        int gridX, gridY;

        while (!gameOver) {
          ui.displayPlayerInfo(infoX, infoY, player1, player2, currentPlayer);

          // Initialize per-turn timer
          time_t turnStartTime = time(NULL);
          bool turnSkipped = false;

          while (true) {
            time_t currentTime = time(NULL);
            int elapsedTime = (int)difftime(currentTime, turnStartTime);
            int secondsLeft = TURN_TIME - elapsedTime;

            // Update and display timer
            ui.displayTimer(timerX, infoY, secondsLeft);

            // Check for timeout
            if (secondsLeft <= 0) {
              // Notify timeout
              settextstyle(8, 0, 3);
              string timeoutMsg = currentPlayer->getPlayerName() +
                                  " ran out of time! Turn skipped.";
              outtextxy(centerX - 200, gridStartY + BOARD_SIZE * CELL_SIZE + 20,
                        (char *)timeoutMsg.c_str());
              delay(2000);  // Wait for 2 seconds

              // Switch player
              currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
              turnSkipped = true;
              break;
            }

            // Handle click
            if (ui.handleClick(gridStartX, gridStartY, gridX, gridY)) {
              // Attempt the move
              if (theRules.makeMove(gridX, gridY,
                                    currentPlayer->getPlayerSymbol())) {
                ui.drawMove(gridStartX, gridStartY, gridX, gridY,
                            currentPlayer->getPlayerSymbol());

                // Check winner
                if (theRules.checkWinner(currentPlayer->getPlayerSymbol())) {
                  currentPlayer->incrementScore();
                  settextstyle(8, 0, 3);
                  string winnerMsg = currentPlayer->getPlayerName() + " wins!";
                  outtextxy(centerX - 80,
                            gridStartY + BOARD_SIZE * CELL_SIZE + 20,
                            (char *)winnerMsg.c_str());
                  delay(2000);
                  gameOver = true;
                  break;
                }

                // Switch player after a successful move
                currentPlayer = (currentPlayer == &player1) ? &player2 : &player1;
                break;
              }
            }

            delay(100);  // Small delay to prevent high CPU usage
          }

          if (gameOver) break;
        }

        // Post-game menu
        bool playAgain = menu.showPostGameMenu(centerX, centerY);
        if (!playAgain) {
          continuePlaying = false;
        }
      }
    }

    // Player vs Bot
    void runGameVsBot(Interface &ui, Menu &menu, Rules &theRules, Table &table,
                      Player &humanPlayer, Bot &botPlayer, int centerX,
                      int centerY, int gridStartX, int gridStartY, int infoX,
                      int timerX, int infoY) {
      bool continuePlaying = true;
      while (continuePlaying) {
        bool gameOver = false;
        Player *currentPlayer = &humanPlayer;  // Human starts first

        theRules.resetBoard();
        table.resetBoard();

        cleardevice();
        ui.drawGrid(gridStartX, gridStartY);

        int gridX, gridY;

        while (!gameOver) {
          ui.displayPlayerInfo(infoX, infoY, humanPlayer, botPlayer,
                              currentPlayer);

          // Initialize per-turn timer
          time_t turnStartTime = time(NULL);
          bool turnSkipped = false;

          while (true) {
            time_t currentTime = time(NULL);
            int elapsedTime = (int)difftime(currentTime, turnStartTime);
            int secondsLeft = TURN_TIME - elapsedTime;

            // Update and display timer
            ui.displayTimer(timerX, infoY, secondsLeft);

            // Check for timeout
            if (secondsLeft <= 0) {
              // Notify timeout
              settextstyle(8, 0, 3);
              string timeoutMsg = currentPlayer->getPlayerName() +
                                  " ran out of time! Turn skipped.";
              outtextxy(centerX - 200, gridStartY + BOARD_SIZE * CELL_SIZE + 20,
                        (char *)timeoutMsg.c_str());
              delay(2000);  // Wait for 2 seconds

              // Switch player
              currentPlayer = (currentPlayer == &humanPlayer)
                                  ? (Player *)&botPlayer
                                  : &humanPlayer;
              turnSkipped = true;
              break;
            }

            if (currentPlayer == &humanPlayer) {
              // Human's turn
              if (ui.handleClick(gridStartX, gridStartY, gridX, gridY)) {
                if (theRules.makeMove(gridX, gridY,
                                      humanPlayer.getPlayerSymbol())) {
                  ui.drawMove(gridStartX, gridStartY, gridX, gridY,
                              humanPlayer.getPlayerSymbol());

                  // Check winner
                  if (theRules.checkWinner(humanPlayer.getPlayerSymbol())) {
                    humanPlayer.incrementScore();
                    settextstyle(8, 0, 3);
                    string winnerMsg = humanPlayer.getPlayerName() + " wins!";
                    outtextxy(centerX - 80,
                              gridStartY + BOARD_SIZE * CELL_SIZE + 20,
                              (char *)winnerMsg.c_str());
                    delay(2000);
                    gameOver = true;
                    break;
                  }

                  // Switch to Bot after a successful move
                  currentPlayer = (Player *)&botPlayer;
                  break;
                }
              }
            } else {
              // Bot's turn
              // Start timing for bot's turn
              // Since bot acts immediately, no need to wait, but we'll still
              // track the time

              delay(800);
              pair<int, int> bestMove =
                  botPlayer.getBestMove(theRules, humanPlayer.getPlayerSymbol());

              if (bestMove.first == -1) {
                // No moves left, declare draw
                settextstyle(8, 0, 3);
                outtextxy(centerX - 30, gridStartY + BOARD_SIZE * CELL_SIZE + 20,
                          (char *)"Draw!");
                delay(2000);
                gameOver = true;
                break;
              } else {
                // Bot makes the move
                theRules.makeMove(bestMove.first, bestMove.second,
                                  botPlayer.getPlayerSymbol());
                ui.drawMove(gridStartX, gridStartY, bestMove.first,
                            bestMove.second, botPlayer.getPlayerSymbol());

                // Check winner
                if (theRules.checkWinner(botPlayer.getPlayerSymbol())) {
                  botPlayer.incrementScore();
                  settextstyle(8, 0, 3);
                  string winnerMsg = botPlayer.getPlayerName() + " wins!";
                  outtextxy(centerX - 80,
                            gridStartY + BOARD_SIZE * CELL_SIZE + 20,
                            (char *)winnerMsg.c_str());
                  delay(2000);
                  gameOver = true;
                  break;
                }

                // Switch back to Human
                currentPlayer = &humanPlayer;
                break;
              }
            }

            delay(100);  // Small delay to prevent high CPU usage
          }

          if (gameOver) break;
        }

        // Post-game menu
        bool playAgain = menu.showPostGameMenu(centerX, centerY);
        if (!playAgain) {
          continuePlaying = false;
        }
      }
    }
  };

  int main() {
    initwindow(1280, 720, "XO");

    // Create needed objects
    Menu menu;
    Interface ui;
    Rules theRules;
    Table table;
    GameManager gameManager;

    // Players
    Player player1;
    Player player2;
    player1.setPlayerSymbol('X');
    player2.setPlayerSymbol('O');

    // Bot
    Bot botPlayer("Bot", 'O');

    // Positions
    int centerX = 1280 / 2;
    int centerY = 720 / 2;
    int gridStartX = centerX - (BOARD_SIZE * CELL_SIZE) / 2;
    int gridStartY = centerY - (BOARD_SIZE * CELL_SIZE) / 2;
    int infoX = gridStartX - 220;
    int timerX = gridStartX + BOARD_SIZE * CELL_SIZE + 50;
    int infoY = gridStartY;

    bool exitGame = false;

    while (!exitGame) {
      cleardevice();
      menu.drawMenu(centerX, centerY);
      int choice = menu.handleMenuClick(centerX, centerY);

      if (choice == MENU_NEW_GAME) {
        // Prompt both players
        cleardevice();
        string name1 = ui.getTextInput(
            "Enter name for Player 1 (X):", gridStartX - 200, centerY - 100);
        player1.setPlayerName(name1.empty() ? "Player1" : name1);

        cleardevice();
        string name2 = ui.getTextInput(
            "Enter name for Player 2 (O):", gridStartX - 200, centerY - 100);
        player2.setPlayerName(name2.empty() ? "Player2" : name2);

        // Reset scores
        player1.resetScore();
        player2.resetScore();

        // Run Player vs Player with updated timing
        gameManager.runGame(ui, menu, theRules, table, player1, player2, centerX,
                            centerY, gridStartX, gridStartY, infoX, timerX,
                            infoY);
      } else if (choice == MENU_RULES) {
        menu.displayRules(centerX, centerY);
      } else if (choice == MENU_EXIT) {
        exitGame = true;
      } else if (choice == MENU_PLAY_BOT) {
        // Player vs Bot
        player1.resetScore();
        botPlayer.resetScore();

        cleardevice();
        string humanName = ui.getTextInput(
            "Enter your name (X):", gridStartX - 200, centerY - 100);
        player1.setPlayerName(humanName.empty() ? "Player" : humanName);
        player1.setPlayerSymbol('X');

        botPlayer.setPlayerName("Bot");
        botPlayer.setPlayerSymbol('O');

        // Run Player vs Bot with updated timing
        gameManager.runGameVsBot(ui, menu, theRules, table, player1, botPlayer,
                                centerX, centerY, gridStartX, gridStartY, infoX,
                                timerX, infoY);
      }
    }

    closegraph();
    return 0;
  }
    