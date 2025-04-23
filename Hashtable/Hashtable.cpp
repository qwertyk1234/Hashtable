#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <locale>
#include <codecvt>
#include <io.h>
#include <fcntl.h>
#include <algorithm>
#include <map>

using namespace std;

// Установка кодировки UTF-8
void setUTF8Console() {
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    setlocale(LC_ALL, "");
}

// Очистка строки от пробелов и невидимых символов
wstring cleanString(const wstring& str) {
    wstring cleaned;
    for (wchar_t ch : str) {
        if (!iswspace(ch) || ch == L' ') { // Убираем табы, \r, \n, но оставляем обычный пробел
            cleaned += ch;
        }
    }
    return cleaned;
}

// Класс для хеш-таблицы тегов HTML
class HTMLHashTable {
private:
    unordered_map<wstring, wstring> table;

public:
    // Добавление тега в хеш-таблицу
    void addTag(const wstring& tag, const wstring& description) {
        table[tag] = description;
    }

    // Удаление тега из хеш-таблицы
    void removeTag(const wstring& tag) {
        auto it = table.find(tag);
        if (it != table.end()) {
            table.erase(it);
        }
        else {
            wcout << L"Тег " << tag << L" не найден в хеш-таблице!" << endl;
        }
    }

    // Поиск тега в хеш-таблице
    bool findTag(const wstring& tag, wstring& description) const {
        auto it = table.find(tag);
        if (it != table.end()) {
            description = it->second;
            return true;
        }
        return false;
    }

    // Вывод всех тегов в хеш-таблице
    void printAllTags() const {
        if (table.empty()) {
            wcout << L"Нет тегов для вывода!" << endl;
            return;
        }

        // Сортировка тегов по ключу
        vector<pair<wstring, wstring>> sortedTags(table.begin(), table.end());
        sort(sortedTags.begin(), sortedTags.end());

        // Вывод всех тегов
        for (const auto& pair : sortedTags) {
            wcout << L"Тег: " << pair.first << endl;
            wcout << L"Описание: " << pair.second << endl;
            wcout << L"-------------------------------" << endl;
        }
    }

    // Загрузка тегов из файла в хеш-таблицу
    void loadFromFile(const string& filename) {
        wifstream file(filename);
        file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        if (!file.is_open()) {
            wcerr << L"Ошибка открытия файла: " << filename.c_str() << endl;
            return;
        }

        wstring tag, desc, line;
        while (true) {
            // Считываем тег
            if (!getline(file, tag)) break;
            // Пропускаем пустые строки между тегами
            while (tag.empty() && getline(file, tag));

            // Считываем описание
            if (!getline(file, desc)) break;

            // Пропускаем разделительную пустую строку (если есть)
            getline(file, line);

            // Очистка пробелов и невидимых символов
            tag = cleanString(tag);
            desc = cleanString(desc);

            if (!tag.empty() && !desc.empty()) {
                addTag(tag, desc);
            }
        }

        file.close();
    }

    // Сохранение тегов в файл
    void saveToFile(const string& filename) const {
        wofstream file(filename);
        file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        if (!file.is_open()) {
            wcerr << L"Ошибка записи в файл: " << filename.c_str() << endl;
            return;
        }

        for (const auto& pair : table) {
            file << pair.first << endl;
            file << pair.second << endl;
            file << endl;  // Пустая строка для разделения
        }

        file.close();
    }

    // Проверка существования тега в all_tags.txt
    bool tagExistsInAllTags(const wstring& tag, const string& allTagsFile) {
        wifstream file(allTagsFile);
        file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        if (!file.is_open()) {
            wcerr << L"Ошибка открытия файла: " << allTagsFile.c_str() << endl;
            return false;
        }

        wstring fileTag, fileDesc, line;
        while (true) {
            if (!getline(file, fileTag)) break;
            while (fileTag.empty() && getline(file, fileTag)); // Пропускаем пустые строки

            if (!getline(file, fileDesc)) break;

            // Пропускаем пустую строку
            getline(file, line);

            fileTag = cleanString(fileTag);
            fileDesc = cleanString(fileDesc);

            if (fileTag == tag) {
                file.close();
                return true;
            }
        }
        file.close();
        return false;
    }

