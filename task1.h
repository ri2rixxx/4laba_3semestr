#ifndef TASK1_H
#define TASK1_H

#include <iostream>
#include <thread>
#include <mutex>
#include <semaphore>
#include <barrier>
#include <atomic>
#include <vector>
#include <chrono>
#include <random>
#include <condition_variable>

using namespace std;
using namespace chrono;

// SpinLock реализация
class SpinLock {
    atomic_flag flag = ATOMIC_FLAG_INIT;
public:
    void lock();
    void unlock();
};

// Monitor реализация
class Monitor {
    mutex mtx;
    condition_variable cv;
    bool ready = false;
public:
    void enter();
    void exit();
};

// Функции для тестирования примитивов
void testMutex(int id, int iterations);
void testSemaphore(int id, int iterations);
void testBarrier(int id, int iterations);
void testSpinLock(int id, int iterations);
void testSpinWait(int id, int iterations);
void testMonitor(int id, int iterations);

// Главная функция задания 1
void task1();

#endif // TASK1_H
