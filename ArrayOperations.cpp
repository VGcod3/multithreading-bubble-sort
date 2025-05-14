#include "ArrayOperations.h"
#include <random>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <iomanip>
#include <mutex>
#include <algorithm>
#include <vector>
#include <future>
#include <chrono>
#include <ctime>
#include <sstream>

// Мютекс для уникнення перемішування виводу з різних потоків
mutex consoleMutex;

// Отримати поточну часову мітку для виведення
string ArrayOperations::getCurrentTimestamp()
{
  auto now = chrono::system_clock::now();
  auto now_c = chrono::system_clock::to_time_t(now);
  auto now_ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;

  stringstream ss;
  ss << put_time(localtime(&now_c), "%H:%M:%S") << "." << setfill('0') << setw(3) << now_ms.count();
  return ss.str();
}

vector<int> ArrayOperations::generateRandomArray(int size, int minValue, int maxValue)
{
  vector<int> array(size);
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> distrib(minValue, maxValue);

  for (int i = 0; i < size; i++)
  {
    array[i] = distrib(gen);
  }

  return array;
}

void ArrayOperations::saveArrayToFile(const vector<int> &array, const string &filename)
{
  ofstream file(filename);
  if (!file.is_open())
  {
    throw runtime_error("Неможливо відкрити файл для запису: " + filename);
  }

  file << array.size() << endl;
  for (int value : array)
  {
    file << value << " ";
  }

  file.close();
  cout << "Файл " << filename << " успішно збережено. Розмір: " << array.size() << " елементів." << endl;
}

vector<int> ArrayOperations::loadArrayFromFile(const string &filename)
{
  ifstream file(filename);
  if (!file.is_open())
  {
    throw runtime_error("Неможливо відкрити файл для читання: " + filename);
  }

  int size;
  if (!(file >> size))
  {
    throw runtime_error("Помилка читання розміру масиву з файлу: " + filename);
  }

  if (size <= 0)
  {
    throw runtime_error("Некоректний розмір масиву в файлі: " + to_string(size));
  }

  vector<int> array(size);
  for (int i = 0; i < size; i++)
  {
    if (!(file >> array[i]))
    {
      throw runtime_error("Помилка читання елементу #" + to_string(i) + " з файлу: " + filename);
    }
  }

  file.close();
  return array;
}

// Helper function for bubble sort in a specific range
void ArrayOperations::bubbleSortRange(vector<int> &array, int start, int end, long long &comparisons, long long &swaps, bool verbose, int threadId)
{
  string threadInfo = threadId >= 0 ? "Потік " + to_string(threadId) : "Основний потік";

  if (verbose)
  {
    lock_guard<mutex> lock(consoleMutex);
    cout << getCurrentTimestamp() << " | " << threadInfo << " | Початок сортування діапазону ["
         << start << " - " << end << ") розміром " << (end - start) << " елементів" << endl;
  }

  for (int i = start; i < end - 1; i++)
  {
    for (int j = start; j < end - (i - start) - 1; j++)
    {
      comparisons++;

      if (verbose && comparisons % 1000 == 0)
      { // Обмежуємо кількість виведень для продуктивності
        lock_guard<mutex> lock(consoleMutex);
        cout << getCurrentTimestamp() << " | " << threadInfo
             << " | Порівняння #" << comparisons << ": "
             << array[j] << " та " << array[j + 1] << endl;
      }

      if (array[j] > array[j + 1])
      {
        if (verbose)
        {
          lock_guard<mutex> lock(consoleMutex);
          cout << getCurrentTimestamp() << " | " << threadInfo
               << " | Обмін #" << swaps + 1 << ": "
               << array[j] << " <-> " << array[j + 1]
               << " (індекси " << j << " <-> " << j + 1 << ")" << endl;
        }

        swap(array[j], array[j + 1]);
        swaps++;
      }
    }

    if (verbose && (i - start + 1) % 10 == 0)
    { // Інформація про прогрес кожні 10 ітерацій
      lock_guard<mutex> lock(consoleMutex);
      cout << getCurrentTimestamp() << " | " << threadInfo
           << " | Прогрес: " << (i - start + 1) << "/" << (end - start - 1)
           << " ітерацій (" << (i - start + 1) * 100 / (end - start - 1) << "%), "
           << comparisons << " порівнянь, " << swaps << " обмінів" << endl;
    }
  }

  if (verbose)
  {
    lock_guard<mutex> lock(consoleMutex);
    cout << getCurrentTimestamp() << " | " << threadInfo << " | Завершено сортування діапазону ["
         << start << " - " << end << "), " << comparisons << " порівнянь, "
         << swaps << " обмінів" << endl;
  }
}

