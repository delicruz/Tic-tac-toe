#include <graphics.h>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

const int CELL_SIZE = 40;
const int BUTTON_WIDTH = 150;
const int BUTTON_HEIGHT = 50;

// Window Manager Class
class WindowManager {
public:
    static void initCenteredWindow(int windowWidth, int windowHeight, const char *title) {
        int screenWidth = 2560;  // 2K Screen Width
        int screenHeight = 1600; // 2K Screen Height

        // Adjust for centering
        int x = (screenWidth - windowWidth) / 2 - 610;
        int y = (screenHeight - windowHeight) / 2 - 410;

        initwindow(windowWidth, windowHeight, title, x, y);
    }

    static void setupMenuWindow() {
        const int menuWidth = 800;
        const int menuHeight = 600;
        initCenteredWindow(menuWidth, menuHeight, "Tic Tac Toe Menu");
    }

    static void setupGameWindow(int rows, int cols) {
        const int extraHeight = 150; // Space for player names above the grid
        const int gameWidth = cols * CELL_SIZE + 40; // Extra margin
        const int gameHeight = rows * CELL_SIZE + extraHeight + 40; // Extra margin
        initCenteredWindow(gameWidth, gameHeight, "Tic Tac Toe Game");
    }
};

struct WinSequence {
    bool hasWinner;
    int startRow, startCol;
    int endRow, endCol;
};

// Class to manage the grid
class Grid {
private:
    int rows, cols;
    vector<vector<char>> grid;
    const int offsetX = 20;  // Horizontal offset
    const int offsetY = 100; // Vertical offset for player names

public:
    Grid(int n, int m) : rows(n), cols(m) {
        grid.resize(rows, vector<char>(cols, ' '));
    }

    void drawWinningLine(int startRow, int startCol, int endRow, int endCol) {
    int startX = offsetX + startCol * CELL_SIZE + CELL_SIZE / 2;
    int startY = offsetY + startRow * CELL_SIZE + CELL_SIZE / 2;
    int endX = offsetX + endCol * CELL_SIZE + CELL_SIZE / 2;
    int endY = offsetY + endRow * CELL_SIZE + CELL_SIZE / 2;

    setcolor(RED);                 // Set the line color to red
    setlinestyle(SOLID_LINE, 0, 3); // Use a solid, thick line
    line(startX, startY, endX, endY); // Draw the line
}

    void drawGrid() {
        setcolor(WHITE);
        for (int i = 0; i <= cols; ++i) {
            line(offsetX + i * CELL_SIZE, offsetY, offsetX + i * CELL_SIZE, offsetY + rows * CELL_SIZE);
        }
        for (int i = 0; i <= rows; ++i) {
            line(offsetX, offsetY + i * CELL_SIZE, offsetX + cols * CELL_SIZE, offsetY + i * CELL_SIZE);
        }
        drawSymbols(); // Corrected placement of drawSymbols()
    }

    void drawSymbols() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (grid[i][j] != ' ') {
                    drawSymbol(i, j, grid[i][j]);
                }
            }
        }
    }

    void drawSymbol(int row, int col, char symbol) {
        int x = offsetX + col * CELL_SIZE + CELL_SIZE / 2;
        int y = offsetY + row * CELL_SIZE + CELL_SIZE / 2;

        if (symbol == 'O') {
            setcolor(LIGHTCYAN);
        } else {
            setcolor(RED);
        }

        settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);
        string sym(1, symbol);
        outtextxy(x - 10, y - 10, (char *)sym.c_str());
    }

    bool isCellEmpty(int row, int col) {
        return grid[row][col] == ' ';
    }

    void setCell(int row, int col, char symbol) {
        grid[row][col] = symbol;
    }

    char getCell(int row, int col) {
        return grid[row][col];
    }

    int getRows() { return rows; }
    int getCols() { return cols; }

    bool isFull() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                if (grid[i][j] == ' ') return false;
            }
        }
        return true;
    }
};


