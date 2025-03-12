#include <ConsoleAPI/terminal.hpp>
#include <ConsoleAPI/colors.hpp>
#include <cmath>
#include <unistd.h>

int main(){
    
    std::wstring gradient = L" .,:;-~=+#%@";

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
                if (rand() % (4) == 0){
                    int inx = (int)((x+tick)%tick*0.1f)%gradient.size();
                    terminal.pix(x, y, gradient[inx], data);
                }
                // if (((x + tick) % size.second) && ((y - tick) % tick))
                //     terminal.pix(x, y, gradient[(int)((x+tick)%tick*0.1f)%gradient.size()], data);
            }
        }

        terminal.draw(data);
        usleep(10000);
        tick++;
    }
}