#include <iostream>
#include <list>
#include <ctime>

struct square {
    int x, y, length;
    square(int x, int y, int length) {
        this->x = x;
        this->y = y;
        this->length = length;
    }
};

std::list <square> minArr;
std::list <square> curArr;

void showArea(int& N, bool isMin) {
    std::list<square> &viewArr = (isMin) ? minArr : curArr;
    int area[N][N];
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            area[i][j] = 0;
        }
    }
    int n = 1;
    for (auto it : viewArr) {
        for (int j = it.y; j < it.y + it.length; j++) {
            for (int i = it.x; i < it.x + it.length; i++) {
                area[j][i] = n;
            }
        }
        n++;
    }
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << area[i][j] << " ";
        }
        std::cout << "\n";
    }
}

void showAnswer(int& N, int& k) {
    std::cout << minArr.size() << "\n";
    for (auto i : minArr) {
        std::cout << i.x * k + 1 << " " << i.y * k + 1 << " " << i.length * k << "\n";
    }
}

bool checkPossibilityToPlace(int& N, int& x, int& y, int& length) {
    if (x + length > N || y + length > N)
        return false;
    for (auto it : curArr) {
        if ((it.x < (x + length) && (it.x + it.length) > x) && (it.y < (y + length) && (it.y + it.length) > y)) {
            return false;
        }
    }
    return true;
}

void defineBase(int& N) {
    int halfN = N / 2;
    if (N % 2 == 0) {
        curArr.emplace_back(0, 0, halfN);
        curArr.emplace_back(0, halfN, halfN);
        curArr.emplace_back(halfN, 0, halfN);
        curArr.emplace_back(halfN, halfN, halfN);
        minArr = curArr;
    } else {
        curArr.emplace_back(0, 0, halfN + 1);
        curArr.emplace_back(0, halfN + 1, halfN);
        curArr.emplace_back(halfN + 1, 0, halfN);
    }
}

bool findEmptyPlace(int& x, int& y, int& N) {
    int l = 1;
    while (!checkPossibilityToPlace(N, x,y, l)) {
        if (y == N - 1) {
            if (x == N - 1) {
                return false;
            }
            else {
                x++;
                y = N / 2;
                continue;
            }
        }
        y++;
    }
    return true;
}

void calculateOptimization(int& N, int x, int y) {
    if (curArr.size() >= minArr.size() && !minArr.empty())
        return;
    for (int length = N / 2; length > 0; --length) {
        if (checkPossibilityToPlace(N, x,y, length)) {
            curArr.emplace_back(x, y, length);
            int tX = x, tY = y;
            if (findEmptyPlace(tX, tY, N)) {
                calculateOptimization(N, tX, tY);
            } else {
                if (curArr.size() < minArr.size() || minArr.empty()) {
                    minArr = curArr;
//                    showArea(N, true);
                }
                curArr.pop_back();
                return;
            }
            curArr.pop_back();
        }
    }
}

void findMin(int& N) {
    int size = N, k = 0;
    for (auto i = size; i > 0; --i)
    {
        if (N % i == 0 && N != i)
        {
            N /= i;
            k = i;
            break;
        }
    }
    defineBase(N);
    calculateOptimization(N,N /2,N / 2 + 1);
    showAnswer(N, k);
}

int main() {
    int N = 0;
    std::cin >> N;
    clock_t start = clock();
    findMin(N);
    clock_t end = clock();
    std::cout << (double)(end - start) / CLOCKS_PER_SEC;
    return 0;
}