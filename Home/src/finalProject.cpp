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

    static void setupGameModeWindow() {
        const int modeWidth = 800;
        const int modeHeight = 600;
        initCenteredWindow(modeWidth, modeHeight, "Select Game Mode");
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

    void drawWinningLine(int startRow, int startCol, int endRow, int endCol) {
    int startX = offsetX + startCol * CELL_SIZE + CELL_SIZE / 2;
    int startY = offsetY + startRow * CELL_SIZE + CELL_SIZE / 2;
    int endX = offsetX + endCol * CELL_SIZE + CELL_SIZE / 2;
    int endY = offsetY + endRow * CELL_SIZE + CELL_SIZE / 2;

    setcolor(RED);                 // Set the line color to red
    setlinestyle(SOLID_LINE, 0, 3); // Use a solid, thick line
    line(startX, startY, endX, endY); // Draw the line
    setlinestyle(SOLID_LINE, 0, 1);
}
    
    void drawGrid() {
        setcolor(WHITE);
        for (int i = 0; i <= cols; ++i) {
            line(offsetX + i * CELL_SIZE, offsetY, offsetX + i * CELL_SIZE, offsetY + rows * CELL_SIZE);
        }
        for (int i = 0; i <= rows; ++i) {
            line(offsetX, offsetY + i * CELL_SIZE, offsetX + cols * CELL_SIZE, offsetY + i * CELL_SIZE);
        }
        drawSymbols();
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

    void clearGrid() {
        for (auto &row : grid) {
            fill(row.begin(), row.end(), ' '); // Reset each cell to ' '
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

struct WinSequence {
    bool hasWinner;
    int startRow, startCol;
    int endRow, endCol;
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
    static void drawGameModeMenu(bool redraw) {
        if (redraw) {
            cleardevice();
            int screenWidth = getmaxx();
            int screenHeight = getmaxy();
            int centerX = screenWidth / 2;

            setcolor(WHITE);
            settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);

            // Draw buttons for game modes
            drawButton(centerX - BUTTON_WIDTH - 20, 200, "Player vs Player");
            drawButton(centerX + 20, 200, "Player vs Bot");
        }
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

    static void drawTimerMenu() {
        int centerX = getmaxx() / 2;

        setcolor(WHITE);
        settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);

        // Draw buttons for timer options
        drawButton(centerX - BUTTON_WIDTH, 200, "10 Seconds");
        drawButton(centerX + 10, 200, "30 Seconds");
        drawButton(centerX - BUTTON_WIDTH / 2, 300, "60 Seconds");

        // Add a title
        string title = "Select Timer Mode";
        settextstyle(SIMPLEX_FONT, HORIZ_DIR, 3);
        outtextxy(centerX - 120, 100, (char*)title.c_str());
        cout << "Me may beo" << endl;
    }

    static void drawNameInputMenu(const string &player1Name, const string &player2Name, int currentName, bool vsBot) {
        cleardevice();
        int screenWidth = getmaxx();
        int centerX = screenWidth / 2;

        setcolor(WHITE);
        settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);

        // Display current name being edited
        string promptText = (currentName == 1) ? "Enter Player 1's Name(5 characters only):" : (vsBot ? "Bot's Name: Bot" : "Enter Player 2's Name(5 characters only):");
        outtextxy(centerX - 200, 100, (char *)promptText.c_str());

        // Show names entered so far
        string player1Text = "Player 1: " + player1Name;
        string player2Text = vsBot ? "Player 2: Bot" : "Player 2: " + player2Name;

        outtextxy(centerX - 200, 200, (char *)player1Text.c_str());
        outtextxy(centerX - 200, 250, (char *)player2Text.c_str());

        // Instructions
        string instructionText = "Type the name and press ENTER to continue.";
        outtextxy(centerX - 250, 400, (char *)instructionText.c_str());
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

// BotManager class for bot strategy and decision making
class BotManager {
private:
    Grid &grid;
    char botSymbol;
    GameLogic gameLogic;
    char opponentSymbol;
    int lastRow, lastCol;

    // Evaluate the overall grid state
    int evaluateGrid() {
        int score = 0;

        // Evaluate each cell
        for (int r = 0; r < grid.getRows(); ++r) {
            for (int c = 0; c < grid.getCols(); ++c) {
                if (grid.getCell(r, c) == botSymbol) {
                    score += evaluatePosition(r, c, botSymbol);
                } else if (grid.getCell(r, c) == opponentSymbol) {
                    score -= evaluatePosition(r, c, opponentSymbol) * 2; // Penalize opponent progress
                }
            }
        }
        return score;
    }

    // Evaluate a single position for a specific symbol
   int evaluatePosition(int row, int col, char symbol) {
    int score = 0;

    // Reward moves closer to the center
    int centerRow = grid.getRows() / 2;
    int centerCol = grid.getCols() / 2;
    score += 50 - abs(centerRow - row) - abs(centerCol - col);

    // Add existing logic for consecutive counts
    score += countConsecutive(row, col, 0, 1, symbol);  // Horizontal
    score += countConsecutive(row, col, 1, 0, symbol);  // Vertical
    score += countConsecutive(row, col, 1, 1, symbol);  // Diagonal 
    score += countConsecutive(row, col, -1, 1, symbol); // Diagonal 

    return score;
}

int countSequence(int row, int col, int rowDir, int colDir, char symbol) {
    int count = 0;

    // Count consecutive symbols in one direction
    for (int i = 1; i < 5; ++i) {
        int newRow = row + i * rowDir;
        int newCol = col + i * colDir;
        if (newRow >= 0 && newRow < grid.getRows() && newCol >= 0 && newCol < grid.getCols()) {
            if (grid.getCell(newRow, newCol) == symbol) {
                count++;
            } else {
                break; // Stop counting if blocked by an opponent or out of bounds
            }
        }
    }

    // Count consecutive symbols in the opposite direction
    for (int i = 1; i < 5; ++i) {
        int newRow = row - i * rowDir;
        int newCol = col - i * colDir;
        if (newRow >= 0 && newRow < grid.getRows() && newCol >= 0 && newCol < grid.getCols()) {
            if (grid.getCell(newRow, newCol) == symbol) {
                count++;
            } else {
                break; // Stop counting if blocked by an opponent or out of bounds
            }
        }
    }

    return count; // Return the total consecutive symbols
}


    // Count consecutive symbols in a direction
    int countConsecutive(int row, int col, int rowDir, int colDir, char symbol) {
        int count = 0, openEnds = 0;

        // Count consecutive symbols in one direction
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

        // Count consecutive symbols in the opposite direction
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

        // Scoring adjustments for defensiveness
        if (count >= 4) return (symbol == botSymbol ? 1000 : -1500);  // Winning or critical threat
        if (count == 3 && openEnds == 2) return (symbol == botSymbol ? 700 : -900);  // Open-ended 3 (very dangerous)
        if (count == 3 && openEnds == 1) return (symbol == botSymbol ? 500 : -600);  // Blocked 3 (important to block)
        if (count == 2 && openEnds == 2) return (symbol == botSymbol ? 200 : -200);  // Open-ended 2 (potential to grow)
        if (count == 2 && openEnds == 1) return (symbol == botSymbol ? 100 : -100);  // Blocked 2 (minor threat)
        
        return 0;
    }

    int minimax(int depth, bool isMaximizing, int alpha, int beta) {
    if (grid.isFull() || depth == 0) {
        return evaluateGrid(); // Evaluate the grid if at terminal depth or full grid
    }

    vector<pair<int, int>> candidateMoves = getCandidateMoves();

    if (isMaximizing) {
        int bestScore = std::numeric_limits<int>::min();
        for (const auto &move : candidateMoves) {
            int r = move.first, c = move.second;
            grid.setCell(r, c, botSymbol); // Simulate bot's move
            int score = minimax(depth - 1, false, alpha, beta); // Recursively minimize
            grid.setCell(r, c, ' '); // Undo the move
            bestScore = std::max(bestScore, score);
            alpha = std::max(alpha, bestScore);
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return bestScore;
    } else {
        int bestScore = std::numeric_limits<int>::max();
        for (const auto &move : candidateMoves) {
            int r = move.first, c = move.second;
            grid.setCell(r, c, opponentSymbol); // Simulate opponent's move
            int score = minimax(depth - 1, true, alpha, beta); // Recursively maximize
            grid.setCell(r, c, ' '); // Undo the move
            bestScore = std::min(bestScore, score);
            beta = std::min(beta, bestScore);
            if (beta <= alpha) break; // Alpha-beta pruning
        }
        return bestScore;
    }
}


    // Check if the bot or opponent can win in the next move
    bool canWinNextMove(char symbol, int &winRow, int &winCol) {
    for (int r = 0; r < grid.getRows(); ++r) {
        for (int c = 0; c < grid.getCols(); ++c) {
            if (grid.isCellEmpty(r, c)) {
                grid.setCell(r, c, symbol); // Simulate bot's move
                if (checkWinner(r, c)) {
                    grid.setCell(r, c, ' '); // Undo move
                    winRow = r;
                    winCol = c;
                    return true; // Winning move found
                }
                grid.setCell(r, c, ' '); // Undo move
            }
        }
    }
    return false; // No winning move found
}

    bool checkWinner(int row, int col) {
    // Call the updated checkWinner from GameLogic
    WinSequence winSeq = gameLogic.checkWinner(row, col);

    if (winSeq.hasWinner) {
        // Highlight the winning sequence on the grid
        grid.drawWinningLine(winSeq.startRow, winSeq.startCol, winSeq.endRow, winSeq.endCol);
        return true; // Winner detected
    }
    return false; // No winner
}

    // Get candidate moves near occupied cells
    vector<pair<int, int>> getCandidateMoves() {
        set<pair<int, int>> candidates;

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
                                candidates.insert({newRow, newCol});
                            }
                        }
                    }
                }
            }
        }
        return vector<pair<int, int>>(candidates.begin(), candidates.end());
    }

    // Detect and block the strongest threat from the opponent
  bool detectAndBlockThreat(char symbol, int &threatRow, int &threatCol) {
    bool fourConsecutiveBlocked = false;

    for (int r = 0; r < grid.getRows(); ++r) {
        for (int c = 0; c < grid.getCols(); ++c) {
            if (grid.isCellEmpty(r, c)) {
                // Simulate placing the player's symbol in this cell
                grid.setCell(r, c, symbol);

                // **1. Check for 4 consecutive symbols**
                int directions[4][2] = {
                    {0, 1},  // Horizontal
                    {1, 0},  // Vertical
                    {1, 1},  // Diagonal
                    {-1, 1}  // Anti-diagonal
                };

                for (auto &dir : directions) {
                    if (countSequence(r, c, dir[0], dir[1], symbol) >= 4) {
                        // Debugging output
                        cout << "Detected 4-symbol threat at (" << r << ", " << c << ")\n";

                        // Block the 4-symbol threat
                        grid.setCell(r, c, ' '); // Undo the move
                        threatRow = r;
                        threatCol = c;
                        return true; // Immediately block and stop further evaluation
                    }
                }

                // **2. Check for open-ended 3-symbol threats**
                if (!fourConsecutiveBlocked && countOpenConsecutive(r, c, symbol) >= 3) {
                    // Debugging output
                    cout << "Detected open-ended 3-symbol threat at (" << r << ", " << c << ")\n";

                    // Tentatively block the 3-symbol threat (if no 4-symbol threat exists)
                    grid.setCell(r, c, ' '); // Undo the move
                    threatRow = r;
                    threatCol = c;

                    // Do not immediately return, continue to ensure no 4-symbol threats exist
                    fourConsecutiveBlocked = true;
                }

                grid.setCell(r, c, ' '); // Undo the move
            }
        }
    }

    // If a 3-symbol threat was found and no 4-symbol threat exists, block it
    return fourConsecutiveBlocked;
}


