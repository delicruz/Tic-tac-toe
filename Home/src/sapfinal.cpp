#include <graphics.h>
#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <limits>
#include <set> 


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

    int countEmptyCells()  {
        int emptyCount = 0;

        for (int r = 0; r < getRows(); ++r) {
            for (int c = 0; c < getCols(); ++c) {
                if (isCellEmpty(r, c)) {
                    emptyCount++;
                }
            }
        }

        return emptyCount;
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

    bool checkWinner(int row, int col) {
        char symbol = grid.getCell(row, col);

        // Horizontal check
        int count = 0;
        for (int j = 0; j < grid.getCols(); ++j) {
            count = (grid.getCell(row, j) == symbol) ? count + 1 : 0;
            if (count == 5) return true;
        }

        // Vertical check
        count = 0;
        for (int i = 0; i < grid.getRows(); ++i) {
            count = (grid.getCell(i, col) == symbol) ? count + 1 : 0;
            if (count == 5) return true;
        }

        // Diagonal (\) check
        count = 0;
        for (int i = -min(row, col); i < min(grid.getRows() - row, grid.getCols() - col); ++i) {
            count = (grid.getCell(row + i, col + i) == symbol) ? count + 1 : 0;
            if (count == 5) return true;
        }

        // Diagonal (/) check
        count = 0;
        for (int i = -min(row, grid.getCols() - 1 - col); i < min(grid.getRows() - row, col + 1); ++i) {
            count = (grid.getCell(row + i, col - i) == symbol) ? count + 1 : 0;
            if (count == 5) return true;
        }

        return false;
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

class BotManager {
private:
    Grid &grid;
    char botSymbol;
    char opponentSymbol;
    int lastRow, lastCol;

    // Dynamic depth adjustment based on the number of empty cells
    int getDynamicDepth() {
        int emptyCells = grid.countEmptyCells();
        if (emptyCells > 70) return 2;   // Early game: shallow search
        if (emptyCells > 50) return 3;   // Mid-game: deeper search
        return 4;                        // Late game: deepest search
    }

    // Evaluate the overall grid state
    int evaluateGrid() {
        int score = 0;
        for (int r = 0; r < grid.getRows(); ++r) {
            for (int c = 0; c < grid.getCols(); ++c) {
                if (grid.getCell(r, c) == botSymbol) {
                    score += evaluatePosition(r, c, botSymbol);
                } else if (grid.getCell(r, c) == opponentSymbol) {
                    score -= evaluatePosition(r, c, opponentSymbol) * 2;
                }
            }
        }
        return score;
    }

    // Evaluate a single position for a specific symbol
    int evaluatePosition(int row, int col, char symbol) {
        int score = 0;
        int centerRow = grid.getRows() / 2;
        int centerCol = grid.getCols() / 2;
        score += 50 - abs(centerRow - row) - abs(centerCol - col);
        score += countConsecutive(row, col, 0, 1, symbol);
        score += countConsecutive(row, col, 1, 0, symbol);
        score += countConsecutive(row, col, 1, 1, symbol);
        score += countConsecutive(row, col, -1, 1, symbol);
        return score;
    }

    // Count consecutive symbols in a direction
    int countConsecutive(int row, int col, int rowDir, int colDir, char symbol) {
        int count = 0, openEnds = 0;
        for (int i = 1; i < 5; ++i) {
            int newRow = row + i * rowDir;
            int newCol = col + i * colDir;
            if (newRow >= 0 && newRow < grid.getRows() && newCol >= 0 && newCol < grid.getCols()) {
                if (grid.getCell(newRow, newCol) == symbol) {
                    count++;
                } else if (grid.isCellEmpty(newRow, newCol)) {
                    openEnds++;
                    break;
                } else {
                    break;
                }
            }
        }
        for (int i = 1; i < 5; ++i) {
            int newRow = row - i * rowDir;
            int newCol = col - i * colDir;
            if (newRow >= 0 && newRow < grid.getRows() && newCol >= 0 && newCol < grid.getCols()) {
                if (grid.getCell(newRow, newCol) == symbol) {
                    count++;
                } else if (grid.isCellEmpty(newRow, newCol)) {
                    openEnds++;
                    break;
                } else {
                    break;
                }
            }
        }
        if (count >= 4) return (symbol == botSymbol ? 1100 : -1000);
        if (count == 3 && openEnds == 2) return (symbol == botSymbol ? 700 : -900);
        if (count == 3 && openEnds == 1) return (symbol == botSymbol ? 500 : -600);
        if (count == 2 && openEnds == 2) return (symbol == botSymbol ? 200 : -200);
        if (count == 2 && openEnds == 1) return (symbol == botSymbol ? 100 : -100);
        return 0;
    }

    // Minimax with dynamic depth and alpha-beta pruning
    int minimax(int depth, bool isMaximizing, int alpha, int beta) {
        if (grid.isFull() || depth == 0) return evaluateGrid();
        if (isMaximizing) {
            int bestScore = std::numeric_limits<int>::min();
            for (const auto &move : getCandidateMoves()) {
                grid.setCell(move.first, move.second, botSymbol);
                int score = minimax(depth - 1, false, alpha, beta);
                grid.setCell(move.first, move.second, ' ');
                bestScore = std::max(bestScore, score);
                alpha = std::max(alpha, bestScore);
                if (beta <= alpha) break;
            }
            return bestScore;
        } else {
            int bestScore = std::numeric_limits<int>::max();
            for (const auto &move : getCandidateMoves()) {
                grid.setCell(move.first, move.second, opponentSymbol);
                int score = minimax(depth - 1, true, alpha, beta);
                grid.setCell(move.first, move.second, ' ');
                bestScore = std::min(bestScore, score);
                beta = std::min(beta, bestScore);
                if (beta <= alpha) break;
            }
            return bestScore;
        }
    }

    // Get candidate moves around occupied cells
    vector<pair<int, int>> getCandidateMoves() {
        vector<pair<int, int>> candidates;
        for (int r = 0; r < grid.getRows(); ++r) {
            for (int c = 0; c < grid.getCols(); ++c) {
                if (grid.getCell(r, c) != ' ') {
                    for (int dr = -1; dr <= 1; ++dr) {
                        for (int dc = -1; dc <= 1; ++dc) {
                            int newRow = r + dr;
                            int newCol = c + dc;
                            if (newRow >= 0 && newRow < grid.getRows() &&
                                newCol >= 0 && newCol < grid.getCols() &&
                                grid.isCellEmpty(newRow, newCol)) {
                                candidates.emplace_back(newRow, newCol);
                            }
                        }
                    }
                }
            }
        }
        return candidates;
    }

public:
    BotManager(Grid &g, char symbol)
        : grid(g), botSymbol(symbol), opponentSymbol(symbol == 'O' ? 'X' : 'O'), lastRow(-1), lastCol(-1) {}

    void makeMove() {
        int depth = getDynamicDepth();
        int bestScore = std::numeric_limits<int>::min();
        int bestRow = -1, bestCol = -1;

        for (const auto &move : getCandidateMoves()) {
            int r = move.first, c = move.second;
            grid.setCell(r, c, botSymbol);
            int score = minimax(depth - 1, false, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
            grid.setCell(r, c, ' ');
            if (score > bestScore) {
                bestScore = score;
                bestRow = r;
                bestCol = c;
            }
        }

        if (bestRow != -1 && bestCol != -1) {
            grid.setCell(bestRow, bestCol, botSymbol);
            lastRow = bestRow;
            lastCol = bestCol;
            std::cout << "Bot places strategically at (" << bestRow << ", " << bestCol << ") with score: " << bestScore << "\n";
        } else {
            std::cout << "Bot could not find a valid move.\n";
        }
    }

    int getLastRow() const { return lastRow; }
    int getLastCol() const { return lastCol; }
};

// Main application class
class Application {
public:
    void run() {
        int rows = 10, cols = 10;
        string player1Name = "Player 1";
        string player2Name = "bot";
        bool player2IsBot = true; // Set to true if Player 2 is a bot

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

                if (x >= leftX && x <= leftX + BUTTON_WIDTH && y >= rowsPlusY && y <= rowsPlusY + BUTTON_HEIGHT && rows < 15) {
                    rows++;
                    redrawMenu = true;
                } else if (x >= leftX && x <= leftX + BUTTON_WIDTH && y >= rowsMinusY && y <= rowsMinusY + BUTTON_HEIGHT && rows > 10) {
                    rows--;
                    redrawMenu = true;
                } else if (x >= rightX && x <= rightX + BUTTON_WIDTH && y >= colsPlusY && y <= colsPlusY + BUTTON_HEIGHT && cols < 29) {
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
        /*WindowManager::setupMenuWindow();
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
        closegraph();*/

        

        // Phase 3: Game Window

    WindowManager::setupGameWindow(rows, cols);

    Grid grid(rows, cols);
    PlayerManager playerManager;
    GameLogic gameLogic(grid);
    BotManager botManager(grid, 'X');
    bool gameOver = false;
    grid.drawGrid();

    while (!gameOver) {
        cleardevice();
        grid.drawGrid();

        // Display player information
        setcolor(WHITE);
        settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);
        outtextxy(20, 10, (char *)("Player 1: " + player1Name + " (O)").c_str());
        outtextxy(20, 50, (char *)("Player 2: " + player2Name + " (Bot)").c_str());

        if (playerManager.getCurrentPlayer() == 'O') {
            // Player 1's turn
            if (ismouseclick(WM_LBUTTONDOWN)) {
                int x = mousex();
                int y = mousey();
                clearmouseclick(WM_LBUTTONDOWN);

                int row = (y - 100) / CELL_SIZE;
                int col = (x - 20) / CELL_SIZE;

                if (row >= 0 && row < rows && col >= 0 && col < cols && grid.isCellEmpty(row, col)) {
                    grid.setCell(row, col, 'O');

                    if (gameLogic.checkWinner(row, col)) {
                        displayWinner(player1Name);
                        gameOver = true;
                    } else if (grid.isFull()) {
                        displayDrawMessage();
                        gameOver = true;
                    } else {
                        playerManager.switchPlayer();
                    }
                }
            }
        } else {
            // Bot's turn
            botManager.makeMove();

            int botRow = botManager.getLastRow();
            int botCol = botManager.getLastCol();

            if (gameLogic.checkWinner(botRow, botCol)) {
                displayWinner(player2Name);
                gameOver = true;
            } else if (grid.isFull()) {
                displayDrawMessage();
                gameOver = true;
            } else {
                playerManager.switchPlayer();
            }
        }

        delay(100);
    }

    closegraph(); // Close the graphics window after the game ends
}

};

// Main function
int main() {
    Application app;
    app.run();
    return 0;
}