// Helper function to merge sorted segments
void ArrayOperations::mergeSortedSegments(vector<int> &array, int numSegments, long long &comparisons, long long &swaps, bool verbose)
{
  int n = array.size();
  int segmentSize = n / numSegments;

  if (verbose)
  {
    cout << getCurrentTimestamp() << " | Злиття | Початок злиття "
         << numSegments << " сегментів розміром ~" << segmentSize << " елементів" << endl;
  }

  vector<int> tempArray(n);

  // Merge each pair of adjacent segments
  for (int segmentLength = segmentSize; segmentLength < n; segmentLength *= 2)
  {
    if (verbose)
    {
      cout << getCurrentTimestamp() << " | Злиття | Обробка сегментів розміром " << segmentLength << endl;
    }

    for (int start = 0; start < n; start += 2 * segmentLength)
    {
      int mid = min(start + segmentLength, n);
      int end = min(start + 2 * segmentLength, n);

      if (verbose)
      {
        cout << getCurrentTimestamp() << " | Злиття | Злиття сегментів ["
             << start << "-" << mid << ") та [" << mid << "-" << end << ")" << endl;
      }

      // Merge two segments
      int i = start, j = mid, k = start;

      while (i < mid && j < end)
      {
        comparisons++;

        if (array[i] <= array[j])
        {
          tempArray[k++] = array[i++];
        }
        else
        {
          if (verbose && swaps % 100 == 0)
          { // Обмежуємо кількість виведень
            cout << getCurrentTimestamp() << " | Злиття | Переміщення елемента з правого сегмента: "
                 << array[j] << " (індекс " << j << ") -> позиція " << k << endl;
          }

          tempArray[k++] = array[j++];
          swaps++; // Count non-adjacent swaps
        }
      }

      // Copy remaining elements
      while (i < mid)
      {
        tempArray[k++] = array[i++];
      }

      while (j < end)
      {
        tempArray[k++] = array[j++];
      }

      // Copy back to original array
      for (int m = start; m < end; m++)
      {
        array[m] = tempArray[m];
      }

      if (verbose)
      {
        cout << getCurrentTimestamp() << " | Злиття | Завершено злиття сегментів ["
             << start << "-" << mid << ") та [" << mid << "-" << end << ")" << endl;
      }
    }
  }

  if (verbose)
  {
    cout << getCurrentTimestamp() << " | Злиття | Завершено злиття всіх сегментів, "
         << comparisons << " порівнянь, " << swaps << " обмінів" << endl;
  }
}

