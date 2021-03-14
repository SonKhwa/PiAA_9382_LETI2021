#include <iostream>
#include <list>
#include <ctime>
#include <fstream>

struct square {
    int x, y, length;
    square(int x, int y, int length) {
        this->x = x;
        this->y = y;
        this->length = length;
    }
};

class EffectiveDivision {
private:
    std::list<square> minArr;
    std::list<square> curArr;
    size_t N{};
public:
    explicit EffectiveDivision(int N) {
        this->N = N;
    }
    void showArea(bool isMin) {
        std::list<square> &viewArr = (isMin) ? minArr : curArr;
        (isMin)? std::cout << "Найденный минимальный массив:\n" : std::cout << "Текущее заполнение массива:\n";
        std::cout << viewArr.size() << "\n";
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

    void showAnswer(int k) {
        std::cout << "Найденный результат: " << minArr.size() << "\n";
        for (auto i : minArr) {
            std::cout << i.x * k + 1 << " " << i.y * k + 1 << " " << i.length * k << "\n";
        }
    }

    bool checkPossibilityToPlace(int x, int y, int length) {
        if (x + length > N || y + length > N)
            return false;
        for (auto it : curArr) {
            if ((it.x < (x + length) && (it.x + it.length) > x) && (it.y < (y + length) && (it.y + it.length) > y)) {
                return false;
            }
        }
        return true;
    }

    void defineBase() {
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

    square* findEmptyPlace(int x, int y) {
        auto* temp_square = new square(x, y, 0);
        while (!checkPossibilityToPlace(temp_square->x, temp_square->y, 1)) {
            if (temp_square->y == N - 1) {
                if (temp_square->x == N - 1) {
                    return nullptr;
                } else {
                    temp_square->x++;
                    temp_square->y = N / 2;
                    continue;
                }
            }
            temp_square->y++;
        }
        return temp_square;
    }

    void calculateOptimization(int x, int y) {
        if (curArr.size() >= minArr.size() && !minArr.empty())
            return;
        for (int length = N / 2; length > 0; --length) {
            if (checkPossibilityToPlace(x, y, length)) {
                curArr.emplace_back(x, y, length);
                showArea(false);
                square* temp_square = findEmptyPlace(x, y);
                if (temp_square) {
                    calculateOptimization(temp_square->x, temp_square->y);
                } else {
                    if (curArr.size() < minArr.size() || minArr.empty()) {
                        minArr = curArr;
                        showArea(true);
                    }
                    curArr.pop_back();
                    return;
                }
                curArr.pop_back();
                delete temp_square;
            }
        }
    }

    void findMin() {
        int size = N, k = 0;
        for (auto i = size; i > 0; --i) {
            if (N % i == 0 && N != i) {
                N /= i;
                k = i;
                break;
            }
        }
        defineBase();
        calculateOptimization(N / 2, N / 2 + 1);
        showAnswer(k);
    }
};

int main() {
    char answer = 'y';
    while (answer == 'y') {
        int N = 0;
        std::cout << "Хотите считать размеры из файла или ввести самостоятельно?(1/2)\n";
        std::cin >> answer;
        clock_t start = clock();
        if (answer == '2') {
            std::cout << "Введите размер главного квадрата!\n";
            std::cin >> N;
            auto *square = new EffectiveDivision(N);
            square->findMin();
            delete square;
        } else {
            std::ifstream fin("test.txt");
            fin >> N;
            std::cout << N << "\n";
            auto *square = new EffectiveDivision(N);
            square->findMin();
            delete square;
            fin.close();
        }
        clock_t end = clock();
        std::cout << "Время выполнения: " << (double) (end - start) / CLOCKS_PER_SEC << "\n";
        std::cout << "Хотите продолжить?(y/n)\n";
        std::cin >> answer;
    }
    return 0;
}