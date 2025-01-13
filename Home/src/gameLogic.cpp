#include <iostream>
#include <vector>
#include <string>

using namespace std;

class TicTacToe {
private:
    int rows, cols;
    vector<vector<char>> grid;
    char currentPlayer;
    bool gameOver;

public:
    TicTacToe(int n, int m) : rows(n), cols(m), currentPlayer('O'), gameOver(false) {
        grid.resize(rows, vector<char>(cols, ' '));
    }

    void handleMove(int row, int col) {
        if (gameOver || row < 0 || row >= rows || col < 0 || col >= cols || grid[row][col] != ' ') {
            return;
        }

        grid[row][col] = currentPlayer;

        if (checkWinner(row, col)) {
            cout << "Player " << currentPlayer << " Wins!\n";
            gameOver = true;
        }

        currentPlayer = (currentPlayer == 'O') ? 'X' : 'O';
    }

    bool checkWinner(int row, int col) {
        char symbol = grid[row][col];

        // Check horizontal
        int count = 0;
        for (int j = 0; j < cols; ++j) {
            count = (grid[row][j] == symbol) ? count + 1 : 0;
            if (count == 5) return true;
        }

        // Check vertical
        count = 0;
        for (int i = 0; i < rows; ++i) {
            count = (grid[i][col] == symbol) ? count + 1 : 0;
            if (count == 5) return true;
        }

        // Check diagonal (\)
        count = 0;
        for (int i = -min(row, col); i < min(rows - row, cols - col); ++i) {
            count = (grid[row + i][col + i] == symbol) ? count + 1 : 0;
            if (count == 5) return true;
        }

        // Check diagonal (/)
        count = 0;
        for (int i = -min(row, cols - 1 - col); i < min(rows - row, col + 1); ++i) {
            count = (grid[row + i][col - i] == symbol) ? count + 1 : 0;
            if (count == 5) return true;
        }

        return false;   
    }

    void displayBoard() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                cout << (grid[i][j] == ' ' ? '.' : grid[i][j]) << " ";
            }
            cout << endl;
        }
    }
};


int main() {
    TicTacToe game(7, 7); 
    game.handleMove(0, 0); 
    game.displayBoard();

return 0;   
}