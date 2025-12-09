#include "task2.h"

Car::Car(string b, double p, int m, string bt, int y) 
    : brand(b), price(p), mileage(m), bodyType(bt), year(y) {}

void Car::print() const {
    cout << setw(15) << left << brand 
         << setw(10) << year 
         << setw(12) << fixed << setprecision(0) << price 
         << setw(10) << mileage 
         << setw(12) << bodyType << endl;
}

vector<Car> generateCars(int count) {
    vector<Car> cars;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> priceDist(500000, 5000000);
    uniform_int_distribution<> mileageDist(0, 300000);
    uniform_int_distribution<> yearDist(2010, 2024);
    
    vector<string> brands = {
        "Toyota", "BMW", "Mercedes", "Audi", "Honda",
        "Volkswagen", "Ford", "Hyundai", "Kia", "Mazda",
        "Nissan", "Lexus", "Volvo", "Subaru", "Chevrolet"
    };
    
    vector<string> bodyTypes = {
        "Sedan", "SUV", "Hatchback", "Coupe", "Wagon", "Crossover"
    };
    
    for (int i = 0; i < count; ++i) {
        cars.emplace_back(
            brands[gen() % brands.size()],
            priceDist(gen),
            mileageDist(gen),
            bodyTypes[gen() % bodyTypes.size()],
            yearDist(gen)
        );
    }
    
    return cars;
}

vector<Car> filterCarsSingleThread(
    const vector<Car>& cars,
    double minPrice, double maxPrice,
    int maxMileage, int minYear)
{
    vector<Car> result;
    for (const auto& car : cars) {
        if (car.price >= minPrice && car.price <= maxPrice &&
            car.mileage <= maxMileage && car.year >= minYear) {
            result.push_back(car);
        }
    }
    return result;
}

vector<Car> filterCarsMultiThread(
    const vector<Car>& cars,
    double minPrice, double maxPrice,
    int maxMileage, int minYear,
    int numThreads)
{
    vector<Car> result;
    mutex resultMutex;
    
    auto processChunk = [&](int start, int end) {
        vector<Car> localResult;
        for (int i = start; i < end; ++i) {
            if (cars[i].price >= minPrice && cars[i].price <= maxPrice &&
                cars[i].mileage <= maxMileage && cars[i].year >= minYear) {
                localResult.push_back(cars[i]);
            }
        }
        
        lock_guard<mutex> lock(resultMutex);
        result.insert(result.end(), localResult.begin(), localResult.end());
    };
    
    vector<thread> threads;
    int chunkSize = cars.size() / numThreads;
    
    for (int i = 0; i < numThreads; ++i) {
        int start = i * chunkSize;
        int end = (i == numThreads - 1) ? cars.size() : (i + 1) * chunkSize;
        threads.emplace_back(processChunk, start, end);
    }
    
    for (auto& t : threads) {
        t.join();
    }
    
    return result;
}

void task2() {
    cout << "\n╔════════════════════════════════════════════════════════════╗" << endl;
    cout << "║  ЗАДАНИЕ 2: Многопоточная фильтрация автомобилей (вар.17) ║" << endl;
    cout << "╚════════════════════════════════════════════════════════════╝" << endl;
    
    int arraySize, numThreads;
    double minPrice, maxPrice;
    int maxMileage, minYear;
    
    cout << "\nВведите размер массива данных: ";
    cin >> arraySize;
    
    cout << "Введите количество потоков (рекомендуется " 
         << thread::hardware_concurrency() << "): ";
    cin >> numThreads;
    
    cout << "\n--- Критерии фильтрации ---" << endl;
    cout << "Минимальная цена (руб): ";
    cin >> minPrice;
    
    cout << "Максимальная цена (руб): ";
    cin >> maxPrice;
    
    cout << "Максимальный пробег (км): ";
    cin >> maxMileage;
    
    cout << "Минимальный год выпуска: ";
    cin >> minYear;
    
    cout << "\nГенерация данных..." << endl;
    vector<Car> cars = generateCars(arraySize);
    cout << "Данные сгенерированы: " << cars.size() << " автомобилей" << endl;
    
    cout << "\n" << string(60, '─') << endl;
    cout << "Параметры обработки:" << endl;
    cout << "  Размер массива:     " << arraySize << endl;
    cout << "  Количество потоков: " << numThreads << endl;
    cout << "  Диапазон цен:       " << minPrice << " - " << maxPrice << " руб" << endl;
    cout << "  Макс. пробег:       " << maxMileage << " км" << endl;
    cout << "  Мин. год выпуска:   " << minYear << endl;
    cout << string(60, '─') << endl << endl;
    
    // Однопоточная обработка
    cout << "Запуск однопоточной обработки..." << endl;
    auto start = high_resolution_clock::now();
    auto result1 = filterCarsSingleThread(cars, minPrice, maxPrice, maxMileage, minYear);
    auto end = high_resolution_clock::now();
    auto duration1 = duration_cast<milliseconds>(end - start).count();
    
    cout << "✓ Однопоточная обработка завершена" << endl;
    cout << "  Время:    " << duration1 << " ms" << endl;
    cout << "  Найдено:  " << result1.size() << " автомобилей" << endl << endl;
    
    // Многопоточная обработка
    cout << "Запуск многопоточной обработки..." << endl;
    start = high_resolution_clock::now();
    auto result2 = filterCarsMultiThread(cars, minPrice, maxPrice, maxMileage, minYear, numThreads);
    end = high_resolution_clock::now();
    auto duration2 = duration_cast<milliseconds>(end - start).count();
    
    cout << "✓ Многопоточная обработка завершена" << endl;
    cout << "  Время:    " << duration2 << " ms" << endl;
    cout << "  Найдено:  " << result2.size() << " автомобилей" << endl;
    cout << "  Ускорение: " << fixed << setprecision(2) 
         << (duration2 > 0 ? (double)duration1 / duration2 : 0) << "x" << endl;
    
    // Вывод результатов
    if (!result1.empty()) {
        cout << "\n" << string(60, '─') << endl;
        cout << "Первые 10 найденных автомобилей:" << endl;
        cout << string(60, '─') << endl;
        cout << setw(15) << left << "Марка" 
             << setw(10) << "Год" 
             << setw(12) << "Цена (руб)" 
             << setw(10) << "Пробег" 
             << setw(12) << "Тип кузова" << endl;
        cout << string(60, '─') << endl;
        
        for (int i = 0; i < min(10, (int)result1.size()); ++i) {
            result1[i].print();
        }
        cout << string(60, '─') << endl;
    } else {
        cout << "\nАвтомобили, соответствующие критериям, не найдены." << endl;
    }
}
