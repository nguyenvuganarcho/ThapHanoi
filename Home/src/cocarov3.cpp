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
#define MENU_SET_TIMER 5 
#define TURN_TIME 60

// Define message area boundaries globally
const int messageAreaX1 = 550; // Adjust based on your layout
const int messageAreaY1 = 675; // Example Y coordinate below the grid
const int messageAreaX2 = 1180; // Adjust based on your layout
const int messageAreaY2 = 720; // Adjust based on your layout

struct WinningLine {  // Structure to store winning line coordinates
    int startX;
    int startY;
    int endX;
    int endY;
};

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
            return false;  // Out of bounds
        }
        if (grid[x][y] != ' ') {
            return false;  // Cell already occupied
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
    bool checkWinner(char symbol, WinningLine &winLine) {  // Modified to include WinningLine
        // 1. Check vertical
        for (int x = 0; x < BOARD_SIZE; x++) {
            int count = 0;
            int startY = -1;
            for (int y = 0; y < BOARD_SIZE; y++) {
                if (getCell(x, y) == symbol) {
                    if (count == 0) startY = y;
                    count++;
                    if (count == 5) {
                        winLine = {x, startY, x, y};
                        return true;
                    }
                }
                else {
                    count = 0;
                }
            }
        }

        // 2. Check horizontal
        for (int y = 0; y < BOARD_SIZE; y++) {
            int count = 0;
            int startX = -1;
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (getCell(x, y) == symbol) {
                    if (count == 0) startX = x;
                    count++;
                    if (count == 5) {
                        winLine = {startX, y, x, y};
                        return true;
                    }
                }
                else {
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
                if (win) {
                    winLine = {x, y, x + 4, y + 4};
                    return true;
                }
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
                if (win) {
                    winLine = {x, y, x + 4, y - 4};
                    return true;
                }
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
            // Vertical lines
            line(startX + i * CELL_SIZE, startY, startX + i * CELL_SIZE,
                 startY + BOARD_SIZE * CELL_SIZE);
            // Horizontal lines
            line(startX, startY + i * CELL_SIZE, startX + BOARD_SIZE * CELL_SIZE,
                 startY + i * CELL_SIZE);
        }
    }

    // Handle single-click: returns true if valid cell
    bool handleClick(int startX, int startY, int &gridX, int &gridY) {
        if (ismouseclick(WM_LBUTTONDBLCLK)) {
            int mouseX, mouseY;
            getmouseclick(WM_LBUTTONDBLCLK, mouseX, mouseY);
            // Convert screen coords to grid coords
            gridX = (mouseX - startX) / CELL_SIZE;
            gridY = (mouseY - startY) / CELL_SIZE;

            clearmouseclick(WM_LBUTTONDBLCLK);  // Clear event
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
        }
        else if (symbol == 'O') {
            circle(centerX, centerY, CELL_SIZE / 4);
        }
    }

    // Show players info
    void displayPlayerInfo(int infoX, int infoY, const Player &p1,
                           const Player &p2, const Player *currentPlayer) {
        settextstyle(8, 0, 3);

        // Player 1
        string p1Info = p1.getPlayerName() + " (" + string(1, p1.getPlayerSymbol()) +
                        ") - Score: " + to_string(p1.getPlayerScore());
        outtextxy(infoX - 110, infoY, (char*)p1Info.c_str());   

        // Player 2
        string p2Info = p2.getPlayerName() + " (" + string(1, p2.getPlayerSymbol()) +
                        ") - Score: " + to_string(p2.getPlayerScore());
        outtextxy(infoX - 110, infoY + 30, (char*)p2Info.c_str());  

       
    }

    // Show countdown timer
    void displayTimer(int timerX, int timerY, int secondsLeft) {
        settextstyle(8, 0, 3);
        int minutes = secondsLeft / 60;
        int seconds = secondsLeft % 60;
        stringstream ss;
        ss << "Time Left: " << (minutes < 10 ? "0" : "") << minutes << ":"
           << (seconds < 10 ? "0" : "") << seconds;

        outtextxy(timerX, timerY, (char*)ss.str().c_str());  // Retained (char*) cast
    }

    // Prompt user for text input on-screen
    string getTextInput(const string &prompt, int x, int y) {
        string input;
        settextstyle(8, 0, 2);
        outtextxy(x, y, (char*)prompt.c_str());  // Retained (char*) cast

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
                }
                else if (ch == 8) {  // Backspace
                    if (!input.empty()) {
                        input.pop_back();
                        // Clear area to redraw text
                        setfillstyle(SOLID_FILL, BLACK);
                        bar(inputX, inputY - 3, inputX + 295, inputY + 17);
                        settextstyle(8, 0, 2);
                        outtextxy(x, y, (char*)prompt.c_str());  // Retained (char*) cast
                        setcolor(WHITE);
                        outtextxy(inputX, inputY, (char*)input.c_str());  // Retained (char*) cast
                    }
                }
                else if (isprint(ch) && input.size() < 25) {
                    input.push_back(ch);
                    outtextxy(inputX, inputY, (char*)input.c_str());  // Retained (char*) cast
                }
            }
            delay(10);
        }
        return input;
    }

    // Draw winning line
    void drawWinningLine(int startX, int startY, const WinningLine &winLine) {  // Added
        setcolor(RED);
        setlinestyle(SOLID_LINE, 0, 3);
        int screenStartX = startX + winLine.startX * CELL_SIZE + CELL_SIZE / 2;
        int screenStartY = startY + winLine.startY * CELL_SIZE + CELL_SIZE / 2;
        int screenEndX = startX + winLine.endX * CELL_SIZE + CELL_SIZE / 2;
        int screenEndY = startY + winLine.endY * CELL_SIZE + CELL_SIZE / 2;
        line(screenStartX, screenStartY, screenEndX, screenEndY);
        setcolor(WHITE);  // Reset color to default

        setlinestyle(SOLID_LINE, 0, 1);
    }

    // Function to display a message in the message area
    void displayMessage(const string &msg, int x1, int y1, int x2, int y2) {
        settextstyle(8, 0, 3); // Set text style
        setcolor(WHITE);        // Set text color

        // Display the message
        outtextxy(x1, y1, (char*)msg.c_str());
    }

    // Function to clear the message area
    void clearMessageArea(int x1, int y1, int x2, int y2) {
        setfillstyle(SOLID_FILL, BLACK); // Use background color
        bar(x1, y1, x2, y2);             // Draw filled rectangle to clear
        setcolor(WHITE);                  // Reset text color
    }
};

