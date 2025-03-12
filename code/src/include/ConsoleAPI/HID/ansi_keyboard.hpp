#pragma once

#include <ConsoleAPI/terminal.hpp>
#include <sstream>
#include <cstring>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/select.h>
    #include <cstring>
#endif

#include <utils/string.hpp>
#include <algorithm>

struct KeyMap {
    const char* sequence;
    std::wstring name;
    int code;
};

struct Key {
    std::wstring name;
    int code;
};

// Стандартные ESC-последовательности для Unix
const KeyMap keyMap[] = {
    {"\033[A", L"UP", 256},
    {"\033[B", L"DOWN", 257},
    {"\033[C", L"RIGHT", 258},
    {"\033[D", L"LEFT", 259},
    {"\033[H", L"HOME", 260},
    {"\033[F", L"END", 261},
    {"\033[2~", L"INSERT", 262},
    {"\033[3~", L"DELETE", 263},
    {"\033[5~", L"PAGE_UP", 264},
    {"\033[6~", L"PAGE_DOWN", 265},
    {"\033[Z", L"SHIFT_TAB", 266},
    {"\033OP", L"F1", 267},
    {"\033OQ", L"F2", 268},
    {"\033OR", L"F3", 269},
    {"\033OS", L"F4", 270},
    {"\033[15~", L"F5", 271},
    {"\033[17~", L"F6", 272},
    {"\033[18~", L"F7", 273},
    {"\033[19~", L"F8", 274},
    {"\033[20~", L"F9", 275},
    {"\033[21~", L"F10", 276},
    {"\033[23~", L"F11", 277},
    {"\033[24~", L"F12", 278}
};

const size_t keyMapSize = sizeof(keyMap) / sizeof(keyMap[0]);

class Keyboard {
        Terminal &terminal;
        std::vector<Key> pressed_keys;

    public:

        void start(){
            terminal.enableRawInput();
        }
        void pollEvents() {
            #ifdef _WIN32
                DWORD events;
                INPUT_RECORD inputRecord;
                HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
                
                while (PeekConsoleInput(hInput, &inputRecord, 1, &events) && events > 0) {
                    if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
                        KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
                        
                        // Обработка специальных клавиш
                        if (keyEvent.wVirtualKeyCode == VK_UP) {
                            pressed_keys.push_back({L"UP", VK_UP});
                        } else if (keyEvent.wVirtualKeyCode == VK_DOWN) {
                            pressed_keys.push_back({L"DOWN", VK_DOWN});
                        } else if (keyEvent.wVirtualKeyCode == VK_LEFT) {
                            pressed_keys.push_back({L"LEFT", VK_LEFT});
                        } else if (keyEvent.wVirtualKeyCode == VK_RIGHT) {
                            pressed_keys.push_back({L"RIGHT", VK_RIGHT});
                        } else if (keyEvent.uChar.UnicodeChar != 0) {
                            // Обработка Unicode символов
                            wchar_t uniChar = keyEvent.uChar.UnicodeChar;
                            pressed_keys.push_back({std::wstring(1, uniChar), uniChar});
                        }
                    }
                    ReadConsoleInput(hInput, &inputRecord, 1, &events);
                }
            #else
                while (terminal.hasInput()) {
                    char c;
                    read(STDIN_FILENO, &c, 1);
                    
                    if (c == '\033') { // Начало ESC-последовательности
                        std::string seq(1, c);
                        char next;
                        
                        if (read(STDIN_FILENO, &next, 1) == 1) {
                            seq += next;
                            if (next == '[') { // CSI sequence
                                while (true) {
                                    if (read(STDIN_FILENO, &next, 1) == 1) {
                                        seq += next;
                                        if (next >= 0x40 && next <= 0x7E) break;
                                    }
                                }
                                // Поиск в карте клавиш
                                for (size_t i = 0; i < keyMapSize; ++i) {
                                    if (seq == keyMap[i].sequence) {
                                        pressed_keys.push_back({keyMap[i].name, keyMap[i].code});
                                        break;
                                    }
                                }
                            }
                        }
                    } else if ((c & 0xC0) == 0xC0) { // Начало UTF-8 символа
                        std::string utf8_char(1, c);
                        int remaining = 0;
                        if ((c & 0xE0) == 0xC0) remaining = 1;
                        else if ((c & 0xF0) == 0xE0) remaining = 2;
                        else if ((c & 0xF8) == 0xF0) remaining = 3;
                        
                        while (remaining > 0 && terminal.hasInput()) {
                            read(STDIN_FILENO, &c, 1);
                            utf8_char += c;
                            remaining--;
                        }
                        
                        try {
                            std::wstring wstr;
                            wstr.append(utf8_char.begin(), utf8_char.end());
                            if (!wstr.empty()) {
                                pressed_keys.push_back({wstr, static_cast<int>(wstr[0])});
                            }
                        } catch (const std::exception&) {}
                    } else { // ASCII символ
                        pressed_keys.push_back({std::wstring(1, c), static_cast<int>(c)});
                    }
                }
            #endif
        }

        void stop(){
            terminal.restoreInput();
        }

        bool isKeyPressed(int key) {
            for (const auto& k : pressed_keys) {
                if (k.code == key) return true;
            }
            return false;
        }

        bool isKeyPressed(const std::wstring& name) {
            for (const auto& k : pressed_keys) {
                if (k.name == name) {
                    // Erase key after processing it to avoid duplicate detection
                    pressed_keys.erase(std::remove_if(pressed_keys.begin(), pressed_keys.end(), [k](const Key& key) { return key.name == k.name; }), pressed_keys.end());
                    return true;
                }
            }
            return false;
        }

        bool isKeyPressed(const Key& key) {
            return isKeyPressed(key.code) || isKeyPressed(key.name);
        }

        Key getKeyPressed() {
            if (pressed_keys.empty()) return {L"", 0};
            Key key = pressed_keys.front();
            pressed_keys.erase(pressed_keys.begin());
            return key;
        }

        std::wstring getUnicodePressed() {
            std::wstring result;
            for (auto it = pressed_keys.begin(); it != pressed_keys.end();) {
                if (it->name.size() == 1) {
                    result += it->name;
                    it = pressed_keys.erase(it);
                } else {
                    ++it;
                }
            }
            return result;
        }

        Keyboard(Terminal &terminal): terminal(terminal) {}
        ~Keyboard(){}
};