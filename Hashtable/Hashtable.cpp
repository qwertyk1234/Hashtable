#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <map>
#include <locale>
#include <codecvt>
#include <io.h>
#include <fcntl.h>
#include <algorithm>

using namespace std;

void setUTF8Console() {
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    setlocale(LC_ALL, "");
}

wstring cleanString(const wstring& str) {
    wstring cleaned;
    for (wchar_t ch : str) {
        if (!iswspace(ch) || ch == L' ') {
            cleaned += ch;
        }
    }
    return cleaned;
}

class HTMLHashTable {
private:
    map<wchar_t, map<wstring, wstring>> table; // ключ - первый символ после '<'

public:
    void addTag(const wstring& tag, const wstring& description) {
        wchar_t key = towlower(tag[1]);
        table[key][tag] = description;
    }

    void removeTag(const wstring& tag) {
        wchar_t key = towlower(tag[1]);
        if (table.count(key) && table[key].count(tag)) {
            table[key].erase(tag);
            if (table[key].empty()) table.erase(key);
        }
        else {
            wcout << L"Такого тега нет в хеш-таблице!" << endl;
        }
    }

    bool findTag(const wstring& tag, wstring& description) const {
        wchar_t key = towlower(tag[1]);
        auto keyIt = table.find(key);
        if (keyIt != table.end()) {
            auto tagIt = keyIt->second.find(tag);
            if (tagIt != keyIt->second.end()) {
                description = tagIt->second;
                return true;
            }
        }
        return false;
    }

    void printAllTags() const {
        for (const auto& outer : table) {
            wcout << L"Теги, начинающиеся на '" << outer.first << L"':" << endl;
            for (const auto& pair : outer.second) {
                wcout << L"Тег: " << pair.first << endl;
                wcout << L"Описание: " << pair.second << endl;
                wcout << L"-------------------------------" << endl;
            }
        }
    }

    void saveToFile(const string& filename) const {
        wofstream file(filename);
        file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        if (!file.is_open()) {
            wcerr << L"Ошибка записи в файл: " << filename.c_str() << endl;
            return;
        }
        for (const auto& outer : table) {
            for (const auto& pair : outer.second) {
                file << pair.first << endl;
                file << pair.second << endl;
                file << endl;
            }
        }
        file.close();
    }

    bool tagExistsInAllTags(const wstring& tag, const string& allTagsFile) {
        wifstream file(allTagsFile);
        file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        if (!file.is_open()) return false;

        wstring fileTag, fileDesc, line;
        while (getline(file, fileTag)) {
            getline(file, fileDesc);
            getline(file, line);
            if (cleanString(fileTag) == tag) {
                return true;
            }
        }
        return false;
    }

    void addTagFromAllTags(const wstring& tag, const string& allTagsFile) {
        wifstream file(allTagsFile);
        file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
        if (!file.is_open()) return;

        wstring fileTag, fileDesc, line;
        while (getline(file, fileTag)) {
            getline(file, fileDesc);
            getline(file, line);
            if (cleanString(fileTag) == tag) {
                addTag(fileTag, fileDesc);
                break;
            }
        }
    }
};

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

void printAllTagsBeautifully(const string& filename) {
    wifstream file(filename);
    file.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    if (!file.is_open()) {
        wcerr << L"Ошибка открытия файла: " << filename.c_str() << endl;
        return;
    }

    map<wchar_t, map<wstring, wstring>> table;
    wstring tag, desc, line;
    while (getline(file, tag)) {
        getline(file, desc);
        getline(file, line);
        tag = cleanString(tag);
        desc = cleanString(desc);
        if (!tag.empty() && !desc.empty()) {
            wchar_t key = towlower(tag[1]);
            table[key][tag] = desc;
        }
    }

    for (const auto& outer : table) {
        wcout << L"Теги, начинающиеся на '" << outer.first << L"':" << endl;
        for (const auto& pair : outer.second) {
            wcout << L"Тег: " << pair.first << endl;
            wcout << L"Описание: " << pair.second << endl;
            wcout << L"-------------------------------" << endl;
        }
    }
}

int main() {
    setUTF8Console();
    HTMLHashTable htmlTags;

    // Загружаем начальные данные из input.txt
    wifstream input("input.txt");
    input.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    if (input.is_open()) {
        wstring tag, desc, line;
        while (getline(input, tag)) {
            getline(input, desc);
            getline(input, line);
            htmlTags.addTag(cleanString(tag), cleanString(desc));
        }
        htmlTags.saveToFile("output.txt");
    }

    while (true) {
        wcout << L"\nМеню:" << endl;
        wcout << L"1. Добавить тег" << endl;
        wcout << L"2. Удалить тег" << endl;
        wcout << L"3. Найти тег" << endl;
        wcout << L"4. Вывести все теги" << endl;
        wcout << L"5. Показать задание (Exercise.txt)" << endl;
        wcout << L"6. Вывести все теги из all_tags.txt" << endl;
        wcout << L"7. Выход" << endl;

        int choice;
        wcin >> choice;
        wcin.ignore();

        if (choice == 1) {
            wcout << L"Введите тег для добавления: ";
            wstring tag;
            getline(wcin, tag);
            tag = cleanString(tag);

            if (htmlTags.tagExistsInAllTags(tag, "all_tags.txt")) {
                htmlTags.addTagFromAllTags(tag, "all_tags.txt");
                htmlTags.saveToFile("output.txt");
                wcout << L"Тег успешно добавлен!" << endl;
            }
            else {
                wcout << L"Такой тег не найден в all_tags.txt!" << endl;
            }
        }
        else if (choice == 2) {
            wcout << L"Введите тег для удаления: ";
            wstring tag;
            getline(wcin, tag);
            htmlTags.removeTag(tag);
            htmlTags.saveToFile("output.txt");
        }
        else if (choice == 3) {
            wcout << L"Введите тег для поиска: ";
            wstring tag;
            getline(wcin, tag);
            wstring description;
            if (htmlTags.findTag(tag, description)) {
                wcout << L"Описание тега: " << description << endl;
            }
            else {
                wcout << L"Тег не найден!" << endl;
            }
        }
        else if (choice == 4) {
            htmlTags.printAllTags();
        }
        else if (choice == 5) {
            printExerciseFile("Exercise.txt");
        }
        else if (choice == 6) {
            printAllTagsBeautifully("all_tags.txt");
        }
        else if (choice == 7) {
            break;
        }
    }
    return 0;
}