    // Добавление тега из файла all_tags.txt в хеш-таблицу
    void addTagFromAllTags(const wstring& tag, const string& allTagsFile) {
        wifstream file(allTagsFile);
        file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        if (!file.is_open()) {
            wcerr << L"Ошибка открытия файла: " << allTagsFile.c_str() << endl;
            return;
        }

        wstring fileTag, fileDesc, line;
        while (true) {
            if (!getline(file, fileTag)) break;
            while (fileTag.empty() && getline(file, fileTag)); // Пропускаем пустые строки

            if (!getline(file, fileDesc)) break;

            // Пропускаем пустую строку
            getline(file, line);

            fileTag = cleanString(fileTag);
            fileDesc = cleanString(fileDesc);

            if (fileTag == tag) {
                addTag(fileTag, fileDesc);
                break;
            }
        }
        file.close();
    }

    // Вывод всех тегов из файла 'all_tags.txt'
    void printAllTagsFromFile(const string& filename) {
        wifstream file(filename);
        file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        if (!file.is_open()) {
            wcerr << L"Ошибка открытия файла: " << filename.c_str() << endl;
            return;
        }

        wstring tag, desc, line;
        map<wchar_t, map<wstring, wstring>> fileTable;
        while (true) {
            // Считываем тег
            if (!getline(file, tag)) break;
            // Пропускаем пустые строки между тегами
            while (tag.empty() && getline(file, tag));

            // Считываем описание
            if (!getline(file, desc)) break;

            // Пропускаем разделительную пустую строку (если есть)
            getline(file, line);

            // Очистка пробелов и невидимых символов
            tag = cleanString(tag);
            desc = cleanString(desc);

            if (!tag.empty() && !desc.empty()) {
                wchar_t firstChar = tolower(tag[1]);
                fileTable[firstChar][tag] = desc;
            }
        }

        file.close();

        // Выводим теги из файла
        for (const auto& firstCharPair : fileTable) {
            wcout << L"Теги, начинающиеся на '" << firstCharPair.first << L"':" << endl;
            for (const auto& tagPair : firstCharPair.second) {
                wcout << L"Тег: " << tagPair.first << endl;
                wcout << L"Описание: " << tagPair.second << endl;
                wcout << L"-------------------------------" << endl;
            }
        }
    }
};

// Функция для вывода содержимого файла 'Exercise.txt'
void printExerciseFile(const string& filename) {
    wifstream file(filename);
    file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    if (!file.is_open()) {
        wcerr << L"Ошибка открытия файла: " << filename.c_str() << endl;
        return;
    }

    wstring line;
    while (getline(file, line)) {
        wcout << line << endl;
    }

    file.close();
}

int main() {
    setUTF8Console();

    HTMLHashTable outputTags;
    outputTags.loadFromFile("input.txt");

    while (true) {
        wcout << L"Выберите действие:" << endl;
        wcout << L"1. Добавить тег" << endl;
        wcout << L"2. Удалить тег" << endl;
        wcout << L"3. Найти тег" << endl;
        wcout << L"4. Вывести все теги" << endl;
        wcout << L"5. Вывести задание по хэшированию" << endl;
        wcout << L"6. Вывести все теги из файла 'all_tags.txt'" << endl;
        wcout << L"7. Выход" << endl;

        int choice;
        wcin >> choice;
        wcin.ignore();  // игнорируем символ новой строки после ввода числа

        if (choice == 1) {
            wcout << L"Введите тег для добавления: ";
            wstring tag;
            getline(wcin, tag);

            wstring cleanedTag = cleanString(tag);

            // Проверка существования тега в all_tags.txt
            if (outputTags.tagExistsInAllTags(cleanedTag, "all_tags.txt")) {
                outputTags.addTagFromAllTags(cleanedTag, "all_tags.txt");
                outputTags.saveToFile("output.txt");
            }
            else {
                wcout << L"Такого тега не существует в файле 'all_tags.txt'!" << endl;
            }
        }
        else if (choice == 2) {
            wcout << L"Введите тег для удаления: ";
            wstring tag;
            getline(wcin, tag);

            outputTags.removeTag(tag);
            outputTags.saveToFile("output.txt");
        }
        else if (choice == 3) {
            wcout << L"Введите тег для поиска: ";
            wstring tag;
            getline(wcin, tag);

            wstring description;
            if (outputTags.findTag(tag, description)) {
                wcout << L"Описание тега: " << description << endl;
            }
            else {
                wcout << L"Тег не найден!" << endl;
            }
        }
        else if (choice == 4) {
            outputTags.printAllTags();  // Вывод всех тегов
        }
        else if (choice == 5) {
            printExerciseFile("Exercise.txt");  // Вывод содержимого файла Exercise.txt
        }
        else if (choice == 6) {
            outputTags.printAllTagsFromFile("all_tags.txt");
        }
        else if (choice == 7) {
            break;
        }
    }

    return 0;
}
