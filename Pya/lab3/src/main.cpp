#include <iostream>
#include <map>
#include <algorithm>
#include <list>
#include <fstream>

using Vertical = char;
using Weight = int;

struct Edge {//стуктура для ребра
    Vertical vertical;
    Weight stream;//заданный поток
    Weight pastedStream;//пропущенный поток
    Edge(Vertical ver, Weight st, Weight fSt) {
        vertical = ver;
        stream = st;
        pastedStream = fSt;
    }
};

bool lowerSortThings (const Edge& first, const Edge& second) {
    return first.vertical > second.vertical;
}

bool upperSortThings (const Edge& first, const Edge& second) {
    return first.vertical < second.vertical;
}

class MaxStreamSearching {//класс для нахождения величины потока
private:
    std::map<Vertical, std::list<Edge>> graph;//стуктура для графа
    std::map<Vertical, bool> isVisited;//стуктура для отмечания посещенных вершин
    Vertical initDestVer, finDestVer;//начальная и конечная вершины
public:
    MaxStreamSearching() {};
    void readData(std::istream& fin, bool isCin) {//ввод данных и формирование списков
        int N = 0;
        if (isCin)
            std::cout << "Введите количество ребер\n";
        fin >> N;
        if (isCin)
            std::cout << "Введите исток и сток\n";
        fin >> initDestVer >> finDestVer;
        std::map<Vertical, std::list<Edge>>::iterator it;
        if (isCin)
            std::cout << "Введите ребра и вес\n";
        for (auto i = 0; i < N; i++) {
            Vertical initVer, finVer;
            Weight weight;
            std::cin >> initVer >> finVer >> weight;
            it = graph.find(initVer);
            if (it != graph.end()) {//если вершина уже записана в граф
                it->second.emplace_back(finVer, weight, 0);
            } else {
                isVisited.insert(std::pair<Vertical, bool>(initVer, false));
                std::list<Edge> listOfVer;
                listOfVer.emplace_back(finVer, weight, 0);
                graph.insert(std::pair<Vertical, std::list<Edge>>(initVer, listOfVer));
            }
        }
        for (auto &key: graph) graph[key.first].sort(lowerSortThings);
    }

    Weight doSearchInDepth(Vertical ver, Weight Cmin) {
        std::cout << "Текущая вершина " << ver << ", а текущий минимальный поток " << Cmin << "\n";
        if (ver == finDestVer) {
            std::cout << "Конечная вершина достигнута, алгоритм завершается\n";
            return Cmin;
        }
        isVisited[ver] = true;
        std::cout << "Отмечаем вершину посещенной\n";
        std::cout << "Рассматриваем ребра до вершин ";
        for (auto& edge : graph.find(ver)->second) {
            std::cout << edge.vertical << " ";
        }
        std::cout << "\n";
        for (auto& edge : graph.find(ver)->second) {
            std::cout << "Рассмотрим ребро до вершины " << edge.vertical << "\n";
            if (!isVisited[edge.vertical] && edge.pastedStream < edge.stream) {
                std::cout << "Ребро было включено в путь. Выполняется рекурсивный поиск минимального потока\n";
                Weight curStream = doSearchInDepth(edge.vertical, std::min(Cmin, edge.stream - edge.pastedStream));
                if (curStream > 0) {
                    edge.pastedStream += curStream;
                    return curStream;
                }
            } else std::cout << "Вершина была уже посещена, либо через ребро уже было пропущено максимальная величина потока\n";
        }
        return 0;
    }

    void doAlgoritm() {
        Weight sum = 0, answer = -1;
        while (answer != 0) {
            answer = doSearchInDepth(initDestVer, 10000);
            std::cout << "Минимальный поток, найденный в текущем пути " << answer << "\n";
            writeData(0);
            for (auto& i : isVisited) {
                i.second = false;
            }
            sum += answer;
        }
        std::cout << "Алгоритм завершился\n";
        writeData(sum);
    }
    void writeData(int sum) {
        if (sum != 0) {
            std::cout << "Суммарный поток " << sum << "\n";
            std::cout << "Ребро графа с фактической величиной протекающего потока\n";
        }
        for (auto i : graph) {
            i.second.sort(upperSortThings);
            for (auto j : i.second) {
                std::cout << i.first << " " << j.vertical << " " << j.pastedStream << "\n";
            }
        }
    }
};

int main() {
    char answ = 'y';
    while (answ == 'y') {
        std::cout << "Хотите считать данные из файла или ввести самостоятельно?(1/2)\n";
        std::cin >> answ;
        auto answer = new MaxStreamSearching();
        if (answ == '2') {
            answer->readData(std::cin, true);
        } else {
            std::ifstream fin("test2.txt");
            answer->readData(fin, false);
            fin.close();
        }
        std::cout << "Начинается работа алгоритма\n";
        answer->doAlgoritm();
        std::cout << "Хотите продолжить?(y/n)\n";
        std::cin >> answ;
    }
    return 0;
}