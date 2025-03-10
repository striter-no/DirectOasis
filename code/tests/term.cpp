#include <ConsoleAPI/terminal.hpp>
#include <ConsoleAPI/colors.hpp>
#include <cmath>
#include <unistd.h>

int main(){
    
    Terminal terminal;
    terminal.startANSI();

    auto size = terminal.getSize();
    const auto rect = terminal.getFullRect();

    int tick = 0;
    while(true){
        terminal.clear();
        std::wstring data = rect;

        for (int x = 0; x < size.first; ++x) {
            for (int y = 0; y < size.second; ++y) {
                if (((x + tick) % size.second) && ((y - tick) % size.first))
                    terminal.pix(x, y, L'#', data);
            }
        }

        terminal.draw(data);
        usleep(15000);
        tick++;
    }
}