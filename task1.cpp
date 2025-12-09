#include "task1.h"

// Глобальные переменные для примитивов
mutex mtx;
counting_semaphore<100> sem(1);
SpinLock spinlock;
atomic<bool> spinwait_flag{false};
Monitor monitor;

// SpinLock реализация
void SpinLock::lock() {
    while (flag.test_and_set(memory_order_acquire));
}

void SpinLock::unlock() {
    flag.clear(memory_order_release);
}

// Monitor реализация
void Monitor::enter() {
    unique_lock<mutex> lock(mtx);
    cv.wait(lock, [this] { return !ready; });
    ready = true;
}

void Monitor::exit() {
    unique_lock<mutex> lock(mtx);
    ready = false;
    cv.notify_one();
}

// Тестовые функции
void testMutex(int id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        mtx.lock();
        char c = 33 + rand() % 94;
        mtx.unlock();
    }
}

void testSemaphore(int id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        sem.acquire();
        char c = 33 + rand() % 94;
        sem.release();
    }
}

void testBarrier(int id, int iterations) {
    static barrier sync_point(thread::hardware_concurrency());
    for (int i = 0; i < iterations; ++i) {
        char c = 33 + rand() % 94;
        sync_point.arrive_and_wait();
    }
}

void testSpinLock(int id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        spinlock.lock();
        char c = 33 + rand() % 94;
        spinlock.unlock();
    }
}

void testSpinWait(int id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        bool expected = false;
        while (!spinwait_flag.compare_exchange_weak(expected, true)) {
            expected = false;
        }
        char c = 33 + rand() % 94;
        spinwait_flag.store(false);
    }
}

void testMonitor(int id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        monitor.enter();
        char c = 33 + rand() % 94;
        monitor.exit();
    }
}

template<typename Func>
double measureTime(Func func, const string& name, int numThreads, int iterations) {
    vector<thread> threads;
    auto start = high_resolution_clock::now();
    
    for (int i = 0; i < numThreads; ++i) {
        threads.emplace_back(func, i, iterations);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start).count();
    
    cout << setw(20) << left << name << ": " << setw(8) << right << duration << " ms" << endl;
    return duration;
}

void task1() {
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║     ЗАДАНИЕ 1: Сравнение примитивов синхронизации         ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝" << endl;
    
    int numThreads, iterations;
    
    cout << "\nВведите количество потоков (рекомендуется " 
         << thread::hardware_concurrency() << "): ";
    cin >> numThreads;
    
    cout << "Введите количество итераций для каждого потока: ";
    cin >> iterations;
    
    cout << "\n" << string(60, '─') << endl;
    cout << "Параметры тестирования:" << endl;
    cout << "  Количество потоков: " << numThreads << endl;
    cout << "  Итераций на поток:  " << iterations << endl;
    cout << string(60, '─') << endl << endl;
    
    cout << "Запуск тестов...\n" << endl;
    
    measureTime(testMutex, "Mutex", numThreads, iterations);
    measureTime(testSemaphore, "Semaphore", numThreads, iterations);
    measureTime(testBarrier, "Barrier", numThreads, iterations / 100);
    measureTime(testSpinLock, "SpinLock", numThreads, iterations);
    measureTime(testSpinWait, "SpinWait", numThreads, iterations / 100);
    measureTime(testMonitor, "Monitor", numThreads, iterations / 100);
    
    cout << "\n" << string(60, '─') << endl;
    cout << "Анализ результатов:" << endl;
    cout << "  • Mutex - стандартный механизм, баланс производительности" << endl;
    cout << "  • Semaphore - для ограничения доступа N потоков" << endl;
    cout << "  • Barrier - синхронизация группы потоков" << endl;
    cout << "  • SpinLock - быстрый для коротких критических секций" << endl;
    cout << "  • SpinWait - активное ожидание, нагружает CPU" << endl;
    cout << "  • Monitor - высокоуровневая абстракция" << endl;
    cout << string(60, '─') << endl;
}