SortMetrics ArrayOperations::bubbleSortMultithreaded(vector<int> &array, int numThreads, bool verbose)
{
  SortMetrics metrics;

  // Calculate initial memory usage
  metrics.memoryUsageBytes = calculateMemoryUsage(array);

  if (verbose)
  {
    cout << "\n=== ДЕТАЛЬНИЙ РЕЖИМ: БАГАТОПОТОКОВЕ СОРТУВАННЯ ===\n";
    cout << getCurrentTimestamp() << " | Початок багатопотокового сортування масиву розміром "
         << array.size() << " елементів" << endl;
  }

  // Start timing
  auto startTime = chrono::high_resolution_clock::now();

  // Determine number of threads if not specified
  if (numThreads <= 0)
  {
    numThreads = thread::hardware_concurrency();
    if (numThreads == 0)
      numThreads = 4; // Default to 4 if hardware_concurrency returns 0
  }

  int n = array.size();

  // Limit number of threads based on array size
  int maxThreads = max(1, n / 1000);
  if (numThreads > maxThreads)
  {
    if (verbose)
    {
      cout << getCurrentTimestamp() << " | Увага: кількість потоків зменшено з " << numThreads
           << " до " << maxThreads << " через малий розмір масиву (" << n << " елементів)." << endl;
    }
    else
    {
      cout << "Увага: кількість потоків зменшено з " << numThreads << " до " << maxThreads
           << " через малий розмір масиву (" << n << " елементів)." << endl;
    }
    numThreads = maxThreads;
  }

  // Print information about threads
  if (verbose)
  {
    cout << getCurrentTimestamp() << " | Сортування на " << numThreads << " потоках" << endl;
  }
  else
  {
    cout << "Виконання сортування на " << numThreads << " потоках..." << endl;
  }

  if (numThreads == 1 && !verbose)
  {
    cout << "Використовується один потік. Багатопотокові переваги не будуть помітні." << endl;
  }

  // Calculate segment size per thread
  int segmentSize = n / numThreads;
  if (verbose)
  {
    cout << getCurrentTimestamp() << " | Розмір сегменту на потік: ~" << segmentSize << " елементів" << endl;
  }
  else
  {
    cout << "Розмір сегменту на потік: ~" << segmentSize << " елементів" << endl;
  }

  vector<thread> threads;
  vector<long long> threadComparisons(numThreads, 0);
  vector<long long> threadSwaps(numThreads, 0);

  // Create and start threads
  if (verbose)
  {
    cout << getCurrentTimestamp() << " | Створення та запуск " << numThreads << " потоків" << endl;
  }

  for (int i = 0; i < numThreads; i++)
  {
    int startIdx = i * segmentSize;
    int endIdx = (i == numThreads - 1) ? n : (i + 1) * segmentSize;

    if (verbose)
    {
      cout << getCurrentTimestamp() << " | Запуск потоку #" << i << " для сортування діапазону ["
           << startIdx << " - " << endIdx << ")" << endl;
    }

    threads.push_back(thread(
        [&array, startIdx, endIdx, &threadComparisons, &threadSwaps, i, verbose]()
        {
          // Run bubble sort on a segment
          long long comparisons = 0;
          long long swaps = 0;

          bubbleSortRange(array, startIdx, endIdx, comparisons, swaps, verbose, i);

          threadComparisons[i] = comparisons;
          threadSwaps[i] = swaps;
        }));
  }

  // Wait for all threads to finish
  for (int i = 0; i < numThreads; i++)
  {
    threads[i].join();

    if (verbose)
    {
      cout << getCurrentTimestamp() << " | Потік #" << i << " завершив роботу" << endl;
    }
  }

  // Sum up comparisons and swaps from all threads
  for (int i = 0; i < numThreads; i++)
  {
    metrics.comparisons += threadComparisons[i];
    metrics.swaps += threadSwaps[i];

    if (verbose)
    {
      cout << getCurrentTimestamp() << " | Метрики потоку #" << i
           << ": " << threadComparisons[i] << " порівнянь, "
           << threadSwaps[i] << " обмінів" << endl;
    }
  }

  // Merge the sorted segments
  if (numThreads > 1)
  {
    if (verbose)
    {
      cout << getCurrentTimestamp() << " | Початок об'єднання " << numThreads << " відсортованих сегментів" << endl;
    }
    else
    {
      cout << "Об'єднання " << numThreads << " відсортованих сегментів..." << endl;
    }

    long long mergeComparisons = 0;
    long long mergeSwaps = 0;
    mergeSortedSegments(array, numThreads, mergeComparisons, mergeSwaps, verbose);

    // Add merging operations to metrics
    metrics.comparisons += mergeComparisons;
    metrics.swaps += mergeSwaps;

    if (verbose)
    {
      cout << getCurrentTimestamp() << " | Завершено об'єднання сегментів: "
           << mergeComparisons << " порівнянь, " << mergeSwaps << " обмінів" << endl;
    }
  }

  // End timing
  auto endTime = chrono::high_resolution_clock::now();
  metrics.executionTimeMs = chrono::duration<double, milli>(endTime - startTime).count();

  // Also return the number of threads used
  metrics.additionalInfo["numThreads"] = to_string(numThreads);

  if (verbose)
  {
    cout << getCurrentTimestamp() << " | Сортування завершено за "
         << fixed << setprecision(3) << metrics.executionTimeMs << " мс" << endl;
    cout << "=== КІНЕЦЬ ДЕТАЛЬНОГО РЕЖИМУ ===\n"
         << endl;
  }

  return metrics;
}

