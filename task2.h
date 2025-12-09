#ifndef TASK2_H
#define TASK2_H

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include <random>
#include <chrono>
#include <iomanip>

using namespace std;
using namespace chrono;

// Структура автомобиля (вариант 17)
struct Car {
    string brand;
    double price;
    int mileage;
    string bodyType;
    int year;
    
    Car(string b, double p, int m, string bt, int y);
    void print() const;
};

// Генерация тестовых данных
vector<Car> generateCars(int count);

// Однопоточная фильтрация
vector<Car> filterCarsSingleThread(
    const vector<Car>& cars,
    double minPrice, double maxPrice,
    int maxMileage, int minYear
);

// Многопоточная фильтрация
vector<Car> filterCarsMultiThread(
    const vector<Car>& cars,
    double minPrice, double maxPrice,
    int maxMileage, int minYear,
    int numThreads
);

// Главная функция задания 2
void task2();

#endif // TASK2_H
