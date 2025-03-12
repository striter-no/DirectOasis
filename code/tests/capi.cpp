#include <ConsoleAPI/capi.hpp>
#include <ConsoleAPI/HID/mouse.hpp>
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
    Console console( 
        Pixel(L".", conv(colors::rgb_back(255, 0, 255)))
    );

    Mouse mouse(console.get_terminal());
    mouse.start();

    float dt = 5000; 
    float termAspect = 9/16.f;

    int tick = 0;
    console.hide_cursor();
    console.get_terminal().enableRawInput();
    while(!console.get_terminal().isCtrlCPressed()){


        mouse.pollEvent();
        console.clear();
        auto [x, y] = mouse.getPosition();
        text(console, L"X: " + std::to_wstring(x) + L" Y: " + std::to_wstring(y) + L" Tick: " + std::to_wstring(tick), 0, 0, conv(colors::Fore.green));

        console.pixel(x, y, Pixel(L"*", conv(colors::Fore.red)));

        console.draw();
        // usleep(dt);
        tick++;
    }

    console.get_terminal().disableMouse();
    console.get_terminal().showCursor();
    console.get_terminal().restoreInput();
    return 0;
}

// for (int y = 0; y < console.height; ++y) {
//     for (int x = 0; x < console.width; ++x) {
//         console.pixel(x, y, Pixel(L" ", conv(colors::rgb_back((float)x / console.width * 255, (float)y / console.height * 255, 0))));
//     }
// }