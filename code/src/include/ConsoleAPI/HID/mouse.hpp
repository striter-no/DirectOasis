// WindowMouse.h (полная реализация)
#pragma once

#include <utility>
#include <cstdint>
#include "./ansi_mouse.hpp"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>
#endif

#ifdef _WIN32
using WindowHandle = HWND;
#elif __linux__
using WindowHandle = Window;
#endif

class WindowMouse {
public:

    WindowMouse() {
        #ifdef _WIN32
            hwnd = GetActiveWindow(); // Получаем окно текущего потока
            if (!hwnd) hwnd = GetForegroundWindow();
        #elif __linux__
            display = XOpenDisplay(nullptr);
            if (!display) throw std::runtime_error("Cannot open X display");
            int revert_to;
            XGetInputFocus(display, &windowHandle, &revert_to);
        #endif
    }

    ~WindowMouse() {
        #ifdef __linux__
            if (display) XCloseDisplay(display);
        #endif
    }

    void pollEvents() {
        updatePosition();
        updateButtons();
    }

    bool isButtonPressed(MouseButton button) const {
        return currentButton == button;
    }

    void moveMouse(int x, int y) {
        #ifdef _WIN32
            POINT clientPoint = { x, y };
            ClientToScreen(hwnd, &clientPoint);
            SetCursorPos(clientPoint.x, clientPoint.y);
        #elif __linux__
            XWarpPointer(display, None, windowHandle, 0, 0, 0, 0, x, y);
            XFlush(display);
        #endif
        position = {x, y};
    }

    std::pair<int, int> getPosition() const {
        return position;
    }

    MouseButton getButtonPressed() const {
        return currentButton;
    }

    void lockCursor() {
        #ifdef _WIN32
            RECT rect;
            GetClientRect(hwnd, &rect);
            POINT pt{rect.left, rect.top};
            ClientToScreen(hwnd, &pt);
            rect.left = pt.x;
            rect.top = pt.y;
            pt.x = rect.right;
            pt.y = rect.bottom;
            ClientToScreen(hwnd, &pt);
            rect.right = pt.x;
            rect.bottom = pt.y;
            ClipCursor(&rect);
        #elif __linux__
            XGrabPointer(display, windowHandle, True, 
                        ButtonPressMask | ButtonReleaseMask | PointerMotionMask,
                        GrabModeAsync, GrabModeAsync, windowHandle, None, CurrentTime);
            XFlush(display);
        #endif
    }

    void unlockCursor() {
        #ifdef _WIN32
            ClipCursor(nullptr);
        #elif __linux__
            XUngrabPointer(display, CurrentTime);
            XFlush(display);
        #endif
    }

    void hideMouse(){
        #ifdef _WIN32
            ShowCursor(FALSE);
        #elif __linux__
            XUnmapWindow(display, windowHandle);
            XFlush(display);
        #endif
    }

    void showMouse(){
        #ifdef _WIN32
            ShowCursor(TRUE);
        #elif __linux__
            XMapWindow(display, windowHandle);
            XFlush(display);
        #endif
    }

private:
    std::pair<int, int> position{0, 0};
    MouseButton currentButton{MouseButton::UNKNOWN};

    #ifdef _WIN32
        HWND hwnd;
    #elif __linux__
        Display* display{nullptr};
        Window windowHandle;
    #endif

    void updatePosition() {
        #ifdef _WIN32
            POINT p;
            GetCursorPos(&p);
            ScreenToClient(hwnd, &p);
            position = {p.x, p.y};
        #elif __linux__
            Window root, child;
            int root_x, root_y, win_x, win_y;
            unsigned int mask;
            XQueryPointer(display, windowHandle, &root, &child, 
                         &root_x, &root_y, &win_x, &win_y, &mask);
            position = {win_x, win_y};
        #endif
    }

    void updateButtons() {
        #ifdef _WIN32
            if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) {
                currentButton = MouseButton::LEFT;
            } else if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) {
                currentButton = MouseButton::RIGHT;
            } else if (GetAsyncKeyState(VK_MBUTTON) & 0x8000) {
                currentButton = MouseButton::MIDDLE;
            } else {
                currentButton = MouseButton::UNKNOWN;
            }
        #elif __linux__
            Window root, child;
            int root_x, root_y, win_x, win_y;
            unsigned int mask;
            XQueryPointer(display, windowHandle, &root, &child, 
                         &root_x, &root_y, &win_x, &win_y, &mask);
            
            if (mask & Button1Mask) currentButton = MouseButton::LEFT;
            else if (mask & Button3Mask) currentButton = MouseButton::RIGHT;
            else if (mask & Button2Mask) currentButton = MouseButton::MIDDLE;
            else if (mask & Button4Mask) currentButton = MouseButton::SCROLL_UP;
            else if (mask & Button5Mask) currentButton = MouseButton::SCROLL_DOWN;
            else currentButton = MouseButton::UNKNOWN;
        #endif
    }
};