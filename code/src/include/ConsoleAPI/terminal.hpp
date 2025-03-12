#pragma once

#include <iostream>
#include <utility>
#include <sstream>
#include <atomic>

std::atomic<bool> g_interrupted{false};

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>
#else
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <termios.h>
    #include <iconv.h>
    #include <cerrno>
    #include <csignal>
    void ctrlChandler(int signal) {
        if (signal == SIGINT) {
            g_interrupted.store(true);
        }
    }
#endif

enum class CtrlKey {
    C, Z, X, R, S, Q, V, B, N
};

class Terminal {
    private:
        int width, height;
        std::wstring prestring;
        

        #ifdef _WIN32
            HANDLE hConsole;
            HANDLE hConsoleOut;
            DWORD originalConsoleMode;
        #else
            struct termios originalTermios;
            int fdOut;
        #endif

        // Конвертер wchar_t → UTF-8 с использованием iconv
        std::string wideToUTF8(const std::wstring& wstr) {
            // std::cout << 1 << std::endl;
            iconv_t cd = iconv_open("UTF-8", "WCHAR_T");
            if (cd == (iconv_t)-1) {
                throw std::runtime_error("iconv_open failed");
            }

            size_t inbytes = wstr.size() * sizeof(wchar_t);
            char* inbuf = (char*)wstr.data();
            std::string result(wstr.size() * 4, '\0');
            char* outbuf = result.data();
            size_t outbytes = result.size();

            if (iconv(cd, &inbuf, &inbytes, &outbuf, &outbytes) == (size_t)-1) {
                iconv_close(cd);
                throw std::runtime_error("Conversion error");
            }

            iconv_close(cd);
            result.resize(result.size() - outbytes);
            return result;
        }

        bool isCtrlPressedWin(CtrlKey key) {
            #ifdef _WIN32
                DWORD events;
                INPUT_RECORD inputRecord;
                if (!PeekConsoleInput(hConsole, &inputRecord, 1, &events) || events == 0)
                    return false;

                if (inputRecord.EventType != KEY_EVENT || !inputRecord.Event.KeyEvent.bKeyDown)
                    return false;

                KEY_EVENT_RECORD keyEvent = inputRecord.Event.KeyEvent;
                bool ctrlPressed = (keyEvent.dwControlKeyState & (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)) != 0;

                if (!ctrlPressed)
                    return false;

                switch (key) {
                    case CtrlKey::C: return keyEvent.wVirtualKeyCode == 'C';
                    case CtrlKey::Z: return keyEvent.wVirtualKeyCode == 'Z';
                    case CtrlKey::X: return keyEvent.wVirtualKeyCode == 'X';
                    // Добавьте другие клавиши по аналогии
                }
            #endif
            return false;
        }

        // Для Unix
        bool isCtrlPressedUnix(CtrlKey key) {
            #ifndef _WIN32
                if (!hasInput())
                    return false;

                char c = 0;
                read(STDIN_FILENO, &c, 1);

                switch (key) {
                    case CtrlKey::C: return c == 3;   // Ctrl+C
                    case CtrlKey::Z: return c == 26;  // Ctrl+Z
                    case CtrlKey::X: return c == 24;  // Ctrl+X
                    case CtrlKey::R: return c == 18;  // Ctrl+R
                    case CtrlKey::S: return c == 19;  // Ctrl+S
                    case CtrlKey::Q: return c == 17;  // Ctrl+Q
                    // Добавьте другие комбинации
                }
            #endif
            return false;
        }

        #ifdef _WIN32
        BOOL WINAPI consoleHandler(DWORD signal) {
            if (signal == CTRL_C_EVENT) {
                g_interrupted = true;
                return TRUE;
            }
            return FALSE;
        }
        #endif

        void _setup(){
            #ifdef _WIN32
            SetConsoleCtrlHandler(consoleHandler, TRUE);
            #else
            std::signal(SIGINT, ctrlChandler);
            #endif
        }

    public:

        bool isCtrlCPressed() const {
            return g_interrupted.load();
        }

        void resetInterrupt() {
            g_interrupted.store(false);
        }

        // bool isCtrlPressed(CtrlKey key) {
        //     #ifdef _WIN32
        //         return isCtrlPressedWin(key);
        //     #else
        //         return isCtrlPressedUnix(key);
        //     #endif
        // }