int countOpenConsecutive(int row, int col, char symbol) {
    int directions[4][2] = {
        {0, 1},  // Horizontal
        {1, 0},  // Vertical
        {1, 1},  // Diagonal 
        {-1, 1}  // Diagonal 
    };

    int maxConsecutive = 0;

    for (auto &dir : directions) {
        int consecutive = 0;
        bool openStart = false, openEnd = false;

        // Check forward direction
        for (int i = 1; i < 5; ++i) {
            int newRow = row + i * dir[0];
            int newCol = col + i * dir[1];
            if (newRow >= 0 && newRow < grid.getRows() && newCol >= 0 && newCol < grid.getCols()) {
                if (grid.getCell(newRow, newCol) == symbol) {
                    consecutive++;
                } else if (grid.isCellEmpty(newRow, newCol)) {
                    openEnd = true;
                    break;
                } else {
                    break;
                }
            }
        }

        // Check backward direction
        for (int i = 1; i < 5; ++i) {
            int newRow = row - i * dir[0];
            int newCol = col - i * dir[1];
            if (newRow >= 0 && newRow < grid.getRows() && newCol >= 0 && newCol < grid.getCols()) {
                if (grid.getCell(newRow, newCol) == symbol) {
                    consecutive++;
                } else if (grid.isCellEmpty(newRow, newCol)) {
                    openStart = true;
                    break;
                } else {
                    break;
                }
            }
        }

        // Consider the sequence valid if it's open on at least one end
        if (openStart || openEnd) {
            maxConsecutive = max(maxConsecutive, consecutive);
        }
    }

    return maxConsecutive;
}

