#ifndef TASK3_H
#define TASK3_H

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace chrono;

// Алгоритм банкира (вариант 1)
class BankersAlgorithm {
private:
    int numProcesses;
    int numResources;
    vector<vector<int>> max;        // Максимальные требования
    vector<vector<int>> allocation; // Текущее распределение
    vector<vector<int>> need;       // Оставшиеся потребности
    vector<int> available;          // Доступные ресурсы
    mutex mtx;
    
public:
    BankersAlgorithm(int p, int r);
    
    void setMaximum(int process, const vector<int>& maxReq);
    void setAvailable(const vector<int>& avail);
    
    bool isSafe();
    bool requestResources(int process, const vector<int>& request);
    void releaseResources(int process);
    
    void printState();
    
    int getNumProcesses() const { return numProcesses; }
    int getNumResources() const { return numResources; }
};

// Главная функция задания 3
void task3();

#endif // TASK3_H
