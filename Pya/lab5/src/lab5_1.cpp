#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <algorithm>

//#define DEBUG

std::map<char, int> alphabet{
        { '$', 0 },
        { 'A', 1 },
        { 'C', 2 },
        { 'G', 3 },
        { 'T', 4 },
        { 'N', 5 } };

#define countOfAlph 6

class Node {
public:
    int name;
    int nextVer[countOfAlph];
    int numOfPattern;
    int suffixRef;
    int autoMove[countOfAlph];
    int suffixFRef;
    int parent;//узел родителя
    bool isEnd;
    Node(int parent, int c) {
        std::fill_n(nextVer, countOfAlph, -1);
        std::fill_n(autoMove, countOfAlph, -1);
        suffixRef = -1;
        suffixFRef = -1;
        this->parent = parent;
        name = c;
        isEnd = false;
    }
};

class  Aho_Corasick {
private:
    std::vector<Node> bohr;
    std::vector<std::string> P;
    std::string T;
    std::vector<std::string> crossPatterns;
    std::vector<std::pair<int, int>> otvet;
    bool isF;
    int crossCount, crossP;

public:
    Aho_Corasick() {
        bohr.emplace_back(0, alphabet['$']);
        isF = true;
        crossCount = 0, crossP = -1;
    }

    void readData(std::istream& fin) {
        int n;
        fin >> T >> n;
        for (int i = 0; i < n; i++) {
            std::string cur_P;
            fin >> cur_P;
            P.push_back(cur_P);
        }
    }

    void addPatternToBohr() {
#ifdef DEBUG
        std::cout << "Начинаем построение бора\n";
#endif
        for (int str = 0; str < P.size(); str++) {
            int n = 0;
#ifdef DEBUG
            std::cout << "Рассматриваем " << str + 1 << " подстроку: " << P[str] << "\n";
#endif
            for (auto& i : P[str]) {
#ifdef DEBUG
                std::cout << "Рассматриваем " << i << " символ\n";
#endif
                int sym;
                if (alphabet.find(i) != alphabet.end()) {
                    sym = alphabet[i];
                }
                else {
                    //error
                    std::cout << "error\n";
                    sym = 0;
                }
                if (bohr[n].nextVer[sym] == -1) {
                    bohr.emplace_back(n, sym);
                    bohr[n].nextVer[sym] = bohr.size() - 1;
#ifdef DEBUG
                    std::cout << "Узел с соответствующим очередной букве символом не найден. Добавим в бор новый.\n";
#endif
                }
                else {
#ifdef DEBUG
                    std::cout << "Узел с соответствующим очередной букве символом найден.\n";
#endif
                }
                n = bohr[n].nextVer[sym];
            }
            bohr[n].isEnd = true;
            bohr[n].numOfPattern = str;
        }
    }

    int getSuffixRef(int ver) {
        if (bohr[ver].suffixRef == -1) {
            if (ver == 0 || bohr[ver].parent == 0) {
#ifdef DEBUG
                std::cout << "Суффиксная ссылка указывает на корень\n";
#endif
                bohr[ver].suffixRef = 0;
            }
            else {
#ifdef DEBUG
                std::cout << "Совершим переход по суффиксной ссылке родителя\n";
#endif
                bohr[ver].suffixRef = getAutoMove(getSuffixRef(bohr[ver].parent), bohr[ver].name);
                return bohr[ver].suffixRef;
            }
        }
#ifdef DEBUG
        if (isF)
            printSuffix(ver);
        else printSuffix(bohr[ver].suffixRef);
#endif
        return bohr[ver].suffixRef;
    }

    int getAutoMove(int ver, int sym) {
        if (bohr[ver].autoMove[sym] == -1) {
            if (bohr[ver].nextVer[sym] != -1) {
#ifdef DEBUG
                std::cout << "Совершим переход к следующему узлу с соответствующим символом\n";
                printSuffix(bohr[ver].nextVer[sym]);
#endif
                bohr[ver].autoMove[sym] = bohr[ver].nextVer[sym];
            }
            else if (ver == 0) {
#ifdef DEBUG
                std::cout << "Следующий узел с соответствующим символом не найден, перейдем в узел корня\n";
#endif
                bohr[ver].autoMove[sym] = 0;
            }
            else {
#ifdef DEBUG
                std::cout << "Следующий узел с соответствующим символом не найден, перейдем по суффиксной ссылке\n";
#endif
                bohr[ver].autoMove[sym] = getAutoMove(getSuffixRef(ver), sym);
            }
        }
        return bohr[ver].autoMove[sym];
    }