void ArrayOperations::printArray(const vector<int> &array, int maxElements)
{
  int size = array.size();

  if (size <= maxElements)
  {
    // Print full array if it's small enough
    for (int value : array)
    {
      cout << value << " ";
    }
  }
  else
  {
    // Print truncated array with indicators
    int halfMax = maxElements / 2;

    // Print first half of the elements
    for (int i = 0; i < halfMax; i++)
    {
      cout << array[i] << " ";
    }

    // Print ellipsis to indicate truncation
    cout << "... [" << (size - maxElements) << " елементів пропущено] ... ";

    // Print last half of the elements
    for (int i = size - halfMax; i < size; i++)
    {
      cout << array[i] << " ";
    }
  }
  cout << endl;
}

size_t ArrayOperations::calculateMemoryUsage(const vector<int> &array)
{
  // Calculate memory usage (vector size + overhead)
  return array.size() * sizeof(int) + sizeof(vector<int>);
}

void ArrayOperations::printMetrics(const SortMetrics &metrics)
{
  cout << "=== Метрики сортування ===" << endl;
  cout << "Кількість порівнянь: " << metrics.comparisons << endl;
  cout << "Кількість обмінів: " << metrics.swaps << endl;
  cout << "Час виконання: " << fixed << setprecision(3) << metrics.executionTimeMs << " мс" << endl;
  cout << "Використана пам'ять: " << metrics.memoryUsageBytes << " байт" << endl;

  // Print additional info if available
  auto it = metrics.additionalInfo.find("numThreads");
  if (it != metrics.additionalInfo.end())
  {
    cout << "Кількість потоків: " << it->second << endl;
  }
}

SortMetrics ArrayOperations::bubbleSort(vector<int> &array, bool verbose)
{
  SortMetrics metrics;

  // Calculate initial memory usage
  metrics.memoryUsageBytes = calculateMemoryUsage(array);

  if (verbose)
  {
    cout << "\n=== ДЕТАЛЬНИЙ РЕЖИМ: ПОСЛІДОВНЕ СОРТУВАННЯ ===\n";
    cout << getCurrentTimestamp() << " | Початок послідовного сортування масиву розміром "
         << array.size() << " елементів" << endl;
  }

  // Start timing
  auto startTime = chrono::high_resolution_clock::now();

  int n = array.size();
  for (int i = 0; i < n - 1; i++)
  {
    for (int j = 0; j < n - i - 1; j++)
    {
      metrics.comparisons++; // Count comparison

      if (verbose && metrics.comparisons % 1000 == 0)
      { // Обмежуємо кількість виведень
        cout << getCurrentTimestamp() << " | Порівняння #" << metrics.comparisons
             << ": " << array[j] << " та " << array[j + 1] << endl;
      }

      if (array[j] > array[j + 1])
      {
        // Swap array[j] and array[j+1]
        if (verbose)
        {
          cout << getCurrentTimestamp() << " | Обмін #" << metrics.swaps + 1
               << ": " << array[j] << " <-> " << array[j + 1]
               << " (індекси " << j << " <-> " << j + 1 << ")" << endl;
        }

        swap(array[j], array[j + 1]);
        metrics.swaps++; // Count swap
      }
    }

    if (verbose && (i + 1) % 10 == 0)
    { // Інформація про прогрес кожні 10 ітерацій
      cout << getCurrentTimestamp() << " | Прогрес: " << (i + 1) << "/" << (n - 1)
           << " ітерацій (" << (i + 1) * 100 / (n - 1) << "%), "
           << metrics.comparisons << " порівнянь, " << metrics.swaps << " обмінів" << endl;
    }
  }

  // End timing
  auto endTime = chrono::high_resolution_clock::now();
  metrics.executionTimeMs = chrono::duration<double, milli>(endTime - startTime).count();

  if (verbose)
  {
    cout << getCurrentTimestamp() << " | Сортування завершено за "
         << fixed << setprecision(3) << metrics.executionTimeMs << " мс" << endl;
    cout << "=== КІНЕЦЬ ДЕТАЛЬНОГО РЕЖИМУ ===\n"
         << endl;
  }

  return metrics;
}

bool ArrayOperations::isSorted(const vector<int> &array)
{
  if (array.empty() || array.size() == 1)
  {
    return true; // Порожній масив або масив з одного елемента вважається відсортованим
  }

  for (size_t i = 1; i < array.size(); i++)
  {
    if (array[i] < array[i - 1])
    {
      return false;
    }
  }

  return true;
}