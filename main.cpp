#include "ArrayOperations.h"
#include "MenuFunctions.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

using namespace std;

// Функція для виведення меню та отримання вибору користувача
int showMainMenu()
{
  cout << "\n===== ГОЛОВНЕ МЕНЮ =====\n";
  cout << "1. Робота з масивом (створення/завантаження/збереження)\n";
  cout << "2. Сортування та аналіз\n";
  cout << "3. Порівняння результатів\n";
  cout << "0. Вихід\n";
  return getIntInput("Ваш вибір: ");
}

// Підменю для роботи з масивом
int showArrayMenu()
{
  cout << "\n===== РОБОТА З МАСИВОМ =====\n";
  cout << "1. Згенерувати випадковий масив\n";
  cout << "2. Завантажити масив із файлу\n";
  cout << "3. Зберегти поточний масив у файл\n";
  cout << "4. Показати масив\n";
  cout << "5. Інформація про масив\n";
  cout << "0. Повернутися до головного меню\n";
  return getIntInput("Ваш вибір: ");
}

// Підменю для сортування і аналізу
int showSortMenu()
{
  cout << "\n===== СОРТУВАННЯ ТА АНАЛІЗ =====\n";
  cout << "1. Сортувати методом бульбашки (послідовно)\n";
  cout << "2. Сортувати методом бульбашки (багатопотоково)\n";
  cout << "3. Перевірити чи масив відсортований\n";
  cout << "4. Показати метрики останнього сортування\n";
  cout << "0. Повернутися до головного меню\n";
  return getIntInput("Ваш вибір: ");
}