    int getSuffixFRef(int ver) {
#ifdef DEBUG
        std::cout << "-------------------------------------------------\n";
#endif
        if (bohr[ver].suffixFRef == -1) {
#ifdef DEBUG
            std::cout << "Для хорошей суффиксной ссылки получим узел по суффиксной ссылке текущего узла\n";
#endif
            int u = getSuffixRef(ver);
            if (u == 0) {
#ifdef DEBUG
                std::cout << "Хорошая суффиксная ссылка указывает на корень\n";
#endif
                bohr[ver].suffixFRef = 0;
            }
            else {
                if (bohr[u].isEnd) {
#ifdef DEBUG
                    std::cout << "Хорошая суффиксная ссылка указывает на конец шаблона\n";
#endif
                    bohr[ver].suffixFRef = u;
                }
                else {
#ifdef DEBUG
                    std::cout << "Хорошая суффиксная ссылка указывает на хорошую суффиксную ссылку узла\n";
#endif
                    bohr[ver].suffixFRef = getSuffixFRef(u);
                }
            }
        }
#ifdef DEBUG
        std::cout << "-------------------------------------------------\n";
#endif
        return bohr[ver].suffixFRef;
    }

    void checkResearching(int ver, int i) {
        for (int y = ver; y != 0; y = getSuffixFRef(y)) {
            if (bohr[y].isEnd) {
#ifdef DEBUG
                std::cout << "Найден шаблон в тексте\n";
                std::cout << i - P[bohr[y].numOfPattern].length() + 1 << " " << bohr[y].numOfPattern + 1 << "\n";
#endif
                if (crossP != -1 && i - crossCount < P[bohr[y].numOfPattern].length() + P[crossP].length()) {
                    if (std::find(crossPatterns.begin(), crossPatterns.end(), P[crossP]) == crossPatterns.end())
                        crossPatterns.push_back(P[crossP]);
                    if (std::find(crossPatterns.begin(), crossPatterns.end(), P[bohr[y].numOfPattern]) == crossPatterns.end())
                        crossPatterns.push_back(P[bohr[y].numOfPattern]);
                }
                crossP = bohr[y].numOfPattern;
                crossCount = i - P[bohr[y].numOfPattern].length();
                otvet.push_back(std::pair<int, int>(i - P[bohr[y].numOfPattern].length() + 1, bohr[y].numOfPattern + 1));
            }
        }
    }

    void printSuffix(int suf) {
        if (isF) {
            std::cout << "Следующий рассматриваемый суффикс:\n";
            isF = false;
        }
        else
            std::cout << "Текущий суффикс:\n";
        std::string suffix;
        int k = suf;
        while (bohr[k].parent != 0) {
            suffix += (char)(bohr[k].name + 48);
            k = bohr[k].parent;
        }
        suffix += (char)(bohr[k].name + 48);
        for (auto& i : alphabet) {
            for (std::string::size_type n = 0; (n = suffix.find((char)(i.second + 48), n)) != std::string::npos; ++n) {
                suffix.replace(n, 1, 1, i.first);
            }
        }
        for (int i = suffix.length() - 1; i > -1; i--)
            std::cout << suffix[i];
        std::cout << "\n";
    }

    void doAlgorithm() {
        int u = 0;
        for (int i = 0; i < T.length(); i++) {
#ifdef DEBUG
            std::cout << "Рассматриваем строку с текущей позицией: ";
            for (int j = 0; j < T.length(); j++)
                if (j == i)
                    std::cout << "\"" << T[j] << "\"";
                else
                    std::cout << T[j];
            std::cout << "\n";
            isF = true;
#endif
            u = getAutoMove(u, alphabet[T[i]]);
            checkResearching(u, i + 1);
        }
#ifdef DEBUG
        std::cout << "Позиции шаблонов в тексте и номер образца:\n";
#endif
        sort(otvet.begin(), otvet.end());
        for (auto& i : otvet)
            std::cout << i.first << " " << i.second << "\n";
        std::cout << "Количество вершин в автомате: " << bohr.size() << "\n";
        std::cout << "Список найденных образцов, имеющих пересечения с другими найденными образцами в строке поиска";
        if (crossPatterns.empty())
            std::cout << ", пуст\n";
        else {
            std::cout << ":\n";
            for (auto& i : crossPatterns) {
                std::cout << i << " ";
            }
        }
    }

};

void startProgram() {
    Aho_Corasick* algorithm = new Aho_Corasick();
    int answer = 2;
#ifdef DEBUG
    std::cout << "Хотите ввести данные из файла или с помощью клавиатуры?(1/2)\n";
    std::cin >> answer;
#endif
    if (answer == 1) {
        std::ifstream fin("test1_2.txt");
        algorithm->readData(fin);
        fin.close();
    }
    else {
#ifdef DEBUG
        std::cout << "Введите текст, количество шаблонов и шаблоны, используя символы алфавита {A,C, G, T, N}:\n";
#endif
        algorithm->readData(std::cin);
    }
    algorithm->addPatternToBohr();
    algorithm->doAlgorithm();
}

int main() {
    startProgram();
    return 0;
}