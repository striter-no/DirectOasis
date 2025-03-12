#include <ConsoleAPI/capi.hpp>
#include <ConsoleAPI/HID/mouse.hpp>
#include <ConsoleAPI/HID/ansi_keyboard.hpp>
#include <unistd.h>

void text(
    Console &console,
    std::wstring text,
    int x, int y,
    std::wstring color = L""
){
    for (int i = 0; i < text.size(); ++i) {
        console.pixel(x + i, y, Pixel(std::wstring{} + text[i], color));
    }
}

int main(){
    float termAspect = 9/16.f;
    float dt = 5000; 
    int tick = 0;

    Console console( 
        Pixel(L".", conv(colors::rgb_back(255, 0, 255)))
    );
    auto &term = console.get_terminal();
    term.enableRawInput();
    
    console.hide_cursor();

    Mouse mouse(term);
    Keyboard keyboard(term);
    mouse.start();

    while(!term.isCtrlCPressed()){
        keyboard.pollEvents();
        mouse.pollEvent();

        console.clear();
        auto [x, y] = mouse.getPosition();
        text(console, L"X: " + std::to_wstring(x) + L" Y: " + std::to_wstring(y) + L" Tick: " + std::to_wstring(tick), 0, 0, conv(colors::Fore.green));


        console.pixel(x, y, Pixel(L"*", conv(colors::Fore.red)));
        console.pixel(0, 2, Pixel(keyboard.getUnicodePressed(), conv(colors::Fore.red)));

        console.draw();
        usleep(dt);
        tick++;
    }

    term.disableMouse();
    term.showCursor();
    term.restoreInput();
    return 0;
}

// for (int y = 0; y < console.height; ++y) {
//     for (int x = 0; x < console.width; ++x) {
//         console.pixel(x, y, Pixel(L" ", conv(colors::rgb_back((float)x / console.width * 255, (float)y / console.height * 255, 0))));
//     }
// }