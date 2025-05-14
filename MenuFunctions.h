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
  cout << prompt;
  while (!(cin >> value))
  {
    cin.clear();
    cin.ignore(10000, '\n');
    cout << "Помилка: введіть ціле число.\n"
         << prompt;
  }
  cin.ignore(10000, '\n'); // Очищуємо буфер вводу
  return value;
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
  string input;
  cout << prompt << " (y/n): ";
  getline(cin, input);
  return !input.empty() && (input[0] == 'y' || input[0] == 'Y');
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
  cout << "=== Інформація про масив ===\n";
  cout << "Розмір масиву: " << array.size() << " елементів\n";

  if (!array.empty())
  {
    int minVal = array[0], maxVal = array[0];
    for (int val : array)
    {
      minVal = min(minVal, val);
      maxVal = max(maxVal, val);
    }
    cout << "Найменше значення: " << minVal << endl;
    cout << "Найбільше значення: " << maxVal << endl;
  }

  size_t memUsage = ArrayOperations::calculateMemoryUsage(array);
  cout << "Використана пам'ять: " << memUsage << " байт\n";
}

// Структура для зберігання результатів сортування
struct SortResult
{
  string name;
  SortMetrics metrics;
  int numThreads;

  SortResult(const string &n, const SortMetrics &m, int threads)
      : name(n), metrics(m), numThreads(threads) {}
};

// Функція для порівняння результатів сортування
void compareSortResults(const vector<SortResult> &results)
{
  cout << "\n===== ПОРІВНЯННЯ РЕЗУЛЬТАТІВ СОРТУВАННЯ =====\n";

  // Знаходимо найкращі результати
  double minTime = results[0].metrics.executionTimeMs;
  long long minComparisons = results[0].metrics.comparisons;
  long long minSwaps = results[0].metrics.swaps;

  for (const auto &result : results)
  {
    minTime = min(minTime, result.metrics.executionTimeMs);
    minComparisons = min(minComparisons, result.metrics.comparisons);
    minSwaps = min(minSwaps, result.metrics.swaps);
  }

  // Вивід заголовка таблиці
  cout << left << setw(20) << "Алгоритм"
       << right << setw(12) << "Потоки"
       << right << setw(15) << "Час (мс)"
       << right << setw(17) << "% від найкр."
       << right << setw(15) << "Порівняння"
       << right << setw(15) << "Обміни" << endl;

  cout << string(95, '-') << endl;

  // Вивід даних в таблиці
  for (const auto &result : results)
  {
    double timePercent = (result.metrics.executionTimeMs / minTime) * 100;

    cout << left << setw(20) << result.name
         << right << setw(12) << result.numThreads
         << right << setw(15) << fixed << setprecision(3) << result.metrics.executionTimeMs
         << right << setw(17) << fixed << setprecision(2) << timePercent
         << right << setw(15) << result.metrics.comparisons
         << right << setw(15) << result.metrics.swaps << endl;
  }

  cout << "\nВисновок:\n";

  // Простий аналіз результатів
  if (results.size() >= 2)
  {
    // Знаходимо найшвидший і найповільніший алгоритми
    int fastestIndex = 0;
    int slowestIndex = 0;

    for (size_t i = 1; i < results.size(); i++)
    {
      if (results[i].metrics.executionTimeMs < results[fastestIndex].metrics.executionTimeMs)
      {
        fastestIndex = i;
      }
      if (results[i].metrics.executionTimeMs > results[slowestIndex].metrics.executionTimeMs)
      {
        slowestIndex = i;
      }
    }

    // Обчислюємо прискорення
    double speedup = results[slowestIndex].metrics.executionTimeMs / results[fastestIndex].metrics.executionTimeMs;

    cout << "- Найшвидший: " << results[fastestIndex].name << " ("
         << results[fastestIndex].numThreads << " потоків, "
         << fixed << setprecision(3) << results[fastestIndex].metrics.executionTimeMs << " мс)\n";

    cout << "- Найповільніший: " << results[slowestIndex].name << " ("
         << results[slowestIndex].numThreads << " потоків, "
         << fixed << setprecision(3) << results[slowestIndex].metrics.executionTimeMs << " мс)\n";

    cout << "- Прискорення: " << fixed << setprecision(2) << speedup << "x\n";

    // Аналіз ефективності багатопотокового сортування
    for (size_t i = 0; i < results.size(); i++)
    {
      if (results[i].name == "Багатопотоковий" && results[i].numThreads > 1)
      {
        // Знаходимо послідовний алгоритм для порівняння
        for (size_t j = 0; j < results.size(); j++)
        {
          if (results[j].name == "Послідовний" ||
              (results[j].name == "Багатопотоковий" && results[j].numThreads == 1))
          {

            double threadSpeedup = results[j].metrics.executionTimeMs / results[i].metrics.executionTimeMs;
            double efficiency = threadSpeedup / results[i].numThreads * 100;

            cout << "- Ефективність багатопотокового сортування з "
                 << results[i].numThreads << " потоками: "
                 << fixed << setprecision(2) << efficiency << "%\n";

            // Показуємо додаткову інформацію про ефективність
            if (efficiency < 50)
            {
              cout << "  (Низька ефективність: можливо, накладні витрати на створення потоків "
                   << "та об'єднання результатів занадто великі для даного розміру масиву)\n";
            }
            else if (efficiency < 80)
            {
              cout << "  (Середня ефективність: помірний виграш від паралелізму)\n";
            }
            else
            {
              cout << "  (Висока ефективність: хороший виграш від паралелізму)\n";
            }

            break;
          }
        }
      }
    }
  }
}

bool getDetailedMode()
{
  return getYesNoInput("Увімкнути детальний режим виконання (показувати порівняння і обміни)?");
}

#endif // MENU_FUNCTIONS_H