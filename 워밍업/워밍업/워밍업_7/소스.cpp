#include <iostream>
#include <string>
#include <sstream>
#include <Windows.h>
#include <algorithm>
#include <limits>
#undef max
using namespace std;

struct Point1 {
	int x1, x2;
	int y1, y2;
};

struct Point2 {
	int x1, x2;
	int y1, y2;
};

const int BOARD_SIZE = 10;

void setColor(int color) {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

void resetColor() {
	setColor(7);
}

void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			if (board[i][j] == '#') {
				setColor(12);
				cout << board[i][j] << ' ';
				resetColor();
			}
			else {
				cout << board[i][j] << ' ';
			}
		}
		cout << '\n';
	}
}

void drawRectangle1(char board[BOARD_SIZE][BOARD_SIZE], Point1 p) {
	if (p.x1 <= p.x2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
		}
	}
	else {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j < BOARD_SIZE; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
			for (int j = 0; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
		}
	}

	if (p.y1 <= p.y2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
		}
	}
	else {
		for (int i = p.y1; i < BOARD_SIZE; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
		}
		for (int i = 0; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
		}
	}

	if (p.x1 > p.x2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = 0; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
			for (int j = p.x1; j < BOARD_SIZE; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
		}
	}

	if (p.y1 > p.y2) {
		for (int i = 0; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
		}
		for (int i = p.y1; i < BOARD_SIZE; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '0';
			}
		}
	}
}

void drawRectangle2(char board[BOARD_SIZE][BOARD_SIZE], Point2 p) {
	if (p.x1 <= p.x2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
		}
	}
	else {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j < BOARD_SIZE; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
			for (int j = 0; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
		}
	}

	if (p.y1 <= p.y2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
		}
	}
	else {
		for (int i = p.y1; i < BOARD_SIZE; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
		}
		for (int i = 0; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
		}
	}

	if (p.x1 > p.x2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = 0; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
			for (int j = p.x1; j < BOARD_SIZE; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
		}
	}

	if (p.y1 > p.y2) {
		for (int i = 0; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
		}
		for (int i = p.y1; i < BOARD_SIZE; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = 'x';
			}
		}
	}
}

