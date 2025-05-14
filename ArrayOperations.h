#ifndef ARRAY_OPERATIONS_H
#define ARRAY_OPERATIONS_H

#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <map>

using namespace std;

struct SortMetrics
{
  long long comparisons;
  long long swaps;
  double executionTimeMs;
  size_t memoryUsageBytes;
  map<string, string> additionalInfo; // Додаткова інформація (ключ-значення)

  SortMetrics() : comparisons(0), swaps(0), executionTimeMs(0), memoryUsageBytes(0) {}
};

class ArrayOperations
{
public:
  // Generate random array of given size
  static vector<int> generateRandomArray(int size, int minValue = 0, int maxValue = 100);

  // Save array to file
  static void saveArrayToFile(const vector<int> &array, const string &filename);

  // Load array from file
  static vector<int> loadArrayFromFile(const string &filename);

  // Bubble sort implementation with metrics
  static SortMetrics bubbleSort(vector<int> &array, bool verbose = false);

  // Multithreaded bubble sort implementation with metrics
  static SortMetrics bubbleSortMultithreaded(vector<int> &array, int numThreads = 0, bool verbose = false);

  // Print array to console (with truncation for large arrays)
  static void printArray(const vector<int> &array, int maxElements = 100);

  // Calculate memory usage of array
  static size_t calculateMemoryUsage(const vector<int> &array);

  // Print sort metrics
  static void printMetrics(const SortMetrics &metrics);

  // Verify if array is sorted
  static bool isSorted(const vector<int> &array);

private:
  // Helper function for bubble sort in a specific range
  static void bubbleSortRange(vector<int> &array, int start, int end, long long &comparisons, long long &swaps, bool verbose = false, int threadId = -1);

  // Helper function to merge sorted segments
  static void mergeSortedSegments(vector<int> &array, int numSegments, long long &comparisons, long long &swaps, bool verbose = false);

  // Get current timestamp for verbose output
  static string getCurrentTimestamp();
};

#endif // ARRAY_OPERATIONS_H