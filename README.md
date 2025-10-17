# Scanner Project


Проект: утилита командной строки для сканирования директорий и поиска "вредоносных" файлов по MD5-хешам.


## Особенности
- C++17
- DLL (scanner_core) с основной логикой
- CLI-программа (scanner_app)
- Многопоточность — пул потоков, число рабочих = std::thread::hardware_concurrency()
- Поддержка Unicode-путей (используется std::filesystem::path и path.u8string() для записи логов)
- Логирование в текстовый файл (std::ofstream)
- Загрузка базы из CSV (`<md5>;<вердикт>`)
- Юнит-тесты на Google Test


## Сборка (Windows / Linux)


1. Создайте директорию сборки и войдите в неё:


```bash
mkdir build && cd build
cmake ..
cmake --build . --config Release
```


2. Запуск приложения (пример):

```./src/app/scanner_app --base ../sample_base.csv --log ../report.log --path C:/path/to/scan```4

3. Запуск тетов:
```ctest --verbose```