class Bot : public Player {
public:
    Bot(const string &botName = "Bot", char botSymbol = 'X') 
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
        const int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
        for (auto &dir : directions) {
            int count = 1;  // Includes the current cell
            count += countOneDirection(rules, startX, startY, -dir[0], -dir[1], symbol);
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
            }
            else {
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
        static const int directions[4][2] = { {1, 0}, {0, 1}, {1, 1}, {1, -1} };
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
            }
            else {
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
                }
                else if (c == ' ') {
                    gapCount++;
                }
                else {
                    // Either oppSymbol or '#'
                    blocked = true;
                }
            }
            if (!blocked && symCount >= N) {
                // Gap must be <= (4 - N)
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
        int bestScore = -999999;
        pair<int, int> bestCell(-1, -1);

        for (int x = 0; x < BOARD_SIZE; x++) {
            for (int y = 0; y < BOARD_SIZE; y++) {
                if (theRules.getCell(x, y) == ' ') {
                    int score =
                        calcScore(theRules, x, y, getPlayerSymbol(), opponentSymbol);
                    if (score > bestScore) {
                        bestScore = score;
                        bestCell = { x, y };
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
        outtextxy(centerX - 150, centerY - 150, (char*)"1. Player vs Player");  // Adjusted positions
        outtextxy(centerX - 150, centerY - 100, (char*)"2. Play vs Bot");        // Retained
        outtextxy(centerX - 150, centerY - 50, (char*)"3. Rules");             // Retained
        outtextxy(centerX - 150, centerY, (char*)"4. Set Timer");              // New Timer Option
        outtextxy(centerX - 150, centerY + 50, (char*)"5. Exit");              // Retained
    }

    int handleMenuClick(int centerX, int centerY) {
        while (true) {
            if (ismouseclick(WM_LBUTTONDOWN)) {
                int mouseX, mouseY;
                getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);

                // We'll check bounding boxes
                if (mouseX >= centerX - 150 && mouseX <= centerX + 150) {
                    // 1. Player vs Player
                    if (mouseY >= centerY - 150 && mouseY <= centerY - 100) {
                        return MENU_NEW_GAME;
                    }
                    // 2. Play vs Bot
                    if (mouseY >= centerY - 100 && mouseY <= centerY - 50) {
                        return MENU_PLAY_BOT;
                    }
                    // 3. Rules
                    if (mouseY >= centerY - 50 && mouseY <= centerY) {
                        return MENU_RULES;
                    }
                    // 4. Set Timer
                    if (mouseY >= centerY && mouseY <= centerY + 50) {
                        return MENU_SET_TIMER;
                    }
                    // 5. Exit
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
        outtextxy(centerX - 200, centerY - 100, (char*)"Rules of XO Game:");  
        outtextxy(centerX - 300, centerY - 50,
            (char*)"1. Players take turns placing their symbol (X or O).");       
        outtextxy(centerX - 300, centerY,
            (char*)"2. The first to form a line of 5 wins.");                     
        outtextxy(centerX - 300, centerY + 50,
            (char*)"3. Lines can be horizontal, vertical, or diagonal.");        
        outtextxy(centerX - 200, centerY + 150,
            (char*)"Click anywhere to return to menu.");                          

        // Wait for a click
        while (!ismouseclick(WM_LBUTTONDOWN)) {
            delay(10);
        }
        int dummyX, dummyY;
        getmouseclick(WM_LBUTTONDOWN, dummyX, dummyY);
        cleardevice();
    }

    // Function to display and handle timer selection
    int setTimer(int centerX, int centerY) {
        cleardevice();
        settextstyle(8, 0, 4);
        outtextxy(centerX - 150, centerY - 100, (char*)"Select Turn Time:");
        outtextxy(centerX - 100, centerY - 50, (char*)"1. 10 Seconds");
        outtextxy(centerX - 100, centerY, (char*)"2. 30 Seconds");
        outtextxy(centerX - 100, centerY + 50, (char*)"3. 1 Minute");
        outtextxy(centerX - 150, centerY + 150, (char*)"Click to confirm selection.");

        int selectedTime = 60;  // Default to 60 seconds

        while (true) {
            if (ismouseclick(WM_LBUTTONDOWN)) {
                int mouseX, mouseY;
                getmouseclick(WM_LBUTTONDOWN, mouseX, mouseY);

                // Define clickable areas for each option
                if (mouseX >= centerX - 100 && mouseX <= centerX + 100) {
                    if (mouseY >= centerY - 50 && mouseY <= centerY - 25) {
                        selectedTime = 10;
                        break;
                    }
                    if (mouseY >= centerY && mouseY <= centerY + 25) {
                        selectedTime = 30;
                        break;
                    }
                    if (mouseY >= centerY + 50 && mouseY <= centerY + 75) {
                        selectedTime = 60;
                        break;
                    }
                }
            }
            delay(10);
        }

        cleardevice();
        settextstyle(8, 0, 3);
        string confirmation = "Turn time set to " + to_string(selectedTime) + " seconds.";
        outtextxy(centerX - 200, centerY, (char*)confirmation.c_str());  // Retained (char*) cast
        delay(3000);  // Display confirmation for 3 seconds
        return selectedTime;
    }

    // Post-game mini-menu
    // Returns true if user picks "Play again", false if "Back to Menu"
    bool showPostGameMenu(int centerX, int centerY) {
        cleardevice();
        settextstyle(8, 0, 4);
        outtextxy(centerX - 150, centerY - 50, (char*)"1. Play again");     
        outtextxy(centerX - 150, centerY, (char*)"2. Back to Menu");      

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
                int infoY, int turnTime) {  // Added turnTime parameter
        bool continuePlaying = true;
        while (continuePlaying) {
            bool gameOver = false;
            Player *currentPlayer = &player1;

            theRules.resetBoard();
            table.resetBoard();

            cleardevice();
            ui.drawGrid(gridStartX, gridStartY);

            int gridX, gridY;
            WinningLine winLine;  // Added

            while (!gameOver) {
                ui.displayPlayerInfo(infoX, infoY, player1, player2, currentPlayer);

                // Initialize per-turn timer
                time_t turnStartTime = time(NULL);
                bool turnSkipped = false;

                while (true) {
                    time_t currentTime = time(NULL);
                    int elapsedTime = (int)difftime(currentTime, turnStartTime);
                    int secondsLeft = turnTime - elapsedTime;  // Use turnTime variable

                    // Update and display timer
                    ui.displayTimer(timerX, infoY, secondsLeft);

                    // Check for timeout
                    if (secondsLeft <= 0) {
                        // Display timeout message
                        string timeoutMsg = currentPlayer->getPlayerName() +
                            " ran out of time! Turn skipped.";
                        ui.displayMessage(timeoutMsg, messageAreaX1 - 175, messageAreaY1,
                                          messageAreaX2, messageAreaY2);

                        delay(10000);  // 10 seconds delay

                        // Clear message area after delay
                        ui.clearMessageArea(messageAreaX1 -175, messageAreaY1,
                                            messageAreaX2, messageAreaY2);

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
                            if (theRules.checkWinner(currentPlayer->getPlayerSymbol(), winLine)) {  // Modified
                                currentPlayer->incrementScore();
                                // Draw the red winning line
                                ui.drawWinningLine(gridStartX, gridStartY, winLine);

                                // Display winner message
                                string winnerMsg = currentPlayer->getPlayerName() + " wins!";
                                ui.displayMessage(winnerMsg, messageAreaX1, messageAreaY1,
                                                  messageAreaX2, messageAreaY2);

                                delay(10000);  // 10 seconds delay

                                // Clear message area after delay
                                ui.clearMessageArea(messageAreaX1, messageAreaY1,
                                                    messageAreaX2, messageAreaY2);

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
                     int timerX, int infoY, int turnTime) {  // Added turnTime parameter
        bool continuePlaying = true;
        while (continuePlaying) {
            bool gameOver = false;
            Player *currentPlayer = &botPlayer;  // Bot starts first

            theRules.resetBoard();
            table.resetBoard();

            cleardevice();
            ui.drawGrid(gridStartX, gridStartY);

            int gridX, gridY;
            WinningLine winLine;  // Added

            while (!gameOver) {
                ui.displayPlayerInfo(infoX, infoY, humanPlayer, botPlayer,
                                     currentPlayer);

                // Initialize per-turn timer
                time_t turnStartTime = time(NULL);
                bool turnSkipped = false;

                while (true) {
                    time_t currentTime = time(NULL);
                    int elapsedTime = (int)difftime(currentTime, turnStartTime);
                    int secondsLeft = turnTime - elapsedTime;  // Use turnTime variable

                    // Update and display timer
                    ui.displayTimer(timerX, infoY, secondsLeft);

                    // Check for timeout
                    if (secondsLeft <= 0) {
                        // Display timeout message
                        string timeoutMsg = currentPlayer->getPlayerName() +
                            " ran out of time! Turn skipped.";
                        ui.displayMessage(timeoutMsg, messageAreaX1 - 175, messageAreaY1,
                                          messageAreaX2, messageAreaY2);

                        delay(2000);  // 2           seconds delay

                        // Clear message area after delay
                        ui.clearMessageArea(messageAreaX1 - 175, messageAreaY1,
                                            messageAreaX2, messageAreaY2);

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
                                if (theRules.checkWinner(humanPlayer.getPlayerSymbol(), winLine)) {  // Modified
                                    humanPlayer.incrementScore();
                                    // Draw the red winning line
                                    ui.drawWinningLine(gridStartX, gridStartY, winLine);

                                    // Display winner message
                                    string winnerMsg = humanPlayer.getPlayerName() + " wins!";
                                    ui.displayMessage(winnerMsg, messageAreaX1, messageAreaY1,
                                                      messageAreaX2, messageAreaY2);

                                    delay(10000);  // 10 seconds delay

                                    // Clear message area after delay
                                    ui.clearMessageArea(messageAreaX1, messageAreaY1,
                                                        messageAreaX2, messageAreaY2);

                                    gameOver = true;
                                    break;
                                }

                                // Switch to Bot after a successful move
                                currentPlayer = (Player *)&botPlayer;
                                break;
                            }
                        }
                    }
                    else {
                        // Bot's turn
                        delay(800);
                        pair<int, int> bestMove =
                            botPlayer.getBestMove(theRules, humanPlayer.getPlayerSymbol());

                        if (bestMove.first == -1) {
                            // No moves left, declare draw
                            string drawMsg = "Draw!";
                            ui.displayMessage(drawMsg, messageAreaX1, messageAreaY1,
                                              messageAreaX2, messageAreaY2);
                            delay(10000);  // 10 seconds delay

                            // Clear message area after delay
                            ui.clearMessageArea(messageAreaX1, messageAreaY1,
                                                messageAreaX2, messageAreaY2);

                            gameOver = true;
                            break;
                        }
                        else {
                            // Bot makes the move
                            theRules.makeMove(bestMove.first, bestMove.second,
                                botPlayer.getPlayerSymbol());
                            ui.drawMove(gridStartX, gridStartY, bestMove.first,
                                bestMove.second, botPlayer.getPlayerSymbol());

                            // Check winner
                            if (theRules.checkWinner(botPlayer.getPlayerSymbol(), winLine)) {  // Modified
                                botPlayer.incrementScore();
                                // Draw the red winning line
                                ui.drawWinningLine(gridStartX, gridStartY, winLine);

                                // Display winner message
                                string winnerMsg = botPlayer.getPlayerName() + " wins!";
                                ui.displayMessage(winnerMsg, messageAreaX1, messageAreaY1,
                                                  messageAreaX2, messageAreaY2);

                                delay(10000);  // 10 seconds delay

                                // Clear message area after delay
                                ui.clearMessageArea(messageAreaX1, messageAreaY1,
                                                    messageAreaX2, messageAreaY2);

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
    player1.setPlayerSymbol('X');  // Player 1 is 'X'
    player2.setPlayerSymbol('O');  // Player 2 is 'O'

    // Bot
    Bot botPlayer("Bot", 'X');  // Bot is 'X'

    // Positions
    int centerX = 1280 / 2;
    int centerY = 720 / 2;
    int gridStartX = centerX - (BOARD_SIZE * CELL_SIZE) / 2;
    int gridStartY = centerY - (BOARD_SIZE * CELL_SIZE) / 2;
    int infoX = gridStartX - 220;  // Positioned to the left of the grid
    int timerX = gridStartX + BOARD_SIZE * CELL_SIZE + 50;  // Positioned to the right
    int infoY = gridStartY;

    // Default turn time (can be overridden by user)
    int turnTime = 60;  // Default to 60 seconds

    bool exitGame = false;

    while (!exitGame) {
        cleardevice();
        menu.drawMenu(centerX, centerY);
        int choice = menu.handleMenuClick(centerX, centerY);

        if (choice == MENU_NEW_GAME) {
            // Player vs Player mode
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

            // Run Player vs Player with selected turnTime
            gameManager.runGame(ui, menu, theRules, table, player1, player2, centerX,
                               centerY, gridStartX, gridStartY, infoX, timerX,
                               infoY, turnTime);
        }
        else if (choice == MENU_PLAY_BOT) {
            // Player vs Bot
            player1.resetScore();
            botPlayer.resetScore();

            cleardevice();
            string humanName = ui.getTextInput(
                "Enter your name (O):", gridStartX - 200, centerY - 100);  // Updated prompt position
            player1.setPlayerName(humanName.empty() ? "Player" : humanName);
            player1.setPlayerSymbol('O');  // Human is 'O'

            botPlayer.setPlayerName("Bot");
            botPlayer.setPlayerSymbol('X');  // Bot is 'X'

            // Run Player vs Bot with selected turnTime
            gameManager.runGameVsBot(ui, menu, theRules, table, player1, botPlayer,
                                     centerX, centerY, gridStartX, gridStartY, infoX,
                                     timerX, infoY, turnTime);
        }
        else if (choice == MENU_RULES) {
            menu.displayRules(centerX, centerY);
        }
        else if (choice == MENU_SET_TIMER) {
            // Set Timer
            int selectedTime = menu.setTimer(centerX, centerY);
            turnTime = selectedTime;  // Update the global turnTime variable
        }
        else if (choice == MENU_EXIT) {
            exitGame = true;
        }
    }

    closegraph();
    return 0;
}
