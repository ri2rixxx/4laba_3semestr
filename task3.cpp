#include "task3.h"

BankersAlgorithm::BankersAlgorithm(int p, int r) 
    : numProcesses(p), numResources(r) {
    max.resize(p, vector<int>(r));
    allocation.resize(p, vector<int>(r, 0));
    need.resize(p, vector<int>(r));
    available.resize(r);
}

void BankersAlgorithm::setMaximum(int process, const vector<int>& maxReq) {
    max[process] = maxReq;
    need[process] = maxReq;
}

void BankersAlgorithm::setAvailable(const vector<int>& avail) {
    available = avail;
}

bool BankersAlgorithm::isSafe() {
    vector<int> work = available;
    vector<bool> finish(numProcesses, false);
    vector<int> safeSequence;
    
    int count = 0;
    while (count < numProcesses) {
        bool found = false;
        
        for (int i = 0; i < numProcesses; ++i) {
            if (!finish[i]) {
                bool canAllocate = true;
                for (int j = 0; j < numResources; ++j) {
                    if (need[i][j] > work[j]) {
                        canAllocate = false;
                        break;
                    }
                }
                
                if (canAllocate) {
                    for (int j = 0; j < numResources; ++j) {
                        work[j] += allocation[i][j];
                    }
                    safeSequence.push_back(i);
                    finish[i] = true;
                    found = true;
                    count++;
                }
            }
        }
        
        if (!found) {
            cout << "  ⚠ Система находится в НЕБЕЗОПАСНОМ состоянии!" << endl;
            return false;
        }
    }
    
    cout << "  ✓ Система в БЕЗОПАСНОМ состоянии" << endl;
    cout << "  Безопасная последовательность: ";
    for (int i : safeSequence) {
        cout << "P" << i << " ";
    }
    cout << endl;
    return true;
}

bool BankersAlgorithm::requestResources(int process, const vector<int>& request) {
    lock_guard<mutex> lock(mtx);
    
    cout << "\n→ Процесс P" << process << " запрашивает ресурсы: [ ";
    for (int r : request) cout << r << " ";
    cout << "]" << endl;
    
    // Проверка: запрос не превышает потребность
    for (int i = 0; i < numResources; ++i) {
        if (request[i] > need[process][i]) {
            cout << "  ✗ Ошибка: запрос превышает максимальную потребность!" << endl;
            return false;
        }
    }
    
    // Проверка: запрос не превышает доступные ресурсы
    for (int i = 0; i < numResources; ++i) {
        if (request[i] > available[i]) {
            cout << "  ⏸ Ресурсы недоступны. Процесс должен ждать." << endl;
            return false;
        }
    }
    
    // Пробное выделение
    for (int i = 0; i < numResources; ++i) {
        available[i] -= request[i];
        allocation[process][i] += request[i];
        need[process][i] -= request[i];
    }
    
    // Проверка безопасности
    if (isSafe()) {
        cout << "  ✓ Запрос УДОВЛЕТВОРЕН!" << endl;
        return true;
    } else {
        // Откат изменений
        for (int i = 0; i < numResources; ++i) {
            available[i] += request[i];
            allocation[process][i] -= request[i];
            need[process][i] += request[i];
        }
        cout << "  ✗ Запрос ОТКЛОНЕН (небезопасное состояние)!" << endl;
        return false;
    }
}

void BankersAlgorithm::releaseResources(int process) {
    lock_guard<mutex> lock(mtx);
    
    cout << "\n← Процесс P" << process << " освобождает все ресурсы" << endl;
    
    for (int i = 0; i < numResources; ++i) {
        available[i] += allocation[process][i];
        need[process][i] += allocation[process][i];
        allocation[process][i] = 0;
    }
    
    cout << "  ✓ Ресурсы освобождены" << endl;
}

void BankersAlgorithm::printState() {
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║              Текущее состояние системы                     ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝" << endl;
    
    cout << "\nДоступные ресурсы: [ ";
    for (int a : available) cout << a << " ";
    cout << "]\n" << endl;
    
    cout << string(70, '─') << endl;
    cout << setw(10) << "Process" 
         << setw(20) << "Allocation" 
         << setw(20) << "Max" 
         << setw(20) << "Need" << endl;
    cout << string(70, '─') << endl;
    
    for (int i = 0; i < numProcesses; ++i) {
        cout << setw(10) << ("P" + to_string(i));
        
        cout << setw(5) << "[ ";
        for (int j = 0; j < numResources; ++j) 
            cout << allocation[i][j] << " ";
        cout << "]" << setw(10) << "";
        
        cout << setw(5) << "[ ";
        for (int j = 0; j < numResources; ++j) 
            cout << max[i][j] << " ";
        cout << "]" << setw(10) << "";
        
        cout << setw(5) << "[ ";
        for (int j = 0; j < numResources; ++j) 
            cout << need[i][j] << " ";
        cout << "]" << endl;
    }
    cout << string(70, '─') << endl;
}