// Class to manage players
class PlayerManager {
private:
    char currentPlayer;

public:
    PlayerManager() : currentPlayer('O') {}

    char getCurrentPlayer() {
        return currentPlayer;
    }

    void switchPlayer() {
        currentPlayer = (currentPlayer == 'O') ? 'X' : 'O';
    }
};

// Class to manage game logic
class GameLogic {
private:
    Grid &grid;
public:
    GameLogic(Grid &g) : grid(g) {}

    // Return a WinSequence structure for the winning sequence
    WinSequence checkWinner(int row, int col) {
        char symbol = grid.getCell(row, col);
        int directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {-1, 1}};

        for (auto &dir : directions) {
            int count = 1;
            int startRow = row, startCol = col;
            int endRow = row, endCol = col;

            // Check forward direction
            for (int i = 1; i < 5; ++i) {
                int newRow = row + i * dir[0];
                int newCol = col + i * dir[1];
                if (newRow >= 0 && newRow < grid.getRows() && newCol >= 0 && newCol < grid.getCols() &&
                    grid.getCell(newRow, newCol) == symbol) {
                    count++;
                    endRow = newRow;
                    endCol = newCol;
                } else {
                    break;
                }
            }

            // Check backward direction
            for (int i = 1; i < 5; ++i) {
                int newRow = row - i * dir[0];
                int newCol = col - i * dir[1];
                if (newRow >= 0 && newRow < grid.getRows() && newCol >= 0 && newCol < grid.getCols() &&
                    grid.getCell(newRow, newCol) == symbol) {
                    count++;
                    startRow = newRow;
                    startCol = newCol;
                } else {
                    break;
                }
            }

            if (count >= 5) {
                return {true, startRow, startCol, endRow, endCol};
            }
        }

        return {false, -1, -1, -1, -1};
    }
};

// Class to manage the game menu
class GameMenu {
public:
     
     static void drawNameInputMenu(const string &player1Name, const string &player2Name, int currentName) {
        cleardevice();
        int screenWidth = getmaxx();
        int centerX = screenWidth / 2;

        setcolor(WHITE);
        settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);

        // Display current name being edited
        string promptText = (currentName == 1) ? "Enter Player 1's Name:" : "Enter Player 2's Name:";
        outtextxy(centerX - 200, 100, (char *)promptText.c_str());

        // Show names entered so far
        string player1Text = "Player 1: " + player1Name;
        string player2Text = "Player 2: " + player2Name;

        outtextxy(centerX - 200, 200, (char *)player1Text.c_str());
        outtextxy(centerX - 200, 250, (char *)player2Text.c_str());

        // Instructions
        string instructionText = "Type the name and press ENTER to continue.";
        outtextxy(centerX - 250, 400, (char *)instructionText.c_str());
    }
    
    static void drawMenu(int rows, int cols) {
        int screenWidth = getmaxx();
        int screenHeight = getmaxy();
        int centerX = screenWidth / 2;
        int centerY = screenHeight / 2;

        // Draw rows and columns text
        setcolor(WHITE);
        settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);
        string rowText = "Rows: " + to_string(rows);
        string colText = "Cols: " + to_string(cols);
        outtextxy(centerX - textwidth((char *)rowText.c_str()) / 2, centerY - 120, (char *)rowText.c_str());
        outtextxy(centerX - textwidth((char *)colText.c_str()) / 2, centerY - 70, (char *)colText.c_str());

        // Recalculate positions
        int leftX = centerX - BUTTON_WIDTH - 40;  // Left side for Rows
        int rightX = centerX + 40;               // Right side for Cols

        // Buttons for Rows
        drawButton(leftX, centerY - 40, "Rows +"); // Top left
        drawButton(leftX, centerY + 40, "Rows -"); // Bottom left

        // Buttons for Cols
        drawButton(rightX, centerY - 40, "Cols +"); // Top right
        drawButton(rightX, centerY + 40, "Cols -"); // Bottom right

        // Start Game button
        drawButton(centerX - BUTTON_WIDTH / 2, centerY + 150, "Start Game");
    }

    static void drawButton(int x, int y, const char *label) {
        setcolor(WHITE);
        setfillstyle(SOLID_FILL, BLACK);
        bar(x, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT);
        rectangle(x, y, x + BUTTON_WIDTH, y + BUTTON_HEIGHT);
        settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);
        int textWidth = textwidth((char *)label);
        int textHeight = textheight((char *)label);
        outtextxy(x + (BUTTON_WIDTH - textWidth) / 2, y + (BUTTON_HEIGHT - textHeight) / 2, (char *)label);
    }

};

