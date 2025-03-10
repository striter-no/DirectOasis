#include <ConsoleAPI/capi.hpp>
#include <unistd.h>

int main(){
    Console console(
        // 50, 10, 
        Pixel(L".", conv(colors::rgb_back(255, 0, 255)))
    );

    console.hide_cursor();
    while(true){
        console.clear();
        for (int y = 0; y < console.height; ++y) {
            for (int x = 0; x < console.width; ++x) {
                int r = round((float)x / console.width * 255);
                int g = round((float)y / console.height * 255);
                console.pixel(x, y, Pixel(L" ", conv(colors::rgb_back(r, g, 0))));
            }
        }
        console.draw();
        
        usleep(5000);
    }
}