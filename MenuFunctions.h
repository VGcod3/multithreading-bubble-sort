#ifndef MENU_FUNCTIONS_H
#define MENU_FUNCTIONS_H

#include "ArrayOperations.h"
#include <iostream>
#include <string>
#include <limits>
#include <iomanip>

using namespace std;

// Очищення буфера вводу
void clearInputBuffer()
{
  cin.clear();
  cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Отримання цілого числа від користувача
int getIntInput(const string &prompt)
{
  int value;
  while (true)
  {
    cout << prompt;
    if (cin >> value)
    {
      clearInputBuffer();
      return value;
    }
    else
    {
      cout << "Помилка: введіть число\n";
      clearInputBuffer();
    }
  }
}

// Отримання рядка від користувача
string getStringInput(const string &prompt)
{
  string value;
  cout << prompt;
  getline(cin, value);
  return value;
}

// Отримання відповіді так/ні від користувача
bool getYesNoInput(const string &prompt)
{
  while (true)
  {
    cout << prompt << " (y/n): ";
    string input;
    getline(cin, input);

    if (input == "y" || input == "Y" || input == "так" || input == "Так" || input == "ТАК")
    {
      return true;
    }
    else if (input == "n" || input == "N" || input == "ні" || input == "Ні" || input == "НІ")
    {
      return false;
    }
    else
    {
      cout << "Будь ласка, введіть 'y' для так або 'n' для ні.\n";
    }
  }
}

// Функція для збереження масиву у файл
void saveArrayToFile(const vector<int> &array)
{
  string filename = getStringInput("Введіть ім'я файлу для збереження: ");
  ArrayOperations::saveArrayToFile(array, filename);
}

// Функція для виведення інформації про масив
void printArrayInfo(const vector<int> &array)
{
  cout << "Розмір масиву: " << array.size() << " елементів ("
       << ArrayOperations::calculateMemoryUsage(array) << " байт)" << endl;

  bool isSorted = ArrayOperations::isSorted(array);
  cout << "Стан масиву: " << (isSorted ? "відсортований" : "не відсортований") << endl;
}

// Структура для зберігання результатів сортування
struct SortResult
{
  string name;
  SortMetrics metrics;
  int numThreads;

  SortResult(const string &n, const SortMetrics &m, int threads = 1)
      : name(n), metrics(m), numThreads(threads) {}
};

// Функція для порівняння результатів сортування
void compareSortResults(const vector<SortResult> &results)
{
  if (results.size() < 2)
  {
    cout << "Недостатньо результатів для порівняння." << endl;
    return;
  }

  cout << "\n=== Порівняння результатів сортування ===\n";
  cout << left << setw(20) << "Метод"
       << setw(15) << "Час (мс)"
       << setw(15) << "Порівняння"
       << setw(15) << "Обміни"
       << setw(15) << "Пам'ять (байт)"
       << setw(10) << "Потоки" << endl;
  cout << string(90, '-') << endl;

  for (const auto &result : results)
  {
    cout << left << setw(20) << result.name
         << setw(15) << fixed << setprecision(3) << result.metrics.executionTimeMs
         << setw(15) << result.metrics.comparisons
         << setw(15) << result.metrics.swaps
         << setw(15) << result.metrics.memoryUsageBytes
         << setw(10) << result.numThreads << endl;
  }

  // Знаходження найшвидшого методу
  auto fastestResult = min_element(results.begin(), results.end(),
                                   [](const SortResult &a, const SortResult &b)
                                   {
                                     return a.metrics.executionTimeMs < b.metrics.executionTimeMs;
                                   });

  cout << "\nНайшвидший метод: " << fastestResult->name << " ("
       << fixed << setprecision(3) << fastestResult->metrics.executionTimeMs << " мс)" << endl;

  // Порівняння з послідовним сортуванням (припускаємо, що перший результат - це послідовне сортування)
  if (results.size() >= 2 && results[0].numThreads == 1)
  {
    for (size_t i = 1; i < results.size(); i++)
    {
      double speedup = results[0].metrics.executionTimeMs / results[i].metrics.executionTimeMs;
      cout << "Прискорення " << results[i].name << " відносно послідовного: "
           << fixed << setprecision(2) << speedup << "x" << endl;
    }
  }
}

#endif // MENU_FUNCTIONS_H