void clearRectangle1(char board[BOARD_SIZE][BOARD_SIZE], Point1 p) {
	if (p.x1 <= p.x2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}
	else {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j < BOARD_SIZE; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
			for (int j = 0; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}

	if (p.y1 <= p.y2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}
	else {
		for (int i = p.y1; i < BOARD_SIZE; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
		for (int i = 0; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}

	if (p.x1 > p.x2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = 0; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
			for (int j = p.x1; j < BOARD_SIZE; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}

	if (p.y1 > p.y2) {
		for (int i = 0; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
		for (int i = p.y1; i < BOARD_SIZE; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}
}

void clearRectangle2(char board[BOARD_SIZE][BOARD_SIZE], Point2 p) {
	if (p.x1 <= p.x2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}
	else {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j < BOARD_SIZE; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
			for (int j = 0; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}

	if (p.y1 <= p.y2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}
	else {
		for (int i = p.y1; i < BOARD_SIZE; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
		for (int i = 0; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}

	if (p.x1 > p.x2) {
		for (int i = p.y1; i <= p.y2; ++i) {
			for (int j = 0; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
			for (int j = p.x1; j < BOARD_SIZE; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}

	if (p.y1 > p.y2) {
		for (int i = 0; i <= p.y2; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
		for (int i = p.y1; i < BOARD_SIZE; ++i) {
			for (int j = p.x1; j <= p.x2; ++j) {
				board[i % BOARD_SIZE][j % BOARD_SIZE] = '.';
			}
		}
	}
}

void detectCollision(char board[BOARD_SIZE][BOARD_SIZE], Point1 p1, Point2 p2) {
	for (int i = p1.y1; i != (p1.y2 + 1) % BOARD_SIZE; i = (i + 1) % BOARD_SIZE) {
		for (int j = p1.x1; j != (p1.x2 + 1) % BOARD_SIZE; j = (j + 1) % BOARD_SIZE) {
			if (p2.y1 <= i && i <= p2.y2 && p2.x1 <= j && j <= p2.x2) {
				board[i][j] = '#';
			}
			else if ((p2.y2 < p2.y1 && (i >= p2.y1 || i <= p2.y2)) ||
				(p2.x2 < p2.x1 && (j >= p2.x1 || j <= p2.x2))) {
				board[i][j] = '#';
			}
		}
	}
}

int main() {
	char board[BOARD_SIZE][BOARD_SIZE];

	bool changeBoard = false;

	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			board[i][j] = '.';
		}
	}

	Point1 p1;
	Point2 p2;

	string input;
	cout << "첫번째 사각형과 두번째 사각형의 좌표값을 입력하세요 (x1, y1) (x2, y2) (x1, y1) (x2, y2) : ";
	getline(cin, input);

	stringstream ss(input);
	char ignore;
	ss >> ignore >> p1.x1 >> ignore >> p1.y1 >> ignore;
	ss >> ignore >> p1.x2 >> ignore >> p1.y2 >> ignore;
	ss >> ignore >> p2.x1 >> ignore >> p2.y1 >> ignore;
	ss >> ignore >> p2.x2 >> ignore >> p2.y2 >> ignore;

	drawRectangle1(board, p1);
	drawRectangle2(board, p2);

	char command;
	while (true) {
		printBoard(board);

		cout << "명령어를 입력하세요 : ";
		cin >> command;

		clearRectangle1(board, p1);
		clearRectangle2(board, p2);

		if (command == 'x') {
			p1.x1 = (p1.x1 + 1) % BOARD_SIZE;
			p1.x2 = (p1.x2 + 1) % BOARD_SIZE;
		}
		else if (command == 'X') {
			p1.x1 = (p1.x1 - 1 + BOARD_SIZE) % BOARD_SIZE;
			p1.x2 = (p1.x2 - 1 + BOARD_SIZE) % BOARD_SIZE;
		}
		else if (command == 'y') {
			p1.y1 = (p1.y1 + 1) % BOARD_SIZE;
			p1.y2 = (p1.y2 + 1) % BOARD_SIZE;
		}
		else if (command == 'Y') {
			p1.y1 = (p1.y1 - 1 + BOARD_SIZE) % BOARD_SIZE;
			p1.y2 = (p1.y2 - 1 + BOARD_SIZE) % BOARD_SIZE;
		}
		else if (command == 's') {
			p1.x2--;
			p1.y2--;
		}
		else if (command == 'S') {
			if (p1.x2 < BOARD_SIZE - 1) {
				p1.x2++;
			}

			if (p1.y2 < BOARD_SIZE - 1) {
				p1.y2++;
			}
		}
		else if (command == 'w') {
			p2.x1 = (p2.x1 + 1) % BOARD_SIZE;
			p2.x2 = (p2.x2 + 1) % BOARD_SIZE;
		}
		else if (command == 'W') {
			p2.x1 = (p2.x1 - 1 + BOARD_SIZE) % BOARD_SIZE;
			p2.x2 = (p2.x2 - 1 + BOARD_SIZE) % BOARD_SIZE;
		}
		else if (command == 'a') {
			p2.y1 = (p2.y1 + 1) % BOARD_SIZE;
			p2.y2 = (p2.y2 + 1) % BOARD_SIZE;
		}
		else if (command == 'A') {
			p2.y1 = (p2.y1 - 1 + BOARD_SIZE) % BOARD_SIZE;
			p2.y2 = (p2.y2 - 1 + BOARD_SIZE) % BOARD_SIZE;
		}
		else if (command == 'd') {
			p2.x2--;
			p2.y2--;
		}
		else if (command == 'D') {
			if (p2.x2 < BOARD_SIZE - 1) {
				p2.x2++;
			}

			if (p2.y2 < BOARD_SIZE - 1) {
				p2.y2++;
			}
		}
		else if (command == 'r') {
			for (int i = 0; i < BOARD_SIZE; ++i) {
				for (int j = 0; j < BOARD_SIZE; ++j) {
					board[i][j] = '.';
				}
			}

			cin.ignore(numeric_limits<streamsize>::max(), '\n');

			string input;
			cout << "첫번째 사각형과 두번째 사각형의 좌표값을 입력하세요 (x1, y1) (x2, y2) (x1, y1) (x2, y2) : ";
			getline(cin, input);

			stringstream ss(input);
			char ignore;
			ss >> ignore >> p1.x1 >> ignore >> p1.y1 >> ignore;
			ss >> ignore >> p1.x2 >> ignore >> p1.y2 >> ignore;
			ss >> ignore >> p2.x1 >> ignore >> p2.y1 >> ignore;
			ss >> ignore >> p2.x2 >> ignore >> p2.y2 >> ignore;

			drawRectangle1(board, p1);
			drawRectangle2(board, p2);
		}
		else if (command == 'q') {
			break;
		}
		drawRectangle1(board, p1);
		drawRectangle2(board, p2);
		detectCollision(board, p1, p2);
	}
	return 0;
}