// Function to display the winner
void displayWinner(const string &playerName) {
    setcolor(YELLOW);
    settextstyle(SIMPLEX_FONT, HORIZ_DIR, 3);
    
    // Create winner message with the player's name
    string winnerMsg = playerName + " Wins!";
    
    // Calculate screen and text dimensions
    int screenWidth = getmaxx();
    int screenHeight = getmaxy();
    int textWidth = textwidth((char *)winnerMsg.c_str());
    int textHeight = textheight((char *)winnerMsg.c_str());
    
    // Center the message on the screen
    int x = (screenWidth - textWidth) / 2;
    int y = (screenHeight - textHeight) / 2;
    
    // Display the message
    outtextxy(x, y, (char *)winnerMsg.c_str());
    delay(3000);
}


// Function to display a draw message
void displayDrawMessage() {
    setcolor(YELLOW);
    settextstyle(SIMPLEX_FONT, HORIZ_DIR, 3);
    string drawMsg = "It's a Draw!";
    int screenWidth = getmaxx();
    int screenHeight = getmaxy();
    int textWidth = textwidth((char *)drawMsg.c_str());
    int textHeight = textheight((char *)drawMsg.c_str());
    int x = (screenWidth - textWidth) / 2;
    int y = (screenHeight - textHeight) / 2;
    outtextxy(x, y, (char *)drawMsg.c_str());
    delay(3000);
}

