#include <iostream>
#include <cstdlib>
#include <ctime>
#include <conio.h> 

using namespace std;

const int BOARD_SIZE = 10;
const int OBSTACLE_COUNT = 10;
const int MAX_STEP = 5;

struct Object {
    int x, y;
};

void printBoard(char board[BOARD_SIZE][BOARD_SIZE], Object o, bool showPlayer) {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (showPlayer && i == o.y && j == o.x)
                cout << '*' << ' ';
            else
                cout << board[i][j] << ' ';
        }
        cout << '\n';
    }
}

char getInput() {
    return _getch();
}

void makeRoad(char board[BOARD_SIZE][BOARD_SIZE]) {
    int x = 0, y = 0;
    board[y][x] = '0';

    while (x != BOARD_SIZE - 1 || y != BOARD_SIZE - 1) {
        int direction = rand() % 4;
        int step = rand() % MAX_STEP + 1;

        for (int i = 0; i < step; ++i) {
            if (direction == 0 && x < BOARD_SIZE - 1) {
                x++;
            }
            else if (direction == 1 && y < BOARD_SIZE - 1) {
                y++;
            }
            else if (direction == 2 && x > 0) {
                x--;
            }
            else if (direction == 3 && y > 0) {
                y--;
            }
            board[y][x] = '0';

            if (x == BOARD_SIZE - 1 && y == BOARD_SIZE - 1) {
                break;
            }
        }
    }

    for (int i = 0; i < OBSTACLE_COUNT; ++i) {
        int ox, oy;
        do {
            ox = rand() % BOARD_SIZE;
            oy = rand() % BOARD_SIZE;
        } while (board[oy][ox] == '0' || (ox == 0 && oy == 0) || (ox == 9 && oy == 9));

        board[oy][ox] = 'X';
    }
}

bool moveObject(char input, char board[BOARD_SIZE][BOARD_SIZE], Object& o) {
    int newX = o.x;
    int newY = o.y;

    if (input == 'w') newY--;
    else if (input == 'a') newX--;
    else if (input == 's') newY++;
    else if (input == 'd') newX++;

    if (newX >= 0 && newX < BOARD_SIZE && newY >= 0 && newY < BOARD_SIZE &&
        board[newY][newX] == '0') {
        o.x = newX;
        o.y = newY;
        return true;
    }

    return false;
}

int main() {
    srand(static_cast<unsigned int>(time(0)));

    char board[BOARD_SIZE][BOARD_SIZE];
    Object o = { -1, -1 };
    bool showPlayer = false;

    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            board[i][j] = '1';
        }
    }

    char input;
    bool isRunning = true;

    while (isRunning) {
        system("cls");
        printBoard(board, o, showPlayer);
        cout << "Enter: 길 생성, r: 말 나타내기, w/a/s/d: 말 이동, q: 종료\n";

        input = getInput();

        if (input == '\r') { 
            makeRoad(board);
        }
        else if (input == 'r') {
            o.x = 0;
            o.y = 0;
            showPlayer = true;
        }
        else if (input == 'w' || input == 'a' || input == 's' || input == 'd') {
            if (!moveObject(input, board, o)) {
                cout << "경로가 아니거나 장애물이 있습니다! 이동할 수 없습니다.\n";
            }
        }
        else if (input == 'q') { 
            isRunning = false;
        }
    }

    cout << "게임이 종료되었습니다.\n";
    return 0;
}