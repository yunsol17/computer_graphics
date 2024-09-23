#include <iostream>
#include <string>
#include <sstream>

using namespace std;

struct Point {
	int x1, x2;
	int y1, y2;
};

const int BOARD_SIZE = 10;

void printBoard(char board[BOARD_SIZE][BOARD_SIZE]) {
	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			cout << board[i][j] << ' ';
		}
		cout << '\n';
	}
}

void drawRectangle(char board[BOARD_SIZE][BOARD_SIZE], Point p) {
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

void clearRectangle(char board[BOARD_SIZE][BOARD_SIZE], Point p) {
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

int main() {
	char board[BOARD_SIZE][BOARD_SIZE];

	for (int i = 0; i < BOARD_SIZE; ++i) {
		for (int j = 0; j < BOARD_SIZE; ++j) {
			board[i][j] = '.';
		}
	}

	Point p;

	string input;
	cout << "두 개의 좌표값을 입력하세요 (x1, y1) (x2, y2) : ";
	getline(cin, input);

	stringstream ss(input);
	char ignore;
	ss >> ignore >> p.x1 >> ignore >> p.y1 >> ignore;
	ss >> ignore >> p.x2 >> ignore >> p.y2 >> ignore;

	drawRectangle(board, p);

	char command;
	while (true) {
		printBoard(board);

		cout << "명령어를 입력하세요 : ";
		cin >> command;
		
		clearRectangle(board, p);

		if (command == 'x') {
			p.x1 = (p.x1 + 1) % BOARD_SIZE;
			p.x2 = (p.x2 + 1) % BOARD_SIZE;
		}
		else if (command == 'X') {
			p.x1 = (p.x1 - 1 + BOARD_SIZE) % BOARD_SIZE;
			p.x2 = (p.x2 - 1 + BOARD_SIZE) % BOARD_SIZE;
		}
		else if (command == 'y') {
			p.y1 = (p.y1 + 1) % BOARD_SIZE;
			p.y2 = (p.y2 + 1) % BOARD_SIZE;
		}
		else if (command == 'Y') {
			p.y1 = (p.y1 - 1 + BOARD_SIZE) % BOARD_SIZE;
			p.y2 = (p.y2 - 1 + BOARD_SIZE) % BOARD_SIZE;
		}
		else if (command == 's') {
			p.x2--;
			p.y2--;
		}
		else if (command == 'S') {
			if (p.x2 < BOARD_SIZE - 1) {
				p.x2++;
			}

			if (p.y2 < BOARD_SIZE - 1) {
				p.y2++;
			}
		}
		else if (command == 'i') {
			p.x2++;
		}
		else if (command == 'I') {
			p.x2--;
		}
		else if (command == 'j') {
			p.y2++;
		}
		else if (command == 'J') {
			p.y2--;
		}
		else if (command == 'a') {
			p.x2++;
			p.y2--;
		}
		else if (command == 'A') {
			p.x2--;
			p.y2++;
		}
		else if (command == 'm') {
			cout << "면적 : " << (p.x2 - p.x1 + 1) * (p.y2 - p.y1 + 1) << '\n';
		}
		else if (command == 'n') {
			cout << "비율 : " << (p.x2 - p.x1 + 1) * (p.y2 - p.y1 + 1) << "%" << '\n';
		}
		else if (command == 'r') {
			for(int i = 0; i < BOARD_SIZE; ++i) {
				for (int j = 0; j < BOARD_SIZE; ++j) {
					board[i][j] = '.';
				}
			}

			string input;
			cout << "새로운 두 개의 좌표값을 입력하세요 (x1, y1) (x2, y2) : ";
			cin.ignore();
			getline(cin, input);

			stringstream ss(input);
			char ignore;
			ss >> ignore >> p.x1 >> ignore >> p.y1 >> ignore;
			ss >> ignore >> p.x2 >> ignore >> p.y2 >> ignore;

			drawRectangle(board, p);
		}
		else if (command == 'q') {
			break;
		}
		drawRectangle(board, p);
	}
	return 0;
}