int main()
{
  vector<int> array;
  bool arrayLoaded = false;
  SortMetrics lastMetrics;
  vector<SortResult> sortResults;
  int lastUsedThreads = 1;

  bool continueProgram = true;
  while (continueProgram)
  {
    int mainChoice = showMainMenu();

    try
    {
      switch (mainChoice)
      {
      case 0: // Вихід
        cout << "Програма завершена.\n";
        return 0;

      case 1:
      { // Робота з масивом
        while (true)
        {
          int arrayChoice = showArrayMenu();

          if (arrayChoice == 0)
            break;

          switch (arrayChoice)
          {
          case 1:
          { // Генерація масиву
            int size = getIntInput("Введіть розмір масиву: ");
            int minValue = getIntInput("Введіть мінімальне значення: ");
            int maxValue = getIntInput("Введіть максимальне значення: ");

            if (size <= 0)
            {
              cout << "Розмір масиву повинен бути більшим за 0.\n";
              break;
            }

            if (minValue > maxValue)
            {
              cout << "Мінімальне значення не може бути більшим за максимальне. Значення поміняються місцями.\n";
              swap(minValue, maxValue);
            }

            array = ArrayOperations::generateRandomArray(size, minValue, maxValue);
            arrayLoaded = true;

            cout << "Масив успішно згенеровано.\n";
            printArrayInfo(array);

            // Пропозиція зберегти масив
            if (getYesNoInput("Бажаєте зберегти згенерований масив у файл?"))
            {
              saveArrayToFile(array);
            }

            // Очищаємо попередні результати сортування
            sortResults.clear();
            break;
          }
          case 2:
          { // Завантаження з файлу
            string filename = getStringInput("Введіть ім'я файлу для зчитування: ");
            array = ArrayOperations::loadArrayFromFile(filename);
            arrayLoaded = true;

            cout << "Масив успішно зчитано з файлу " << filename << endl;
            printArrayInfo(array);

            if (array.size() <= 100 && getYesNoInput("Бажаєте переглянути завантажений масив?"))
            {
              cout << "Масив: ";
              ArrayOperations::printArray(array);
            }

            // Очищаємо попередні результати сортування
            sortResults.clear();
            break;
          }
          case 3:
          { // Збереження у файл
            if (!arrayLoaded)
            {
              cout << "Помилка: спочатку потрібно завантажити або згенерувати масив.\n";
              break;
            }
            saveArrayToFile(array);
            break;
          }
          case 4:
          { // Перегляд масиву
            if (!arrayLoaded)
            {
              cout << "Помилка: спочатку потрібно завантажити або згенерувати масив.\n";
              break;
            }

            int maxElements = -1;
            if (array.size() > 100)
            {
              cout << "Масив дуже великий (" << array.size() << " елементів).\n";
              cout << "1. Показати весь масив\n";
              cout << "2. Показати обрізаний масив\n";
              int displayChoice = getIntInput("Ваш вибір: ");

              if (displayChoice == 1)
              {
                maxElements = array.size();
              }
              else
              {
                maxElements = getIntInput("Введіть максимальну кількість елементів для відображення: ");
                maxElements = max(10, maxElements); // Мінімум 10 елементів
              }
            }
            else
            {
              maxElements = array.size();
            }

            cout << "Масив: ";
            ArrayOperations::printArray(array, maxElements);
            break;
          }
          case 5:
          { // Інформація про масив
            if (!arrayLoaded)
            {
              cout << "Помилка: спочатку потрібно завантажити або згенерувати масив.\n";
              break;
            }

            printArrayInfo(array);
            break;
          }
          default:
            cout << "Помилка: невірний вибір. Виберіть опцію від 0 до 5.\n";
          }
        }
        break;
      }

      case 2:
      { // Сортування та аналіз
        while (true)
        {
          int sortChoice = showSortMenu();

          if (sortChoice == 0)
            break;

          if (!arrayLoaded && sortChoice >= 1 && sortChoice <= 3)
          {
            cout << "Помилка: спочатку потрібно завантажити або згенерувати масив.\n";
            continue;
          }

          switch (sortChoice)
          {
          case 1:
          { // Послідовне сортування
            // Зберігаємо копію масиву для можливості порівняння результатів
            vector<int> arrayCopy = array;

            cout << "Початок сортування масиву розміром " << array.size() << " елементів...\n";

            bool detailedMode = getDetailedMode();

            lastMetrics = ArrayOperations::bubbleSort(arrayCopy, detailedMode);

            bool isSorted = ArrayOperations::isSorted(arrayCopy);
            cout << "Масив " << (isSorted ? "успішно відсортований" : "НЕ відсортований") << ".\n";

            if (isSorted)
            {
              ArrayOperations::printMetrics(lastMetrics);

              // Зберігаємо результат для порівняння
              sortResults.push_back(SortResult("Послідовний", lastMetrics, 1));

              // Пропонуємо зберегти результат
              if (getYesNoInput("Бажаєте оновити оригінальний масив відсортованим?"))
              {
                array = arrayCopy;
                if (getYesNoInput("Бажаєте зберегти відсортований масив у файл?"))
                {
                  saveArrayToFile(array);
                }
              }
            }
            break;
          }
          case 2:
          { // Багатопотокове сортування
            // Зберігаємо копію масиву для можливості порівняння результатів
            vector<int> arrayCopy = array;

            cout << "Початок багатопотокового сортування масиву розміром " << array.size() << " елементів...\n";

            int numThreads = getIntInput("Введіть кількість потоків (0 для автоматичного визначення): ");

            bool detailedMode = getDetailedMode();

            lastMetrics = ArrayOperations::bubbleSortMultithreaded(arrayCopy, numThreads, detailedMode);
            lastUsedThreads = stoi(lastMetrics.additionalInfo["numThreads"]);

            bool isSorted = ArrayOperations::isSorted(arrayCopy);
            cout << "Масив " << (isSorted ? "успішно відсортований" : "НЕ відсортований") << ".\n";

            if (isSorted)
            {
              ArrayOperations::printMetrics(lastMetrics);

              // Зберігаємо результат для порівняння
              sortResults.push_back(SortResult("Багатопотоковий", lastMetrics, lastUsedThreads));

              // Пропонуємо зберегти результат
              if (getYesNoInput("Бажаєте оновити оригінальний масив відсортованим?"))
              {
                array = arrayCopy;
                if (getYesNoInput("Бажаєте зберегти відсортований масив у файл?"))
                {
                  saveArrayToFile(array);
                }
              }
            }
            break;
          }
          case 3:
          { // Перевірка сортування
            bool isSorted = ArrayOperations::isSorted(array);
            cout << "Результат перевірки: масив " << (isSorted ? "відсортований" : "НЕ відсортований") << endl;

            if (!isSorted && getYesNoInput("Бажаєте відсортувати масив?"))
            {
              cout << "Виберіть метод сортування:\n";
              cout << "1. Звичайне сортування\n";
              cout << "2. Багатопотокове сортування\n";
              int choice = getIntInput("Ваш вибір: ");

              bool detailedMode = getDetailedMode();

              if (choice == 1)
              {
                lastMetrics = ArrayOperations::bubbleSort(array, detailedMode);
                sortResults.push_back(SortResult("Послідовний", lastMetrics, 1));
              }
              else
              {
                int numThreads = getIntInput("Введіть кількість потоків (0 для автоматичного визначення): ");
                lastMetrics = ArrayOperations::bubbleSortMultithreaded(array, numThreads, detailedMode);
                lastUsedThreads = stoi(lastMetrics.additionalInfo["numThreads"]);
                sortResults.push_back(SortResult("Багатопотоковий", lastMetrics, lastUsedThreads));
              }

              cout << "Масив успішно відсортовано.\n";
              ArrayOperations::printMetrics(lastMetrics);

              if (getYesNoInput("Бажаєте зберегти відсортований масив у файл?"))
              {
                saveArrayToFile(array);
              }
            }
            break;
          }
          case 4:
          { // Показ метрик
            if (sortResults.empty())
            {
              cout << "Немає доступних результатів сортування.\n";
            }
            else
            {
              cout << "Доступні результати сортування:\n";
              for (size_t i = 0; i < sortResults.size(); i++)
              {
                cout << i + 1 << ". " << sortResults[i].name
                     << " (" << sortResults[i].metrics.executionTimeMs << " мс, "
                     << sortResults[i].numThreads << " потоків)\n";
              }

              int index = getIntInput("Виберіть номер результату для перегляду (0 для всіх): ");
              if (index == 0)
              {
                for (const auto &result : sortResults)
                {
                  cout << "\n=== " << result.name << " сортування ===\n";
                  ArrayOperations::printMetrics(result.metrics);
                }
              }
              else if (index >= 1 && index <= static_cast<int>(sortResults.size()))
              {
                ArrayOperations::printMetrics(sortResults[index - 1].metrics);
              }
              else
              {
                cout << "Невірний номер результату.\n";
              }
            }
            break;
          }
          default:
            cout << "Помилка: невірний вибір. Виберіть опцію від 0 до 4.\n";
          }
        }
        break;
      }

      case 3:
      { // Порівняння результатів
        if (sortResults.size() < 2)
        {
          cout << "Недостатньо результатів для порівняння. Виконайте принаймні два різні сортування.\n";
        }
        else
        {
          compareSortResults(sortResults);

          if (getYesNoInput("Бажаєте очистити історію результатів?"))
          {
            sortResults.clear();
            cout << "Історія результатів очищена.\n";
          }
        }
        break;
      }

      default:
        cout << "Помилка: невірний вибір. Виберіть опцію від 0 до 3.\n";
      }
    }
    catch (const exception &e)
    {
      cout << "Помилка: " << e.what() << endl;
    }
  }

  return 0;
}