void task3() {
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║       ЗАДАНИЕ 3: Алгоритм банкира (вариант 1)             ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝" << endl;
    
    int numProcesses, numResources;
    
    cout << "\nВведите количество процессов: ";
    cin >> numProcesses;
    
    cout << "Введите количество типов ресурсов: ";
    cin >> numResources;
    
    BankersAlgorithm banker(numProcesses, numResources);
    
    // Ввод максимальных требований
    cout << "\n--- Ввод максимальных требований ---" << endl;
    for (int i = 0; i < numProcesses; ++i) {
        cout << "Процесс P" << i << " (максимум для " << numResources << " ресурсов): ";
        vector<int> maxReq(numResources);
        for (int j = 0; j < numResources; ++j) {
            cin >> maxReq[j];
        }
        banker.setMaximum(i, maxReq);
    }
    
    // Ввод доступных ресурсов
    cout << "\nВведите количество доступных ресурсов (" << numResources << " значений): ";
    vector<int> available(numResources);
    for (int i = 0; i < numResources; ++i) {
        cin >> available[i];
    }
    banker.setAvailable(available);
    
    banker.printState();
    
    // Интерактивное меню
    while (true) {
        cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
        cout << "║                         МЕНЮ                               ║" << endl;
        cout << "╠════════════════════════════════════════════════════════════╣" << endl;
        cout << "║ 1. Запросить ресурсы для процесса                         ║" << endl;
        cout << "║ 2. Освободить ресурсы процесса                            ║" << endl;
        cout << "║ 3. Показать текущее состояние                             ║" << endl;
        cout << "║ 4. Проверить безопасность системы                         ║" << endl;
        cout << "║ 5. Симуляция многопоточных запросов                       ║" << endl;
        cout << "║ 0. Выход                                                  ║" << endl;
        cout << "╚════════════════════════════════════════════════════════════╝" << endl;
        cout << "Выберите действие: ";
        
        int choice;
        cin >> choice;
        
        if (choice == 0) {
            break;
        }
        
        switch (choice) {
            case 1: {
                int process;
                cout << "Номер процесса (0-" << numProcesses - 1 << "): ";
                cin >> process;
                
                if (process < 0 || process >= numProcesses) {
                    cout << "Неверный номер процесса!" << endl;
                    break;
                }
                
                cout << "Запрашиваемые ресурсы (" << numResources << " значений): ";
                vector<int> request(numResources);
                for (int i = 0; i < numResources; ++i) {
                    cin >> request[i];
                }
                
                banker.requestResources(process, request);
                break;
            }
            
            case 2: {
                int process;
                cout << "Номер процесса (0-" << numProcesses - 1 << "): ";
                cin >> process;
                
                if (process < 0 || process >= numProcesses) {
                    cout << "Неверный номер процесса!" << endl;
                    break;
                }
                
                banker.releaseResources(process);
                break;
            }
            
            case 3:
                banker.printState();
                break;
            
            case 4:
                cout << "\nПроверка безопасности системы..." << endl;
                banker.isSafe();
                break;
            
            case 5: {
                cout << "\nСимуляция многопоточных запросов..." << endl;
                cout << "Количество потоков для симуляции: ";
                int numThreads;
                cin >> numThreads;
                
                vector<thread> threads;
                
                for (int i = 0; i < numThreads; ++i) {
                    threads.emplace_back([&banker, i, numResources, numProcesses]() {
                        this_thread::sleep_for(milliseconds(100 * i));
                        
                        int process = rand() % numProcesses;
                        vector<int> request(numResources);
                        for (int j = 0; j < numResources; ++j) {
                            request[j] = rand() % 3;
                        }
                        
                        banker.requestResources(process, request);
                    });
                }
                
                for (auto& t : threads) {
                    t.join();
                }
                
                cout << "\nСимуляция завершена!" << endl;
                break;
            }
            
            default:
                cout << "Неверный выбор!" << endl;
        }
    }
}
