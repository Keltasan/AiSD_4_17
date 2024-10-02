#include <iostream>
#include <set>
#include <vector>
#include <chrono>
#include <fstream>
#include <algorithm>

using MySet = std::set<int>;
using MyIt = MySet::iterator;
using MySeq = std::vector<MyIt>;

int main() {
    std::ofstream fout("output.txt");
    const int num_tests = 10; // Количество тестов
    bool debug = false;       // Отключаем отладочный вывод

    for (int p = 10; p <= 200; p += 10) {
        MySet myset;
        MySeq myseq;

        // Генерация множества
        for (int i = 0; i < p; ++i) {
            myset.insert(rand() % 1000); // Вставка случайных чисел
        }

        // Подготовка последовательности
        for (auto it = myset.begin(); it != myset.end(); ++it) {
            myseq.push_back(it);
        }

        // Измерение времени выполнения операций
        auto t1 = std::chrono::high_resolution_clock::now();

        // Пример операций с множеством (объединение и пересечение)
        MySet another_set;
        for (int i = 0; i < p / 2; ++i) {
            another_set.insert(rand() % 1000);
        }

        // Пример операции объединения
        MySet result_set;
        std::set_union(myset.begin(), myset.end(),
                       another_set.begin(), another_set.end(),
                       std::inserter(result_set, result_set.begin()));

        // Пример операции пересечения
        MySet intersection_set;
        std::set_intersection(myset.begin(), myset.end(),
                              another_set.begin(), another_set.end(),
                              std::inserter(intersection_set, intersection_set.begin()));

        auto t2 = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

        fout << p << " " << duration << "\n"; // Запись результатов в файл

        if (debug) {
            std::cout << "Размер множества: " << p << ", Время: " << duration << " мкс" << std::endl;
        }
    }

    fout.close();
    return 0;
}