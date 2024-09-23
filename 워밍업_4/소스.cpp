#include <iostream>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <algorithm>

using namespace std;

const int BOARD_SIZE = 6;

void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            cout << board[i][j] << ' ';
        }
        cout << '\n';
    }
}

bool convertInput(char rowChar, char colChar, int& row, int& col) {
    if (colChar >= 'a' && colChar <= 'e') {
        col = colChar - 'a' + 1;
    }
    else {
        return false;
    }

    if (rowChar >= '1' && rowChar <= '5') {
        row = rowChar - '1' + 1;
    }
    else {
        return false;
    }

    return true;
}

void resetGame(char board[BOARD_SIZE][BOARD_SIZE], char hiddenBoard[BOARD_SIZE][BOARD_SIZE], vector<char>& symbols) {
    board[0][0] = ' ';
    board[0][1] = 'a';
    board[0][2] = 'b';
    board[0][3] = 'c';
    board[0][4] = 'd';
    board[0][5] = 'e';
    board[1][0] = '1';
    board[2][0] = '2';
    board[3][0] = '3';
    board[4][0] = '4';
    board[5][0] = '5';

    for (int i = 1; i < BOARD_SIZE; ++i) {
        for (int j = 1; j < BOARD_SIZE; ++j) {
            board[i][j] = '*';
            hiddenBoard[i][j] = '*';
        }
    }

    srand(static_cast<unsigned int>(time(0)));
    random_shuffle(symbols.begin(), symbols.end());

    int index = 0;
    for (int i = 1; i < BOARD_SIZE; ++i) {
        for (int j = 1; j < BOARD_SIZE; ++j) {
            hiddenBoard[i][j] = symbols[index];
            index++;
        }
    }
}

int main() {
    char board[BOARD_SIZE][BOARD_SIZE];
    char hiddenBoard[BOARD_SIZE][BOARD_SIZE];

    vector<char> symbols = { 'A', 'A', 'B', 'B', 'C', 'C', 'D', 'D', 'E', 'E', 'F', 'F', 'G', 'G', 'H', 'H', 'I', 'I', 'J', 'J', 'K', 'K', 'L', 'L', '@' };

    resetGame(board, hiddenBoard, symbols);

    char row1Char, col1Char, row2Char, col2Char;
    int row1 = 0, col1 = 0, row2 = 0, col2 = 0;

    while (true) {
        printBoard(board);

        cout << "좌표를 입력하세요(문자숫자 문자숫자, r을 입력하면 리셋): ";
        string input;
        cin >> input;

        if (input == "r") {
            resetGame(board, hiddenBoard, symbols);
            continue;
        }

        row1Char = input[1];
        col1Char = input[0];

        cin >> col2Char >> row2Char;

        if (!convertInput(row1Char, col1Char, row1, col1) || !convertInput(row2Char, col2Char, row2, col2)) {
            cout << "잘못된 입력입니다. 다시 입력하세요.\n";
            continue;
        }

        char firstChar = hiddenBoard[row1][col1];
        char secondChar = hiddenBoard[row2][col2];

        board[row1][col1] = firstChar;
        board[row2][col2] = secondChar;
        printBoard(board);

        if (firstChar == '@' || secondChar == '@') {
            char revealedChar = (firstChar == '@') ? secondChar : firstChar;

            for (int i = 1; i < BOARD_SIZE; ++i) {
                for (int j = 1; j < BOARD_SIZE; ++j) {
                    if (hiddenBoard[i][j] == revealedChar) {
                        board[i][j] = revealedChar;
                    }
                }
            }
        }
        else if (firstChar == secondChar) {
            board[row1][col1] = firstChar;
            board[row2][col2] = secondChar;
        }
        else {
            board[row1][col1] = '*';
            board[row2][col2] = '*';
        }
    }

    return 0;
}