        // Проверка наличия ввода (для Unix)
        bool hasInput() {
            #ifndef _WIN32
                struct timeval tv = {0, 0};
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(STDIN_FILENO, &fds);
                return select(STDIN_FILENO+1, &fds, nullptr, nullptr, &tv) > 0;
            #endif
            return false;
        }

        bool supportsVT() {
            #ifdef _WIN32
                DWORD mode;
                GetConsoleMode(hConsole, &mode);
                return (mode & ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
            #else
                return true; 
            #endif
        }

        Terminal() {
            _setup();
            #ifdef _WIN32
                hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
                hConsoleOut = GetStdHandle(STD_OUTPUT_HANDLE);
                GetConsoleMode(hConsole, &originalConsoleMode);
            #else
                tcgetattr(STDIN_FILENO, &originalTermios);
                fdOut = STDOUT_FILENO;
                setvbuf(stdout, nullptr, _IONBF, 0);
            #endif

            getSize(width, height);
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

        std::pair<int, int> getSize() {
            int width, height;
            getSize(width, height);
            return std::make_pair(width, height);
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
                raw_write("\033[?1003h");
            #endif
        }

        void disableMouse() {
            #ifdef _WIN32
                DWORD mode;
                GetConsoleMode(hConsole, &mode);
                mode &= ~ENABLE_MOUSE_INPUT;
                SetConsoleMode(hConsole, mode);
            #else
                raw_write("\033[?1003l");
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

        void draw(std::wstring &&data) {
            draw(data);
        }

        void draw(std::wstring &data) {
            #ifdef _WIN32
                DWORD written;
                if(!WriteConsoleW(hConsoleOut, data.c_str(), static_cast<DWORD>(length), &written, nullptr)) {
                    throw std::runtime_error("Write console error");
                }
            #else
                std::string converted_str = wideToUTF8(data);

                if(converted_str.size() == static_cast<size_t>(-1)) 
                    throw std::runtime_error("Encoding error");
                
                ssize_t result = write(fdOut, converted_str.c_str(), converted_str.size());
                
                if(result == -1) 
                    throw std::runtime_error("Write error");
            #endif
        }

        void fill(wchar_t simb){
            int width, height;
            getSize(width, height);

            std::wstring data;
            for(int y = 0; y < height; ++y)
                data += std::wstring(width, simb) + L'\n';

            draw(data);
        }

        std::wstring getFullRect(wchar_t simb = L' '){
            int width, height;
            getSize(width, height);

            std::wstring data;
            for(int y = 0; y < height; ++y)
                data += std::wstring(width, simb) + L'\n';
            
            return data;
        }

        void pix(int x, int y, wchar_t simb, std::wstring &data){            
            if(x >= 0 && x < width && y >= 0 && y < height)
                data[y * width + x] = simb;
        }

        void cursor(int x, int y){
            #ifdef _WIN32
                COORD coord = {static_cast<SHORT>(x), static_cast<SHORT>(y)};
                SetConsoleCursorPosition(hConsoleOut, coord);
            #else
                std::wstringstream ss;
                ss << L"\033[" << y+1 << ";" << x+1 << "H";
                draw(ss.str());
            #endif
        }

        void clear(){
            raw_write("\033[H");
        }

        void hideCursor() {
            #ifdef _WIN32
                CONSOLE_CURSOR_INFO cci;
                cci.dwSize = 1;
                cci.bVisible = FALSE;
                SetConsoleCursorInfo(hConsole, &cci);
            #else
                raw_write("\033[?25l");
            #endif
        }

        void showCursor() {
            #ifdef _WIN32
                CONSOLE_CURSOR_INFO cci;
                cci.dwSize = 1;
                cci.bVisible = TRUE;
                SetConsoleCursorInfo(hConsole, &cci);
            #else
                raw_write("\033[?25h");
            #endif
        }

        void enableVT(){
            #ifdef _WIN32
                DWORD mode;
                GetConsoleMode(hConsole, &mode);
                mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
                SetConsoleMode(hConsole, mode);
            #else
                raw_write("\033[?1049h");
            #endif
        }

        void raw_write(std::string data){
            ssize_t result = write(fdOut, data.c_str(), data.size());
            if (result == -1) {
                throw std::runtime_error("Show cursor error");
            }
        }
};