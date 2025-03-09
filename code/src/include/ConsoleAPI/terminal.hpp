#pragma once

#include <iostream>
#include <utility>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <termios.h>
#endif

class Terminal {
    private:
        #ifdef _WIN32
            HANDLE hConsole;
            HANDLE hConsoleOut;
            DWORD originalConsoleMode;
        #else
            struct termios originalTermios;
            int fdOut;
        #endif

    public:
        Terminal() {
            #ifdef _WIN32
                hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
                GetConsoleMode(hConsole, &originalConsoleMode);
            #else
                tcgetattr(STDIN_FILENO, &originalTermios);
                fdOut = STDOUT_FILENO;
                setvbuf(stdout, nullptr, _IONBF, 0);
            #endif
        }

        ~Terminal() {
            #ifdef _WIN32
                SetConsoleMode(hConsole, originalConsoleMode);
            #else
                tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
            #endif
        }

        // Вывод значений с поддержкой разных типов
        template<class T>
        void print(T value) {
            std::wcout << value;  
        }

        template<class... Args>
        void print(Args... args) {
            (print(args), ...);
        }

        // Включение ANSI-последовательностей (Windows)
        void startANSI() {
            #ifdef _WIN32
                DWORD mode = originalConsoleMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hConsole, mode);
            #endif
        }

        // Получение размеров терминала
        void getSize(int &width, int &height) {
            #ifdef _WIN32
                CONSOLE_SCREEN_BUFFER_INFO csbi;
                GetConsoleScreenBufferInfo(hConsole, &csbi);
                width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
                height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            #else
                struct winsize ws{};
                ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
                width = ws.ws_col;
                height = ws.ws_row;
            #endif
        }

        // Управление мышью
        void enableMouse() {
            #ifdef _WIN32
                DWORD mode;
                GetConsoleMode(hConsole, &mode);
                mode |= ENABLE_MOUSE_INPUT;
                SetConsoleMode(hConsole, mode);
            #else
                print("\033[?1003h");
            #endif
        }

        void disableMouse() {
            #ifdef _WIN32
                DWORD mode;
                GetConsoleMode(hConsole, &mode);
                mode &= ~ENABLE_MOUSE_INPUT;
                SetConsoleMode(hConsole, mode);
            #else
                print("\033[?1003l");
            #endif
        }

        // Управление режимом ввода
        void enableRawInput() {
            #ifdef _WIN32
                DWORD mode;
                GetConsoleMode(hConsole, &mode);
                mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT);
                SetConsoleMode(hConsole, mode);
            #else
                struct termios t = originalTermios;
                t.c_lflag &= ~(ICANON | ECHO);
                tcsetattr(STDIN_FILENO, TCSANOW, &t);
            #endif
        }

        void restoreInput() {
            #ifdef _WIN32
                SetConsoleMode(hConsole, originalConsoleMode);
            #else
                tcsetattr(STDIN_FILENO, TCSANOW, &originalTermios);
            #endif
        }

        void 
};