public:
    BotManager(Grid &g, char symbol)
        : grid(g), botSymbol(symbol), gameLogic(g), opponentSymbol(symbol == 'O' ? 'X' : 'O'), lastRow(-1), lastCol(-1) {}

    void makeMove() {
    int winRow, winCol;
    int threatRow, threatCol;
    int forkRow, forkCol;

    // Check if the bot can win immediately
    if (canWinNextMove(botSymbol, winRow, winCol)) {
        grid.setCell(winRow, winCol, botSymbol);
        lastRow = winRow;
        lastCol = winCol;
        cout << "Bot wins immediately by placing at (" << winRow << ", " << winCol << ")." << endl;
        return;
    }

    // Check if the player has a winning move and block it
    if (detectAndBlockThreat(opponentSymbol, threatRow, threatCol)) {
        grid.setCell(threatRow, threatCol, botSymbol);
        lastRow = threatRow;
        lastCol = threatCol;
        cout << "Bot blocks at (" << threatRow << ", " << threatCol << ")." << endl;
        return;
    }

    // Strategic move using Minimax
    int bestScore = std::numeric_limits<int>::min();
    int bestRow = -1, bestCol = -1;
    vector<pair<int, int>> candidateMoves = getCandidateMoves();

    for (const auto &move : candidateMoves) {
        int r = move.first, c = move.second;
        grid.setCell(r, c, botSymbol);
        int score = minimax(3, false, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        grid.setCell(r, c, ' '); // Undo move

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
        cout << "Bot places strategically at (" << bestRow << ", " << bestCol << ")." << endl;
    } else {
        cout << "Bot could not find a valid move." << endl;
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
        string player1Name = "", player2Name = "";
        bool vsBot = false;
        int timerMode = 30; // Default timer mode   
    

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

        // Phase 2: Timer Mode Selection
    WindowManager::setupMenuWindow();
    bool redrawTimerMenu = true;
    while (true) {
        if (redrawTimerMenu) {
            cleardevice();
            GameMenu::drawTimerMenu();
            redrawTimerMenu = false;
        }

        if (ismouseclick(WM_LBUTTONDOWN)) {
            int x = mousex();
            int y = mousey();
            clearmouseclick(WM_LBUTTONDOWN);

            if (x >= centerX - BUTTON_WIDTH && x <= centerX && y >= 200 && y <= 250) {
                timerMode = 10; // 10 seconds
                break;
            } else if (x >= centerX + 10 && x <= centerX + BUTTON_WIDTH + 10 && y >= 200 && y <= 250) {
                timerMode = 30; // 30 seconds
                break;
            } else if (x >= centerX - BUTTON_WIDTH / 2 && x <= centerX + BUTTON_WIDTH / 2 && y >= 300 && y <= 350) {
                timerMode = 60; // 60 seconds
                break;
           } else {
                 redrawTimerMenu = true; // Ensure the menu redraws if no button is clicked
                    }
        }

        delay(10);
    }

    closegraph(); // Close the timer selection menu


        // Phase 3: Game Mode Selection
        WindowManager::setupGameModeWindow();
        bool redrawGameMode = true;
        while (true) {
            GameMenu::drawGameModeMenu(redrawGameMode);
            redrawGameMode = false;

            if (ismouseclick(WM_LBUTTONDOWN)) {
                int x = mousex();
                int y = mousey();
                clearmouseclick(WM_LBUTTONDOWN);

                int centerX = getmaxx() / 2;

                if (x >= centerX - BUTTON_WIDTH - 20 && x <= centerX - 20 && y >= 200 && y <= 200 + BUTTON_HEIGHT) {
                    vsBot = false;
                    break; // Player vs Player
                } else if (x >= centerX + 20 && x <= centerX + BUTTON_WIDTH + 20 && y >= 200 && y <= 200 + BUTTON_HEIGHT) {
                    vsBot = true;
                    break; // Player vs Bot
                } 
            }

            delay(10);
        }

        closegraph(); // Close the game mode selection menu

        // Phase 4: Menu for Player Names
        WindowManager::setupMenuWindow();
        int currentName = 1;
        while (true) {
            if (vsBot && currentName > 1) break; // Only one player for vs Bot mode

            GameMenu::drawNameInputMenu(player1Name, player2Name, currentName, vsBot);
            if (kbhit()) {
                char key = getch();
                if (key == '\r') { // ENTER key to switch to next name
                    currentName++;
                    if (currentName > 2) break; // Exit name input phase
                } else if (key == '\b') { // BACKSPACE to remove last character
                    if (currentName == 1 && !player1Name.empty()) player1Name.pop_back();
                    if (currentName == 2 && !player2Name.empty() && !vsBot) player2Name.pop_back();
                } else { // Append character
                    if (currentName == 1) player1Name += key;
                    if (currentName == 2 && !vsBot) player2Name += key;
                }
            }
            delay(50);
        }
        if (vsBot) player2Name = "Bot"; // Assign default name for bot
        closegraph(); //Close the player names Menu

        // Phase 5: Game Window
        WindowManager::setupGameWindow(rows, cols);
        Grid grid(rows, cols);
        PlayerManager playerManager;
        GameLogic gameLogic(grid);
        BotManager botManager(grid, 'X'); // Initialize bot
        bool gameOver = false;
        GameMenu menu; 
        bool needRestart = true;

        int countdown = timerMode; 
        time_t startTime = time(nullptr); // Record the start time

        cleardevice();                               
        grid.drawGrid(); 
       

        while (!gameOver) {
            setcolor(WHITE);
            settextstyle(SIMPLEX_FONT, HORIZ_DIR, 2);
            outtextxy(20, 10, (char *)("Player 1: " + player1Name + " (O)").c_str());
            outtextxy(20, 50, (char *)("Player 2: " + player2Name + " (X)").c_str());

        // Display timer at the bottom-right corner
            string timerText = "Time Left: " + to_string(countdown) + "s";
            int timerTextWidth = textwidth((char *)timerText.c_str());
            int timerTextHeight = textheight((char *)timerText.c_str());
            int screenWidth = getmaxx();
            int screenHeight = getmaxy();
            outtextxy(screenWidth - timerTextWidth - 20, screenHeight - timerTextHeight - 20, (char *)timerText.c_str());


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
        

    // Bot's turn in Player vs Bot mode
        if (vsBot && playerManager.getCurrentPlayer() == 'X') {
          cout << "Bot's turn to play." << endl; // Debug statement
        botManager.makeMove(); // Make the bot move
        int botRow = botManager.getLastRow();
        int botCol = botManager.getLastCol();
        
        if (botRow == -1 || botCol == -1) {
        cout << "Bot did not make a move. Check bot logic." << endl; // Debug statement
    } else {
        cout << "Bot placed at (" << botRow << ", " << botCol << ")." << endl; // Debug statement
    }

        cleardevice(); // Clear screen and redraw
        grid.drawGrid();

        // Check for a winner or draw
        WinSequence winSeq = gameLogic.checkWinner(botRow, botCol);
        if (winSeq.hasWinner) {
            grid.drawWinningLine(winSeq.startRow, winSeq.startCol, winSeq.endRow, winSeq.endCol);
            gameOver = true;
            displayWinner(player2Name);
        } else if (grid.isFull()) {
            gameOver = true;
            displayDrawMessage();
        } else {
            playerManager.switchPlayer(); // Switch to the player
            countdown = timerMode;       // Reset the timer
            startTime = time(nullptr);   // Reset the start time
        }
    } else if (ismouseclick(WM_LBUTTONDOWN)) { // Player's turn
        int x = mousex();
        int y = mousey();
        clearmouseclick(WM_LBUTTONDOWN);

        int row = (y - 100) / CELL_SIZE;
        int col = (x - 20) / CELL_SIZE;

        if (row >= 0 && row < rows && col >= 0 && col < cols && grid.isCellEmpty(row, col)) {
            char player = playerManager.getCurrentPlayer();
            grid.setCell(row, col, player);
            cleardevice(); // Clear screen and redraw
            grid.drawGrid();

            // Check for a winner or draw
            WinSequence winSeq = gameLogic.checkWinner(row, col);
            if (winSeq.hasWinner) {
                grid.drawWinningLine(winSeq.startRow, winSeq.startCol, winSeq.endRow, winSeq.endCol);
                gameOver = true;
                displayWinner(player == 'O' ? player1Name : player2Name);
            } else if (grid.isFull()) {
                gameOver = true;
                displayDrawMessage();
            } else {
                playerManager.switchPlayer(); // Switch to the bot (if vsBot)
                countdown = timerMode;       // Reset the timer
                startTime = time(nullptr);   // Reset the start time
                    }
                }
            }
            delay(10);
        }
        if (gameOver) {
    cout << "Game over! Press any key to exit." << endl;
    getch(); // Wait for user input before exiting
    closegraph(); // Close the graphics window explicitly
}
    }
};

int main() {
    Application app;
    app.run();
    return 0;
    }
