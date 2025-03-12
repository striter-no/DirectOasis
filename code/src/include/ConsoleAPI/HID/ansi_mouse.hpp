#pragma once

#include <ConsoleAPI/terminal.hpp>
#include <termios.h>
#include <unistd.h>
#include <sys/select.h>
#include <sstream>

#include <utils/string.hpp>

enum class MouseButton {
    UNKNOWN,
    LEFT,
    RIGHT,
    MIDDLE,
    SCROLL_UP,
    SCROLL_DOWN
};

class Mouse {
private:
    Terminal& terminal;
    int x;
    int y;
    MouseButton pressedButton;
    struct termios oldSettings;
    bool isTrackingEnabled;

    void enableRawMode() {
        tcgetattr(STDIN_FILENO, &oldSettings);
        struct termios raw = oldSettings;
        raw.c_lflag &= ~(ICANON | ECHO);
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    }

    void disableRawMode() {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldSettings);
    }

    void flushInput() {
        while (terminal.hasInput()) {
            char c;
            read(STDIN_FILENO, &c, 1);
        }
    }

    std::string readEscapeSequence() {
        std::string seq;
        char c;
        
        // Читаем первый байт
        if (!terminal.hasInput() || read(STDIN_FILENO, &c, 1) != 1) return "";
        seq += c;

        // Проверяем начало escape-последовательности
        if (c != '\033') return seq;

        // Читаем второй байт
        if (!terminal.hasInput() || read(STDIN_FILENO, &c, 1) != 1) return seq;
        seq += c;

        // Проверяем формат CSI
        if (c != '[') return seq;

        // Читаем до конца последовательности
        while (terminal.hasInput()) {
            if (read(STDIN_FILENO, &c, 1) != 1) break;
            seq += c;
            
            // Проверяем конец SGR-последовательности
            if (c == 'M' || c == 'm') break;
        }
        return seq;
    }

public:
    Mouse(Terminal& term) : terminal(term), x(0), y(0), pressedButton(MouseButton::UNKNOWN), isTrackingEnabled(false) {}

    ~Mouse() {
        if (isTrackingEnabled) {
            stop();
        }
    }

    void start() {
        enableRawMode();
        terminal.enableMouse();
        terminal.raw_write("\033[?1006h"); // Включаем SGR и все события мыши
        isTrackingEnabled = true;
    }

    void stop() {
        terminal.raw_write("\033[?1006l"); // Выключаем SGR и отслеживание
        terminal.disableMouse();
        disableRawMode();
        isTrackingEnabled = false;
    }

    bool pollEvent() {
        if (!isTrackingEnabled) return false;

        std::string seq = readEscapeSequence();
        if (seq.empty()) return false;

        // Проверяем SGR-формат
        if (utils::str::startswith(seq, "\033[<")) {
            std::stringstream ss(seq.substr(3));
            std::string token;
            std::vector<std::string> parts;
            
            while (std::getline(ss, token, ';')) {
                parts.push_back(token);
            }
            
            if (parts.size() < 3) return false;
            
            int button = std::stoi(parts[0]);
            x = std::stoi(parts[1]) - 1; // Преобразуем в 0-based
            y = std::stoi(parts[2].substr(0, parts[2].size()-1)) - 1;
            
            bool isRelease = seq.back() == 'm';
            bool isScroll = (button & 0x40) != 0;
            
            if (isScroll) {
                switch (button & 0x0F) {
                    case 64: pressedButton = MouseButton::SCROLL_UP; break;
                    case 65: pressedButton = MouseButton::SCROLL_DOWN; break;
                    default: pressedButton = MouseButton::UNKNOWN;
                }
            } else {
                switch (button & 0x07) {
                    case 0: pressedButton = MouseButton::LEFT; break;
                    case 1: pressedButton = MouseButton::MIDDLE; break;
                    case 2: pressedButton = MouseButton::RIGHT; break;
                    default: pressedButton = MouseButton::UNKNOWN;
                }
            }
            
            if (isRelease) pressedButton = MouseButton::UNKNOWN;
            return true;
        }

        // Классический XTerm-формат
        if (utils::str::startswith(seq, "\033[M")) {
            if (seq.size() < 6) return false;
            
            unsigned char button = seq[3];
            unsigned char cx = seq[4];
            unsigned char cy = seq[5];
            
            x = cx - 33;
            y = cy - 33;
            
            if (button & 0x40) {
                pressedButton = MouseButton::UNKNOWN;
            } else {
                switch (button & 0x07) {
                    case 0: pressedButton = MouseButton::LEFT; break;
                    case 1: pressedButton = MouseButton::MIDDLE; break;
                    case 2: pressedButton = MouseButton::RIGHT; break;
                    default: pressedButton = MouseButton::UNKNOWN;
                }
            }
            return true;
        }
        
        return false;
    }

    bool setPosition(int x, int y) {
        #ifdef _WIN32
            // Для Windows: конвертируем координаты терминала в экранные
            HWND hwnd = GetConsoleWindow();
            if (!hwnd) return false;

            RECT rect;
            GetWindowRect(hwnd, &rect);
            
            int termWidth, termHeight;
            terminal.getSize(termWidth, termHeight);
            
            // Рассчитываем экранные координаты
            int screenX = rect.left + (x * (rect.right - rect.left)) / termWidth;
            int screenY = rect.top + (y * (rect.bottom - rect.top)) / termHeight;
            
            return SetCursorPos(screenX, screenY);
        #else
            std::stringstream ss;
            ss << "\033[" << y+1 << ";" << x+1 << "M";
            terminal.raw_write(ss.str());
            this->x = x;
            this->y = y;
            return true;
        #endif
    }

    bool center() {
        int width, height;
        terminal.getSize(width, height);
        return setPosition(width/2, height/2);
    }

    std::pair<int, int> getPosition() const {
        return {x, y};
    }

    bool isButtonPressed(MouseButton button) const {
        return pressedButton == button;
    }
};