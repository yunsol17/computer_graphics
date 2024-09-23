#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

void generateMatrix(int** matrix, int n) {
    int countTwo = 0;

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (countTwo < 2 && rand() % 16 == 0) {
                matrix[i][j] = 2;
                countTwo++;
            }
            else {
                matrix[i][j] = rand() % 2;
            }
        }
    }

    while (countTwo < 2) {
        int i = rand() % n;
        int j = rand() % n;
        if (matrix[i][j] != 2) {
            matrix[i][j] = 2;
            countTwo++;
        }
    }
}

void mulMatrix(int** matrix1, int** matrix2, int** mulResult, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            mulResult[i][j] = 0;
            for (int k = 0; k < n; ++k) {
                mulResult[i][j] += matrix1[i][k] * matrix2[k][j];
            }
        }
    }
}

void addMatrix(int** matrix1, int** matrix2, int** addResult, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            addResult[i][j] = matrix1[i][j] + matrix2[i][j];
        }
    }
}

void subMatrix(int** matrix1, int** matrix2, int** subResult, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            subResult[i][j] = matrix1[i][j] - matrix2[i][j];
        }
    }
}

void getMinorMatrix(int** matrix, int** temp, int p, int q, int n) {
    int row = 0, col = 0;

    for (int i = 0; i < n; i++) {
        if (i == p)
            continue;

        col = 0;
        for (int j = 0; j < n; j++) {
            if (j == q)
                continue;

            temp[row][col] = matrix[i][j];
            col++;
        }
        row++;
    }
}

int determinant(int** matrix, int n) {
    if (n == 1) {
        return matrix[0][0];
    }

    int det = 0;
    int sign = 1;

    int** temp = new int* [n - 1];
    for (int i = 0; i < n - 1; i++) {
        temp[i] = new int[n - 1];
    }

    for (int f = 0; f < n; f++) {
        getMinorMatrix(matrix, temp, 0, f, n);
        det += sign * matrix[0][f] * determinant(temp, n - 1);
        sign = -sign;
    }
    for (int i = 0; i < n - 1; i++) {
        delete[] temp[i];
    }
    delete[] temp;

    return det;
}

void transMatrix(int** matrix, int** trans, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            trans[j][i] = matrix[i][j];
        }
    }
}

void mulMatrixByNumber(int** matrix, int n, int num) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            matrix[i][j] *= num;
        }
    }
}

int** createMatrix(int rows, int cols) {
    int** matrix = new int* [rows];
    for (int i = 0; i < rows; i++) {
        matrix[i] = new int[cols];
    }
    return matrix;
}

void deleteMatrix(int** matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        delete[] matrix[i];
    }
    delete[] matrix;
}

void printMatrices(int** matrix1, int** matrix2, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            cout << matrix1[i][j] << " ";
        }
        cout << "\t";

        for (int j = 0; j < n; ++j) {
            cout << matrix2[i][j] << " ";
        }
        cout << '\n';
    }
}

void printMatrix(int** matrix, int rows, int cols) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            cout << matrix[i][j] << " ";
        }
        cout << '\n';
    }
}

void printMatrixBasedOnState(int** matrix1, int** matrix2, int n, int state) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (state == 0) {
                cout << matrix1[i][j] << " ";
            }
            else if (state == 1) {
                if (matrix1[i][j] % 2 == 0) {
                    cout << matrix1[i][j] << " ";
                }
                else {
                    cout << "ㅁ ";
                }
            }
            else if (state == 2) {
                if (matrix1[i][j] % 2 != 0) {
                    cout << matrix1[i][j] << " ";
                }
                else {
                    cout << "ㅁ ";
                }
            }
        }

        cout << "\t";

        for (int j = 0; j < n; ++j) {
            if (state == 0) {
                cout << matrix2[i][j] << " ";
            }
            else if (state == 1) {
                if (matrix2[i][j] % 2 == 0) {
                    cout << matrix2[i][j] << " ";
                }
                else {
                    cout << "ㅁ ";
                }
            }
            else if (state == 2) {
                if (matrix2[i][j] % 2 != 0) {
                    cout << matrix2[i][j] << " ";
                }
                else {
                    cout << "ㅁ ";
                }
            }
        }
        cout << '\n';
    }
}

int main() {
    srand(static_cast<unsigned>(time(0)));

    int n = 4;
    int** matrix1 = createMatrix(n, n);
    int** matrix2 = createMatrix(n, n);
    int** mulResult = createMatrix(n, n);
    int** addResult = createMatrix(n, n);
    int** subResult = createMatrix(n, n);
    int** trans1 = createMatrix(n, n);
    int** trans2 = createMatrix(n, n);

    int state = 0;

    generateMatrix(matrix1, n);
    generateMatrix(matrix2, n);

    printMatrices(matrix1, matrix2, n);

    char command;
    while (true) {
        cout << "명령어를 누르세요 : ";
        cin >> command;

        if (command == 'm') {
            mulMatrix(matrix1, matrix2, mulResult, n);
            printMatrix(mulResult, n, n);
        }
        else if (command == 'a') {
            addMatrix(matrix1, matrix2, addResult, n);
            printMatrix(addResult, n, n);
        }
        else if (command == 'd') {
            subMatrix(matrix1, matrix2, subResult, n);
            printMatrix(subResult, n, n);
        }
        else if (command == 'r') {
            int det1 = determinant(matrix1, n);
            int det2 = determinant(matrix2, n);
            cout << det1 << ", " << det2 << '\n';
        }
        else if (command == 't') {
            transMatrix(matrix1, trans1, n);
            transMatrix(matrix2, trans2, n);

            printMatrix(trans1, n, n);
            cout << "\n";
            printMatrix(trans2, n, n);

            int detTrans1 = determinant(trans1, n);
            int detTrans2 = determinant(trans2, n);

            cout << "\n" << detTrans1 << ", " << detTrans2 << '\n';
        }
        else if (command == 'e') {
            state = (state + 1) % 3;
            printMatrixBasedOnState(matrix1, matrix2, n, state);
        }
        else if (command == 's') {
            generateMatrix(matrix1, n);
            generateMatrix(matrix2, n);
            printMatrices(matrix1, matrix2, n);
        }
        else if (command >= '1' && command <= '9') {
            int num = command - '0';
            mulMatrixByNumber(matrix1, n, num);
            mulMatrixByNumber(matrix2, n, num);
            printMatrices(matrix1, matrix2, n);
        }
        else if (command == 'q') {
            break;
        }
    }

    deleteMatrix(matrix1, n);
    deleteMatrix(matrix2, n);
    deleteMatrix(mulResult, n);
    deleteMatrix(addResult, n);
    deleteMatrix(subResult, n);
    deleteMatrix(trans1, n);
    deleteMatrix(trans2, n);

    return 0;
}