// Main application class
class Application {
public:
    void run() {
        int rows = 10, cols = 10;
        string player1Name = "";
        string player2Name = "";

        // Phase 1: Menu for Grid Size
        WindowManager::setupMenuWindow();
        bool redrawMenu = true;

        // Define button positions
        int centerX = getmaxx() / 2;
        int centerY = getmaxy() / 2;
        int leftX = centerX - BUTTON_WIDTH - 40;
        int rightX = centerX + 40;
        int rowsPlusY = centerY - 40;
        int rowsMinusY = centerY + 40;
        int colsPlusY = centerY - 40;
        int colsMinusY = centerY + 40;
        int startButtonX = centerX - BUTTON_WIDTH / 2;
        int startButtonY = centerY + 150;

        while (true) {
            if (redrawMenu) {
                cleardevice();
                GameMenu::drawMenu(rows, cols);
                redrawMenu = false;
            }

            if (ismouseclick(WM_LBUTTONDOWN)) {
                int x = mousex();
                int y = mousey();
                clearmouseclick(WM_LBUTTONDOWN);

                if (x >= leftX && x <= leftX + BUTTON_WIDTH && y >= rowsPlusY && y <= rowsPlusY + BUTTON_HEIGHT && rows < 19) {
                    rows++;
                    redrawMenu = true;
                } else if (x >= leftX && x <= leftX + BUTTON_WIDTH && y >= rowsMinusY && y <= rowsMinusY + BUTTON_HEIGHT && rows > 10) {
                    rows--;
                    redrawMenu = true;
                } else if (x >= rightX && x <= rightX + BUTTON_WIDTH && y >= colsPlusY && y <= colsPlusY + BUTTON_HEIGHT && cols < 30) {
                    cols++;
                    redrawMenu = true;
                } else if (x >= rightX && x <= rightX + BUTTON_WIDTH && y >= colsMinusY && y <= colsMinusY + BUTTON_HEIGHT && cols > 10) {
                    cols--;
                    redrawMenu = true;
                } else if (x >= startButtonX && x <= startButtonX + BUTTON_WIDTH && y >= startButtonY && y <= startButtonY + BUTTON_HEIGHT) {
                    break; // Start Game
                }
            }

            delay(10);
        }

        closegraph(); // Close the grid size menu

        // Phase 2: Menu for Player Names
        WindowManager::setupMenuWindow();
        int currentName = 1;
        while (true) {
            GameMenu::drawNameInputMenu(player1Name, player2Name, currentName);
            if (kbhit()) {
                char key = getch();
                if (key == '\r') { // ENTER key to switch to next name
                    currentName++;
                    if (currentName > 2) break; // Exit name input phase
                } else if (key == '\b') { // BACKSPACE to remove last character
                    if (currentName == 1 && !player1Name.empty()) player1Name.pop_back();
                    if (currentName == 2 && !player2Name.empty()) player2Name.pop_back();
                } else { // Append character
                    if (currentName == 1) player1Name += key;
                    if (currentName == 2) player2Name += key;
                }
            }
            delay(50);
        }
        closegraph();

        // Phase 3: Game Window
        WindowManager::setupGameWindow(rows, cols);
        Grid grid(rows, cols);
        PlayerManager playerManager;
        GameLogic gameLogic(grid);
        bool gameOver = false;

        int countdown = 5; 
        time_t startTime = time(nullptr); // Record the start time

        grid.drawGrid();
        while (!gameOver) {
            setcolor(WHITE);
            settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);
            outtextxy(20, 10, (char *)("Player 1: " + player1Name + " (O)").c_str());
            outtextxy(20, 50, (char *)("Player 2: " + player2Name + " (X)").c_str());
            
            // Display timer at the top-right corner
            string timerText = "Time Left: " + to_string(countdown) + "s";
            int timerTextWidth = textwidth((char *)timerText.c_str());
            int screenWidth = getmaxx();
            outtextxy(screenWidth - timerTextWidth - 20, 10, (char *)timerText.c_str());

            // Update the timer
            if (time(nullptr) - startTime >= 1) { // Check if 1 second has passed
                countdown--;
                startTime = time(nullptr); // Reset start time for the next second
                if (countdown == 0) {
                if (playerManager.getCurrentPlayer() == 'O') {
                        displayWinner(player2Name); 
                    } else {
                        displayWinner(player1Name); 
                    }
                    gameOver = true;
                    break;
                }
            }


            if (ismouseclick(WM_LBUTTONDOWN)) {
                int x = mousex();
                int y = mousey();
                clearmouseclick(WM_LBUTTONDOWN);

                int row = (y - 100) / CELL_SIZE; 
                int col = (x - 20) / CELL_SIZE;  
                if (row >= 0 && row < rows && col >= 0 && col < cols && grid.isCellEmpty(row, col)) {
                    char player = playerManager.getCurrentPlayer();
                    grid.setCell(row, col, player);
                    cleardevice();
                    grid.drawGrid();

                   WinSequence winSeq = gameLogic.checkWinner(row, col);
            if (winSeq.hasWinner) {
                grid.drawWinningLine(winSeq.startRow, winSeq.startCol, winSeq.endRow, winSeq.endCol);
                gameOver = true;
                displayWinner(player == 'O' ? player1Name : player2Name);
            } else if (grid.isFull()) {
                gameOver = true;
                displayDrawMessage();
            } else {
                        playerManager.switchPlayer();
                        countdown = 5; // Reset timer for the next player
                        startTime = time(nullptr); // Reset start time
                    }
                }
            }
            delay(10);
        }
    }
};

// Main function
int main() {
    Application app;
    app.run();
    return 0;
}

